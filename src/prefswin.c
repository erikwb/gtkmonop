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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "stub.h"
#include "structs.h"
#include "gdk_functions.h"
#include "prefswin.h"
#include "theme.h"

/* local function prototypes */
static void on_prefs_ok_clicked (GtkButton *, GtkWidget *);
static void on_prefs_apply_clicked (GtkButton *, GtkWidget *);
static void on_prefs_cancel_clicked (GtkButton *, GtkWidget *);
static GtkWidget *new_theme_area(GtkWidget *);
static GtkWidget *new_image_area(GtkWidget *);
static GtkWidget *new_server_area(GtkWidget *);
static int fill_themelist(GtkWidget *);
static void on_select_row(GtkCList *, gint, gint, GdkEvent *, GtkWidget *);
static gboolean on_prefswin_die (GtkWidget *, GdkEvent *, GtkWidget *);

/* external function bodies */
GtkWidget *new_prefswin(void) 
{
	extern GtkWidget *mainwin;
	GtkWidget *prefswin, *vbox, *widget, *bigtable;
	GtkWidget *ok, *apply, *cancel, *buttonbox, *label, *notebook;

	prefswin = gtk_window_new (GTK_WINDOW_DIALOG);
	gtk_widget_realize(prefswin);
	gtk_window_set_title (GTK_WINDOW (prefswin), _("Preferences"));

	gtk_object_set_data(GTK_OBJECT(mainwin), "prefswin", prefswin);
	
	bigtable = gtk_table_new(1, 1, FALSE);
	gtk_widget_show(bigtable);
	gtk_container_add(GTK_CONTAINER(prefswin), bigtable);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_table_attach(GTK_TABLE(bigtable), vbox, 0, 1, 0, 1,
			 GTK_EXPAND | GTK_SHRINK | GTK_FILL,
			 GTK_EXPAND | GTK_SHRINK | GTK_FILL,
			 4, 4);

	notebook = gtk_notebook_new();
	gtk_notebook_popup_disable(GTK_NOTEBOOK(notebook));
	gtk_widget_show (notebook);
	gtk_container_add(GTK_CONTAINER(vbox), notebook);

	/* Themes */
	label = gtk_label_new(_("Themes"));
	gtk_widget_show (label);
	widget = new_theme_area(prefswin);
	gtk_widget_show(widget);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), widget, label);

	/* Images */
	label = gtk_label_new(_("Images"));
	gtk_widget_show(label);
	widget = new_image_area(prefswin);
	gtk_widget_show(widget);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), widget, label);
	
	/* Server */
	/*
	  label = gtk_label_new(_("Server"));
	  gtk_widget_show(label);
	  widget = new_server_area(prefswin);
	  gtk_widget_show(widget);
	  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), widget, label);
	*/

	/* Separator */
	widget = gtk_hseparator_new();
	gtk_widget_show(widget);
	gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, FALSE, 5);

	/* Buttonbox */
	buttonbox = gtk_hbutton_box_new();
	gtk_widget_show (buttonbox);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(buttonbox), GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(buttonbox), 5);
	gtk_box_pack_start(GTK_BOX(vbox), buttonbox, FALSE, FALSE, 5);
	// gtk_widget_set_sensitive(button, FALSE);

	ok = gtk_button_new_with_label(_("OK"));
	gtk_widget_show(ok);
	gtk_container_add(GTK_CONTAINER(buttonbox), ok);
	
	apply = gtk_button_new_with_label(_("Apply"));
	gtk_widget_show(apply);
	gtk_container_add(GTK_CONTAINER(buttonbox), apply);
	
	cancel = gtk_button_new_with_label(_("Cancel"));
	gtk_widget_show(cancel);
	gtk_container_add(GTK_CONTAINER(buttonbox), cancel);
	
	gtk_signal_connect (GTK_OBJECT (ok), "clicked",
			    GTK_SIGNAL_FUNC (on_prefs_ok_clicked),
			    prefswin);
	gtk_signal_connect (GTK_OBJECT (apply), "clicked",
			    GTK_SIGNAL_FUNC (on_prefs_apply_clicked),
			    prefswin);
	gtk_signal_connect (GTK_OBJECT (cancel), "clicked",
			    GTK_SIGNAL_FUNC (on_prefs_cancel_clicked),
			    prefswin);

	gtk_signal_connect (GTK_OBJECT (prefswin), "delete_event",
			    GTK_SIGNAL_FUNC (on_prefswin_die),
			    mainwin);
	gtk_signal_connect (GTK_OBJECT (prefswin), "destroy_event",
			    GTK_SIGNAL_FUNC (on_prefswin_die),
			    mainwin);

	return(prefswin);
}


