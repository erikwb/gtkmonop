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

/* gdk_functions.c: this file sucks.  I hate this file. */

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <string.h>

#include "stub.h"
#include "structs.h"
#include "assetbox.h"
#include "misc_functions.h"
#include "gdk_functions.h"

#define BOARD_X (int)(scale_x*((9*t->space.w)+(2*t->space.h)))
#define BOARD_Y (int)(scale_y*((9*t->space.w)+(2*t->space.h)))

/* globals */
GtkStyle *active_text_style;
GtkStyle *inactive_text_style;

/* filewide globals */
double scale_x;
double scale_y;

static GdkPixmap *boardsnapshot = NULL;
static GdkPixmap *boardpix = NULL;
static GtkWidget *board;
extern player *players;

/* local function prototypes */
static estate *get_estate_from_coords(int, int);
static void draw_icon_rotate(GdkDrawable *, GdkPixmap *, GdkBitmap *, int, int, offsetdata, offsetdata);
static void draw_icon(GdkDrawable *, GdkPixmap *, GdkBitmap *, int, int, offsetdata, offsetdata);
static void redraw_board(GtkWidget *);
static imagedata *new_imagedata(void);
static void load_pixbuf(imagedata *idata, char *name);

/* external function bodies */
void update_board(void) 
{
	player *first, *step;
	int i, j, id;
	extern estate *estates[];
	extern gmonopprefs *prefs;
	theme *t = prefs->theme;
	images *im = t->im;

	gdk_draw_pixmap( boardsnapshot,
			 board->style->fg_gc[board->state],
			 boardpix, 0, 0, 0, 0,
			 -1, -1 );

	for(i = 0; i < 10; i++) for (j = 0; j < 4; j++) {
		id = i+(j*10);
		if(estates[id]->owner) {
			if(prefs->drawflags) /* draw the flags */
				draw_icon(boardsnapshot,
					  (estates[id]->mortgaged ? 
					   im->mflags[estates[id]->owner->id]->pixmap : 
					   im->flags[estates[id]->owner->id]->pixmap),
					  (estates[id]->mortgaged ? 
					   im->mflags[estates[id]->owner->id]->mask : 
					   im->flags[estates[id]->owner->id]->mask),
					  i, j, t->space, t->flag);
		} else if(estates[id]->ownable) {
			if(prefs->drawdeeds) /* draw the deeds */
				draw_icon(boardsnapshot, 
					  im->deeds[estates[id]->group]->pixmap, 
					  im->deeds[estates[id]->group]->mask,
					  i, j, t->space, t->deed);
		}

		if(estates[id]->houses) { /* draw the houses */
			draw_icon_rotate(boardsnapshot, 
					 im->house[(estates[id]->houses)-1][j]->pixmap,
					 im->house[(estates[id]->houses)-1][j]->mask,
					 i, j, t->space, t->house[(estates[id]->houses)-1]);
		}
		
	}
	
	if(players) { /* draw the tokens */
		step = first = players;
		do {
			/* figure out where to put the icon */
			j = (int)(step->position/10);
			i = step->position % 10;
			draw_icon(boardsnapshot, im->tokens[step->id]->pixmap, 
				  im->tokens[step->id]->mask,
				  i, j, t->space, t->token[step->id]);
			step = step->next;
		} while( step != first );
	}
	
	redraw_board(board);
}

void on_resize(void) 
{
	int tmp;
	int board_width;
	double old_scale_x, old_scale_y;
	extern gmonopprefs *prefs;
	
	board_width = 9*prefs->theme->space.w + 2*prefs->theme->space.h;

	tmp = (int)(scale_x*4);
	old_scale_x = (float)tmp/4.0f;
	tmp = (int)(scale_y*4);
	old_scale_y = (float)tmp/4.0f;	
	
	scale_x = ((double)board->allocation.width)/((float)board_width);
	scale_y = ((double)board->allocation.height)/((float)board_width);

	tmp = (int)(scale_x*4);
	scale_x = (float)tmp/4.0f;
	tmp = (int)(scale_y*4);
	scale_y = (float)tmp/4.0f;

	if(old_scale_x != scale_x || old_scale_y != scale_y) {
		init_pixbufs();
		init_board();
	}
}
		
void update_statusbar(void) 
{
	extern gmonopprefs *prefs;
	player *me = player_of_id(prefs->me);
	if(me)
		statprintf("Cash: $%d", me->cash);
}

