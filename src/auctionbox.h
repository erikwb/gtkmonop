#ifndef AUCTIONBOX_H
#define AUCTIONBOX_H

#include "structs.h"

extern GtkWidget *new_auctionbox(auction *);
extern void kill_auctionbox(auction *);
extern void update_auctionbox(auction *, player *, int);

#endif
