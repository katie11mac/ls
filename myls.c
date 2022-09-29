/*
 * myls.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define BUF_SIZE 1024

void listFiles(char *currdir, int flaga, int flagl);

int
main(int argc, char *argv[])
{
    char *currdir;
    char buf[BUF_SIZE];
    int opt, flaga, flagl;

    flaga = 0;
    flagl = 0;

    while((opt = getopt(argc, argv, "la")) != -1){
        if (opt == 'a'){
            flaga = 1;
        }else if(opt == 'l'){
            flagl = 1;
        }
    }

    currdir = getcwd(buf , BUF_SIZE); 
    /* currdir needed to check ofr errors but could be replaced with buff
    */
    
    listFiles(currdir, flaga, flagl);

    /* delete this later
    */ 
    return 0;
    
}

void listFiles(char *currdir, int flaga, int flagl){

    DIR *dirPointer;
    struct dirent *itemRead;
    struct passwd *userInfo;
    struct group *groupInfo;
    struct tm *timePointer;
    char *name;
    char path[BUF_SIZE], perm[11], date[BUF_SIZE];
    int mode;
    struct stat sb;

    dirPointer = opendir(currdir);

    while((itemRead = readdir(dirPointer))!= NULL){
        
        /* need to check if a is provided or not 
        */ 
        if (flaga == 0){

            name = itemRead->d_name;

            snprintf(path, BUF_SIZE, "%s%s%s",currdir,"/",name); 

            if (name[0] != '.') {
                
                stat(path,&sb);
                
                mode = sb.st_mode;

                perm[0]= ((S_ISDIR(mode) != 0) ? 'd': '-');

                perm[1]= (((S_IRUSR & mode) != 0) ? 'r': '-');
                perm[2]= (((S_IWUSR & mode) != 0) ? 'w': '-');
                perm[3]= (((S_IXUSR & mode) != 0) ? 'x': '-');

                perm[4]= (((S_IRGRP & mode) != 0) ? 'r': '-');
                perm[5]= (((S_IWGRP & mode) != 0) ? 'w': '-');
                perm[6]= (((S_IXGRP & mode) != 0) ? 'x': '-');

                perm[7]= (((S_IROTH & mode) != 0) ? 'r': '-');
                perm[8]= (((S_IWOTH & mode) != 0) ? 'w': '-');
                perm[9]= (((S_IXOTH & mode) != 0) ? 'x': '-');

                perm[10] = '\0';

                printf("%s ",perm);

                printf("%ld ", sb.st_nlink);

                userInfo = getpwuid(sb.st_uid);

                printf("%s ", userInfo->pw_name);

                groupInfo = getgrgid(sb.st_gid);

                printf("%s ", groupInfo->gr_name);

                printf("%ld ", sb.st_size);

                timePointer = localtime(&(sb.st_mtime));

                strftime(date, BUF_SIZE, "%b %d %H:%M", timePointer);

                printf("%s ", date);
                
                printf("%s ", name);
                
                printf("\n"); 
                
            } 
        }else{
           printf("%s\n", itemRead->d_name); 
        }
    }

}

void longListing(int flagl,)
