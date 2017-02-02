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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <malloc.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <string.h>
#include <signal.h>
#include <libgmonopd/libgmonopd.h>

#include "structs.h"
#include "client.h"
#include "theme.h"
#include "stub.h"
#include "mainwin.h"
#include "misc_functions.h"
#include "game_functions.h"
#include "gdk_functions.h"

/* windows that may pop up */
GtkWidget *mainwin;

/* misc useful globals */
gamelist *gameslist;
gmonopprefs *prefs;
estate *estates[40];
int monopd_pid;

player *players;
trade *trades;
auction *auctions;
gmonopd *server;

int showtraffic;
int clientpid;
int gamestarted;

int main(int argc, char *argv[])
{
	extern double scale_x, scale_y;
	char dir[512];

#ifdef ENABLE_NLS
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain(PACKAGE);
#endif
	gtk_set_locale();
	gtk_init (&argc, &argv);

	prefs = new_malloc(sizeof(gmonopprefs));
	prefs->drawflags = 1;
	prefs->drawdeeds = 1;
	prefs->buttons = 0;

	strcpy(dir, GTKMONOP_DATA);
	strcat(dir, "/themes/");
	prefs->theme = get_theme_info(dir, "gradient");

	initialize_client();

	/* ugly */
	if(argv[1] && !strcmp(argv[1], "--showtraffic"))
		showtraffic = 1;

	scale_x = 1.0f;
	scale_y = 1.0f;

	init_colors();
	init_pixmaps();
	init_pixbufs();

	mainwin = new_mainwin();
	gtk_widget_show(mainwin);

	set_geometry();
	init_board();

	/* easter egg! uncomment this for hours of fun
	g_timeout_add(200, start_fruity_graphics, NULL);
	*/
	gtk_signal_connect (GTK_OBJECT (mainwin), "size-allocate",
			    GTK_SIGNAL_FUNC (on_resize),
			    NULL);

	gtk_main();

	return 0;
}
