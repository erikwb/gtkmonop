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

#include <glib.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <libgmonopd/libgmonopd.h>

#include "stub.h"
#include "structs.h"
#include "client.h"
#include "misc_functions.h"
#include "assetbox.h"
#include "gdk_functions.h"
#include "notebook.h"
#include "xpms.h"
#include "game_functions.h"

/* local function prototypes */
static GtkWidget *new_gameboard_tab(GtkWidget *);
static GtkWidget *new_assets_tab(GtkWidget *);
static GtkWidget *new_client_tab(GtkWidget *);
static void new_connectdialog(char *, int, GtkWidget *);
static gint connectdialog_timeout (GtkWidget *);
static void on_startgame_clicked(GtkButton *, gpointer);
static void on_endgame_clicked(GtkButton *, gpointer);
static void on_joingame_clicked(GtkButton *, gpointer);
static void on_newgame_clicked(GtkButton *, gpointer);
static void on_setdesc_clicked(GtkButton *, gpointer);
static void on_server_clicked(GtkButton *, GtkWidget *);
static void on_connect_clicked(GtkButton *, GtkWidget *);
static void on_select_row(GtkCList *, gint, gint, GdkEvent *, gpointer);
static void on_tab_change(GtkNotebook *, GtkNotebookPage *, gint, gpointer);

/* external function bodies */
GtkWidget *new_notebook(GtkWidget *mainwin) 
{
	extern gmonopprefs *prefs;
	images *im = prefs->theme->im;
	GdkPixmap *pix;
	GdkBitmap *bmap;
	GtkWidget *notebook;
	GtkWidget *tab, *temp;
	
	/* make the notebook */
	notebook = gtk_notebook_new ();
	gtk_object_set_data (GTK_OBJECT (mainwin), "notebook", notebook);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
	gtk_notebook_popup_disable(GTK_NOTEBOOK(notebook));
	gtk_widget_show (notebook);

	/* gameboard tab */
	tab = new_gameboard_tab(mainwin);
	pix = gdk_pixmap_create_from_xpm_d(mainwin->window, &bmap, NULL, 
					   smallboard_xpm);
	temp = pixmap_label(pix, bmap, "Game board");
	gtk_widget_show (temp);
	gtk_object_set_data(GTK_OBJECT(mainwin), "gameboardtab", tab);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tab, temp);

	/* assets tab */
	tab = new_assets_tab(mainwin);
	temp = pixmap_label(im->deeds[4]->pixmapbase, im->deeds[4]->maskbase,
			    "Assets");
	gtk_widget_show(temp);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tab, temp);
	gtk_object_set_data(GTK_OBJECT(mainwin), "assettab", tab);
	gtk_object_set_data(GTK_OBJECT(notebook), "assettab", tab);
	gtk_object_set_data(GTK_OBJECT(tab), "label", 
			    gtk_object_get_data(GTK_OBJECT(temp), "label"));
	gtk_object_set_data(GTK_OBJECT(mainwin), "assettablabel", 
			    gtk_object_get_data(GTK_OBJECT(temp), "label"));
	
	/* client tab */
	tab = new_client_tab(mainwin);
	gtk_container_add (GTK_CONTAINER (notebook), tab);
	temp = gtk_label_new(_("Client"));
	gtk_widget_show(temp);
	gtk_object_set_data(GTK_OBJECT(mainwin), "clienttab", tab);

	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook),
				    gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 2),
				    temp);

	gtk_signal_connect (GTK_OBJECT(notebook), "switch-page",
			    GTK_SIGNAL_FUNC (on_tab_change),
			    NULL);

	return notebook;
}

