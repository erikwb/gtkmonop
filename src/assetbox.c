/*
 * gtkmonop - the gtk+ monopoly simulator
 * Copyright (C) 2001 Erik Bourget
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <gtk/gtk.h>
#include <stdlib.h>

#include "structs.h"
#include "stub.h"
#include "client.h"
#include "estateview.h"
#include "misc_functions.h"
#include "assetbox.h"

#define NODE_KEY "NODE_KEY"
#define NAME_KEY "NAME_KEY"

GtkWidget *frame[10], *tree[10], *moneytree[10];
GtkCTreeNode *root[10], *grouptree[10][10], *moneyroot[10];
extern estate *estates[];

int filled = 0;

/* local function prototypes */
void fill_assetbox(GtkWidget *);
void asset_add_estate(estate *, player *);
void asset_remove_estate(estate *, player *);
gboolean show_asset_popup (GtkWidget *, GdkEventButton *, gpointer);
void on_tradeadd_activated (GtkWidget *, trade *);
void on_mortgage_activated (GtkWidget *, estate *);
void on_unmortgage_activated (GtkWidget *, estate *);
void on_buyhouse_activated (GtkWidget *, estate *);
void on_sellhouse_activated (GtkWidget *, estate *);
void on_no_action_activated (GtkWidget *, gpointer);
void on_estateprops_activated (GtkWidget *, estate *);

/* external function bodies */
void assetbox_moneychanged(player *p) 
{
	extern int gamestarted;
	char buf[20];
	if(!gamestarted) return;
	sprintf(buf, "Money: $%d", p->cash);
	gtk_ctree_node_set_text(GTK_CTREE(moneytree[p->id]), 
				moneyroot[p->id],
				0, buf);
}

void update_assetbox(estate *e, player *from, player *to) 
{
	extern GtkWidget *mainwin;
	GtkWidget *assetbox;

	assetbox = gtk_object_get_data(GTK_OBJECT(mainwin), "assetbox");
	
	/* update the sucker */

	if(from) asset_remove_estate(e, from);
	if(to) asset_add_estate(e, to);
}

void fill_assetbox(GtkWidget *assetbox) 
{
	extern estate *estates[];
	int i, j, max_group_id;
	GtkWidget *temp, *vbox2[8];
	char *colors[9], *str[1];
	
	gtk_widget_realize(assetbox);
	for(i = 0; i < 9; i++) {
		colors[i] = malloc(64);
		switch(i) {
		case 0:
			strcpy(colors[i], "Miscellaneous");
			break;
		case 1:
			strcpy(colors[i], "Dark Purple");
			break;
		case 2:
			strcpy(colors[i], "Light Blue");
			break;
		case 3:
			strcpy(colors[i], "Light Purple");
			break;
		case 4:
			strcpy(colors[i], "Orange");
			break;
		case 5:
			strcpy(colors[i], "Red");
			break;
		case 6:
			strcpy(colors[i], "Yellow");
			break;
		case 7:
			strcpy(colors[i], "Green");
			break;
		case 8:
			strcpy(colors[i], "Dark Blue");
			break;
		}
	}

	temp = NULL;
	
	for(i = 1; player_of_id(i); i++) {
		/* make a frame */
		frame[i] = gtk_frame_new (_(player_of_id(i)->name));
		gtk_widget_show (frame[i]);
		gtk_box_pack_start (GTK_BOX (assetbox), frame[i], TRUE, TRUE, 2);
		
		/* make a vbox for the frame */
		vbox2[i] = gtk_vbox_new(FALSE, 4);
		gtk_box_set_homogeneous(GTK_BOX(vbox2[i]), FALSE);
		gtk_widget_show(vbox2[i]);
			
		gtk_container_add(GTK_CONTAINER(frame[i]), vbox2[i]);
		
		/* make a money tree */
		moneytree[i] = gtk_ctree_new(1, 0);
		gtk_clist_set_row_height (GTK_CLIST (moneytree[i]), 16);
		gtk_clist_set_column_width (GTK_CLIST (moneytree[i]), 0, 200);
		gtk_clist_set_selection_mode (GTK_CLIST (moneytree[i]),
					      GTK_SELECTION_EXTENDED);
		gtk_ctree_set_show_stub (GTK_CTREE (moneytree[i]), FALSE);
		gtk_ctree_set_indent (GTK_CTREE (moneytree[i]), 16);
		gtk_ctree_set_line_style (GTK_CTREE (moneytree[i]), GTK_CTREE_LINES_DOTTED);
		gtk_widget_show(moneytree[i]);
		
		/* stick money tree in frame */
		gtk_box_pack_start (GTK_BOX (vbox2[i]), moneytree[i], FALSE, FALSE, 0);
		
		/* make a root node */
		*str = malloc(20);
		sprintf(*str, "Money: $%d", player_of_id(i)->cash);
		moneyroot[i] = gtk_ctree_insert_node( GTK_CTREE(moneytree[i]),
						      NULL, NULL, str, 5,
						      NULL, NULL, NULL, NULL,
						      FALSE, TRUE);
		free(*str);
		
		/* make a property tree */
		tree[i] = gtk_ctree_new (1, 0);
		gtk_clist_set_row_height (GTK_CLIST (tree[i]), 20);
		gtk_clist_set_column_width (GTK_CLIST (tree[i]), 0, 200);
		gtk_clist_set_selection_mode (GTK_CLIST (tree[i]),
					      GTK_SELECTION_EXTENDED);
		gtk_ctree_set_show_stub (GTK_CTREE (tree[i]), FALSE);
		gtk_ctree_set_indent (GTK_CTREE (tree[i]), 16);
		gtk_ctree_set_line_style (GTK_CTREE (tree[i]), GTK_CTREE_LINES_DOTTED);

		gtk_widget_show(tree[i]);
		gtk_container_add (GTK_CONTAINER (vbox2[i]), tree[i]);

		/* make a root node */
		*str = malloc(20);
		strcpy(*str, "Estates");
		root[i] = gtk_ctree_insert_node( GTK_CTREE(tree[i]),
						 NULL, NULL, str, 5,
						 NULL, NULL, NULL, NULL,
						 FALSE, TRUE);
		free(*str);
		
		/* add clicked callbacks */
		gtk_signal_connect (GTK_OBJECT (tree[i]), "button_press_event",
				    GTK_SIGNAL_FUNC (show_asset_popup),
				    NULL);
	}

	/* figure out what the highest group id is */
	max_group_id = 0;
	for(i = 0; i < 40; i++) {
	 	if(estates[i]->ownable && (max_group_id < estates[i]->group)) 
			max_group_id = estates[i]->group;
	}

	/* populate tree with group name tags */
	for(i = 1; player_of_id(i); i++)
		for(j = 0; j <= max_group_id; j++) {
			str[0] = colors[j];
			grouptree[j][i] = gtk_ctree_insert_node( GTK_CTREE(tree[i]),
								 root[i], NULL, str, 5,
								 NULL, NULL, NULL, NULL,
								 FALSE, TRUE);
		}

}