void init_board(void) 
{
	extern GtkWidget *mainwin;
	extern estate *estates[40];
	extern gmonopprefs *prefs;
	GtkStyle *style;
	int i, j;
	theme *t = prefs->theme;
	images *im = t->im;
	
	board = gtk_object_get_data(GTK_OBJECT(mainwin), "drawingarea");
	style = gtk_widget_get_style(board);

	/* create the board pixmaps */
	if(boardpix) gdk_pixmap_unref(boardpix);
	if(boardsnapshot) gdk_pixmap_unref(boardsnapshot);
	
	boardsnapshot = gdk_pixmap_new(board->window,
				       board->allocation.width,
				       board->allocation.height, -1);
	boardpix = gdk_pixmap_new(board->window,
				       board->allocation.width,
				       board->allocation.height, -1);

	/* set a default bgcolor */
	gdk_draw_rectangle( boardpix,
			    board->style->white_gc,
			    TRUE, 0, 0, board->allocation.width, board->allocation.height);

	/* and draw the corners in */
	gdk_draw_pixmap (boardpix,
			 board->style->fg_gc[board->state],
			 im->corners->pixmap,
			 0, 0,
			 BOARD_X - (int)(scale_x*(t->space.h)), 
			 BOARD_Y - (int)(scale_y*(t->space.h)),
			 (int)(scale_x*(t->space.h)),
			 (int)(scale_y*(t->space.h)));
	gdk_draw_pixmap (boardpix,
			 board->style->fg_gc[board->state],
			 im->corners->pixmap,
			 0, 0,
			 0, BOARD_Y - (int)(scale_y*(t->space.h)),
			 (int)(scale_x*(t->space.h)),
			 (int)(scale_y*(t->space.h)));
	gdk_draw_pixmap (boardpix,
			 board->style->fg_gc[board->state],
			 im->corners->pixmap,
			 0, 0,
			 0, 
			 0,
			 (int)(scale_x*(t->space.h)),
			 (int)(scale_y*(t->space.h)));
	gdk_draw_pixmap (boardpix,
			 board->style->fg_gc[board->state],
			 im->corners->pixmap,
			 0, 0,
			 BOARD_X - (int)(scale_x*(t->space.h)), 0,
			 (int)(scale_x*(t->space.h)),
			 (int)(scale_y*(t->space.h)));
	
	/* and fill it with estate data */
	/* TODO: respect transparency */
	for(i = 1; i < 10; i++) { 
		gdk_draw_pixmap( boardpix,
				 board->style->fg_gc[board->state],
				 im->spaces[0]->pixmap,
				 (estates[i]->group * (int)(scale_x*t->space.w)), 0,
				 BOARD_X - (int)(scale_x*(t->space.h)) - (i * (int)(scale_x*t->space.w)),
				 BOARD_Y - (int)(scale_y*(t->space.h)),
				 (int)(scale_x*(t->space.w)), (int)(scale_y*(t->space.h)));
		gdk_draw_pixmap( boardpix,
				 board->style->fg_gc[board->state],
				 im->spaces[1]->pixmap,
				 0, estates[i+10]->group * (int)(scale_y*t->space.w),
				 0, BOARD_Y - (int)(scale_y*(t->space.h)) - (i * (int)(scale_y*t->space.w)),
				 (int)(scale_x*(t->space.h)), (int)(scale_y*(t->space.w)));
		gdk_draw_pixmap( boardpix,
				 board->style->fg_gc[board->state],
				 im->spaces[2]->pixmap,
				 8*(int)(scale_x*(t->space.w)) - (estates[i+20]->group * (int)(scale_x*(t->space.w))), 0,
				 (int)(scale_x*(t->space.h)) + ((i-1) * (int)(scale_x*t->space.w)), 0,
				 (int)(scale_x*(t->space.w)), (int)(scale_y*(t->space.h)));
		gdk_draw_pixmap( boardpix,
				 board->style->fg_gc[board->state],
				 im->spaces[3]->pixmap,
				 0, 8*(int)(scale_y*(t->space.w)) - (estates[i+30]->group * (int)(scale_y*(t->space.w))),
				 BOARD_X - (int)(scale_x*(t->space.h)), 
				 (int)(scale_y*(t->space.h)) + ((i-1) * (int)(scale_y*t->space.w)),
				 (int)(scale_x*(t->space.h)), (int)(scale_y*(t->space.w)));
	}

	/* and fill the squares with icons */
	
	for(i = 0; i < 10; i++) for(j = 0; j < 4; j++) {
		if(estates[i+(10*j)]->type) {
			if(!strcmp(estates[i+(10*j)]->type, "chance"))
				draw_icon_rotate(boardpix, im->chance[j]->pixmap,
						 im->chance[j]->mask,
						 i, j, t->space, t->chance);
			if(!strcmp(estates[i+(10*j)]->type, "communitychest"))
				draw_icon_rotate(boardpix, im->cchest[j]->pixmap,
						 im->cchest[j]->mask,
						 i, j, t->space, t->cchest);
			if(!strcmp(estates[i+(10*j)]->type, "railroad"))
				draw_icon_rotate(boardpix, im->railroad[j]->pixmap,
						 im->railroad[j]->mask,
						 i, j, t->space, t->railroad);
			if(!strcmp(estates[i+(10*j)]->type, "go"))
				draw_icon_rotate(boardpix, im->go[j]->pixmap, im->go[j]->mask,
						 i, j, t->space, t->go);
			if(!strcmp(estates[i+(10*j)]->type, "freeparking"))
				draw_icon_rotate(boardpix, im->freeparking[j]->pixmap,
						 im->freeparking[j]->mask, i, j, 
						 t->space, t->freeparking);
			if(!strcmp(estates[i+(10*j)]->type, "jail"))
				draw_icon_rotate(boardpix, im->jail[j]->pixmap,
						 im->jail[j]->mask, i, j,
						 t->space, t->jail);
			if(!strcmp(estates[i+(10*j)]->type, "tax"))
				draw_icon_rotate(boardpix, im->tax[j]->pixmap,
						 im->tax[j]->mask, i, j,
						 t->space, t->tax);
		}
	}

	gtk_signal_connect(GTK_OBJECT(board),
			   "expose_event",
			   GTK_SIGNAL_FUNC(redraw_board),
			   NULL);

	update_board();
}

