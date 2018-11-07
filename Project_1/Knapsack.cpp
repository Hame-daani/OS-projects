#include <iostream>
#include <unistd.h>
#include <sys/mman.h> /* for shm_open */
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include <signal.h>
#include <sys/wait.h>
#include<fstream>

using namespace std;

void *createMemory(const char *, int);
void *openMemory(const char *, int);

int main()
{
    const int numCPU = (int)sysconf(_SC_NPROCESSORS_ONLN);

    const char *ids_shm_name = "ids_shm";
    const int ids_shm_size = sizeof(pid_t) * numCPU;
    const char *seeds_shm_name = "seeds_shm";
    const int seeds_shm_size = sizeof(int) * numCPU;

    pid_t *ids_shm = (pid_t *)createMemory(ids_shm_name, ids_shm_size);

    int *seeds_shm = (int *)createMemory(seeds_shm_name, seeds_shm_size);

    srand(time(0));
    pid_t pid;
    for (int i = 0; i < numCPU; i++)
    {
        pid = fork();
        if (pid == 0)
            break;
        ids_shm[i] = pid;
        seeds_shm[i] = rand();
    }
    
    bool child = (pid == 0);
    if (child)
    {
        execl("child.o", "child.o", NULL);
        exit(0);
    }
    else
    {
        for (int i = 0; i < numCPU; i++)
        {
            wait(NULL);
        }
        cout << "parent run" << endl;
        for (int i = 0; i < numCPU; i++)
        {
            cout << "child " << i << " id: " << ids_shm[i] << " seed: " << seeds_shm[i] << endl;
        }
        cout << getpid() << endl;
        munmap(ids_shm, ids_shm_size);
        shm_unlink(ids_shm_name);
        return 0;
    }
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
