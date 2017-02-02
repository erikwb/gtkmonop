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

#include "stub.h"
#include "aboutwin.h"

GtkWidget *new_aboutwin (void)
{
	GtkWidget *aboutwin;
	GtkWidget *vbox;
	GtkWidget *temp;
	GtkWidget *textbox;
	GtkWidget *buttonbox;
	GtkWidget *ok;

	aboutwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_usize (aboutwin, 300, 150);
	gtk_window_set_title (GTK_WINDOW (aboutwin), _("About"));

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (aboutwin), vbox);

	temp = gtk_label_new (_("GTKmonop v0.3.0"));
	gtk_widget_show (temp);
	gtk_box_pack_start (GTK_BOX (vbox), temp, FALSE, FALSE, 0);

	textbox = gtk_text_new (NULL, NULL);
	gtk_widget_show (textbox);
	gtk_box_pack_start (GTK_BOX (vbox), textbox, TRUE, TRUE, 0);
	gtk_text_insert (GTK_TEXT (textbox), NULL, NULL, NULL,
			 _("Copyright (C) Erik Bourget 2001\n\nSegfaults are due to user error."), strlen(_("Copyright (C) Erik Bourget 2001\n\nSegfaults are due to user error.")));
	
	buttonbox = gtk_hbutton_box_new ();
	gtk_widget_show (buttonbox);
	gtk_box_pack_start (GTK_BOX (vbox), buttonbox, FALSE, FALSE, 0);
	
	ok = gtk_button_new_with_label (_("OK..."));
	gtk_widget_show (ok);
	gtk_container_add (GTK_CONTAINER (buttonbox), ok);
	GTK_WIDGET_SET_FLAGS (ok, GTK_CAN_DEFAULT);

	gtk_signal_connect_object (GTK_OBJECT (ok), "clicked",
				   GTK_SIGNAL_FUNC (gtk_widget_destroy),
				   GTK_OBJECT (aboutwin));
	
	return aboutwin;
}
