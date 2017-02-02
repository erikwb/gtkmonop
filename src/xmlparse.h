#ifndef XMLPARSE_H
#define XMLPARSE_H

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

extern void parse_display (xmlDocPtr, xmlNodePtr);
extern void parse_commandlist (xmlDocPtr, xmlNodePtr);
extern void parse_updateplayerlist(xmlDocPtr, xmlNodePtr);
extern void parse_client(xmlDocPtr, xmlNodePtr);
extern void parse_gamelist(xmlDocPtr, xmlNodePtr);
extern void parse_updategamelist(xmlDocPtr, xmlNodePtr);
extern void parse_msg(xmlDocPtr, xmlNodePtr);
extern void parse_playerupdate(xmlDocPtr, xmlNodePtr);
extern void parse_newturn(xmlDocPtr, xmlNodePtr);
extern void parse_estateupdate(xmlDocPtr, xmlNodePtr);
extern void parse_tradeupdate(xmlDocPtr, xmlNodePtr);
extern void parse_auctionupdate(xmlDocPtr, xmlNodePtr);
extern void parse_gameupdate(xmlDocPtr, xmlNodePtr);

#endif
