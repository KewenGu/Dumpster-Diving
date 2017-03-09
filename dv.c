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
    char paths[argc - 1][BUFSIZE];
    char cwd[BUFSIZE];
    int opt, numPaths;

    if (argc < 2) {
        fprintf(stderr, "Usage: ./dv [-h] file [file ...]\nSee \'./dv -h\'\n");
        exit(-1);
    }
    else {
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

        getcwd(cwd, BUFSIZE);
        numPaths = extractPaths(argc, argv, paths, DV, DUMPSTER);
        removeAll(numPaths, paths, cwd, NORMAL, RECURSIVE);

    }
}

void printUsage() {
    printf("Usage: ./dv [-h] file [file ...]\n  -h : display basic usage message\n  file [file ...]: file(s) to be removed\n");
}
