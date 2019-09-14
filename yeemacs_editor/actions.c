#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "actions.h"
#include "editor_state.h"

void save_to_file(gpointer data);

void clear_buffer(GtkTextBuffer* text_buffer) {
    GtkTextIter start_iter;
    gtk_text_buffer_get_start_iter(text_buffer, &start_iter);
    GtkTextIter end_iter;
    gtk_text_buffer_get_end_iter(text_buffer, &end_iter);
    gtk_text_buffer_delete(text_buffer, &start_iter, &end_iter);
}

void text_buffer_append_text(GtkTextBuffer* text_buffer, const char* text) {
    GtkTextIter end_iter;
    gtk_text_buffer_get_end_iter(text_buffer, &end_iter);
    gtk_text_buffer_insert(text_buffer, &end_iter, text, -1);
}

void open_file_button_clicked(GtkWidget* widget, gpointer data) {
    EditorState* editor_state = get_editor_state();
    GtkWindow* parent_window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    GtkSourceBuffer* editor_buffer = editor_state->editor_buffer;
    GtkFileChooserDialog* file_chooser_dialog;
    file_chooser_dialog = GTK_FILE_CHOOSER_DIALOG(
            gtk_file_chooser_dialog_new(
                    "Open File",
                    parent_window,
                    GTK_FILE_CHOOSER_ACTION_OPEN,
                    ("_Cancel"),
                    GTK_RESPONSE_CANCEL,
                    ("_Open"),
                    GTK_RESPONSE_ACCEPT,
                    NULL));
    GtkFileChooser* chooser = GTK_FILE_CHOOSER(file_chooser_dialog);
    gint res = gtk_dialog_run(GTK_DIALOG(file_chooser_dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        GFile* file = gtk_file_chooser_get_file(chooser);
        g_free(editor_state->parse_name);
        editor_state->parse_name = g_file_get_parse_name(file);
                //g_file_get_basename(file);
        GtkSourceFile* source_file = gtk_source_file_new();
        gtk_source_file_set_location(source_file, file);
        GtkSourceFileLoader* file_loader = gtk_source_file_loader_new(editor_buffer, source_file);
        gtk_source_file_loader_load_async(
                file_loader,
                G_PRIORITY_HIGH,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL);
        gtk_source_file_loader_load_finish(file_loader, NULL, NULL);
        g_object_unref(file);
    }
    gtk_widget_destroy(GTK_WIDGET(file_chooser_dialog));
}

void undo_editor_button_clicked(GtkWidget* widget, gpointer data) {
    EditorState* editor_state = get_editor_state();
    gtk_source_buffer_undo(editor_state->editor_buffer);
}

void redo_editor_button_clicked(GtkWidget* widget, gpointer data) {
    EditorState* editor_state = get_editor_state();
    gtk_source_buffer_redo(editor_state->editor_buffer);
}

void run_file(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    EditorState* editor_state = get_editor_state();
    GtkSourceBuffer* console_source_buffer = editor_state->console_buffer;
    char* runner_program = "./yeet";
    char* stderr_redirection = "2>&1";
    int command_len = strlen(runner_program)
            + strlen(" ")
            + strlen(editor_state->parse_name)
            + strlen(" ")
            + strlen(stderr_redirection);
    char command_string[command_len + 1];
    sprintf(command_string, "%s \"%s\" %s", runner_program, editor_state->parse_name, stderr_redirection);
    // FIXME Extract opening of shell script to main
    FILE* shell_stream = popen(command_string, "r");
    const int buffer_size = 1000;
    char console_output[buffer_size + 1];
    while (!feof(shell_stream)) {
        int num_bytes_read = fread(console_output, sizeof(char), buffer_size, shell_stream);
        int num_chars_read = num_bytes_read / sizeof(char);
        console_output[num_chars_read] = '\0';
        text_buffer_append_text(GTK_TEXT_BUFFER(console_source_buffer), console_output);
    }
    pclose(shell_stream);
}

void run_button_clicked(GtkWidget* widget, gpointer data) {
    EditorState* editor_state = get_editor_state();
    if (!editor_state->parse_name) {
        save_file_button_clicked(widget, run_file);
    } else {
        save_to_file(run_file);
    }
    reset_console_buffer();
}

void save_to_file(gpointer data) {
    EditorState* editor_state = get_editor_state();
    GFile* file = g_file_parse_name(editor_state->parse_name);
            //g_file_new_for_path(editor_state->parse_name);
    GtkSourceFile* source_file = gtk_source_file_new();
    GtkSourceFileSaver* saver = gtk_source_file_saver_new_with_target(
            editor_state->editor_buffer,
            source_file,
            file);
    GAsyncReadyCallback callback = (GAsyncReadyCallback) data;
    gtk_source_file_saver_save_async(
            saver,
            G_PRIORITY_DEFAULT,
            NULL,
            NULL,
            NULL,
            NULL,
            callback,
            NULL);
    gtk_source_file_saver_save_finish(saver, NULL, NULL);
}

void save_file_button_clicked(GtkWidget* widget, gpointer data) {
    EditorState* editor_state = get_editor_state();
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    GtkFileChooserDialog* file_chooser_dialog = GTK_FILE_CHOOSER_DIALOG(
            gtk_file_chooser_dialog_new(
                    "Choose File",
                    window,
                    GTK_FILE_CHOOSER_ACTION_SAVE,
                    ("Cancel"),
                    GTK_RESPONSE_CANCEL,
                    ("Save"),
                    GTK_RESPONSE_ACCEPT,
                    NULL));
    GtkFileChooser* file_chooser = GTK_FILE_CHOOSER(file_chooser_dialog);
    gint result = gtk_dialog_run(GTK_DIALOG(file_chooser_dialog));
    if (result == GTK_RESPONSE_ACCEPT) {
        GFile* file = gtk_file_chooser_get_file(file_chooser);
        g_free(editor_state->parse_name);
        editor_state->parse_name = g_file_get_parse_name(file);
                //g_file_get_basename(file);
        save_to_file(data);
        g_object_unref(file);
    }
    gtk_widget_destroy(GTK_WIDGET(file_chooser));
}

void reset_console_buffer(void) {
    EditorState* editor_state = get_editor_state();
    GtkTextBuffer* text_buffer = GTK_TEXT_BUFFER(editor_state->console_buffer);
    clear_buffer(text_buffer);
    text_buffer_append_text(text_buffer, "Console output: \n");
}

void clear_editor_buffer(GObject* source_object, GAsyncResult* res, gpointer user_data) {
    EditorState* editor_state = get_editor_state();
    GtkTextBuffer* text_buffer = GTK_TEXT_BUFFER(editor_state->editor_buffer);
    clear_buffer(text_buffer);
}

void new_file_button_clicked(GtkWidget* widget, gpointer data) {
    EditorState* editor_state = get_editor_state();
    if (!editor_state->parse_name) {
        save_file_button_clicked(widget, clear_editor_buffer);
    } else {
        save_to_file(clear_editor_buffer);
    }
    g_free(editor_state->parse_name);
    editor_state->parse_name = NULL;
}
