#include <stdio.h>

#define NUMBER_BOOKS 26

typedef struct{
    char name[100];
    int pages;
    float price;
} book;

int main() {
    printf("hello\n");

    book mybook;
    mybook.pages = 100;
    mybook.price = 19.99;
    
    book library[NUMBER_BOOKS];
    for (int i=0; i< NUMBER_BOOKS; i++)
    {
        sprintf(library[i].name, "%c", "A"+i);
    }

}

void change_book_name(book *mybook)
{
    sprintf(mybook->name, "Ethan");
}