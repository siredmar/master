#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rs232/rs232.h"
#include "helpers.h"
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <errno.h>

//#define _GNU_SOURCE 1

#define USE_THREADS 0

/** The maximum size of a string that is sent via UART */
#define MAXUARTSTR 250
#define TIMEOUT_TIMES_50MS 700
#define TIMEOUT_50MS       5000
#define GTK_MAIN_ITERATION()         // while( gtk_events_pending() ) \
      gtk_main_iteration()

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

char file_opened = 1;

/* windows declarations */
GtkWidget *window_main, *window_about, *window_preferences;

/* window_main widgets */
GtkWidget *text_hexfile, *text_info, *progressbar, *button_program, *menu_edid, *menu_file;

/* window_preferences widges */
GtkWidget *entry_serial_interface, *entry_baudrate, *button_save_settings, *button_discard_settings;

hexfileType *hexfile;

unsigned char checksum = 0x6A;

//pid_t child;

int waitForInterrupt = 0;

typedef enum
{
   NO_ACK = 0,
   ACK
}CMD_ACK_Type;

volatile CMD_ACK_Type CMD_ACK = NO_ACK;

volatile int new_data = 0;
int checksum_valid = 0;

int shm_id;
short timeoutCounter;

//struct sembuf semaphore;
//int semid;

void disableWidgets()
{
   gtk_widget_set_sensitive(button_program, 0);
   gtk_widget_set_sensitive(window_main, 0);
}

void enableWidgets()
{
   gtk_widget_set_sensitive(button_program, 1);
   gtk_widget_set_sensitive(window_main, 1);
}

static void loadSerialSettings()
{
   comport = gtk_entry_get_text(GTK_ENTRY(entry_serial_interface));
   baudrate = return_baudrate(gtk_entry_get_text(GTK_ENTRY(entry_baudrate)));
}

static int writeConfigFile()
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

   GtkTextviewAppendInfo("Writing config file: %s.\n", filename);

   fclose(fp);
   return 0;
}

static int readConfigFile()
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

static void rs232_data_received()
{
   char buf[4] = {0};
   char next_char;

   read(comport_fd, buf, 4);
   next_char = buf[0];
   if(next_char == '#')
   {
      uart_str_cnt = 0;
      block_finished = 0;
   }
   if(next_char == '*' && !block_finished)
   {
      block_finished = 1;
      uart_str[uart_str_cnt++] = next_char;
      switch(uart_str[1])
      {
      case '1':
      case '2':
      case '3':
         //printf("ACK%c received\n", uart_str[1]);
         CMD_ACK = ACK;
         break;
      }
   }

   if(!block_finished)
   {
      uart_str[uart_str_cnt] = next_char;
      uart_str_cnt++;
   }
}

static char *returnSerialCommand(unsigned char cmd, unsigned char hex, unsigned char nodata_flag)
{
   unsigned char *buf;

   if(nodata_flag == 1)
   {
      buf = (unsigned char*) malloc(sizeof(unsigned char) * 3);
      sprintf(buf, "#%c*", cmd);
      //printf("0x%.2X 0x%.2X 0x%.2X\n", buf[0], buf[1], buf[2]);
   }
   else
   {
      buf = (unsigned char*) malloc(sizeof(unsigned char) * 4);
      sprintf(buf, "#%c%c*", cmd, hex);
      // printf("0x%.2X 0x%.2X 0x%.2X 0x%.2X\n", buf[0], buf[1], buf[2], buf[3]);
   }
   return buf;
}

