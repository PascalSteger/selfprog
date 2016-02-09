#include "sys/types.h"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <iostream>

pid_t child_pid = -1 ; //Global

void kill_child(int sig)
{
    kill(child_pid,SIGKILL);
}

int main(int argc, char *argv[])
{
  signal(SIGALRM,(void (*)(int))kill_child);
  child_pid = fork();
  if (child_pid > 0) {
    /*PARENT*/
    alarm(1); // wait 1 second, then kill child
    /*
     * Do parent's tasks here.
     */
    std::cout << "started parent, waiting 30s" << std::endl;
    wait(NULL);
  }
  else if (child_pid == 0){
    /*CHILD*/
    /*
     * Do child's tasks here.
     */
    std::cout << ".started child, sleeping 5 seconds" << std::endl;
    sleep(6);
    std::cout << ".finished sleeping"<<std::endl;
  }
}
