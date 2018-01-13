/*
 *	Date:	21 Dec 2017
 *	Author:	Lukas Hägele <lukas.haegele93@web.de>
 *
 *	Description: Simple Application that aims to print out Serial Port Communication to the terminal
 *
 */

#include <stdlib.h>		// standard library
#include <fcntl.h>		// file control definitions
#include <errno.h>		// error numer definitions
#include <stdio.h>		// standard input/output functions
#include <string.h>		// string function definitions
#include <termios.h>	// POSIX terminal control definitions
#include <unistd.h> 	// UNIX standard function definitions
#include <signal.h>		// UNIX signal processing

#define FALSE 0
#define TRUE 1

void signal_handler_IO (int status);  // definition of SIGIO signal handler
void signal_int (int status);  // definition of SIGINT signal handler

static volatile int STOP = FALSE;
static int wait_flag = TRUE;  // TRUE while no signal received
static int fd;  // file descriptor of serial port

int main ()
{
	/* open port */
// const char *portname = "/dev/ttyACM0";  // COM Port of connected device
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

	/* error handling */
	if(tcgetattr(fd, &tty) != 0)  // tcgetattr: get parameters that are associated with the terminal and store them in termios structure
	{
		fprintf(stderr, "error %d from tcgetattr", errno);  // display error code
		exit(-1); // terminate program
	}
	else
	{
		printf("tcgetattr() succeeded.\n");
	}

	/* configure terminal */
	cfsetispeed(&tty, B19200);  // set input speed to Baudrate 19,200

	// basic terminal input control
	tty.c_iflag = IGNPAR | ICRNL;

	// basic terminal output control
	tty.c_oflag = 0;

	// hardware control of the terminal
	tty.c_cflag |= (CS8 | CREAD | CLOCAL);  // force 8bit input, enable receiver, ignore modem conrol lines

	// control various terminal functions
	tty.c_lflag = ICANON;

	// control characters
	tty.c_cc[VEOF]	= 4;  // Ctrl-d
	tty.c_cc[VMIN]	= 1;  // read() is satisfied with # character byte(s)
	tty.c_cc[VTIME] = 0;  // inter-character timer off

	// treat unused data
	tcflush(fd, TCIFLUSH);  // discards unread data

	/* error handling */
	if(tcsetattr(fd, TCSANOW, &tty) != 0)  // change will occur immediately
	{
		fprintf(stderr, "error %d from tcsetattr", errno);  // display error code
		exit(-1);  // terminate program
	}
	else
	{
		printf("tcsetattr() succeeded\n");
	}

	/* signal handler */
	struct sigaction saio;  // definition of signal signal action
	saio.sa_handler = signal_handler_IO;
	//sigemptyset(&saio.sa_mask);
	//sigaddset(&saio.sa_mask, SIGINT);  // what is this doing??
	//saio.sa_flags = 0;
	//saio.sa_restorer = NULL;
	if(sigaction(SIGIO, &saio, NULL) < 0)  // what is this doing??
	{
		fprintf(stderr, "%s: sigaction() failed.\n", portname);
	}
	else
	{
		printf("sigaction() succeeded.\n");
	}

	struct sigaction act_int;
	act_int.sa_handler = signal_int;
	sigaction(SIGINT, &act_int, NULL);

	fcntl(fd, F_SETOWN, getpid());  // allow process to receive SIGIO
	fcntl(fd, F_SETFL, O_ASYNC);  // make file descriptor asyncronous

	/* read character byte */
	unsigned char buf[255];  // input buffer

	int n = 0;  // number of bytes that were read

	while(STOP == FALSE)  // error handling to cancel loop?
	{
		printf(".\n");
		sleep(1);  // sleep for 1 second

		if(wait_flag == FALSE)
		{
			n = read(fd, buf, sizeof(buf));  // read from input buffer
			buf[n] = 0;  // set end of string so we can use printf
			printf(":%s:%d\n", buf, n);

			if (n == 1)
			{
				STOP = TRUE;  // stop loop if only a CR was input
				wait_flag = TRUE;  // wait for new input
			}
		}
	}

	return 0;
}


/* signal handler */
void signal_handler_IO (int status)
{
	printf("received SIGIO signal.\n");
	wait_flag = FALSE;  // indicate reception of characters
}

void signal_int (int signum)
{
	printf("\nclosing file descriptor.\n");
	close(fd);
	signal(SIGINT, SIG_DFL);
	kill(getpid(), SIGINT);
}