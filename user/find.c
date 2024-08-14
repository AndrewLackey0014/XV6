#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), '\0', DIRSIZ-strlen(p));
  // \0 for null space to eliminate the spacing format without = endless loop
  return buf;
}

void
find(char *path, char *search, char *buf, int fd, struct stat st)
{
    char *p;
    struct dirent de;
    struct stat st_next;
    //printf("%d\n", sizeof buf);
    //displaying size of 8?
    if(strlen(path) + 1 + DIRSIZ + 1 > 512){
        printf("find: path too long\n");
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0){
            printf("find: cannot stat %s\n", fmtname(buf));
            // printf("%d\n", st.type);
            continue;
        }
        //search deeper in directory
        if(st.type == T_DIR){
            //dont go into . and .. they are listed as directories
            if(strcmp(fmtname(buf), ".") != 0 && strcmp(fmtname(buf), "..")){
                //for next dir
                int fd_next;
                if((fd_next = open(buf, 0)) < 0){
                    fprintf(2, "find: cannot open %s\n", buf);
                }
                if(fstat(fd, &st_next) < 0){
                    printf("find: cannot stat %s\n", path);
                    close(fd);
                    return;
                }
                //recurse
                find(buf, search, buf, fd_next, st_next);
                close(fd_next);
            }
        //at a file
        }else if(st.type == T_FILE){
            if(strcmp(fmtname(buf), search) == 0){
                //found the file
                printf("%s\n", buf);
            }   
        }
        //this is the main print

        //printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
}

int main(int argc, char *argv[]){
    //make sure it has a target
    if(argc < 2){
        printf("Find needs a target\n");
        exit(0);
    }
    for(int i = 2; i < argc; i++){
        char buf[512];
        int fd;
        struct stat st;
        if((fd = open(argv[1], 0)) < 0){
            fprintf(2, "find: cannot open %s\n", argv[1]);
        }

        if(fstat(fd, &st) < 0){
            fprintf(2, "find: cannot stat %s\n", argv[1]);
            close(fd);
        }
        find(argv[1], argv[i], buf, fd, st);
        close(fd);
    }
    exit(0);

}