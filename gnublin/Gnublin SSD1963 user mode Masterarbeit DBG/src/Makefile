
#0ELDK-5.0
SRC = src/dio/dio_lcfg.c src/dio/dio.c src/tft/tft.c src/tft/tft_lcfg.c src/main.c 

CC = arm-linux-gnueabi-gcc
#CC = arm-linux-gnueabihf-gcc
INCLDIR = /home/armin/dev/gnublin/gnublin-linux-3.x

#INCLDIR = /home/armin/dev/gnublin/gnublin-develop-kernel/linux-2.6.33-lpc313x
#INCLDIR = /home/armin/dev/gnublin/gnublin-develop-kernel/linux-3.3.0-lpc313x

CCOPT = -Wall -g
TARGET = gnublin_ssd1963

gnublin_ssd1963: clean
	$(CC) $(CCOPT) -I$(INCLDIR) -o $(TARGET) $(SRC) -O3

clean:
		rm -f $(TARGET) *~

copy:
	scp gnublin_ssd1963 root@gnublin:/root
