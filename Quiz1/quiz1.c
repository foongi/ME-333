#include <stdio.h>

#define MYLEN 10

int main() {

    int i;
    int nums[MYLEN];
    int array_nums = 0;

    while(i < MYLEN){
        printf("Input a number between -5 and 5\n");
        scanf("%d", &nums[i]);

        if (nums[i] >= -5 && nums[i] <= 5) {
            i++;
        } 
        else {
            print("invalid number");
        }
    }
    


    return 0;

}