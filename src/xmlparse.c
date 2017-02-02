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

/* xmlparse.c : functions for manipulating our gamestate via interpreting the
   xml */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "misc_functions.h"
#include "gdk_functions.h"
#include "structs.h"
#include "assetbox.h"
#include "tradebox.h"
#include "auctionbox.h"
#include "cardview.h"
#include "stub.h"
#include "client.h"
#include "xmlparse.h"
#include "game_functions.h"

void parse_display (xmlDocPtr doc, xmlNodePtr cur) 
{
	GtkWidget *cardwin = NULL;
	char *buf;

	if((buf = xmlGetProp(cur, (xmlChar *) "type"))) {
		cardwin = new_cardview(cur);
		gtk_widget_show(cardwin);
	}
}

void parse_commandlist (xmlDocPtr doc, xmlNodePtr cur)
{
	extern GtkWidget *mainwin;
	GtkWidget *buttonbox, *button;
	extern gmonopprefs *prefs;
	char *buf;
	xmlNodePtr step;
	int buttons = 0;
	
	buttonbox = gtk_object_get_data(GTK_OBJECT(mainwin), "buttonbox");
	prefs->buttons = 0;
	
	for(step = cur->children; step; step = step->next) {
		if(!strcmp(step->name, "command"))
			if((buf = xmlGetProp(step, (xmlChar *) "id"))) {
				if(!strcmp(buf, "roll"))
					prefs->buttons |= BUTTON_ROLL;
				if(!strcmp(buf, "buy"))
					prefs->buttons |= BUTTON_BUYESTATE;
				if(!strcmp(buf, "endturn"))
					prefs->buttons |= BUTTON_ENDTURN;
				if(!strcmp(buf, "auction"))
					prefs->buttons |= BUTTON_AUCTION;
				if(!strcmp(buf, "usecard"))
					prefs->buttons |= BUTTON_JAILCARD;
				if(!strcmp(buf, "payjail"))
					prefs->buttons |= BUTTON_PAYJAIL;
				if(!strcmp(buf, "jailroll"))
					prefs->buttons |= BUTTON_JAILROLL;
				if(!strcmp(buf, "paydebt"))
					prefs->buttons |= BUTTON_PAYDEBT;
				if(!strcmp(buf, "declarebankrupt"))
					prefs->buttons |= BUTTON_BANKRUPT;
			}
	}
	
	button = gtk_object_get_data(GTK_OBJECT(buttonbox), "label");
	if(button && button->parent == GTK_WIDGET(buttonbox)) {
		gtk_container_remove(GTK_CONTAINER(buttonbox), button);
	}
	
	button = gtk_object_get_data(GTK_OBJECT(buttonbox), "roll");
	if(button && button->parent == GTK_WIDGET(buttonbox))
		gtk_container_remove(GTK_CONTAINER(buttonbox), button);

	button = gtk_object_get_data(GTK_OBJECT(buttonbox), "buy");
	if(button && button->parent == GTK_WIDGET(buttonbox))
		gtk_container_remove(GTK_CONTAINER(buttonbox), button);

	button = gtk_object_get_data(GTK_OBJECT(buttonbox), "auction");
	if(button && button->parent == GTK_WIDGET(buttonbox))
		gtk_container_remove(GTK_CONTAINER(buttonbox), button);

	button = gtk_object_get_data(GTK_OBJECT(buttonbox), "endturn");
	if(button && button->parent == GTK_WIDGET(buttonbox))
		gtk_container_remove(GTK_CONTAINER(buttonbox), button);

	button = gtk_object_get_data(GTK_OBJECT(buttonbox), "payjail");
	if(button && button->parent == GTK_WIDGET(buttonbox))
		gtk_container_remove(GTK_CONTAINER(buttonbox), button);

	button = gtk_object_get_data(GTK_OBJECT(buttonbox), "jailroll");
	if(button && button->parent == GTK_WIDGET(buttonbox))
		gtk_container_remove(GTK_CONTAINER(buttonbox), button);

	button = gtk_object_get_data(GTK_OBJECT(buttonbox), "jailcard");
	if(button && button->parent == GTK_WIDGET(buttonbox))
		gtk_container_remove(GTK_CONTAINER(buttonbox), button);

	button = gtk_object_get_data(GTK_OBJECT(buttonbox), "paydebt");
	if(button && button->parent == GTK_WIDGET(buttonbox))
		gtk_container_remove(GTK_CONTAINER(buttonbox), button);

	button = gtk_object_get_data(GTK_OBJECT(buttonbox), "declarebankrupt");
	if(button && button->parent == GTK_WIDGET(buttonbox))
		gtk_container_remove(GTK_CONTAINER(buttonbox), button);

	if(prefs->buttons & BUTTON_ROLL) {
		buttons++;
		button = gtk_object_get_data(GTK_OBJECT(buttonbox), "roll");
		gtk_container_add(GTK_CONTAINER(buttonbox), button);
	}

	if(prefs->buttons & BUTTON_BUYESTATE)
	{
		buttons++;
		button = gtk_object_get_data(GTK_OBJECT(buttonbox), "buy");
		gtk_container_add(GTK_CONTAINER(buttonbox), button);
	}

	if(prefs->buttons & BUTTON_AUCTION)
	{
		buttons++;
		button = gtk_object_get_data(GTK_OBJECT(buttonbox), "auction");
		gtk_container_add(GTK_CONTAINER(buttonbox), button);
	}

	if(prefs->buttons & BUTTON_ENDTURN)
	{
		buttons++;
		button = gtk_object_get_data(GTK_OBJECT(buttonbox), "endturn");
		gtk_container_add(GTK_CONTAINER(buttonbox), button);
	}

	if(prefs->buttons & BUTTON_PAYJAIL)
	{
		buttons++;
		button = gtk_object_get_data(GTK_OBJECT(buttonbox), "payjail");
		gtk_container_add(GTK_CONTAINER(buttonbox), button);
	}

	if(prefs->buttons & BUTTON_JAILROLL)
	{
		buttons++;
		button = gtk_object_get_data(GTK_OBJECT(buttonbox), "jailroll");
		gtk_container_add(GTK_CONTAINER(buttonbox), button);
	}

	if(prefs->buttons & BUTTON_JAILCARD)
	{
		buttons++;
		button = gtk_object_get_data(GTK_OBJECT(buttonbox), "jailcard");
		gtk_container_add(GTK_CONTAINER(buttonbox), button);
	}

	if(prefs->buttons & BUTTON_PAYDEBT)
	{
		buttons++;
		button = gtk_object_get_data(GTK_OBJECT(buttonbox), "paydebt");
		gtk_container_add(GTK_CONTAINER(buttonbox), button);
	}

	if(prefs->buttons & BUTTON_BANKRUPT)
	{
		buttons++;
		button = gtk_object_get_data(GTK_OBJECT(buttonbox), "declarebankrupt");
		gtk_container_add(GTK_CONTAINER(buttonbox), button);
	}
	
	if(buttons == 0) {
		button = gtk_label_new(_("No commands available"));
		gtk_widget_show(button);
		gtk_object_set_data(GTK_OBJECT(buttonbox), "label", button);
		gtk_container_add(GTK_CONTAINER(buttonbox), button);
	}
	
}

