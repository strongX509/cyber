#include <stdlib.h>
#include <stdio.h>

/* computes the sum of the first n natural numbers */
int sum(int n)
{
    int s = 0;

    while (n > 0)
    {
        s += n;
        n -= 1;
    }
    return s;
}

/* main program */
int main(int argc, char** argv)
{
    int n;

    /* we expect at least one command line argument */
    if (argc < 2)
    {
        exit(1);
    }
    n = atoi(argv[1]);

    printf("n = %d, s = %d\n", n, sum(n));
    exit(0);
}
