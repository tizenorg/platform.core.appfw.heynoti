#

Name:           heynoti
Version:        0.0.2
Release:        41
License:        Apache License, Version 2.0
Summary:        HEY (ligHt Easy speedy) notification library
Group:          System/Libraries
Source0:        %{name}-%{version}.tar.gz
Source1001: 	heynoti.manifest

BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(glib-2.0)

%description
lightweight notification library, service APIs

%package devel
Summary:        Notification library
Group:          Development/Libraries
Requires:       %{name} = %{version}
Requires:       heynoti

%description devel
heynoti API (devel)

%prep
%setup -q
cp %{SOURCE1001} .


%build
%cmake . -DCMAKE_BUILD_TYPE="Debug"


make %{?_smp_mflags}

%install
%make_install
mkdir -p %{buildroot}/usr/share/license
install LICENSE %{buildroot}/usr/share/license/%{name}
mkdir -p %{buildroot}/opt/share/noti

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/libheynoti.so.0
%{_libdir}/libheynoti.so.0.0.2
%{_bindir}/heynotitool
%attr(1755,root,root) /opt/share/noti
/usr/share/license/%{name}


%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_includedir}/heynoti/SLP_Heynoti_PG.h
%{_includedir}/heynoti/heynoti.h
%{_libdir}/pkgconfig/heynoti.pc
%{_libdir}/libheynoti.so