void parse_auctionupdate (xmlDocPtr doc, xmlNodePtr cur) 
{
	extern GtkWidget *mainwin;
	extern auction *auctions;
	extern GtkStyle *active_text_style;
	extern estate *estates[];
	char *buf, *buf2;
	auction *a, *astep;
	GtkWidget *notebook;
	GtkWidget *temp;
	char str[1024];

	notebook = gtk_object_get_data(GTK_OBJECT(mainwin), "notebook");
	if((buf = xmlGetProp(cur, (xmlChar *) "type"))) {
		if(!strcmp(buf, "new")) {
			/* alloc the auction */
			if(auctions) {
				a = (auction *) new_malloc(sizeof(auction));
				a->next = auctions->next;
				auctions->next = a;
			} else {
				a = auctions = (auction *) new_malloc(sizeof(auction));
			}
			
			a->next = (auction *)NULL;
			a->highbid = 0;
			a->highbidder = (player *)NULL;
			a->widget = NULL;
			a->count = 0;
			a->id = atoi(xmlGetProp(cur, (xmlChar *) "auctionid"));
			
			if(xmlGetProp(cur, (xmlChar *) "estateid"))
				sprintf(str, "Auction for %s", estates[atoi(xmlGetProp(cur, (xmlChar *) "estateid"))]->name);
			else
				sprintf(str, "Auction");
			
			a->tablabel = gtk_label_new(str);
			gtk_widget_show(a->tablabel);
			a->widget = new_auctionbox(a);
			gtk_widget_show(a->widget);
			
			gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
						 a->widget,
						 a->tablabel);
		} else if(!strcmp(buf, "edit")) {
			a = auction_of_id(atoi(xmlGetProp(cur, (xmlChar *) "auctionid")));
			if((buf2 = xmlGetProp(cur, (xmlChar *) "estateid"))) {
				a->estate = estates[atoi(buf2)];
				sprintf(str, "Auction for %s", a->estate->name);
				gtk_label_set_text(GTK_LABEL(a->tablabel), str);
			}
			if((buf2 = xmlGetProp(cur, (xmlChar *) "highbid"))) {
				update_auctionbox(a, 
						  player_of_id(atoi(xmlGetProp(cur, (xmlChar *) "highbidder"))),
						  atoi(buf2));
			}
			if((buf2 = xmlGetProp(cur, (xmlChar *) "message"))) {

				temp = gtk_object_get_data(GTK_OBJECT(a->widget), "label");
				if((xmlGetProp(cur, (xmlChar *) "status")) && atoi(xmlGetProp(cur, (xmlChar *) "status"))) {
					gtk_label_get(GTK_LABEL(temp), &buf);
					strcpy(str, buf);
					strcat(str, " ");
					strcat(str, buf2);
				} else
					strcpy(str, buf2);
			
				notebook = gtk_object_get_data(GTK_OBJECT(mainwin), "notebook");
				if(a->widget != (gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 
									   gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)))))
					gtk_widget_set_style (a->tablabel, active_text_style);
				gtk_label_set_text(GTK_LABEL(temp), str);
			}
			
		} else if(!strcmp(buf, "completed")) {
			a = auction_of_id(atoi(xmlGetProp(cur, (xmlChar *) "auctionid")));
			kill_auctionbox(a);

			a = NULL;
			astep = auctions;
			while(astep->id != atoi(xmlGetProp(cur, (xmlChar *) "auctionid"))) {
				a = astep;
				astep = astep->next;
			}
			/* we will kill astep, a is the one before astep */

			if(a) a->next = astep->next; /* skip over astep */
			else auctions = astep->next; /* killing the first one */

			free(astep);
		}
	}
}

