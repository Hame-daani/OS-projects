#include <sys/wait.h>
#include <iostream>
#include <unistd.h>

using namespace std;

int main()
{
    // get number of proccesors
    int numCPU = (int)sysconf(_SC_NPROCESSORS_ONLN);
    cout << "Number of Processors is " + to_string(numCPU);
    
    // operation
    char o;
    cout << "\nStart?(Y or N): ";
    cin >> o;
    if (o == 'y')
    {
        pid_t child[numCPU];
        pid_t pid = 0;
        int i = numCPU - 1;
        // create childs
        while (i >= 0 && (pid = fork()) != -1)
        {
            if (pid == 0)
                break;
            child[i--] = pid;
        }
        if (pid > 0) //Father
        {
            pid_t wpid;
            int status = 0;
            while ((wpid = wait(&status)) > 0)
                ;
            for (int j = 0; j < numCPU; j++)
            {
                cout << "Child " + to_string(j) + " Id: " + to_string(child[j]) << endl;
            }
            return 0;
        } // childs
        if (pid == 0)
        {
            cout << "This is Child" << endl;
            exit(0);
        }
    }
    else
        return 0;
}
