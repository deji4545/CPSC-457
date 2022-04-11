//Name - Ayodeji Osho
//Class - CPSC 457 T09
//Student ID -30071771

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <stdio.h>
#include <chrono>  
#include <thread>

#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

#include "safecall.h"

// ====================================================================
// this is the function you need to reimplement
// ====================================================================
//
// safecall(n) calls unsafe(n) and returns values as follows.
//
// if the call to unsafe() crashes, safecall() will return -2
//
// if unsafe() does not return within 1s, safecall() will return -1
//   please note that safecall() must always return within 1s
//
// if the call to unsafe() returns within 1s, safecall() will return
// the value that unsafe() returned
//
int safecall(int i)
{
  unlink("/tmp/ayo-safecall.txt");
  int nap_time = 1;

  pid_t pid;
  auto start_time = std::chrono::steady_clock::now();
  pid = fork();

  if (pid == 0)
  {
    int result = unsafe(i);
    FILE *fp = fopen("/tmp/ayo-safecall.txt", "w");
    fprintf(fp, "%d", result);
    fclose(fp);
    exit(0);
  }
  else
  {
    while (1)
    {
      std::this_thread::sleep_for(std::chrono::microseconds(nap_time));
      auto pid_result = waitpid(pid, NULL, WNOHANG);

      if (pid_result >= 1)
      {

        int res;
        FILE *fr = fopen("/tmp/ayo-safecall.txt", "r");

        if (fr == NULL)
        {
          return -2;
        }

        fscanf(fr, "%d", &res);
        fclose(fr);
        return res;
      }

      auto curr_time = std::chrono::steady_clock::now();
      double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(curr_time - start_time).count() / 1000000.0;
      if( elapsed > 1.0) { 
        kill( pid, SIGKILL);
        return -1;
      }
    }
  }

  // My recommendation:
  //
  // remove temporary file
  // pid = fork()
  // in child process:
  //   call result=unsafe(i)
  //   write result to temporary file
  //   exit
  // in parent process:
  //   remember current time
  //   loop:
  //     sleep for 0.000001s
  //     waitpid(pid, NULL, WNOHANG)
  //     if waitpid successful
  //       read results from file
  //       if reading results was successful, return result
  //       othersise return -2
  //     measure elapsed time since loop started
  //     if elapsed time > 1s
  //       kill child
  //       return -1
  //

  // Alternative implementation:
  //
  // Very similar to the above, but instead of using a busy loop with
  // sleep, we can spawn an additional child process. The 2nd child
  // will simply sleep for 1s and then exit.
  //
  // In the parent, we can call wait(NULL) to see which child finished
  // first. if the child that calls unsafe() finished first, then we
  // know it finished under 1s. We can kill the 2nd child and collect
  // results...
  //
  // If the 2nd child finished first, we know that the unsafe() is still
  // running even after 1s. We can kill the 1st child and return -1.
  //
}
