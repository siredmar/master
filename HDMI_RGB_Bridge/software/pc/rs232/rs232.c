
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

#include "rs232.h"
#include "../helpers.h"

//extern int received_flag;
extern void rs232_data_received();
//extern int waitForInterrupt;
extern volatile int new_data;
//extern unsigned char checksum;
//extern int checksum_valid;
extern int comport_fd;

struct termios termAttr, oltAttr;

int rs232_returnBaudrate(int baudrate)
{
   int baudr;

   switch(baudrate)
   {
   case      50 : baudr = B50;
   break;
   case      75 : baudr = B75;
   break;
   case     110 : baudr = B110;
   break;
   case     134 : baudr = B134;
   break;
   case     150 : baudr = B150;
   break;
   case     200 : baudr = B200;
   break;
   case     300 : baudr = B300;
   break;
   case     600 : baudr = B600;
   break;
   case    1200 : baudr = B1200;
   break;
   case    1800 : baudr = B1800;
   break;
   case    2400 : baudr = B2400;
   break;
   case    4800 : baudr = B4800;
   break;
   case    9600 : baudr = B9600;
   break;
   case   19200 : baudr = B19200;
   break;
   case   38400 : baudr = B38400;
   break;
   case   57600 : baudr = B57600;
   break;
   case  115200 : baudr = B115200;
   break;
   case  230400 : baudr = B230400;
   break;
   case  460800 : baudr = B460800;
   break;
   case  500000 : baudr = B500000;
   break;
   case  576000 : baudr = B576000;
   break;
   case  921600 : baudr = B921600;
   break;
   case 1000000 : baudr = B1000000;
   break;
   default      : debugOutput("invalid baudrate\n");
   return(0);
   break;
   }
   return baudr;
}

void signal_handler_IO (int status);

int rs232_open_port(const char *comport, int baudrate)
{
   struct sigaction saio;
   int baudRate;
   int fd;

   fd = open(comport, O_RDWR | O_NOCTTY | O_NONBLOCK | O_SYNC);
   if (fd == -1)
   {
      debugOutput("open_port: Unable to open comport\n");
      return 1;
   }

   saio.sa_handler = signal_handler_IO;
   saio.sa_flags = 0;
   saio.sa_restorer = NULL;
   sigaction(SIGIO, &saio, NULL);

   fcntl(fd, F_SETFL, FNDELAY);
   fcntl(fd, F_SETOWN, getpid());
   fcntl(fd, F_SETFL,  O_ASYNC );

   tcgetattr(fd, &oltAttr);

   tcgetattr(fd, &termAttr);
   baudRate = rs232_returnBaudrate(baudrate);
   debugOutput("baudrate: %i\n", baudRate);
   cfsetispeed(&termAttr, baudRate);
   cfsetospeed(&termAttr, baudRate);
   //termAttr.c_cc[VTIME]    = 1;     /* inter-character timer unused */
   //termAttr.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
   termAttr.c_cflag &= ~PARENB;
   termAttr.c_cflag &= ~CSTOPB;
   termAttr.c_cflag &= ~CSIZE;
   termAttr.c_cflag |= CS8;
   termAttr.c_cflag |= (CLOCAL | CREAD);
   termAttr.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
   termAttr.c_iflag &= ~(IXON | IXOFF | IXANY);
   termAttr.c_oflag &= ~OPOST;
   tcsetattr(fd,TCSANOW,&termAttr);

   debugOutput("%s opened....\n", comport);

   return fd;
}

void rs232_puts(int comport, const char *text, int length)
{
   write(comport, text, length);
}

int rs232_close_port(int comport)
{
   tcsetattr(comport, TCSANOW, &oltAttr);
   debugOutput("%d closed....\n", comport_fd);
   close(comport);
   return 0;
}

void signal_handler_IO(int status)
{
   debugOutput("signal_handler_IO called, new_data: %d\n", new_data);
   new_data++;

}


