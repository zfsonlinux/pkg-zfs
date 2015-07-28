#!/bin/sh

version="$1"

cat <<EOF > debian/changelog
$version

  * New upstream GIT version.

 -- Turbo Fredriksson <turbo@debian.org>  `date -R`

EOF
cat debian/changelog.in >> debian/changelog

cp debian/changelog debian/changelog.in
git commit -m 'New upstream GIT version.' debian/changelog.in
