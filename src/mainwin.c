
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

/* mainwin.c - the main gtkmonop window */

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <stdlib.h>

#include "structs.h"
#include "stub.h"
#include "client.h"
#include "notebook.h"
#include "aboutwin.h"
#include "prefswin.h"
#include "misc_functions.h"
#include "gdk_functions.h"
#include "xpms.h"
#include "mainwin.h"

/* local function prototypes */
static GtkWidget *new_playerlist(GtkWidget *);
static GtkWidget *new_textframe(GtkWidget *);
static GtkWidget *new_chatframe(GtkWidget *);
static GtkWidget *new_playerlist_popup(player *);
static gboolean show_playerlist_popup (GtkWidget *, GdkEventButton *, gpointer);
static void show_aboutwin(GtkMenuItem *, gpointer);
static void show_prefswin(GtkMenuItem *, GtkWidget *);
static void on_chatentry_activated (GtkEditable *, gpointer);
static void on_roll_clicked (GtkButton *, gpointer);
static void on_buy_clicked (GtkButton *, gpointer);
static void on_auction_clicked (GtkButton *, gpointer);
static void on_endturn_clicked (GtkButton *, gpointer);
static void on_payjail_clicked (GtkButton *, gpointer);
static void on_jailroll_clicked (GtkButton *, gpointer);
static void on_jailcard_clicked (GtkButton *, gpointer);
static void on_paydebt_clicked (GtkButton *button, gpointer user_data);
static void on_declarebankrupt_clicked (GtkButton *button, gpointer user_data);
static void on_trade_activated(GtkWidget *, player *);
static void win_set_icon(GtkWidget *win);

