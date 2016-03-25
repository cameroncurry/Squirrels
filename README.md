# Squirrels
Parallel Design Patterns Coursework: Implementation of squirrel model using the Actor Pattern

##Compile
Compile using Makefile in src directory (requires mpic++):
```
$ make
```

##Run
Run simulation with the run script which argument of number of mpi processes:
```
$ ./run n-procs  
```
eg. ./run 64

##Verbose Output
Verbose output is possible using `SQURL_LOG` variable in squirrel_const.h within actors directory.

Turn on verbose output with: ```#define SQURL_LOG 1```
Turn off: ```#define SQURL_LOG 0```

Verbose output is "grepable"

To grep verbose output, redirect standard output to a file of your choice

To see actors starting up and shutting down, grep for "INIT"