/* local function bodies */
int fill_themelist(GtkWidget *themelist) 
{
	DIR *d;
	struct dirent *de;
	char buf[1024], dir[1024];
	int i;
	char *basedir[2];
	theme *t;
	int themecount, row;

	strcpy (dir, getenv ("HOME"));
	strcat (dir, "/.gtkmonop/themes");

	basedir[0] = GTKMONOP_DATA"/themes";
	basedir[1] = dir;

	themecount = 0;

	for (i = 0; i < 2; i ++) {
		d = opendir (basedir[i]);
		if (d) {
			while ((de = readdir(d))) {
				strcpy (buf, basedir[i]);
				strcat (buf, "/");

				if((t = get_theme_info(buf, de->d_name))) {
					row = gtk_clist_append(GTK_CLIST(themelist), &(t->name));
					gtk_clist_set_row_data(GTK_CLIST(themelist), row, t);
					themecount ++;
				}
			}
			closedir (d);
		}
	}
	return themecount;
}

GtkWidget *new_theme_area(GtkWidget *prefswin) 
{
	GtkWidget *frame, *themelist, *label, *namelabel, *authlabel;
	GtkWidget *table, *widget, *desclabel, *table1;
	
	themelist = gtk_clist_new (1);
	gtk_clist_set_column_width (GTK_CLIST(themelist), 0, 160);
	gtk_clist_set_selection_mode (GTK_CLIST(themelist), GTK_SELECTION_SINGLE);
	fill_themelist(themelist);
	gtk_widget_show (themelist);

	gtk_signal_connect (GTK_OBJECT(themelist), "select-row",
			    GTK_SIGNAL_FUNC (on_select_row),
			    prefswin);

	label = gtk_label_new (_("Select a theme from the list.\n"
				 "Install new themes in ~/.gtkmonop/themes/"));
	gtk_widget_show (label);

	table1 = gtk_table_new (3, 2, FALSE);
	gtk_object_set_data(GTK_OBJECT(prefswin), "table", table1);
	gtk_table_set_row_spacings (GTK_TABLE(table1), 3);
	gtk_table_set_col_spacings (GTK_TABLE(table1), 3);

	widget = gtk_label_new (_("Name:"));
	gtk_misc_set_alignment(GTK_MISC(widget), 0.0f, 0.0f);
	gtk_widget_show (widget);
	gtk_table_attach (GTK_TABLE(table1), widget, 0, 1, 0, 1,
			  GTK_EXPAND | GTK_FILL, 0, 2, 0);

	widget = gtk_label_new (_("Author:"));
	gtk_misc_set_alignment(GTK_MISC(widget), 0.0f, 0.0f);
	gtk_widget_show (widget);
	gtk_table_attach (GTK_TABLE(table1), widget, 0, 1, 1, 2,
			  GTK_EXPAND | GTK_FILL, 0, 2, 0);

	widget = gtk_label_new (_("Description:"));
	gtk_misc_set_alignment(GTK_MISC(widget), 0.0f, 0.0f);
	gtk_widget_show (widget);
	gtk_table_attach (GTK_TABLE(table1), widget, 0, 1, 2, 3,
			  GTK_EXPAND | GTK_FILL, 0, 2, 0);
	namelabel = gtk_label_new ("");
	gtk_misc_set_alignment(GTK_MISC(namelabel), 0.0f, 0.0f);
	gtk_widget_show (namelabel);
	gtk_table_attach (GTK_TABLE(table1), namelabel, 1, 2, 0, 1,
			  GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	authlabel = gtk_label_new ("");
	gtk_misc_set_alignment(GTK_MISC(authlabel), 0.0f, 0.0f);
	gtk_widget_show (authlabel);
	gtk_table_attach (GTK_TABLE(table1), authlabel, 1, 2, 1, 2,
			  GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	desclabel = gtk_text_new (0, 0);
	gtk_widget_set_usize (desclabel, 250, 40);
	gtk_widget_show (desclabel);
	gtk_table_attach (GTK_TABLE(table1), desclabel, 1, 2, 2, 3,
			  GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	gtk_object_set_data(GTK_OBJECT(prefswin), "namelabel", namelabel);
	gtk_object_set_data(GTK_OBJECT(prefswin), "authlabel", authlabel);
	gtk_object_set_data(GTK_OBJECT(prefswin), "desclabel", desclabel);
	
	gtk_widget_show (table1);
	
	frame = gtk_frame_new (_("Selected Theme"));
	gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
	gtk_widget_set_usize (frame, 350, 100);
	gtk_widget_show (frame);
	gtk_container_add (GTK_CONTAINER(frame), table1);
	
	table = gtk_table_new (2, 2, FALSE);
	gtk_table_attach (GTK_TABLE(table), themelist, 0, 1, 0, 2,
			  GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 3, 3);
	gtk_table_attach (GTK_TABLE(table), label, 1, 2, 0, 1,
			  GTK_EXPAND | GTK_FILL, GTK_FILL, 3, 3);
	gtk_table_attach (GTK_TABLE(table), frame, 1, 2, 1, 2,
			  GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 3, 3);
	return(table);
}

GtkWidget *new_image_area(GtkWidget *prefswin) 
{
	extern gmonopprefs *prefs;
	GtkWidget *table, *temp;
	images *im = prefs->theme->im;
	
	table = gtk_table_new (2, 2, FALSE);
	
	temp = gtk_pixmap_new (im->deeds[4]->pixmapbase, im->deeds[4]->maskbase);
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 0, 1,
			 0, 1,
			 GTK_FILL, GTK_FILL,
			 5, 0);

	temp = gtk_check_button_new_with_label("Draw title deeds");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(temp), prefs->drawdeeds);
	gtk_object_set_data(GTK_OBJECT(prefswin), "drawdeeds", temp);

	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 1, 2,
			 0, 1,
			 GTK_FILL, GTK_FILL,
			 5, 0);

	temp = gtk_pixmap_new (im->flags[3]->pixmapbase, im->flags[3]->maskbase);
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 0, 1,
			 1, 2,
			 GTK_FILL, GTK_FILL,
			 5, 0);
	
	temp = gtk_check_button_new_with_label("Draw ownership flags");
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp,
			 1, 2,
			 1, 2,
			 GTK_FILL, GTK_FILL,
			 5, 0);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(temp), prefs->drawflags);
	gtk_object_set_data(GTK_OBJECT(prefswin), "drawflags", temp);
	
	return table;
}

