#include <iostream>
#include <unistd.h>
#include <sys/mman.h> /* for shm_open */
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include <signal.h>
#include <sys/wait.h>

using namespace std;

int main()
{
    int numCPU = sysconf(_SC_NPROCESSORS_ONLN);

    const char *ids_shm_name = "ids_shm";
    const int ids_shm_size = sizeof(pid_t) * numCPU;
    const char *seeds_shm_name = "seeds_shm";
    const int seeds_shm_size = sizeof(int) * numCPU;

    int fd;
    fd = shm_open(ids_shm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, ids_shm_size);

    pid_t *ids_shm = (pid_t *)mmap(0, ids_shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    fd = shm_open(seeds_shm_name, O_CREAT | O_RDWR, 0666);

    ftruncate(fd, seeds_shm_size);
    
    int *seeds_shm = (int *)mmap(0, seeds_shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    srand(time(0));
    pid_t pid;
    for (int i = 0; i < numCPU; i++)
    {
        pid = fork();
        if (pid == 0)
            break;
        *ids_shm = pid;
        ids_shm += sizeof(pid);
        *seeds_shm = rand();
        seeds_shm += sizeof(int);
    }
    if (pid == 0)
    {
        execl("child.o", "child.o", NULL);
        exit(0);
    }
    if (pid > 0)
    {
        for (int i = 0; i < numCPU; i++)
        {
            wait(NULL);
        }
        cout << "parent run" << endl;
        cout << getpid() << endl;
        munmap(ids_shm, ids_shm_size);
        shm_unlink(ids_shm_name);
        return 0;
    }
}
