#ifndef MISC_FUNCITONS_H
#define MISC_FUNCTIONS_H

#include <stdlib.h>
#include <gtk/gtk.h>
#include "structs.h"

extern player *findcurrent(void);
extern auction *auction_of_id(int);
extern trade *trade_of_id(int);
extern player *player_of_id(int);
extern void *new_malloc(size_t sz);
extern void clear_textbox(void);
extern int tprintf(const char *, ...);
extern int chatprintf(const char *, ...);
extern int statprintf(const char *, ...);

#endif
