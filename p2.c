/*
Nick schneider
816879772
John Carroll
cs570
program2
10/12/2016
This program is everthing that program two does and i have added feautres so there can be up to ten pipes, $value gets the
enviroment variable, environ allows the setting and print of variables.
*/

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

char *newargv[MAXITEM];											//holds the pointers to where the comand lives in the comand array
char comand[256];											//holds the line typed in to the shell
int p_ipe, g_than, l_than, a_nd, p_count,arg_pos[20];
int slash_flag,g_pipe,flags,g_redirect, l_redirect,end;
char *file_gthan, *file_lthan;
char *env_name, *temp;

//recursive pipe funtion
	int pip_e(int pcount,int arg, int in){
//if it is hte last pipe	
		if(pcount == 0){
			if(g_pipe ==1){
				flags =O_CREAT|O_EXCL|O_WRONLY;
				if((g_redirect = open(file_gthan,flags,premission)) < 0){
					perror("file alrady exsists");
					_exit(-1);
					}
				if(dup2(g_redirect,STDOUT_FILENO) ==-1){
					perror("dup2 failed in pipe on last stdout");
					_exit(-1);
					}
				g_pipe =0;
				}
			if(in != STDIN_FILENO)
				dup2(in,STDIN_FILENO);
			if(execvp(newargv[arg_pos[arg]],newargv+arg_pos[arg]) == -1){
				perror("execvp failed in pipe on last grandchild");
				_exit(-1);
				}
			return 0;			
		}else {
//if it is first or middle pipes
			pid_t grand_kid;
			int fd[2];
			if(pipe(fd) == -1){
				perror("pipe fail on fd in iteration");
				return -1;	
				}
			if((grand_kid =fork()) == -1){
				perror("fork failed in a pipe");
				return -1;
				}
			else if(grand_kid == 0){	
				close(fd[0]);
				if(in != STDIN_FILENO)
					dup2(in,STDIN_FILENO);
				if(fd[1] != STDOUT_FILENO)									
					dup2(fd[1],STDOUT_FILENO);
				if(execvp(newargv[arg_pos[arg]],newargv+arg_pos[arg])==-1){
					perror("execvp failed during a pipe in one of the grandchildren");
					fprintf(stderr,"failed in the %d child \n",arg);
					_exit(-1);
					}
				_exit(0);
			}else{
				close(fd[1]);
				close(in);
				pip_e(pcount-1,arg+1,fd[0]);
				for(;;){
					pid_t pid;
					pid = wait(NULL);
					if(pid == grand_kid)
						break;
					}
				}
			return 0;
			}
		return 0;
		}

//parces the input from get word and sets flags
	int parse(){
		int current = 0,position=0,size,greater_than=0,less_than=0;
		p_ipe=0, g_than=0, l_than=0, p_count=0;
		memset(newargv, '\0', sizeof(newargv));
		memset(arg_pos,0,sizeof(arg_pos));
		for(;;){

			if(current == MAXITEM){
				break;
				}
			size=0;
			size = getword(&comand[position]);
			if(size == 0)
				break;
			if(size == -1 && newargv[0] == '\0'){
				return -9;
				} 
			if(size == -1 && newargv[0] != '\0'){
				end =1;		
				}
			if(size == -1 && comand[position] == '$'){
				if(position == -1)
					return -9;
				else
					end=1;
				}
			if(p_ipe ==1){
				arg_pos[p_count]  = current;
				p_ipe=0;
				}
			if(comand[position]=='$' && size < -1){
				temp = &comand[position+1];
				if(greater_than == 1){
					file_gthan = getenv(temp);
					if(file_gthan == NULL){
						perror("unknown env variable next to redirect");
						return -7;
						}
					if(p_count > 0)
						g_pipe =1;
					greater_than=0;
				}else if(less_than ==1){
					file_lthan = getenv(temp);
					if(file_lthan == NULL){
						perror("unknown env variable next to redirect");
						return -7;
						}
					less_than=0;
				}else {
					env_name=getenv(temp);
					if(!env_name){
						perror("getenv failed. unknown variable");
						return -7;
						}
					newargv[current]= env_name;
					}
				current++;
				continue;
				}
			if(greater_than == 1){
				if(p_count> 0){
					g_pipe=1;
					}
				file_gthan = &comand[position];
				position+=(size+1);
				greater_than=0;
				continue;
				}
			 if(less_than == 1){
			 	file_lthan = &comand[position];
				position+=(size+1);
				less_than=0;
				continue;
				}		
			if(size == 1 && comand[position] == '|'){
				if(slash_flag ==1){
					newargv[current] = &comand[position];
					position+=(size+1);
					current++;
					slash_flag =0;
					continue;	
					}
				newargv[current] = NULL;
				current ++;
				position+=(size+1);
				p_ipe=1;
				p_count ++;
				continue;
				}
			if(size == 1 && comand[position] == '>'){
				if(slash_flag ==1){
					newargv[current] = &comand[position];
					position+=(size+1);
					current++;
					slash_flag =0;
					continue;	
					}
				greater_than=1;
				g_than +=1;
				position+=(size+1);
				continue;
				}
			if(size == 1 && comand[position] == '<'){
				if(slash_flag ==1){
					newargv[current] = &comand[position];
					position+=(size+1);
					current++;
					slash_flag =0;
					continue;	
					}
				less_than=1;
				l_than += 1;
				position+=(size+1);
				continue;
				}
			newargv[current] = &comand[position];
			position+=(size+1);
			current++;
			slash_flag = 0;
			}
		newargv[current]=NULL;
		if(comand[position-2] =='&' && slash_flag ==0){
		 	a_nd=1;
			newargv[current-1] ='\0';
			}			
		return position-1;
	}
	
	
