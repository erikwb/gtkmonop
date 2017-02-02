#ifndef TRADEBOX_H
#define TRADEBOX_H

#include "structs.h"

extern GtkWidget *new_tradebox(trade *);
extern void kill_tradebox(trade *);
extern void update_tradebox(trade *, int, int, int);

#endif
