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

/* client.c: connects to the monopd server and adds the gIoChannel to the main
   event loop */

#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <libgmonopd/libgmonopd.h>
#include <glib.h>
#include <libxml/parser.h>

#include "misc_functions.h"
#include "game_functions.h"
#include "client.h"

#define MAXMESSAGE 20000 /* hope that's enough, eh */

/* globals */
int connected = 0;

/* filewide globals */
GIOChannel *io = NULL;
int io_id = 0;

/* translate between the enum values and the strings we'll receive */
struct inmsgt {
	enum inmsg_type num;
	char *str;
};
struct outmsgt {
	enum outmsg_type num;
	char *str;
};

struct inmsgt inmsgtable[] = {
    {IN_COMMANDLIST, "commandlist"},
    {IN_UPDATEGAMELIST, "updategamelist"},
    {IN_PLAYERUPDATE, "playerupdate"},
    {IN_UPDATEPLAYERLIST, "updateplayerlist"},
    {IN_MSG, "msg"},
    {IN_NEWTURN, "newturn"},
    {IN_ESTATEUPDATE, "estateupdate"},
    {IN_SERVER, "server"},
    {IN_CLIENT, "client"},
    {IN_TRADEUPDATE, "tradeupdate"},
    {IN_AUCTIONUPDATE, "auctionupdate"},
    {IN_GAMEUPDATE, "gameupdate"},
    {IN_DISPLAY, "display"},
    /* internal communication here */
    {0, 0}
};

struct outmsgt outmsgtable[] = {
    {OUT_DISCONNECT, ".d"},
    {OUT_CHAT, ""},
    {OUT_GAMENEW, ".gn"},
    {OUT_GAMEDESC, ".gd"},
    {OUT_GAMESTART, ".gs"},
    {OUT_BUY, ".eb"},
    {OUT_AUCTION, ".ea"},
    {OUT_AUCTIONBID, ".a"},
    {OUT_BUYHOUSE, ".hb"},
    {OUT_SELLHOUSE, ".hs"},
    {OUT_ENDTURN, ".E"},
    {OUT_NICK, ".n"},
    {OUT_USECARD, ".c"},
    {OUT_LISTGAME, ".gl"},
    {OUT_JOINGAME, ".gj"},
    {OUT_LISTPLAYERS, ".gp"},
    {OUT_MORTGAGE, ".m"},
    {OUT_PAYJAIL, ".jp"},
    {OUT_JAILCARD, ".jc"},
    {OUT_JAILROLL, ".jr"},
    {OUT_TRADENEW, ".Tn"},
    {OUT_TRADEACCEPT, ".Ta"},
    {OUT_TRADEESTATE, ".Te"},
    {OUT_TRADEMONEY, ".Tm"},
    {OUT_TRADEREJECT, ".Tr"},
    {OUT_ROLL, ".r"},
    {OUT_PAYDEBT, ".p"},
    {OUT_DECLAREBANKRUPT, ".D"},
    {OUT_UNMORTGAGE, ".u"},
    {OUT_CONFIRM, ".t"},
};

/* local function prototypes */
gboolean on_input_received(GIOChannel *);
enum inmsg_type inmsg_translate (xmlNodePtr);
char *outmsg_translate (enum outmsg_type);

/* external function bodies */
void client_init(char *hostname, int port)
{
	int sock;
	struct hostent *h;
	struct sockaddr_in sa;

	/* set up the connection */
	h = gethostbyname (hostname);
	memset(&sa, 0, sizeof(sa));
	memcpy(&sa.sin_addr, h->h_addr, h->h_length);
	sa.sin_family = h->h_addrtype;
	sa.sin_port = htons(port);
	sock = socket(sa.sin_family, SOCK_STREAM, 0);
	if(sock < 0) return;
	if(connect (sock, (struct sockaddr *)&sa, sizeof(sa)) < 0)
		return;
	
	io = g_io_channel_unix_new(sock);
	
	io_id = g_io_add_watch(io, G_IO_IN, (GIOFunc) on_input_received, NULL);
	connected = 1;
}

void client_disconnect() 
{
	extern int gamestarted;

	if(io_id) {
		g_source_remove(io_id);
		g_io_channel_close(io);
		g_io_channel_unref(io);
		io_id = 0;
	}
	
	gamestarted = connected = 0;
}

void client_outmessage(enum outmsg_type msgtype, char *str) 
{
	char buf[MAXMESSAGE];
	int written;
	extern int showtraffic;
	
	strcpy (buf, outmsg_translate(msgtype));
	if (str)
		strcat (buf, str);
	
	sprintf(buf, "%s\r\n", buf);
	if(showtraffic) fprintf(stderr, "%s\n", buf);	

	g_io_channel_write(io, buf, strlen(buf), &written);
}

void shutdown_gtkmonop(GtkWidget *mainwin) 
{
	extern gmonopd *server;
	if(connected) client_disconnect();
	if(server) gmonopd_kill(server);
	
	gtk_widget_destroy(mainwin);
	gtk_main_quit();
}

/* local function bodies */
gboolean on_input_received(GIOChannel *io) 
{
	extern int showtraffic;
	char *buf;
	int read;
	enum inmsg_type msgtype;
	xmlDocPtr doc;
	xmlNodePtr node;
	int i;
	
	buf = new_malloc(MAXMESSAGE);

	/* read a single line */
	for(i = 0, read = 1; read > 0; i++) {
		g_io_channel_read(io, &buf[i], 1, &read);
		if(buf[i] == '\n') break;
	}
	buf[++i] = 0;
	
	if(showtraffic) fprintf(stderr, "%s\n", buf);

	if(buf[0] == '<') { /* looks like an XML element */
		doc = xmlParseMemory(buf, strlen(buf));
		if((doc != (xmlDocPtr)NULL) &&
		   (!xmlStrcmp("monopd", doc->children->name))) {
			/* after the <monopd> */
			node = doc->children->children; 
			do {
				msgtype = inmsg_translate(node);
				monopd_inmessage(msgtype, doc, node);
				node = node->next;
			} while(node != NULL); /* get all messages */ 
		}
	}
	
	free(buf);
	return TRUE;
}

enum inmsg_type inmsg_translate (xmlNodePtr node)
{
	char *str;
	int i;
	
	str = (char *)(node->name);
	
	for (i = 0; inmsgtable[i].str; i++)
		if(!strcmp(str, inmsgtable[i].str))
			return inmsgtable[i].num;
	return 0;
}

char *outmsg_translate (enum outmsg_type num)
{
	int i;
	for (i = 0; outmsgtable[i].num; i++)
		if (outmsgtable[i].num==num) return outmsgtable[i].str;
	return NULL;
}
