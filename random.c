// 0, 1 중 랜덤 출력

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main(){
    srand(time(NULL));

    while(1){
        int outputValue=rand()%2;

        printf("%d\n", outputValue);
        fflush(stdout);

        sleep(10);
    }
    return 0;
}
