#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>


//Skeleton for program comes from...
//Source:   https://github.com/Wal33D/Processes-N-Pipes/blob/master/processMessagePassing.c
#define P1_READ     0
#define P2_WRITE    1
#define P2_READ     2
#define P1_WRITE    3

// the total number of pipe *pairs* we need
#define NUM_PIPES   2

/*
Main takes input from command line, calls input validation to make sure of proper input,
then creates the pipes we will need and the forks the child process, Parent and Child
execute they're respective code
*/
int main(int argc, char *argv[]) {

    assert(argc > 1);

    int fd[2 * NUM_PIPES];    //Declare int[] of file descriptors

    int len = 0;
    int i;             //Declare length and integer for count

    pid_t pid;              //Declare process id

    char parent[strlen(argv[1])];   //Declare Parent array

    char child[strlen(argv[1])];    //Declare Child array

    //if(inputValidation(argc, argv) == 0) /* Check for proper input */
    //TODO: Create validation fuction

    strcpy(parent, argv[1]);

    // create all the descriptor pairs we need
    for (i = 0; i < NUM_PIPES; ++i) {
        if (pipe(fd + (i * 2)) < 0) {
            perror("Failed to allocate pipes");
            exit(EXIT_FAILURE);
        }
    }

    // fork() returns 0 for child process, child-pid for parent process.
    if ((pid = fork()) < 0) {
        perror("Failed to fork process");
        return EXIT_FAILURE;
    }
    //////////////////////////////Childs Code BEGINS//////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    // if the pid is zero, this is the child process
    if (pid == 0) {
        // Child. Start by closing descriptors we
        //  don't need in this process
        close(fd[P1_READ]);
        close(fd[P1_WRITE]);

        // used for output
        pid = getpid();

        // wait for parent to send us a value
        len = (int) read(fd[P2_READ], &child, (size_t) len);
        if (len < 0) {
            perror("Child: Failed to read data from pipe");
            exit(EXIT_FAILURE);
        } else if (len == 0) {
            // not an error, but certainly unexpected
            fprintf(stderr, "Child: Read EOF from pipe");
        } else {

            // report pid to console
            printf("Child(%d): Recieved Message\n\nChild(%d): Toggling Case and Sending to Parent\n", pid, pid);

            // send the message to toggleString and write it to pipe//
            //if (write(fd[P2_WRITE], toggleString(child), strlen(child)) < 0)
            //TODO: Create function for sharing of integer array

            {
                perror("Child: Failed to write response value");
                exit(EXIT_FAILURE);

            }

        }

        // finished. close remaining descriptors.
        close(fd[P2_READ]);
        close(fd[P2_WRITE]);

        return EXIT_SUCCESS;
    }
    //////////////////////////////Childs Code ENDS////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////Parent Code BEGINS//////////////////////////////////

    // Parent. close unneeded descriptors
    close(fd[P2_READ]);
    close(fd[P2_WRITE]);

    // used for output
    pid = getpid();

    // send a value to the child

    printf("\nParent(%d): Sending %s to Child\n\n", pid, argv[1]);
    if (write(fd[P1_WRITE], argv[1], strlen(argv[1])) != strlen(argv[1])) {
        perror("Parent: Failed to send value to child ");
        exit(EXIT_FAILURE);
    }

    // now wait for a response
    len = (int) read(fd[P1_READ], &parent, strlen(parent));
    if (len < 0) {
        perror("Parent: failed to read value from pipe");
        exit(EXIT_FAILURE);
    } else if (len == 0) {
        // not an error, but certainly unexpected
        fprintf(stderr, "Parent(%d): Read EOF from pipe", pid);
    } else {
        // report what we received
        printf("\nParent(%d): Received %s from Child\n\n", pid, parent);
    }

    // close down remaining descriptors
    close(fd[P1_READ]);
    close(fd[P1_WRITE]);

    // wait for child termination
    wait(NULL);

    return EXIT_SUCCESS;

}
//////////////////////////////Parent Code ENDS//////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////