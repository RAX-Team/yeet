#ifndef YEET_GUI_EDITOR_STATE_H
#define YEET_GUI_EDITOR_STATE_H

#include <gtksourceview/gtksource.h>
#include <pthread.h>

typedef struct {
    GtkSourceBuffer* editor_buffer;
    GtkSourceBuffer* console_buffer;
    gchar* parse_name;
    FILE* event_loop_running_state;
    pthread_t event_loop_thread;
} EditorState;

EditorState* get_editor_state();
void initialize_state(GtkSourceBuffer* editor, GtkSourceBuffer* console, FILE* event_loop_running_state);

#endif //YEET_GUI_EDITOR_STATE_H
