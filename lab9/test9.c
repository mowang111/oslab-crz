#include <stdio.h>

void memtest(){
    while(1);
}

int main(){
    printf("memtest address: %p\n",(void*)memtest);
    memtest();
    return 0;
}