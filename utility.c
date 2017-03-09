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
#include <utime.h>

#include "utility.h"


int removeAll(int numPaths, char paths[][BUFSIZE], char *DUMPSTER, int fmode, int rmode) {

    struct stat s;
    char errMsg[BUFSIZE];
    int i;

    for (i = 0; i < numPaths; i++) {
        if (lstat(paths[i], &s) < 0) {
            strcpy(errMsg, paths[i]);
            perror(errMsg);
        }
        else {
            if (S_ISDIR(s.st_mode)) {
                if (rmode)
                    removeDir(paths[i], DUMPSTER, fmode);
                else
                    fprintf(stderr, "Cannot remove \'%s\': Is a directory.\n", paths[i]);
            }
            else {
                removeFile(paths[i], DUMPSTER, fmode);
            }
        }
    }

    return 0;
}


int removeDir(char *path, char *DUMPSTER, int fmode) {

    DIR *dir;
    char errMsg[BUFSIZE];
    if ((dir = opendir(path)) == NULL) {
        strcpy(errMsg, "Cannot open directory ");
        strcat(errMsg, path);
        perror(errMsg);
        exit(-1);
    }

    struct dirent *d;
    struct stat s;
    char newDUMPSTER[BUFSIZE], newPath[BUFSIZE];

    strcpy(newDUMPSTER, DUMPSTER);
    strcat(newDUMPSTER, "/");
    strcat(newDUMPSTER, basename(path));

    if (!fmode) {
        int count = checkExistance(basename(path), DUMPSTER);

        if (count > 0) {
            char suffix[8];
            sprintf(suffix, "%d", count);
            strcat(newDUMPSTER, ".");
            strcat(newDUMPSTER, suffix);
        }
        printf("%s\n", newDUMPSTER);

        if (access(newDUMPSTER, F_OK) < 0) {
            if (mkdir(newDUMPSTER, 777) < 0) {
                perror("Cannot make new directory in dumpster");
                exit(-1);
            }
        }
    }
    while ((d = readdir(dir)) != NULL) {
        /* Skip the names "." and ".." as we don't want to recurse on them. */
        if (strcmp(d->d_name, ".") && strcmp(d->d_name, "..")) {

            strcpy(newPath, path);
            strcat(newPath, "/");
            strcat(newPath, d->d_name);
            // printf("%s\n", newPath);

            if (lstat(newPath, &s) < 0) {
                strcpy(errMsg, newPath);
                perror(errMsg);
            }

            if (S_ISDIR(s.st_mode)) {
                removeDir(newPath, newDUMPSTER, fmode);
            }
            else {
                removeFile(newPath, newDUMPSTER, fmode);
            }
        }
    }

    closedir(dir);
    remove(path);
    return 0;
}


int removeFile(char *path, char *DUMPSTER, int fmode) {

    if (fmode) {
        printf("%s\n", path);
        unlink(path);
    }
    else {
        char newPath[BUFSIZE];
        strcpy(newPath, DUMPSTER);
        strcat(newPath, "/");
        strcat(newPath, basename(path));
        printf("%s\n", newPath);

        int count = checkExistance(basename(path), DUMPSTER);

        if (count > 0) {
            char suffix[8];
            sprintf(suffix, "%d", count);
            strcat(newPath, ".");
            strcat(newPath, suffix);
        }

        if (rename(path, newPath) < 0) {
            if (errno == EXDEV) {
                printf("File is on another partition.\n");
                moveFile(path, newPath);
                unlink(path);
            }
            else {
                char errMsg[BUFSIZE];
                strcpy(errMsg, "Cannot remove file ");
                strcat(errMsg, path);
                perror(errMsg);
                exit(-1);
            }
        }
    }
    return 0;
}


