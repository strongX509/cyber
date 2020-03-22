#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
int main(void)
{
    char string[] = "/bin/sh";
    char *argv[2];

    argv[0] = string;
    argv[1] = NULL;
    execve(string, argv, NULL);
    exit(0);
}
