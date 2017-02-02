## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@ \
        -I/usr/include/libxml2
	
CPPFLAGS= -DGTKMONOP_DATA=\"$(pkgdatadir)\"

bin_PROGRAMS = gtkmonop

gtkmonop_SOURCES = \
	main.c \
	mainwin.c mainwin.h \
	aboutwin.c aboutwin.h \
	prefswin.c prefswin.h \
	notebook.c notebook.h \
	client.c client.h \
	game_functions.c game_functions.h \
	gdk_functions.c gdk_functions.h \
	misc_functions.c misc_functions.h \
	xmlparse.c xmlparse.h \
	assetbox.c assetbox.h \
	tradebox.c tradebox.h \
	auctionbox.c auctionbox.h \
	estateview.c estateview.h \
	cardview.c cardview.h \
	theme.c theme.h \
	structs.h stub.h 

gtkmonop_LDADD = @GTK_LIBS@ -lxml2 -lgmonopd -lpthread -lgdk_pixbuf

