#include <iostream>
#include <unistd.h>
#include <sys/mman.h> /* for shm_open */
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include <signal.h>
#include <sys/wait.h>
#include <fstream>
#include <string.h>

using namespace std;

int getIndex(pid_t *, int);
int getSeed(int *, int);
void *createMemory(const char *, int);
void *openMemory(const char *, int);
bool repeatCheck(int *, int, int);
int getWeightSum(int *, int, int[100][2]);
bool exceedWeight(int *, int, int, int[100][2], int);

#define WEIGHT 0
#define VALUE 1
#define IDEAL_WEIGHT 1000

int main()
{
    const int numCPU = (int)sysconf(_SC_NPROCESSORS_ONLN);

    const char *ids_shm_name = "ids_shm";
    const int ids_shm_size = sizeof(pid_t) * numCPU;
    const char *seeds_shm_name = "seeds_shm";
    const int seeds_shm_size = sizeof(int) * numCPU;

    pid_t *ids_shm = (pid_t *)openMemory(ids_shm_name, ids_shm_size);

    int *seeds_shm = (int *)openMemory(seeds_shm_name, seeds_shm_size);

    int data[100][2];
    ifstream f;
    f.open("data.txt");
    int i = 0;
    data[i][WEIGHT] = 0;
    data[i][VALUE] = 0;
    while (f >> data[i + 1][WEIGHT] && f >> data[i + 1][VALUE])
        i++;
    f.close();

    int myIndex = getIndex(ids_shm, numCPU);
    int mySeed = getSeed(seeds_shm, myIndex);
    srand(mySeed);

    if (myIndex == 0)
    {
        int num = rand() % 100 + 1;
        int *result = new int[num];
        int n;
        memset(result, 0, num * sizeof(*result));
        for (int j = 0; j < num; j++)
        {
            n = rand() % 100 + 1;
            while (repeatCheck(result, num, n) || exceedWeight(result, num, n, data, IDEAL_WEIGHT))
                n = rand() % 100 + 1;
            cout << n << endl;
            result[j] = n;
        }

        for (int j = 0; j < num; j++)
        {
            cout << j << " : " << result[j] << " - " << data[result[j]][WEIGHT] << endl;
            ;
        }
        cout << getWeightSum(result, num, data);
        delete result;
    }
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

bool repeatCheck(int *result, int num, int n)
{
    cout << "repeatCheck" << endl;
    for (int i = 0; i < num; i++)
    {
        if (result[i] == n)
            return true;
    }
    return false;
}

int getWeightSum(int *result, int num, int data[100][2])
{
    cout << "sum" << endl;
    int sum = 0;
    for (int i = 0; i < num; i++)
    {
        sum += data[result[i]][WEIGHT];
    }
    return sum;
}

bool exceedWeight(int *result, int num, int n, int data[100][2], int ideal)
{
    cout << "exceed" << endl;
    int sum = getWeightSum(result, num, data);
    return ((sum + data[n][WEIGHT]) > ideal);
}