/* local function bodies */
void asset_add_estate (estate *e, player *p)
{
	extern gmonopprefs *prefs;
	images *im = prefs->theme->im;
	gchar *text[1];
	text[0] = e->name;

	e->node = gtk_ctree_insert_node (GTK_CTREE (tree[p->id]),
					 grouptree[e->group][p->id], NULL, text, 5,
					 im->deeds[e->group]->pixmapbase, NULL, 
					 im->deeds[e->group]->pixmapbase, NULL,
					 FALSE, TRUE);
	gtk_ctree_node_set_row_data (GTK_CTREE(tree[p->id]), e->node, e);
}

void asset_remove_estate (estate *e, player *p)
{
	if(e->node == NULL) return;
	gtk_ctree_remove_node (GTK_CTREE (tree[p->id]), e->node);
}

gboolean show_asset_popup (GtkWidget *widget, GdkEventButton *event, gpointer data) 
{
	GtkWidget *popup;
	estate *e;
	int row, col;
	
	if(event->button == 3) { /* right click */
		if (gtk_clist_get_selection_info
		    (GTK_CLIST (widget), event->x, event->y, &row, &col) < 0)
			return FALSE;
		gtk_clist_unselect_all (GTK_CLIST (widget));
		gtk_clist_select_row (GTK_CLIST (widget), row, 0);
		if(gtk_clist_get_row_data(GTK_CLIST(widget), row)) {
			if ((e = gtk_clist_get_row_data(GTK_CLIST(widget), row))) {
				popup = new_asset_popup(e);
				gtk_widget_show(popup);
				gtk_menu_popup(GTK_MENU(popup), NULL, NULL, NULL, NULL, event->button, event->time);
				return TRUE;
			}
		}
	}
	return FALSE;
}

