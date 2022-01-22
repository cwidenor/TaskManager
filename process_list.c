#include "process_list.h"
#include "process_info.h"
#include "free.h"
#include "process_actions.h"
#include "home.h"

/*
 * Based on: https://www.linuxquestions.org/questions/programming-9/how-to-create-a-table-using-gtk-720323/
 * This is the primary function for the entirety of the process listing
 * It creates the process listing window and signals to make the popup menu startable
 */
void funkytown() {
  currentProcessType = 0;
  GtkWidget *window, *scrolledWin;
  GtkListStore *store;
  GtkTreeIter iter;
  int i = 0;
  int count = 0;
  
  struct process *processList = getProcessList(currentProcessType, &count);
  
  // Create a new tree model
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Process Listing");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_widget_set_size_request (window, 650, 400);
  treeview = gtk_tree_view_new ();
  setupTreeView (treeview);
  store = gtk_list_store_new (COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_DOUBLE, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
  
  // Add all of the products to the GtkListStore.
  while (i < count) {
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, NAME, processList[i].name,
    STATUS, processList[i].status, CPU, processList[i].cpuPercent, ID, processList[i].id, MEMORY , processList[i].memory, USER, processList[i].user, -1);
    i++;
  }

  // Add the tree model to the tree view and unreference it so that the model will be destroyed along with the tree view.
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
  g_object_unref (store);
  scrolledWin = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledWin),
  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (scrolledWin), treeview);
  gtk_container_add (GTK_CONTAINER (window), scrolledWin);
  g_signal_connect(treeview, "button-press-event", (GCallback) viewOnButtonPressed, NULL);
  g_signal_connect(treeview, "popup-menu", (GCallback) viewOnPopupMenu, NULL);
  gtk_widget_show_all (window);
  gtk_main ();

  return;
}

/*
 * This function is based on: https://www.linuxquestions.org/questions/programming-9/how-to-create-a-table-using-gtk-720323/
 * It creates the columns of the process list tree view and makes them sortable
 */
static void setupTreeView (GtkWidget *treeview) {
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  // Create a new GtkCellRendererText, add it to the tree view column and append the column to the tree view.
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("Process Name", renderer, "text", NAME, NULL);
  gtk_tree_view_column_set_sort_column_id(column, NAME);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("Status", renderer, "text", STATUS, NULL);
  gtk_tree_view_column_set_sort_column_id(column, STATUS);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("CPU Percent", renderer, "text", CPU, NULL);
  gtk_tree_view_column_set_sort_column_id(column, CPU);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("ID", renderer, "text", ID, NULL);
  gtk_tree_view_column_set_sort_column_id(column, ID);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("Memory", renderer, "text", MEMORY, NULL);
  gtk_tree_view_column_set_sort_column_id(column, MEMORY);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("User", renderer, "text", USER, NULL);
  gtk_tree_view_column_set_sort_column_id(column, USER);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
}

/*
 * This function is the callback handler for if "Stop" is selected from the popup menu
 * It stops the currently selected process then refreshes the process list
 */
void viewPopupMenuOnStop(GtkWidget *menuitem, gpointer userdata) {
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gint task_id;
  GtkTreeModel *model;
  GtkTreeIter iter;
  
  if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
    gtk_tree_model_get(model, &iter, ID, &task_id, -1);
    if (task_id == getpid()) {
      //g_print(("Can't stop process self"));
    } else {
      stopProc(task_id);
      refresh();
      //g_print("Process stopped\n");
    }
  }
  //g_print ("Do something!\n");
} /* view_popup_menu_on_stop() */

/*
 * This function is the callback handler for if "Continue" is selected from the popup menu
 * It continues the currently selected process then refreshes the process list
 */
void viewPopupMenuOnCont(GtkWidget *menuitem, gpointer userdata) {
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gint taskId;
  GtkTreeModel *model;
  GtkTreeIter iter;
  
  if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
    gtk_tree_model_get(model, &iter, ID, &taskId, -1);
    if (taskId == getpid()) {
      //g_print(("Can't stop process self"));
    } else {
      continueProc(taskId);
      refresh();
      //g_print("Process continued\n");
    }
  }
  //g_print ("Do something!\n");
} /* view_popup_menu_on_cont() */

/*
 * This function is the callback handler for if "Kill" is selected from the popup menu
 * It kills the currently selected process then refreshes the process list
 */