void load_pixbuf(imagedata *idata, char *name)
{
	extern gmonopprefs *prefs;
	char path[1024], file[1024];
	
	strcpy(path, prefs->theme->directory);
	sprintf(file, "%s/gtkmonop-%s.png", path, name);
	
	idata->base = gdk_pixbuf_new_from_file(file);
	if(!idata->base)
		fprintf(stderr, "Failed to open %s\n", file);
	else
		gdk_pixbuf_render_pixmap_and_mask(idata->base, &idata->pixmapbase, &idata->maskbase, 128);
}

void init_pixmaps() 
{
	extern gmonopprefs *prefs;
	char path[1024], file[1024];
	int i, j;
	images *im = prefs->theme->im;
	
	strcpy(path, prefs->theme->directory);
	
	for(i = 0; i < 8; i++) { /* pixmaps with one for each player */
		/* tokens */
		sprintf(file, "token%d", i+1);
		load_pixbuf(im->tokens[i], file);
		sprintf(file, "flag%d", i+1);
		load_pixbuf(im->flags[i], file);
		sprintf(file, "mflag%d", i+1);
		load_pixbuf(im->mflags[i], file);
	}
	
	for(i = 0; i < 9; i++) { /* deeds */
		sprintf(file, "deed%d", i);
		load_pixbuf(im->deeds[i], file);
	}
	
	load_pixbuf(im->corners, "corners");
	
	/* all the rotated pixmaps */
	for(j = 0; j < 4; j++) {
		for(i = 0; i < 5; i++) { /* houses */
			sprintf(file, "house%d-%d", i+1, j*90);
			load_pixbuf(im->house[i][j], file);
		}
		sprintf(file, "spaces-%d", j*90);
		load_pixbuf(im->spaces[j], file);
		sprintf(file, "railroad-%d", j*90);
		load_pixbuf(im->railroad[j], file);
		sprintf(file, "chance-%d", j*90);
		load_pixbuf(im->chance[j], file);
		sprintf(file, "cchest-%d", j*90);
		load_pixbuf(im->cchest[j], file);
		sprintf(file, "jail-%d", j*90);
		load_pixbuf(im->jail[j], file);
		sprintf(file, "tojail-%d", j*90);
		load_pixbuf(im->tojail[j], file);
		sprintf(file, "go-%d", j*90);
		load_pixbuf(im->go[j], file);
		sprintf(file, "freeparking-%d", j*90);
		load_pixbuf(im->freeparking[j], file);
		sprintf(file, "airport-%d", j*90);
		load_pixbuf(im->airport[j], file);
		sprintf(file, "tax-%d", j*90);
		load_pixbuf(im->tax[j], file);
	}
}

