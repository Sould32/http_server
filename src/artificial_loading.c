#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include <time.h>
#include "artificial_loading.h"
#include "http_response.h"

/*
 * TODO: Replace fprintf with methods to write to socket
 * TODO: Get message length for inclusion with HTTP header
 * TODO: Write HTTP headers too.
 * TODO: Add limit to number of runloop procedures that can be running
 */

/*
 * Procedure that spins for 15 seconds on a separate thread
 */
static void * runloop_proc(void * data){
	struct timespec start;
	struct timespec end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	while(1){
		clock_gettime(CLOCK_MONOTONIC, &end);
		if((end.tv_sec - start.tv_sec) + 
				(end.tv_nsec - start.tv_nsec)/1e9 >= 15){
			break;
		}
	}
	return NULL;
}

/*
 * Spawn thread that loops for 15 seconds
 */
void runloop(int fd){
	//Spawn off thread. Make it detached so we don't need to clean up.
	pthread_t thread;
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&thread, &thread_attr, &runloop_proc, NULL)){
		//Unable to create thread
		//Probably return 503 (Service unavailable) with retry-after header
		//saying to wait 15 seconds
	}
	pthread_attr_destroy(&thread_attr);
	response_head(fd, HTTP_OK, "Started 15 second spin");
	//fprintf(fd, "Started 15 second spin");
}

static pthread_mutex_t block_lock = PTHREAD_MUTEX_INITIALIZER;
static int block_count = 0;
static void* first_block = NULL;
char * cnt = "Mapped and touched 256MB anonymous memory, now have 0 blocks allocated in total.";
/*
 * Allocate 256 MB block to poke at memory usage
 *
 * Succeeds unless mmap failed.
 * Maximum of six blocks can be allocated.
 */
void allocanon(int fd){
	pthread_mutex_lock(&block_lock);
	if(block_count == 6){
		pthread_mutex_unlock(&block_lock);
		response_head(fd, HTTP_OK, "Reached maximum of 6 blocks, request ignored");
		return;
	}
	//Allocate 256 MB
	void * block = mmap(NULL, 256 << 20, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
	if(block){
		block_count++;
		*((void**)block) = first_block;
		first_block = block;
		cnt[53] = '0'+ block_count;
		response_head(fd, HTTP_OK, cnt);	
		//fprintf(fd, "Mapped and touched 256MB anonymous memory, now have %d blocks allocated in total.", block_count);
	}
	else{
		//Internal server error
	}
	pthread_mutex_unlock(&block_lock);
}
char* cnt1 = "Unmapped 256 MB, 0 blocks left.";
/*
 * Free previously allocated block of memory
 *
 * Last block allocated is first freed
 * This function should always succeed unless something very odd and probably
 * hardware-related happens.
 */
void freeanon(int fd){
	pthread_mutex_lock(&block_lock);
	if(block_count){
		block_count--;
		void* oldblock = first_block;
		first_block = *((void**) oldblock);
		if(munmap(oldblock, 256 << 20)){
			// munmap failed? /Should/ never happen.
			// Return internal server error
		}
		cnt1[17]= block_count;
		response_head(fd, HTTP_OK, cnt1); 
		//fprintf(fd, "Unmapped 256 MB, %d blocks left.", block_count);
	}
	else{
		response_head(fd, HTTP_OK, "No Blocks allocated.");
		//fprintf(fd, "No blocks allocated.");
	}
	pthread_mutex_unlock(&block_lock);
}
