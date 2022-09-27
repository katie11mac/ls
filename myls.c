/*
 * myls.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

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
    char *name;
    int mode, num;
    struct stat sb;

    firstDir = opendir(currdir);

    while((firstDirRead = readdir(firstDir))!= NULL){
        
        /* need to check if a is provided or not 
        */ 
        if (flaga == 0){

            name = firstDirRead->d_name;

            if (name[0] != '.') {
                
                num = stat(currdir,&sb);
                
                mode = sb.st_mode;
                
                printf("name of the file is %s\n", name);
                
                printf("the st_mode of %s is %d\n", name, mode);
                
            } 
        }else{
           printf("%s\n", firstDirRead->d_name); 
        }
    }

}
