/*
DScheduler.c

Student Name : Matthew Nitsopoulos
Student ID # : 170673880

*/

#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dscheduler.h"


/*
Any required standard libraries and your header files here
*/
#include "math.h"

struct schedulerInput loadRequest(){
    struct schedulerInput results;
    int numRequests;
    char line_buffer[MAX_LINE_LENGTH];
    char direction;
    char *token;
    
    //Process simulation input line by line
    fgets(line_buffer, MAX_LINE_LENGTH, stdin);

    token = strtok(line_buffer, " ");
    sscanf(token, "%d",&numRequests);
    
    token = strtok(NULL, " ");
    sscanf(token, "%d",&results.startTrack);
       
    token = strtok(NULL, " ");
    sscanf(token, "%c",&direction);
    results.direction = direction == 'u' ? 1 : -1;
    
    results.requests.elements = numRequests;
    results.requests.data = (int *)malloc(sizeof(int)*numRequests);
    if ( results.requests.data == NULL ){
        fprintf( stderr, "Was unable to allocate space for requests data.\n");
        exit( BAD_MALLOC );
    }
    
    for (int i = 0; i < numRequests; i++){
        token = strtok(NULL, " ");
        sscanf(token, "%d", &results.requests.data[i]);
    }
    
    return results;
}

void printResults(struct schedulerResult results){
    for (int i = 0; i < results.requests.elements; i++){
        printf("%4d", results.requests.data[i]);
    }
    printf("Total Head Movement %5d\n", results.totalHeadMovement);
}

struct schedulerResult processRequest(enum POLICIES policy, struct schedulerInput request){
    struct schedulerResult results;
    
    switch(policy){
        case FCFS:
            return process_FCFS_request(request);
        case SSTF:
            return process_SSTF_request(request);
        case SCAN:
            return process_SCAN_request(request);
        case C_SCAN:
            return process_C_SCAN_request(request);
    }
    return results;
}



/* Fill in the following functions */
struct schedulerResult process_FCFS_request(struct schedulerInput request){
    struct schedulerResult results;
	results.requests.data = (int *)malloc(sizeof(int)*request.requests.elements);
	results.requests.elements = request.requests.elements;
	int i = 0;
	results.totalHeadMovement = 0;
	
	results.totalHeadMovement += abs(request.startTrack - request.requests.data[0]);
	for (i = 0; i < request.requests.elements; i++) {
		if ((i + 1) < request.requests.elements) {
			results.totalHeadMovement += abs(request.requests.data[i + 1] - request.requests.data[i]);
		}
		results.requests.data[i] = request.requests.data[i];
	}
	
    return results;
}

struct schedulerResult process_SSTF_request(struct schedulerInput request) {
    struct schedulerResult results;
	results.requests.data = (int *)malloc(sizeof(int)*request.requests.elements);
	results.requests.elements = request.requests.elements;
	results.totalHeadMovement = 0;
	int i = 0, temp = 0, counter = 0, tarray[request.requests.elements], target = request.startTrack, last = 0;
	for (i = 0; i < request.requests.elements; i++) {
		tarray[i] = request.requests.data[i];
	}
	
	for (i = 0; i < request.requests.elements; i++) {
		if (abs(request.startTrack - request.requests.data[i]) < abs(request.startTrack - request.requests.data[temp])) {
			temp = i;
		}
	}
	
	if (target > tarray[temp]) {
		results.totalHeadMovement += target - tarray[temp];
	}
	else {
		results.totalHeadMovement += tarray[temp];
	}
	last = tarray[temp];
	results.requests.data[counter] = tarray[temp];
	target = tarray[temp];
	tarray[temp] = -1;
	counter++;
		
	while (counter < request.requests.elements) {
		
		i = 0;
		while (i < request.requests.elements) {
			if (counter == request.requests.elements - 1 && tarray[i] != -1) {
				temp = i;
			}
			else if (tarray[i] != -1 && abs(target - tarray[i]) < abs(target - tarray[temp])) {
				temp = i;
			}
			i++;
		}
		
		if (target > tarray[temp]) {
			results.totalHeadMovement += target - tarray[temp];
		}
		else {
			results.totalHeadMovement += tarray[temp] - last;
		}
		last = tarray[temp];
		results.requests.data[counter] = tarray[temp];
		target = tarray[temp];
		tarray[temp] = -1;
		counter++;
	}
	tarray[i] = target;
	
    return results;
}

struct schedulerResult process_SCAN_request(struct schedulerInput request){
    struct schedulerResult results;
	results.requests.data = (int *)malloc(sizeof(int)*request.requests.elements);
	results.requests.elements = request.requests.elements;
	results.totalHeadMovement = 0;
	int i = 0, temp = 0, counter = 0, tarray[request.requests.elements], high = 0, direction = 1, j = 0, current = request.startTrack, low = 1023;
	
	for (i = 0; i < request.requests.elements; i++) {
		tarray[i] = request.requests.data[i];
	}
	
