/*
 *	Date:	12 Jan 2018
 *	Author:	Lukas Hägele <lukas.haegele93@web.de>
 *
 *	Description: Simple Application to read Serial Port by polling
 *
 */

 #include <stdlib.h>		// standard library
 #include <fcntl.h>		// file control definitions
 #include <errno.h>		// error numer definitions
 #include <stdio.h>		// standard input/output functions
 #include <string.h>		// string function definitions
 #include <termios.h>	// POSIX terminal control definitions
 #include <unistd.h> 	// UNIX standard function definitions

 int main ()
 {
   const char *portname = "/dev/ttyUSB0";
   int fd = open(portname, O_RDONLY | O_NOCTTY | O_NONBLOCK);
   if (fd < 0)
 	 {
     fprintf(stderr, "error %d opening %s: %s \n", errno, portname, strerror(errno));  // display error code
 		 exit(-1);  // terminate program with failure value != 0
   }

   struct termios tty;
   memset(&tty, 0, sizeof(tty));
   //tcgetattr(fd, &tty);

   cfsetispeed(&tty, B19200);

   // tty.c_iflag = 0;
   // tty.c_oflag = 0;
   // tty.c_cflag = CS8 | CREAD;
   // tty.c_lflag = 0;
   // tty.c_cc[VMIN] = 1;
   // tty.c_cc[VTIME] = 5;

   tcflush(fd, TCIFLUSH);
   tcsetattr(fd, TCSANOW, &tty);

   unsigned char buf[255];
   int n = 0;

   while(1)
   {
     n = read(fd, buf, sizeof(buf));  // is buffer being cleared here?
     if(n>0)
     {
       buf[n] = 0;
       printf("%s", buf);
       fflush(stdout);
     }
   }

   return 0;
 }
