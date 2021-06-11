#include <stdio.h>
#include <unistd.h>

int ackerman(int m, int n)
{
    if (m == 0)
        return n + 1;
    if (m > 0 && n == 0)
        return ackerman(m - 1, 1);
    if (m > 0 && n > 0)
        return ackerman(m - 1, ackerman(m, n - 1));
}

int main(int argc, char* argv[])
{
    int j;
    setbuf(stdout, NULL);
    printf("sleeper 1\n");
    for (j = 0; j < 1000000; j++)
    {
        //ackerman(3, 11);
        printf("%d ", j);
        //fflush(stdout);
        sleep(1);
    }
    return 0;
}