void update_gamelist(void) 
{
	extern gamelist *gameslist;
	extern GtkWidget *mainwin;
	game *games;
	GtkWidget *joinlist;
	
	char buf1[20], buf2[512], buf3[20], *bufx[3] = { buf1, buf2, buf3 };

	joinlist = gtk_object_get_data(GTK_OBJECT(mainwin), "gamelist");
	gtk_clist_clear(GTK_CLIST(joinlist));
	gtk_clist_freeze(GTK_CLIST(joinlist));
	for(games = gameslist->games; 
	    games != (game *)NULL; 
	    games = games->next) {
		strcpy(buf1, (char *)(games->id));
		strcpy(buf2, (char *)(games->name));
		strcpy(buf3, (char *)(games->players));
		gtk_clist_append(GTK_CLIST(joinlist), bufx);
	}
	
	gtk_clist_thaw(GTK_CLIST(joinlist));
}

/* local function bodies */
GtkWidget *new_gameboard_tab(GtkWidget *mainwin) 
{
	GtkWidget *drawingarea, *notebook;
	
	drawingarea = gtk_drawing_area_new();
	gtk_widget_ref(drawingarea);
	gtk_widget_set_events (drawingarea, GDK_EXPOSURE_MASK 
			       | GDK_BUTTON_PRESS_MASK);
	gtk_widget_ref (drawingarea);
	gtk_object_set_data_full (GTK_OBJECT (mainwin), "drawingarea", drawingarea,
				  (GtkDestroyNotify) gtk_widget_unref);

	notebook = gtk_object_get_data(GTK_OBJECT(mainwin), "notebook");
	gtk_object_set_data(GTK_OBJECT(drawingarea), "notebook", notebook);
	
	gtk_drawing_area_size(GTK_DRAWING_AREA(drawingarea), 444, 444);
	gtk_widget_show(drawingarea);

	gtk_signal_connect (GTK_OBJECT(drawingarea), 
			    "button_press_event",
			    GTK_SIGNAL_FUNC(board_button_press_event), 
			    (gpointer)(NULL));

	return drawingarea;
}

GtkWidget *new_assets_tab(GtkWidget *mainwin) 
{
	GtkWidget *scrolledwindow, *viewport;
	GtkWidget *assetbox;

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);

	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), 
					GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

	viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

	assetbox = gtk_vbox_new (TRUE, 4);
	gtk_widget_show (assetbox);
	gtk_object_set_data(GTK_OBJECT(mainwin), "assetbox", assetbox);
	gtk_container_add(GTK_CONTAINER(viewport), assetbox);
	gtk_box_set_homogeneous(GTK_BOX(assetbox), FALSE);	

	GTK_WIDGET_UNSET_FLAGS (assetbox, GTK_CAN_FOCUS);
	GTK_WIDGET_UNSET_FLAGS (assetbox, GTK_CAN_DEFAULT);

	return scrolledwindow;
}

