%define name gtkmonop
%define version 0.3.0
%define release 1

Summary: gtkmonop
Name: %{name}
Version: %{version}
Release: %{release}
License: GPL
Group: Amusements/Games
Source: %{name}-%{version}.tar.bz2
URL: http://gtkmonop.sourceforge.net
Buildroot: %{_tmppath}/%{name}-buildroot

%description
gtkmonop is a GTK+ client to the monopd boradgame server.

%prep
rm -rf $RPM_BULID_ROOT

%setup

%build

%configure

%make

%install
%makeinstall

%post

%postun

%clean
rm -rf $RPM_BULID_ROOT

%files
%defattr(-,root,root)
%doc AUTHORS COPYING NEWS README ChangeLog
/usr/bin/*
/usr/share/gtkmonop/*
/usr/share/man/*

%changelog
* Sun Oct 28 2001 Erik Bourget <ebourg@po-box.mcgill.ca> 0.3.0
- made an rpm
