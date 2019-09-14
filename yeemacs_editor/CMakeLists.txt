cmake_minimum_required(VERSION 3.14)
project(yeet_gui C)

set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GTKSOURCEVIEW REQUIRED gtksourceview-3.0)

include_directories (${GTK3_INCLUDE_DIRS})
include_directories (${GTKSOURCEVIEW_INCLUDE_DIRS})
include_directories(../yeet_lang/src/pi_interface)

link_directories (${GTK3_LIBRARY_DIRS})
link_directories (${GTKSOURCEVIEW_LIBRARY_DIRS})


add_executable(yeemacs
        yeemacs.c
        actions.c
        actions.h
        editor_state.c
        editor_state.h
        ../yeet_lang/src/pi_interface/event_loop.h
        ../yeet_lang/src/interpreter/stack.h)


add_definitions (${GTK3_CFLAGS_OTHER})
add_definitions (${GTKSOURCEVIEW_CFLAGS_OTHER})

target_link_libraries (
        yeemacs
        ${GTK3_LIBRARIES}
        ${GTKSOURCEVIEW_LIBRARIES})



