Name:           devclean
Version:        1.0.1
Release:        1%{?dist}
Summary:        Developer cache inspection and cleanup utility
License:        MIT
URL:            https://github.com/ChinmayBansal010/devclean

%description
DevClean discovers common developer caches, reports their size and activity,
and provides guarded cleanup workflows.

%files
%{_bindir}/devclean
%{_mandir}/man1/devclean.1*

%changelog
* Sun Aug 16 2026 Chinmay Bansal <chinmay8521@gmail.com> - 1.0.1-1
- Initial package metadata
