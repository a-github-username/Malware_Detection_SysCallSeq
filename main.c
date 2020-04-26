#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    // Process ID for the child process
    int syscall;
    pid_t pid;
    pid = fork();
    // Setting the shared file
    FILE* theFile;
    theFile = fopen("data.txt","w");

    if (theFile == NULL) {
        printf("Error! Could not open file\n");
        exit(-1);
    }
    fprintf(theFile, "%d", 0);
    fclose(theFile);
    // If there was a fork error, then exit
    if (pid < 0) {
        fprintf(stderr, "Fork Failed");
        exit(-1);
    } else if (pid == 0){
      ptrace(PTRACE_TRACEME, 0, NULL, NULL);
      printf("The child process with PID: %d is created and is set to be traced \n", (int) getpid());
	    char *args[] = {argv[2], "C", "Programming", NULL};
	    execv(argv[1], args);

    }
    // Waiting for the child process to run
    waitpid(pid, 0, 0);
    ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);
    // The while loop runs until the child file is finished
    while (1) {
        theFile = fopen("data.txt","w");

        // This loop is to extract the system calls and write them to
        // the shared file
        for(int i = 0; i < 5; i ++){
            // a single step into the system call
            if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1){}
            if (waitpid(pid, 0, 0) == -1){}
            syscall = ptrace(PTRACE_PEEKUSER, pid, sizeof(long)*ORIG_EAX);
            if(syscall != -1) {
                    fprintf(theFile,"%d",syscall);
                    fprintf(theFile," ");
            } else {
                break;
            }
                    // Exiting the child system call
            if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1){}
            if (waitpid(pid, 0, 0) == -1){}
        }
        fclose(theFile);

        int checkNum = 2;
        int checker = 0;
        do{
            theFile = fopen("data.txt","r");
            if (theFile == NULL) {
                printf("Error! Could not open file \n");
                exit(-1);
            }
            fscanf(theFile, "%d", &checkNum);
            fclose(theFile);
            if(checkNum == 0 || checkNum == 1){
                checker = 1;
            }
        } while(checker != 1);

        // Prints the result of detection
        if (checkNum == 1){
            printf("Malware detected.\n");
            kill(pid, SIGTERM);
            break;
        } else {
            printf("Malware not detected.\n");
        }
	    if(syscall == -1) {
	        exit(-1);
	    }
    }

return 0;
}



