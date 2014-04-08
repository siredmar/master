SRC = helpers.c main.c rs232/rs232.c
all:
	gtk-builder-convert glade/main.glade glade/main.xml
	gcc -Wall -g $(SRC) -o edid_writer `pkg-config --cflags gtk+-2.0 pkg-config --libs gtk+-2.0 gmodule-2.0` `pkg-config --libs gthread-2.0` -lX11