GtkWidget *pixmap_label (GdkPixmap *pm, GdkBitmap *mask, char *str)
{
	GtkWidget *box, *widget;
	box = gtk_hbox_new (FALSE, 5);
	widget = gtk_pixmap_new (pm, mask);
	gtk_widget_show (widget);
	gtk_box_pack_start (GTK_BOX(box), widget, TRUE, TRUE, 0);
	widget = gtk_label_new (str);
	gtk_widget_show (widget);
	gtk_box_pack_start (GTK_BOX(box), widget, TRUE, TRUE, 0);

	gtk_object_set_data(GTK_OBJECT(box), "label", widget);
	return box;
}

void init_colors (void)
{
	GtkWidget *label;
	GtkStyle  *rcstyle;

	/* create a dummy label so we can fetch the associated rc style */
	label = gtk_label_new ("");
	rcstyle = gtk_rc_get_style (label);
	gtk_widget_destroy (label);

	/* this is needed for some (broken?) themes */
	if (rcstyle == NULL)
		rcstyle = gtk_style_new ();
	else
		gtk_style_ref (rcstyle);
		
	/* make styles */
	active_text_style = gtk_style_copy (rcstyle);
	inactive_text_style = gtk_style_copy (rcstyle);

	/* load colours */
	active_text_style->fg[0] = rcstyle->fg[4];
	active_text_style->fg[0].red = 65535;

	/* free */
	gtk_style_unref (rcstyle);
}

gint board_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) 
{
	GtkWidget *popup;
	estate *e;

	if(event->button == 3) { /* right click */
		if ((e = get_estate_from_coords((int)event->x, (int)event->y)) &&
		    e->name) {
			popup = new_asset_popup(e);
			gtk_menu_popup(GTK_MENU(popup), NULL, NULL, NULL, NULL, event->button, event->time);
			
			return TRUE;
		}
	}
	return TRUE;
}

