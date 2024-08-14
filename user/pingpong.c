#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int fds[2];
    char byte[1];
    if(pipe(fds) < 0){ // create pipe
      
        exit(2);
    }

    int pid = fork(); //dispatch processes 
    if (pid < 0) {  // error check
        exit(2);
    }

    if(pid == 0){ //child
        read(fds[0], byte, 1);
        printf("%d: received p%cng\n", getpid(), *byte);
        write(fds[1], "o", 1);
        
    }
    else{
        write(fds[1], "i", 1);
        wait(0);
        read(fds[0], byte, 1);
        printf("%d: received p%cng\n", getpid(), *byte);

    }
    close(fds[0]);
    close(fds[1]);
 
    
    
    exit(0);
}

    