GtkWidget *new_asset_popup(estate *e) 
{
	extern trade *trades;
	extern gmonopprefs *prefs;
	
	trade *step;
	GtkWidget *popup;
	GtkWidget *item;
	GtkAccelGroup *popup_accels;
	int items = 0;
	char str[1024];
	
	popup = gtk_menu_new ();
	popup_accels = gtk_menu_ensure_uline_accel_group(GTK_MENU(popup));

	sprintf(str, "Properties of %s", e->name);
	item = gtk_menu_item_new_with_label(str);
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(popup), item);
	gtk_signal_connect (GTK_OBJECT(item), "activate",
			    GTK_SIGNAL_FUNC (on_estateprops_activated),
			    e);
	items++;
	
	for(step = trades; step; step = step->next) {
		if((step->p[0] == e->owner) || (step->p[1] == e->owner)) {
			if(step->included[e->id])
				sprintf(str, "Remove %s from trade with %d. %s", e->name, 
					((step->p[0]->id == prefs->me) ? step->p[1]->id : step->p[0]->id),
					((step->p[0]->id == prefs->me) ? step->p[1]->name : step->p[0]->name));
			else
				sprintf(str, "Add %s to trade with %d. %s", e->name, 
					((step->p[0]->id == prefs->me) ? step->p[1]->id : step->p[0]->id),
					((step->p[0]->id == prefs->me) ? step->p[1]->name : step->p[0]->name));

			item = gtk_menu_item_new_with_label(str);
			gtk_widget_show(item);
			gtk_container_add(GTK_CONTAINER(popup), item);

			gtk_object_set_data(GTK_OBJECT(item), "estate", e);
			
			gtk_signal_connect (GTK_OBJECT(item), "activate",
					    GTK_SIGNAL_FUNC (on_tradeadd_activated),
					    step);
			items++;
		}
	}

	/* add separator if necessary */
	if(items && (e->mortgageable || e->unmortgageable || e->canbuyhouses || e->cansellhouses)) { 
		item = gtk_menu_item_new();
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(popup), item);
	}
	
	if(e->mortgageable) {
		sprintf(str, "Mortgage %s ($%d)", e->name, e->mortgageprice);
		item = gtk_menu_item_new_with_label(str);
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(popup), item);
		gtk_signal_connect (GTK_OBJECT(item), "activate",
				    GTK_SIGNAL_FUNC (on_mortgage_activated),
				    e);
		items++;
	}
	if(e->unmortgageable) {
		sprintf(str, "Unmortgage %s ($%d)", e->name, e->unmortgageprice);
		item = gtk_menu_item_new_with_label(str);
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(popup), item);
		gtk_signal_connect (GTK_OBJECT(item), "activate",
				    GTK_SIGNAL_FUNC (on_unmortgage_activated),
				    e);
		items++;
	}
	if(e->canbuyhouses) {
		sprintf(str, "Build a house on %s ($%d)", e->name, e->houseprice);
		item = gtk_menu_item_new_with_label(str);
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(popup), item);
		gtk_signal_connect (GTK_OBJECT(item), "activate",
				    GTK_SIGNAL_FUNC (on_buyhouse_activated),
				    e);
		items++;
	}
	if(e->cansellhouses) {
		sprintf(str, "Sell a house on %s ($%d)", e->name, e->sellhouseprice);
		item = gtk_menu_item_new_with_label(str);
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(popup), item);
		gtk_signal_connect (GTK_OBJECT(item), "activate",
				    GTK_SIGNAL_FUNC (on_sellhouse_activated),
				    e);
		items++;
	}
	
	if(!items) {
		item = gtk_menu_item_new_with_label("No actions available");
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(popup), item);
		gtk_signal_connect (GTK_OBJECT(item), "activate",
				    GTK_SIGNAL_FUNC (on_no_action_activated),
				    NULL);
	}		

	return popup;
}

void on_no_action_activated (GtkWidget *widget, gpointer moo) 
{
}
	
void on_mortgage_activated (GtkWidget *widget, estate *e) 
{
	char *buf = (char *)malloc(20);
	
	sprintf(buf, "%d", e->id);
	client_outmessage(OUT_MORTGAGE, buf);
	free(buf);
}

void on_unmortgage_activated (GtkWidget *widget, estate *e) 
{
	char *buf = (char *)malloc(20);
	
	sprintf(buf, "%d", e->id);
	client_outmessage(OUT_UNMORTGAGE, buf);
	free(buf);
}

void on_buyhouse_activated (GtkWidget *widget, estate *e) 
{
	char *buf = (char *)malloc(20);
	
	sprintf(buf, "%d", e->id);
	client_outmessage(OUT_BUYHOUSE, buf);
	free(buf);
}

void on_sellhouse_activated (GtkWidget *widget, estate *e) 
{
	char *buf = (char *)malloc(20);
	
	sprintf(buf, "%d", e->id);
	client_outmessage(OUT_SELLHOUSE, buf);
	free(buf);
}

void on_tradeadd_activated (GtkWidget *widget, trade *t) 
{
	estate *e;
	char *buf = (char *)malloc(20);
	
	e = gtk_object_get_data(GTK_OBJECT(widget), "estate");
	t->included[e->id] = !t->included[e->id];
	sprintf(buf, "%d:%d", t->id, e->id);
	client_outmessage(OUT_TRADEESTATE, buf);
	free(buf);
}

void on_estateprops_activated (GtkWidget *widget, estate *e)
{
	if(!e->estateview_open) {
		e->estateview = new_estateview(e);
		gtk_widget_show(e->estateview);
		e->estateview_open = TRUE;
	}
}

void tree_remove_widget(GtkWidget *widget, int i) 
{
	GtkCTreeNode *node;
	
	node = gtk_object_get_data (GTK_OBJECT (widget), NODE_KEY);
	if(node == NULL)
		return;
	gtk_ctree_remove_node (GTK_CTREE (tree[i]), GTK_CTREE_NODE (node));
}