/* external function bodies */
GtkWidget *new_mainwin() 
{
	GtkWidget *mainwin;
	GtkAccelGroup *accel_group, *gamemenu_accels, *helpmenu_accels;
	GtkWidget *table, *table2;
	GtkWidget *handlebox, *menubar;
	GtkWidget *gameitem, *prefsitem, *helpitem, *quititem, *aboutitem;
	GtkWidget *gamemenu, *helpmenu;
	GtkWidget *buttonbox;
	GtkWidget *roll, *buy, *endturn, *payjail, *jailroll;
	GtkWidget *jailcard, *auction;
	GtkWidget *paydebt, *declarebankrupt;
	GtkWidget *statusbar, *temp;
	GtkWidget *chatentry;
	GtkWidget *notebook;
	
	guint tmp_key;

	accel_group = gtk_accel_group_new ();

	/* create the window */
	mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (mainwin), _("GTKmonop"));
	gtk_window_set_policy (GTK_WINDOW (mainwin), FALSE, TRUE, FALSE);

        /* this gives it a GDK window so pixmaps can be made off of it */
	gtk_widget_realize(mainwin);
	
	table = gtk_table_new (4, 2, FALSE);
	gtk_table_set_col_spacings(GTK_TABLE(table), 3);
	gtk_widget_show (table);
	gtk_container_add (GTK_CONTAINER (mainwin), table);
	
	/* menus ahead */
	handlebox = gtk_handle_box_new ();
	gtk_widget_show (handlebox);
	gtk_table_attach (GTK_TABLE (table), handlebox, 0, 2, 0, 1,
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			  (GtkAttachOptions) (GTK_FILL), 0, 0);
	
	menubar = gtk_menu_bar_new ();
	gtk_widget_show (menubar);
	gtk_container_add (GTK_CONTAINER (handlebox), menubar);

	/* Game menu */
	gamemenu = gtk_menu_new ();
	
	gameitem = gtk_menu_item_new_with_label ("");
	tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (gameitem)->child),
					 _("_Game"));
	gtk_widget_add_accelerator (gameitem, "activate_item", accel_group,
				    tmp_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
	gtk_widget_show (gameitem);
	gtk_container_add (GTK_CONTAINER (menubar), gameitem);
	
	prefsitem = gtk_menu_item_new_with_label ("");
	tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN(prefsitem)->child),
					 _("_Preferences"));
	gtk_widget_add_accelerator (prefsitem, "activate_item", accel_group,
				    tmp_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
	gtk_widget_show (prefsitem);
	gtk_container_add (GTK_CONTAINER (gamemenu), prefsitem);
	
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (gameitem), gamemenu);
	gamemenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (gamemenu));

	quititem  = gtk_menu_item_new_with_label ("");
	tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (quititem)->child),
					 _("_Quit"));
	gtk_widget_add_accelerator (quititem, "activate_item", gamemenu_accels,
				    tmp_key, 0, 0);
	gtk_widget_show (quititem);
	gtk_container_add (GTK_CONTAINER (gamemenu), quititem);

	/* Help menu */
	helpitem = gtk_menu_item_new_with_label ("");
	tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (helpitem)->child),
					 _("_Help"));
	gtk_widget_add_accelerator (helpitem, "activate_item", accel_group,
				    tmp_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
	gtk_menu_item_right_justify(GTK_MENU_ITEM(helpitem));
	gtk_widget_show (helpitem);
	gtk_container_add (GTK_CONTAINER (menubar), helpitem);

	helpmenu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (helpitem), helpmenu);
	helpmenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (helpmenu));

	aboutitem = gtk_menu_item_new_with_label (_("About..."));
	gtk_widget_show (aboutitem);
	gtk_container_add (GTK_CONTAINER (helpmenu), aboutitem);

	/* Buttonbox */
	handlebox = gtk_handle_box_new ();
	gtk_widget_show (handlebox);
	gtk_table_attach (GTK_TABLE (table), handlebox, 0, 2, 1, 2,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (GTK_FILL), 0, 0);
	
	buttonbox = gtk_hbutton_box_new ();
	gtk_object_set_data (GTK_OBJECT (mainwin), "buttonbox", buttonbox);
	gtk_widget_show (buttonbox);
	gtk_widget_set_usize (buttonbox, -2, 42);
	gtk_container_add (GTK_CONTAINER (handlebox), buttonbox);

	gtk_container_set_border_width (GTK_CONTAINER (buttonbox), 5);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox), GTK_BUTTONBOX_START);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (buttonbox), 0);
	gtk_button_box_set_child_size (GTK_BUTTON_BOX (buttonbox), 65, 27);
	gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (buttonbox), 0, 0);

	roll = gtk_button_new_with_label (_("Roll"));
	gtk_widget_show (roll);
	gtk_widget_ref(roll);
	gtk_object_set_data(GTK_OBJECT(buttonbox), "roll", roll);
	GTK_WIDGET_SET_FLAGS (roll, GTK_CAN_DEFAULT);

	buy = gtk_button_new_with_label (_("Buy"));
	gtk_widget_show (buy);
	gtk_widget_ref(buy);
	gtk_object_set_data(GTK_OBJECT(buttonbox), "buy", buy);
	GTK_WIDGET_SET_FLAGS (buy, GTK_CAN_DEFAULT);

	auction = gtk_button_new_with_label(_("Auction"));
	gtk_widget_show(auction);
	gtk_widget_ref(auction);
	gtk_object_set_data(GTK_OBJECT(buttonbox), "auction", auction);
	GTK_WIDGET_SET_FLAGS (auction, GTK_CAN_DEFAULT);

	endturn = gtk_button_new_with_label (_("End Turn"));
	gtk_widget_show (endturn);
	gtk_widget_ref(endturn);
	gtk_object_set_data(GTK_OBJECT(buttonbox), "endturn", endturn);
	GTK_WIDGET_SET_FLAGS (endturn, GTK_CAN_DEFAULT);

	payjail = gtk_button_new_with_label (_("Pay Jail"));
	gtk_widget_show (payjail);
	gtk_widget_ref(payjail);
	gtk_object_set_data(GTK_OBJECT(buttonbox), "payjail", payjail);
	GTK_WIDGET_SET_FLAGS (payjail, GTK_CAN_DEFAULT);

	jailroll = gtk_button_new_with_label (_("Jail Roll"));
	gtk_widget_show (jailroll);
	gtk_widget_ref(jailroll);
	gtk_object_set_data(GTK_OBJECT(buttonbox), "jailroll", jailroll);
	GTK_WIDGET_SET_FLAGS (jailroll, GTK_CAN_DEFAULT);

	jailcard = gtk_button_new_with_label (_("Use Jail Card"));
	gtk_widget_show (jailcard);
	gtk_widget_ref(jailcard);
	gtk_object_set_data(GTK_OBJECT(buttonbox), "jailcard", jailcard);
	GTK_WIDGET_SET_FLAGS (jailcard, GTK_CAN_DEFAULT);

	paydebt = gtk_button_new_with_label (_("Pay off debt"));
	gtk_widget_show(paydebt);
	gtk_widget_ref(paydebt);
	gtk_object_set_data(GTK_OBJECT(buttonbox), "paydebt", paydebt);
	GTK_WIDGET_SET_FLAGS (paydebt, GTK_CAN_DEFAULT);

	declarebankrupt = gtk_button_new_with_label (_("Declare Bankruptcy"));
	gtk_widget_show(declarebankrupt);
	gtk_widget_ref(declarebankrupt);
	gtk_object_set_data(GTK_OBJECT(buttonbox), "declarebankrupt", declarebankrupt);
	GTK_WIDGET_SET_FLAGS (declarebankrupt, GTK_CAN_DEFAULT);

	/* statusbar */
	statusbar  = gtk_statusbar_new ();
	gtk_object_set_data (GTK_OBJECT (mainwin), "statusbar", statusbar);
	gtk_widget_show (statusbar);
	gtk_table_attach (GTK_TABLE (table), statusbar, 0, 2, 3, 4,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);

	/* notebook */
	notebook = new_notebook(mainwin);
	gtk_table_attach (GTK_TABLE (table), notebook, 0, 1, 2, 3,
			  (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
			  (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL), 0, 0);

	/* playerlist, textbox, chatbox */
	table2 = gtk_table_new (4, 1, FALSE);
	gtk_widget_show (table2);
	gtk_table_attach (GTK_TABLE (table), table2, 1, 2, 2, 3,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (GTK_FILL | GTK_SHRINK), 0, 0);

	temp = new_chatframe(mainwin);
	gtk_table_attach (GTK_TABLE (table2), temp, 0, 1, 2, 3,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL | GTK_SHRINK), 0, 0);

	temp = new_textframe(mainwin);
	gtk_table_attach (GTK_TABLE (table2), temp, 0, 1, 1, 2,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (GTK_FILL | GTK_SHRINK), 0, 0);
	
	temp = new_playerlist(mainwin);
	gtk_table_attach (GTK_TABLE (table2), temp, 0, 1, 0, 1,
			  (GtkAttachOptions) (GTK_FILL),
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

	chatentry = gtk_entry_new ();
	gtk_widget_ref (chatentry);
	gtk_object_set_data (GTK_OBJECT (mainwin), "chatentry", chatentry);
	
	gtk_widget_show (chatentry);
	gtk_table_attach (GTK_TABLE (table2), chatentry, 0, 1, 3, 4,
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize (chatentry, -2, 20);

	gtk_signal_connect (GTK_OBJECT (chatentry), "activate",
			    GTK_SIGNAL_FUNC (on_chatentry_activated),
			    NULL);

	/* connect button signals */
	gtk_signal_connect (GTK_OBJECT (mainwin), "delete_event",
			    GTK_SIGNAL_FUNC (shutdown_gtkmonop),
			    NULL);
	gtk_signal_connect (GTK_OBJECT (mainwin), "destroy_event",
			    GTK_SIGNAL_FUNC (shutdown_gtkmonop),
			    NULL);

	gtk_signal_connect (GTK_OBJECT (prefsitem), "activate",
			    GTK_SIGNAL_FUNC (show_prefswin),
			    mainwin);
	gtk_signal_connect (GTK_OBJECT (quititem), "activate",
			    GTK_SIGNAL_FUNC (shutdown_gtkmonop),
			    NULL);
	gtk_signal_connect (GTK_OBJECT (aboutitem), "activate",
			    GTK_SIGNAL_FUNC (show_aboutwin),
			    NULL);

	gtk_signal_connect (GTK_OBJECT (roll), "clicked",
			    GTK_SIGNAL_FUNC (on_roll_clicked),
			    NULL);
	gtk_signal_connect (GTK_OBJECT (buy), "clicked",
			    GTK_SIGNAL_FUNC (on_buy_clicked),
			    NULL);
	gtk_signal_connect (GTK_OBJECT (auction), "clicked",
			    GTK_SIGNAL_FUNC (on_auction_clicked),
			    NULL);
	gtk_signal_connect (GTK_OBJECT (endturn), "clicked",
			    GTK_SIGNAL_FUNC (on_endturn_clicked),
			    NULL);
	gtk_signal_connect (GTK_OBJECT (payjail), "clicked",
                      GTK_SIGNAL_FUNC (on_payjail_clicked),
			    NULL);
	gtk_signal_connect (GTK_OBJECT (jailroll), "clicked",
			    GTK_SIGNAL_FUNC (on_jailroll_clicked),
			    NULL);
	gtk_signal_connect (GTK_OBJECT (jailcard), "clicked",
			    GTK_SIGNAL_FUNC (on_jailcard_clicked),
			    NULL);
	gtk_signal_connect (GTK_OBJECT (paydebt), "clicked",
			    GTK_SIGNAL_FUNC (on_paydebt_clicked),
			    NULL);
	gtk_signal_connect (GTK_OBJECT (declarebankrupt), "clicked",
			    GTK_SIGNAL_FUNC (on_declarebankrupt_clicked),
			    NULL);

	gtk_window_add_accel_group (GTK_WINDOW (mainwin), accel_group);

	/* set the icon */
	win_set_icon(mainwin);
	
	return mainwin;
}

/* local function bodies */
GtkWidget *new_playerlist(GtkWidget *mainwin) 
{
	GtkWidget *scrolledwindow, *viewport, *playerlist, *frame;
	GtkWidget *cidlabel, *pidlabel, *namelabel;	

	frame = gtk_frame_new(_("Players"));
	gtk_widget_show(frame);
	
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), 
					GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_widget_show (scrolledwindow);
	gtk_container_add(GTK_CONTAINER(frame), scrolledwindow);
	
	viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

	playerlist = gtk_clist_new (3);
	gtk_object_set_data (GTK_OBJECT (mainwin), _("playerlist"), playerlist);
	gtk_widget_set_usize (playerlist, 200, 80);
	gtk_widget_show (playerlist);

	gtk_clist_set_column_width (GTK_CLIST (playerlist), 0, 23);
	gtk_clist_set_column_width (GTK_CLIST (playerlist), 1, 21);
	gtk_clist_set_column_width (GTK_CLIST (playerlist), 2, 80);
	gtk_clist_column_titles_show (GTK_CLIST (playerlist));
	
	cidlabel = gtk_label_new ("Icon");
	gtk_widget_ref (cidlabel);
	gtk_widget_show (cidlabel);
	gtk_clist_set_column_widget (GTK_CLIST (playerlist), 0, cidlabel);
	
	pidlabel = gtk_label_new ("PID");
	gtk_widget_ref (pidlabel);
	gtk_widget_show (pidlabel);
	gtk_clist_set_column_widget (GTK_CLIST (playerlist), 1, pidlabel);
	
	namelabel = gtk_label_new ("Name");
	gtk_widget_ref (namelabel);
	gtk_widget_show (namelabel);
	gtk_clist_set_column_widget (GTK_CLIST (playerlist), 2, namelabel);

	gtk_signal_connect (GTK_OBJECT (playerlist), "button_press_event",
			    GTK_SIGNAL_FUNC (show_playerlist_popup),
			    NULL);

	gtk_container_add (GTK_CONTAINER (viewport), playerlist);
	GTK_WIDGET_UNSET_FLAGS (playerlist, GTK_CAN_FOCUS);
	GTK_WIDGET_UNSET_FLAGS (playerlist, GTK_CAN_DEFAULT);

	gtk_clist_column_titles_hide(GTK_CLIST(playerlist));

	return frame;
}

