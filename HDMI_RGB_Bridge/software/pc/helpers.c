/*
 * helpers.c
 *
 *  Created on: Mar 31, 2014
 *      Author: armin
 */
#include "helpers.h"


char* hexToString(unsigned char x)
{
   const char *hex = "0123456789ABCDEF";

   char ret[4];
   sprintf(ret, "0x%c%c\n", hex[x >> 4 & 0x0F], hex[x & 0x0F]);
   return ret;
}


void GtkTextviewAppend(GtkWidget *textview, gchar *text)
{
   GtkTextBuffer *tbuffer;
   GtkTextIter ei;

   tbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
   gtk_text_buffer_get_end_iter(tbuffer, &ei);
   gtk_text_buffer_insert(tbuffer, &ei, text, -1);
}


gchar *open_filename(GtkWidget * widget)
{
   GtkWidget *chooser;
   gchar *filename = NULL;
   chooser = gtk_file_chooser_dialog_new("Open File...",
         GTK_WINDOW(widget),
         GTK_FILE_CHOOSER_ACTION_OPEN,
         GTK_STOCK_CANCEL,
         GTK_RESPONSE_CANCEL,
         GTK_STOCK_OPEN, GTK_RESPONSE_OK,
         NULL);
   if (gtk_dialog_run(GTK_DIALOG(chooser)) == GTK_RESPONSE_OK) {
      filename =
            gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser));
   }
   gtk_widget_destroy(chooser);
   return filename;
}

void clearTextWidget(GtkWidget *widget)
{
   GtkTextBuffer *buffer = NULL;
   gtk_text_view_set_buffer(widget, buffer);
}

void on_window_destroy (GtkObject *object, gpointer user_data)
{
   gtk_main_quit();
}


int return_comport(char *comport)
{
   int retval = 0;

   if (strcmp(comport, "ttyS0") == 0)
      retval = 0;
   else if (strcmp(comport, "ttyS1") == 0)
      retval = 1;
   else if (strcmp(comport, "ttyS2") == 0)
      retval = 2;
   else if (strcmp(comport, "ttyS3") == 0)
      retval = 3;
   else if (strcmp(comport, "ttyS4") == 0)
      retval = 4;
   else if (strcmp(comport, "ttyS5") == 0)
      retval = 5;
   else if (strcmp(comport, "ttyS6") == 0)
      retval = 6;
   else if (strcmp(comport, "ttyS7") == 0)
      retval = 7;
   else if (strcmp(comport, "ttyS8") == 0)
      retval = 8;
   else if (strcmp(comport, "ttyS9") == 0)
      retval = 9;
   else if (strcmp(comport, "ttyS10") == 0)
      retval = 10;
   else if (strcmp(comport, "ttyS11") == 0)
      retval = 11;
   else if (strcmp(comport, "ttyS12") == 0)
      retval = 12;
   else if (strcmp(comport, "ttyS13") == 0)
      retval = 13;
   else if (strcmp(comport, "ttyS14") == 0)
      retval = 14;
   else if (strcmp(comport, "ttyS15") == 0)
      retval = 15;
   else if (strcmp(comport, "ttyUSB0") == 0)
      retval = 16;
   else if (strcmp(comport, "ttyUSB1") == 0)
      retval = 17;
   else if (strcmp(comport, "ttyUSB@") == 0)
      retval = 18;
   else if (strcmp(comport, "ttyUSB3") == 0)
      retval = 19;
   else if (strcmp(comport, "ttyUSB4") == 0)
      retval = 20;
   else if (strcmp(comport, "ttyUSB5") == 0)
      retval = 21;
   printf("return_comport->comport: %s, retval: %d\n", comport, retval);
   return retval;
}

int return_baudrate(char *baud)
{
   int retval;
   retval = atoi(baud);

   return retval;
}
