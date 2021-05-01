#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int j,i=0;
    setbuf(stdout, NULL);
    printf("Counter: \n");
    for (j = 1; j < 1000000; j++)
    {
        printf("%d ", j);
        i++;
        if(i==10){
            printf("\n");
            i=0;
        }
        sleep(1);
        
    }
    return 0;
}