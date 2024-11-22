Summary: Driver for the SerialFC family of boards
Name: serialfc
Version: 2.3.2
Release: 1
License: GPL
Group: System Environment/Kernel
URL: https://github.com/commtech/serialfc-linux
Source0: serialfc-linux.tar
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires: gcc
Requires(post): dkms
Requires(preun): dkms
BuildArch: noarch

%description
Driver for the SerialFC family of boards

%prep
tar -xf ../SOURCES/serialfc-linux.tar

%install
%{__rm} -rf %{buildroot}
%define dkms_name %{name}
%define dkms_vers %{version}
# Kernel module sources install for dkms
%{__mkdir_p} %{buildroot}%{_usrsrc}/%{dkms_name}-%{dkms_vers}/
%{__cp} -a * %{buildroot}%{_usrsrc}/%{dkms_name}-%{dkms_vers}/
# Adding udev rules
%{__mkdir_p} %{buildroot}%{_sysconfdir}/udev/rules.d
%{__cp} serialfc.rules %{buildroot}%{_sysconfdir}/udev/rules.d

%clean
%{__rm} -rf %{buildroot}

%post
%define dkms_name %{name}
%define dkms_vers %{version}
# Add to DKMS registry
dkms add -m %{dkms_name} -v %{dkms_vers} || :
# Rebuild and make available for the currenty running kernel
dkms build -m %{dkms_name} -v %{dkms_vers}  || :
dkms install -m %{dkms_name} -v %{dkms_vers} || :

%preun
%define dkms_name %{name}
%define dkms_vers %{version}
# Remove all versions from DKMS registry
dkms remove -m %{dkms_name} -v %{dkms_vers} --all || :

%files
%defattr(-,root,root,-)
%{_usrsrc}/%{dkms_name}-%{dkms_vers}/
%{_sysconfdir}/udev/rules.d/
%doc

%changelog
* Tue Jun 11 2019 u-test <u-test@localhost.localdomain> - dkms-1
- Initial build.

