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


void loadSerialSettings()
{
   comport = return_comport(gtk_entry_get_text(GTK_ENTRY(entry_serial_interface)));
   baudrate = return_baudrate(gtk_entry_get_text(GTK_ENTRY(entry_baudrate)));
}

int writeConfigFile()
{
   FILE * fp;
   char sprintf_buf[255];
   char filename[255];

   sprintf(filename, "%s/.edid_writer/config", getenv("HOME"));
   fp = fopen(filename, "w+");
   if (fp == NULL)
   {

      GtkTextviewAppendInfo("Cannot open config file %s! Check permissions!\n", filename);
      return 1;
   }
   sprintf(sprintf_buf, "%s\n%s\n", gtk_entry_get_text(GTK_ENTRY(entry_serial_interface)), gtk_entry_get_text(GTK_ENTRY(entry_baudrate)));
   fputs(sprintf_buf, fp);

   //sprintf(sprintf_buf, );
   GtkTextviewAppendInfo("Writing config file: %s.\n", filename);

   fclose(fp);
   return 0;
}

int readConfigFile()
{
   FILE * fp;
   char * line = NULL;
   size_t len = 0;
   ssize_t read;
   char filename[255];
   char config_file_read[2][255];
   int cnt = 0;
   char* pRemoveNewline ;


   sprintf(filename, "%s/.edid_writer/config", getenv("HOME"));
   fp = fopen(filename, "r");
   if (fp == NULL)
   {
      GtkTextviewAppendInfo("Cannot open config file %s. Setting default values!\n", filename);
      writeConfigFile();
      return 1;
   }
   while ((read = getline(&line, &len, fp)) != -1)
   {
      sprintf(config_file_read[cnt], "%s", line);
      pRemoveNewline = strstr(config_file_read[cnt++], "\n");
      *pRemoveNewline = '\0';
   }

   GtkTextviewAppendInfo("Reading config file: %s. Setting comport to %s at %s baud.\n", filename, config_file_read[0], config_file_read[1]);

   gtk_entry_set_text(GTK_ENTRY(entry_serial_interface), config_file_read[0]);
   gtk_entry_set_text(GTK_ENTRY(entry_baudrate), config_file_read[1]);


   if (line)
      free(line);
   fclose(fp);
   return 0;
}

void program_edid(GtkWidget * widget, gpointer user_data)
{
   int ret;
   ret = RS232_OpenComport(comport, baudrate);
   if(ret == 1)
   {
      GtkTextviewAppendInfo("Error opening Comport! Check port and baudrate!\n");
   }
}

void preferences_back(GtkWidget * widget, gpointer user_data)
{
   loadSerialSettings();
   GtkTextviewAppendInfo("Set comport to %s at %s baud\n",gtk_entry_get_text(GTK_ENTRY(entry_serial_interface)), gtk_entry_get_text(GTK_ENTRY(entry_baudrate)));
   writeConfigFile();
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
   int cnt = 0;
   //char c;
   clearTextWidget(text_hexfile);

   if(filename == NULL)
   {
      GtkTextviewAppendInfo("Error opening file: no file selected!\n");
      return 1;
   }
   else
   {
      GtkTextviewAppendInfo("Opening %s\n", filename);
   }
   fp = fopen(filename, "rb");
   if (fp == NULL)
   {
      GtkTextviewAppendInfo("Error opening file!\n");
      return 2;
   }
   fseek (fp, 0, SEEK_END);
   lSize = ftell (fp);
   rewind (fp);

   buffer = (char*) malloc (sizeof(char)*lSize);
   edid_raw = (unsigned char*) malloc (sizeof(unsigned char)*lSize);
   if (buffer == NULL)
   {
      GtkTextviewAppendInfo("Memory error ", stderr);
      return 3;
   }

   result = fread (buffer, 1, lSize, fp);
   if (result != lSize)
   {
      GtkTextviewAppendInfo("Reading error ", stderr);
      return 4;
   }

   for(cnt = 0; cnt < lSize; cnt++)
   {
      edid_raw[cnt] = (0xFF & buffer[cnt]);
      GtkTextviewAppendHexfile("0x%.2X ", edid_raw[cnt]);
   }
   fclose(fp);
   free(buffer);
   return 0;
}

void file_open(GtkWidget * widget, gpointer user_data)
{

   gchar *filename;

   filename = open_filename(window_main);
   open_binary_file(filename);
}


int main(int argc, char *argv[])
{
   GtkBuilder *builder;
   int ret;
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

   //gtk_widget_show(window_preferences);
   //gtk_widget_hide(window_preferences);

   ret = readConfigFile();

   if (ret == 1)
   {
      loadSerialSettings();

   }

   if(argc > 1)
   {
      if(fileExists(argv[1]))
      {
         open_binary_file(argv[1]);
      }
      else
      {
         GtkTextviewAppendInfo("Error: File does not exist!\n");
      }
   }

   gtk_main();



   //   gdk_threads_leave();
   return 0;
}