GtkWidget *new_client_tab(GtkWidget *mainwin) 
{
	GtkWidget *temp;
	GtkWidget *joinframe, *table, *hbox, *vbox, *scrolledwindow, *descentry;
	GtkWidget *connectframe, *table2, *vbox2, *hostcombo, *hostentry, *portentry;
	GtkWidget *nickentry, *portcombo, *hbox2;
	GtkWidget *gamelist, *connectlabel;
	GtkWidget *buttonbox, *buttonbox2, *buttonbox3;
	GtkWidget *startgame, *endgame, *joingame, *newgame, *setdesc;
	GtkWidget *startserver, *connect;
	GList *hostcombo_items = NULL;
	GList *portcombo_items = NULL;
	
	table = gtk_table_new (2, 1, FALSE);
	gtk_widget_show (table);
	
	/* joingame frame */
	joinframe = gtk_frame_new (_("Join Game"));
	gtk_widget_show (joinframe);
	gtk_table_attach (GTK_TABLE (table), joinframe, 0, 1, 1, 2,
			  (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
			  (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL), 0, 0);
	gtk_container_set_border_width (GTK_CONTAINER (joinframe), 3);
	gtk_frame_set_label_align (GTK_FRAME (joinframe), 0.02, 0.5);
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (joinframe), vbox);

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), 
					GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);

	/* game list */
	gamelist = gtk_clist_new (3);
	gtk_widget_ref (gamelist);
	gtk_object_set_data(GTK_OBJECT(mainwin), "gamelist", gamelist);
	gtk_widget_show (gamelist);

	gtk_container_add (GTK_CONTAINER (scrolledwindow), gamelist);
	gtk_clist_set_column_width (GTK_CLIST (gamelist), 0, 34);
	gtk_clist_set_column_width (GTK_CLIST (gamelist), 1, 231);
	gtk_clist_set_column_width (GTK_CLIST (gamelist), 2, 80);
	gtk_clist_column_titles_show (GTK_CLIST (gamelist));

	gtk_signal_connect (GTK_OBJECT(gamelist), "select-row",
			    GTK_SIGNAL_FUNC (on_select_row),
			    NULL);

	temp = gtk_label_new (_("ID"));
	gtk_widget_show (temp);
	gtk_clist_set_column_widget (GTK_CLIST (gamelist), 0, temp);

	temp = gtk_label_new (_("Description"));
	gtk_widget_show (temp);
	gtk_clist_set_column_widget (GTK_CLIST (gamelist), 1, temp);

	temp = gtk_label_new (_("Players"));
	gtk_widget_show (temp);
	gtk_clist_set_column_widget (GTK_CLIST (gamelist), 2, temp);

	/* buttons */
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

	descentry = gtk_entry_new ();
	gtk_object_set_data (GTK_OBJECT (mainwin), "descentry", descentry);
	gtk_widget_show (descentry);
	gtk_box_pack_start (GTK_BOX (hbox), descentry, TRUE, TRUE, 0);
	
	buttonbox = gtk_hbutton_box_new ();
	gtk_widget_show (buttonbox);
	gtk_box_pack_start (GTK_BOX (hbox), buttonbox, FALSE, FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (buttonbox), 5);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox), GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (buttonbox), 0);
	gtk_button_box_set_child_size (GTK_BUTTON_BOX (buttonbox), 20, 27);
	gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (buttonbox), 0, 0);

	setdesc = gtk_button_new_with_label (_("Set Description"));
	gtk_widget_show (setdesc);
	gtk_object_set_data(GTK_OBJECT(mainwin), "setdesc", setdesc);
	GTK_WIDGET_SET_FLAGS (setdesc, GTK_CAN_DEFAULT);
	gtk_container_add (GTK_CONTAINER (buttonbox), setdesc);

	buttonbox2 = gtk_hbutton_box_new ();
	gtk_widget_show (buttonbox2);
	gtk_box_pack_start (GTK_BOX (vbox), buttonbox2, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (buttonbox2), 5);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox2), GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (buttonbox2), 0);
	gtk_button_box_set_child_size (GTK_BUTTON_BOX (buttonbox2), 20, 27);
	gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (buttonbox2), 0, 0);

	startgame = gtk_button_new_with_label (_("Start Game"));
	gtk_widget_show (startgame);
	gtk_object_set_data(GTK_OBJECT(mainwin), "startgame", startgame);
	GTK_WIDGET_SET_FLAGS (startgame, GTK_CAN_DEFAULT);
	gtk_container_add (GTK_CONTAINER (buttonbox2), startgame);

	joingame = gtk_button_new_with_label (_("Join Game"));
	gtk_widget_show (joingame);
	gtk_object_set_data(GTK_OBJECT(mainwin), "joingame", joingame);
	GTK_WIDGET_SET_FLAGS (joingame, GTK_CAN_DEFAULT);
	gtk_container_add (GTK_CONTAINER (buttonbox2), joingame);

	newgame = gtk_button_new_with_label (_("New Game"));
	gtk_widget_show (newgame);
	gtk_object_set_data(GTK_OBJECT(mainwin), "newgame", newgame);
	GTK_WIDGET_SET_FLAGS (newgame, GTK_CAN_DEFAULT);
	gtk_container_add (GTK_CONTAINER (buttonbox2), newgame);

	gtk_widget_set_sensitive(gamelist, FALSE);
	gtk_widget_set_sensitive(newgame, FALSE);
	gtk_widget_set_sensitive(joingame, FALSE);
	gtk_widget_set_sensitive(startgame, FALSE);
	gtk_widget_set_sensitive(setdesc, FALSE);
	gtk_widget_set_sensitive(descentry, FALSE);

	/* connect frame */
	connectframe = gtk_frame_new (_("Connect"));
	gtk_widget_show (connectframe);
	gtk_table_attach (GTK_TABLE (table), connectframe, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL), 0, 0);
	gtk_container_set_border_width (GTK_CONTAINER (connectframe), 3);
	gtk_frame_set_label_align (GTK_FRAME (connectframe), 0.02, 0.5);

	vbox2 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox2);
	gtk_container_add (GTK_CONTAINER (connectframe), vbox2);

	table2 = gtk_table_new (2, 4, FALSE);
	gtk_widget_show (table2);
	gtk_box_pack_start (GTK_BOX (vbox2), table2, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (table2), 6);
	gtk_table_set_col_spacings (GTK_TABLE (table2), 6);

	temp = gtk_label_new (_("Host"));
	gtk_widget_show (temp);
	gtk_table_attach (GTK_TABLE (table2), temp, 0, 1, 0, 1,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (temp), 0, 0.5);

	temp = gtk_label_new (_("Nick"));
	gtk_widget_show (temp);
	gtk_table_attach (GTK_TABLE (table2), temp, 0, 1, 1, 2,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (temp), 0, 0.5);

	hostcombo = gtk_combo_new ();
	gtk_widget_show (hostcombo);
	gtk_table_attach (GTK_TABLE (table2), hostcombo, 1, 2, 0, 1,
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);
	hostcombo_items = g_list_append (hostcombo_items, 
					 (gpointer) _("localhost"));
	hostcombo_items = g_list_append (hostcombo_items, 
					 (gpointer) _("odinnet.ath.cx"));
	hostcombo_items = g_list_append (hostcombo_items, 
					 (gpointer) _("monopd.capsi.com"));
	gtk_combo_set_popdown_strings (GTK_COMBO (hostcombo), hostcombo_items);
	g_list_free (hostcombo_items);

	/* host, nick entries */

	hostentry = GTK_COMBO (hostcombo)->entry;
	gtk_object_set_data (GTK_OBJECT (mainwin), "hostentry", hostentry);
	gtk_widget_show (hostentry);
	gtk_entry_set_text (GTK_ENTRY (hostentry), _("localhost"));

	nickentry = gtk_entry_new ();
	gtk_object_set_data (GTK_OBJECT (mainwin), "nickentry", nickentry);
	gtk_widget_show (nickentry);
	gtk_table_attach (GTK_TABLE (table2), nickentry, 1, 4, 1, 2,
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);
	gtk_entry_set_text (GTK_ENTRY (nickentry), _("Abner"));

	temp = gtk_label_new (_("Port"));
	gtk_widget_show (temp);
	gtk_table_attach (GTK_TABLE (table2), temp, 2, 3, 0, 1,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (temp), 0, 0.5);

	portcombo = gtk_combo_new ();
	gtk_widget_show (portcombo);
	gtk_table_attach (GTK_TABLE (table2), portcombo, 3, 4, 0, 1,
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize (portcombo, 1, -2);
	portcombo_items = g_list_append (portcombo_items, 
					 (gpointer) _("1234"));
	portcombo_items = g_list_append (portcombo_items, 
					 (gpointer) _("1235"));
	gtk_combo_set_popdown_strings (GTK_COMBO (portcombo), portcombo_items);
	g_list_free (portcombo_items);

	portentry = GTK_COMBO (portcombo)->entry;
	gtk_object_set_data (GTK_OBJECT (mainwin), "portentry", portentry);
	gtk_widget_show (portentry);
	gtk_widget_set_usize (portentry, 110, -2);
	gtk_entry_set_text (GTK_ENTRY (portentry), _("1234"));

	/* buttonbox */

	hbox2 = gtk_hbox_new(FALSE, 4);
	gtk_widget_show(hbox2);
	gtk_box_pack_start (GTK_BOX (vbox2), hbox2, TRUE, TRUE, 0);
	connectlabel = gtk_label_new("");
	gtk_widget_show(connectlabel);
	gtk_object_set_data(GTK_OBJECT(mainwin), "connectstatuslabel",
			    connectlabel);
	gtk_box_pack_start (GTK_BOX (hbox2), connectlabel, TRUE, TRUE, 0);

	buttonbox3 = gtk_hbutton_box_new ();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(buttonbox3), 
				  GTK_BUTTONBOX_END);
	gtk_widget_show (buttonbox3);
	gtk_box_pack_start (GTK_BOX (hbox2), buttonbox3, FALSE, FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (buttonbox3), 5);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox3), 
				   GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (buttonbox3), 0);
	gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (buttonbox3), 0, 0);

	startserver = gtk_button_new_with_label(_("Start server"));
	gtk_widget_show(startserver);
	gtk_object_set_data(GTK_OBJECT(mainwin), "startserver", startserver);
	gtk_container_add(GTK_CONTAINER(buttonbox3), startserver);
	GTK_WIDGET_SET_FLAGS(startserver, GTK_CAN_DEFAULT);

	connect = gtk_button_new_with_label (_("Connect"));
	gtk_widget_show (connect);
	gtk_container_add (GTK_CONTAINER (buttonbox3), connect);
	GTK_WIDGET_SET_FLAGS (connect, GTK_CAN_DEFAULT);
	gtk_object_set_data(GTK_OBJECT(mainwin), "connectbutton", connect);

	/* connect button signals */

	gtk_signal_connect (GTK_OBJECT(startgame), "clicked",
			    GTK_SIGNAL_FUNC (on_startgame_clicked),
			    NULL);
