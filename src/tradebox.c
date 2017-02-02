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
#include <malloc.h>

#include "structs.h"
#include "stub.h"
#include "client.h"
#include "misc_functions.h"
#include "assetbox.h"
#include "tradebox.h"

#define NODE_KEY "NODE_KEY"
#define NAME_KEY "NAME_KEY"
#define TREE_KEY "TREE_KEY"

/* local function prototypes */
void on_accept_clicked (GtkButton *, trade *);
void on_reject_clicked (GtkButton *, trade *);
void on_money_clicked (GtkButton *, trade *);
void trade_add_estate (trade *, estate *, GtkCTreeNode *, int);
void trade_remove_estate(trade *, estate *, int);
gboolean show_trade_popup (GtkWidget *, GdkEventButton *, trade *);
GtkWidget *new_tradegroup_popup(estate *, trade *);
void on_tradedel_activated (GtkWidget *, trade *);

/* external function bodies */
GtkWidget *new_tradebox(trade *t) 
{
	int i, j, max_group_id;
	extern estate *estates[];
	extern gmonopprefs *prefs;
	estate *e;
	GtkWidget *temp, *accept, *reject, *buttonbox, *scrolledwindow, *vbox, *viewport, *widget, *hbox;
	GtkWidget *spinbutton, *moneybutton, *vbox2;
	GtkCTreeNode *tempnode;
	char *colors[9];
	char *str[1];
	char buf[512];

	widget = gtk_vbox_new(FALSE, 4);
	gtk_box_set_homogeneous(GTK_BOX(widget), FALSE);
	vbox = gtk_vbox_new(FALSE, 4);
	gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
	gtk_widget_show(vbox);
	
	hbox = gtk_hbox_new(FALSE, 4);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 3);
	
	/* accept button, reject button, in a hbox/hbuttonbox so it looks nice */
	accept = gtk_check_button_new_with_label(_("Accept trade"));
	gtk_widget_show (accept);
	gtk_container_add (GTK_CONTAINER (hbox), accept);
	GTK_WIDGET_SET_FLAGS (accept, GTK_CAN_DEFAULT);
	gtk_object_set_data(GTK_OBJECT(vbox), "accept", accept);

	gtk_signal_connect (GTK_OBJECT (accept), "clicked",
			    GTK_SIGNAL_FUNC (on_accept_clicked),
			    t);
	t->acceptbutton = GTK_TOGGLE_BUTTON(accept);
	
	buttonbox = gtk_hbutton_box_new ();
	gtk_widget_show (buttonbox);
	gtk_widget_set_usize (buttonbox, -2, 42);
	gtk_box_pack_start(GTK_BOX(hbox), buttonbox, FALSE, FALSE, 3);
	
	gtk_container_set_border_width (GTK_CONTAINER (buttonbox), 5);
	gtk_box_set_homogeneous(GTK_BOX(buttonbox), FALSE);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox), GTK_BUTTONBOX_START);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (buttonbox), 0);
	gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (buttonbox), 0, 0);
	
	reject = gtk_button_new_with_label(_("Reject trade"));
	gtk_button_set_relief(GTK_BUTTON(reject), GTK_RELIEF_HALF);
	gtk_widget_show(reject);
	gtk_container_add (GTK_CONTAINER (buttonbox), reject);
	GTK_WIDGET_SET_FLAGS (reject, GTK_CAN_DEFAULT);

	gtk_signal_connect (GTK_OBJECT (reject), "clicked",
			    GTK_SIGNAL_FUNC (on_reject_clicked),
			    t);

	spinbutton = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 
									   player_of_id(prefs->me)->cash, 
									   1, 50, 5)), 
					 1, 0);
	gtk_widget_show(spinbutton);
	gtk_container_add(GTK_CONTAINER(hbox), spinbutton);
	gtk_object_set_data (GTK_OBJECT(vbox), "spinbutton", spinbutton);
	
	moneybutton = gtk_button_new_with_label(_("Offer Money"));
	gtk_button_set_relief(GTK_BUTTON(moneybutton), GTK_RELIEF_HALF);
	gtk_widget_show (moneybutton);
	gtk_container_add (GTK_CONTAINER (buttonbox), moneybutton);
	GTK_WIDGET_SET_FLAGS (moneybutton, GTK_CAN_DEFAULT);

	gtk_signal_connect (GTK_OBJECT(moneybutton), "clicked",
			    GTK_SIGNAL_FUNC(on_money_clicked),
			    t);

	/* assetview frames */
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);

	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), 
					GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

	gtk_container_add(GTK_CONTAINER(vbox), scrolledwindow);
	viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);
	gtk_container_add(GTK_CONTAINER (viewport), widget);

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

	for(i = 0; i < 2; i++) {
		/* make a frame */
		sprintf(buf, "%d. %s", t->p[i]->id, t->p[i]->name);
		t->frame[i] = gtk_frame_new(buf);
		gtk_widget_show(t->frame[i]);
		gtk_box_pack_start (GTK_BOX (widget), t->frame[i], FALSE, FALSE, 3);

		/* make a vbox for the frame */
		vbox2 = gtk_vbox_new(FALSE, 4);
		gtk_box_set_homogeneous(GTK_BOX(vbox2), FALSE);
		gtk_widget_show(vbox2);
		gtk_container_add(GTK_CONTAINER(t->frame[i]), vbox2);

		/* make a money tree */
		t->moneytree[i] = gtk_ctree_new(1, 0);
		gtk_clist_set_row_height (GTK_CLIST (t->moneytree[i]), 16);
		gtk_clist_set_column_width (GTK_CLIST (t->moneytree[i]), 0, 200);
		gtk_clist_set_selection_mode (GTK_CLIST (t->moneytree[i]),
					      GTK_SELECTION_EXTENDED);
		gtk_ctree_set_show_stub (GTK_CTREE (t->moneytree[i]), FALSE);
		gtk_ctree_set_indent (GTK_CTREE (t->moneytree[i]), 16);
		gtk_ctree_set_line_style (GTK_CTREE (t->moneytree[i]), GTK_CTREE_LINES_DOTTED);
		gtk_widget_show(t->moneytree[i]);

		/* stick money tree in frame */
		gtk_box_pack_start (GTK_BOX (vbox2), t->moneytree[i], FALSE, FALSE, 0);

		/* make a root node */
		*str = malloc(10);
		strcpy(*str, "Money: $0");
		tempnode = gtk_ctree_insert_node( GTK_CTREE(t->moneytree[i]),
					      NULL, NULL, str, 5,
					      NULL, NULL, NULL, NULL,
					      FALSE, TRUE);
		gtk_object_set_data(GTK_OBJECT(t->moneytree[i]), "node", tempnode);
		free(*str);

		/* make a property tree */
		t->tree[i] = gtk_ctree_new (1, 0);
		gtk_clist_set_row_height (GTK_CLIST (t->tree[i]), 20);
		gtk_clist_set_column_width (GTK_CLIST (t->tree[i]), 0, 200);
		gtk_clist_set_selection_mode (GTK_CLIST (t->tree[i]),
					      GTK_SELECTION_EXTENDED);
		gtk_ctree_set_show_stub (GTK_CTREE (t->tree[i]), FALSE);
		gtk_ctree_set_indent (GTK_CTREE (t->tree[i]), 16);
		gtk_ctree_set_line_style (GTK_CTREE (t->tree[i]), GTK_CTREE_LINES_DOTTED);
		gtk_widget_show(t->tree[i]);

		/* stick tree in frame */
		gtk_box_pack_start (GTK_BOX (vbox2), t->tree[i], FALSE, FALSE, 0);

		/* make a root node */
		*str = malloc(20);
		strcpy(*str, "Estates");
		t->root[i] = gtk_ctree_insert_node( GTK_CTREE(t->tree[i]),
						    NULL, NULL, str, 5,
						    NULL, NULL, NULL, NULL,
						    FALSE, TRUE);
		free(*str);
		
		/* add clicked callbacks */
		gtk_signal_connect (GTK_OBJECT (t->tree[i]), "button_press_event",
				    GTK_SIGNAL_FUNC (show_trade_popup),
				    t);
	}
	
	/* this max_group_id stuff is useless until we get gamestart confirmation after the initial
	   estateupdates again */
	max_group_id = 0;
	for(i = 0; i < 40; i++) {
		if(estates[i]->ownable && (max_group_id < estates[i]->group)) 
			max_group_id = estates[i]->group;
	}

	max_group_id = 8;

	for(i = 0; i < 2; i++) {
		for(j = 0; j <= max_group_id; j++) {
			e = new_malloc(sizeof(estate));
			e->group = j;
			e->dummy = TRUE;
			e->id = i;
			str[0] = colors[j];
			t->grouptree[j][i] = gtk_ctree_insert_node( GTK_CTREE(t->tree[i]),
								    t->root[i], NULL, str, 5,
								    NULL, NULL, NULL, NULL,
								    FALSE, TRUE);
			gtk_ctree_node_set_row_data (GTK_CTREE(t->tree[i]), t->grouptree[j][i], e);
		}
	}	
	gtk_widget_show(widget);
	gtk_object_set_data(GTK_OBJECT(vbox), "trade", t);
	return vbox;
}

