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
    const int ids_shm_size = sizeof(pid_t);

    int fd;
    fd = shm_open(ids_shm_name, O_RDWR, 0666);
    pid_t *ids_shm = (pid_t *)mmap(0, ids_shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    cout << "child run" << endl;
    for (int i = 0; i < numCPU + 1; i++)
    {
        pid_t val = *ids_shm;
        if (val == getpid())
        {
            cout << "I'm " << val << " child " << i << endl;
            break;
        }
        ids_shm += sizeof(val);
    }
    return 0;
}
