#include <stdio.h>  


int main()
{
    for(int i=33; i <= 127; i++)
    {
        printf("%3d: %c   ", i, i);
        if((i-2) % 5 == 0)
        {
            printf("\n\n");
        }
    }
    return 0;
}