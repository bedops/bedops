.. _installation:

Installation
============

BEDOPS is available to users as :ref:`pre-built binaries <installation_via_packages>` and :ref:`source code <installation_via_source_code>`.

.. _installation_via_packages:

======================
Via pre-built packages
======================

Pre-built binaries offer the easiest and fastest installation option for users of BEDOPS. At this time, we offer binaries for 32- and 64-bit versions of Linux and OS X (Intel) platforms.

-----
Linux
-----

1. Download the current 32- or 64-bit package for Linux from `Github BEDOPS Releases <https://github.com/bedops/bedops/releases>`_.
2. Extract the package to a location of your choice. 
   In the case of 32-bit Linux: ::

       $ tar jxvf bedops_linux_i386-vx.y.z.tar.bz2

   In the case of 64-bit Linux: ::

       $ tar jxvf bedops_linux_x86_64-vx.y.z.tar.bz2

   Replace ``x``, ``y`` and ``z`` with the version number of BEDOPS you have downloaded.
3. Copy the extracted binaries to a location of your choice which is in your environment's ``PATH``, *e.g.* ``~/opt/bin``: ::

       $ cp bin/* ~/opt/bin

   Change this destination folder, as needed.

--------
Mac OS X
--------

1. Download the current Mac OS X package for BEDOPS from `Github BEDOPS Releases <https://github.com/bedops/bedops/releases>`_.
2. Locate the installer package (usually located in ``~/Downloads`` |--| this will depend on your web browser configuration):

.. image:: ../assets/installation/bedops_macosx_installer_icon.png

3. Double-click to open the installer package. It will look something like this:

.. image:: ../assets/installation/bedops_macosx_installer_screen.png

4. Follow the instructions to install BEDOPS and library dependencies to your Mac. (If you are upgrading from a previous version, components will be overwritten or removed, as needed.)

.. _installation_via_source_code:

===============
Via source code
===============

At this time, compilation of BEDOPS requires GCC 4.7 or greater (which includes support for `C++11 <http://en.wikipedia.org/wiki/C%2B%2B11>`_ features required by core BEDOPS tools).

1. If you do not have GCC 4.7 or greater installed, first do so. You can check this with ``gcc --version``, *e.g.*: 

   ::

     $ gcc --version
     gcc (MacPorts gcc48 4.8.2_0+universal) 4.8.2
     ...

   For Mac OS X users, we recommend first installing `Apple Xcode <https://developer.apple.com/xcode/>`_ and its Command Line Tools, via the ``Preferences > Downloads`` option within Xcode. Then install GCC 4.7 or greater using `MacPorts <http://www.macports.org>`_, setting GCC to be the default compiler, *e.g.*: 
 
   :: 

     $ sudo port install gcc48 +universal
     $ sudo port install gcc_select
     $ sudo port select --list gcc
     ...
     $ sudo port select --set gcc mp-gcc48

   In the future, we may provide full support for OS X compilation via Clang/LLVM, which is the default compiler included with Xcode.

   For Linux users, use your favorite package manager to install the requisite compiler. For example, in Ubuntu, you might run the following: 

   ::
 
     $ sudo apt-get install gcc

2. Install a ``git`` client of your choice, if you do not already have one installed. Github offers an `installation guide <https://help.github.com/articles/set-up-git#platform-all>`_.

3. Clone the BEDOPS Git repository in an appropriate local directory: 

   ::
  
     $ git clone https://github.com/bedops/bedops.git
  
4. (Linux only) Run ``make static`` in the top-level of the local copy of the BEDOPS repository: 

   ::

     $ cd bedops
     $ make static

5. (Mac OS X only) Run ``make build_all_darwin_intel_fat`` in the top-level of the local copy of the BEDOPS repository:

   ::

     $ cd bedops
     $ make build_all_darwin_intel_fat

6. Install compiled binaries and scripts to a local ``bin`` folder: 

   ::

     $ make install

7. Copy the extracted binaries to a location of your choice that is in your environment's ``PATH``, *e.g.* ``~/opt/bin``: 

   ::
 
     $ cp bin/* ~/opt/bin

   Change this destination folder, as needed.

=====================================================
Building an OS X installer package for redistribution
=====================================================

1. Follow steps 1-3 and step 5 from the :ref:`Via Source Code <installation_via_source_code>` documentation.

2. Run ``make install_osx_packaging_bins`` in the top-level of the local copy of the BEDOPS repository:

   ::

     $ make install_osx_packaging_bins

3. Install `WhiteBox Packages.app <http://s.sudre.free.fr/Software/Packages/about.html>`_, an application for building OS X installers, if not already installed.

4. Create a ``build`` directory to store the installer and open the ``BEDOPS.pkgproj`` file in the top-level of the local copy of the BEDOPS repository, in order to open the BEDOPS installer project, *e.g.*:

   ::
     
     $ mkdir packaging/os_x/build && open packaging/os_x/BEDOPS.pkgproj

   This will open up the installer project with the ``Packages.app`` application.

5. Within ``Packages.app``, modify the project to include the current project version number or other desired changes, as applicable.

6. Run the ``Build > Build`` menu selection to construct the installer package, located in the ``packaging/os_x/build`` subdirectory. Move this installer to the desired location with ``mv`` or the OS X Finder.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