/*	gtk_signal_connect (GTK_OBJECT(endgame), "clicked",
			    GTK_SIGNAL_FUNC (on_endgame_clicked),
			    NULL); */
	gtk_signal_connect (GTK_OBJECT(joingame), "clicked",
			    GTK_SIGNAL_FUNC (on_joingame_clicked),
			    gamelist);	
	gtk_signal_connect (GTK_OBJECT(newgame), "clicked",
			    GTK_SIGNAL_FUNC (on_newgame_clicked),
			    NULL);
	gtk_signal_connect (GTK_OBJECT(setdesc), "clicked",
			    GTK_SIGNAL_FUNC (on_setdesc_clicked),
			    descentry);
	gtk_signal_connect (GTK_OBJECT(startserver), "clicked",
			    GTK_SIGNAL_FUNC (on_server_clicked),
			    connectlabel);
	gtk_signal_connect (GTK_OBJECT (connect), "clicked",
			    GTK_SIGNAL_FUNC (on_connect_clicked),
			    mainwin);
	return table;
}

void new_connectdialog(char *server, int port, GtkWidget *mainwin) 
{
	GtkWidget *connectdialog, *vbox, *progressbar, *cancel, *label;
	guint *timeouttag;
	char buf[1024];

	if(gtk_object_get_data(GTK_OBJECT(mainwin), "connectdialog")) return;
	
	sprintf(buf, "%s:%d", server, port);

	connectdialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (connectdialog), _("Connecting ..."));
	gtk_window_set_policy (GTK_WINDOW (connectdialog), FALSE, FALSE, 
			       FALSE);
	
	gtk_object_set_data(GTK_OBJECT(mainwin), "connectdialog",
			    connectdialog);
	
	vbox = gtk_vbox_new(TRUE, 4);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(connectdialog), vbox);
	
	label = gtk_label_new(_(buf));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

	progressbar = gtk_progress_bar_new ();
	gtk_progress_set_activity_mode (GTK_PROGRESS(progressbar),
					TRUE);
	gtk_widget_show (progressbar);
	gtk_object_set_data(GTK_OBJECT(connectdialog), "progressbar",
			    progressbar);
	gtk_box_pack_start (GTK_BOX(vbox), progressbar, TRUE, TRUE, 0);
	
	cancel = gtk_button_new_with_label("Cancel");
	gtk_widget_show(cancel);
	gtk_box_pack_start (GTK_BOX(vbox), cancel, TRUE, TRUE, 0);
	gtk_object_set_data(GTK_OBJECT(cancel), "dialog", connectdialog);
	
	timeouttag = malloc(sizeof(guint));
	*timeouttag = gtk_timeout_add (20, (GtkFunction)connectdialog_timeout,
				       mainwin);
	gtk_object_set_data(GTK_OBJECT(mainwin), "timeouttag", timeouttag);
	
	gtk_signal_connect (GTK_OBJECT(cancel), "clicked",
			    GTK_SIGNAL_FUNC(connectdialog_destroy), NULL);
	gtk_widget_show(connectdialog);
}

