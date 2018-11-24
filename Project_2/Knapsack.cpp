#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <fstream>

using namespace std;

/* a struct for child threads to pass requirment parametrs. */
struct Params
{
    int id;
    vector<int> result;
};

static int runTime;
static int maxWeight;
static int datas[100][2];
static const char *dataFile = "data.txt";

/* for use in data array indexes */
#define WEIGHT 0
#define VALUE 1

void *process(void *);
void loadFile(const char *);
void calcute(vector<int> &);
bool repeatCheck(vector<int> &, int);
int getWeightSum(vector<int> &);
int getValueSum(vector<int> &);
bool exceedWeight(vector<int> &);
int findBestChild(Params **, int);
void createThreads(pthread_t *, Params **, int);
void printResults(Params **, int, int);

/* use in the thread that print run time */
void *timer(void *p)
{
    time_t start = time(NULL);
    time_t now = time(NULL);
    while ((now - start) <= runTime)
    {
        cout << "Run Time: " << now - start << "\r";
        fflush(stdout);
        now = time(NULL);
    }
    cout << endl;
}

int main()
{
    /* find number of processors */
    const int numCPU = (int)sysconf(_SC_NPROCESSORS_ONLN);

    /* get run time from user */
    cout << "\033[33mInsert Run Time(in seconds): \033[0m";
    cin >> runTime;

    /* get max weight from user */
    cout << "\033[33mInsert Maximum Weight: \033[0m";
    cin >> maxWeight;

    /* load data from 'dataFile' const, into 'datas' global variable */
    loadFile(dataFile);

    /* create child threads and save thread's id in 'tids'
    and thread's parametrs in 'params'*/
    pthread_t *tids = new pthread_t[numCPU];
    Params **params = new Params *[numCPU];
    createThreads(tids, params, numCPU);

    /* create a thread to print timer */
    pthread_t id;
    pthread_create(&id, NULL, timer, NULL);
    pthread_join(id, NULL);

    /* wait for each child thread to finish. */
    for (int i = 0; i < numCPU; i++)
    {
        pthread_join(tids[i], NULL);
    }

    int bestChild = findBestChild(params, numCPU);

    /* print results */
    printResults(params, numCPU, bestChild);

    delete[] params;
    delete[] tids;
    return 0;
}

/* main method that run in child threads */
void *process(void *p)
{
    Params *params = (Params *)p;

    // run calcute method as long as run time variable
    time_t start = time(NULL);
    time_t now = time(NULL);
    while ((now - start) <= runTime)
    {
        calcute(params->result);
        now = time(NULL);
    }

    pthread_exit(0);
}

void loadFile(const char *fileName)
{
    ifstream f;
    f.open(fileName);
    int i = 0;
    datas[i][WEIGHT] = 0;
    datas[i][VALUE] = 0;
    while (f >> datas[i + 1][WEIGHT] && f >> datas[i + 1][VALUE])
        i++;
    f.close();
}

/* cheack if item n is exist in vector or not */
bool repeatCheck(vector<int> &vec, int n)
{
    for (int i = 0; i < vec.size(); i++)
    {
        if (vec[i] == n)
            return true;
    }
    return false;
}

/* return sum of weights of items in vector */
int getWeightSum(vector<int> &vect)
{
    int sum = 0;
    for (int i = 0; i < vect.size(); i++)
    {
        sum += datas[vect[i]][WEIGHT];
    }
    return sum;
}

/* return sum of values of items in vector */
int getValueSum(vector<int> &vect)
{
    int sum = 0;
    for (int i = 0; i < vect.size(); i++)
    {
        sum += datas[vect[i]][VALUE];
    }
    return sum;
}

/* check if the items weight exceed the maximum weight */
bool exceedWeight(vector<int> &vect)
{
    int sum = getWeightSum(vect);
    return sum > maxWeight;
}

/* calcute a result by random and change best result with it if its better */
void calcute(vector<int> &bestResult)
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

    } while (!exceedWeight(result));

    // pop the last item that make weight exceed the max
    result.pop_back();
    
    if (getValueSum(result) > getValueSum(bestResult))
    {
        bestResult = result;
    }
}

/* finding the child with best result */
int findBestChild(Params **params, int numCPU)
{
    int bestChild = 0;
    for (int i = 0; i < numCPU; i++)
    {
        if (getValueSum(params[i]->result) > getValueSum(params[bestChild]->result))
            bestChild = i;
    }
    return bestChild;
}

/* create thread for numbers of specific number */
void createThreads(pthread_t *tids, Params **params, int num)
{
    for (int i = 0; i < num; i++)
    {
        params[i] = new Params();
        params[i]->id = i;
        if ((pthread_create(&tids[i], NULL, process, params[i])) != 0)
            perror("\033[31mCan't Create Thread.\033[0m");
    }
}

/* print child threads result */
void printResults(Params **params, int num, int bestChild)
{
    for (int i = 0; i < num; i++)
    {
        if (i == bestChild)
        {
            cout << "\033[32mChild " << i + 1 << " Weight: " << getWeightSum(params[i]->result) << " Value: " << getValueSum(params[i]->result) << "\033[0m\n";
        }
        else
        {
            cout << "Child " << i + 1 << " Weight: " << getWeightSum(params[i]->result) << " Value: " << getValueSum(params[i]->result) << "\n";
        }
    }
}
