# Packaging for ZFS on Linux

The home for this git repository is:

* https://github.com/zfsonlinux/pkg-zfs

ZoL packages at https://launchpad.net/~zfs-native (the PPA) and
http://archive.zfsonlinux.org/debian/ are built from this repository using the
git-buildpackage tool.


## Casual Build Instructions

If you are using APT to install ZoL, then just do quick builds like this:
```
$ apt-get source --build zfs-linux
```
This requires a corresponding `deb-src` line for each `deb` line in the
`/etc/apt/sources.list.d` file for ZoL.


## Developer Build Instructions

1. Clone this repository:
```
$ git clone git://github.com/zfsonlinux/pkg-zfs.git
$ cd pkg-zfs
```

1. List the current releases by branch name:
```
$ git branch --list 'master/*'
```

1. Or list previous releases by tag name:
```
$ git tag --list 'master/*'
```

1. Checkout the branch name or tag name that you want to build.  For example,
the latest code for Ubuntu 12.04 Precise Pangolin is:
```
$ git checkout master/ubuntu/precise
```

1. Now compile it:
```
$ ./autogen.sh
$ git-buildpackage -uc -us
```

1. And clean the working tree afterwards by doing this:
```
$ git clean -df
$ git reset --hard
```

# Release Instructions

1. Build a binary+source release like this:
```
$ git-buildpackage --git-tag [-sa|-sd]
```
(The `-sa` switch means "upload a new upstream tarball" for an out-series
build. The `-sd` switch means "only upload the new overlay" for an in-series
builds.)

1. Synchronize the release bucket to your working copy.
```
$ s3cmd sync --dry-run s3://archive.zfsonlinux.org/ ./archive.zfsonlinux.org/
$ s3cmd sync s3://archive.zfsonlinux.org/ ./archive.zfsonlinux.org/
```

1. Update the release bucket like this:
```
$ cd ./archive/zfsonlinux.org/debian/
$ reprepro include wheezy /tmp/zfs-linux_${version}_amd64.changes
```

1. Do a local installation in a clean sandbox to ensure that the Release and
Sources are sensible.

1. Give notice that you're touching the release bucket, and synchronize the new
packages:
```
$ s3cmd sync --dry-run ./archive.zfsonlinux.org/ s3://archive.zfsonlinux.org/
$ s3cmd sync ./archive.zfsonlinux.org/ s3://archive.zfsonlinux.org/
```

(Ideally, you would sync the pool first, and then sync the meta to ensure the
smallest possible window of inconsistency.)


## Upstream Repositories

The `upstream` branch in this repository is an unmodified copy of the
http://github.com/zfsonlinux/zfs.git mainline.
