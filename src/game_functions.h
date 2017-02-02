#ifndef GAME_FUNCTIONS_H
#define GAME_FUNCTIONS_H

#include <libxml/parser.h>
#include "client.h"

extern void monopd_inmessage (enum inmsg_type, xmlDocPtr, xmlNodePtr);
extern void initialize_client(void);

#endif
