#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

struct Memory
{
    pid_t *IDs;
};

#define MEMORY_SIZE sizeof(Memory)
#define MEMORY_NAME "shm"

int main()
{
    // get number of proccesors
    int numCPU = (int)sysconf(_SC_NPROCESSORS_ONLN);
    cout << "Number of Processors is " + to_string(numCPU) << endl;

    //create shared memory
    int shm_fd;
    if ((shm_fd = shm_open(MEMORY_NAME, O_CREAT | O_RDWR, 0666)) == -1)
    {
        cout << "error in shm open" << endl;
        return 0;
    }
    if ((ftruncate(shm_fd, MEMORY_SIZE)) == -1)
    {
        cout << "error in ftruncate" << endl;
        return 0;
    }
    Memory *shm = (Memory *)mmap(0, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // test memory
    shm->IDs = new pid_t[numCPU];
    for (int i = 0; i < numCPU; i++)
        shm->IDs[i] = getpid() + i;
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        for (int i = 0; i < numCPU; i++)
            cout << shm->IDs[i] << endl;
        cout << "child done" << endl;
        return 0;
    }
    if (pid > 0)
    {
        wait(NULL);
        cout << "parent done" << endl;
        return 0;
    }

    return 0;
}
