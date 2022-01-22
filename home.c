#include "home.h"
#include "graphs.h"
#include "process_info.h"
#include "file_systems.h"
#include "page_one.h"
#include "drawings.h"
#include "process_list.h"

int maxY = 20000;
extern char *text;

// looked at for reference and used code from
// https://developer.gnome.org/gtk3/stable/gtk-getting-started.html and
// https://stackoverflow.com/questions/9823558/gtk-app-process-not-killed

/*
 * Exits program.
 */

static void destroy(GtkWidget *widget, gpointer data)
{
    free(text);
    text = NULL;
    gtk_main_quit ();
    exit(1);
} /* destroy() */

/*
 * Calls function to create process window.
 */

static void processesCall (GtkWidget *widget, gpointer data)
{ 
  funkytown();
} /* processesCall() */


/*
 * Creates threads and buttons used to open windows with data from backend.
 */

static void activate(GtkApplication *app, gpointer user_data) {

  graphInit();
  
  pthread_t netThread;
  pthread_create(&netThread, NULL, getNetworkHistory, (void*)&netThread);
    
  pthread_t memThread;
  pthread_create(&memThread, NULL, getMemHistory, (void*)&memThread);
    
  pthread_t swapThread;
  pthread_create(&swapThread, NULL, getSwapHistory, (void*)&swapThread);
   
  pthread_t cpuThread;
  pthread_create(&cpuThread, NULL, getCpuHistory, (void*)&cpuThread);

  GtkWidget *window;
  GtkWidget *systemButton;
  GtkWidget *resourcesButton;
  GtkWidget *fileSystemsButton;
  GtkWidget *processesButton;
  GtkWidget *buttonBox;
  GtkWidget *title;
  GtkWidget *grid;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 550, 400);

  grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
  gtk_container_add(GTK_CONTAINER(window), grid);

  title = gtk_label_new(""); 
  gtk_grid_attach(GTK_GRID(grid), title, 0, 0, 1, 1);
  title = gtk_label_new(""); 
  gtk_grid_attach(GTK_GRID(grid), title, 0, 1, 1, 1); 
  title = gtk_label_new(""); 

  gtk_widget_set_hexpand (title, TRUE);
  gtk_label_set_markup(GTK_LABEL(title), "<span font_desc=\"55.0\" color=\"red\">Task Manager</span>");
  gtk_label_set_xalign(GTK_LABEL(title), .5);

  gtk_grid_attach(GTK_GRID(grid), title, 0, 2, 1, 1); 

  title = gtk_label_new("");
  gtk_widget_set_hexpand (title, TRUE);
  gtk_label_set_markup(GTK_LABEL(title), "<span font_desc=\"15.0\">By: Kyle Buzza, Chris Widenor, Isabel Battaglioli</span>");
  gtk_label_set_xalign(GTK_LABEL(title), .5);

  gtk_grid_attach(GTK_GRID(grid), title, 0, 3, 1, 1);

  title = gtk_label_new("");
  gtk_grid_attach(GTK_GRID(grid), title, 0, 4, 1, 1);
  title = gtk_label_new(""); 
  gtk_grid_attach(GTK_GRID(grid), title, 0, 5, 1, 1); 

  // buttonBox code from developer.gnome
  buttonBox = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_hexpand (buttonBox, TRUE); 
  gtk_grid_attach(GTK_GRID(grid), buttonBox, 0, 6, 1, 1);

  systemButton = gtk_button_new_with_label ("System");
  resourcesButton = gtk_button_new_with_label ("Resources");
  processesButton = gtk_button_new_with_label ("Processes");
  fileSystemsButton = gtk_button_new_with_label ("File Systems");

  text = malloc(sizeof(char) * 100);

  g_signal_connect (systemButton, "clicked", G_CALLBACK (startSystem), NULL);
  g_signal_connect (resourcesButton, "clicked", G_CALLBACK (initializeDrawing), NULL);
  g_signal_connect (processesButton, "clicked", G_CALLBACK (processesCall), user_data);
  g_signal_connect (fileSystemsButton, "clicked", G_CALLBACK (activateMount), NULL);

  // buttonBox code from developer.gnome
  gtk_container_add (GTK_CONTAINER (buttonBox), systemButton);
  gtk_container_add (GTK_CONTAINER (buttonBox), resourcesButton);
  gtk_container_add (GTK_CONTAINER (buttonBox), fileSystemsButton);
  gtk_container_add (GTK_CONTAINER (buttonBox), processesButton);

  // code from stackoverflow @ line 14
  g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);

  gtk_widget_show_all(window);

  gtk_main ();

} /* activate() */

/*
 * Creates application and calls function "activate".
 * Referenced "main" function from gnome.developer @ line 13 above.
 */

int main (int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new ("task.manager", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
} /* main() */