void parse_tradeupdate (xmlDocPtr doc, xmlNodePtr cur) 
{
	char *buf, *buf2;
	extern gmonopprefs *prefs;
	extern GtkWidget *mainwin;
	trade *t, *tstep;
	extern GtkStyle *active_text_style;
	extern trade *trades;
	int i;
	char str[1024];
	xmlNodePtr step;
	GtkWidget *notebook;

	notebook = gtk_object_get_data(GTK_OBJECT(mainwin), "notebook");
	
	if((buf = xmlGetProp(cur, (xmlChar *) "type")))	{
		if(!strcmp(buf, "new")) {
			/* alloc the trade */
			if(trades) {
				t = (trade *) new_malloc(sizeof(trade));
				t->next = trades->next;
				trades->next = t;
			} else {
				t = trades = (trade *) new_malloc(sizeof(trade));
			}
			
			t->next = (trade *)NULL;
			t->name = (char *)NULL;
			t->id = atoi(xmlGetProp(cur, (xmlChar *) "tradeid"));
			for(i = 0; i < 40; i++)
				t->included[i] = 0;
			
			for(step = cur->children; step; step = step->next) { /* for each player */
				if(!strcmp(step->name, "tradeplayer")) {
					if(atoi(xmlGetProp(step, (xmlChar *) "playerid")) == prefs->me)
						/* add me to trade */
						t->p[0] = player_of_id(prefs->me);
					else
						/* add player to trade */
						t->p[1] = player_of_id(atoi(xmlGetProp(step, (xmlChar *) "playerid")));
				}
			}
			
			/* add the trade to the notebook */
			sprintf(str, "Trade with %d. %s", t->p[1]->id, t->p[1]->name);
			t->tablabel = gtk_label_new(str);
			t->widget = new_tradebox(t);
			gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
						 t->widget,
						 t->tablabel);
		} else if(!strcmp(buf, "edit")) {
			t = trade_of_id(atoi(xmlGetProp(cur, (xmlChar *) "tradeid")));

			/* color the tab label */
			if(t->widget != (gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 
								   gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)))))
				gtk_widget_set_style (t->tablabel, active_text_style);
			
			/* process the children */
			for(step = cur->children; step; step = step->next) {
				if(!strcmp(step->name, "estate")) {
					trades->included[atoi(xmlGetProp(cur, (xmlChar *) "tradeid"))] =
						atoi(xmlGetProp(step, (xmlChar *) "included"));
					update_tradebox(t,
							atoi(xmlGetProp(step, (xmlChar *) "estateid")), 
							atoi(xmlGetProp(cur, (xmlChar *) "actor")), 
							atoi(xmlGetProp(step, (xmlChar *) "included")));
				} else if(!strcmp(step->name, "tradeplayer")) {
					if((buf2 = xmlGetProp(step, (xmlChar *) "accept"))) {
						i = ((atoi(xmlGetProp(step, (xmlChar *) "playerid"))) == prefs->me) ? 0 : 1;
						if(atoi(buf2))
							sprintf(str, "%d. %s (Accepted)", t->p[i]->id, t->p[i]->name);
						else
							sprintf(str, "%d. %s", t->p[i]->id, t->p[i]->name);
						if(i == 0)
							gtk_toggle_button_set_active(t->acceptbutton, atoi(buf2));
								
						gtk_frame_set_label(GTK_FRAME(t->frame[i]), str);
					}
					
					if((buf2 = xmlGetProp(step, (xmlChar *) "money"))) {
						i = ((atoi(xmlGetProp(step, (xmlChar *) "playerid"))) == prefs->me) ? 0 : 1;
						sprintf(str, "Money: $%d", atoi(buf2));
						gtk_ctree_node_set_text(GTK_CTREE(t->moneytree[i]), 
									gtk_object_get_data(GTK_OBJECT(t->moneytree[i]), "node"),
									0, str);
					}
				}
			}
		} else if(!strcmp(buf, "rejected") || !strcmp(buf, "completed")) {
			kill_tradebox(trade_of_id(atoi(xmlGetProp(cur, (xmlChar *) "tradeid"))));

			/* remove it from the linked list */

			t = NULL;
			tstep = trades;
			while(tstep->id != atoi(xmlGetProp(cur, (xmlChar *) "tradeid"))) {
				t = tstep;
				tstep = tstep->next;
			}
			/* we will kill tstep, t is the one before tstep */

			if(t) t->next = tstep->next; /* skip over tstep */
			else trades = tstep->next; /* killing the first one */

			free(tstep);
			return;
		}
	}
}

