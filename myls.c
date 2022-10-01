/*
 * myls.c
 * 
 * Lindsey Schweizter and Katie Macalintal 
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
#include <errno.h>
#include <libgen.h>


#define BUF_SIZE 1024

void listFilesDirectory(char *currName, int flaga, int flagl);
void longListing(char *name, char *currName);
void listFile(char *currName, int flaga, int flagl); 
long counterCheck(long currCount, long size); 
long lenOfInt(int num); 
long lenOfLong(long num); 

int main(int argc, char *argv[]){
    char *currName;
    char buf[BUF_SIZE];
    int opt, flaga, flagl, i, flagMulti;

    flaga = 0;
    flagl = 0;
    flagMulti = 0; 

    while((opt = getopt(argc, argv, "la")) != -1){
       if(opt == 'a'){
            flaga = 1;
        }else if(opt == 'l'){
            flagl = 1;
        }
    }

    if(argc - optind > 1){
        flagMulti = 1; 
    }else if(argc - optind == 0){
        currName = getcwd(buf, BUF_SIZE); 
        if(currName == NULL){
            perror("getcwd");
            exit(1);
        }
        listFilesDirectory(currName, flaga, flagl);
    }
    
    for(i = optind; i < argc; i++){
        if(opendir(argv[i]) == NULL){
            if(errno == ENOTDIR){
                listFile(argv[i], flaga, flagl); 
            }else{
                printf("%s: No such file or directory\n", argv[i]); 
            }
        }else{
            if(flagMulti == 1){
                printf("%s: \n", argv[i]); 
            }
            listFilesDirectory(argv[i], flaga, flagl);
        }
    }

    /* delete this later
    */ 
    return 0;
}

void listFile(char *currName, int flaga, int flagl){
    char *parentDir; 
    DIR *dirPointer;
    struct dirent *itemRead;
    char *name;

    parentDir = dirname(currName); 
    
    dirPointer = opendir(parentDir);

    if(dirPointer == NULL){
        perror("opendir");
    }
    
    errno = 0; 

    while((itemRead = readdir(dirPointer)) != NULL){
        
        name = itemRead->d_name;

        if(strcmp(name, currName) == 0){
            if(flaga == 0){
                if(name[0] != '.'){
                    if(flagl == 1){
                        longListing(name, parentDir);
                    }else{
                        printf("%s ", name);  
                    }
                } 
            }else{
                if(flagl == 1){
                    longListing(name, parentDir);
                }else{
                    printf("%s ", name);
                } 
            }
        }
    }
    printf("\n");

    if(errno == 1){
        exit(2);
    }

}

void listFilesDirectory(char *currName, int flaga, int flagl){

    DIR *dirPointer;
    struct dirent *itemRead;
    char *name;
    long charCounter; 

    charCounter = 0; 
    dirPointer = opendir(currName);

    if(dirPointer == NULL){
        perror("opendir");
    }
    
    errno = 0; 

    while((itemRead = readdir(dirPointer)) != NULL){
        
        name = itemRead->d_name;

        if(flaga == 0){
            if(name[0] != '.'){
                if(flagl == 1){
                    longListing(name, currName);
                }else{
                    charCounter = counterCheck(charCounter, strlen(name)); 
                    printf("%s ", name);  
                }
            } 
        }else{
            if(flagl == 1){
                longListing(name, currName);
            }else{
                charCounter = counterCheck(charCounter, strlen(name)); 
                printf("%s ", name);
            } 
        }
    }
    printf("\n");

    if(errno == 1){
        exit(2);
    }
}


void longListing(char *name, char *currName){

    struct passwd *userInfo;
    struct group *groupInfo;
    struct tm *timePointer;
    char perm[11], date[14];
    int mode;
    struct stat sb;
    char path[BUF_SIZE];
    int charCounter; 


    snprintf(path, BUF_SIZE, "%s%s%s", currName, "/", name);
    
    if(stat(path, &sb) == -1){
        perror("stat");
    }
                
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

    userInfo = getpwuid(sb.st_uid);
    groupInfo = getgrgid(sb.st_gid);
    timePointer = localtime(&(sb.st_mtime));

    if(timePointer == NULL){
        printf("localtime encountered an error\n");
        exit(8);
    }
    
    if(strftime(date, BUF_SIZE, "%b %d %H:%M", timePointer) == 0){
        printf("strftime encountered an error\n");
    }

    printf("%s ", perm);
    charCounter = sizeof(perm); 

    charCounter = counterCheck(charCounter, lenOfLong(sb.st_nlink)); 
    printf("%ld ", sb.st_nlink);

    if(userInfo == NULL){
        perror("getpwuid");
        charCounter = counterCheck(charCounter, lenOfInt(sb.st_uid)); 
        printf("%d ", sb.st_uid);
    }else{
        charCounter = counterCheck(charCounter, strlen(userInfo->pw_name));
        printf("%s ", userInfo->pw_name);
    }

    if(groupInfo == NULL){
        perror("getgrgid");
        charCounter = counterCheck(charCounter, lenOfInt(sb.st_gid)); 
        printf("%d ", sb.st_gid);
    }else{
        charCounter = counterCheck(charCounter, strlen(groupInfo->gr_name));
        printf("%s ", groupInfo->gr_name);
    }
    
    charCounter = counterCheck(charCounter, lenOfLong(sb.st_size)); 
    printf("%ld ", sb.st_size);

    charCounter = counterCheck(charCounter, sizeof(date));
    printf("%s ", date);

    charCounter = counterCheck(charCounter, strlen(name));
    printf("%s \n", name);
}

long counterCheck(long currCount, long size){
    /* Adding 1 for each space */
    currCount = currCount + 1 + size;
    printf("CURRENT CHAR COUNTER: %ld ", currCount); 
    if(currCount >= 80){
        printf("\n"); 
        return size; 
    }else{
        return currCount; 
    }
}


long lenOfInt(int num){
    long len = 0;

    while (num != 0){
        len++;
        num /= 10;
    }
    
    return len;
}

long lenOfLong(long num){
    long len = 0;

    while (num != 0){
        len++;
        num /= 10;
    }
    
    return len;
}

