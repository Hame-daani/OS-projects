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
    const int SIZE = 4096;
    const char *name = "OS";
    const char *msg_0 = "Hello";
    const char *msg_1 = "World!";

    int fd;
    char *ptr;
    fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SIZE);
    ptr = (char *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    sprintf(ptr, "%s", msg_0);
    ptr += strlen(msg_0);
    sprintf(ptr, "%s", msg_1);
    ptr += strlen(msg_1);

    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        cout << "child start." << endl;
        execl("child.o", "child.o", NULL);
    }
    if (pid > 0)
    {
        kill(pid, SIGSTOP);
        sprintf(ptr, "%s", "Another");
        ptr += strlen("Another");
        kill(pid, SIGCONT);
        cout << "parent wait" << endl;
        wait(NULL);
        printf("%s", (char *)ptr);
        shm_unlink(name);
        cout << "\nparent done" << endl;
    }

    return 0;
}
