#  RedHat package specification file
#
#  This file is part of dbPager Classes Library (DCL)
#
#  Copyright (c) 2010 Dennis Prochko <wolfsoft@mail.ru>
#
#  DCL is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation version 3.
#
#  DCL is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with DCL; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor,
#  Boston, MA  02110-1301  USA

Summary: A C++ library for cross-platform application development
Name: libdcl
Version: 0.1.4
Release: 1
License: GPLv3
Group: System Environment/Libraries
URL: http://dbpager.com/dcl
Packager: Dennis Prochko <wolfsoft@mail.ru>
Vendor: Dennis Prochko, http://dbpager.com/dcl

Buildrequires: gcc-c++, make, doxygen
%if 0%{?suse_version} || 0%{?sles_version}
Buildrequires: libxslt, apache2-devel, unixODBC-devel, gtk2-devel, libqt4-devel, libopenssl-devel
%endif
%if 0%{?mdkversion}
Buildrequires: xsltproc, apache-devel, libunixODBC-devel, libgtk+2.0_0-devel, libqt4-devel, libopenssl0.9.8-devel
%endif
%if 0%{?fedora}
Buildrequires: libxslt, httpd-devel, unixODBC-devel, gtk2-devel, qt4-devel, openssl-devel
%endif
%if 0%{?centos_version} || 0%{?rhel_version}
Buildrequires: libxslt, httpd-devel, unixODBC-devel, gtk2-devel, qt4-devel, openssl-devel
%endif


Source: libdcl-%{version}.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
The dbPager Classes Library (DCL) is a C++ framework for cross-platform
application development.
DCL provides single-source portability across Microsoft Windows, Linux and all
major Unix variants.

%package devel
Summary: Header files, libraries, examples and development documentation for %{name}
Group: Development/Libraries

%description devel
The dbPager Classes Library (DCL) is a C++ framework for cross-platform
application development.
DCL provides single-source portability across Microsoft Windows, Linux and all
major Unix variants.
This is a development files and the documentation of the library needed by
software developer who wants to use the library in own software.

%package -n libdclgtk
Summary: A C++ library for cross-platform application development (GUI backend)
Group: System Environment/Libraries

%description -n libdclgtk
The dbPager Classes Library (DCL) is a C++ framework for cross-platform
application development.
DCL provides single-source portability across Microsoft Windows, Linux and all
major Unix variants.
This is a library GUI GTK+ backend.

%package -n libdclnet
Summary: A C++ library for cross-platform application development (network)
Group: System Environment/Libraries

%description -n libdclnet
The dbPager Classes Library (DCL) is a C++ framework for cross-platform
application development.
DCL provides single-source portability across Microsoft Windows, Linux and all
major Unix variants.
This is a network library.

%package -n libdclodbc
Summary: A C++ library for cross-platform application development (database)
Group: System Environment/Libraries

%description -n libdclodbc
The dbPager Classes Library (DCL) is a C++ framework for cross-platform
application development.
DCL provides single-source portability across Microsoft Windows, Linux and all
major Unix variants.
This is a database (ODBC) library.

%prep
%setup

%build

%if 0%{?mandriva_version} > 2006  
export PATH=/usr/lib/qt4/bin:$PATH  
export QTDIR=%{_prefix}/lib/qt4/  
%endif  

%configure \
%if 0%{?suse_version} || 0%{?sles_version}
	--docdir=$RPM_BUILD_ROOT%{_docdir}/%{name} \
%endif
	--disable-doxygen-ps \
	--disable-doxygen-pdf
%{__make} %{?_smp_mflags}

%install
%{__rm} -rf %{buildroot}
%makeinstall

%post
/sbin/ldconfig 2>/dev/null

%postun
/sbin/ldconfig 2>/dev/null

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%doc AUTHORS ChangeLog COPYING NEWS README
%doc %{_mandir}/man3/libdclbase.3.gz
%{_libdir}/libdclbase*.so.*
%{_datadir}/locale/*/*/*.mo

%files devel
%defattr(-, root, root, 0755)
%if 0%{?suse_version} || 0%{?sles_version}
%{_docdir}/%{name}/html/
%else
%doc %{_docdir}/%{name}/html/
%endif
%doc examples
%{_includedir}/dcl/
%{_libdir}/*.so
%{_libdir}/*.la
%{_libdir}/*.a
%{_libdir}/pkgconfig/*.pc

%files -n libdclgtk
%defattr(-, root, root, 0755)
%{_libdir}/libdclgtk*.so.*

%files -n libdclnet
%defattr(-, root, root, 0755)
%{_libdir}/libdclnet*.so.*

%files -n libdclodbc
%defattr(-, root, root, 0755)
%{_libdir}/libdclodbc*.so.*

%changelog
* Fri May 28 2010 Dennis Prochko <wolfsoft@mail.ru> - 0.1.3
- Bugfixes from upstream.

* Fri Jan 15 2010 Dennis Prochko <wolfsoft@mail.ru> - 0.1.2
- Bugfixes from upstream.

* Mon Dec 07 2009 Dennis Prochko <wolfsoft@mail.ru> - 0.1.0-1
- Initial package.
