#include <gtk/gtk.h>
#include <cairo.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rs232/rs232.h"

//#include "handlers/handlers.h"

#define  YOFFSET 255
#define MAX 100

int timetable_gfx[100] = { 0 };
int values_gfx[100] = { 0 };

int running_state = 0;
char *buf;
char *buf2;
char *buf3;
char trennzeichen[] = " ";
char *line;
int token_array[2];
int timetable[100] = { 0 };
int values[100] = { 0 };
int comport;
int baudrate;
int cnt;
pid_t child;


gchar *filename;

GtkWidget *window, *entry_device, *entry_baudrate, *button_run, *text_status, *entry_manual;
FILE *fp;

void GtkTextviewAppend(GtkWidget *textview, gchar *text)
{
   GtkTextBuffer *tbuffer;
   GtkTextIter ei;

   tbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
   gtk_text_buffer_get_end_iter(tbuffer, &ei);
   gtk_text_buffer_insert(tbuffer, &ei, text, -1);
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

/* Expose Event Callback: used for cairo to draw the graph */
static gboolean on_expose_event(GtkWidget * widget, GdkEventExpose * event,
      gpointer data)
{
   int i = 0;
   cairo_t *cr;
   cr = gdk_cairo_create(widget->window);
   cairo_move_to(cr, 0, YOFFSET);
   cairo_set_source_rgb(cr, 0, 0, 0);
   cairo_set_line_width(cr, 1);

   for (i = 0; i <= 15; i++) {
      cairo_line_to(cr, timetable_gfx[i], YOFFSET - values_gfx[i]);

   }
   cairo_line_to(cr, timetable_gfx[15], YOFFSET);
   cairo_stroke(cr);
}

/* Entry changed Callbak: used to refresh the graph */
void entry_changed(GtkWidget * widget, gpointer user_data)
{
   read_table();
   gtk_widget_queue_draw(drawing);

}

/* File->New Callback: reset values */
void file_new(GtkWidget * widget, gpointer user_data)
{
   clear_table();

}

void file_open(GtkWidget * widget, gpointer user_data)
{
   clear_table();
   cnt = 0;
   //      i = 0;
   filename = open_filename(window);
   g_print("open->filename: %s\n", filename);
   GtkTextviewAppend(text_status, "Open File: ");
   GtkTextviewAppend(text_status, filename);
   GtkTextviewAppend(text_status, "\n");
   fp = fopen(filename, "r");
   if (fp == NULL) {
      g_print("Error opening profile file!\n");
   }
   while (!feof(fp)) {
      int i;
      i = 0;
      char *token;
      fgets(line, MAX, fp);

      token = strtok(line, "\t ,.");
      while (token != NULL) {
         token_array[i++] = atoi(token);
         token = strtok(NULL, "\t,. ");

      }

   }
}


/* Run clicked callback: indicates that button was pressed */
void file_save(GtkWidget * widget, gpointer user_data)
{
   g_print("save->filename: %s\n", filename);
   fp = fopen(filename, "w");
   if (fp == NULL) {
      g_print("Error opening profile file!\n");
      GtkTextviewAppend(text_status, "Error opening Profile File!\n");

   }

   fclose(fp);
}

/* Run clicked callback: indicates that button was pressed */
void file_save_as(GtkWidget * widget, gpointer user_data)
{
   g_print("save->filename: %s\n", filename);
   filename = open_filename(window);
   fp = fopen(filename, "w");
   if (fp == NULL) {
      g_print("Error opening profile file!\n");
      GtkTextviewAppend(text_status, "Error opening Profile File!\n");
   }


   fclose(fp);
}



/* Run clicked callback: indicates that button was pressed */
void send_clicked(GtkWidget * widget, gpointer user_data)
{
   char *buf = (char*) malloc(10*sizeof(char));
   comport = return_comport(gtk_entry_get_text(entry_device));
   baudrate = return_baudrate(gtk_entry_get_text(entry_baudrate));
   if (OpenComport(comport, baudrate)) {
      g_print("Error opening comport!\n");
   }

   //    g_print("Sending: %s\n", buf);

   buf = gtk_entry_get_text(entry_manual);
   SendBuf(comport, buf, strlen(buf));

   GtkTextviewAppend(text_status, "Sent: ");
   GtkTextviewAppend(text_status, (gchar *)buf);
   GtkTextviewAppend(text_status, "\n");
   CloseComport(comport);
}

void run_clicked(GtkWidget * widget, gpointer user_data){
   char buf[100];
   char buf2[100];
   char buf3[100];

   int i;



   if (running_state == 0) {
      child = fork();
      gtk_button_set_label(button_run, "Stop");
      running_state = 1;
   } else {
      running_state = 0;
      SendBuf(comport, "#000*", 5);
      gtk_button_set_label(button_run, "Run");
      CloseComport(comport);
      kill(child, SIGKILL);
   }

   if (!child) {
      {
      /// todo: communication between child and parent for text_status output!
      g_print("run clicked...\n");
      comport = return_comport(gtk_entry_get_text(entry_device));
      baudrate = return_baudrate(gtk_entry_get_text(entry_baudrate));
      g_print("comport: /dev/%s -> no. %d\n",
            gtk_entry_get_text(entry_device), comport);
      g_print("baudrate: %s -> no. %d\n",
            gtk_entry_get_text(entry_baudrate), baudrate);
      read_table();

      if (OpenComport(comport, baudrate)) {
         g_print("Error opening comport!\n");
         GtkTextviewAppend(text_status, "Error opening comport!\n");
      }

      sprintf(buf2, "#%s*", buf3);


      GtkTextviewAppend(text_status, "Sent: ");
      //    GtkTextviewAppend(text_status, (gchar *)buf2);
      GtkTextviewAppend(text_status, "\n");
      //	    GtkTextviewAppend(text_status, "Sent: ");
      //	    GtkTextviewAppend(text_status, buf2);
      //	    GtkTextviewAppend(text_status, "\n");

      if (running_state == 1) {

         SendBuf(comport, buf2, strlen(buf2));
      }
   }
   CloseComport(comport);

}
else if (child > 0) {

} else
   perror("Fork error\n");




//    gtk_widget_queue_draw(drawing);
}

/* Destroy callback: quits gtk */
void on_window_destroy(GtkObject * object, gpointer user_data)
{
   kill(child, SIGKILL);
   gtk_main_quit();
}

void quit_clicked(GtkObject * object, gpointer user_data)
{
   kill(child, SIGKILL);
   gtk_main_quit();
}

int main(int argc, char *argv[])
{

   int cnt;
   line = (char *) malloc(sizeof(char) * MAX);

   GtkBuilder *builder;

   g_thread_init(NULL);
   gdk_threads_init();

   gdk_threads_enter();
   gtk_init(&argc, &argv);


   builder = gtk_builder_new();
   gtk_builder_add_from_file(builder, "glade/main.xml", NULL);

   window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

   entry_device =
         GTK_WIDGET(gtk_builder_get_object(builder, "entry_device"));
   entry_baudrate =
         GTK_WIDGET(gtk_builder_get_object(builder, "entry_baudrate"));
   button_run = GTK_WIDGET(gtk_builder_get_object(builder, "button_run"));
   text_status = GTK_WIDGET(gtk_builder_get_object(builder, "text_status"));
   entry_manual = GTK_WIDGET(gtk_builder_get_object(builder, "entry_manual"));

   gtk_builder_connect_signals(builder, NULL);
   g_object_unref(G_OBJECT(builder));



   gtk_widget_show(window);
   gtk_main();
   gdk_threads_leave();
   return 0;
}
