#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rs232/rs232.h"
#include "helpers.h"


/** The maximum size of a string that is sent via UART */
#define MAXUARTSTR 250

unsigned char *edid_string[128];
unsigned char *edid_raw;
char *comport;
int received_flag = 0;

int baudrate;
long hexfile_size;
int comport_fd;

/** Counter variable used for determining the type and the end of the received data set */
char uart_str_cnt = 0;

/** Status variable, indicating whether a new data set is incoming (1) or not (0) */
char new_data = 0;

/** Buffer holding the received data set */
char uart_str[MAXUARTSTR] = "";

/** Status variable, indicating whether the end of a data set has been found (\n\r) */
char block_finished = 0;

/** Index of the beginning of a new data set within the UART buffer */
char new_data_index = 0;


/* windows declarations */
GtkWidget *window_main, *window_about, *window_preferences;

/* window_main widgets */
GtkWidget *text_hexfile, *text_info, *progressbar, *button_program;

/* window_preferences widges */
GtkWidget *entry_serial_interface, *entry_baudrate, *button_save_settings, *button_discard_settings;

enum
{
   CMD_1_ACK = 1,
   CMD_2_ACK = 2,
   CMD_3_ACK = 3
};


void loadSerialSettings()
{
   comport = gtk_entry_get_text(GTK_ENTRY(entry_serial_interface));
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

/* TODO: test if ACK signals are working */
int rs232_data_received()
{
   //__asm("nop");
   char next_char;
   // uint8 timer_val = 0;
   read(comport_fd, next_char, 1);

   if(next_char == '#')
   {
      uart_str_cnt = 0;
      block_finished = 0;
   }
   if(next_char == '*' && !block_finished)
   {
      block_finished = 1;
      switch(uart_str[1])
      {
      case '1':
         setAck(CMD_1_ACK);
         break;

      case '2':
         setAck(CMD_2_ACK);
         break;

      case '3':
         setAck(CMD_3_ACK);
         break;
      default:

         break;
      }
   }

   if(!block_finished)
   {
      uart_str[uart_str_cnt] = next_char;
      uart_str_cnt++;
   }
}

void waitForAck(int ack)
{
   while(!ack);
}

void setAck(int ack)
{
   ack = 1;
}

void resetAck(int ack)
{
   ack = 0;
}

void program_edid(GtkWidget * widget, gpointer user_data)
{
   /* TODO: check if hexfile was loaded */

   unsigned char sprintf_buf[255];
   //int comport_fd;
   int cnt;

   comport_fd = rs232_open_port(comport, baudrate);
   //ret = RS232_OpenComport(comport, baudrate);
   printf("comport_fd: %i\n", comport_fd);
   if(comport_fd)
   {
      GtkTextviewAppendInfo("Opening %s successfully! Writing EDID data into EEPROM... \n", comport);
   }
   else
   {
      GtkTextviewAppendInfo("Error %s Comport! Check port and baudrate!\n", comport);
   }

   rs232_puts(comport_fd, "#s*");
   waitForAck(CMD_1_ACK);
   resetAck(CMD_1_ACK);

   //printf("hexfile_size: %i\n", hexfile_size);
   for(cnt = 0; cnt < hexfile_size; cnt++)
   {
      sprintf(sprintf_buf, "#w,%c*", edid_raw[cnt]);
      //RS232_cputs(comport, sprintf_buf);
      waitForAck(CMD_2_ACK);
      resetAck(CMD_2_ACK);

      gtk_progress_set_percentage(GTK_PROGRESS_BAR(progressbar), (float)cnt / (float)hexfile_size);

   }
   //RS232_cputs(comport, "#x*");
   waitForAck(CMD_3_ACK);
   resetAck(CMD_3_ACK);

   gtk_progress_set_percentage(GTK_PROGRESS_BAR(progressbar), 1);

   //rs232_close_port(comport_fd);
   //RS232_CloseComport(comport);

   GtkTextviewAppendInfo("EDID data successfully written into EEPROM.\n");
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
   hexfile_size = ftell (fp);
   rewind (fp);

   buffer = (char*) malloc (sizeof(char)*hexfile_size);
   edid_raw = (unsigned char*) malloc (sizeof(unsigned char)*hexfile_size);
   if (buffer == NULL)
   {
      GtkTextviewAppendInfo("Memory error ", stderr);
      return 3;
   }

   result = fread (buffer, 1, hexfile_size, fp);
   if (result != hexfile_size)
   {
      GtkTextviewAppendInfo("Reading error ", stderr);
      return 4;
   }

   for(cnt = 0; cnt < hexfile_size; cnt++)
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
   progressbar         = GTK_WIDGET(gtk_builder_get_object(builder, "progressbar"));
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

   readConfigFile();
   loadSerialSettings();

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
