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
#include "structs.h"
#include "stub.h"
#include "gdk_functions.h"
#include "estateview.h"

/* local function prototypes */
static void on_ev_ok_clicked (GtkButton *, estate *);
static gboolean on_estateview_die (GtkWidget *, GdkEvent *, estate *);

/* external function bodies */
GtkWidget *new_estateview(estate *e) 
{
	GtkWidget *estateview;
	GtkWidget *table, *bigtable, *hbox, *vbox, *frame, *buttonbox;
	GtkWidget *temp;
	extern gmonopprefs *prefs;
	images *im = prefs->theme->im;
	char str[1024];

	estateview = gtk_window_new (GTK_WINDOW_DIALOG);
	gtk_window_set_title (GTK_WINDOW(estateview), _(e->name));
	gtk_widget_realize(estateview);

	bigtable = gtk_table_new(1, 1, FALSE);
	gtk_widget_show(bigtable);
	gtk_container_add(GTK_CONTAINER(estateview), bigtable);

	vbox = gtk_vbox_new(FALSE, 4);
	gtk_widget_show(vbox);
	gtk_table_attach(GTK_TABLE(bigtable), vbox, 0, 1, 0, 1,
			 GTK_EXPAND | GTK_SHRINK | GTK_FILL,
			 GTK_EXPAND | GTK_SHRINK | GTK_FILL,
			 4, 4);
	
	hbox = gtk_hbox_new(FALSE, 4);
	gtk_widget_show(hbox);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);

	temp = pixmap_label(im->deeds[e->group]->pixmapbase, im->deeds[e->group]->maskbase, e->name);
	gtk_widget_show(temp);
	gtk_container_add(GTK_CONTAINER(hbox), temp);
	gtk_box_set_child_packing(GTK_BOX(hbox), temp, FALSE, FALSE, 0,
				  GTK_PACK_START);
	
	frame = gtk_frame_new(_("Properties"));
	gtk_widget_show(frame);
	gtk_container_add(GTK_CONTAINER(vbox), frame);

	table = gtk_table_new(6, 4, FALSE);
	gtk_widget_show(table);
	gtk_container_add (GTK_CONTAINER(frame), table);
	
	temp = gtk_label_new(_("Owner:"));
	gtk_misc_set_alignment(GTK_MISC(temp), 1.0f, 0.0f);
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 0, 1,
			 0, 1,
			 GTK_FILL, GTK_FILL,
			 5, 0);
	if(e->owner)
		temp = pixmap_label(im->tokens[e->owner->id]->pixmapbase, im->tokens[e->owner->id]->maskbase, e->owner->name);
	else
		temp = gtk_label_new("Unowned");

	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 1, 2,
			 0, 1,
			 GTK_EXPAND, GTK_EXPAND,
			 5, 0);

	temp = gtk_label_new(_("Houses:"));
	gtk_misc_set_alignment(GTK_MISC(temp), 1.0f, 0.0f);
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 0, 1, 1, 2,
			 GTK_FILL, GTK_FILL,
			 5, 0);
	
	sprintf(str, "%d", e->houses);
	temp = gtk_label_new(_(str));
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 1, 2, 1, 2,
			 GTK_EXPAND, GTK_EXPAND,
			 5, 0);

	temp = gtk_label_new(_("Price:"));
	gtk_misc_set_alignment(GTK_MISC(temp), 1.0f, 0.0f);
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 0, 1, 2, 3,
			 GTK_FILL, GTK_FILL,
			 5, 0);
	
	sprintf(str, "$%d", e->price);
	temp = gtk_label_new(_(str));
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 1, 2, 2, 3,
			 GTK_EXPAND, GTK_EXPAND,
			 5, 0);
	
	temp = gtk_label_new(_("House price:"));
	gtk_misc_set_alignment(GTK_MISC(temp), 1.0f, 0.0f);
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 0, 1, 3, 4,
			 GTK_FILL, GTK_FILL,
			 5, 0);
	
	sprintf(str, "$%d", e->houseprice);
	temp = gtk_label_new(_(str));
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 1, 2, 3, 4,
			 GTK_EXPAND, GTK_EXPAND,
			 5, 0);

	temp = gtk_label_new(_("Rent:"));
	gtk_misc_set_alignment(GTK_MISC(temp), 1.0f, 0.0f);
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 0, 1, 4, 5,
			 GTK_FILL, GTK_FILL,
			 5, 0);

	sprintf(str, "$%d", e->rent[0]);
	temp = gtk_label_new(_(str));
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 1, 2, 4, 5,
			 GTK_EXPAND, GTK_EXPAND,
			 5, 0);

	temp = gtk_label_new(_("With 1 house:"));
	gtk_misc_set_alignment(GTK_MISC(temp), 1.0f, 0.0f);
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 2, 3, 0, 1,
			 GTK_FILL, GTK_FILL,
			 5, 0);

	sprintf(str, "$%d", e->rent[1]);
	temp = gtk_label_new(_(str));
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 3, 4, 0, 1,
			 GTK_EXPAND, GTK_EXPAND,
			 5, 0);

	temp = gtk_label_new(_("With 2 houses:"));
	gtk_misc_set_alignment(GTK_MISC(temp), 1.0f, 0.0f);
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 2, 3, 1, 2,
			 GTK_FILL, GTK_FILL,
			 5, 0);

	sprintf(str, "$%d", e->rent[2]);
	temp = gtk_label_new(_(str));
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 3, 4, 1, 2,
			 GTK_EXPAND, GTK_EXPAND,
			 5, 0);

	temp = gtk_label_new(_("With 3 houses:"));
	gtk_misc_set_alignment(GTK_MISC(temp), 1.0f, 0.0f);
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 2, 3, 2, 3,
			 GTK_FILL, GTK_FILL,
			 5, 0);

	sprintf(str, "$%d", e->rent[3]);
	temp = gtk_label_new(_(str));
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 3, 4, 2, 3,
			 GTK_EXPAND, GTK_EXPAND,
			 5, 0);

	temp = gtk_label_new(_("With 4 houses:"));
	gtk_misc_set_alignment(GTK_MISC(temp), 1.0f, 0.0f);
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 2, 3, 3, 4,
			 GTK_FILL, GTK_FILL,
			 5, 0);

	sprintf(str, "$%d", e->rent[4]);
	temp = gtk_label_new(_(str));
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 3, 4, 3, 4,
			 GTK_EXPAND, GTK_EXPAND,
			 5, 0);

	temp = gtk_label_new(_("With a hotel:"));
	gtk_misc_set_alignment(GTK_MISC(temp), 1.0f, 0.0f);
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 2, 3, 4, 5,
			 GTK_FILL, GTK_FILL,
			 5, 0);

	sprintf(str, "$%d", e->rent[5]);
	temp = gtk_label_new(_(str));
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 3, 4, 4, 5,
			 GTK_EXPAND, GTK_EXPAND,
			 5, 0);

	temp = gtk_hseparator_new();
	gtk_widget_show(temp);
	gtk_container_add(GTK_CONTAINER(vbox), temp);

	buttonbox = gtk_hbutton_box_new ();
	gtk_widget_show (buttonbox);
	gtk_container_add (GTK_CONTAINER (vbox), buttonbox);
	gtk_container_set_border_width (GTK_CONTAINER (buttonbox), 5);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox),
				   GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (buttonbox), 0);
	gtk_button_box_set_child_size (GTK_BUTTON_BOX (buttonbox), 65, 27);
	gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (buttonbox), 0, 0);


	temp = gtk_button_new_with_label("OK");
	gtk_widget_show(temp);
	gtk_container_add(GTK_CONTAINER(buttonbox), temp);

	gtk_signal_connect (GTK_OBJECT (temp), "clicked",
			    GTK_SIGNAL_FUNC (on_ev_ok_clicked),
			    e);

	gtk_signal_connect (GTK_OBJECT (estateview), "delete_event",
			    GTK_SIGNAL_FUNC (on_estateview_die),
			    e);
	gtk_signal_connect (GTK_OBJECT (estateview), "destroy_event",
			    GTK_SIGNAL_FUNC (on_estateview_die),
			    e);
	return estateview;
}

/* local function bodies */
void on_ev_ok_clicked (GtkButton *button, estate *e)
{
	on_estateview_die(e->estateview, NULL, e);
}
gboolean on_estateview_die (GtkWidget *estateview, GdkEvent *event, estate *e) 
{
	e->estateview_open = FALSE;
	gtk_widget_destroy(estateview);
	return TRUE;
}

