/*
 * myls.c
 * 
 * Implements ls with the -a -l option functionality.  
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
#include <errno.h>
#include <libgen.h>

#define BUF_SIZE 1024
#define MAX_COL_SIZE 80

void listFile(char *currName, int showHiddenFiles, int isLongListing); 
int listFilesDirectory(char *currName, int showHiddenFiles, int isLongListing);
int longListing(char *name, char *currName, int showFullPath);
long counterCheck(long currCount, long size); 

int main(int argc, char *argv[]) {
    char *currName;
    char buf[BUF_SIZE];
    int showHiddenFiles, isLongListing, gaveMultiFiles;
    int opt;
    int i;
    int minorErrorOccured;

    showHiddenFiles = 0;
    isLongListing = 0;
    gaveMultiFiles = 0; 
    minorErrorOccured = 0;

    while((opt = getopt(argc, argv, "la")) != -1) {
        switch(opt) {
            case 'a': 
                showHiddenFiles = 1;
                break;
            case 'l':
                isLongListing = 1;
                break;
            default:
                exit(1); 
        }
    }

    if(argc - optind > 1) {
        gaveMultiFiles = 1; 
    } else if(argc - optind == 0) {
        currName = getcwd(buf, BUF_SIZE); 
        if(currName == NULL) {
            perror("getcwd");
            exit(2);
        }
        minorErrorOccured = listFilesDirectory(currName, showHiddenFiles, isLongListing);
    }
    
    // NOTE: Assumptions are dangerous (could use stat to be more clear)
    for(i = optind; i < argc; i++) {
        if(opendir(argv[i]) == NULL) {
            if(errno == ENOTDIR) {
                listFile(argv[i], showHiddenFiles, isLongListing);
            } else {
                perror(argv[i]);
            }
        } else {
            if(gaveMultiFiles == 1) {
                printf("\n%s: \n", argv[i]);
            }
            minorErrorOccured = listFilesDirectory(argv[i], showHiddenFiles, isLongListing);
        }
    }
    printf("\n");
    return minorErrorOccured; //Return a non-zero value if something went wrong, and zero otherwise
}

/*
* listFile()
* Print out the file information when a file is provided as an argument
*/
void listFile(char *currName, int showHiddenFiles, int isLongListing) {
    char *parentDir; 
    char *base; 
    int fullPathProvided; 

    fullPathProvided = 0; 
    base = basename(currName); 
    parentDir = dirname(currName); 

    if (strcmp(parentDir, ".") != 0) {
        fullPathProvided = 1;
    } 

    if(showHiddenFiles == 0) {
        if(base[0] != '.') {
            if(isLongListing == 1) {
                longListing(base, parentDir, fullPathProvided);
            } else {
                if (fullPathProvided == 0) {
                    printf("%s ", base);
                } else {
                    printf("%s/%s ", parentDir, base);
                }
            }
        } 
    } else {
        if(isLongListing == 1) {
            longListing(base, parentDir, fullPathProvided);
        } else {
            if (fullPathProvided == 0) {
                printf("%s ", base);
            } else {
                printf("%s/%s ", parentDir, base);
            }
        } 
    }
}

/*
* listFilesDirectory() 
* Print out the relevant information on all the items in a directory 
*/
int listFilesDirectory(char *currName, int showHiddenFiles, int isLongListing) {
    DIR *dirPointer;
    struct dirent *itemRead;
    char *name;
    long charCounter; 

    charCounter = 0; 
    dirPointer = opendir(currName);

    if(dirPointer == NULL) {
        perror("opendir");
    }
    
    errno = 0; 

    while((itemRead = readdir(dirPointer)) != NULL) {
        
        name = itemRead->d_name;

        if(showHiddenFiles == 0) {
            if(name[0] != '.') {
                if(isLongListing == 1) {
                    longListing(name, currName, 0);
                } else {
                    charCounter = counterCheck(charCounter, strlen(name)); 
                    printf("%s ", name);  
                }
            }
        } else {
            if(isLongListing == 1) {
                longListing(name, currName, 0);
            } else {
                charCounter = counterCheck(charCounter, strlen(name)); 
                printf("%s ", name);
            }
        }
        errno = 0;
    }
    if(errno != 0) {
        perror("readdir");
        return -1;
    }

    return 0;
}

/*
* longListing 
* Print out the extra details associated with the -l option of ls for an item 
*/
int longListing(char *name, char *currName, int showFullPath) {
    struct passwd *userInfo;
    struct group *groupInfo;
    struct tm *timePointer;
    char perm[11], date[14];
    int mode;
    struct stat sb;
    char path[BUF_SIZE];

    snprintf(path, BUF_SIZE, "%s%s%s", currName, "/", name);
    
    if(stat(path, &sb) == -1) {  
        perror("stat");
        return -1;
    }
                
    mode = sb.st_mode;

    perm[0] = ((S_ISDIR(mode) != 0) ? 'd': '-');

    perm[1] = (((S_IRUSR & mode) != 0) ? 'r': '-');
    perm[2] = (((S_IWUSR & mode) != 0) ? 'w': '-');
    perm[3] = (((S_IXUSR & mode) != 0) ? 'x': '-');

    perm[4] = (((S_IRGRP & mode) != 0) ? 'r': '-');
    perm[5] = (((S_IWGRP & mode) != 0) ? 'w': '-');
    perm[6] = (((S_IXGRP & mode) != 0) ? 'x': '-');

    perm[7] = (((S_IROTH & mode) != 0) ? 'r': '-');
    perm[8] = (((S_IWOTH & mode) != 0) ? 'w': '-');
    perm[9] = (((S_IXOTH & mode) != 0) ? 'x': '-');

    perm[10] = '\0';

    printf("%s ", perm);

    printf("%ld ", sb.st_nlink);

    userInfo = getpwuid(sb.st_uid);
    if(userInfo == NULL) {
        printf("%d ", sb.st_uid);
    } else {
        printf("%s ", userInfo->pw_name);
    }

    groupInfo = getgrgid(sb.st_gid);
    if(groupInfo == NULL) {
        printf("%d ", sb.st_gid);
    } else {
        printf("%s ", groupInfo->gr_name);
    }
    
    printf("%ld ", sb.st_size);

    timePointer = localtime(&(sb.st_mtime));
    if(timePointer == NULL) {
        printf("localtime encountered an error\n");
        exit(6);
    }
    
    strftime(date, BUF_SIZE, "%b %d %H:%M", timePointer); 

    printf("%s ", date);

    if (showFullPath == 0) {
        printf("%s\n", name);
    } else {
        printf("%s\n", path);
    }

    return 0; 
}

/*
* counterCheck
* Keep count of the number of columns occupied by printed items 
*/
long counterCheck(long currCount, long size) {
    /* Adding 1 for each space */
    currCount = currCount + 1 + size;
    if(currCount >= MAX_COL_SIZE) {
        printf("\n"); 
        return size; 
    } else {
        return currCount; 
    }
}