void viewPopupMenuOnKill(GtkWidget *menuitem, gpointer userdata) {
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gint taskId;
  GtkTreeModel *model;
  GtkTreeIter iter;
  if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
    gtk_tree_model_get(model, &iter, ID, &taskId, -1);
    if (taskId == getpid()) {
      //g_print(("Can't stop process self"));
    } else {
      killProc(taskId);
      refresh();
      //g_print("Process killed\n");
    }
  }
  //g_print ("Do something!\n");
} /* view_popup_menu_on_kill() */

/*
 * This function is the callback handler for if "Memory Maps" is selected from the popup menu
 * It creates a window that displays the memory map of the currently selected process
 */
void viewPopupMenuOnMap(GtkWidget *menuitem, gpointer userdata) {
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

  gint taskId;
  GtkTreeModel *model;
  GtkTreeIter iter;
  if (!gtk_tree_selection_get_selected(selection, &model, &iter))
  {
    return;
  }
  gtk_tree_model_get(model, &iter, ID, &taskId, -1);
  //g_print("%i", taskId);

  GtkWidget *mapWindow, *mapScroll, *mapTreeView;
  GtkListStore *mapStore;
  GtkTreeIter mapIter;

  int count = 0;
  int i = 0;
  struct memMap *mapList = getMemMap(taskId, &count); 
  
  mapWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (mapWindow), "Memory Map");
  gtk_container_set_border_width (GTK_CONTAINER (mapWindow), 10);
  gtk_widget_set_size_request (mapWindow, 1200, 500);
  mapTreeView = gtk_tree_view_new ();
  setupMapTree (mapTreeView);
  
  // Create a new tree model with three columns, as string, gint and guint.
  mapStore = gtk_list_store_new (MCOLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
  
  // Add all of the products to the GtkListStore.
  while (i < count) {
    gtk_list_store_append (mapStore, &mapIter);
    gtk_list_store_set (mapStore, &mapIter, FNAME, mapList[i].filename,
    START, mapList[i].VMStart, END, mapList[i].VMEnd, FLAGS , mapList[i].flags, OFFSET, mapList[i].VMOffset, DEV, mapList[i].dev, INODE, mapList[i].inode, SIZE, mapList[i].VMSize, -1);
    i++;
  }

  // Add the tree model to the tree view and unreference it so that the model will be destroyed along with the tree view.
  gtk_tree_view_set_model (GTK_TREE_VIEW (mapTreeView), GTK_TREE_MODEL (mapStore));
  g_object_unref (mapStore);
  mapScroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (mapScroll),
  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (mapScroll), mapTreeView);
  gtk_container_add (GTK_CONTAINER (mapWindow), mapScroll);
  gtk_widget_show_all (mapWindow);

  freeMemMap(mapList, count);

  //g_print ("Do something!\n");
} /* view_popup_menu_on_map() */

/*
 * This function is the callback handler for if "Open Files" is selected from the popup menu
 * It creates a new window that diplays the files opened by the selected process
 */
void viewPopupMenuOnFile(GtkWidget *menuitem, gpointer userdata) {
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gint taskId;
  GtkTreeModel *model;
  GtkTreeIter iter;
  if (!gtk_tree_selection_get_selected(selection, &model, &iter))
  {
    return;
  }
  gtk_tree_model_get(model, &iter, ID, &taskId, -1);
  GtkWidget *fileWindow, *fileScroll, *fileTreeView;
  GtkListStore *fileStore;
  GtkTreeIter fileIter;

  int count = 0;
  int i = 0;
  struct pFile *fileList = getProcessFiles(taskId, &count); 

  fileWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (fileWindow), "Open files");
  gtk_container_set_border_width (GTK_CONTAINER (fileWindow), 10);
  gtk_widget_set_size_request (fileWindow, 550, 500);
  fileTreeView = gtk_tree_view_new ();
  setupFileTree (fileTreeView);

  // Create a new tree model with three columns, as string, gint and guint.
  fileStore = gtk_list_store_new (FCOLUMNS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);

  // Add all of the products to the GtkListStore.
  while (i < count) {
    gtk_list_store_append (fileStore, &fileIter);
    gtk_list_store_set (fileStore, &fileIter, FD, fileList[i].fd, TYPE, fileList[i].type, OBJECT, fileList[i].object, -1);
    i++;
  }

  // Add the tree model to the tree view and unreference it so that the model will be destroyed along with the tree view.
  gtk_tree_view_set_model (GTK_TREE_VIEW (fileTreeView), GTK_TREE_MODEL (fileStore));
  g_object_unref (fileStore);
  fileScroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (fileScroll),
  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (fileScroll), fileTreeView);
  gtk_container_add (GTK_CONTAINER (fileWindow), fileScroll);
  gtk_widget_show_all (fileWindow);

  freeProcFiles(fileList, count);

  //g_print ("Do something!\n");
} /* view_popup_menu_on_file() */

