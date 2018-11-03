#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

using namespace std;

int main()
{
    cout << "child running" << endl;
    const int SIZE = 4096;
    const char *name = "OS";

    int fd;
    char *ptr;
    fd = shm_open(name, O_RDWR, 0666);
    ptr = (char *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    char *msg = (char *)ptr;
    ptr+=strlen(msg);
    cout << msg << endl;
    sprintf(ptr, "%s", "its child");
    cout << "\nchild done" << endl;
    return 0;
}