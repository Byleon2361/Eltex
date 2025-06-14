#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
    pid_t pid = 0;

    pid = fork();
    if (pid == 0)  // первый ребенок rootа (1)
    {
        printf("I am parent1, my pid=%d, my ppid=%d\n", getpid(), getppid());
        pid = fork();
        if (pid == 0)  // первый ребенок первого родителя(3)
        {
            printf("I am first child of parent1, my pid=%d, my ppid=%d\n", getpid(), getppid());
            exit(3);
        }
        else if (pid > 0)
        {
            pid = fork();
            if (pid == 0)  // второй ребенок первого родителя(4)
            {
                printf("I am second child of parent1, my pid=%d, my ppid=%d\n", getpid(), getppid());
                exit(4);
            }
            wait(NULL);  // 4
        }
        wait(NULL);  // 3
        exit(1);
    }
    else if (pid > 0)  // root(0)
    {
        printf("I am grandparent, my pid=%d, my ppid=%d\n", getpid(), getppid());
        pid = fork();
        if (pid == 0)  // второй ребенок root(2)
        {
            printf("I am parent2, my pid=%d, my ppid=%d\n", getpid(), getppid());
            pid = fork();
            if (pid == 0)  // ребенок второго родителя(5)
            {
                printf("I am child of parent2, my pid=%d, my ppid=%d\n", getpid(), getppid());
                exit(5);
            }
            wait(NULL);  // 5
            exit(2);
        }
        wait(NULL);  // 2
    }
    wait(NULL);  // 1
}
