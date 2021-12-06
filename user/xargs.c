#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

/*

Xargs: takes output of a command and pass it as argument of another command.

1. Use fork and exec to invoke he command on each line of input. Use wait in the parent to wait for the child to complete the command.

2. To read individual lines of input, read a character at a time unil a newline appears.

3. kernel/param.h
#define MAXARG 32 // max exec arguments
#define MATHPATH 128 // max file path name

4. Does Xarg argument order or priority exist?
5. The diff. bet. argnum's init val?
6. Why does this work?
7. What's wrong w/ the 'grep' thing?

*/

int main(int argc, char* argv[]){
  char* args[MAXARG]; // args
  //int argnum = argc - 1; // # of args
  int argnum = 0;

  char temp; // temp read
  char buf[512]; // buffer
  int readstat = 1; //init flag

  for(int i = 1; i < argc; i++){
    //args[i] = argv[i]; // put args into the array
    args[argnum++] = argv[i];
  }
  while(readstat){
    int curr = 0;
    int argcurr = 0;
      
    while(1){
      readstat = read(0, &temp, sizeof(temp));

      if(!readstat)
        exit(0); // nth to read from 
      
      // not a blank or \n then put it in the buffer
      // cannot use strcmp? 
      if((temp !=  ' ') && (temp != '\n')){
          buf[curr++] = temp;
      }
    
      else{
        if(temp == ' '){
          buf[curr++] = 0;
          args[argnum++] = &buf[argcurr]; // char vs char*
          argcurr = curr;
        }
        if(temp == '\n'){
          args[argnum++] = &buf[argcurr];
          argcurr = curr;
          break;
        }
      }  
    } // end of while loop 2
  
    if(!fork()){
      exec(args[0], args);
    }

    else{
      wait(0);
    }
  } // end of while loop 1
  exit(0);
}