/*
 * This function is the callback handler for if "Detailed View" is selected from the popup menu
 * It creates a new window that displays the details of the selected process
 */
void viewPopupMenuOnDetail(GtkWidget *menuitem, gpointer userdata) {
  struct process p;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  GtkTreeModel *model;
  GtkTreeIter iter;

  if (!gtk_tree_selection_get_selected(selection, &model, &iter))
    return;
  gtk_tree_model_get(model, &iter, NAME, &p.name, -1);
  gtk_tree_model_get(model, &iter, STATUS, &p.status, -1);
  gtk_tree_model_get(model, &iter, CPU, &p.cpuPercent, -1);
  gtk_tree_model_get(model, &iter, ID, &p.id, -1);
  gtk_tree_model_get(model, &iter, MEMORY, &p.memory, -1);
  gtk_tree_model_get(model, &iter, USER, &p.user, -1);
  GtkWidget *detailWindow, *vbox, *text;

  char *format;
  int row;
  detailWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_position(GTK_WINDOW(detailWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(detailWindow), 550, 600);
  gtk_window_set_title(GTK_WINDOW(detailWindow), "Process Properties");
  gtk_container_set_border_width(GTK_CONTAINER(detailWindow), 20);

  struct detailView details = getProcessDetails(p);
  char *name = details.procName;
  char *user = details.user;
  char *state = details.status;
  char *memory = details.memory;
  char *vMemory = details.vMemory;
  char *rMemory = details.rMemory;
  char *sMemory = details.sMemory;
  char *time = details.cpuTime;
  char *start = details.startTime;

  row = 0;
  vbox = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(vbox), 6);
  printText("Process Name: ", name, vbox, row);
  row++;
  printText("User: ", user, vbox, row);
  row++;
  printText("Status: ", state, vbox, row);
  row++;
  printText("Memory: ", memory, vbox, row);
  row++;
  printText("Virtual Memory: ", vMemory, vbox, row);
  row++;
  printText("Resident Memory: ", rMemory, vbox, row);
  row++;
  printText("Shared Memory: ", sMemory, vbox, row);
  row++;
  printText("CPU Time: ", time, vbox, row);
  row++;
  printText("Started: ", start, vbox, row);

  gtk_container_add(GTK_CONTAINER(detailWindow), vbox);
  gtk_widget_show_all(detailWindow);

  freeDetailView(details);

  //g_print ("Do something!\n");
} /* view_popup_menu_on_detail() */

/*
 * This function is the callback handler for if "Show All Processes" is selected from the popup menu
 * It changes the currentProcessType then calls refresh in order to show all of the processes
 */
void viewPopupMenuOnAll (GtkWidget *menuitem, gpointer userdata) {
  currentProcessType = 0;
  refresh();
  //g_print ("Listing all processes\n");
} /* view_popup_menu_on_all() */

/*
 * This function is the callback handler for if "Show User Processes" is selected from the popup menu
 * It changes the currentProcessType then calls refresh in order to show only the user processes
 */
void viewPopupMenuOnUser(GtkWidget *menuitem, gpointer userdata) {
  currentProcessType = 1;
  refresh();
  //g_print ("Listing user processes\n");
} /* view_popup_menu_on_user() */

/*
 * This function is the callback handler for if "Show Running Processes" is selected from the popup menu
 * It changes the currentProcessType then calls refresh in order to show only the running processes
 */
void viewPopupMenuOnRunning (GtkWidget *menuitem, gpointer userdata) {
  currentProcessType = 2;
  refresh();
  //g_print ("Listing running processes\n");
} /* view_popup_menu_on_running() */

/*
 * This function is the callback handler for if "Refresh" is selected from the popup menu
 * It calls refresh upon selection
 */