void parse_client(xmlDocPtr doc, xmlNodePtr cur) 
{
	/* prefs->me is my current id */
	char *buf;
	extern gmonopprefs *prefs;
		
	if((buf = xmlGetProp(cur, (xmlChar *) "playerid")))
		prefs->me = atoi(buf);
}

void parse_updategamelist(xmlDocPtr doc, xmlNodePtr cur) 
{
	extern gamelist *gameslist;
	game *trailer, *step;
	char *type;

	if(gameslist == (gamelist *)NULL) {
		gameslist = new_malloc(sizeof(gamelist));
		gameslist->games = (game *)NULL;
	}
	
	trailer = step = gameslist->games;

	type = xmlGetProp(cur, (xmlChar *) "type");
	
	if(!strcmp(type, "full")) {
		parse_gamelist(doc, cur);
		return;
	} else if(!strcmp(type, "add")) {
		
		while(step != (game *)NULL) {
			trailer = step;
			step = step->next;
		} /* find the last game in the list */
		
		for(cur = cur->children; cur; cur = cur->next) { /* for each game to add */
			if(!xmlStrcmp(cur->name, (const xmlChar *) "game")) {
				if(trailer == (game *)NULL) /* no games */
					step = gameslist->games = new_malloc(sizeof(game));
				else
					step = trailer->next = new_malloc(sizeof(game));
				step->id = xmlGetProp(cur, (const xmlChar *) "id");
				step->name = new_malloc(1024);
				if(xmlGetProp(cur, (const xmlChar *) "description"))
					strcpy(step->name, xmlGetProp(cur, (const xmlChar *) "description"));
				else
					strcpy(step->name, "");
				step->players = xmlGetProp(cur, (const xmlChar *) "players");
				step->next = NULL;
				trailer = step;
			}
		}
		return;
	} else if(!strcmp(type, "del")) {
		for(cur = cur->children; cur; cur = cur->next) { /* for each game to delete */
			for(step = gameslist->games; step; step = step ? step->next : NULL) { 
				/* for each game in our list */
				if(!xmlStrcmp(cur->name, (const xmlChar *) "game")) { /* if it's a game */
					if(!xmlStrcmp(xmlGetProp(cur, (xmlChar *) "id"), step->id)) { 
						/* and it matches the game in our list */
						if(trailer == step) { /* first one */
							gameslist->games = step->next;
							free(step); /* die! */
							step = gameslist->games;
						} else {
							trailer->next = step->next;
							free(step);
							step = trailer->next;
						} /* take it out of the list */
					}
				}
			}
		}
	} else if (!strcmp(type, "edit")) {
		for(cur = cur->children; cur; cur = cur->next) {
			/* for each game the server sends us */
			for(step = gameslist->games; step; step = step ? step->next : NULL) {
				/* for each game in our list */
				if(!strcmp(cur->name, (const xmlChar *) "game")) {
					if(!strcmp(xmlGetProp(cur, (xmlChar *) "id"), step->id)) {
						if(xmlGetProp(cur, (xmlChar *) "description"))
							step->name = xmlGetProp(cur, (xmlChar *) "description");
						if(xmlGetProp(cur, (xmlChar *) "players")) 
							step->players = xmlGetProp(cur, (xmlChar *) "players");
					}
				}
			}
		}
	}
}

