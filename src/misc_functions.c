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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <gtk/gtk.h>

#include "structs.h"
#include "stub.h"
#include "misc_functions.h"

player *findcurrent() 
{
	extern gmonopprefs *prefs;
	extern player *players;
	player *step;

	for(step = players; step->id != prefs->currentturn; step = step->next) {}
	return step;
}

player *player_of_id(int id) 
{
	extern player *players;
	player *step;
	
	if((step = players)) {
		do {
			if(step->id == id) return step;
			step = step->next;
		} while (step != players);
	}
	
	return NULL;
}

auction *auction_of_id(int id) 
{
	extern auction *auctions;
	auction *step;
	for(step = auctions; step; step = step->next)
		if(step->id == id) return step;
	return NULL;
}

trade *trade_of_id(int id) 
{
	extern trade *trades;
	trade *step;
	for(step = trades; step; step = step->next)
		if(step->id == id) return step;
	return NULL;
}

void *new_malloc(size_t sz)
{
	void *ret = malloc(sz);
	EENTER;
	
	if(ret == NULL) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	return ret;
}

void clear_textbox(void) 
{
	extern GtkWidget *mainwin;
	GtkWidget *textbox;
	EENTER;
	
	textbox = gtk_object_get_data(GTK_OBJECT(mainwin), "textbox");
	gtk_text_backward_delete(GTK_TEXT(textbox), 
				 gtk_text_get_length(GTK_TEXT(textbox)));
}

int tprintf(const char *format, ...) 
{
	extern GtkWidget *mainwin;
	GtkWidget *textbox;

	va_list arg;
	int done;
	char buf[1000];

	EENTER;

	textbox = gtk_object_get_data(GTK_OBJECT(mainwin), "textbox");
	
	va_start(arg, format);
	done = vsprintf(buf, format, arg);
	va_end(arg);
	gtk_text_insert(GTK_TEXT(textbox), NULL, NULL, NULL, buf, -1);

	return done;
}

int chatprintf(const char *format, ...) 
{
	extern GtkWidget *mainwin;
	GtkWidget *chatbox;
	
	va_list arg;
	int done;
	char buf[1024];
	
	EENTER;
	
	chatbox = gtk_object_get_data(GTK_OBJECT(mainwin), "chatbox");
	
	va_start(arg, format);
	done = vsprintf(buf, format, arg);
	va_end(arg);
	gtk_text_insert(GTK_TEXT(chatbox), NULL, NULL, NULL, buf, -1);
	
	return done;
}

int statprintf(const char *format, ...) 
{
	extern GtkWidget *mainwin;
	GtkWidget *statusbar;
	va_list arg;
	int done;
	char buf[1000];
	
	EENTER;

	statusbar = gtk_object_get_data(GTK_OBJECT(mainwin), "statusbar");
	va_start(arg, format);
	done = vsprintf(buf, format, arg);
	va_end(arg);

	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), 1);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1, buf);

	return done;
}

