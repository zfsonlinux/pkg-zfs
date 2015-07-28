#!/bin/sh

version="$1"

if [ -z "${version}" ]; then
    echo "Usage: $0 <version_string>"
    exit 1
fi

if [ -z "${EMAIL}" -o -z "${FULLNAME}" ]; then
    echo "Missing EMAIL and/or FULLNAME env variable!"
    exit 1
fi

cat <<EOF > debian/changelog
$version

  * New upstream GIT version.

 -- ${FULLNAME} <${EMAIL}>  `date -R`

EOF
cat debian/changelog.in >> debian/changelog