/* that was painful. */	

void parse_gamelist(xmlDocPtr doc, xmlNodePtr cur) 
{
	extern gamelist *gameslist;
	game *trailer, *step;
	
	if(gameslist == (gamelist *)NULL) {
		gameslist = new_malloc(sizeof(gamelist));
		gameslist->games = (game *)NULL;
	}
	
	cur = cur->children;
	gameslist->games = new_malloc(sizeof(game));
	trailer = step = gameslist->games;
		
	if(cur == NULL) {
		free(gameslist->games);
		gameslist->games = (game *)NULL;
		return;
	}
	
	while(cur != NULL) {
		if (!xmlStrcmp(cur->name, (const xmlChar *) "game")) {
			step->id = xmlGetProp(cur, (const xmlChar *) "id");
			//			step->name = new_malloc(1024);
			if(xmlGetProp(cur, (const xmlChar *) "description"))
				step->name = xmlGetProp(cur, (const xmlChar *) "description");
			
			//				strcpy(step->name, xmlGetProp(cur, (const xmlChar *) "description"));
			step->players = xmlGetProp(cur, (const xmlChar *) "players");
		}
		step->next = new_malloc(sizeof(game));
		trailer = step;
		step = step->next;
		step->next = (game *)NULL;
		cur = cur->next;
	}
	
	if(trailer != step)
		free(trailer->next);
	
	trailer->next = (game *)NULL;
	return;
}

