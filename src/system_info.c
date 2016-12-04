#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "system_info.h"

/*
 * TODO: Replace fprintf with methods to write to socket
 * TODO: Get message length for inclusion with HTTP header
 * TODO: Write HTTP headers too.
 */

/*
 * Send a JSON response with load average information
 */
void loadavg(FILE* fd){
	FILE * loadfile = fopen("/proc/loadavg", "r");
	if(loadfile == NULL){
		//return internal server error
	}
	float last1, last5, last10;
	int running, total;
	int lastPID;
	if(fscanf(loadfile, "%f %f %f %d/%d %d", &last1, &last5, &last10, &running, &total, &lastPID) == 6){
		fprintf(fd, "{\"total_threads\": \"%d\", \"loadavg\": [\"%f\", \"%f\", \"%f\"], \"running_threads\": \"%d\"}", total, last1, last5, last10, running);
	}
	else{
		//Error reading file
		fprintf(stderr, "Error reading /proc/loadavg. Unexpected format.\n");
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
void meminfo(FILE* fd){
	//open /proc/meminfo
	FILE * memfile = fopen("/proc/meminfo", "r");
	if(memfile == NULL){
		//Return internal server error
	}
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
			fprintf(fd, ", ");
		}
		else{
			fprintf(fd, "{");
			headed = true;
		}
		//Get rid of the colon
		char * colon = strchr(name, ':');
		if(colon != NULL){
			*colon = '\0';
		}
		fprintf(fd, "\"%s\": \"%ld\"", name, value);
	}
	fprintf(fd, "}");
	//Close file
	if(fclose(memfile)){
		// Failed to close a readonly file?!
		// We can probably ignore this, because there's nothing we could do to
		// fix it.
	}

}
