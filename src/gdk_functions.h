#ifndef GDK_FUNCTIONS_H
#define GDK_FUNCTIONS_H

#include <gtk/gtk.h>
#include "structs.h"

extern void init_board(void);
extern void update_board(void);
extern void update_statusbar(void);
extern GtkWidget *pixmap_label (GdkPixmap *, GdkBitmap *, char *);
extern void init_colors (void);
extern void init_pixmaps(void);
extern void init_pixbufs(void);
extern gint board_button_press_event(GtkWidget *, GdkEventButton *, gpointer);
extern void on_resize(void);
extern images *new_images(void);
extern gboolean start_fruity_graphics(void);
extern void set_geometry(void);

#endif
