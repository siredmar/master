#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rs232/rs232.h"
#include "helpers.h"

unsigned char *edid_string[128];
unsigned char edid_raw[128];

/* windows declarations */
GtkWidget *window_main, *window_about, *window_preferences;//, *scrolled_window_hexfile, *scrolled_window_status;

/* window_main widgets */
GtkWidget *text_hexfile, *text_info, *status, *button_program;

/* window_preferences widges */
GtkWidget *entry_serial_interface, *entry_baudrate, *button_save_settings, *button_discard_settings;

//GtkTextBuffer *buffer_hexfile, *buffer_status;



void program_edid(GtkWidget * widget, gpointer user_data)
{

}

void preferences_discard(GtkWidget * widget, gpointer user_data)
{

}

void preferences_save(GtkWidget * widget, gpointer user_data)
{

}

void help_abou(GtkWidget * widget, gpointer user_data)
{

}

void edit_preferences(GtkWidget * widget, gpointer user_data)
{

}




void file_open(GtkWidget * widget, gpointer user_data)
{

   char sprintf_buf[255];
   int cnt = 0;
   char c;
   FILE *fp;
   gchar *filename;

   long lSize;
   char * buffer;
   size_t result;


   clearTextWidget(text_hexfile);

   filename = open_filename(window_main);
   sprintf(sprintf_buf, "Opening %s\n", filename);
   GtkTextviewAppend(text_info, sprintf_buf);

   fp = fopen(filename, "rb");
   if (fp == NULL)
   {
      GtkTextviewAppend(text_info, "Error opening profile file!\n");
   }
   fseek (fp, 0, SEEK_END);
   lSize = ftell (fp);
   rewind (fp);

   // allocate memory to contain the whole file:
   buffer = (unsigned char*) malloc (sizeof(unsigned char)*lSize);
   if (buffer == NULL)
   {
      sprintf(buffer, "Memory error ", stderr);
      GtkTextviewAppend(text_info, buffer);
   }

   result = fread (buffer, 1, lSize, fp);
   if (result != lSize)
   {
      sprintf(sprintf_buf, "Reading error ", stderr);
      GtkTextviewAppend(text_info, sprintf_buf);
   }

   printf("%s\n", buffer);
   for(cnt = 0; cnt < lSize; cnt++)
   {
      edid_raw[cnt] = buffer[cnt];
      sprintf(sprintf_buf, "0x%.2X ", buffer[cnt]);
      printf("cnt[%i]: 0x%.2X\n", cnt, buffer[cnt]);
      GtkTextviewAppend(text_hexfile, sprintf_buf);
   }
   //   do {
   //      c = fgetc (fp);
   //      edid_raw[cnt] = c;
   //      printf("cnt[%i]: 0x%X\n", cnt, c);
   //      sprintf(buffer, "%c", c);
   //      GtkTextviewAppend(text_hexfile, buffer);
   //      cnt++;
   //   } while (c != EOF);


}


int main(int argc, char *argv[])
{




   GtkBuilder *builder;

   g_thread_init(NULL);
   gdk_threads_init();

   gdk_threads_enter();
   gtk_init(&argc, &argv);

   builder = gtk_builder_new();
   gtk_builder_add_from_file(builder, "glade/main.xml", NULL);

   /* connect glade windows to gtk windows */
   window_main        = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
   window_about       = GTK_WIDGET(gtk_builder_get_object(builder, "window_about"));
   window_preferences = GTK_WIDGET(gtk_builder_get_object(builder, "window_preferences"));

   /* connect glade widgets to gtk widgets for window_main */
   text_hexfile        = GTK_WIDGET(gtk_builder_get_object(builder, "text_hexfile"));
   text_info           = GTK_WIDGET(gtk_builder_get_object(builder, "text_info"));
   status              = GTK_WIDGET(gtk_builder_get_object(builder, "status"));
   button_program      = GTK_WIDGET(gtk_builder_get_object(builder, "button_program"));

   /* connect glade widgets to gtk widgets for window_preferences */
   entry_serial_interface  = GTK_WIDGET(gtk_builder_get_object(builder, "entry_serial_interface"));
   entry_baudrate          = GTK_WIDGET(gtk_builder_get_object(builder, "entry_baudrate"));
   button_save_settings    = GTK_WIDGET(gtk_builder_get_object(builder, "button_save_settings"));
   button_discard_settings = GTK_WIDGET(gtk_builder_get_object(builder, "button_discard_settings"));
   //
   //   buffer_hexfile = gtk_text_buffer_new (NULL);
   //   buffer_status = gtk_text_buffer_new (NULL);

   //   scrolled_window_hexfile = gtk_scrolled_window_new (NULL, NULL);
   //   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window_hexfile),
   //                                     GTK_POLICY_AUTOMATIC,
   //                                     GTK_POLICY_AUTOMATIC);
   //
   //
   //   scrolled_window_status = gtk_scrolled_window_new (NULL, NULL);
   //   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window_status),
   //                                     GTK_POLICY_AUTOMATIC,
   //                                     GTK_POLICY_AUTOMATIC);

   gtk_builder_connect_signals(builder, NULL);
   g_object_unref(G_OBJECT(builder));

   gtk_widget_show(window_main);
   gtk_main();
   gdk_threads_leave();
   return 0;
}
