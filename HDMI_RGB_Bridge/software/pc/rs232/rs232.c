
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

extern int received_flag;
//
//char comports[30][16]={"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4","/dev/ttyS5",
//      "/dev/ttyS6","/dev/ttyS7","/dev/ttyS8","/dev/ttyS9","/dev/ttyS10","/dev/ttyS11",
//      "/dev/ttyS12","/dev/ttyS13","/dev/ttyS14","/dev/ttyS15","/dev/ttyUSB0",
//      "/dev/ttyUSB1","/dev/ttyUSB2","/dev/ttyUSB3","/dev/ttyUSB4","/dev/ttyUSB5",
//      "/dev/ttyAMA0","/dev/ttyAMA1","/dev/ttyACM0","/dev/ttyACM1",
//      "/dev/rfcomm0","/dev/rfcomm1","/dev/ircomm0","/dev/ircomm1"};

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
   default      : printf("invalid baudrate\n");
   return(0);
   break;
   }
   return baudr;
}

void rs232_receive_handler (int status);   /* definition of signal handler */

int rs232_open_port(char *comport, int baudrate)
{
   //int n;

   struct termios termAttr;
   struct sigaction saio;
   int baudRate;
   int fd;

   fd = open(comport, O_RDWR | O_NOCTTY | O_NDELAY);
   if (fd == -1)
   {
      perror("open_port: Unable to open comport\n");
      return 1;
   }

   saio.sa_handler = rs232_receive_handler;
   saio.sa_flags = 0;
   saio.sa_restorer = NULL;
   sigaction(SIGIO, &saio, NULL);

   fcntl(fd, F_SETFL, FNDELAY);
   fcntl(fd, F_SETOWN, getpid());
   fcntl(fd, F_SETFL,  O_ASYNC ); /**<<<<<<------This line made it work.**/

   tcgetattr(fd, &termAttr);
   baudRate = rs232_returnBaudrate(baudrate);
   //printf("baudrate: %i\n", baudRate);
   cfsetispeed(&termAttr, baudRate);
   cfsetospeed(&termAttr, baudRate);
   termAttr.c_cflag &= ~PARENB;
   termAttr.c_cflag &= ~CSTOPB;
   termAttr.c_cflag &= ~CSIZE;
   termAttr.c_cflag |= CS8;
   termAttr.c_cflag |= (CLOCAL | CREAD);
   termAttr.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
   termAttr.c_iflag &= ~(IXON | IXOFF | IXANY);
   termAttr.c_oflag &= ~OPOST;
   tcsetattr(fd,TCSANOW,&termAttr);

   printf("%s opened....\n", comport);

   return fd;
}

int rs232_sendByte(int comport, unsigned char byte)
{
   int n;

   n = write(comport, &byte, 1);
   if(n < 0)  return 1;

   return 0;
}

//int rs232_sendBuf(int comport, unsigned char *buf, int size)
//{
//   return(write(comport, buf, size));
//}

void rs232_puts(int comport, const char *text)
{
  while(*text != 0)  rs232_sendByte(comport, *(text++));
}

int rs232_close_port(int comport)
{
   printf("%s closed....\n", comport);
   close(comport);

   return 0;
}

void rs232_receive_handler(int status)
{
   printf("received data from UART.\n");
   rs232_data_received();
}


