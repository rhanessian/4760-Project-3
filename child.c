#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>  
#include <unistd.h>    
#include <string.h> 
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <sys/sem.h>
#define MAXPIDS 20

const char* program_name;

void print_error(const char* message){
	char buffer[128];
	snprintf(buffer, sizeof(buffer), "%s:%s", program_name, message);
	perror(buffer);
}

int semid;

union semun {
	int val;                /* value for SETVAL */
	struct semid_ds *buf;   /* buffer for IPC_STAT & IPC_SET */
	unsigned short *array;  /* array for GETALL & SETALL */
	struct seminfo *__buf;  /* buffer for IPC_INFO */
};

void log_event(int num, const char* message) {
	char filename[32];
	snprintf(filename, sizeof(filename), "logfile.%d", num);
	FILE* f = fopen(filename, "a");
	time_t timer;
    struct tm* tm_info;
	timer = time(NULL);
    tm_info = localtime(&timer);
    char buffer[26];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", tm_info);
    fprintf(f, "%s: %s\n", buffer, message);
    fclose(f);
}

int main (int argc, char *argv[]) {
	
	srand(time(NULL));
	
	key_t key_sem = ftok("master.c", 987);
	semid = semget(key_sem, 1, 0);
	
	int num = atoi(argv[1]);
	
	time_t timer;
    char buffer[26];
    struct tm* tm_info;
    
	printf("%ld\n", (long)getpid());
	
	struct sembuf p = { 0, -1, SEM_UNDO};
	struct sembuf v = { 0, +1, SEM_UNDO};
	
	for(int i = 0; i < 3; i++) {
		log_event(num, "Entering critical section...");
		
		if (semop(semid, &p, 1) < 0) {
			print_error("semop");
			exit(1);
		}
		
		log_event(num, "Entered critical section.");
		sleep(1 + rand()%5);
		timer = time(NULL);
    	tm_info = localtime(&timer);
    	strftime(buffer, sizeof(buffer), "%H:%M:%S", tm_info);
		FILE* f = fopen("cstest", "a");
		fprintf(f, "%s Queue %d File modified by process number %d\n", buffer, num, (int)getpid());
		fclose(f);
		sleep(1 + rand()%5);
		log_event(num, "Exiting critical section...");
		
		if (semop(semid, &v, 1) < 0) {
    		print_error("semop");
    		exit(1);
		}
		log_event(num, "Exited.");
	}
	
	struct sembuf sb = {0, 1, 0}; 
	
    if (semop(semid, &sb, 1) == -1) {
        print_error("semop");  
    }

	return 0;
}		