void init_pixbufs(void) 
{
	extern gmonopprefs *prefs;
	int i, j;
	theme *t = prefs->theme;
	images *im = t->im;

	if(im->corners->scaled) gdk_pixbuf_unref(im->corners->scaled);
	im->corners->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->corners->base), TRUE, 8, 
					     (int)(t->space.h*4*scale_x), 
					     (int)(t->space.h*scale_y));
	gdk_pixbuf_scale(im->corners->base, im->corners->scaled, 0, 0, 
			 4*(int)(t->space.h*scale_x),
			 (int)(t->space.h*scale_y), 0.0f, 0.0f, scale_x, scale_y, 
			 GDK_INTERP_BILINEAR);
	gdk_pixbuf_render_pixmap_and_mask(im->corners->scaled, &im->corners->pixmap, &im->corners->mask, 128);

	for(i = 0; i < 8; i++) {
		if(im->tokens[i]->scaled) gdk_pixbuf_unref(im->tokens[i]->scaled);
		im->tokens[i]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->tokens[i]->base), TRUE, 8, 
						 (int)(t->token[i].w*scale_x), 
						 (int)(t->token[i].h*scale_y));
		gdk_pixbuf_scale(im->tokens[i]->base, im->tokens[i]->scaled, 0, 0, 
				 (int)(t->token[i].w*scale_x), (int)(t->token[i].h*scale_y),
				 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
		gdk_pixbuf_render_pixmap_and_mask(im->tokens[i]->scaled, &im->tokens[i]->pixmap, &im->tokens[i]->mask, 128);
		
                if(im->flags[i]->scaled) gdk_pixbuf_unref(im->flags[i]->scaled);
		im->flags[i]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->flags[i]->base), TRUE, 8, 
						(int)(t->flag.w*scale_x), (int)(t->flag.h*scale_y));
		gdk_pixbuf_scale(im->flags[i]->base, im->flags[i]->scaled, 0, 0, 
				 (int)(t->flag.w*scale_x), (int)(t->flag.h*scale_y),
				 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
                gdk_pixbuf_render_pixmap_and_mask(im->flags[i]->scaled, &im->flags[i]->pixmap, &im->flags[i]->mask, 128);
		
                if(im->mflags[i]->scaled) gdk_pixbuf_unref(im->mflags[i]->scaled);
                im->mflags[i]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->mflags[i]->base), TRUE, 8, 
						 (int)(t->flag.w*scale_x),
						 (int)(t->flag.h*scale_y));
                gdk_pixbuf_scale(im->mflags[i]->base, im->mflags[i]->scaled, 0, 0, 
                                 (int)(t->flag.w*scale_x), (int)(t->flag.h*scale_y),
                                 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
                gdk_pixbuf_render_pixmap_and_mask(im->mflags[i]->scaled, &im->mflags[i]->pixmap, &im->mflags[i]->mask, 128);
	}
	
	for(i = 0; i < 9; i++) {
		if(im->deeds[i]->scaled) gdk_pixbuf_unref(im->deeds[i]->scaled);
                im->deeds[i]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->deeds[i]->base), TRUE, 8, 
						(int)(t->deed.w*scale_x), (int)(t->deed.h*scale_y));
                gdk_pixbuf_scale(im->deeds[i]->base, im->deeds[i]->scaled, 0, 0, 
                                 (int)(t->deed.w*scale_x), (int)(t->deed.h*scale_y),
                                 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
                gdk_pixbuf_render_pixmap_and_mask(im->deeds[i]->scaled, &im->deeds[i]->pixmap, &im->deeds[i]->mask, 128);
	}
	
	for(j = 0; j < 4; j++) { /* the rotated ones */
		for(i = 0; i < 5; i++) {
			if(im->house[i][j]->scaled) gdk_pixbuf_unref(im->house[i][j]->scaled);
			im->house[i][j]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->house[i][j]->base), TRUE, 8, 
								 (int)((j%2 ? t->house[i].h : t->house[i].w)*scale_x),
								 (int)((j%2 ? t->house[i].w : t->house[i].h)*scale_y));
			gdk_pixbuf_scale(im->house[i][j]->base, im->house[i][j]->scaled, 0, 0,
					 (int)(((j % 2) ? t->house[i].h : t->house[i].w)*scale_x),
					 (int)(((j % 2) ? t->house[i].w : t->house[i].h)*scale_y),
					 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
			gdk_pixbuf_render_pixmap_and_mask(im->house[i][j]->scaled, &(im->house[i][j]->pixmap), &(im->house[i][j]->mask), 128);
		}

		if(im->chance[j]->scaled) gdk_pixbuf_unref(im->chance[j]->scaled);
		im->chance[j]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->chance[j]->base), TRUE, 8, 
						       (int)((j%2 ? t->chance.h : t->chance.w)*scale_x), 
						       (int)((j%2 ? t->chance.w : t->chance.h)*scale_y));
		gdk_pixbuf_scale(im->chance[j]->base, im->chance[j]->scaled, 0, 0,
				 (int)(((j % 2) ? t->chance.h : t->chance.w)*scale_x),
				 (int)(((j % 2) ? t->chance.w : t->chance.h)*scale_y),
				 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
		gdk_pixbuf_render_pixmap_and_mask(im->chance[j]->scaled, &im->chance[j]->pixmap, &im->chance[j]->mask, 128);

                if(im->cchest[j]->scaled) gdk_pixbuf_unref(im->cchest[j]->scaled);
		im->cchest[j]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->cchest[j]->base), TRUE, 8, 
						 (int)((j%2 ? t->cchest.h : t->cchest.w)*scale_x), 
						 (int)((j%2 ? t->cchest.w : t->cchest.h)*scale_y));
                gdk_pixbuf_scale(im->cchest[j]->base, im->cchest[j]->scaled, 0, 0,
                                 (int)(((j % 2) ? t->cchest.h : t->cchest.w)*scale_x),
                                 (int)(((j % 2) ? t->cchest.w : t->cchest.h)*scale_y),
                                 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
                gdk_pixbuf_render_pixmap_and_mask(im->cchest[j]->scaled, &im->cchest[j]->pixmap, &im->cchest[j]->mask, 128);

                if(im->tax[j]->scaled) gdk_pixbuf_unref(im->tax[j]->scaled);
		im->tax[j]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->tax[j]->base), TRUE, 8, 
						 (int)((j%2 ? t->tax.h : t->tax.w)*scale_x), 
						 (int)((j%2 ? t->tax.w : t->tax.h)*scale_y));
                gdk_pixbuf_scale(im->tax[j]->base, im->tax[j]->scaled, 0, 0,
                                 (int)(((j % 2) ? t->tax.h : t->tax.w)*scale_x),
                                 (int)(((j % 2) ? t->tax.w : t->tax.h)*scale_y),
                                 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
                gdk_pixbuf_render_pixmap_and_mask(im->tax[j]->scaled, &im->tax[j]->pixmap, &im->tax[j]->mask, 128);

                if(im->railroad[j]->scaled) gdk_pixbuf_unref(im->railroad[j]->scaled);
		im->railroad[j]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->railroad[j]->base), TRUE, 8, 
						   (int)((j%2 ? t->railroad.h : t->railroad.w)*scale_x), 
						   (int)((j%2 ? t->railroad.w : t->railroad.h)*scale_y));
                gdk_pixbuf_scale(im->railroad[j]->base, im->railroad[j]->scaled, 0, 0,
                                 (int)(((j % 2) ? t->railroad.h : t->railroad.w)*scale_x),
                                 (int)(((j % 2) ? t->railroad.w : t->railroad.h)*scale_y),
                                 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
                gdk_pixbuf_render_pixmap_and_mask(im->railroad[j]->scaled, &im->railroad[j]->pixmap, &im->railroad[j]->mask, 128);

                if(im->jail[j]->scaled) gdk_pixbuf_unref(im->jail[j]->scaled);
		im->jail[j]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->jail[j]->base), TRUE, 8,
						   (int)((j%2 ? t->jail.h : t->jail.w)*scale_x),
						   (int)((j%2 ? t->jail.w : t->jail.h)*scale_y));
                gdk_pixbuf_scale(im->jail[j]->base, im->jail[j]->scaled, 0, 0,
                                 (int)(((j % 2) ? t->jail.h : t->jail.w)*scale_x),
                                 (int)(((j % 2) ? t->jail.w : t->jail.h)*scale_y),
                                 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
                gdk_pixbuf_render_pixmap_and_mask(im->jail[j]->scaled, &im->jail[j]->pixmap, &im->jail[j]->mask, 128);

                if(im->tojail[j]->scaled) gdk_pixbuf_unref(im->tojail[j]->scaled);
		im->tojail[j]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->tojail[j]->base), TRUE, 8,
						   (int)((j%2 ? t->tojail.h : t->tojail.w)*scale_x),
						   (int)((j%2 ? t->tojail.w : t->tojail.h)*scale_y));
                gdk_pixbuf_scale(im->tojail[j]->base, im->tojail[j]->scaled, 0, 0,
                                 (int)(((j % 2) ? t->tojail.h : t->tojail.w)*scale_x),
                                 (int)(((j % 2) ? t->tojail.w : t->tojail.h)*scale_y),
                                 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
                gdk_pixbuf_render_pixmap_and_mask(im->tojail[j]->scaled, &im->tojail[j]->pixmap, &im->tojail[j]->mask, 128);

                if(im->airport[j]->scaled) gdk_pixbuf_unref(im->airport[j]->scaled);
		im->airport[j]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->airport[j]->base), TRUE, 8,
						  (int)((j%2 ? t->airport.h : t->airport.w)*scale_x),
						  (int)((j%2 ? t->airport.w : t->airport.h)*scale_y));
                gdk_pixbuf_scale(im->airport[j]->base, im->airport[j]->scaled, 0, 0,
                                 (int)(((j % 2) ? t->airport.h : t->airport.w)*scale_x),
                                 (int)(((j % 2) ? t->airport.w : t->airport.h)*scale_y),
                                 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
                gdk_pixbuf_render_pixmap_and_mask(im->airport[j]->scaled, &im->airport[j]->pixmap, &im->airport[j]->mask, 128);

                if(im->spaces[j]->scaled) gdk_pixbuf_unref(im->spaces[j]->scaled);
		im->spaces[j]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->spaces[j]->base), TRUE, 8,
						 9*(int)((j%2 ? t->space.h : t->space.w)*scale_x),
						 9*(int)((j%2 ? t->space.w : t->space.h)*scale_y));
                gdk_pixbuf_scale(im->spaces[j]->base, im->spaces[j]->scaled, 0, 0,
                                 9*(int)(((j % 2) ? t->space.h : t->space.w)*scale_x),
                                 9*(int)(((j % 2) ? t->space.w : t->space.h)*scale_y),
                                 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
                gdk_pixbuf_render_pixmap_and_mask(im->spaces[j]->scaled, &im->spaces[j]->pixmap, &im->spaces[j]->mask, 128);

                if(im->go[j]->scaled) gdk_pixbuf_unref(im->go[j]->scaled);
		im->go[j]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->go[j]->base), TRUE, 8,
						 9*(int)((j%2 ? t->go.h : t->go.w)*scale_x),
						 9*(int)((j%2 ? t->go.w : t->go.h)*scale_y));
                gdk_pixbuf_scale(im->go[j]->base, im->go[j]->scaled, 0, 0,
                                 (int)(((j % 2) ? t->go.h : t->go.w)*scale_x),
                                 (int)(((j % 2) ? t->go.w : t->go.h)*scale_y),
                                 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
                gdk_pixbuf_render_pixmap_and_mask(im->go[j]->scaled, &im->go[j]->pixmap, &im->go[j]->mask, 128);

                if(im->freeparking[j]->scaled) gdk_pixbuf_unref(im->freeparking[j]->scaled);
		im->freeparking[j]->scaled = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(im->freeparking[j]->base), TRUE, 8,
						      9*(int)((j%2 ? t->freeparking.h : t->freeparking.w)*scale_x),
						      9*(int)((j%2 ? t->freeparking.w : t->freeparking.h)*scale_y));
                gdk_pixbuf_scale(im->freeparking[j]->base, im->freeparking[j]->scaled, 0, 0,
                                 (int)(((j % 2) ? t->freeparking.h : t->freeparking.w)*scale_x),
                                 (int)(((j % 2) ? t->freeparking.w : t->freeparking.h)*scale_y),
                                 0.0f, 0.0f, scale_x, scale_y, GDK_INTERP_BILINEAR);
                gdk_pixbuf_render_pixmap_and_mask(im->freeparking[j]->scaled, &im->freeparking[j]->pixmap, &im->freeparking[j]->mask, 128);
	}
}

