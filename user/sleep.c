#include "kernel/types.h"
#include "user/user.h"
//#include "kernel/stat.h"
//#include "kernel/syscall.h"


int main(int argc, char* argv[]){
  int second; // sleep time
  
  if(argc == 1){
    fprintf(2, "Not enough arguments");
    exit(2);
  }

  second = atoi(argv[1]); // argv[0]=='sleep', argv[1]=='10'
  //parseint
  
  if(second > 0)
    sleep(second);
  else{ 
    fprintf(2, "Error: Invalid time interval");
    exit(2);
  }
  exit(1);
}
