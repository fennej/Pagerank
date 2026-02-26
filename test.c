#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(){
    int *tab=malloc(10*sizeof(int));
    for(int i=0;i<10;i++){
        tab[i]=i;
    }
    return 0;
}