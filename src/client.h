#ifndef CLIENT_H
#define CLIENT_H

/* enums */
enum inmsg_type {
	IN_UNUSED, 
	IN_NOMSG,

	/* Network messages */
	IN_SERVER,
	IN_CLIENT,

	/* In-game updates */
	IN_GAMEUPDATE,
	IN_PLAYERUPDATE,
	IN_ESTATEUPDATE,
	IN_TRADEUPDATE,
	IN_AUCTIONUPDATE,
	IN_NEWTURN,

	/* List updates */
	IN_UPDATEPLAYERLIST,
	IN_UPDATEGAMELIST,

	/* Informative stuff */
	IN_COMMANDLIST,
	IN_MSG,
	IN_DISPLAY
};

enum outmsg_type {
	OUT_UNUSED,

	/* Game commands */
	OUT_GAMENEW, 
	OUT_GAMESTART,
	OUT_JOINGAME,
	OUT_GAMEDESC,
	
	/* Estate commands */
	OUT_BUY,
	OUT_AUCTION,
	OUT_AUCTIONBID,
	OUT_MORTGAGE,
	OUT_UNMORTGAGE,
	OUT_BUYHOUSE,
	OUT_SELLHOUSE,
	
	/* Trade commands */
	OUT_TRADENEW,
	OUT_TRADEESTATE,
	OUT_TRADEMONEY,
	OUT_TRADEREJECT,
	OUT_TRADEACCEPT,
	
	/* Jail commands */
	OUT_JAILROLL,
	OUT_JAILCARD,
	OUT_PAYJAIL,
	OUT_USECARD,
	
	/* Linux stock market */
	OUT_DECLAREBANKRUPT,
	OUT_PAYDEBT,

	/* Normal turn commands */
	OUT_ROLL,
	OUT_ENDTURN,
	OUT_CONFIRM,
	
	/* Miscellaneous stuff */
	OUT_LISTGAME,
	OUT_LISTPLAYERS,
	OUT_NICK,
	OUT_CHAT,
	OUT_DISCONNECT
};

extern void client_init(char *, int);
extern void client_disconnect();
extern void shutdown_gtkmonop(GtkWidget *);
extern void client_outmessage (enum outmsg_type msgtype, char *str);

#endif
