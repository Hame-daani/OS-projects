#include <iostream>
#include <unistd.h>
#include <sys/mman.h> /* for shm_open */
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include <fstream>
#include <string.h>
#include <vector>

using namespace std;

int getIndex(pid_t *, int);
int getSeed(int *, int);
void *openMemory(const char *, int);
bool repeatCheck(vector<int> &, int);
int getWeightSum(vector<int> &, int[100][2]);
int getValueSum(vector<int> &, int[100][2]);
bool exceedWeight(vector<int> &, int[100][2], int);
void process(vector<int> &, int[100][2], int);

#define WEIGHT 0
#define VALUE 1
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

    pid_t *ids_shm = (pid_t *)openMemory(ids_shm_name, ids_shm_size);

    int *seeds_shm = (int *)openMemory(seeds_shm_name, seeds_shm_size);

    int *params_shm = (int *)openMemory(params_shm_name, params_shm_size);

    int *result_shm = (int *)openMemory(result_shm_name, result_shm_size);

    int *result_value_shm = (int *)openMemory(result_value_shm_name, result_value_shm_size);

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

    vector<int> bestResult;

    cout << "Child " << myIndex + 1 << " Running..." << endl;

    int runTime = params_shm[TIME];

    time_t start = time(NULL);
    time_t now = time(NULL);
    
    while ((now - start) <= runTime)
    {
        process(bestResult, data, params_shm[IDEAL_WEIGHT]);
        now = time(NULL);
    }

    result_value_shm[myIndex] = getValueSum(bestResult, data);

    for (int j = 0; j < bestResult.size(); j++)
    {
        result_shm[(myIndex * 100) + j] = bestResult[j];
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

void *openMemory(const char *name, int size)
{
    int fd;
    fd = shm_open(name, O_RDWR, 0666);

    void *ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    return ptr;
}

bool repeatCheck(vector<int> &vec, int n)
{
    for (int i = 0; i < vec.size(); i++)
    {
        if (vec[i] == n)
            return true;
    }
    return false;
}

int getWeightSum(vector<int> &vect, int data[100][2])
{
    int sum = 0;
    for (int i = 0; i < vect.size(); i++)
    {
        sum += data[vect[i]][WEIGHT];
    }
    return sum;
}

int getValueSum(vector<int> &vect, int data[100][2])
{
    int sum = 0;
    for (int i = 0; i < vect.size(); i++)
    {
        sum += data[vect[i]][VALUE];
    }
    return sum;
}

bool exceedWeight(vector<int> &vect, int data[100][2], int ideal)
{
    int sum = getWeightSum(vect, data);
    return sum > ideal;
}

void process(vector<int> &bestResult, int data[100][2], int ideal)
{
    int n;
    vector<int> result;
    do
    {
        do
        {
            n = rand() % 100 + 1;
        } while (repeatCheck(result, n));

        result.push_back(n);

    } while (!exceedWeight(result, data, ideal));

    result.pop_back();
    if (getValueSum(result, data) > getValueSum(bestResult, data))
    {
        bestResult = result;
    }
}
