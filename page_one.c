#include "page_one.h"
#include "process_info.h"
#include "free.h"

// looked at for reference: 
// https://developer.gnome.org/gtk3/stable/ch01s02.html

/*
 * Adds bolded text to grid passed in parameters.
 */

void print_bold(char *text, GtkWidget *grid, int row) {
  // print headers
  GtkWidget *myLabel;
  char *format;

  myLabel = gtk_label_new(NULL);
  format = g_strdup_printf("<b>%s</b>", text);
  gtk_label_set_markup(GTK_LABEL(myLabel), format);
  gtk_label_set_xalign(GTK_LABEL(myLabel), 0);
  gtk_grid_attach(GTK_GRID(grid), myLabel, 0, row, 1, 1);
  g_free(format);
} /* print_bold() */

/*
 * Adds regular text to the grid passed in parameters.
 */

void print_reg_text(char *text, GtkWidget *grid, int row) {
  // print normal text
  GtkWidget *myLabel;
  char *format;

  format = g_strdup_printf("%s", text);
  myLabel = gtk_label_new(format);
  gtk_label_set_xalign(GTK_LABEL(myLabel), 0);
  gtk_widget_set_margin_start(myLabel, 20);
  gtk_grid_attach(GTK_GRID(grid), myLabel, 0, row,  1, 1);
  g_free(format);
} /* print_reg_text() */

/* 
 * Creates and populates window with data about system.
 */

void startSystem(GtkApplication *app, gpointer thing){  
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *text;
  char *format;
  int row;

  // initializa window settings
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 550, 600);
  gtk_window_set_title(GTK_WINDOW(window), "System Tab");
  gtk_container_set_border_width(GTK_CONTAINER(window), 20);

  // gather information from system
  struct system info = getSystemInfo();
  char *name = strdup(info.OSVersion);
  char *oVersion = strchr(name, ' ');
  char *kVersion = strdup(info.kernelVersion);
  char *availableMem = malloc(sizeof(char) * 100);
  char *pVersion = malloc(sizeof(char) * (strlen(info.processorVersion) + 15));

  char *freeDisk = malloc(sizeof(char) * 100);
  sprintf(freeDisk, "Available Disk Space: %f Gib", info.availableDisk);
  sprintf(pVersion, "Processor: %s", info.processorVersion);
  sprintf(availableMem, "Memory: %d Gib", info.memory);
  oVersion[0] = 0;
  oVersion++; 

  // create grid system
  row = 0;
  grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 6);

  // print information
  print_bold("SYSTEM INFORMATION", grid, row);
  row++;
  print_bold(name, grid, row);
  row++;
  print_reg_text(oVersion, grid, row);
  row++;
  print_reg_text(kVersion, grid, row);
  row++;
  print_bold("Hardware", grid, row);
  row++;
  print_reg_text(availableMem, grid, row);
  row++;
  print_reg_text(pVersion, grid, row);
  row++;
  print_bold("System Status", grid, row);
  row++;
  print_reg_text(freeDisk, grid, row);
  
  // add information to window and close
  gtk_container_add(GTK_CONTAINER(window), grid);
  gtk_widget_show_all(window);

  freeSystemInfo(info);
  free(name);
  free(kVersion);
  free(availableMem);
  free(pVersion);
  free(freeDisk);

  name = NULL;
  kVersion = NULL;
  availableMem = NULL;
  pVersion = NULL;
  freeDisk = NULL;

  gtk_main(); 
} /* startSystem() */