void parse_updateplayerlist(xmlDocPtr doc, xmlNodePtr cur) 
{
	extern player *players;
	extern GtkWidget *mainwin;
	extern gmonopprefs *prefs;
	player *step, *trailer;
	GtkWidget *playerlist;
	char *buf1, *buf2, *buf3, *bufx[3];
	images *im = prefs->theme->im;

	int row;
	
	playerlist = (GtkWidget *)gtk_object_get_data(GTK_OBJECT(mainwin), "playerlist");
	gtk_clist_freeze(GTK_CLIST(playerlist));

	buf1 = buf2 = buf3 = malloc(20);
	strcpy(buf1, "");

	if(!strcmp(xmlGetProp(cur, (const xmlChar *) "type"), "add")) {
		cur = cur->children;
		while(cur != NULL) { /* for each player */
			step = new_malloc(sizeof(player));
			
			if(!strcmp(cur->name, (const xmlChar *) "player")) {
				buf3 = step->name = xmlGetProp(cur, (const xmlChar *) "name");
				if(xmlGetProp(cur, (const xmlChar *) "playerid")) 
					buf2 = xmlGetProp(cur, (const xmlChar *) "playerid");
				else {
					buf2 = new_malloc(2);
					strcpy(buf2, "1");
				}
				
				step->id = atoi(buf2);
				step->cash = 0;
				step->position = 0;
				bufx[0] = buf1;
				bufx[1] = buf2;
				bufx[2] = buf3;
				/* add the row and make it unselectable */
				row = gtk_clist_append(GTK_CLIST(playerlist), bufx);
				gtk_clist_set_selectable(GTK_CLIST(playerlist), row, FALSE);
				gtk_clist_set_row_data(GTK_CLIST(playerlist), row, buf2);
				gtk_clist_set_pixmap(GTK_CLIST(playerlist), row, 0,
						     im->tokens[step->id]->pixmapbase, 
						     im->tokens[step->id]->maskbase);
			}
			if(players) {
				trailer = players->next;
				players->next = step;
				step->next = trailer;
			} else {
				players = step;
				trailer = (player *) NULL;
				step->next = players;
			}
			cur = cur->next;
		}
	} else if(!strcmp(xmlGetProp(cur, (const xmlChar *) "type"), "full")) { 
		/* full */
		gtk_clist_clear(GTK_CLIST(playerlist));
		
		cur = cur->children;
		trailer = step = players = malloc(sizeof(player));
		
		while(cur != NULL) {
			if(!strcmp(cur->name, (const xmlChar *) "player")) {
				buf3 = step->name = xmlGetProp(cur, (const xmlChar *) "name");
				if(xmlGetProp(cur, (const xmlChar *) "playerid"))
					buf2 = xmlGetProp(cur, (const xmlChar *) "playerid");
				else {
					buf2 = new_malloc(2);
					strcpy(buf2, "0");
				}
				
				step->id = atoi(buf2);
				
				step->cash = 0;
				step->position = 0;
				bufx[0] = buf1;
				bufx[1] = buf2;
				bufx[2] = buf3;
				
				row = gtk_clist_append(GTK_CLIST(playerlist), bufx);
				gtk_clist_set_selectable(GTK_CLIST(playerlist), row, FALSE);
				gtk_clist_set_row_data(GTK_CLIST(playerlist), row, buf2);
				gtk_clist_set_pixmap(GTK_CLIST(playerlist), row, 0, im->tokens[step->id]->pixmap, im->tokens[step->id]->mask);
			}
			step->next = new_malloc(sizeof(player));
			trailer = step;
			step = step->next;
			step->next = (player *)NULL;
			cur = cur->next;
		}
		
		free(trailer->next); // aka step
		trailer->next = players; // circular linked list
	}
	gtk_clist_thaw(GTK_CLIST(playerlist));
		

}

void parse_msg(xmlDocPtr doc, xmlNodePtr cur) 
{
	extern int gamestarted;
	
	if(!xmlStrcmp(xmlGetProp(cur, (const xmlChar *) "type"), "chat")) {
		chatprintf("<%s> %s\n", 
			   xmlGetProp(cur, (const xmlChar *) "author"),
			   xmlGetProp(cur, (const xmlChar *) "value"));
	} else if(!xmlStrcmp(xmlGetProp(cur, (const xmlChar *) "type"), "startgame")) {
		gamestarted = 1;
	} else {
		tprintf("%s\n", xmlGetProp(cur, (const xmlChar *) "value"));
	}
}

