#include "file_systems.h"
#include "process_info.h"
#include "free.h"

// looked at and used code from 
// https://developer.gnome.org/gnome-devel-demos/stable/scrolledwindow.c.html.en
// for scrolled window

/*
 * Called by file "home.c".
 * Create and populates page that holds file_system data.
 * Implements scroll window.
 */

void activateMount(GtkApplication *app, gpointer user_data) {
  // Declare variables
  GtkWidget *window;
  GtkWidget *scrolledWindow;
  GtkWidget *grid;
  GtkWidget *label;

  // Create a window with a title, and a default size
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Mount");
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size (GTK_WINDOW (window), 900, 600);

  grid = gtk_grid_new();
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 3);

  // scrolled_window from developer.gnome
  scrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledWindow), 10);
  
  //char *format;
  label = gtk_label_new(NULL);
  char *format1 = g_strdup_printf("<b>%s</b>", "DEVICE");
  gtk_label_set_markup(GTK_LABEL(label), format1);
  gtk_grid_attach(GTK_GRID(grid), label, 1, 0, 1, 1);
  label = gtk_label_new(NULL);
  char *format2 = g_strdup_printf("<b>%s</b>", "DIRECTORY");
  gtk_label_set_markup(GTK_LABEL(label), format2);
  gtk_grid_attach(GTK_GRID(grid), label, 2, 0, 1, 1);
  label = gtk_label_new(NULL);
  char *format3 = g_strdup_printf("<b>%s</b>", "TYPE");
  gtk_label_set_markup(GTK_LABEL(label), format3);
  gtk_grid_attach(GTK_GRID(grid), label, 3, 0, 1, 1);
  label = gtk_label_new(NULL);
  char *format4 = g_strdup_printf("<b>%s</b>", "TOTAL");
  gtk_label_set_markup(GTK_LABEL(label), format4);
  gtk_grid_attach(GTK_GRID(grid), label, 4, 0, 1, 1);
  label = gtk_label_new(NULL);
  char *format5 = g_strdup_printf("<b>%s</b>", "FREE");
  gtk_label_set_markup(GTK_LABEL(label), format5);
  gtk_grid_attach(GTK_GRID(grid), label, 5, 0, 1, 1);
  label = gtk_label_new(NULL);
  char *format6 = g_strdup_printf("<b>%s</b>", "AVAILABLE");
  gtk_label_set_markup(GTK_LABEL(label), format6);
  gtk_grid_attach(GTK_GRID(grid), label, 6, 0, 1, 1);
  label = gtk_label_new(NULL);
  char *format7 = g_strdup_printf("<b>%s</b>", "USED");
  gtk_label_set_markup(GTK_LABEL(label), format7);
  gtk_grid_attach(GTK_GRID(grid), label, 7, 0, 1, 1);
 
  int column = 1;
  char *totalSpace = malloc(sizeof(char) * 100);
  char *freeSpace = malloc(sizeof(char) * 100);
  char *availableSpace = malloc(sizeof(char) * 100);
  char *usedSpace = malloc(sizeof(char) * 100);
  int row = 1;
  int mountCount;

  struct mount *temp = getMounts(&mountCount);
  for (int i = 0; i < mountCount; i++) {
    label = gtk_label_new(temp[i].device);
    gtk_grid_attach(GTK_GRID(grid), label, column, row, 1, 1);
    column++;
    label = gtk_label_new(temp[i].directory);
    gtk_grid_attach(GTK_GRID(grid), label, column, row, 1, 1);
    column++;
    label = gtk_label_new(temp[i].type);
    gtk_grid_attach(GTK_GRID(grid), label, column, row, 1, 1);
    column++;
    sprintf(totalSpace, "%ld Gib", temp[i].total);
    label = gtk_label_new(totalSpace);
    gtk_grid_attach(GTK_GRID(grid), label, column, row, 1, 1);
    column++;
    sprintf(freeSpace, "%ld Gib", temp[i].free);
    label = gtk_label_new(freeSpace);
    gtk_grid_attach(GTK_GRID(grid), label, column, row, 1, 1);
    column++;
    sprintf(availableSpace, "%ld Gib", temp[i].available);
    label = gtk_label_new(availableSpace);
    gtk_grid_attach(GTK_GRID(grid), label, column, row, 1, 1);
    column++;
    sprintf(usedSpace, "%ld Gib", temp[i].used);
    label = gtk_label_new(usedSpace);
    gtk_grid_attach(GTK_GRID(grid), label, column, row, 1, 1);
    column++; 
    column = 1;
    row++;
  }

  // following code with scrolledWindow from developer.gnome
  gtk_container_add (GTK_CONTAINER (scrolledWindow), grid);
  
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledWindow),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);

  gtk_container_add (GTK_CONTAINER (window), scrolledWindow);

  gtk_widget_show_all (window);

  freeMounts(temp, mountCount);
  free(totalSpace);
  free(availableSpace);
  free(freeSpace);
  free(usedSpace);

  totalSpace = NULL;
  availableSpace = NULL;
  freeSpace = NULL;
  usedSpace = NULL;

  g_free(format1);
  g_free(format2);
  g_free(format3);
  g_free(format4);
  g_free(format5);
  g_free(format6);
  g_free(format7);

  gtk_main();
  
} /* activateMount() */
