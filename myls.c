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

#define SIZE_FOR_NOW 1024

void listFiles(char *currdir, int flaga, int flagl);

int
main(int argc, char *argv[])
{
    char *currdir;
    char buf[SIZE_FOR_NOW];
    int opt, flaga, flagl;

    flaga =0;
    flagl=0;

    while((opt = getopt(argc, argv, "la")) != -1){
        
        if (opt == 'a'){
            flaga=1;
        }else if(opt == 'l'){
            flagl=1;
        }
    }

    currdir = getcwd(buf , SIZE_FOR_NOW); 
    /* currdir needed to check ofr errors but could be replaced with buff
    */

    printf("the curr directory path is %s\n", currdir);
    
    printf("flaga value is %d\n", flaga);
    printf("flagl value is %d\n", flagl);

    printf("the first argument is %s\n", argv[1]);
    
    listFiles(currdir, flaga, flagl);

    /* delete this later
    */ 
    return 0;
    
}

void listFiles(char *currdir, int flaga, int flagl){

    DIR *firstDir;
    struct dirent *firstDirRead;
    struct passwd *userinfo;
    struct group *groupinfo;
    struct tm *tm;
    char *name;
    char path[SIZE_FOR_NOW], perm[11], date[SIZE_FOR_NOW];
    int mode;
    struct stat sb;

    firstDir = opendir(currdir);

    while((firstDirRead = readdir(firstDir))!= NULL){
        
        /* need to check if a is provided or not 
        */ 
        if (flaga == 0){

            name = firstDirRead->d_name;

            snprintf(path, SIZE_FOR_NOW, "%s%s%s",currdir,"/",name); 

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

                printf("permissions %s\n",perm);

                printf("# of links %ld\n", sb.st_nlink);

                userinfo = getpwuid(sb.st_uid);

                printf("user name is %s\n", userinfo->pw_name);

                groupinfo = getgrgid(sb.st_gid);

                printf("group name is %s\n", groupinfo->gr_name);

                printf("file size: %ld bytes\n", sb.st_size);

                tm = localtime(&(sb.st_mtime));

                strftime(date, SIZE_FOR_NOW, "%b %d %H:%M", tm);

                printf("file date: %s\n", date);
                
                printf("name of the file is %s\n", name);
                
                printf("\n"); 
                
            } 
        }else{
           printf("%s\n", firstDirRead->d_name); 
        }
    }

}
