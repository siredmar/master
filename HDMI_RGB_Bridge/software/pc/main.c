#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rs232/rs232.h"
#include "helpers.h"
#include <pthread.h>

#define USE_THREADS 0

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

hexfileType *hexfile;

unsigned char checksum = 0x6A;

int waitForInterrupt = 0;

typedef enum
{
   NO_ACK = 0,
   ACK
}CMD_ACK_Type;

volatile CMD_ACK_Type CMD_1_ACK = NO_ACK;
volatile CMD_ACK_Type CMD_2_ACK = NO_ACK;
volatile CMD_ACK_Type CMD_3_ACK = NO_ACK;

volatile int new_data = 0;
int checksum_valid = 0;


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

void setAck(CMD_ACK_Type ack)
{
   ack = ACK;
   //printf("set ACK\n");
}

void resetAck(CMD_ACK_Type ack)
{
   ack = NO_ACK;
   //printf("set ACK\n");
}


int rs232_data_received()
{
   char buf[4];
   read(comport_fd, buf, 4);
   // printf("buf: %s\n", buf);

   //__asm("nop");
   char next_char;
   // uint8 timer_val = 0;
   next_char = buf[0];

   printf("next_char: %c\n", next_char);
   if(next_char == '#')
   {
      uart_str_cnt = 0;
      block_finished = 0;
   }
   if(next_char == '*' && !block_finished)
   {
      block_finished = 1;
      uart_str[uart_str_cnt++] = next_char;
      printf("uart_str: %s\n", uart_str);
      switch(uart_str[1])
      {
      case '1':
         printf("setting CMD_1_ACK = ACK\n");

         CMD_1_ACK = ACK;
         //setAck(CMD_1_ACK);
         break;
//
//      case '2':
//         CMD_2_ACK = ACK;
//         break;
//
//      case '3':
//         CMD_3_ACK = ACK;
//         break;
//      default:

         break;
      }
   }

   if(!block_finished)
   {
      uart_str[uart_str_cnt] = next_char;
      uart_str_cnt++;
   }
}

//void rs232_data_received()
//{
//   char buf[4];
//   read(comport_fd, buf, 3);
//
//   //buf[4] = '\0';
//   printf("string: %s\n", buf);
//   switch(buf[1])
//   {
//   case '1':
//      printf("setting CMD_1_ACK = ACK\n");
//      CMD_1_ACK = ACK;
//      //setAck(CMD_1_ACK);
//      break;
//
//   case '2':
//      printf("setting CMD_2_ACK = ACK\n");
//      CMD_2_ACK = ACK;//setAck(CMD_2_ACK);
//      break;
//
//   case '3':
//      printf("setting CMD_3_ACK = ACK\n");
//      CMD_3_ACK = ACK;//setAck(CMD_3_ACK);
//      break;
//   default:
//
//      break;
//   }
//}

static char *returnSerialCommand(unsigned char cmd, unsigned char hex, unsigned char nodata_flag)
{
   unsigned char buf[4];

   if(nodata_flag == 1)
   {
     // buf = (unsigned char*) malloc(sizeof(unsigned char) * 3);
      buf[0] = '#'; printf("0x%.2X ", buf[0]);
      buf[1] = cmd; printf("0x%.2X ", buf[1]);
      buf[2] = '*'; printf("0x%.2X\n", buf[2]);
      buf[3] = 0;   printf("0x%.2X\n", buf[3]);


   }
   else
   {
     // buf = (unsigned char*) malloc(sizeof(unsigned char) * 4);
      buf[0] = '#'; printf("0x%.2X ", buf[0]);
      buf[1] = cmd; printf("0x%.2X ", buf[1]);
      buf[2] = hex; printf("0x%.2X ", buf[2]);
      buf[3] = '*'; printf("0x%.2X\n", buf[3]);
      //sprintf(buf, "#%c%c*", cmd, hex);
   }
   return buf;
}

