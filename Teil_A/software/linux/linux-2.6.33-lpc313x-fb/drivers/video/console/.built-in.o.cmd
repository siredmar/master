cmd_drivers/video/console/built-in.o :=  arm-linux-gnueabi-ld -EL    -r -o drivers/video/console/built-in.o drivers/video/console/dummycon.o drivers/video/console/fbcon.o drivers/video/console/bitblit.o drivers/video/console/font.o drivers/video/console/softcursor.o drivers/video/console/fbcon_rotate.o drivers/video/console/fbcon_cw.o drivers/video/console/fbcon_ud.o drivers/video/console/fbcon_ccw.o 
