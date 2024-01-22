%bcond_without zypp
%bcond_without packagekit

%if 0%{!?cmake_build:1}
%define __cmake_in_source_build 1
%define _vpath_srcdir .
%define _vpath_builddir %{_target_platform}
%define cmake_build \
  mkdir -p %{_vpath_builddir};%__cmake --build "%{__cmake_builddir}" %{?_smp_mflags} --verbose
%define __cmake_builddir %{!?__cmake_in_source_build:%{_vpath_builddir}}%{?__cmake_in_source_build:.}
%endif
%if 0%{!?cmake_install:1}
%define cmake_install \
  DESTDIR="%{buildroot}" %__cmake --install "%{__cmake_builddir}"
%endif

Summary: Community Os Update Service
Name: cosupdateservice
Version: 0.1
Release: 0
License: GPL-2.0
URL: https://github.com/SailfishOS-SonyXperia/cosupdateservice
Source0: %{name}-%{version}.tar.gz
BuildRequires: cmake
BuildRequires: extra-cmake-modules
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5DBus)
BuildRequires: pkgconfig(Qt5Test)
BuildRequires: pkgconfig(ssu)
# FIXME cmake symbols not found eventhou
# they are used and not pkgconfig
%if %{with packagekit}
BuildRequires: pkgconfig(packagekitqt5)
%endif
%if %{with zypp}
BuildRequires: pkgconfig(libzypp)
%endif
BuildRequires: pkgconfig(libsystemd)

%description

%prep
%autosetup -p1 %{name}-%{version}

%build
mkdir -p build
cd build
# Workaround broken cmake macro
%cmake -DLIBEXEC_INSTALL_DIR:PATH=%{_libexecdir}  ..
%cmake_build

%install
cd build
%cmake_install

install -dm755 %{buildroot}%{_sysconfdir}/%{name}/conf.d


%post
%systemd_user_post cosupdatesearch.timer cosupdateproxy.service

%preun
%systemd_user_preun cosupdatesearch.timer cosupdateproxy.service

%postun
%systemd_user_postun cosupdatesearch.timer cosupdateproxy.service

%files
%defattr(-,root,root,-)
%{_libexecdir}/%{name}
%{_libexecdir}/cosupdatesearch
%{_libexecdir}/cosupdateproxy
%{_datadir}/dbus-1/interfaces/org.sailfishos.CosUpdater.xml
%{_datadir}/dbus-1/services/org.sailfishos.CosUpdater.service
%{_datadir}/mapplauncherd/privileges.d/%{name}.privileges
%{_datadir}/qlogging-categories5
%{_prefix}/lib/%{name}/conf.d
%{_userunitdir}/cosupdatesearch.timer
%{_userunitdir}/cosupdatesearch.service
%{_userunitdir}/cosupdateproxy.service
%{_userunitdir}/%{name}.service
%{_userpresetdir}/%{name}.preset
%dir %{_sysconfdir}/%{name}
%dir %{_sysconfdir}/%{name}/conf.d
