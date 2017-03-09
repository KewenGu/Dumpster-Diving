
Kewen Gu
Jan 24, 2016

There're 4 files including rm.c, dv.c, dump.c, and Makefile.

Before running the program, make sure that your machine has a environment variable TRASH has been set.
You can set the environment variable using "export TRASH=$TRASH:/your/path"
If TRASH environment variable is not set when running the program, a new Trash directory will by created by the program in the current working directory.

Use command "make" to compile the files.
After making the program, enter
    "./rm [-f | -h | -r] file [file ...]" or
    "./dv [-h] file [file ...]" or
    "./dump [-h]"

See usage by specifying the '-h' option.
