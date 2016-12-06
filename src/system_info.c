#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "system_info.h"
#include "http_response.h"

/*
 * TODO: Replace fprintf with methods to write to socket
 * TODO: Get message length for inclusion with HTTP header
 * TODO: Write HTTP headers too.
 */

/*
 * Send a JSON response with load average information
 */
void loadavg(int fd){
	FILE * loadfile = fopen("/proc/loadavg", "r");
	if(loadfile == NULL){
		//return internal server error
	}
	float last1, last5, last10;
	int running, total;
	int lastPID;
	if(fscanf(loadfile, "%f %f %f %d/%d %d", &last1, &last5, &last10, &running, &total, &lastPID) == 6){
		char buffer[300];
		snprintf( buffer, 300, "{\"total_threads\": \"%d\", \"loadavg\": [\"%f\", \"%f\", \"%f\"], \"running_threads\": \"%d\"}", total, last1, last5, last10, running);
		//TODO adjust the length of the file
		response_head(fd, HTTP_OK, buffer);
		//fprintf(fd, HTTP_OK, );
	}
	else{
		//Error reading file
		response_head(fd, HTTP_EXPECTATION_FAILED, "Error reading /proc/loadavg. Unexpected format.\n");
		//fprintf(stderr, "Error reading /proc/loadavg. Unexpected format.\n");
	}
	if(fclose(loadfile)){
		// Failed to close a readonly file?!
		// We can probably ignore this, because there's nothing we could do to
		// fix it.
	}

}
/*
 * Send a JSON response with memory usage information
 */
void meminfo(int fd){
	//open /proc/meminfo
	FILE * memfile = fopen("/proc/meminfo", "r");
	if(memfile == NULL){
		//Return internal server error
	}
		char buffer[5000];
	int buffer_count = 0;
	
	//For each line, read name and value
	char name[128]; //Fixed size, but should be large enough
	long value;
	int c;
	bool headed = false;
	while(fscanf(memfile, "%s %ld", name, &value) != EOF){
		//Scan to end of line
		while((c = fgetc(memfile)) != '\n'){
			if(c == EOF) break;
		}
		//Package as JSON
		if(headed){
			buffer_count += snprintf((buffer+buffer_count), 5000-buffer_count, ", ");
			//chain.curren_char = asprintf(&chain.cuurent_char, ", "); 
			//fprintf(fd, ", ");
		}
		else{
			buffer_count += snprintf((buffer+buffer_count), 5000-buffer_count, "{");
			//fprintf(fd, "{");
			headed = true;
		}
		//Get rid of the colon
		char * colon = strchr(name, ':');
		if(colon != NULL){
			*colon = '\0';
		}
		buffer_count += snprintf((buffer+buffer_count), 5000-buffer_count, "\"%s\": \"%ld\"", name, value);
		//fprintf(fd, "\"%s\": \"%ld\"", name, value);
	}
	buffer_count += snprintf((buffer+buffer_count), 5000-buffer_count, "}");
	//fprintf(fd, "}");
	//Close file
	if(fclose(memfile)){
		// Failed to close a readonly file?!
		// We can probably ignore this, because there's nothing we could do to
		// fix it.
	}
	response(fd, HTTP_OK, buffer, buffer_count);
}
