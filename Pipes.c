#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>


//Skeleton for program comes from...
//Source:   https://github.com/Wal33D/Processes-N-Pipes/blob/master/processMessagePassing.c
#define P1_READ     0
#define P2_WRITE    1
#define P2_READ     2
#define P1_WRITE    3

// the total number of pipe *pairs* we need
#define NUM_PIPES   2

bool isNumber(char const number[]) {
    int i = 0;

    //checking for negative numbers
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++) {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}

void usage() {
    printf("usage:\t Pipes array_size datafile\n"
           "array_size:\t How much array will need to be filled up before sending via pipe\n"
           "datafile:\t name of file with numbers to be used\n");
}

/*
Main takes input from command line, calls input validation to make sure of proper input,
then creates the pipes we will need and the forks the child process, Parent and Child
execute they're respective code
*/
int main(int argc, char *argv[]) {

    assert(argc > 1);

    //Input Validation
    if (argc != 3) {
        usage();
        exit(EXIT_FAILURE);
    }

    if (!isNumber(argv[1])) {
        fprintf(stderr, "Non-integer value inputted\n");
        usage();
        exit(1);
    }
    //Input validation complete.

    int fd[2 * NUM_PIPES];    //Declare int[] of file descriptors

    int len = 0;
    int i;             //Declare length and integer for count

    pid_t pid;              //Declare process id

    //Converting argv[1] to integer value that program can use
    char *p;

    long conv = strtol(argv[1], &p, 10);
    int array_size = (int) conv;

    char parent[array_size];   //Declare Parent array
    printf("Parent array has %d elements", (int) strlen(parent));

    char child[array_size];    //Declare Child array

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
            fprintf(stderr, "Child: Read EOF from pipe\n\n");
        } else {

            // report pid to console
            printf("Child(%d): Received Message\n\nChild(%d): Sending to Parent\n", pid, pid);

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
        perror("Parent: failed to read value from pipe\n\n");
        exit(EXIT_FAILURE);
    } else if (len == 0) {
        // not an error, but certainly unexpected
        fprintf(stderr, "Parent(%d): Read EOF from pipe\n\n", pid);
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

#pragma clang diagnostic pop