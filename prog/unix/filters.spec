Name:						filters
%define version 1.2
%define _bindir /usr/bin
Version:				%{version}
Release:				0
Summary:				Program Development Tools
Group:					Applications Development
License:				GPL 3.0 License
URL:						.
Vendor:					.
Source:					filters-%{version}.tar.gz
Prefix:					/usr
Packager:				questaware
BuildRoot:			%{_tmppath}/%{name}-root

%description
Various tools including
	+ ffg combines find and grep
	+ chrad

%prep
%setup -cq

%build
echo Hello
pwd
cd ./prog/unix
make

%install
mkdir -p %{buildroot}%{_bindir}
install -m 755 prog/unix/chrad		%{buildroot}%{_bindir}
install -m 755 prog/unix/ffg			%{buildroot}%{_bindir}

%clean

%files
%defattr(-,root,root)
%{_bindir}/chrad
%{_bindir}/ffg

%changelog

