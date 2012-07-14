# no post install
#%define __os_install_post %{nil}
# no strip
#%define __strip /bin/true
# no debug package
#%define debug_package %{nil}
# disable automatic dependency processing
#AutoReqProv: no

Name:           ru.webhamster.mytetra
Version:        1.28.5
Release:        1
License:        GPL v.3.0
Summary:        Personal manager for information accumulation
Url:            http://webhamster.ru/site/page/index/articles/projectcode/138
Group:          Utility
Source0:        %{name}-%{version}.tar.gz
#BuildRequires:  pkgconfig(QtCore)
BuildRequires:  libqt-devel

%description
This is effective personal manager for information accumulation.

%prep
%setup -q

%build
# Add here commands to configure the package.
#%qmake
#qmake -makefile -nocache QMAKE_STRIP=: PREFIX=%{_prefix}

# Add here commands to compile the package.
#make %{?jobs:-j%jobs}
#make %{?_smp_mflags}
make

%install
# Add here commands to install the package.
#%qmake_install
make install INSTALL_ROOT=%{buildroot}

%files
%defattr(-,root,root,-)
# %{_prefix}/*
/*
