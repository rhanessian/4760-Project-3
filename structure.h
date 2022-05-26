#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#define MAXPIDS 20

struct shrd_mem {
	bool choosing[MAXPIDS];
	int numbers[MAXPIDS];
};

