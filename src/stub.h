#ifndef STUB_H
#define STUB_H

#include <stdio.h>
#include <unistd.h>

/* standard gettext macros */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

#define STUB_FUNCTION tprintf("\tSTUB: %s at " __FILE__ ", line %d, thread %d\n",__FUNCTION__,__LINE__,getpid())

#define SEGV tprintf("\tSEGV requested: %s at " __FILE__ ", line %d, thread %d\n",__FUNCTION__,__LINE__,getpid());printf("%d\n", *((int *)NULL))

#ifdef EXTREME_DEBUG_MESSAGES
#  define EENTER tprintf("Entering " __FUNCTION__ "() ...\n")
#  define EEXIT tprintf("Exiting " __FUNCTION__ "() ...\n")
#else
#  define EENTER
#  define EEXIT
#endif

#ifdef DEBUG_MESSAGES
#  define ENTER tprintf("entering " __FUNCTION__ "() ...\n")
#  define EXIT  tprintf("exiting " __FUNCTION__ "() ...\n")
#else
#  define ENTER
#  define EXIT
#endif

#endif