	if (request.direction == 1) {
		for (i = 0; i < request.requests.elements; i++) {
			if (tarray[i] > high) {
				high = tarray[i];
			}
		}
		
		while (counter < request.requests.elements) {
			i = 0;
			if (current >= high && direction == 1) {
				results.totalHeadMovement += 1023 - current; 
				current = 1023;
				direction = 0;
			}
			else {
				while (i < request.requests.elements) {
					if (counter == request.requests.elements - 1 && tarray[i] != -1) {
						temp = i;
					}
					else if (tarray[i] != -1 && tarray[i] > current && tarray[i] > tarray[temp] && direction == 1) {
						temp = i;
					}
					else if (tarray[i] != -1 && tarray[i] < current && tarray[i] > tarray[temp] && direction == 0) {
						temp = i;
					}
					i++;
				}
				
				if (direction == 1) {
					results.totalHeadMovement += tarray[temp] - current;
					current = tarray[temp];
				}
				else {
					results.totalHeadMovement += current - tarray[temp];
					current = tarray[temp];
				}
				results.requests.data[counter] = tarray[temp];
				tarray[temp] = -1;
				counter++;
			}
			
			//printf("Head: %d\n", results.totalHeadMovement);
			for (j = 0; j < request.requests.elements; j++) {
				if (tarray[j] != -1) {
					temp = j;
					break;
				}
			}
		}
	}
	else {
		for (i = 0; i < request.requests.elements; i++) {
			if (tarray[i] < low) {
				low = tarray[i];
			}
		}
		direction = 0;
		while (counter < request.requests.elements) {
			i = 0;
			if (current <= low && direction == 0) {
				results.totalHeadMovement += current;
				current = 0;
				direction = 1;
			}
			else {
				while (i < request.requests.elements) {
					if (tarray[i] != -1 && tarray[i] > current && tarray[i] > tarray[temp] && direction == 1) {
						temp = i;
					}
					else if (tarray[i] != -1 && tarray[i] < current && tarray[i] > tarray[temp] && direction == 0) {
						temp = i;
					}
					i++;
				}
				
				if (direction == 1) {
					results.totalHeadMovement += tarray[temp] - current;
					current = tarray[temp];
				}
				else {
					results.totalHeadMovement += current - tarray[temp];
					current = tarray[temp];
				}
				//printf("Total: %d\n", results.totalHeadMovement);
				//printf("Current: %d\n", current);
				results.requests.data[counter] = tarray[temp];
				tarray[temp] = -1;
				counter++;
			}
			
			//printf("Head: %d\n", results.totalHeadMovement);
			for (j = 0; j < request.requests.elements; j++) {
				if (tarray[j] != -1) {
					temp = j;
					break;
				}
			}
		}
		
	}
	
	
    return results;
}

struct schedulerResult process_C_SCAN_request(struct schedulerInput request){
    struct schedulerResult results;
	results.requests.data = (int *)malloc(sizeof(int)*request.requests.elements);
	results.requests.elements = request.requests.elements;
	results.totalHeadMovement = 0;
	int i = 0, temp = -1, counter = 0, tarray[request.requests.elements], high = 0, j = 0, current = request.startTrack;
	
	for (i = 0; i < request.requests.elements; i++) {
		tarray[i] = request.requests.data[i];
	}
	
	if (request.direction == 1) {
		for (i = 0; i < request.requests.elements; i++) {
			if (tarray[i] > high) {
				high = tarray[i];
			}
		}
		
		while (counter < request.requests.elements) {
			i = 0;
			if (current >= high) {
				results.totalHeadMovement += 1023 - current;
				results.totalHeadMovement += 1023;
				current = 0;
			}
			else {
				temp = -1;
				while (i < request.requests.elements) {
					if (counter == request.requests.elements - 1 && tarray[i] != -1) {
						temp = i;
					}
					else if (tarray[i] != -1 && tarray[i] > current && temp == -1) {
						temp = i;
					}
					else if (tarray[i] != -1 && tarray[i] > current && tarray[i] < tarray[temp]) {
						temp = i;
					}
					i++;
				}
				

				results.totalHeadMovement += tarray[temp] - current;
				current = tarray[temp];

				results.requests.data[counter] = tarray[temp];
				tarray[temp] = -1;
				counter++;
			}
			
			//printf("Head: %d\n", results.totalHeadMovement);
			for (j = 0; j < request.requests.elements; j++) {
				if (tarray[j] != -1) {
					temp = j;
					break;
				}
			}
		}
	}
	else {
		for (i = 0; i < request.requests.elements; i++) {
			if (tarray[i] > high) {
				high = tarray[i];
			}
		}
		
		results.totalHeadMovement += current;
		current = 0;
		
		while (counter < request.requests.elements) {
			
			i = 0;
			while (i < request.requests.elements) {
				if (tarray[i] != -1 && tarray[i] < tarray[temp]) {
					temp = i;
				}
				i++;
			}

			results.totalHeadMovement += tarray[temp] - current;
			current = tarray[temp];

			results.requests.data[counter] = tarray[temp];
			tarray[temp] = -1;
			counter++;
			
			for (j = 0; j < request.requests.elements; j++) {
				if (tarray[j] != -1) {
					temp = j;
					break;
				}
			}
			
		}
			
	}
	
    return results;
}