GtkWidget *new_textframe(GtkWidget *mainwin)
{
	GtkWidget *frame, *scrolledwindow, *textbox;

	frame = gtk_frame_new (_("Server Messages"));
	gtk_widget_show (frame);

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_container_add (GTK_CONTAINER (frame), scrolledwindow);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), 
					GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

	textbox = gtk_text_new (NULL, NULL);
	gtk_text_set_word_wrap(GTK_TEXT(textbox), TRUE);
	gtk_object_set_data (GTK_OBJECT (mainwin), "textbox", textbox);
	gtk_widget_show (textbox);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), textbox);
	gtk_widget_set_usize (textbox, 200, 80);

	return frame;
}

GtkWidget *new_chatframe(GtkWidget *mainwin) 
{
	GtkWidget *frame, *scrolledwindow;
	GtkWidget *chatbox;

	frame = gtk_frame_new (_("Chat"));
	gtk_widget_show (frame);

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_container_add (GTK_CONTAINER (frame), scrolledwindow);
	gtk_widget_set_usize (scrolledwindow, 200, -2);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), 
					GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	chatbox = gtk_text_new (NULL, NULL);
	gtk_text_set_word_wrap(GTK_TEXT(chatbox), TRUE);
	gtk_object_set_data(GTK_OBJECT (mainwin), "chatbox", chatbox);
	gtk_widget_show (chatbox);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), chatbox);
	gtk_widget_set_usize (chatbox, 200, 100);

	return frame;
}