void viewPopupMenuOnRefresh (GtkWidget *menuitem, gpointer userdata) {
  refresh();
  //g_print ("Refreshing\n");
} /* view_popup_menu_on_refresh() */

/*
 * Outline for this function based on: https://people.gnome.org/~csaavedra/documents/treeview-tutorial/sec-selections-context-menus.html
 * The subsequent callback handlers are all partially based on that too
 * This function creates the popup menu that you see when the treeview is right clicked, and connects each button to its respective handler
 */
void viewPopupMenu(GtkWidget *treeview, GdkEventButton *event, gpointer userdata) {
  GtkWidget *menu, *stopMi, *contMi, *killMi, *mapMi, *fileMi, *detailMi, *viewMi;
  GtkWidget *sep1, *allMi, *userMi, *runningMi, *viewMenu, *sep2, *refMi, *loadMi;
  menu = gtk_menu_new();

  stopMi = gtk_menu_item_new_with_label("Stop");
  contMi = gtk_menu_item_new_with_label("Continue");
  killMi = gtk_menu_item_new_with_label("Kill");
  mapMi = gtk_menu_item_new_with_label("Memory Maps");
  fileMi = gtk_menu_item_new_with_label("Open Files");
  detailMi = gtk_menu_item_new_with_label("Detailed View");
  viewMi = gtk_menu_item_new_with_label("View");
  sep1 = gtk_separator_menu_item_new();
  allMi = gtk_menu_item_new_with_label("Show All Processes");
  userMi = gtk_menu_item_new_with_label("Show Current User Processes");
  runningMi = gtk_menu_item_new_with_label("Show Running Processes");
  sep2 = gtk_separator_menu_item_new();
  refMi = gtk_menu_item_new_with_label("Refresh");
  loadMi = gtk_menu_item_new_with_label(getLoadAvgString());

  g_signal_connect(stopMi, "activate",
                   (GCallback) viewPopupMenuOnStop, treeview);
  g_signal_connect(contMi, "activate",
                   (GCallback) viewPopupMenuOnCont, treeview);
  g_signal_connect(killMi, "activate",
                   (GCallback) viewPopupMenuOnKill, treeview);
  g_signal_connect(mapMi, "activate",
                   (GCallback) viewPopupMenuOnMap, treeview);
  g_signal_connect(fileMi, "activate",
                   (GCallback) viewPopupMenuOnFile, treeview);
  g_signal_connect(detailMi, "activate",
                   (GCallback) viewPopupMenuOnDetail, treeview);
  g_signal_connect(refMi, "activate",
                    (GCallback) viewPopupMenuOnRefresh, treeview);

  gtk_menu_shell_append(GTK_MENU_SHELL(menu), stopMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), contMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), killMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), mapMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), fileMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), detailMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), sep1);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), viewMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), sep2);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), refMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), loadMi);

  viewMenu = gtk_menu_new();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM(viewMi), viewMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), allMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), userMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(viewMenu), runningMi);
  g_signal_connect(allMi, "activate",
                   (GCallback) viewPopupMenuOnAll, treeview);
  g_signal_connect(userMi, "activate",
                   (GCallback) viewPopupMenuOnUser, treeview);
  g_signal_connect(runningMi, "activate",
                   (GCallback) viewPopupMenuOnRunning, treeview);
  gtk_widget_show_all(menu);

  // Note: event can be NULL here when called from view_onPopupMenu; gdk_event_get_time() accepts a NULL argument
  gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
                 (event != NULL) ? event->button : 0,
                 gdk_event_get_time((GdkEvent*)event));
} /* view_popup_menu() */

/*
 * Taken from: https://people.gnome.org/~csaavedra/documents/treeview-tutorial/sec-selections-context-menus.html
 * This function checks button presses and calls view_popup_menu if it was a right click
 */
gboolean viewOnButtonPressed(GtkWidget *treeview, GdkEventButton *event, gpointer userdata) {
  // single click with the right mouse button
  if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3) {
    //g_print ("Single right click on the tree view.\n");
    if (1) {
      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
      if (gtk_tree_selection_count_selected_rows(selection)  <= 1) {
         GtkTreePath *path;
         if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview),
                                           event->x, event->y,
                                           &path, NULL, NULL, NULL)) {
           gtk_tree_selection_unselect_all(selection);
           gtk_tree_selection_select_path(selection, path);
           gtk_tree_path_free(path);
         }
      }
    }
    viewPopupMenu(treeview, event, userdata);
    return TRUE; /* we handled this */
  }
  return FALSE; /* we did not handle this */
} /* view_onButtonPressed() */

