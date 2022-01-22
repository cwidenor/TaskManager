#include <gtk/gtk.h>

enum {
  NAME,
  STATUS,
  CPU,
  ID,
  MEMORY,
  USER,
  COLUMNS
};

enum {
  FNAME,
  START,
  END,
  FLAGS,
  OFFSET,
  DEV,
  INODE,
  SIZE,
  MCOLUMNS
} maps;

enum {
  FD,
  TYPE,
  OBJECT,
  FCOLUMNS
} files;

void funkytown();
static void setupTreeView (GtkWidget*);
static void setupMapTree (GtkWidget *);
static void setupFileTree (GtkWidget *);
void viewPopupMenuOnStop (GtkWidget *, gpointer);
void viewPopupMenuOnCont (GtkWidget *, gpointer);
void viewPopupMenuOnKill (GtkWidget *, gpointer);
void viewPopupMenuOnMap (GtkWidget *, gpointer);
void viewPopupMenuOnFile (GtkWidget *, gpointer);
void viewPopupMenuOnDetail (GtkWidget *, gpointer);
void viewPopupMenuOnAll (GtkWidget *, gpointer);
void viewPopupMenuOnUser (GtkWidget *, gpointer);
void viewPopupMenuOnRunning (GtkWidget *, gpointer);
void viewPopupMenuOnRefresh (GtkWidget *, gpointer);
void viewPopupMenu (GtkWidget *, GdkEventButton *, gpointer);
gboolean viewOnButtonPressed (GtkWidget *, GdkEventButton *, gpointer);
gboolean viewOnPopupMenu (GtkWidget *, gpointer);
void refresh();
void printText(char *, char *, GtkWidget *, int);

GtkTreeSelection *selection;
GtkWidget *treeview;
int currentProcessType;