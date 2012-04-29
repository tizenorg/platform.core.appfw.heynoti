
Name:       heynoti
Summary:    HEY (ligHt Easy speedy) notification library
Version:    0.0.2
Release:    35
Group:      System/Libraries
License:    Apache License, Version 2.0
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

BuildRequires:  cmake

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


%post
/sbin/ldconfig
mkdir -p /opt/share/noti
chmod 1755 /opt/share/noti

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%{_libdir}/libheynoti.so.0
%{_libdir}/libheynoti.so.0.0.2
%{_bindir}/heynotitool


%files devel
%defattr(-,root,root,-)
%{_includedir}/heynoti/SLP_Heynoti_PG.h
%{_includedir}/heynoti/heynoti.h
%{_libdir}/pkgconfig/heynoti.pc
%{_libdir}/libheynoti.so
