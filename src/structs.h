#ifndef STRUCTS_H
#define STRUCTS_H

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <libxml/tree.h>

#define BUTTON_ROLL 0x001
#define BUTTON_BUYESTATE 0x002
#define BUTTON_AUCTION 0x004
#define BUTTON_ENDTURN 0x008
#define BUTTON_PAYJAIL 0x010
#define BUTTON_JAILROLL 0x020
#define BUTTON_JAILCARD 0x040
#define BUTTON_BANKRUPT 0x080
#define BUTTON_PAYDEBT 0x100

typedef struct player {
        char *name;
        int cash;
        int piece;
	int id;
        int position;
	int gameid;
	int tradeid;
	GtkWidget *grouptree[10];
	struct player *next;
} player;

typedef struct game 
{
	xmlChar *id;
	xmlChar *players;
	xmlChar *name;
	struct game *next;
} game;

typedef struct gamelist 
{
	struct game *games;
} gamelist;

typedef struct offsetdata
{
	int w;
	int h;
	int x;
	int y;
} offsetdata;

typedef struct imagedata 
{
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	GdkPixmap *pixmapbase;
	GdkBitmap *maskbase;
	GdkPixbuf *base;
	GdkPixbuf *scaled;
} imagedata;

typedef struct images 
{
	imagedata *spaces[4];
	imagedata *house[5][4];
	imagedata *tokens[8];
	imagedata *flags[8];
	imagedata *mflags[8];
	imagedata *deeds[9];
	imagedata *chance[4];
	imagedata *cchest[4];
	imagedata *railroad[4];
	imagedata *airport[4];
	imagedata *go[4];
	imagedata *jail[4];
	imagedata *tojail[4];
	imagedata *freeparking[4];
	imagedata *tax[4];
	imagedata *corners;
} images;

typedef struct theme
{
	char *directory;
	
	char *author;
	char *name;
	char *description;

	offsetdata space;
	offsetdata house[5];
	offsetdata token[8];
	offsetdata flag;
	offsetdata deed;
	offsetdata chance;
	offsetdata cchest;
	offsetdata railroad;
	offsetdata airport;
	offsetdata go;
	offsetdata jail;
	offsetdata tojail;
	offsetdata freeparking;
	offsetdata tax;

	images *im;
} theme;

typedef struct gmonopprefs
{
	int me;
	int currentturn;
	int drawflags;
	int drawdeeds;
	int buttons;
	char *monopdpath;
	theme *theme;
} gmonopprefs;

typedef struct estate {
	char *name;
	char *type;
	
	player * owner;

	int mortgaged;
	int houses;
	int group;
	int ownable, mortgageable, unmortgageable, canbuyhouses, cansellhouses;
	int price, houseprice, sellhouseprice, mortgageprice, unmortgageprice;
	int id;
	int dummy;
	int rent[6];

	GtkCTreeNode *node;
	GtkWidget *estateview;
	int estateview_open;
} estate;

typedef struct trade 
{
	char *name;
	int id;
	int included[40];
	player *p[2];
	GtkWidget *widget;
	GtkWidget *frame[2];
	GtkWidget *tree[2];
	GtkCTreeNode *root[2];
	GtkWidget *moneytree[2];
	GtkCTreeNode *grouptree[10][2];
	GtkCTreeNode *esnode[40];
	GtkWidget *tablabel;
	GtkToggleButton *acceptbutton;
	struct trade *next;
} trade;

typedef struct auction
{
	player *actor;
	int id;
	int count;
	int highbid;
	estate *estate;
	player *highbidder;
	GtkWidget *widget;
	GtkWidget *tablabel;
	struct auction *next;
} auction;

#endif
