#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rs232/rs232.h"
#include "helpers.h"

unsigned char *edid_string[128];
unsigned char *edid_raw;
int comport;
int baudrate;

/* windows declarations */
GtkWidget *window_main, *window_about, *window_preferences;

/* window_main widgets */
GtkWidget *text_hexfile, *text_info, *status, *button_program;

/* window_preferences widges */
GtkWidget *entry_serial_interface, *entry_baudrate, *button_save_settings, *button_discard_settings;



void program_edid(GtkWidget * widget, gpointer user_data)
{

}

void preferences_back(GtkWidget * widget, gpointer user_data)
{
   char sprintf_buf[255];
   loadSerialSettings();
   sprintf(sprintf_buf, "Set comport to %s at %s baud\n",gtk_entry_get_text(entry_serial_interface), gtk_entry_get_text(entry_baudrate));
   GtkTextviewAppend(text_info, sprintf_buf);
   gtk_widget_hide(window_preferences);
}

void edit_preferences(GtkWidget * widget, gpointer user_data)
{
   gtk_widget_show(window_preferences);
}

unsigned char open_binary_file(char *filename)
{
   FILE *fp;
   long lSize;
   char * buffer;
   size_t result;
   char sprintf_buf[255];
   int cnt = 0;
   //char c;
   clearTextWidget(text_hexfile);

   if(filename == NULL)
   {
      sprintf(sprintf_buf, "Error opening file: no file selected!\n");
      GtkTextviewAppend(text_info, sprintf_buf);
      return 1;
   }
   else
   {
      sprintf(sprintf_buf, "Opening %s\n", filename);
      GtkTextviewAppend(text_info, sprintf_buf);
   }
   fp = fopen(filename, "rb");
   if (fp == NULL)
   {
      GtkTextviewAppend(text_info, "Error opening file!\n");
      return 2;
   }
   fseek (fp, 0, SEEK_END);
   lSize = ftell (fp);
   rewind (fp);

   buffer = (unsigned char*) malloc (sizeof(unsigned char)*lSize);
   edid_raw = (unsigned char*) malloc (sizeof(unsigned char)*lSize);
   if (buffer == NULL)
   {
      sprintf(buffer, "Memory error ", stderr);
      GtkTextviewAppend(text_info, buffer);
      return 3;
   }

   result = fread (buffer, 1, lSize, fp);
   if (result != lSize)
   {
      sprintf(sprintf_buf, "Reading error ", stderr);
      GtkTextviewAppend(text_info, sprintf_buf);
      return 4;
   }

   //printf("%s\n", buffer);
   for(cnt = 0; cnt < lSize; cnt++)
   {
      edid_raw[cnt] = (0xFF & buffer[cnt]);
      sprintf(sprintf_buf, "0x%.2X ", edid_raw[cnt]);
      printf("cnt[%i]: 0x%.2X\n", cnt, edid_raw[cnt]);
      GtkTextviewAppend(text_hexfile, sprintf_buf);
   }
   fclose(fp);
   free(buffer);

}

void file_open(GtkWidget * widget, gpointer user_data)
{

   gchar *filename;

   filename = open_filename(window_main);
   open_binary_file(filename);
}

void loadSerialSettings()
{
//   char *sprintf_buf;
//   static int firstrun = 1;
//   if(firstrun)
//   {
      comport = return_comport(gtk_entry_get_text(entry_serial_interface));
      baudrate = return_baudrate(gtk_entry_get_text(entry_baudrate));

//      firstrun = 0;
//   }
}

int main(int argc, char *argv[])
{
   GtkBuilder *builder;

   //   g_thread_init(NULL);
   //   gdk_threads_init();
   //
   //   gdk_threads_enter();
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


   gtk_builder_connect_signals(builder, NULL);
   g_object_unref(G_OBJECT(builder));

   gtk_widget_show(window_main);
   loadSerialSettings();

   if(argc > 1)
   {
      if(fileExists(argv[1]))
      {
         open_binary_file(argv[1]);
      }
      else
      {
         GtkTextviewAppend(text_info, "Error: File does not exist!\n");
      }
   }

   gtk_main();



   //   gdk_threads_leave();
   return 0;
}
