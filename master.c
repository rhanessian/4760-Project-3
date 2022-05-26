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
#include <getopt.h>
#include <sys/sem.h>
#define MAXPIDS 20

pid_t pids[MAXPIDS];

int semid;

union semun {
	int val;                /* value for SETVAL */
	struct semid_ds *buf;   /* buffer for IPC_STAT & IPC_SET */
	unsigned short *array;  /* array for GETALL & SETALL */
	struct seminfo *__buf;  /* buffer for IPC_INFO */
};

const char* program_name;

void print_error(const char* message){
	char buffer[128];
	snprintf(buffer, sizeof(buffer), "%s:%s", program_name, message);
	perror(buffer);
}

void sighandler(int signum) {
	printf("Caught signal %d, exiting...\n", signum);
	for (int i = 0; i < MAXPIDS; i++)
		if (pids[i] != 0)	
			kill(pids[i], SIGCHLD);
	union semun u;
	if (semctl(semid, 0, IPC_RMID, u) == -1) 
    	print_error("semctl");
	exit(1);
}

void delete_pid(pid_t pid) {
	for (int i = 0; i < MAXPIDS; i++) 
		if (pid == pids[i])
			pids[i] = 0;
}

void handle_child(int signum) {
	pid_t pid = wait(NULL);
	printf("Caught signal from pid %ld\n", (long)pid);
	delete_pid(pid);
}

int find_space(void) {
	for (int i = 0; i < MAXPIDS; i++) 
		if (pids[i] == 0)
			return i;
	return -1;
}

int main (int argc, char *argv[]) {
	program_name = argv[0];
	signal(SIGINT, sighandler);
	signal(SIGCHLD, handle_child);
	signal(SIGALRM, sighandler);
	
	remove("cstest");
	for (int i = 0; i < MAXPIDS; i++){
		char filename[32];
		snprintf(filename, sizeof(filename), "logfile.%d", i);
		remove(filename);
	}
	
	int c, n;
	
	int ss = 100;
	
	opterr = 0;
	
	while  ((c = getopt (argc, argv, "t:")) != -1)
		switch (c) {
			case 't': 
				ss = atoi(optarg);
				break;
			default: 
				printf("unknown\n");
				break;
		}
	
	if (argc <= optind) {
		printf("Need argument n\n");
		return(-1);
	} else {
		n = atoi(argv[optind]);	
	}
	if (n > 20){
		fprintf(stderr, "Warning: n cannot be greater than 20.\n");
		n = 20;
	}
	alarm(ss);
	
	printf("ss = %d, n = %d\n", ss, n);
	
	key_t key_sem = ftok("master.c", 987);
	if ((semid = semget(key_sem, 1, 0666 | IPC_CREAT)) < 0)
		print_error("semget");
	union semun u;
	u.val = 1;
	if (semctl(semid, 0, SETVAL, u) < 0) { 
		print_error("semctl");
	}
	
	pid_t pid;
	
	for (int i = 0; i < n; i++) {
		int ind;
		while ((ind = find_space()) < 0)
			;
			
		
		if((pid = fork()) == 0) {
			char string_num[8];
			snprintf(string_num, sizeof(string_num), "%d", ind);
			char *args[] = {"./child", string_num, 0};
			char *env[] = { 0 };
			execve("./child", args, env);
			print_error("execve");
			exit(1);
		} else {
			pids[ind] = pid;
		}
	}	
		
	while(wait(NULL) > 0)
	;

	if (semctl(semid, 0, IPC_RMID, u) == -1) 
   		print_error("semctl");
	
	return 0;
}










