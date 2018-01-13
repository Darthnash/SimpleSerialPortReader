#include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <sys/signal.h>
    #include <errno.h>
    #include <termios.h>

    void signal_handler_IO (int status);   /* definition of signal handler */

    int n;
    int fd;
    int connected;
    struct termios termAttr;
    struct sigaction saio;

    int main(int argc, char *argv[])
    {
         fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
         if (fd == -1)
         {
            perror("open_port: Unable to open /dev/ttyO1\n");
            exit(1);
         }

         saio.sa_handler = signal_handler_IO;
         saio.sa_flags = 0;
         saio.sa_restorer = NULL;
         sigaction(SIGIO,&saio,NULL);

         fcntl(fd, F_SETFL, FNDELAY);
         fcntl(fd, F_SETOWN, getpid());

         tcgetattr(fd,&termAttr);
         // baudRate = B115200;
         cfsetispeed(&termAttr,B19200);
         cfsetospeed(&termAttr,B19200);
         termAttr.c_cflag &= ~PARENB;
         termAttr.c_cflag &= ~CSTOPB;
         termAttr.c_cflag &= ~CSIZE;
         termAttr.c_cflag |= CS8;
         termAttr.c_cflag |= (CLOCAL | CREAD);
         termAttr.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
         termAttr.c_iflag &= ~(IXON | IXOFF | IXANY);
         termAttr.c_oflag &= ~OPOST;
         tcsetattr(fd,TCSANOW,&termAttr);
         printf("UART1 configured....\n");

         connected = 1;
         while(connected == 1){
               // some code
         }

         close(fd);
         exit(0);
    }

    void signal_handler_IO (int status)
    {
         printf("received data from UART.\n");
    }
