#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <libgen.h>
#include <dirent.h>

#include "utility.h"


int main(int argc, char *argv[]) {

    char *DUMPSTER = (char *) calloc(BUFSIZE, sizeof(char));
    int opt;
    int fmode = NORMAL, rmode = NORMAL;

    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
                printUsage();
                exit(0);
                break;
        }
    }

    if ((DUMPSTER = getenv("TRASH")) == NULL) {
        printf("Cannot access environment TRASH.\nCreating TRASH environment for user in the current working directory...\n");
        if (access(DUMPSTER_PATH, F_OK) < 0) {
            if (mkdir(DUMPSTER_PATH, 777) < 0) {
                perror("Cannot make dumpster directory");
                exit(-1);
            }
            perror("");
            exit(-1);
        }
        if (setenv("TRASH", DUMPSTER_PATH, 0) < 0) {
            perror("Cannot set environment TRASH");
            exit(-1);
        }
        else {
            printf("Environment created.\n");
            DUMPSTER = getenv("TRASH");
        }
    }

    DIR *dir;
    if ((dir = opendir(DUMPSTER)) == NULL) {
        perror("Cannot open dumpster directory");
        exit(-1);
    }

    struct dirent *d;
    struct stat s;
    char newPath[BUFSIZE];
    char errMsg[BUFSIZE];

    while ((d = readdir(dir)) != NULL) {
        /* Skip the names "." and ".." as we don't want to recurse on them. */
        if (strcmp(d->d_name, ".") && strcmp(d->d_name, "..")) {

            strcpy(newPath, DUMPSTER);
            strcat(newPath, "/");
            strcat(newPath, d->d_name);
            // printf("%s\n", newPath);

            if (lstat(newPath, &s) < 0) {
                strcpy(errMsg, newPath);
                perror(errMsg);
            }

            if (S_ISDIR(s.st_mode)) {
                removeDir(newPath, DUMPSTER, FORCE);
            }
            else {
                removeFile(newPath, DUMPSTER, FORCE);
            }
        }
    }
    closedir(dir);
}

void printUsage() {
    printf("Usage: ./dump [-h]\n  -h : display basic usage message\n");
}