/* local function bodies */
estate *get_estate_from_coords(int x, int y) 
{
	extern estate *estates[];
	extern gmonopprefs *prefs;
	theme *t = prefs->theme;
	int i;
	if(y >= (int)((t->space.h + (9 * t->space.w)) * scale_y)) /* bottom row */
		for(i = 9; i >= 0; i--) 
			if(x >= (int)((t->space.h) + (t->space.w * i)) * scale_x) return estates[9-i];
	if(x <= (int)(t->space.h * scale_x)) /* left column */
		for(i = 9; i >= 0; i--)
			if(y >= (int)((t->space.h) + (t->space.w * i)) * scale_y) return estates[19-i];
	if(y <= (int)(t->space.h * scale_y)) /* top row */
		for(i = 0; i <= 9; i++)
			if(x <= (int)((t->space.h) + (t->space.w * i)) * scale_x) return estates[20+i];
	if(x >= (int)((t->space.h + (9 * t->space.w)) * scale_x)) /* right column */
		for(i = 0; i <= 9; i++)
			if(y <= (int)((t->space.h) + (t->space.w * i)) * scale_y) return estates[30+i];
	return NULL;
}

void draw_icon(GdkDrawable *dest, GdkPixmap *src, GdkBitmap *mask, 
		      int i, int j, offsetdata space, offsetdata offsets) 
{
	GdkGC *gc;
	int x, y;

	switch(j) {
	case 0: /* bottom row */
		x = (space.h + (9 - i)*(space.w)) + (offsets.x);
		y = (space.h + (9 * space.w)) + (offsets.y);
		break;
	case 1: /* left column */
		x = (space.h) - (offsets.y + offsets.h);
		y = (space.h + (9 - i)*(space.w)) + (offsets.x);
		break;
	case 2: /* top row */
		x = (space.h + (i * (space.w))) - (offsets.x + offsets.w);
		y = (space.h) - (offsets.y + offsets.h);
		break;
	case 3: /* right column */
		x = (space.h + (9 * space.w)) + (offsets.y);
		y = (space.h + (i * space.w)) - (offsets.x + offsets.w);
		break;
	default:
		x = y = 0;
	}

	gc = gdk_gc_new(dest);
	gdk_gc_ref(gc);
	gdk_gc_set_clip_mask(gc, mask);
	gdk_gc_set_clip_origin(gc, (int)x*scale_x, (int)y*scale_y);
	
	gdk_draw_pixmap( dest, gc, src, 0, 0, (int)(x*scale_x), (int)(y*scale_y), 
			 (int)(offsets.w*scale_x), (int)(offsets.h*scale_y));
	gdk_gc_unref(gc);
}

