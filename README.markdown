The Solaris Porting Layer (SPL) for Debian and Ubuntu. Build packages
from this source repository by running:

  $ dpkg-buildpackage -us -uc

The SPL comes with an automated test suite called SPLAT.  The test suite
is implemented in two parts.  There is a kernel module which contains
the tests and a user space utility which controls which tests are run.
To run the full test suite:

    $ sudo insmod ./module/splat/splat.ko
    $ sudo ./cmd/splat --all

Full documentation for building, configuring, testing, and using the
SPL can be found at: <http://zfsonlinux.org>