void program_edid(GtkWidget * widget, gpointer user_data)
{
   /* TODO: check if hexfile was loaded */
   int command_sent = 0;
   int command_index = 0;


   comport_fd = rs232_open_port(comport, baudrate);

   if(comport_fd)
   {
      GtkTextviewAppendInfo("Opening %s successfully! Writing EDID data into EEPROM... \n", comport);
   }
   else
   {
      GtkTextviewAppendInfo("Error %s Comport! Check port and baudrate!\n", comport);
   }

   while(1)
   {

      checksum_valid = 1;
      if(command_sent == 0)
      {
         CMD_1_ACK = NO_ACK;
         waitForInterrupt = 0;
       //  printf("string: %s\n", returnSerialCommand(hexfile[command_index].cmd, hexfile[command_index].hex, hexfile[command_index].nodata_flag));
         rs232_puts(comport_fd, returnSerialCommand(hexfile[command_index].cmd, hexfile[command_index].hex, hexfile[command_index].nodata_flag), 4);
        // printf("send: %s\n", returnSerialCommand(hexfile[command_index].cmd, hexfile[command_index].hex, hexfile[command_index].nodata_flag));
         //rs232_puts(comport_fd, "#s*");
         waitForInterrupt = 1;
         command_sent = 1;
         //printf("command_sent: %d\n", command_sent);
      }

      if(command_sent && new_data > 0)
      {

         if(new_data >= 2)
         {
            //printf("command_sent: %d, new_data: %d\n", command_sent, new_data);
            rs232_data_received();
           // printf("rs232_data_received();\n");
            //printf("command_sent: %d, new_data: %d\n", command_sent, new_data);
            //ack_received = 1;

         }
      }

      if(command_sent == 1 && CMD_1_ACK == ACK)
      {
         new_data = 0;
         //printf("ACK1: %d, ACK2: %d, ACK3: %d\n", CMD_1_ACK, CMD_2_ACK, CMD_3_ACK);
         // printf("ACK received, continuing with next instruction\n");

         CMD_1_ACK = NO_ACK;

         //printf("CMD_1_ACK: %d\n", CMD_1_ACK);
         //printf("#################################################\n");
         command_sent = 0;

         if(command_index < hexfile_size + CMD_S_SIZE + CMD_X_SIZE)
         {
            //printf("ACK: command_index: %d\n", command_index);
            command_index++;
            //continue;   // continue with next command
         }
         else
         {
            break;      // exit loop because all commands are sent
         }
      }

      //printf("command_index: %d\n", command_index);
      usleep(50000);
      //break;
   }
   checksum_valid = 0;
   printf("sending ended ...\n");

   GtkTextviewAppendInfo("Checksum 0x%.2X\n", checksum);
}

/* TODO: test if ACK signals are working */





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

   hexfile = (hexfileType*)malloc((hexfile_size + CMD_X_SIZE + CMD_S_SIZE) * sizeof(hexfileType)); // +2 for start and end condition commands
   //hexfile = (hexfileType*)malloc(hexfile_size * 2 * sizeof(unsigned char) + 2); // +2 for start and end condition commands

   /* load the actual data into the program */
   for(cnt = 0; cnt < hexfile_size; cnt++)
   {
      edid_raw[cnt] = (0xFF & buffer[cnt]);

      GtkTextviewAppendHexfile("0x%.2X ", edid_raw[cnt]);
   }

   /* Set expected acknowledge values for each command */
   hexfile[0].cmd = 's';
   hexfile[0].hex = 0;
   hexfile[0].ack = '1';
   hexfile[0].nodata_flag = 1;

   for(cnt = 0; cnt < hexfile_size; cnt++)
   {
      hexfile[cnt+CMD_S_SIZE].cmd = 'w';
      hexfile[cnt+CMD_S_SIZE].hex = edid_raw[cnt];
      hexfile[cnt+CMD_S_SIZE].ack = '2';
      hexfile[cnt+CMD_S_SIZE].nodata_flag = 0;
   }

   hexfile[hexfile_size +  CMD_X_SIZE].cmd = 'x';
   hexfile[hexfile_size +  CMD_X_SIZE].hex = 0;
   hexfile[hexfile_size +  CMD_X_SIZE].ack = '3';
   hexfile[hexfile_size +  CMD_X_SIZE].nodata_flag = 1;

   for(cnt = 0; cnt < hexfile_size + CMD_S_SIZE + CMD_X_SIZE; cnt++)
   {
      printf("%u\tcmd: %c, hex: 0x%.2X, ack: %c, nodata_flag: %u\n", cnt, hexfile[cnt].cmd, hexfile[cnt].hex, hexfile[cnt].ack, hexfile[cnt].nodata_flag);
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
   //pthread_t main_tid;
   //
   //
   //
   //   g_thread_init(NULL);
   //   gdk_threads_init();
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