/*
 * Taken from: https://people.gnome.org/~csaavedra/documents/treeview-tutorial/sec-selections-context-menus.html
 * This function is the handler to call view_popup_menu
 */
gboolean viewOnPopupMenu(GtkWidget *treeview, gpointer userdata) {
  viewPopupMenu(treeview, NULL, userdata);
  return TRUE;
} /* view_onPopupMenu() */

/*
 * This function refreshes the store of values in the process listing
 */
void refresh() {
  GtkListStore *store;
  GtkTreeIter iter;
  int i = 0;
  int count = 0;
  struct process *processList = getProcessList(currentProcessType, &count);
  store = gtk_list_store_new (COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_DOUBLE, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
  while (i < count) {
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, NAME, processList[i].name,
    STATUS, processList[i].status, CPU, processList[i].cpuPercent, ID, processList[i].id, MEMORY , processList[i].memory, USER, processList[i].user,-1);
    i++;
  }
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
  g_object_unref (store);
} /* refresh() */

/*
 * This function creates the columns of the treeview made when "Memory Maps" is selected 
 */
static void setupMapTree(GtkWidget *mapTreeView) {
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  // Create a new GtkCellRendererText, add it to the tree view column and append the column to the tree view.
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("Filename", renderer, "text", FNAME, NULL);
  gtk_tree_view_column_set_sort_column_id(column, FNAME);
  gtk_tree_view_append_column (GTK_TREE_VIEW (mapTreeView), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("VM Start", renderer, "text", START, NULL);
  gtk_tree_view_column_set_sort_column_id(column, START);
  gtk_tree_view_append_column (GTK_TREE_VIEW (mapTreeView), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("VM End", renderer, "text", END, NULL);
  gtk_tree_view_column_set_sort_column_id(column, END);
  gtk_tree_view_append_column (GTK_TREE_VIEW (mapTreeView), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("Flags", renderer, "text", FLAGS, NULL);
  gtk_tree_view_column_set_sort_column_id(column, FLAGS);
  gtk_tree_view_append_column (GTK_TREE_VIEW (mapTreeView), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("VM Offset", renderer, "text", OFFSET, NULL);
  gtk_tree_view_column_set_sort_column_id(column, OFFSET);
  gtk_tree_view_append_column (GTK_TREE_VIEW (mapTreeView), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("Dev", renderer, "text", DEV, NULL);
  gtk_tree_view_column_set_sort_column_id(column, DEV);
  gtk_tree_view_append_column (GTK_TREE_VIEW (mapTreeView), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("Inode", renderer, "text", INODE, NULL);
  gtk_tree_view_column_set_sort_column_id(column, INODE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (mapTreeView), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("VM Size (KiB)", renderer, "text", SIZE, NULL);
  gtk_tree_view_column_set_sort_column_id(column, SIZE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (mapTreeView), column);
} /* setup_map_tree() */

/*
 * This function creates the columns for the treeview when "Open Files" is selected
 */
static void setupFileTree(GtkWidget *fileTreeView) {
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("FD", renderer, "text", FD, NULL);
  gtk_tree_view_column_set_sort_column_id(column, FD);
  gtk_tree_view_append_column (GTK_TREE_VIEW (fileTreeView), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("Type", renderer, "text", TYPE, NULL);
  gtk_tree_view_column_set_sort_column_id(column, TYPE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (fileTreeView), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes
  ("Object", renderer, "text", OBJECT, NULL);
  gtk_tree_view_column_set_sort_column_id(column, OBJECT);
  gtk_tree_view_append_column (GTK_TREE_VIEW (fileTreeView), column);
} /* setup_file_tree() */

/*
 * This function prints all of the text seen in a processes detailed view
 */
void printText(char *text, char *value, GtkWidget *grid, int row) {
  GtkWidget *myLabel;
  char *format;
  format = g_strdup_printf("%s%s", text, value);
  myLabel = gtk_label_new(format);
  gtk_label_set_xalign(GTK_LABEL(myLabel), 0);
  gtk_widget_set_margin_start(myLabel, 20);
  gtk_grid_attach(GTK_GRID(grid), myLabel, 0, row,  1, 1);
  g_free(format);
} /* print_text() */