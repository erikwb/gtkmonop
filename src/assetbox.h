#ifndef ASSETBOX_H
#define ASSETBOX_H

#include <gtk/gtk.h>
#include "structs.h"

extern void assetbox_moneychanged(player *);
extern void update_assetbox(estate *, player *, player *);
extern GtkWidget *new_asset_popup(estate *);
extern void fill_assetbox(GtkWidget *);
 
#endif