void program_edid(GtkWidget * widget, gpointer user_data)
{
   int command_sent = 0;
   int command_index = 0;
   int timeoutFlag = 0;

   int loopExitFlag = 0;

   timeoutCounter = 0;
//   disableWidgets();
   if(file_opened == 1)
   {
      GtkTextviewAppendInfo("No file opened!\n");
   }
   else
   {
      comport_fd = rs232_open_port(comport, baudrate);
//      if(comport_fd)
//      {
//         GtkTextviewAppendInfo("Opening %s successfully! Writing EDID data into EEPROM... \n", comport);
//         GTK_MAIN_ITERATION();
//      }
//      else
//      {
//         GtkTextviewAppendInfo("Error %s Comport! Check port and baudrate!\n", comport);
//         GTK_MAIN_ITERATION();
//      }

      while(timeoutCounter < TIMEOUT_TIMES_50MS)
      {
         checksum_valid = 1;
         if(command_sent == 0 && command_index < hexfile_size + CMD_S_SIZE + CMD_X_SIZE)
         {
            CMD_ACK = NO_ACK;
//            waitForInterrupt = 0;
            rs232_puts(comport_fd, returnSerialCommand(hexfile[command_index].cmd, hexfile[command_index].hex, hexfile[command_index].nodata_flag), 4);

            //            gdk_threads_enter();

            //            gdk_threads_leave();
            //gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), (float)command_index/(float)(hexfile_size + 2));
//            waitForInterrupt = 1;
            command_sent = 1;
         }

         if(command_sent && new_data > 0)
         {
            if(new_data >= 2)
            {
               rs232_data_received();
            }
         }

         if(command_sent == 1 && CMD_ACK == ACK)
         {
            new_data = 0;
            CMD_ACK = NO_ACK;
            command_sent = 0;

            if(command_index < hexfile_size + CMD_S_SIZE + CMD_X_SIZE)
            {
               printf("CHILD::command_index: %i < %i + 2\n", command_index, hexfile_size);
               command_index++;
            }
         }

         if(command_index >= hexfile_size + CMD_S_SIZE + CMD_X_SIZE)
         {

            timeoutCounter = TIMEOUT_TIMES_50MS + 1;
         }

         printf("loop, timeoutCounter: %i \n", timeoutCounter);


//         gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), (float)command_index/(float)(hexfile_size + CMD_S_SIZE + CMD_X_SIZE));
//         GTK_MAIN_ITERATION();


         usleep(TIMEOUT_50MS);

         timeoutCounter++;
      }

      checksum_valid = 0;

//      if(command_index >= hexfile_size + CMD_S_SIZE + CMD_X_SIZE)
//      {
//         GtkTextviewAppendInfo("Writing content successfully... \n");
//         GTK_MAIN_ITERATION();
//      }
//      else
//      {
//         GtkTextviewAppendInfo("Writing timed out! Please try again... \n");
//         GTK_MAIN_ITERATION();
//      }

//         enableWidgets();
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

static unsigned char open_binary_file(char *filename)
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

   //   for(cnt = 0; cnt < hexfile_size + CMD_S_SIZE + CMD_X_SIZE; cnt++)
   //   {
   //      printf("%u\tcmd: %c, hex: 0x%.2X, ack: %c, nodata_flag: %u\n", cnt, hexfile[cnt].cmd, hexfile[cnt].hex, hexfile[cnt].ack, hexfile[cnt].nodata_flag);
   //   }

   fclose(fp);
   free(buffer);
   return 0;
}

void file_open(GtkWidget * widget, gpointer user_data)
{

   gchar *filename;

   filename = open_filename(window_main);
   file_opened = open_binary_file(filename);
}

int main(int argc, char *argv[])
{
   GtkBuilder *builder;
   //pthread_t main_tid;
   //
   shm_id = shmget(IPC_PRIVATE, sizeof(short), IPC_CREAT | 0644);
   if (shm_id == -1) {
      printf("error creating shared memory!\n");
      exit(1);
   }
   timeoutCounter = (short) shmat(shm_id, NULL, 0);
   if ((short) timeoutCounter == -1) {
      printf("error attaching shared memory\n");
      exit(2);
   }
   timeoutCounter = 0;
   //
   //   XInitThreads();
   g_thread_init(NULL);
   gdk_threads_init();
   gdk_threads_enter();

   //   init_semaphore();


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
         file_opened = open_binary_file(argv[1]);
      }
      else
      {
         GtkTextviewAppendInfo("Error: File does not exist!\n");
      }
   }

   gtk_main();



   gdk_threads_leave();
   return 0;
}
