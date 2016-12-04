#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>

void runloop(FILE * fd){
	//Spawn off thread
	fprintf(fd, "Started 15 second spin");
}

static int block_count = 0;
static void* first_block = NULL;

void allocanon(FILE * fd){
	if(block_count == 6){
		fprintf(fd, "Reached maximum of 6 blocks, request ignored");
		return;
	}
	//Allocate 256 MB
	void * block = mmap(NULL, 256 << 20, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
	if(block){
		block_count++;
		*((void**)block) = first_block;
		first_block = block;
		fprintf(fd, "Mapped and touched 256MB anonymous memory, now have %d blocks allocated in total.", block_count);
	}
	else{
		//Internal server error
	}
}

void freeanon(FILE * fd){
	//Free old block
	if(block_count){
		block_count--;
		void* oldblock = first_block;
		first_block = *((void**) oldblock);
		if(munmap(oldblock, 256 << 20)){
			//munmap failed? /Should/ never happen.
			perror("munmap failed");
		}
		fprintf(fd, "Unmapped 256 MB, %d blocks left.", block_count);
	}
	else{
		fprintf(fd, "No blocks allocated.");
	}
}
