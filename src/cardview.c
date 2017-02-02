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

#include <stdlib.h>
#include <gtk/gtk.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "structs.h"
#include "stub.h"
#include "gdk_functions.h"
#include "misc_functions.h"
#include "cardview.h"

/* local function prototypes */
static void on_cv_ok_clicked (GtkButton *, GtkWidget *);

/* external function bodies */
GtkWidget *new_cardview (xmlNodePtr node) 
{
	GtkWidget *cardview;
	GtkWidget *vbox, *frame, *scrolledwindow, *buttonbox, *temp;
	player *p;
	char *buf;
	
	buf = xmlGetProp(node, (xmlChar *) "playerid");
	p = player_of_id(atoi(buf));

	cardview = gtk_window_new (GTK_WINDOW_DIALOG);

	buf = malloc(1024);
	sprintf(buf, "Card for %d. %s", p->id, p->name);
	gtk_window_set_title (GTK_WINDOW(cardview), _(buf));
	free(buf);
	
	vbox = gtk_vbox_new(FALSE, 4);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(cardview), vbox);
	
	buf = xmlGetProp(node, (xmlChar *) "name");
	frame = gtk_frame_new(_(buf));
	gtk_widget_show(frame);
	gtk_container_add(GTK_CONTAINER(vbox), frame);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_container_add (GTK_CONTAINER (frame), scrolledwindow);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), 
					GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

	temp = gtk_text_new (NULL, NULL);
	gtk_text_set_word_wrap(GTK_TEXT(temp), TRUE);
	gtk_widget_show (temp);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), temp);
	gtk_widget_set_usize (temp, 200, 80);
	
	buf = xmlGetProp(node, (xmlChar *) "description");
	gtk_text_insert(GTK_TEXT(temp), NULL, NULL, NULL, buf, -1);
	
	buttonbox = gtk_hbutton_box_new();
	gtk_widget_show(buttonbox);
	gtk_container_add(GTK_CONTAINER(vbox), buttonbox);

	gtk_container_set_border_width (GTK_CONTAINER (buttonbox), 5);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox), GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (buttonbox), 0);
	gtk_button_box_set_child_size (GTK_BUTTON_BOX (buttonbox), 65, 27);
	gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (buttonbox), 0, 0);

	temp = gtk_button_new_with_label(_("OK"));
	gtk_widget_show(temp);
	gtk_container_add(GTK_CONTAINER(buttonbox), temp);
	
	gtk_signal_connect (GTK_OBJECT (temp), "clicked",
			    GTK_SIGNAL_FUNC (on_cv_ok_clicked),
			    cardview);

	return cardview;
}

/* local function bodies */	
void on_cv_ok_clicked (GtkButton *moo, GtkWidget *cardview) 
{
	gtk_widget_destroy(cardview);
}
