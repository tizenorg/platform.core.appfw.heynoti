
Name:       heynoti
Summary:    HEY (ligHt Easy speedy) notification library
Version:    0.0.2
Release:    5.1
Group:      System/Libraries
License:    GNU LGPL
Source0:    heynoti-%{version}.tar.gz
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
#BuildRequires:  pkgconfig(sglib)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(ecore)


%description
lightweight notification library, service APIs


%package devel
Summary:    Notification library
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   heynoti

%description devel
heynoti API (devel)


%prep
%setup -q 


%build
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DCMAKE_BUILD_TYPE="Debug"


make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install




%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig





%files
%{_libdir}/libheynoti.so.0
%{_libdir}/libheynoti.so.0.0.2
%{_bindir}/heynotitool
#/usr/bin/heynotitool

%files devel
%{_includedir}/heynoti/SLP_Heynoti_PG.h
%{_includedir}/heynoti/heynoti.h
%{_libdir}/pkgconfig/heynoti.pc
%{_libdir}/libheynoti.so

