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

At this time, compilation of BEDOPS requires GCC (both ``gcc`` and ``g++``) 4.8 or greater, which includes support for `C++11 <http://en.wikipedia.org/wiki/C%2B%2B11>`_ features required by core BEDOPS tools. Other tools may be required, some which are platform-specific, as described in the installation documentation that follows.

.. _installation_via_source_code_on_linux

-----
Linux
-----

1. If you do not have GCC 4.8 or greater installed, first install these tools. You can check this with ``gcc --version``, *e.g.*: 

   ::

     $ gcc --version
     gcc (GCC) 4.8.0 20130127 (experimental)

   For Linux users, use your favorite package manager to install the requisite compiler. For example, in Ubuntu, you might run the following: 

   ::
 
     $ sudo apt-get install gcc-4.8
     $ sudo apt-get install g++-4.8

   If you already have ``gcc`` and need to update it, you might run something like:

   ::

     $ sudo apt-get update
     $ sudo apt-get upgrade -y
     $ sudo apt-get dist-upgrade

   The specifics will depend on your distribution and what you want to install. You can check with your system administration or support staff if you are unsure what your options are.

2. Install a ``git`` client of your choice, if you do not already have one installed. Github offers an `installation guide <https://help.github.com/articles/set-up-git#platform-all>`_.

3. Clone the BEDOPS Git repository in an appropriate local directory: 

   ::
  
     $ git clone https://github.com/bedops/bedops.git
  
4. Enter the top-level of the local copy of the BEDOPS repository and run ``make static`` to begin the build process:

   ::

     $ cd bedops
     $ make static

5. Once the build is complete, install compiled binaries and scripts to a local ``bin`` folder: 

   ::

     $ make install

6. Copy the extracted binaries to a location of your choice that is in your environment's ``PATH``, *e.g.* ``~/opt/bin``: 

   ::
 
     $ cp bin/* ~/opt/bin

   Change this destination folder, as needed.

.. _installation_via_source_code_on_mac_os_x

--------
Mac OS X
--------

1. If you do not have GCC 4.8 or greater installed, first do so. You can check this with ``gcc --version``, *e.g.*: 

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


2. Install a ``git`` client of your choice, if you do not already have one installed. Github offers an `installation guide <https://help.github.com/articles/set-up-git#platform-all>`_.

   Alternatively, use ``apt-get`` to install one, *e.g.*

   ::

     $ sudo apt-get install git

3. Clone the BEDOPS Git repository in an appropriate local directory: 

   ::
  
     $ git clone https://github.com/bedops/bedops.git
  
4. Run ``make build_all_darwin_intel_fat`` in the top-level of the local copy of the BEDOPS repository:

   ::

     $ cd bedops
     $ make build_all_darwin_intel_fat

5. Once the build is complete, install compiled binaries and scripts to a local ``bin`` folder: 

   ::

     $ make install

6. Copy the extracted binaries to a location of your choice that is in your environment's ``PATH``, *e.g.* ``~/opt/bin``: 

   ::
 
     $ cp bin/* ~/opt/bin

   Change this destination folder, as needed.

.. _installation_via_source_code_on_cygwin

------
Cygwin
------

1. Make sure you are running a 64-bit version of Cygwin. Compilation of BEDOPS on 32-bit versions of Cygwin is not supported.

   To be sure, open up your Cywin installer application (separate from the Cygwin terminal application) and look for the **64 bit** marker next to the setup application version number. For instance, here is a screenshot of the Cygwin installer that is version 2.831 and is 64-bit:

   .. image:: ../assets/installation/bedops_cygwin_installer_screen.png

2. Check that you have GCC 4.8 or greater installed. You can check this by opening the Cygwin terminal window (note that this is not the same as the Cygwin installer application) and typing ``gcc --version``, *e.g.*: 

   ::

     $ gcc --version
     gcc (GCC) 4.8.2
     ...

   If you do not have ``gcc`` installed, then open the Cygwin (64-bit) installer application again, navigate through the current setup options, and then mark the GCC 4.8.* packages for installation:

   .. image:: ../assets/installation/bedops_cygwin_installer_gcc_screen.png

   If it helps, type in ``gcc`` into the search field to filter results to GCC-related packages. Make sure to mark the following packages for installation, at least:

   * **gcc-core**
   * **gcc-debuginfo**
   * **gcc-g++**
   * **gcc-tools-xyz**
   * **libgcc1**

   Click "Next" to follow directives to install those and any other selected package items. Then run ``gcc --version`` as before, to ensure you have a working GCC setup.

3. Install a ``git`` client of your choice. You can compile one or use the precompiled ``git`` package available through the Cygwin (64-bit) installer:

   .. image:: ../assets/installation/bedops_cygwin_installer_git_screen.png

   If it helps, type in ``git`` into the search field to filter results to Git-related packages. Make sure to install the following package, at least:

   * **git**

4. In a Cygwin terminal window, clone the BEDOPS Git repository to an appropriate local directory:

   ::

     $ git clone https://github.com/bedops/bedops.git

4. Enter the top-level of the local copy of the BEDOPS repository and run ``make static`` to begin the build process:

   ::

     $ cd bedops
     $ make static

5. Once the build is complete, install compiled binaries and scripts to a local ``bin`` folder: 

   ::

     $ make install

6. Copy the extracted binaries to a location of your choice that is in your environment's ``PATH``, *e.g.* ``/usr/bin``: 

   ::
 
     $ cp bin/* /usr/bin

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
