.. _installation:

Installation
============

BEDOPS is available to users as :ref:`pre-built binaries <installation_via_packages>` and :ref:`source code <installation_via_source_code>`.

.. _installation_via_packages:

======================
Via pre-built packages
======================

Pre-built binaries offer the easiest and fastest installation option for users of BEDOPS. At this time, we offer binaries for 64-bit versions of Linux and OS X (Intel) platforms. 32-bit binaries can be built via source code by adjusting compile-time variables.

-----
Linux
-----

1. Download the current 64-bit package for Linux from `Github BEDOPS Releases <https://github.com/bedops/bedops/releases>`_.
2. Extract the package to a location of your choice. In the case of 64-bit Linux: ::

       $ tar jxvf bedops_linux_x86_64-vx.y.z.tar.bz2

   Replace ``x``, ``y`` and ``z`` with the version number of BEDOPS you have downloaded.
3. Copy the extracted binaries to a location of your choice which is in your environment's ``PATH``, *e.g.* ``/usr/local/bin``: ::

       $ cp bin/* /usr/local/bin

   Change this destination folder, as needed.

--------
Mac OS X
--------

1. Download the current Mac OS X package for BEDOPS from `Github BEDOPS Releases <https://github.com/bedops/bedops/releases>`_.
2. Locate the installer package (usually located in ``~/Downloads`` |--| this will depend on your web browser configuration):

   .. image:: ../assets/installation/bedops_macosx_installer_icon.png

3. Double-click to open the installer package. It will look something like this:

   .. image:: ../assets/installation/bedops_macosx_installer_screen_v2.png
      :width: 99%

4. Follow the instructions to install BEDOPS and library dependencies to your Mac. (If you are upgrading from a previous version, components will be overwritten or removed, as needed.)

.. _installation_via_source_code:

===============
Via source code
===============

.. _installation_via_source_code_on_linux:

-----
Linux
-----

Compilation of BEDOPS on Linux requires GCC 4.8.2 (both ``gcc`` and ``g++`` and related components) or greater, which includes support for `C++11 <http://en.wikipedia.org/wiki/C%2B%2B11>`_ features required by core BEDOPS tools. Other tools may be required as described in the installation documentation that follows.

1. If you do not have GCC 4.8.2 or greater installed (both ``gcc`` and ``g++``), first install these tools. You can check the state of your GCC installation with ``gcc --version`` and ``g++ --version``, *e.g.*: 

   ::

     $ gcc --version
     gcc (GCC) 4.8.2 20140120 (Red Hat 4.8.2-15)
     ...

   If you lack a compiler or have a compiler that is older than 4.8.2, use your favorite package manager to install or upgrade the newer package. For example, in Ubuntu, you might run the following: 

   ::
 
     $ sudo apt-get install gcc-4.8
     $ sudo apt-get install g++-4.8
     $ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
     $ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50

   The specifics of this process will depend on your distribution and what you want to install. Please check with your system administration or support staff if you are unsure what your options are.

   You may also need to install static libraries. For instance, in a CentOS- or RH-like environment:

   ::

     $ sudo yum install libstdc++-static
     $ sudo yum install glibc-static

   In Ubuntu, you might instead do:

   ::

     $ sudo apt-get install libc6-dev
     $ sudo apt-get install build-essentials

2. Install a ``git`` client of your choice, if you do not already have one installed. Github offers an `installation guide <https://help.github.com/articles/set-up-git#platform-all>`_.

   Alternatively, use ``apt-get`` or another package manager to install one, *e.g.* in Ubuntu:

   ::

     $ sudo apt-get install git

   And in CentOS:

   ::

     $ sudo yum install git

3. Clone the BEDOPS Git repository in an appropriate local directory: 

   ::
  
     $ git clone https://github.com/bedops/bedops.git
  
4. Enter the top-level of the local copy of the BEDOPS repository and run ``make`` to begin the build process:

   ::

     $ cd bedops
     $ make

   Running :code:`make` on its own will build so-called "typical" BEDOPS binaries, which make assumptions about line length for most usage scenarios. 

   Use :code:`make megarow` or :code:`make float128` to build support for longer-length rows, or BED data which requires statistical or measurement operations with :ref:`bedmap` with 128-bit precision floating point support.

   If you want all build types, run :code:`make all`.

.. tip:: BEDOPS supports parallel builds, which speeds up compilation considerably. If you are compiling on a multicore or multiprocessor workstation, edit the ``JPARALLEL`` variable in the top-level Makefile, or override it, specifying the number of cores or processors you wish to use to compile.

5. Once the build is complete, install compiled binaries and scripts to a local ``bin`` directory: 

   ::

     $ make install

   If you ran :code:`make megarow` or :code:`make float128`, instead use :code:`make install_megarow` or :code:`make install_float128`, respectively, to install those binaries.

   If you ran :code:`make all`, use :code:`make install_all` to install all binaries of the three types (typical, megarow, and float128) to the :code:`./bin` directory. You can use the :code:`switch-BEDOPS-binary-type` script to switch symbolic links to one of the three binary types.

6. Copy the extracted binaries to a location of your choice that is in your environment's ``PATH``, *e.g.* ``/usr/local/bin``: 

   ::
 
     $ cp bin/* /usr/local/bin

   Change this destination folder, as needed.

.. _installation_via_source_code_on_mac_os_x:

--------
Mac OS X
--------

In Mac OS X, you have a few options to install BEDOPS via source code: Compile the code manually, or use the Bioconda or Homebrew package manager to manage installation.

Compilation of BEDOPS on Mac OS X requires Clang/LLVM 3.5 or greater, which includes support for `C++11 <http://en.wikipedia.org/wiki/C%2B%2B11>`_ features required by core BEDOPS tools. Other tools may be required as described in the installation documentation that follows. GNU GCC is no longer required for compilation on OS X hosts.

^^^^^^^^^^^^^^^^^^
Manual compilation
^^^^^^^^^^^^^^^^^^

1. If you do not have Clang/LLVM 3.5 or greater installed, first do so. You can check this with ``clang -v``, *e.g.*: 

   ::

     $ clang -v
     Apple LLVM version 8.0.0 (clang-800.0.42.1)
     ...

   For Mac OS X users, we recommend installing `Apple Xcode <https://developer.apple.com/xcode/>`_ and its Command Line Tools, via the ``Preferences > Downloads`` option within Xcode. At the time of this writing, Xcode 8.2.1 (8C1002) includes the necessary command-line tools to compile BEDOPS.

2. Install a ``git`` client of your choice, if you do not already have one installed. Github offers an `installation guide <https://help.github.com/articles/set-up-git#platform-all>`_.

3. Clone the BEDOPS Git repository in an appropriate local directory: 

   ::
  
     $ git clone https://github.com/bedops/bedops.git
  
4. Run ``make`` in the top-level of the local copy of the BEDOPS repository:

   ::

     $ cd bedops
     $ make

   Running :code:`make` on its own will build so-called "typical" BEDOPS binaries, which make assumptions about line length for most usage scenarios. 

   Use :code:`make megarow` or :code:`make float128` to build support for longer-length rows, or BED data which requires statistical or measurement operations with :ref:`bedmap` with 128-bit precision floating point support.

   If you want all build types, run :code:`make all`.

.. tip:: BEDOPS supports parallel builds, which speeds up compilation considerably. If you are compiling on a multicore or multiprocessor workstation, edit the ``JPARALLEL`` variable in the top-level Makefile, or override it, specifying the number of cores or processors you wish to use to compile.

5. Once the build is complete, install compiled binaries and scripts to a local ``bin`` folder: 

   ::

     $ make install

   If you ran :code:`make megarow` or :code:`make float128`, instead use :code:`make install_megarow` or :code:`make install_float128`, respectively, to install those binaries.

   If you ran :code:`make all`, use :code:`make install_all` to install all binaries of the three types (typical, megarow, and float128) to the :code:`./bin` directory.

   You can use the :code:`switch-BEDOPS-binary-type` script to switch symbolic links to one of the three binary types.

6. Copy the extracted binaries to a location of your choice that is in your environment's ``PATH``, *e.g.* ``/usr/local/bin``: 

   ::
 
     $ cp bin/* /usr/local/bin

   Change this destination folder, as needed.

^^^^^^^^^^^^^^^^^^^^^^^^^
Installation via Bioconda
^^^^^^^^^^^^^^^^^^^^^^^^^

Bioconda is a bioinformatics resource that extends the Conda package manager with scientific software packages, including BEDOPS. We aim to keep the recipe concurrent with the present release; occasionally, it may be a minor version behind.

What follows are steps taken from the `Bioconda installation page <https://bioconda.github.io/>`_. Use this guide for the most current set of instructions, which we briefly cover here:

1. Follow the instructions on `Conda's website <http://conda.pydata.org/miniconda.html>`_ to install the Miniconda package, which installs the ``conda`` command-line tool.

2. If you have not already done so, add the Conda channels that Bioconda depends upon:

   ::

      $ (conda config --add channels r)
      $ conda config --add channels defaults
      $ conda config --add channels conda-forge
      $ conda config --add channels bioconda

3. Install the BEDOPS package:

   ::

      $ conda install bedops

`Other recipes <https://bioconda.github.io/recipes.html#recipes>`_ are available for installation, as well.
   
^^^^^^^^^^^^^^^^^^^^^^^^^
Installation via Homebrew
^^^^^^^^^^^^^^^^^^^^^^^^^

Homebrew is a popular package management toolkit for Mac OS X. It facilitates easy installation of common scientific and other packages. Homebrew can usually offer a version of BEDOPS concurrent with the present release; occasionally, it may be one or two minor versions behind.

1. If you do not have Clang/LLVM 3.5 or greater installed, first do so. You can check this with ``clang -v``, *e.g.*: 

   ::

     $ clang -v
     Apple LLVM version 8.0.0 (clang-800.0.42.1)
     ...

   For Mac OS X users, we recommend installing `Apple Xcode <https://developer.apple.com/xcode/>`_ and its Command Line Tools, via the ``Preferences > Downloads`` option within Xcode. At the time of this writing, Xcode 8.2.1 (8C1002) includes the necessary command-line tools to compile BEDOPS.

2. Follow the instructions listed on the `Homebrew site <http://brew.sh>`_ to install the basic package manager components.

3. Run the following command:

   ::

     $ brew install bedops

.. _installation_via_source_code_on_docker:

------
Docker
------

`Docker <https://www.docker.com/what-docker>`_ containers wrap up a piece of software (such as BEDOPS) in a complete, self-contained VM.

To set up a CentOS 7-based Docker container with BEDOPS binaries, you can use the following steps:

    ::

       $ git clone https://github.com/bedops/bedops.git
       $ cd bedops
       $ make docker
       ...
       $ docker run -i -t bedops

The following then generates a set of RPMs using the CentOS 7 image, which can run in CentOS 6 and Fedora 21 containers:

    ::

       $ make rpm

Thanks go to Leo Comitale for his efforts here.

.. _installation_via_source_code_on_cygwin:

------
Cygwin
------

1. Make sure you are running a 64-bit version of Cygwin. Compilation of BEDOPS on 32-bit versions of Cygwin is not supported.

   To be sure, open up your Cywin installer application (separate from the Cygwin terminal application) and look for the **64 bit** marker next to the setup application version number: 

   .. image:: ../assets/installation/bedops_cygwin_installer_screen.png
      :width: 99%

   For instance, this Cygwin installer is version 2.831 and is 64-bit.

2. Check that you have GCC 4.8.2 or greater installed. You can check this by opening the Cygwin terminal window (note that this is not the same as the Cygwin installer application) and typing ``gcc --version``, *e.g.*: 

   ::

     $ gcc --version
     gcc (GCC) 4.8.2
     ...

   If you do not have ``gcc`` installed, then open the Cygwin (64-bit) installer application again, navigate through the current setup options, and then mark the GCC 4.8.* packages for installation:

   .. image:: ../assets/installation/bedops_cygwin_installer_gcc_screen.png
      :width: 99%

   If it helps, type in ``gcc`` into the search field to filter results to GCC-related packages. Make sure to mark the following packages for installation, at least:

   * **gcc-core**
   * **gcc-debuginfo**
   * **gcc-g++**
   * **gcc-tools-xyz**
   * **libgcc1**

   Click "Next" to follow directives to install those and any other selected package items. Then run ``gcc --version`` as before, to ensure you have a working GCC setup.

3. Install a ``git`` client of your choice. You can compile one or use the precompiled ``git`` package available through the Cygwin (64-bit) installer:

   .. image:: ../assets/installation/bedops_cygwin_installer_git_screen.png
      :width: 99%

   If it helps, type in ``git`` into the search field to filter results to Git-related packages. Make sure to install the following package, at least:

   * **git**

4. In a Cygwin terminal window, clone the BEDOPS Git repository to an appropriate local directory:

   ::

     $ git clone https://github.com/bedops/bedops.git

4. Enter the top-level of the local copy of the BEDOPS repository and run ``make`` to begin the build process:

   ::

     $ cd bedops
     $ make

.. tip:: BEDOPS now supports parallel builds. If you are compiling on a multicore or multiprocessor workstation, use ``make -j N`` where ``N`` is ``2``, ``4`` or however many cores or processors you have, in order to parallelize and speed up the build process.

5. Once the build is complete, install compiled binaries and scripts to a local ``bin`` folder: 

   ::

     $ make install

6. Copy the extracted binaries to a location of your choice that is in your environment's ``PATH``, *e.g.* ``/usr/bin``: 

   ::
 
     $ cp bin/* /usr/bin

   Change this destination folder, as needed.

.. _installation_os_x_installer_construction:

=====================================================
Building an OS X installer package for redistribution
=====================================================

1. Follow steps 1-3 and step 5 from the :ref:`Via Source Code <installation_via_source_code>` documentation.

2. Run ``make install_osx_packaging_bins`` in the top-level of the local copy of the BEDOPS repository:

   ::

     $ make install_osx_packaging_bins

3. Install `WhiteBox Packages.app <http://s.sudre.free.fr/Software/Packages/about.html>`_, an application for building OS X installers, if not already installed. 

    On 10.13 hosts, it may be necessary to install a more recent development build of ``Packages.app`` via `Packages Q&A #6 <http://s.sudre.free.fr/Software/Packages/Q&A_6.html>`_.

4. Create a ``build`` directory to store the installer and open the ``BEDOPS.pkgproj`` file in the top-level of the local copy of the BEDOPS repository, in order to open the BEDOPS installer project, *e.g.*:

   ::
     
     $ mkdir -p packaging/os_x/build && open packaging/os_x/BEDOPS.pkgproj

   This will open up the installer project with the ``Packages.app`` application.

5. Within ``Packages.app``, modify the project to include the current project version number or other desired changes, as applicable. Make sure the project is set up to build a `"flat"-formatted (xar) <http://s.sudre.free.fr/Stuff/Ivanhoe/FLAT.html>`_ package, not a bundle, otherwise the digital signing step will fail.

6. Run the ``Build > Build`` menu selection to construct the installer package, located in the ``packaging/os_x/build`` subdirectory. Move this installer to the ``/tmp`` directory: 

   ::

     $ mv packaging/os_x/build/BEDOPS\ X.Y.Z.pkg /tmp/BEDOPS.X.Y.Z.unsigned.pkg

7. Find the ``Developer ID Installer`` name that will be used to digitally sign the installer ``pkg`` file, *e.g.*:

   ::

     $ security find-certificate -a -c "Developer ID Installer" | grep "alis"
         "alis"<blob>="Developer ID Installer: Foo B. Baz (ABCD12345678)"

   Here, the name is ``Developer ID Installer: Foo B. Baz``. 

   (This certificate name is unique to the developer. If necessary, you may need to sign up for a `Mac Developer Program <https://developer.apple.com/programs/mac/>`_ account with Apple to set up `required certificates <https://developer.apple.com/library/mac/documentation/IDEs/Conceptual/AppDistributionGuide/DistributingApplicationsOutside/DistributingApplicationsOutside.html>`_.)

8. Sign the package installer, *e.g.*:

   ::

     $ productsign --timestamp --sign "Developer ID Installer: Foo B. Baz" /tmp/BEDOPS.X.Y.Z.unsigned.pkg /tmp/BEDOPS.X.Y.Z.signed.pkg

9. Compress the signed ``pkg`` file (via OS X zip, for instance) and publish via GitHub releases (see :ref:`release preparation <release>` for information about publishing the installer).

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
