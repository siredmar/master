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
