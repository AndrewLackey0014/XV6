#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("Sleep needs a time\n");
        exit(0);
    }
    sleep(*argv[1]);
    exit(0);
}