void kill_tradebox(trade *t) 
{
	extern GtkWidget *mainwin;
	GtkWidget *notebook;
	notebook = gtk_object_get_data(GTK_OBJECT(mainwin), "notebook");
	gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), gtk_notebook_page_num(GTK_NOTEBOOK(notebook), t->widget));
	/* this also gtk_widget_destroys t->widget */
	t->widget = NULL;
}

void update_tradebox(trade *t, int eid, int to, int add) 
{
	extern GtkWidget *mainwin;
	extern estate *estates[];
	extern gmonopprefs *prefs;
	GtkWidget *notebook;
	estate *e;
	
	e = estates[eid];
	notebook = gtk_object_get_data(GTK_OBJECT(mainwin), "notebook");
	
	if(add)
		trade_add_estate(t, e, t->grouptree[e->group][(e->owner->id == prefs->me) ? 0 : 1], 
				 (estates[eid]->owner->id == prefs->me) ? 0 : 1);
	else
		trade_remove_estate(t, e, (estates[eid]->owner->id == prefs->me) ? 0 : 1);
}

/* local function bodies */
void trade_add_estate (trade *t, estate *e, GtkCTreeNode *parent, int i)
{
	extern gmonopprefs *prefs;
	images *im = prefs->theme->im;
	gchar *text[1];
	text[0] = e->name;

	t->esnode[e->id] = gtk_ctree_insert_node (GTK_CTREE (t->tree[i]),
						  parent, NULL, text, 5,
						  im->deeds[e->group]->pixmapbase, NULL,
						  im->deeds[e->group]->pixmapbase, NULL,
						  FALSE, TRUE);
	gtk_ctree_node_set_row_data (GTK_CTREE(t->tree[i]), t->esnode[e->id], e);
}

