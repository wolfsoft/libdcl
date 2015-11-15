#!/bin/sh

# cleaning up
test ! -f Makefile || make distclean

# delete all temporary and backup files
find . -iname '*~'|xargs rm -f
find . -iname '*.bak'|xargs rm -f
find . -iname '*.lot'|xargs rm -f
find . -iname ".deps"|xargs rm -rf
find . -iname ".libs"|xargs rm -rf

# brought in by gettext
rm -f gettext.h
rm -rf intl
rm -f po/POTFILES.in

# brought in by autopoint
rm -f ABOUT-NLS
rm -rf build
rm -rf intl
find m4/* -prune -not -iname 'ax_*' -not -iname 'Makefile.am'|xargs rm -f
rm -f po/Makefile.in.in
rm -f po/remove-potcdate.sin

# generated by aclocal
rm -f aclocal.m4

# generated by autoconf
rm -f configure

# generated or brought in by automake
rm -f po/*.pot
rm -f po/stamp-po
rm -f po/*.gmo
rm -f aclocal.m4
rm -f configure
rm -rf autom4te.cache
rm -rf build/
find . -name Makefile|xargs rm -f
find . -name Makefile.in|xargs rm -f
rm -f install-sh
rm -f missing
rm -f config.*
rm -f stamp-h1
rm -f depcomp
rm -f ltmain.sh
rm -f libtool
