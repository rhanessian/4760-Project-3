#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#define SHMSIZE 27
#define MAXPIDS 20

struct shrd_mem {
	bool choosing[MAXPIDS];
	int numbers[MAXPIDS];
};

/*union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};
*/

struct sembuf p = { 0, -1, SEM_UNDO};
struct sembuf v = { 0, +1, SEM_UNDO};