GtkWidget *new_playerlist_popup(player *p) 
{
	extern GtkWidget *mainwin;
	extern gmonopprefs *prefs;
	GtkWidget *popup;
	GtkWidget *playerlist;
	GtkWidget *tradeitem;

	GtkAccelGroup *popup_accels;
	char str[1024];
	
	playerlist = gtk_object_get_data(GTK_OBJECT(mainwin), "playerlist");
	if(p->id != prefs->me) {
		popup = gtk_menu_new ();
		popup_accels = gtk_menu_ensure_uline_accel_group(GTK_MENU(popup));

		sprintf(str, "Trade with %d. %s...", p->id, p->name);
		tradeitem = gtk_menu_item_new_with_label(_(str));
		gtk_widget_show(tradeitem);
		gtk_container_add(GTK_CONTAINER(popup), tradeitem);
	
		gtk_signal_connect (GTK_OBJECT(tradeitem), "activate",
				    GTK_SIGNAL_FUNC(on_trade_activated), p);
		return popup;
	}
	return NULL;
}

void on_trade_activated (GtkWidget *widget, player *p) 
{
	char *buf = (char *)malloc(6);
	sprintf(buf, "%d", p->id);
	client_outmessage(OUT_TRADENEW, buf);
	free(buf);
}

gboolean show_playerlist_popup (GtkWidget *widget, GdkEventButton *event, gpointer data) 
{
	GtkWidget *popup;
	player *p;
	int row, col;

	if(event->button == 3) { /* right click */
		if (gtk_clist_get_selection_info
		    (GTK_CLIST (widget), event->x, event->y, &row, &col) < 0)
			return FALSE;
		gtk_clist_unselect_all (GTK_CLIST (widget));
		if(gtk_clist_get_row_data(GTK_CLIST(widget), row)) {
			if ((p = player_of_id(atoi(gtk_clist_get_row_data(GTK_CLIST(widget), row))))) {
				popup = new_playerlist_popup(p);
				if(popup) {
					gtk_widget_show(popup);
					gtk_menu_popup(GTK_MENU(popup), NULL, NULL, NULL, NULL, event->button, event->time);
				}
			}
		}
	}
	return TRUE;
}