void parse_playerupdate(xmlDocPtr doc, xmlNodePtr cur) 
{
	extern GtkWidget *mainwin;
	extern GtkStyle *active_text_style;
	extern player *players;
	extern int gamestarted;
	GtkWidget *notebook, *assettab, *assettablabel;
	player *player;
	char *str;

	int pid = atoi(xmlGetProp(cur, (const xmlChar *) "playerid"));
	
	if(pid == -1)
		return;
	
	player = player_of_id(pid);
	notebook = gtk_object_get_data(GTK_OBJECT(mainwin), "notebook");
	assettab = gtk_object_get_data(GTK_OBJECT(mainwin), "assettab");
	assettablabel = gtk_object_get_data(GTK_OBJECT(mainwin), "assettablabel");

	/* color the asset tab label if money has changed */
	if(xmlGetProp(cur, (xmlChar *) "money"))
		if(assettab != (gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 
							  gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)))))
			gtk_widget_set_style (assettablabel, active_text_style);

	if(player) { /* not the first */
		if((str = xmlGetProp(cur, (const xmlChar *) "name")))
			player->name = str;
		if((str = xmlGetProp(cur, (const xmlChar *) "money"))) {
			player->cash = atoi(str);
			assetbox_moneychanged(players);
		}
		
		if((str = xmlGetProp(cur, (const xmlChar *) "location"))) {
			/*
			// this is all for token movement stuff
			if(!xmlStrcmp(xmlGetProp(cur, (const xmlChar *) "directmove"), "0")) {
			//if it's not a direct move
			for(i = atoi(str); player->position != i; player->position++) {
			if(player->position >= 40) 
			//have to take an extra because of the increment 
			player->position -= 41;
			update_board();
			}
			} else {
			*/
			player->position = atoi(str);
			//}
			client_outmessage(OUT_CONFIRM, str);
		}
	} else { /* first player we're dealing with */
		players = malloc(sizeof(player));
		players->position = 0;
		players->cash = 0;

		players->id = atoi(xmlGetProp(cur, (const xmlChar *) "playerid"));
		if((str = xmlGetProp(cur, (const xmlChar *) "name")))
			players->name = str;
		if((str = xmlGetProp(cur, (const xmlChar *) "money"))) {
			players->cash = atoi(str);
			assetbox_moneychanged(players);
		}
		if((str = xmlGetProp(cur, (const xmlChar *) "location")))
			players->position = atoi(str);
		players->next = players;
	}
	
	if(gamestarted) {
		if(xmlGetProp(cur, (const xmlChar *) "location"))
			update_board();
		if(xmlGetProp(cur, (const xmlChar *) "money"))
			update_statusbar();
	}
}

void parse_newturn(xmlDocPtr doc, xmlNodePtr cur) 
{
	/* highlight the player whose turn it is and make everything else
	   unselectable */

	extern GtkWidget *mainwin;
	int i, j, pid;
	char *buf;
	GdkColor *color;
	GtkStyle *rcstyle;
	GtkWidget *playerlist = gtk_object_get_data(GTK_OBJECT(mainwin), "playerlist");
	color = malloc(sizeof(GdkColor));

	rcstyle = gtk_rc_get_style (playerlist);
	if (rcstyle == NULL)
		rcstyle = gtk_style_new ();
	else
		gtk_style_ref (rcstyle);
	
	pid = atoi(xmlGetProp(cur, (const xmlChar *) "player"));
	j = 1921; /* not a player id */
	
	for(i = 0; j != pid; i++) {
		if(!gtk_clist_get_text(GTK_CLIST(playerlist), i, 1, &buf))
			return;
		j = atoi(buf);
	}
	for(j = 0; gtk_clist_get_text(GTK_CLIST(playerlist), j, 1, &buf); j++);
	
	if(i == 1)
		gtk_clist_set_background(GTK_CLIST(playerlist), j-1, &(rcstyle->bg[4]));
	else
		gtk_clist_set_background(GTK_CLIST(playerlist), i-2, &(rcstyle->bg[4]));

	color->red = 20000;
	color->green = 50000;
	color->blue = 20000;

	gtk_clist_set_background(GTK_CLIST(playerlist), i-1, color);
}

