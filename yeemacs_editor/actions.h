#ifndef ACTIONS_H
#define ACTIONS_H

#include <gtk/gtk.h>

void open_file_button_clicked(GtkWidget* widget, gpointer data);
void undo_editor_button_clicked(GtkWidget* widget, gpointer data);
void redo_editor_button_clicked(GtkWidget* widget, gpointer data);
void run_button_clicked(GtkWidget* widget, gpointer data);
void save_file_button_clicked(GtkWidget* widget, gpointer data);
void clear_editor_buffer(GObject* source_object, GAsyncResult* res, gpointer user_data);
void new_file_button_clicked(GtkWidget* widget, gpointer data);
void stop_button_clicked(GtkWidget* widget, gpointer data);
void reset_console_buffer(void);

#endif //ACTIONS_H