int moveFile(char *path, char *DUMPSTER) {

    int infile, outfile;
    char errMsg[BUFSIZE];
    struct stat s;
    mode_t permission;
    struct utimbuf ut;

    if ((infile = open(path, O_RDONLY)) < 0) {
        strcpy(errMsg, "Cannot open file ");
        strcat(errMsg, path);
        perror(errMsg);
        exit(-1);
    }

    if ((outfile = open(DUMPSTER, O_WRONLY | O_CREAT)) < 0) {
        strcpy(errMsg, "Cannot open file ");
        strcat(errMsg, path);
        perror(errMsg);
        exit(-1);
    }

    int bytesRead;
    unsigned char *buffer = malloc(4096);
    while ((bytesRead = read(infile, buffer, 4096))) {
        write(outfile, buffer, bytesRead);
    }

    if (lstat(path, &s) < 0) {
        strcpy(errMsg, path);
        perror(errMsg);
        exit(-1);
    }

    if (s.st_mode & S_IRUSR)
        permission = S_IRUSR;
    if (s.st_mode & S_IWUSR)
        permission = permission | S_IWUSR;
    if (s.st_mode & S_IXUSR)
        permission = permission | S_IXUSR;
    if (s.st_mode & S_IRGRP)
        permission = permission | S_IRGRP;
    if (s.st_mode & S_IWGRP)
        permission = permission | S_IWGRP;
    if (s.st_mode & S_IXGRP)
        permission = permission | S_IXGRP;
    if (s.st_mode & S_IROTH)
        permission = permission | S_IRGRP;
    if (s.st_mode & S_IWOTH)
        permission = permission | S_IWGRP;
    if (s.st_mode & S_IXOTH)
        permission = permission | S_IXGRP;

    fchmod(outfile, permission);

    ut.actime = s.st_atime;
    ut.modtime = s.st_mtime;
    utime(DUMPSTER, &ut);

    close(infile);
    close(outfile);
    free(buffer);
    return 0;
}


int checkExistance(char *filename, char *dirname) {

    char path[BUFSIZE];
    strcpy(path, dirname);
    strcat(path, "/");
    strcat(path, filename);

    int count = 0;
    char suffix[8];
    char newPath[BUFSIZE];
    strcpy(newPath, path);

    while (access(newPath, F_OK) == 0) {
        count++;
        sprintf(suffix, "%d", count);
        strcpy(newPath, path);
        strcat(newPath, ".");
        strcat(newPath, suffix);
    }

    return count;
}


char *toAbsolutePath(char *path) {

    if (!strcmp(path, "/") || !strcmp(path, ".") || !strcmp(path, "..")) {
        fprintf(stderr, "Cannot remove directory %s\n", path);
        exit(-1);
    }
    else if (strstr(path, "/") == NULL) {
        struct stat s;
        if (stat(path, &s) < 0) {
            perror(path);
            exit(-1);
        }
        char cwd[BUFSIZE];
        getcwd(cwd, BUFSIZE);
        char *absPath = (char *) calloc(BUFSIZE, sizeof(char));
        strcpy(absPath, cwd);
        strcat(absPath, "/");
        strcat(absPath, path);
        if (S_ISDIR(s.st_mode))
            strcat(absPath, "/");
        // printf("%s\n", absPath);
        return absPath;
    }
    else
        return path;
}


char *toDumpsterPath(char *path, char *DUMPSTER) {

    char *dumpPath = (char *) calloc(BUFSIZE, sizeof(char));
    strcpy(dumpPath, DUMPSTER);
    strcat(dumpPath, "/");
    strcat(dumpPath, basename(path));
    return dumpPath;
}


int extractPaths(int argc, char *argv[], char paths[][BUFSIZE], int mode, char *DUMPSTER) {

    int i, n = 0;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            if (mode == RM)
                strcpy(paths[n], toAbsolutePath(argv[i]));
            else if (mode == DV)
                strcpy(paths[n], toDumpsterPath(argv[i], DUMPSTER));
            n++;
        }
    }
    return n;
}