void parse_estateupdate(xmlDocPtr doc, xmlNodePtr cur) 
{
	/* fill in the estate information */
	extern estate *estates[];
	extern int gamestarted;
	char *buf1, *buf2;

	if ((buf1 = xmlGetProp(cur, (const xmlChar *) "estateid"))) {
		estates[atoi(buf1)]->id = atoi(buf1);

		/* FIXME: should I be free()ing all of these char *'s? */ 
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "owner")) &&
		   (atoi(buf2) >= 0)) {
			update_assetbox(estates[atoi(buf1)], 
					estates[atoi(buf1)]->owner, 
					player_of_id(atoi(buf2)));
			estates[atoi(buf1)]->owner = player_of_id(atoi(buf2));
		}
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "houses")))
			estates[atoi(buf1)]->houses = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "mortgaged")))
			estates[atoi(buf1)]->mortgaged = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "name"))) {
			estates[atoi(buf1)]->name = malloc(strlen(buf2)+1);
			strcpy(estates[atoi(buf1)]->name, buf2);
		}
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "groupid")))
			estates[atoi(buf1)]->group = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "can_be_mortgaged")))
			estates[atoi(buf1)]->mortgageable = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "can_be_unmortgaged")))
			estates[atoi(buf1)]->unmortgageable = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "can_buy_houses")))
			estates[atoi(buf1)]->canbuyhouses = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "can_sell_houses")))
			estates[atoi(buf1)]->cansellhouses = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "can_be_owned")))
			estates[atoi(buf1)]->ownable = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "houseprice")))
			estates[atoi(buf1)]->houseprice = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "sellhouseprice")))
			estates[atoi(buf1)]->sellhouseprice = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "mortgageprice")))
			estates[atoi(buf1)]->mortgageprice = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "unmortgageprice")))
			estates[atoi(buf1)]->unmortgageprice = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "price")))
			estates[atoi(buf1)]->price = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "rent0")))
			estates[atoi(buf1)]->rent[0] = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "rent1")))
			estates[atoi(buf1)]->rent[1] = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "rent2")))
			estates[atoi(buf1)]->rent[2] = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "rent3")))
			estates[atoi(buf1)]->rent[3] = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "rent4")))
			estates[atoi(buf1)]->rent[4] = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "rent5")))
			estates[atoi(buf1)]->rent[5] = atoi(buf2);
		if((buf2 = xmlGetProp(cur, (const xmlChar *) "type")))
			strcpy(estates[atoi(buf1)]->type, buf2);
		if(gamestarted)
			update_board();
	}
}

void parse_gameupdate(xmlDocPtr doc, xmlNodePtr cur) 
{
	extern gmonopprefs *prefs;
	extern int gamestarted;
	extern GtkWidget *mainwin;
	GtkWidget *playerlist, *assetbox;
	char *buf, *buf2;
	int pid, i, j = 0;
	GdkColor *color = new_malloc(sizeof(GdkColor));
	playerlist = gtk_object_get_data(GTK_OBJECT(mainwin), "playerlist");
	
	j = 31337; /* not a playerid */
	
	if((buf2 = xmlGetProp(cur, (const xmlChar *) "status"))) {
		if(!strcmp(buf2, "init"))
			/* do stuff */
			return;
		if(!strcmp(buf2, "started")) {
			pid = prefs->me;
			gamestarted = 1;
			assetbox = gtk_object_get_data(GTK_OBJECT(mainwin),
						       "assetbox");
			fill_assetbox(assetbox);

			color->red = 60000;
			color->green = 2000;
			color->blue = 2000;
			
			for(i = 0; j != pid; i++) { 
				if(!gtk_clist_get_text(GTK_CLIST(playerlist),
						       i, 1, &buf))
					return;
				j = atoi(buf);
			}

			gtk_clist_set_foreground(GTK_CLIST(playerlist), i-1, 
						 color);
			init_board();
			return;
		}
		if(!strcmp(buf2, "finished")) {
			initialize_client();
			gtk_clist_clear(GTK_CLIST(playerlist));
		}
	}
}

		
