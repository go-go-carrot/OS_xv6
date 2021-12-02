#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]){
  
  //file descriptor
  int parent[2];
  int child[2];

  char buffer[8] = ""; // usage?
  char parentping[8] = "ping";
  char childpong[8] = "pong";

  // base case
  if(pipe(parent) < 0 || pipe(child) < 0){
    fprintf(2, "Error: Pipe return value is less than zero");
    exit(1); // general error
  }
  
  int fork_val= fork();
  int pid = getpid(); // process ID
  
  if(fork_val < 0){
    fprintf(2, "Error: Fail to fork");
    exit(1);
  }

  // child
  if(fork_val == 0){
    close(parent[1]); // close parent write
    close(child[0]); // close child read

    read(parent[0], buffer, 1); // read 1 byte from the buffer
    printf("%d: received %s\n", pid, parentping);
    write(child[1], buffer, 1); // write 1 byte

    close(parent[0]); // close parent read
    close(child[1]); // close child write

    exit(0);
  }

  // parent
  else{
    close(parent[0]); // close parent read
    close(child[1]); // close child write

    write(parent[1], buffer, 1); // write 1 byte
    read(child[0], buffer, 1); // read 1 byte
    printf("%d: received %s\n", pid, childpong);

    close(parent[1]); // close parent write
    close(child[0]); // close child write

    exit(0);
  }
}
