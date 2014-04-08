/*
 * helpers.h
 *
 *  Created on: Mar 31, 2014
 *      Author: armin
 */

#ifndef HELPERS_H_
#define HELPERS_H_
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/sem.h>

typedef struct
{
   unsigned char cmd;
   unsigned char hex;
   unsigned char ack;
   unsigned char nodata_flag;
}hexfileType;

#define CMD_S_SIZE (1)
#define CMD_W_SIZE (1)
#define CMD_X_SIZE (1)

#define LOCK       -1
#define UNLOCK      1
#define PERM 0666      /* Zugriffsrechte */
#define KEY  123458L


//int semaphore_operation (int op);
//int init_semaphore (void);
void on_window_destroy (GtkObject *object, gpointer user_data);
gchar *open_filename(GtkWidget * widget);
void GtkTextviewAppend(GtkWidget *textview, gchar *text);
int return_comport(const gchar *comport);
int return_baudrate(const gchar *baud);
void clearTextWidget(GtkWidget *widget);
char charToNibble(char c);
unsigned char *hexStringToBytes(char *inhex);
unsigned char fileExists(char *fname);
void GtkTextviewAppendInfo(const char *format, ...);
void GtkTextviewAppendHexfile(const char *format, ...);


#endif /* HELPERS_H_ */
