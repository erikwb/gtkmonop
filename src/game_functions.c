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

/* game_functions.c:  this file deals with actions to take on incoming network
   messages. parse_*() are in xml_parse.c. */

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <ctype.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

#include "structs.h"
#include "stub.h"
#include "client.h"
#include "misc_functions.h"
#include "game_functions.h"
#include "gdk_functions.h"
#include "notebook.h"
#include "xmlparse.h"

extern player *players;
extern gamelist *gameslist;

void monopd_inmessage (enum inmsg_type msgtype, 
		       xmlDocPtr doc, 
		       xmlNodePtr node) 
{
	extern GtkWidget *mainwin;
	extern int connected;
	GtkWidget *temp;
	
	char *str;
	
	switch(msgtype) {
	case IN_DISPLAY: /* card */
		parse_display(doc, node);
		break;
		
	case IN_CLIENT: /* info about us */
		parse_client(doc, node);
		break;
		
	case IN_SERVER: /* connected to server */
		/* send nick */
		temp = gtk_object_get_data(GTK_OBJECT(mainwin),
					   "connectstatuslabel");
		gtk_label_set(GTK_LABEL(temp), "Connected to server.");
		
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "nickentry");
		str = gtk_editable_get_chars(GTK_EDITABLE(temp), 0, -1);
		client_outmessage(OUT_NICK, str);
		g_free(str);
		connected = 1;
		connectdialog_destroy();
		
		temp = gtk_object_get_data(GTK_OBJECT(mainwin),
					   "connectbutton");
		gtk_label_set_text(GTK_LABEL(GTK_BUTTON(temp)->child),
				   "Disconnect");
		
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "hostentry");
		gtk_widget_set_sensitive(temp->parent, FALSE);
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "portentry");
		gtk_widget_set_sensitive(temp->parent, FALSE);

		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "startgame");
		gtk_widget_set_sensitive(temp, TRUE);
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "joingame");
		gtk_widget_set_sensitive(temp, TRUE);
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "newgame");
		gtk_widget_set_sensitive(temp, TRUE);
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "setdesc");
		gtk_widget_set_sensitive(temp, TRUE);
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "descentry");
		gtk_widget_set_sensitive(temp, TRUE);
		temp = gtk_object_get_data(GTK_OBJECT(mainwin), "gamelist");
		gtk_widget_set_sensitive(temp, TRUE);
		break;
		
	case IN_UPDATEGAMELIST:
		parse_updategamelist(doc, node);
		update_gamelist();
		break;
		
	case IN_UPDATEPLAYERLIST:
		parse_updateplayerlist(doc, node);
		update_board();
		break;

	case IN_PLAYERUPDATE:
		parse_playerupdate(doc, node);
		break;

	case IN_MSG: /* text server-message */
		parse_msg(doc, node);
		break;
		
	case IN_NEWTURN:
		parse_newturn(doc, node);
		break;
		
	case IN_ESTATEUPDATE:
		parse_estateupdate(doc, node);
		break;

	case IN_TRADEUPDATE:
		parse_tradeupdate(doc, node);
		break;
		
	case IN_AUCTIONUPDATE:
		parse_auctionupdate(doc, node);
		break;
		
	case IN_COMMANDLIST:
		parse_commandlist(doc, node);
		break;
		
	case IN_GAMEUPDATE: /* game status */
		parse_gameupdate(doc, node);
		break;

	default:
		
	}
}

void initialize_client()
{
	extern player *players;
	extern trade *trades;
	extern auction *auctions;
	extern estate *estates[];
	extern int gamestarted, connected;
	extern gamelist *gameslist;
	int i, j;

	gamestarted = connected = 0;
	gameslist = (gamelist *)NULL;
	players = (player *)NULL;
	trades = (trade *)NULL;
	auctions = (auction *)NULL;

	/* default out all the estates, oo pretty rainbow */
	for(i = 0, j = 0; i < 40; i++, j++) {
		estates[i] = malloc(sizeof(estate));
		estates[i]->owner = (player *)NULL;
		estates[i]->mortgaged = 0;
		estates[i]->houses = 0;
		estates[i]->name = NULL;
		estates[i]->type = malloc(20);
		strcpy(estates[i]->type, "other");
		estates[i]->ownable = 0;
		estates[i]->group = j;
		estates[i]->price = 0;
		estates[i]->houseprice = 0;
		estates[i]->dummy = 0;
		estates[i]->rent[0] = 0;
		estates[i]->rent[1] = 0;
		estates[i]->rent[2] = 0;
		estates[i]->rent[3] = 0;
		estates[i]->rent[4] = 0;
		estates[i]->rent[5] = 0;
		estates[i]->estateview = NULL;
		estates[i]->estateview_open = FALSE;

		if(j > 7) j = 0;
	}
}