void on_chatentry_activated (GtkEditable *editable, gpointer user_data)
{
	char *text;
	text = gtk_entry_get_text(GTK_ENTRY(editable));
	
	if(strlen(text) == 0) return;
	client_outmessage(OUT_CHAT, text);
	gtk_entry_set_text (GTK_ENTRY(editable), "");
}


void show_prefswin(GtkMenuItem *menuitem, GtkWidget *mainwin) 
{
	GtkWidget *prefswin;
	
	if(gtk_object_get_data(GTK_OBJECT(mainwin), "prefswin") == NULL) {
		prefswin = new_prefswin();
		gtk_widget_show(prefswin);
	}
}


void show_aboutwin(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *aboutwin;
	aboutwin = new_aboutwin();
	gtk_widget_show(aboutwin);
}

void on_roll_clicked (GtkButton *button, gpointer user_data)
{
	client_outmessage(OUT_ROLL, NULL);
}
void on_buy_clicked (GtkButton *button, gpointer user_data)
{
	client_outmessage(OUT_BUY, NULL);
}
void on_auction_clicked (GtkButton *button, gpointer user_data)
{
	client_outmessage(OUT_AUCTION, NULL);
}
void on_endturn_clicked (GtkButton *button, gpointer user_data)
{
	client_outmessage(OUT_ENDTURN, NULL);
}
void on_payjail_clicked (GtkButton *button, gpointer user_data)
{
	client_outmessage(OUT_PAYJAIL, NULL);
}
void on_jailroll_clicked (GtkButton *button, gpointer user_data)
{
	client_outmessage(OUT_JAILROLL, NULL);
}
void on_jailcard_clicked (GtkButton *button, gpointer user_data)
{
	client_outmessage(OUT_JAILCARD, NULL);
}
void on_paydebt_clicked (GtkButton *button, gpointer user_data) 
{
	client_outmessage(OUT_PAYDEBT, NULL);
}
void on_declarebankrupt_clicked (GtkButton *button, gpointer user_data) 
{
	client_outmessage(OUT_DECLAREBANKRUPT, NULL);
}

void win_set_icon (GtkWidget *win)
{
	GdkPixmap *pix;
	GdkBitmap *mask;

	GdkAtom icon_atom;
	glong data[2];

	gtk_widget_realize (win);

	pix = gdk_pixmap_create_from_xpm_d(win->window, &mask, NULL, 
					   smallboard_xpm);
	data[0] = GDK_WINDOW_XWINDOW (pix);
	data[1] = GDK_WINDOW_XWINDOW (mask);
	icon_atom = gdk_atom_intern ("KWM_WIN_ICON", FALSE);
	gdk_property_change (win->window, icon_atom, icon_atom,
			     32, GDK_PROP_MODE_REPLACE, (guchar *) data, 2);

	gdk_window_set_icon (win->window, NULL, pix, mask);
	//	gdk_window_set_icon_name (win->window, PACKAGE);
}
