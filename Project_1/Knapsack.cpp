#include <iostream>
#include <unistd.h>
#include <sys/mman.h> /* for shm_open */
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include <sys/wait.h>
#include <fstream>

using namespace std;

void *createMemory(const char *, int);

#define IDEAL_WEIGHT 0
#define TIME 1

int main()
{
    const int numCPU = (int)sysconf(_SC_NPROCESSORS_ONLN);

    const char *ids_shm_name = "ids_shm";
    const char *seeds_shm_name = "seeds_shm";
    const char *result_shm_name = "result_shm";
    const char *params_shm_name = "params_shm";
    const char *result_value_shm_name = "result_value_shm";

    const int params_shm_size = sizeof(int) * 2;
    const int ids_shm_size = sizeof(pid_t) * numCPU;
    const int seeds_shm_size = sizeof(int) * numCPU;
    const int result_value_shm_size = sizeof(int) * numCPU;
    const int result_shm_size = sizeof(int) * numCPU * 100;

    pid_t *ids_shm = (pid_t *)createMemory(ids_shm_name, ids_shm_size);

    int *seeds_shm = (int *)createMemory(seeds_shm_name, seeds_shm_size);

    int *params_shm = (int *)createMemory(params_shm_name, params_shm_size);

    int *result_shm = (int *)createMemory(result_shm_name, result_shm_size);

    int *result_value_shm = (int *)createMemory(result_value_shm_name, result_value_shm_size);

    int a;
    cout << "Insert Ideal Weight: ";
    cin >> a;
    params_shm[IDEAL_WEIGHT] = a;
    cout << "Insert Run Time: ";
    cin >> a;
    params_shm[TIME] = a;

    srand(time(NULL));

    pid_t pid = 0;
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

        for (int j = 0; j < numCPU; j++)
        {
            cout << "child " << j + 1 << " - "
                 << "Value : " << result_value_shm[j] << endl;

            for (int i = 0; i < 100; i++)
            {
                if (result_shm[(j * 100) + i] == 0)
                {
                    cout << endl
                         << endl;
                    break;
                }

                cout << result_shm[(j * 100) + i] << " ";
                if (i > 0 && i % 10 == 0)
                    cout << endl;
            }
        }

        munmap(ids_shm, ids_shm_size);
        shm_unlink(ids_shm_name);

        munmap(seeds_shm, seeds_shm_size);
        shm_unlink(seeds_shm_name);

        munmap(result_shm, result_shm_size);
        shm_unlink(result_shm_name);

        munmap(params_shm, params_shm_size);
        shm_unlink(params_shm_name);

        munmap(result_value_shm, result_value_shm_size);
        shm_unlink(result_value_shm_name);

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