// for sigterm
	void myhandler(int signum){
		printf("end");
	}
	
	
    	 main(){
	   signal(SIGTERM,myhandler);
           int parsetest,bgchild,eror;
	   pid_t kid_pid;
	   
//start the loop
           for (;;) {
	   	parsetest =0, bgchild=0, a_nd=0;
                printf("%s","p2: ");
		if((parsetest=parse()) == 0)
			continue;
		
		
//test to see is parce found an error and continue to next promp
		if(parsetest == -7){				
			continue;
			}
		if(parsetest == -9)
			break;
		if(newargv[0] == '\0'){
			if(g_than ==1)
				perror("can not write empty to file this way");
			if(l_than ==1)
				perror("can not read from nothing");
			continue;
			}
					
//test for cd then handle the cd comand			
  		if(strcmp(newargv[0],"cd") == 0) {		
			if(newargv[2] != NULL){
				perror("too many argumant to cd");
				continue;
				}
			char* path;
			if(newargv[1]==NULL){
				path = getenv("HOME");
				char* rst = strchr(path,'\n');
				if(rst != NULL)
					*rst= '\0';
				if(chdir(path)==-1)
					perror("chdir home didnt work");
			}else {
				path = newargv[1];
				if((chdir(path)) == -1)
					perror("chdir didnt work");
				}
			}
			
//test for environ and handle if true
		if(strcmp(newargv[0],"environ")==0){
			if(a_nd ==1){
				printf("environ [%d] \n", getpid());
				}
			if(newargv[3] != '\0' || newargv[1] == '\0'){
				perror("wrong format for arguments");
				continue;
				}
			char* temp;
			if(newargv[2] == NULL){
				temp=getenv(newargv[1]);
				if(temp == NULL){
					temp = " ";
					if(setenv(newargv[1],temp,1) == -1){
						perror("setenv failed. wrong inputs");
						continue;
						}
					}
				printf("%s \n",temp);
				
				continue;
			}else{  	
				if(setenv(newargv[1],newargv[2],1) == -1)
					perror("setenv failed. wrong inputs");
				continue;
				
				}
			}
			
//clear out stdout and stderror	
		fflush(stdout);
		fflush(stderr);
		
//fork kid to exec input from user		
                if ((kid_pid=fork()) == -1) {			
			perror("fork failed");
			continue;
			
//kid
		} else if(kid_pid == 0) {
		
//check for back round programs and handle a backround child
			if(a_nd == 1){
	       			bgchild=open("/dev/null",O_RDONLY);
	       			if(dup2(bgchild,STDIN_FILENO) == -1){
					perror("dup2 failed in the redirection the back round child std to dev/null");
					_exit(-1);
					}
				}
				
//out put redirecting
			if(g_than == 1 && g_pipe == 0){
				flags =O_CREAT|O_EXCL|O_WRONLY;
				if((g_redirect = open(file_gthan,flags,premission)) < 0){
					perror("file alrady exsists");
					_exit(-1);
					}
				if(dup2(g_redirect,STDOUT_FILENO) ==-1){
					perror("dup2 failed gthan");
					_exit(-1);
					}	
				}
				
//input redirecting
			if(l_than == 1){
				flags =O_RDONLY;
				if((l_redirect = open(file_lthan,flags,premission)) <0){
					perror("file dosent exsit");
					_exit(-1);
					}
				if(dup2(l_redirect,STDIN_FILENO) == -1){
					perror("dup2 failed lthan");
					_exit(-1);
					}
				}
				
//calls pipe. recursive pipe function
			if(p_count > 0){
				if(pip_e(p_count,0,STDIN_FILENO) ==-1){
					perror("pip_e returned -1 somthing happend");
					exit(-1);
					}
				_exit(0);
				}
			
                       if(execvp(newargv[0],newargv)== -1) {
		       		perror("execvp failed in child");
				_exit(-1);
                       }
		       _exit(0);
		 
//parent      
                }else if(kid_pid > 0) {
		
//prints the pid of the backround process
			if(a_nd ==1){
				printf("%s [%d] \n",newargv[0],kid_pid);
				fflush(stdin);
				fflush(stdout);
				continue;
				}

//wait for child to die before breaking the loop
			for(;;){
				pid_t pid_child;
				pid_child = wait(NULL);
				if(pid_child == kid_pid)
					break;
				}
			if(end == 1)
				break;
			}
			
		}
	   
	   
           killpg(getpid(), SIGTERM); // Terminate any children that are
                                      // still running. WARNING: giving bad args
                                      // to killpg() can kill the autograder
           printf("p2 terminated.\n");// MAKE SURE this printf comes AFTER killpg
           exit(0);
	   return 0;
     }