GtkWidget *new_server_area(GtkWidget *prefswin) 
{
	GtkWidget *table, *temp;
	extern gmonopprefs *prefs;
	
	table = gtk_table_new (2, 2, FALSE);
	
	temp = gtk_label_new(_("monopd binary location:"));
	gtk_widget_show(temp);
	gtk_table_attach(GTK_TABLE(table), temp, 0, 1, 0, 1, 0, 0, 5, 0);

	temp = gtk_entry_new();
	gtk_widget_show(temp);
	gtk_entry_set_text(GTK_ENTRY(temp), prefs->monopdpath);
	gtk_entry_set_editable(GTK_ENTRY(temp), TRUE);
	gtk_object_set_data(GTK_OBJECT(prefswin), "monopdentry", temp);
	gtk_table_attach(GTK_TABLE(table), temp, 1, 2, 0, 1, GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			 GTK_FILL | GTK_EXPAND | GTK_SHRINK, 5, 0);
	

	return table;
}

void on_select_row(GtkCList *themelist, gint row, gint column,
		   GdkEvent *event, GtkWidget *prefswin)
{
	GtkWidget *widget;
	theme *t;
	t = gtk_clist_get_row_data(GTK_CLIST(themelist), row);
	gtk_object_set_data(GTK_OBJECT(prefswin), "theme", t);

	widget = gtk_object_get_data(GTK_OBJECT(prefswin), "namelabel");
	gtk_label_set_text(GTK_LABEL(widget), _(t->name));
	
	widget = gtk_object_get_data(GTK_OBJECT(prefswin), "authlabel");
	gtk_label_set_text(GTK_LABEL(widget), _(t->author));
	
	widget = gtk_object_get_data(GTK_OBJECT(prefswin), "desclabel");
	/* clear the textbox */
	gtk_text_backward_delete(GTK_TEXT(widget),
				 gtk_text_get_length(GTK_TEXT(widget)));
	gtk_text_insert(GTK_TEXT(widget), NULL, NULL, NULL, t->description, -1);
}

void on_prefs_ok_clicked (GtkButton *button, GtkWidget *prefswin)
{
	extern GtkWidget *mainwin;

	on_prefs_apply_clicked(button, prefswin);
	update_board();

	on_prefswin_die(prefswin, NULL, mainwin);
}

void on_prefs_apply_clicked (GtkButton *button, GtkWidget *prefswin) 
{
	extern gmonopprefs *prefs;

	GtkWidget *drawflags, *drawdeeds;
	theme *t;

	drawflags = gtk_object_get_data(GTK_OBJECT(prefswin), "drawflags");
	drawdeeds = gtk_object_get_data(GTK_OBJECT(prefswin), "drawdeeds");

	prefs->drawflags = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(drawflags));
	prefs->drawdeeds = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(drawdeeds));	
	
	if((t = gtk_object_get_data(GTK_OBJECT(prefswin), "theme"))) {
		prefs->theme = t;
		init_pixmaps();
		init_pixbufs();
		init_board();
		set_geometry();
	}
}
		

void on_prefs_cancel_clicked (GtkButton *button, GtkWidget *prefswin)
{
	extern GtkWidget *mainwin;
	on_prefswin_die(prefswin, NULL, mainwin);
}	

gboolean on_prefswin_die (GtkWidget *prefswin, GdkEvent *event, GtkWidget *mainwin) 
{
	gtk_object_set_data(GTK_OBJECT(mainwin), "prefswin", NULL);
	gtk_widget_destroy(prefswin);
	return TRUE;
}
