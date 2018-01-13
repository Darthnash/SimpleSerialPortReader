/*
 *	Date:	9 Jan 2018
 *	Author:	Lukas Hägele <lukas.haegele93@web.de>
 *
 *	Description: Simple Application to get cozy with Unix Signal Handling
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int signum)
{
  // print out received signal
  printf("got signal %d\n", signum);
}

void close_gently(int signum)
{
	printf("shutting down...\n");
	exit(0);
}

void goto_sleep(int signum)
{
  // go to sleep
  printf("sleeping...\n" );
  pause();

  // wake up
  printf("waking up...\n");
}


int main()
{
  // counter variable
  unsigned char i = 0;

  // init SIGTERM signal Handling
  struct sigaction act_term;
  act_term.sa_handler = signal_handler;
  sigaction(SIGTERM, &act_term, NULL);

  // init SIGINT signal Handing
  struct sigaction act_int;
  act_int.sa_handler = close_gently;
  sigaction(SIGINT, &act_int, NULL);

  struct sigaction act_slp;
  act_slp.sa_handler = signal_handler;
  sigaction(SIGUSR1, &act_slp, NULL);

  // go to sleep
  while(1)
  {
    printf("i = %d\n", i);
    i++;
    sleep(1);
  }


  return 0;
}