void draw_icon_rotate(GdkDrawable *dest, GdkPixmap *src, GdkBitmap *mask, 
		      int i, int j, offsetdata space, offsetdata offsets) 
{
	GdkGC *gc;
	int x, y, w, h;

	switch(j) {
	case 0: /* bottom row */
		x = (space.h + (9 - i)*(space.w)) + (offsets.x);
		y = (space.h + (9 * space.w)) + (offsets.y);
		break;
	case 1: /* left column */
		x = (space.h) - (offsets.y + offsets.h);
		y = (space.h + (9 - i)*(space.w)) + (offsets.x);
		break;
	case 2: /* top row */
		x = (space.h + (i * (space.w))) - (offsets.x + offsets.w);
		y = (space.h) - (offsets.y + offsets.h);
		break;
	case 3: /* right column */
		x = (space.h + (9 * space.w)) + (offsets.y);
		y = (space.h + (i * space.w)) - (offsets.x + offsets.w);
		break;
	default:
		x = y = 0;
	}

	w = (j % 2) ? (int)offsets.h*scale_y : (int)offsets.w*scale_x;
	h = (j % 2) ? (int)offsets.w*scale_x : (int)offsets.h*scale_y;

	gc = gdk_gc_new(dest);
	gdk_gc_ref(gc);
	gdk_gc_set_clip_mask(gc, mask);
	gdk_gc_set_clip_origin(gc, (int)x*scale_x, (int)y*scale_y);
	
	gdk_draw_pixmap( dest, gc, src, 0, 0, (int)(x*scale_x), (int)(y*scale_y), w, h);


	gdk_gc_unref(gc);
}

