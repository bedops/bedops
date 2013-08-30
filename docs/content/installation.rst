Installation
============

BEDOPS is available to users as pre-built binaries and source code.

======================
Via pre-built packages
======================

Pre-built binaries offer the easiest installation option for users of BEDOPS. At this time, we offer binaries for 32- and 64-bit versions of Linux and OS X (Intel) platforms.

-----
Linux
-----

1. Download the current 32- or 64-bit package for Linux from Github.
2. Extract the package to a location of your choice. 
   In the case of 32-bit Linux:
   ::
       $ tar jxvf bedops_linux_i386-vx.y.z.tar.bz2
   In the case of 64-bit Linux:
   ::
       $ tar jxvf bedops_linux_x86_64-vx.y.z.tar.bz2
   Replace ``x``, ``y`` and ``z`` with the version number of BEDOPS you have downloaded.
3. Copy the extracted binaries to a location of your choice which is in your environment's ``PATH``, *e.g.* ``~/opt/bin``:
   ::
       $ cp bin/* ~/opt/bin
   Change this destination folder, as needed.

--------
Mac OS X
--------

1. Download the current Mac OS X package for BEDOPS from Github.
2. Locate the installer package (usually located in ``~/Downloads`` |--| this will depend on your web browser configuration):

.. image:: ../assets/installation/bedops_macosx_installer_icon.png

3. Double-click to open the installer package. It will look something like this:

.. image:: ../assets/installation/bedops_macosx_installer_screen.png

4. Follow the instructions to install BEDOPS and library dependencies to your Mac. (If you are upgrading from a previous version, components will be overwritten or removed, as needed.)

===============
Via source code
===============

At this time, compilation of BEDOPS requires GCC 4.7 or greater (which includes support for `C++11 <http://en.wikipedia.org/wiki/C%2B%2B11>`_ features required by core BEDOPS tools).

1. If you do not have GCC 4.7 or greater installed, first do so.

   For Mac OS X users, we recommend first installing `Apple Xcode <https://developer.apple.com/xcode/>`_ and its Command Line Tools, via the ``Preferences > Downloads`` option within Xcode. Then install GCC 4.7 or greater using `MacPorts <http://www.macports.org>`_. In the future, we may provide support for OS X compilation via Clang/LLVM, which is the default compiler included with Xcode.

2. Clone the BEDOPS Git repository in an appropriate local directory: ::

   $ git clone https://github.com/alexpreynolds/bedops.git

3. Run ``make`` in the top-level of the local copy of the BEDOPS repository: ::

   $ cd bedops
   $ make

4. Install compiled binaries and scripts to a local ``bin`` folder: ::

   $ make install

5. Copy the extracted binaries to a location of your choice that is in your environment's ``PATH``, *e.g.* ``~/opt/bin``: ::
 
   $ cp bin/* ~/opt/bin

   Change this destination folder, as needed.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
