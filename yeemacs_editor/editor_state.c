#include "editor_state.h"

EditorState* get_editor_state() {
    static EditorState editor_state;
    return &editor_state;
}

void initialize_state(GtkSourceBuffer* editor, GtkSourceBuffer* console, FILE* event_loop_running_state) {
    EditorState* state = get_editor_state();
    state->editor_buffer = editor;
    state->console_buffer = console;
    state->parse_name = NULL;
    state->event_loop_running_state = event_loop_running_state;
    state->event_loop_thread = -1;
}