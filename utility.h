#define RM 1
#define DV 2
#define FORCE 10
#define RECURSIVE 20
#define NORMAL 0
#define BUFSIZE 256
#define DUMPSTER_PATH "./Trash"


int removeAll(int numPaths, char paths[][BUFSIZE], char *DUMPSTER, int fmode, int rmode);

int removeDir(char *path, char *DUMPSTER, int fmode);

int removeFile(char *path, char *DUMPSTER, int fmode);

int moveFile(char *path, char *DUMPSTER);

int checkExistance(char *filename, char *dirname);

char *toAbsolutePath(char *path);

char *toDumpsterPath(char *path, char *DUMPSTER);

int extractPaths(int argc, char *argv[], char paths[][BUFSIZE], int mode, char *DUMPSTER);

void printUsage();
