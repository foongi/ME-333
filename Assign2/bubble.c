#include <stdio.h>
#include <string.h>
#define MAXLENGTH 100       // max length of string input

void getString(char *str);  // helper prototypes
void printResult(char *str);
int greaterThan(char ch1, char ch2);
void swap(char *str, int index1, int index2);

int main(void) {
    int len;                  // length of the entered string
    char str[MAXLENGTH];      // input should be no longer than MAXLENGTH
    // here, any other variables you need

    getString(str);
    len = strlen(str);        // get length of the string, from string.h
    
    for(int i = len-1; i > 0; i--) // loop through all the different stopping pts. n-1 down to 1
    {
        for(int start = 0; start < i; start++)  // increment index of left side of potential swap
                                            // starting at 0, and going up to 1 less than the last swappable #
        {
            if(greaterThan(str[start], str[start+1])) // check if char at index start > char at index start+1
            {
                // if so, swap them
                swap(str, start, start+1);
            }
        }
    }
    
    // put nested loops here to put the string in sorted order
    printResult(str);
    return(0);
}

/*
Gets string input from the user (putting it in *str)
*/
void getString(char *str)
{
    printf("Enter the string you'd like to sort: ");
    scanf("%s", str);
    return;
}

/*
Prints the resulting sorted string out (given the string)
*/
void printResult(char *str)
{
    printf("Here is the sorted string:  %s", str);
    return;
}

/*
Checks if the 1st char input is greater (in ascii value) than the second char input.
Returns 1 if true, 0 if false (ch1 less than or equal to ch2).
*/
int greaterThan(char ch1, char ch2)
{
    return ch1 > ch2;
}

/*
Swaps string characters at index 1 and index 2
*/
void swap(char *str, int index1, int index2)
{
    char placeholder = str[index1];
    str[index1] = str[index2];
    str[index2] = placeholder;

    return;
}
