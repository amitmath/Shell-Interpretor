#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include<sys/wait.h>
#define MAX 100 /*Max no of arguments */
int n=0; /* Stores the number of commands to be executed */
static int count=0;
char* args[MAX];
void shell_loop();
void parse_cmd(char * cmd);
char* skipWS(char *s);
void func_cd(char *path);
void make_shell_wait();
void welcome();
int run(char *cmd,int pipe1,int pipe2,int pipe3);
int exec(int pipe1,int pipe2,int pipe3);
int main()
{
printf("Type \'exit\' to exit\n");
shell_loop();
return 0;
}
void shell_loop()
{
char *cmd;
size_t cmd_size = 32;
cmd = (char *)malloc(cmd_size * sizeof(char));
welcome();
while(1){
printf("$:");
getline(&cmd,&cmd_size,stdin); /*gets the command from user*/
fflush(stdin);
pharse_cmd(cmd);
make_shell_wait(n);
n=0;
}
return;
}
void parse_cmd(char * cmd)
{
/*Variables to count no of pipes */
int pipe1=0,pipe2=1;
cmd=skipWS(cmd);
char *pipe_handler=strchr(cmd,'|'); //Finds pipes in the command
while(pipe_handler!=NULL)
{
*pipe_handler='\0';
pipe1=run(cmd,pipe1,pipe2,0);
cmd=pipe_handler+1;
pipe_handler=strchr(cmd,'|');
pipe2=0;
}
pipe1=run(cmd,pipe1,pipe2,1);
make_shell_wait(n);
n=0;
}
int run(char *cmd,int pipe1,int pipe2,int pipe3)
{
int i=0;
cmd=skipWS(cmd);
char *pipe_handler=strchr(cmd,' ');
while(pipe_handler!=NULL) /* If options then */
{
*pipe_handler='\0';
args[i]=cmd;
i++;
cmd=skipWS(pipe_handler+1);
pipe_handler=strchr(cmd,' ');
}
if(cmd[0]!='\0') /* Code to handle if no options are present */
{
args[i]=cmd;
pipe_handler=strchr(cmd,'\n');
pipe_handler[0]='\0';
i++;
}
args[i]=NULL;
if(args[0]!=NULL)
{
if (strcmp(args[0], "exit")==0)
exit(0);
else if(strcmp(args[0], "cd")==0)
{
func_cd(args[1]);
return 0;
}
else
n++;
return exec(pipe1,pipe2,pipe3);
}
return 0;
}
int exec(int pipe1,int pipe2,int pipe3)
{
pid_t pid;
int pipefd[2];
pipe(pipefd);
pid=fork();
if(pid==0)
{
if (pipe1 == 0 && pipe2 == 1 && pipe3 == 0) {
dup2( pipefd[1], STDOUT_FILENO );
}
else if (pipe1 != 0 && pipe2 == 0 && pipe3 == 0) {
dup2(pipe1, STDIN_FILENO);
dup2(pipefd[1], STDOUT_FILENO);
} else {
dup2( pipe1, STDIN_FILENO);
}
if (execvp( args[0], args) == -1)
printf("Could not run command!\n");
}
if(pipe1!=0)
close(pipe1);
close(pipefd[1]);
if(pipe3==1)
close(pipefd[0]);
return pipefd[0];
}
char* skipWS(char *s)
{
while(isspace(*s)){
if(isspace(*s))
++s;
}
return s;
}
void make_shell_wait()
{
int i;
for(i=0;i<n;i++)
{
wait(NULL);
}
}
void func_cd(char *path)
{
chdir(path);
}
void welcome()
{
printf(" - --+-+ +-++-+ +-+--++-+ -+-++-++-+ \n");
printf(" ++++¦ ¦ ¦ ¦+-+ ¦-+¦-+¦ ¦ ¦¦¦ ¦ ¦ pha \n");
printf(" ++ - - +-++-+ - -+-+-++++-++-+ - \n");
}