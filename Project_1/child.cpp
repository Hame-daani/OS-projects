#include <iostream>
#include <unistd.h>
#include <sys/mman.h> /* for shm_open */
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include <signal.h>
#include <sys/wait.h>

using namespace std;

int getIndex(pid_t *, int);
int getSeed(int *, int);
void *createMemory(const char *, int);
void *openMemory(const char *, int);

int main()
{
    const int numCPU = (int)sysconf(_SC_NPROCESSORS_ONLN);

    const char *ids_shm_name = "ids_shm";
    const int ids_shm_size = sizeof(pid_t) * numCPU;
    const char *seeds_shm_name = "seeds_shm";
    const int seeds_shm_size = sizeof(int) * numCPU;

    pid_t *ids_shm = (pid_t *)openMemory(ids_shm_name, ids_shm_size);

    int *seeds_shm = (int *)openMemory(seeds_shm_name, seeds_shm_size);

    //cout << "child run" << endl;
    int myIndex = getIndex(ids_shm, numCPU);
    int mySeed = getSeed(seeds_shm, myIndex);
    srand(mySeed);

    cout << getpid() << " child number: " << myIndex << " seed: " << mySeed << endl;

    return 0;
}

int getIndex(pid_t *ptr, int num)
{
    pid_t id = getpid();
    for (int i = 0; i < num; i++)
    {
        if (ptr[i] == id)
            return i;
    }
    return -1;
}

int getSeed(int *ptr, int index)
{
    return ptr[index];
}

void *createMemory(const char *name, int size)
{
    int fd;
    fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, size);

    void *ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    return ptr;
}

void *openMemory(const char *name, int size)
{
    int fd;
    fd = shm_open(name, O_RDWR, 0666);

    void *ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    return ptr;
}
