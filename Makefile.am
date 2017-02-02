SUBDIRS=src

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/themes; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/themes; \
	  for themedir in $(srcdir)/themes/*; do \
	    if test -d $$themedir; then \
	      $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/$$themedir; \
	      for themefile in $(srcdir)/$$themedir/*; do \
	        if test -f $$themefile; then \
                  $(INSTALL_DATA) $$themefile $(DESTDIR)$(pkgdatadir)/$$themedir; \
                fi \
              done \
            fi \
          done \
        fi