void connectdialog_destroy (void)
{
	extern GtkWidget *mainwin;
	GtkWidget *connectdialog;
	guint *timeouttag;
	
	connectdialog = gtk_object_get_data(GTK_OBJECT(mainwin), 
					    "connectdialog");
	timeouttag = gtk_object_get_data(GTK_OBJECT(mainwin), "timeouttag");

	gtk_timeout_remove (*timeouttag);
	*timeouttag = 0;
	free(timeouttag);
	gtk_object_set_data(GTK_OBJECT(mainwin), "timeouttag", NULL);
	
	if (!connectdialog) return;
	gtk_object_set_data(GTK_OBJECT(connectdialog), "progressbar", NULL);
	gtk_object_set_data(GTK_OBJECT(mainwin), "connectdialog", NULL);
	gtk_widget_destroy(connectdialog);
}

gint connectdialog_timeout (GtkWidget *mainwin)
{
	GtkWidget *progressbar;
	GtkWidget *connectdialog;
	
	GtkAdjustment *adj;

	connectdialog = gtk_object_get_data(GTK_OBJECT(mainwin), 
					    "connectdialog");
	progressbar = gtk_object_get_data(GTK_OBJECT(connectdialog), 
					  "progressbar");
	if(progressbar == NULL) return FALSE;
	
	adj = GTK_PROGRESS(progressbar)->adjustment;
	gtk_progress_set_value (GTK_PROGRESS(progressbar),
				(adj->value+1)>adj->upper ?
				adj->lower : adj->value+1);
	return TRUE;
}

