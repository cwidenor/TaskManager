#include <gtk/gtk.h>
#include <cairo.h>
#include "graphs.h"
#include "process_info.h"
#include "drawings.h"

extern int maxY;
char *text;

// looked at and used some code from 
// https://stackoverflow.com/questions/48833065/gtk3-and-cairo-g-timeout-add-doesnt-work

/*
 * Called in function "initializeDrawing".
 * Checks if window is still open and tells g_timeout_add 
 * whether or not to keep going.
 * Function layout from "timeout" function mentioned @ line 11 above.
 */

gboolean timeout(GtkWidget *widget) {
  if (GTK_IS_WIDGET(widget) == FALSE){
    return FALSE;
  } 
  gtk_widget_queue_draw(widget); 
  return TRUE;

} /* timeout() */

/*
 * Catches signal for closing window.
 */

void closeWindow(GtkWidget *widget, gpointer data) {

} /* closeWindow() */

/*
 * Called by "initializeDrawing" function.
 * Draws graphs for CPU, Network and Mem/Swap history.
 */

void drawGraph(GtkWidget* widget, cairo_t *cr) {
    // CPU HISTORY
    cairo_move_to(cr, 10.0, 20.0);
    cairo_show_text(cr, "CPU HISTORY");
    
    cairo_rectangle (cr, 10, 30, 530, 130);     
    cairo_set_source_rgb (cr, .85, .85, .85);   
    cairo_fill (cr);

    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_move_to (cr, 40.0, 140.0);
    cairo_line_to (cr, 530.0, 140.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 40.0, 140.0);
    cairo_line_to (cr, 40.0, 38.0);
    cairo_stroke(cr);

    // y axis lines
    cairo_set_source_rgb (cr, 0.9, 0.9, 0.9);
    cairo_move_to (cr, 41.0, 40.0);
    cairo_line_to (cr, 530.0, 40.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 41.0, 60.0);
    cairo_line_to (cr, 530.0, 60.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 41.0, 80.0);
    cairo_line_to (cr, 530.0, 80.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 41.0, 100.0);
    cairo_line_to (cr, 530.0, 100.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 41.0, 120.0);
    cairo_line_to (cr, 530.0, 120.0);
    cairo_stroke(cr);

    // x axis lines
    cairo_move_to (cr, 118.0, 139.0);
    cairo_line_to (cr, 118.0, 39.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 200.0, 139.0);
    cairo_line_to (cr, 200.0, 39.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 282.0, 139.0);
    cairo_line_to (cr, 282.0, 39.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 364.0, 139.0);
    cairo_line_to (cr, 364.0, 39.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 446.0, 139.0);
    cairo_line_to (cr, 446.0, 39.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 528.0, 139.0);
    cairo_line_to (cr, 528.0, 39.0);
    cairo_stroke(cr);

    // scale numbers
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

    // y axis numbers
    cairo_move_to(cr, 10.0, 43);
    cairo_show_text(cr, "100%");

    cairo_move_to(cr, 13.0, 63.0);
    cairo_show_text(cr, "80%");

    cairo_move_to(cr, 13.0, 83.0);
    cairo_show_text(cr, "60%");

    cairo_move_to(cr, 13.0, 103.0);
    cairo_show_text(cr, "40%");

    cairo_move_to(cr, 13.0, 123.0);
    cairo_show_text(cr, "20%");

    cairo_move_to(cr, 18.0, 140.0);
    cairo_show_text(cr, "0%");

    // x axis numbers
    cairo_move_to(cr, 40.0, 150.0);
    cairo_show_text(cr, "60 Seconds");

    cairo_move_to(cr, 117.0, 150.0);
    cairo_show_text(cr, "50");

    cairo_move_to(cr, 199.0, 150.0);
    cairo_show_text(cr, "40");

    cairo_move_to(cr, 281.0, 150.0);
    cairo_show_text(cr, "30");

    cairo_move_to(cr, 358.0, 150.0);
    cairo_show_text(cr, "20");

    cairo_move_to(cr, 440.0, 150.0);
    cairo_show_text(cr, "10");

    cairo_move_to(cr, 526.0, 150.0);
    cairo_show_text(cr, "0");

    cairo_set_source_rgb (cr, 0.7, 0.0, 0.0);
    cairo_move_to (cr, 40, 140 - (102 * (gCpuHist[0] / 100)));

    int firstX = 40;
    int firstY = 140 - (102 * (gCpuHist[0] / 100));
    int secondX = 0;
    int secondY = 0;
    int endX = 0;
    int endY = 0;
    int count = 1;

    for (int i = 41; i < 530; i+=8){
      endX = i;
      endY = 140 - (102 * (gCpuHist[count] / 100));
      secondX = firstX + ((endX - firstX) / 2);
      secondY = firstY + ((endY - firstY) / 2);
      cairo_curve_to(cr, firstX, firstY, secondX, secondY, endX, endY); 
      cairo_stroke(cr);
      cairo_move_to(cr, 60, 180);
      cairo_set_font_size(cr, 12);
      cairo_show_text(cr, "CPU Percent");
      cairo_move_to(cr, endX, endY); 
      cairo_set_font_size(cr, 9);
      firstX = endX;
      firstY = endY;
      count++; 
    } 

    //MEMORY AND SWAP HISTORY
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_move_to(cr, 10.0, 220.0);
    cairo_show_text(cr, "MEMORY AND SWAP HISTORY");

    cairo_rectangle (cr, 10, 230, 530, 130);  
    cairo_set_source_rgb (cr, .85, .85, .85);  
    cairo_fill (cr);

    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_move_to (cr, 40.0, 340.0);
    cairo_line_to (cr, 530.0, 340.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 40.0, 340.0);
    cairo_line_to (cr, 40.0, 238.0);
    cairo_stroke(cr);

    // y axis lines
    cairo_set_source_rgb (cr, 0.9, 0.9, 0.9);
    cairo_move_to (cr, 41.0, 240.0);
    cairo_line_to (cr, 530.0, 240.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 41.0, 260.0);
    cairo_line_to (cr, 530.0, 260.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 41.0, 280.0);
    cairo_line_to (cr, 530.0, 280.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 41.0, 300.0);
    cairo_line_to (cr, 530.0, 300.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 41.0, 320.0);
    cairo_line_to (cr, 530.0, 320.0);
    cairo_stroke(cr);
 

    // x axis lines
    cairo_move_to (cr, 118.0, 339.0);
    cairo_line_to (cr, 118.0, 239.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 200.0, 339.0);
    cairo_line_to (cr, 200.0, 239.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 282.0, 339.0);
    cairo_line_to (cr, 282.0, 239.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 364.0, 339.0);
    cairo_line_to (cr, 364.0, 239.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 446.0, 339.0);
    cairo_line_to (cr, 446.0, 239.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 528.0, 339.0);
    cairo_line_to (cr, 528.0, 239.0);
    cairo_stroke(cr);

    // y axis numbers
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_move_to(cr, 10.0, 243);
    cairo_show_text(cr, "100%");

    cairo_move_to(cr, 13.0, 263.0);
    cairo_show_text(cr, "80%");

    cairo_move_to(cr, 13.0, 283.0);
    cairo_show_text(cr, "60%");

    cairo_move_to(cr, 13.0, 303.0);
    cairo_show_text(cr, "40%");

    cairo_move_to(cr, 13.0, 323.0);
    cairo_show_text(cr, "20%");

    cairo_move_to(cr, 18.0, 340.0);
    cairo_show_text(cr, "0%");

    // x axis numbers
    cairo_move_to(cr, 40.0, 350.0);
    cairo_show_text(cr, "60 Seconds");

    cairo_move_to(cr, 117.0, 350.0);
    cairo_show_text(cr, "50");

    cairo_move_to(cr, 199.0, 350.0);
    cairo_show_text(cr, "40");

    cairo_move_to(cr, 281.0, 350.0);
    cairo_show_text(cr, "30");

    cairo_move_to(cr, 358.0, 350.0);
    cairo_show_text(cr, "20");

    cairo_move_to(cr, 440.0, 350.0);
    cairo_show_text(cr, "10");

    cairo_move_to(cr, 526.0, 350.0);
    cairo_show_text(cr, "0");

    cairo_set_source_rgb (cr, 0.0, 0.8, 0.0);
    cairo_move_to (cr, 40, 340 - (102 * (gMemHist[0] / 100)));

    firstX = 40;
    firstY = 340 - (102 * (gMemHist[0] / 100));
    secondX = 0;
    secondY = 0;
    endX = 0;
    endY = 0;

    count = 1;

    for (int i = 41; i < 530; i+=8){ 
      endX = i;
      endY = 340 - (102 * (gMemHist[count] / 100));
      secondX = firstX + ((endX - firstX) / 2);
      secondY = firstY + ((endY - firstY) / 2);
      cairo_curve_to(cr, firstX, firstY, secondX, secondY, endX, endY); 
      cairo_stroke(cr);
      cairo_move_to(cr, 60, 380);
      cairo_set_font_size(cr, 12);
      cairo_show_text(cr, "Memory Percent");
      cairo_move_to(cr, endX, endY); 
      cairo_set_font_size(cr, 9);
      firstX = endX;
      firstY = endY;
      count++; 
    }  

    cairo_set_source_rgb (cr, 0.0, 0.0, 0.9);
    cairo_move_to (cr, 40, 340 - (102 * (gSwapHist[0] / 100)));

    firstX = 40;
    firstY = 340 - (102 * (gSwapHist[0] / 100));
    secondX = 0;
    secondY = 0;
    endX = 0;
    endY = 0;

    count = 1;

    for (int i = 41; i < 530; i+=8){
      endX = i;
      endY = 340 - (102 * (gSwapHist[count] / 100));
      secondX = firstX + ((endX - firstX) / 2);
      secondY = firstY + ((endY - firstY) / 2);
      cairo_curve_to(cr, firstX, firstY, secondX, secondY, endX, endY); 
      cairo_stroke(cr);
      cairo_move_to(cr, 180, 380);
      cairo_set_font_size(cr, 12);
      cairo_show_text(cr, "Swap Percent");
      cairo_move_to(cr, endX, endY); 
      cairo_set_font_size(cr, 9);
      firstX = endX;
      firstY = endY;
      count++; 
    }  

    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

    //NETWORK HISTORY
    cairo_move_to(cr, 10.0, 420.0);
    cairo_show_text(cr, "NETWORK HISTORY (measured in KiB/s)");

    cairo_rectangle (cr, 10, 430, 530, 130);    
    cairo_set_source_rgb (cr, .85, .85, .85);  
    cairo_fill (cr);

    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_move_to (cr, 40.0, 540.0);
    cairo_line_to (cr, 530.0, 540.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 40.0, 540.0);
    cairo_line_to (cr, 40.0, 438.0);
    cairo_stroke(cr);

    // y axis lines
    cairo_set_source_rgb (cr, 0.9, 0.9, 0.9);
    cairo_move_to (cr, 41.0, 440.0);
    cairo_line_to (cr, 530.0, 440.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 41.0, 460.0);
    cairo_line_to (cr, 530.0, 460.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 41.0, 480.0);
    cairo_line_to (cr, 530.0, 480.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 41.0, 500.0);
    cairo_line_to (cr, 530.0, 500.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 41.0, 520.0);
    cairo_line_to (cr, 530.0, 520.0);
    cairo_stroke(cr);
 
    // x axis lines
    cairo_move_to (cr, 118.0, 539.0);
    cairo_line_to (cr, 118.0, 439.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 200.0, 539.0);
    cairo_line_to (cr, 200.0, 439.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 282.0, 539.0);
    cairo_line_to (cr, 282.0, 439.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 364.0, 539.0);
    cairo_line_to (cr, 364.0, 439.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 446.0, 539.0);
    cairo_line_to (cr, 446.0, 439.0);
    cairo_stroke(cr);

    cairo_move_to (cr, 528.0, 539.0);
    cairo_line_to (cr, 528.0, 439.0);
    cairo_stroke(cr);

    // y axis numbers
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_move_to(cr, 15.0, 443);
    int num = maxY / 1000;
    sprintf(text, "%dk", num);
    cairo_show_text(cr, text);

    num = (maxY / 5) * 4;
    num = num / 1000;
    sprintf(text, "%dk", num);
    cairo_move_to(cr, 15.0, 463.0);
    cairo_show_text(cr, text);

    num = (maxY / 5) * 3;
    num = num / 1000;
    sprintf(text, "%dk", num);
    cairo_move_to(cr, 15.0, 483.0);
    cairo_show_text(cr, text);

    num = (maxY / 5) * 2;
    num = num / 1000;
    sprintf(text, "%dk", num);
    cairo_move_to(cr, 17.0, 503.0);
    cairo_show_text(cr, text);

    num = (maxY / 5) * 1;
    num = num / 1000;
    sprintf(text, "%dk", num); 
    cairo_move_to(cr, 17.0, 523.0);
    cairo_show_text(cr, text);

    cairo_move_to(cr, 17.0, 540.0);
    cairo_show_text(cr, "0k"); 
   
    // x axis numbers
    cairo_move_to(cr, 40.0, 550.0);
    cairo_show_text(cr, "60 Seconds");

    cairo_move_to(cr, 117.0, 550.0);
    cairo_show_text(cr, "50");

    cairo_move_to(cr, 199.0, 550.0);
    cairo_show_text(cr, "40");

    cairo_move_to(cr, 281.0, 550.0);
    cairo_show_text(cr, "30");

    cairo_move_to(cr, 358.0, 550.0);
    cairo_show_text(cr, "20");

    cairo_move_to(cr, 440.0, 550.0);
    cairo_show_text(cr, "10");

    cairo_move_to(cr, 526.0, 550.0);
    cairo_show_text(cr, "0");
 
	  cairo_set_source_rgb (cr, 0.4, 0.5, 0.0);
    
    cairo_move_to(cr, 40, 539);
    firstX = 40;
    firstY = 539;
    secondX = 0;
    secondY = 0;
    endX = 0;
    endY = 0;

    count = 1; 
    for (int i = 41; i < 530; i+=8){ 
      endX = i;
      if ((gNetSend[count] / maxY) > 1){
        maxY = gNetSend[count];
      }
      if (maxY < 20000){
        maxY = 20000;
      }
      
      endY = 540 - (102 * (gNetSend[count] / maxY));
      secondX = firstX + ((endX - firstX) / 2);
      secondY = firstY + ((endY - firstY) / 2);
      cairo_curve_to(cr, firstX, firstY, secondX, secondY, endX, endY); 
      cairo_stroke(cr);
      cairo_move_to(cr, 60, 580);
      cairo_set_font_size(cr, 12);
      cairo_show_text(cr, "Net Sent");
      cairo_move_to(cr, endX, endY); 
      cairo_set_font_size(cr, 9);
      firstX = endX;
      firstY = endY;
      count++; 
    }  

    cairo_set_source_rgb (cr, 0.6, 0.2, 0.8);

    cairo_move_to(cr, 40, 540);
    firstX = 40;
    firstY = 540;
    secondX = 0;
    secondY = 0;
    endX = 0;
    endY = 0;

    count = 1;

    for (int i = 41; i < 530; i+=8){
      endX = i;
      if ((gNetRecieve[count] / maxY) > 1){
        maxY = gNetRecieve[count];
      } 
      if (maxY < 20000){
        maxY = 20000;
      }
      endY = 540 - (102 * (gNetRecieve[count] / maxY)); 
      secondX = firstX + ((endX - firstX) / 2);
      secondY = firstY + ((endY - firstY) / 2);
      cairo_curve_to(cr, firstX, firstY, secondX, secondY, endX, endY); 
      cairo_stroke(cr);
      cairo_move_to(cr, 140, 580);
      cairo_set_font_size(cr, 12);
      cairo_show_text(cr, "Net Recieved");
      cairo_move_to(cr, endX, endY); 
      cairo_set_font_size(cr, 9);
      firstX = endX;
      firstY = endY;
      count++; 
    }  

    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0); 
} /* drawGraph() */

/*
 * Called by home.c
 * Create window and draw area then starts drawing process.
 * Roughly follows "main" function from stackoverflow @ line 11 above.
 */

void initializeDrawing(GtkApplication *app, gpointer user_data) {
    GtkWidget* window;
    GtkWidget* darea;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    darea = gtk_drawing_area_new();

    gtk_container_add(GTK_CONTAINER(window), darea);

    gtk_window_set_default_size(GTK_WINDOW(window), 550, 600);

    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(drawGraph), NULL);
 
    g_timeout_add(1000, (GSourceFunc)timeout, window);
    
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(closeWindow), NULL);
 
    gtk_widget_show_all(window);

    gtk_main();
} /* initializeDrawing() */
