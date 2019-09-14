#include <stdlib.h>

#include "actions.h"
#include "editor_state.h"
#include "../yeet_lang/src/pi_interface/event_loop.h"

#define FULLSCREEN_MODE FALSE

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);
    GtkBuilder* builder = gtk_builder_new_from_file("gui_active.glade");

    /* Setup window and text_buffers */
    GtkApplicationWindow* window = GTK_APPLICATION_WINDOW(
            gtk_builder_get_object(builder, "window"));
    
    if (FULLSCREEN_MODE) {
        gtk_window_fullscreen(GTK_WINDOW(window));
    }

    GtkSourceView* editor_source_view = GTK_SOURCE_VIEW(
            gtk_builder_get_object(builder, "editor_source_view"));

    GtkSourceBuffer* editor_source_buffer = GTK_SOURCE_BUFFER(
            gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor_source_view)));

    GtkSourceView* console_source_view = GTK_SOURCE_VIEW(
            gtk_builder_get_object(builder, "console_source_view"));

    GtkSourceBuffer* console_source_buffer = GTK_SOURCE_BUFFER(
            gtk_text_view_get_buffer(GTK_TEXT_VIEW(console_source_view)));

    initialize_state(editor_source_buffer, console_source_buffer, fopen(RUNNING_STATE_FILE, "w"));

    /* Setup css styling for the main window */
    GtkCssProvider* css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "style.css", NULL);
    GtkStyleContext* window_style_context = gtk_widget_get_style_context(GTK_WIDGET(window));
    gtk_style_context_add_provider(
            window_style_context,
            GTK_STYLE_PROVIDER(css_provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_style_context_add_class(window_style_context, "window");

    /* Setup syntax highlighting */
    /* The language manager is owned by GtkSourceView library and must not be unref'ed. */
    GtkSourceLanguageManager* language_manager = gtk_source_language_manager_new();
    GtkSourceLanguage* yeet_language = gtk_source_language_manager_get_language(
            language_manager,
            "haskell");
    gtk_source_buffer_set_language(editor_source_buffer, yeet_language);

    /* Setup buttons */
    GtkButton* new_button = GTK_BUTTON(gtk_builder_get_object(builder, "new_button"));
    GtkButton* open_button = GTK_BUTTON(gtk_builder_get_object(builder, "open_button"));
    GtkButton* save_button = GTK_BUTTON(gtk_builder_get_object(builder, "save_button"));
    GtkButton* undo_button = GTK_BUTTON(gtk_builder_get_object(builder, "undo_button"));
    GtkButton* redo_button = GTK_BUTTON(gtk_builder_get_object(builder, "redo_button"));
    GtkButton* run_button = GTK_BUTTON(gtk_builder_get_object(builder, "run_button"));
    GtkButton* exit_button = GTK_BUTTON(gtk_builder_get_object(builder, "exit_button"));
    g_signal_connect(new_button, "clicked", G_CALLBACK(new_file_button_clicked), NULL);
    g_signal_connect(open_button, "clicked", G_CALLBACK(open_file_button_clicked), NULL);
    g_signal_connect(undo_button, "clicked", G_CALLBACK(undo_editor_button_clicked), NULL);
    g_signal_connect(redo_button, "clicked", G_CALLBACK(redo_editor_button_clicked), NULL);
    g_signal_connect(run_button, "clicked", G_CALLBACK(run_button_clicked), NULL);
    g_signal_connect(exit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(save_button, "clicked", G_CALLBACK(save_file_button_clicked), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    reset_console_buffer();
    gtk_widget_show(GTK_WIDGET(window));
    gtk_main();
    g_object_unref(builder);

    fclose(get_editor_state()->event_loop_running_state);
    return EXIT_SUCCESS;
}
