 #include <stdio.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "getword.h"
#define MAXITEM 100 /* max number of words per line */
#define premission 0755
  
  	int pip_e(int pcount,int arg, int in);
	int parse();
	void myhandler(int signum);
	main();
 

