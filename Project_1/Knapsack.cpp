#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

int main()
{
    // get number of proccesors
    int numCPU = (int)sysconf(_SC_NPROCESSORS_ONLN);
    cout << "Number of Processors is " + to_string(numCPU) << endl;
    
    return 0;
}