void trade_remove_estate (trade *t, estate *e, int i) 
{
	if(t->esnode[e->id] == NULL)
		return;
	gtk_ctree_remove_node (GTK_CTREE (t->tree[i]), t->esnode[e->id]);
}

void on_accept_clicked (GtkButton *button, trade *t)
{
	char str[20];
	sprintf(str, "%d", t->id);
	client_outmessage(OUT_TRADEACCEPT, str);
}

void on_reject_clicked (GtkButton *button, trade *t)
{
	char str[20];
	sprintf(str, "%d", t->id);
	client_outmessage(OUT_TRADEREJECT, str);
}

void on_money_clicked (GtkButton *button, trade *t)
{
	GtkWidget *spinbutton;
	char str[20];
	spinbutton = gtk_object_get_data(GTK_OBJECT(t->widget), "spinbutton");
	
	sprintf(str, "%d:%d", t->id, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton)));
	client_outmessage(OUT_TRADEMONEY, str);
}

gboolean show_trade_popup (GtkWidget *widget, GdkEventButton *event, trade *t) 
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
			if ((e = gtk_clist_get_row_data(GTK_CLIST(widget), 
							row))) {
				if(e->dummy)
					popup = new_tradegroup_popup(e, t);
				else 
					popup = new_asset_popup(e);
				gtk_widget_show(popup);
				gtk_menu_popup(GTK_MENU(popup), NULL, NULL,
					       NULL, NULL, event->button,
					       event->time);
				return TRUE;
			}
		}
	}
	return FALSE;
}

GtkWidget *new_tradegroup_popup(estate *e, trade *t) 
{
	GtkWidget *popup;
	GtkWidget *item;
	GtkAccelGroup *popup_accels;
	char str[1024];
	int i, items = 0;
	extern estate *estates[];
	
	popup = gtk_menu_new();
	popup_accels = gtk_menu_ensure_uline_accel_group(GTK_MENU(popup));
	
	for(i = 0; i < 40; i++) {
		if((estates[i]->group == e->group) &&
		   (estates[i]->owner == t->p[e->id]) &&
		   (!t->included[i])) {
			sprintf(str, "Add %s to trade", estates[i]->name);
			item = gtk_menu_item_new_with_label(str);
			gtk_widget_show(item);
			gtk_container_add(GTK_CONTAINER(popup), item);
			gtk_object_set_data(GTK_OBJECT(item), "estate", estates[i]);
			
			gtk_signal_connect (GTK_OBJECT(item), "activate",
					    GTK_SIGNAL_FUNC (on_tradedel_activated),
					    t);
			items++;
		}
	}
	
	if(!items) {
		item = gtk_menu_item_new_with_label("No actions available");
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(popup), item);
	}
	
	return popup;
}

void on_tradedel_activated (GtkWidget *widget, trade *t) 
{
	estate *e;
	char *buf = (char *)malloc(20);
	
	e = gtk_object_get_data(GTK_OBJECT(widget), "estate");
	t->included[e->id] = !t->included[e->id];
	sprintf(buf, "%d:%d", t->id, e->id);
	client_outmessage(OUT_TRADEESTATE, buf);
	free(buf);
}
