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
#include "auctionbox.h"

/* local function prototypes */
void on_bid_clicked(GtkButton *, auction *);

/* external function bodies */
GtkWidget *new_auctionbox(auction *a) 
{
	GtkWidget *vbox, *hbox, *bid, *spinbutton, *buttonbox, *clist, *viewport, *scrolledwindow;
	GtkWidget *temp;
	GtkObject *adj;
	extern gmonopprefs *prefs;
	char buf1[20], buf2[20], buf3[20], *bufx[3] = { buf1, buf2, buf3 };
		
	vbox = gtk_vbox_new(FALSE, 4);
	gtk_widget_show(vbox);

	hbox = gtk_hbox_new(FALSE, 4);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	temp = gtk_label_new(NULL);
	gtk_widget_show(temp);
	gtk_box_pack_start (GTK_BOX(hbox), temp, TRUE, TRUE, 0);
	gtk_object_set_data(GTK_OBJECT(vbox), "label", temp);

	buttonbox = gtk_hbutton_box_new();
	gtk_widget_show (buttonbox);
	gtk_box_pack_start (GTK_BOX (hbox), buttonbox, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (buttonbox), 5);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox), GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (buttonbox), 5);
	gtk_button_box_set_child_size (GTK_BUTTON_BOX (buttonbox), 40, 27);
	gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (buttonbox), 0, 0);
	
	bid = gtk_button_new_with_label("Place bid");
	gtk_widget_show(bid);
	gtk_box_pack_start (GTK_BOX (buttonbox), bid, TRUE, TRUE, 0);
	
	adj = gtk_adjustment_new(0, 0, player_of_id(prefs->me)->cash, 1, 50, 5);
	gtk_object_set_data(GTK_OBJECT(vbox), "adj", adj);
	spinbutton = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 1, 0);
	gtk_widget_show(spinbutton);
	gtk_object_set_data(GTK_OBJECT(vbox), "spinbutton", spinbutton);
	gtk_box_pack_start (GTK_BOX (buttonbox), spinbutton, FALSE, TRUE, 0);

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);

	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), 
					GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

	gtk_container_add(GTK_CONTAINER(vbox), scrolledwindow);
	viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);
	
	sprintf(buf1, "#");
	sprintf(buf2, "Player");
	sprintf(buf3, "Amount");
	clist = gtk_clist_new_with_titles(3, bufx);
	gtk_clist_set_column_width (GTK_CLIST (clist), 0, 25);
	gtk_clist_set_column_width (GTK_CLIST (clist), 1, 300);
	gtk_widget_show(clist);
	gtk_container_add(GTK_CONTAINER(viewport), clist);
	gtk_object_set_data(GTK_OBJECT(vbox), "clist", clist);

	gtk_signal_connect (GTK_OBJECT(bid), "clicked",
			    GTK_SIGNAL_FUNC (on_bid_clicked),
			    a);
	gtk_object_set_data(GTK_OBJECT(vbox), "auction", a);
	return vbox;
}

void update_auctionbox(auction *a, player *p, int bid) 
{
	extern GtkStyle *active_text_style;
	extern GtkWidget *mainwin;
	extern gmonopprefs *prefs;
	
	char *buf1, *buf2, *buf3, *bufx[3];
	GtkWidget *clist, *spinbutton;
	GtkWidget *notebook;
	GtkObject *adj, *temp;
	
	clist = gtk_object_get_data(GTK_OBJECT(a->widget), "clist");
	spinbutton = gtk_object_get_data(GTK_OBJECT(a->widget), "spinbutton");

	bufx[0] = buf1 = malloc(5);
	sprintf(buf1, "%d", ++(a->count));
	bufx[1] = buf2 = malloc(500);
	sprintf(buf2, "%d. %s", p->id, p->name);
	bufx[2] = buf3 = malloc(6);
	sprintf(buf3, "$%d", bid);

	adj = gtk_adjustment_new(bid+1, bid+1, player_of_id(prefs->me)->cash, 1, 50, 5);
	gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(spinbutton), GTK_ADJUSTMENT(adj));
	gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton));
	temp = gtk_object_get_data(GTK_OBJECT(a->widget), "adj");
	gtk_object_set_data(GTK_OBJECT(a->widget), "adj", adj);
	gtk_clist_prepend(GTK_CLIST(clist), bufx);
	free(buf1);
	free(buf2);
	free(buf3);

	notebook = gtk_object_get_data(GTK_OBJECT(mainwin), "notebook");

	if(a->widget != (gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 
						   gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)))))
		gtk_widget_set_style (a->tablabel, active_text_style);
}

void kill_auctionbox(auction *a) 
{
	extern GtkWidget *mainwin;
	GtkWidget *notebook;
	
	notebook = gtk_object_get_data(GTK_OBJECT(mainwin), "notebook");
	gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), gtk_notebook_page_num(GTK_NOTEBOOK(notebook), a->widget));
	a->widget = NULL;
}

/* local function bodies */
void on_bid_clicked(GtkButton *foo, auction *a) 
{
	GtkWidget *spinbutton;
	char *str = malloc(512);

	spinbutton = gtk_object_get_data(GTK_OBJECT(a->widget), "spinbutton");
	sprintf(str, "%d:%d", a->id, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton)));

	client_outmessage(OUT_AUCTIONBID, str);
	free(str);
}
