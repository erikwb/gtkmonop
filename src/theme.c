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

/* theme.c: functions that deal with reading and retrieving theme info */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "stub.h"
#include "misc_functions.h"
#include "gdk_functions.h"
#include "structs.h"
#include "theme.h"

/* local function prototypes */
theme *new_theme(char *name);
int load_offsets(char *, char *, offsetdata *);

/* external function bodies */
theme *get_theme_info (char *dir, char *themedir) 
{
	FILE *f;
	char str[256], str2[256], *buf;
	theme *t;
	int i, next;

	strcpy(str, dir);
	strcat(str, themedir);
	strcat(str, "/theme.conf");
	
	f = fopen(str, "r");
	if(!f)
		return NULL; /* no theme.conf */

	/* make the theme */
	t = new_theme(themedir);
	t->directory = new_malloc(strlen(dir)+strlen(themedir)+1);
	sprintf(t->directory, "%s%s", dir, themedir);

	while(!feof(f)) { 
		/* fill it with info from theme.conf */
		next = 0;
		fgets(str, sizeof(str), f);	
		
		if(strstr(str, "=")) {
			buf = strtok(str, "=");
			if(!strcmp(buf, "author")) {
				buf = strtok(NULL, "\n\0");
				t->author = new_malloc(strlen(buf) + 1);
				strcpy(t->author, buf);
			} else if(!strcmp(buf, "description")) {
				buf = strtok(NULL, "\n\0");
				t->description = new_malloc(strlen(buf) + 1);
				strcpy(t->description, buf);
			} else {
				if(load_offsets(buf, "space", &(t->space))) continue;
				if(load_offsets(buf, "flag", &(t->flag))) continue;
				if(load_offsets(buf, "deed", &(t->deed))) continue;
				if(load_offsets(buf, "chance", &(t->chance))) continue;
				if(load_offsets(buf, "cchest", &(t->cchest))) continue;
				if(load_offsets(buf, "airport", &(t->airport))) continue;
				if(load_offsets(buf, "railroad", &(t->railroad))) continue;
				if(load_offsets(buf, "go", &(t->go))) continue;
				if(load_offsets(buf, "jail", &(t->jail))) continue;
				if(load_offsets(buf, "tojail", &(t->tojail))) continue;
				if(load_offsets(buf, "freeparking", &(t->freeparking))) continue;
				if(load_offsets(buf, "tax", &(t->tax))) continue;

				for(i = 1; i < 6; i++) {
					sprintf(str2, "house%d", i);
					if(load_offsets(buf, str2, &(t->house[i-1]))) {
						next = 1;
						break;
					}
				}
				if(next) continue;

				for(i = 1; i < 9; i++) {
					sprintf(str2, "token%d", i);
					if(load_offsets(buf, str2, &(t->token[i-1]))) {
						next = 1;
						break;
					}
				}
				if(next) continue;
			}
		}
	}
	return t;
}

/* local function bodies */
theme *new_theme(char *name) 
{
	theme *t;
	t = new_malloc(sizeof(theme));
	memset(t, 0, sizeof(theme));

	t->name = malloc(strlen(name)+1);
	strcpy(t->name, name);

	t->im = new_images();

	return t;
}

/* load_offsets really checks to see if the line is applicable to
   attrib, shoots the data into offset, and exits */
int load_offsets(char *buf, char *attrib, offsetdata *offset) 
{
	char str[1024];
	
	sprintf(str, "%s_x", attrib);
	if(!strcmp(buf, str)) {
		buf = strtok(NULL, "\n\0");
		offset->w = atoi(buf);
		return 1;
	}

	sprintf(str, "%s_y", attrib);
	if(!strcmp(buf, str))
	{
		buf = strtok(NULL, "\n\0");
		offset->h = atoi(buf);
		return 1;
	}

	sprintf(str, "%s_x_offset", attrib);
	if(!strcmp(buf, str)) {
		buf = strtok(NULL, "\n\0");
		offset->x = atoi(buf);
		return 1;
	}

	sprintf(str, "%s_y_offset", attrib);
	if(!strcmp(buf, str)) {
		buf = strtok(NULL, "\n\0");
		offset->y = atoi(buf);
		return 1;
	}
	return 0;
}
