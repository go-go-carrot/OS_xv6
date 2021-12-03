#include "kernel/types.h"
#include "user/user.h"
/*
p = get a number from left neighbor
print p
loop:
  n = get a # from left neighbor
  if (n%p)
    send n to right neighbor;
*/


// for redirection
// fd == 0: dup read-end
// fd == 1: dup write-end
void redir(int fd, int p[]){
  close(fd);
  dup(p[fd]);
  close(p[0]); // close pipe RW
  close(p[1]);
}

void primes(){
  int fd[2];
  int n;
  int prime = 0;
  int pid = getpid();

  // base case
  if(read(0, &prime, sizeof(int)) <= 0){ // value less than zero
    fprintf(2, "Error: Invalid input");
    exit(1);
  }

  if(pipe(fd) < 0){
    fprintf(2, "Error: Unable to create a pipe!");
    exit(1);
  }
  
  printf("prime %d\n", prime);
    
  // child process
  if(!fork()){
    redir(0, fd); // duplicate read-end
    primes(); // then refine
  }
  else{
    redir(1, fd); // duplicate write-end
    while(read(0, &n, sizeof(int))){ // while reading
      if(n % prime){ // if prime does not divide n then write
        write(1, &n, sizeof(int)); 
      }
    }
    close(1);
    wait(&pid); // wait until the child finishes
  }
 //printf("prime %d\n", prime);
}

int main(int argc, char* argv[]){
  int fd[2];
  int pid = getpid();
  
  if(pipe(fd) < 0){
    fprintf(2, "Error: Unable to create a pipe!");
    exit(1);
  }
  
  if(!fork()){
    redir(0, fd);
    primes();
  }

  else{
    redir(1, fd);
    for(int i = 2; i < 36; i++){
      write(1, &i, sizeof(int));
    }
    close(1);
    wait(&pid);
  }
  exit(0);
}

/*
double sqrt(double n){
  double root = n/3;
  if(n <= 0) return 0;
  for(int i = 0; i < 32; i++)
    root = (root + n / root)/2;
  return root;
}

// writes natural#
int naturalnum(){
  int d[2]; // descriptor
  
  // base case
  if(pipe(d) < 0){
    fprintf(2, "Error: Unable to create a pipe!\n");
    exit(1); // general error
  }
 
  pipe(d);

  // parent feeds 2 thru 35
  if(!fork()){
    for(int i = 2; i < 36; i++){ // 1 not needed
      write(d[1], &i, sizeof(int));
      printf("This is natural num: %d\n", i);
    }
    close(d[1]); // close write
    exit(0);
  }

  close(d[1]);
  return d[0];
}

// read natrual# and writes prime# 
int primes(int fd_in){
  int input;
  int d[2]; // descriptor
 
  // base case
  if(pipe(d) < 0){
    fprintf(2, "Error: Unable to create a pipe!\n");
    exit(1); // general error
  }
  
  pipe(d);

  // child executes primes(), read from L, write to R
  // child process
  int temp = 34;
  int divisor = 0;

  if (!fork()){
    while(temp){
      read(d[0], &input, sizeof(int)); // read input
      for(int i = 2; i < 35; i++){
        for(int j = 2; j <= sqrt(i); j++){
          if(input % j == 0){
            divisor = 1; // divisor exists
            break;
          }
      }
      if(!divisor){ // if input is a prime#
        printf("This is primenum: %d\n", input);
        write(d[1], &input, sizeof(int));
      }// write prime#
    }
    temp--;
    close(fd_in);
    close(d[1]);
    exit(0);
    }
  }
  close(fd_in);
  close(d[1]);
  return d[0];
}

int main(int argc, char* argv[]){
  int fd_in = naturalnum();
  int prime = primes(fd_in);

  while(read(fd_in, &prime, sizeof(int))){
      printf("This is fd_in: %d\n", fd_in);
      printf("prime %d\n", prime);
      //fd_in = prime(fd_in);
  }
  exit(0);
}
*/