void redraw_board(GtkWidget *board)
{
	gdk_draw_pixmap( board->window,
			 board->style->fg_gc[board->state],
			 boardsnapshot, 0, 0, 0, 0,
			 -1, -1 );
}

images *new_images(void)
{
	int i, j;

	images *im = (images *)malloc(sizeof(images));
	for(i = 0; i < 8; i++) {
		im->tokens[i] = new_imagedata();
		im->flags[i] = new_imagedata();
		im->mflags[i] = new_imagedata();
		im->deeds[i] = new_imagedata();
	}
	im->deeds[8] = new_imagedata();
	
	for(i = 0; i < 4; i++) {
		im->chance[i] = new_imagedata();
		im->cchest[i] = new_imagedata();
		im->railroad[i] = new_imagedata();
		im->jail[i] = new_imagedata();
		im->airport[i] = new_imagedata();
		im->spaces[i] = new_imagedata();
		im->go[i] = new_imagedata();
		im->freeparking[i] = new_imagedata();
		im->tojail[i] = new_imagedata();
		im->tax[i] = new_imagedata();

		for(j = 0; j < 5; j++)
			im->house[j][i] = new_imagedata();
	}
	im->corners = new_imagedata();

	return im;
}

imagedata *new_imagedata() 
{
	imagedata *im = (imagedata *)malloc(sizeof(imagedata));

	im->pixmap = (GdkPixmap *)NULL;
	im->mask = (GdkBitmap *)NULL;
	im->base = (GdkPixbuf *)NULL;
	im->scaled = (GdkPixbuf *)NULL;

	return im;
}

gboolean start_fruity_graphics(void) 
{
	extern int gamestarted;
	extern estate *estates[40];
	int i;

	if(gamestarted) return FALSE;

	for(i = 0; i < 40; i++) {
		estates[i]->group++;
		if(estates[i]->group > 8)
			estates[i]->group = 1;
	}

	init_board();
	return TRUE;
}

void set_geometry(void) 
{
	extern GtkWidget *mainwin;
	extern gmonopprefs *prefs;
	GtkWidget *drawingarea;
	GdkGeometry geom;
	int board_width;

	drawingarea = gtk_object_get_data(GTK_OBJECT(mainwin), "drawingarea");
	board_width = 9*prefs->theme->space.w + 2*prefs->theme->space.h;

	geom.min_width = board_width/4;
	geom.min_height = board_width/4;
	geom.max_width = board_width;
	geom.max_height = board_width;
	geom.base_width = board_width/4;
	geom.base_height = board_width/4;
	geom.width_inc = board_width/4;
	geom.height_inc = board_width/4;
	geom.min_aspect = 0.50;
	geom.max_aspect = 2.00;

	gtk_window_set_geometry_hints(GTK_WINDOW(mainwin), drawingarea, &geom,
				      (GdkWindowHints)(GDK_HINT_BASE_SIZE | 
						       GDK_HINT_ASPECT | 
						       GDK_HINT_RESIZE_INC | 
						       GDK_HINT_MIN_SIZE));
}

		

