# Add doxygen support
include aminclude.am

if DX_COND_doc
all-local: doxygen-doc
else
all-local:
	echo "No need to build the documentation"
endif

# Additional files to be distributed.
EXTRA_DIST = $(DX_CONFIG) aminclude.am footer.htm manpage.xsl *.xml *.txt

# Clean all doxygen generated files
MOSTLYCLEANFILES = $(DX_CLEANFILES)

# Generate man pages
cleantext = $(sed) -e 's/^[ \t]*//;s/[ \t]*$$//;/^$$/d'

libdclbase.3: libdclbase.3.xml
	$(xsltproc) $< | $(cleantext) > $@

man_MANS = libdclbase.3

# Clean up
clean-local:
	rm -rf doxygen-doc
	rm -f libdclbase.3
	
# Install
if DX_COND_doc
guidedir = @docdir@/html

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(guidedir)
	current_dir=`pwd`
	cd doxygen-doc/html/ && tar --exclude='.svn' -cf - . | tar -C $(DESTDIR)$(guidedir) -xvf -
	cd $(current_dir)

uninstall-local:
	if test -d $(DESTDIR)$(guidedir); then \
		rm -rf $(DESTDIR)$(guidedir)/*; \
		rmdir $(DESTDIR)$(guidedir) || true; \
	fi
endif

