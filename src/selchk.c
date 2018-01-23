/*
 *	Date: 17 Jan 2018
 *	Author:	Lukas Hägele <lukas.haegele93@web.de>
 *
 *	Description: Simple Application to try out select()
 *
 */

 #include <stdlib.h>  // standard library
 #include <fcntl.h>   // file control definitions
 #include <errno.h>   // error numer definitions
 #include <stdio.h>   // standard input/output functions
 #include <string.h>  // string function definitions
 #include <termios.h>	// POSIX terminal control definitions
 #include <unistd.h> 	// UNIX standard function definitions
 #include <signal.h>		// UNIX signal processing


 #define FALSE 0
 #define TRUE 1

 void signal_int (int status);  // definition of SIGINT signal handler

 static volatile int STOP = FALSE;
 static int fd;  // file descriptor of serial port

 int main ()
 {
 	/* open port */
 	const char *portname = "/dev/ttyUSB0";
 	fd = open(portname, O_RDONLY | O_NOCTTY | O_NONBLOCK);  // open Port for reading and catch return value, returns descriptor of new file

 	/* error handling */
 	if (fd < 0)
 	{
 		fprintf(stderr, "error %d opening %s: %s \n", errno, portname, strerror(errno));  // display error code
 		exit(-1);  // terminate program with failure value != 0
 	}
 	else
 	{
 		printf("%s: opened.\n", portname);
 	}

 	/* terminal control */
 	struct termios tty;  // declare teletypewriter/terminal variable
 	memset(&tty, 0, sizeof(tty));  // fill memory space of tty with 0 (reset values)

 	/* configure terminal */
 	cfsetispeed(&tty, B19200);  // set input speed to Baudrate 19,200

 	// basic terminal input control
 	tty.c_iflag = IGNPAR;

 	/* error handling */
 	if (tcsetattr(fd, TCSANOW, &tty) != 0)  // change will occur immediately
 	{
 		fprintf(stderr, "error %d from tcsetattr", errno);  // display error code
 		exit(-1);  // terminate program
 	}
 	else
 	{
 		printf("tcsetattr() succeeded\n");
 	}

 	/* signal handler */
 	struct sigaction act_int;
 	act_int.sa_handler = signal_int;
 	sigaction(SIGINT, &act_int, NULL);

  /* init select() */
  fd_set rfds;  // define read file descriptor set
  struct timeval tv;  // time structure, used for timeout of select()
  int retval;  // return value
  FD_ZERO(&rfds);  // empty rfds
  FD_SET(fd, &rfds);  // include fd in rfds
  tv.tv_sec = 5;  // wait up to 5.0 sec, tv could be passed as NULL
  tv.tv_usec = 0;

 	/* read character byte */
 	unsigned char buf[255];  // input buffer

 	int n = 0;  // number of bytes that were read

 	// treat unused data
 	tcflush(fd, TCIFLUSH);  // discards unread data

 	while (STOP == FALSE)  // error handling to cancel loop?
 	{
    retval = select(fd+1, &rfds, NULL, NULL, NULL);

    if (retval == -1)
    {
      fprintf(stderr, "error %d from select", errno);  // display error code
   		exit(-1);  // terminate program
    }
    else if (retval)
    {
      n = read(fd, buf, sizeof(buf));  // read from input buffer
 			if(n < 0)
 			{
 				printf("read() failed.\n");
 				exit(-1);  // terminate program
 			}
 			buf[n] = 0;  // set end of string so we can use printf
 			printf("%s", buf);
 			fflush(stdout);
    }
 	}

 	return 0;
 }

 /* signal handler */
 void signal_int (int signum)
 {
 	printf("\nclosing file descriptor.\n");
 	close(fd);
 	signal(SIGINT, SIG_DFL);
 	kill(getpid(), SIGINT);
 }