void on_startgame_clicked(GtkButton *foo, gpointer bar) {
	client_outmessage(OUT_GAMESTART, NULL);
}

void on_endgame_clicked(GtkButton *foo, gpointer bar) {
	//	client_outmessage(OUT_ENDGAME, NULL);
}

void on_joingame_clicked(GtkButton *foo, gpointer gamelist) 
{
	/* check if I should be mallocing the id */
	char *id;
	int *pk_row;

	pk_row = gtk_object_get_data(GTK_OBJECT(gamelist), "pk_row");
	if(!pk_row) return;
	if(pk_row < 0) return;
	
	gtk_clist_get_text(GTK_CLIST(gamelist), *pk_row, 0, &id);
	client_outmessage(OUT_JOINGAME, id);
}

void on_newgame_clicked(GtkButton *foo, gpointer bar) 
{
	client_outmessage(OUT_GAMENEW, NULL);
}

void on_setdesc_clicked(GtkButton *foo, gpointer descentry) {
	char *desc;
	desc = gtk_editable_get_chars(GTK_EDITABLE(descentry), 0, -1);
	client_outmessage(OUT_GAMEDESC, desc);
}

void on_server_clicked(GtkButton *button, GtkWidget *connectlabel) 
{
	extern gmonopd *server;
	extern GtkWidget *mainwin;
	extern int connected;

	if(server) {
		if(connected) on_connect_clicked(NULL, mainwin);
		gmonopd_kill(server);
		server = NULL;
		gtk_label_set_text(GTK_LABEL(button->child),
				   "Start server");
		gtk_label_set_text(GTK_LABEL(connectlabel),
				   "Stopped server.");
	} else {
		/* disconnect from current game */
		if(connected) on_connect_clicked(NULL, mainwin);
		server = gmonopd_init(1234);
		if(server) { 
			gmonopd_run(server);
			gtk_label_set_text(GTK_LABEL(button->child),
					   "Stop server");
			gtk_label_set_text(GTK_LABEL(connectlabel),
					   "Started server.");
			/* connect to server */
			on_connect_clicked(NULL, mainwin);
		} else
			gtk_label_set_text(GTK_LABEL(connectlabel), "Error starting server.");
	}
}

