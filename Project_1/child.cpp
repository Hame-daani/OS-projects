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

int main()
{
    int numCPU = sysconf(_SC_NPROCESSORS_ONLN);

    const char *ids_shm_name = "ids_shm";
    const int ids_shm_size = sizeof(pid_t) * numCPU;
    const char *seeds_shm_name = "seeds_shm";
    const int seeds_shm_size = sizeof(int) * numCPU;

    int fd;
    fd = shm_open(ids_shm_name, O_RDWR, 0666);

    pid_t *ids_shm = (pid_t *)mmap(0, ids_shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    fd = shm_open(seeds_shm_name, O_RDONLY, 0666);

    int *seeds_shm = (int *)mmap(0, seeds_shm_size, PROT_READ, MAP_SHARED, fd, 0);

    //cout << "child run" << endl;
    int myIndex = getIndex(ids_shm, numCPU);
    int mySeed = getSeed(seeds_shm, myIndex);
    srand(mySeed);

    cout << "child " << myIndex << " of " << getppid() << " seed: " << mySeed << " random: " << rand() << endl;

    return 0;
}

int getIndex(pid_t *ptr, int num)
{
    for (int i = 0; i < num; i++)
    {
        pid_t val = *ptr;
        if (val == getpid())
        {
            return i;
        }
        ptr += sizeof(val);
    }
}

int getSeed(int *ptr, int index)
{
    for (int i = 0; i < index; i++)
        ptr += sizeof(int);
    return *ptr;
}