void on_connect_clicked(GtkButton *foo, GtkWidget *mainwin) 
{
	char *host;
	int port;
	extern int connected;
	GtkWidget *hostentry, *portentry, *temp;

	if(!connected) {
		hostentry = gtk_object_get_data(GTK_OBJECT(mainwin),
						"hostentry");
		portentry = gtk_object_get_data(GTK_OBJECT(mainwin),
						"portentry");
		
		host = gtk_editable_get_chars(GTK_EDITABLE(hostentry), 0, -1);
		port = atoi(gtk_editable_get_chars(GTK_EDITABLE(portentry), 
						   0, -1));
		client_init(host, port);
		connected = 0;
		new_connectdialog(host, port, mainwin);

		g_free(host);
	} else {
		client_outmessage(OUT_DISCONNECT, NULL);
		client_disconnect();
		temp = gtk_object_get_data(GTK_OBJECT(mainwin),
					   "playerlist");
		gtk_clist_clear(GTK_CLIST(temp));

		temp = gtk_object_get_data(GTK_OBJECT(mainwin),
					   "connectbutton");
		gtk_label_set_text(GTK_LABEL(GTK_BUTTON(temp)->child),
				   "Connect");
		
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "hostentry");
		gtk_widget_set_sensitive(temp->parent, TRUE);
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "portentry");
		gtk_widget_set_sensitive(temp->parent, TRUE);

		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "startgame");
		gtk_widget_set_sensitive(temp, FALSE);
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "joingame");
		gtk_widget_set_sensitive(temp, FALSE);
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "newgame");
		gtk_widget_set_sensitive(temp, FALSE);
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "setdesc");
		gtk_widget_set_sensitive(temp, FALSE);
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "descentry");
		gtk_widget_set_sensitive(temp, FALSE);
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "gamelist");
		gtk_widget_set_sensitive(temp, FALSE);

		temp = gtk_object_get_data(GTK_OBJECT(mainwin),
					   "connectstatuslabel");
		gtk_label_set(GTK_LABEL(temp), "Disconnected from server.");
		initialize_client();
	}
}

void on_select_row(GtkCList *gamelist, gint row, gint column,
		   GdkEvent *event, gpointer user_data)
{
	int *moo;
	
	moo = gtk_object_get_data(GTK_OBJECT(gamelist), "pk_row");
	if(moo)
		free(moo);
	moo = malloc(sizeof(int));
	*moo = row;
	gtk_object_set_data(GTK_OBJECT(gamelist), "pk_row", moo);
}

void on_tab_change(GtkNotebook *notebook, GtkNotebookPage *page, 
		   gint pagenum, gpointer user_data) 
{
	extern GtkStyle *inactive_text_style;
	GtkWidget *tablabel;

	trade *t;
	auction *a;
	
	if((t = gtk_object_get_data(GTK_OBJECT(page->child), "trade")))
		gtk_widget_set_style (t->tablabel, inactive_text_style);
	else if((a = gtk_object_get_data(GTK_OBJECT(page->child), "auction")))
		gtk_widget_set_style (a->tablabel, inactive_text_style);
	else if (page->child == gtk_object_get_data(GTK_OBJECT(notebook), 
						    "assettab")) {
		tablabel = gtk_object_get_data(GTK_OBJECT(page->child), 
					       "label");
		gtk_widget_set_style (tablabel, inactive_text_style);
	}
}
