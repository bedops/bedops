.. _release:

Release
=======

This document attempts to enumerate steps to get from a development branch to a final release, with all associated packages and documentation changes.

===========
Preparation
===========

Preparing a major, minor or maintenance release of BEDOPS from a development branch involves several steps, which we outline here:

1. Review the `Github issues list <https://github.com/bedops/bedops/issues>`_

   a. Close out open documentation or feature issues, making necessary pushes to the current development branch.

   b. If any issues can't be closed out, rename the assigned version tag to the next release version (*e.g.*, *v2p4p3* to *v2p5p0*, etc.)

2. Pull the most recent commit for the development branch to a local folder on build hosts (Linux with sufficiently old kernel, current OS X, etc.).

   a. Follow the :ref:`Installation (via source code) <installation_via_source_code>` documentation to build BEDOPS for the given platform. 

      1) For Linux, we build two versions, one 64-bit and one 32-bit. It may help to use `VirtualBox <https://www.virtualbox.org>`_ or a similar virtualization host to set up and run different (and consistent) versions of Linux build hosts.

      2) For Mac OS X, we currently build the Mac target with whatever the modern Xcode and current OS X release happens to be (currently, command-line tools that ship with Xcode 6 and OS X Yosemite/10.10). If things work correctly, build flags generate "fat" binaries that should run on 10.7 and newer OS releases.

   b. For all platforms, run test suites for various tools and conversion scripts; tests should pass on supported platforms. If not, add an Issue ticket, fix it, close it and start over with the build/test process.

   c. If things work properly, make a bzip2-compressed tarball from the compiled binaries. 

   The naming scheme we currently use for Linux packages is as follows:

   ::

     bedops_linux_x86_64-vX.Y.Z.tar.bz2 (64-bit)
     bedops_linux_i386-vX.Y.Z.tar.bz2 (32-bit)

   For Mac OS X, we build a Zip-compressed :ref:`OS X Installer bundle <installation_os_x_installer_construction>` with the following name scheme:

   ::

     BEDOPS.X.Y.Z.mpkg.zip

   The *X.Y.Z* scheme should follow the development branch name, *e.g.* 2.4.3, etc.

3. Collect tarballs for all platforms in one location for later addition via Github site.

=======
Release
=======

1. Merge BEDOPS development branch into master branch:

   ::

     $ git checkout master
     $ git pull origin master
     $ git merge vXpYpZ
     $ git push origin master

   Ideally, whatever steps are used to merge the development branch into the master branch should preserve the overall commit history.

   As before, the *X.Y.Z* scheme should follow the development branch name, *e.g.* 2.4.3, etc.

2. Add a `new release <https://github.com/bedops/bedops/releases/new>`_ via the Github site. Or click on the `Draft a new release <https://github.com/bedops/bedops/releases>`_ button from the Github Releases page.

   Fill out the resulting form, as described below:

   a. *Tag version* should be of the form *vX.Y.Z* (using the "semantic versioning" naming scheme triggers Github to set up useful and automatic package features). 

   Tags should be applied to the *master* branch, since we pushed the development branch up to the master branch.

   b. *Release title* can be of the form *BEDOPS vX.Y.Z*.

   c. *Describe this release* can be populated with the following Markdown-formatted boilerplate:

   ::

     Downloads are available at the bottom of this page. Please read the [BEDOPS vX.Y.Z revision history](http://bedops.readthedocs.org/en/latest/content/revision-history.html#vX-Y-Z), which summarizes new features and fixes in this release.

     ------

     ### Linux
     **bedops_linux_x86_64-vX.Y.Z.tar.bz2** (64-bit)
     **bedops_linux_i386-vX.Y.Z.tar.bz2** (32-bit)
     This package of BEDOPS vX.Y.Z binaries is for Linux 64- and 32-bit hosts. Pick the installer that matches your host architecture. If your host can run 64-bit binaries, we recommend downloading the 64-bit package.

     For installation instructions, please read [§2.1.1. Linux] (http://bedops.readthedocs.org/en/latest/content/installation.html#linux) of the BEDOPS Installation document.

     ------

     ### Mac OS X
     **BEDOPS.X.Y.Z.mpkg.zip**
     This package of BEDOPS vX.Y.Z binaries is an installer for OS X (10.7 - 10.10) running on Intel-based Macs.

     For installation instructions, please read [§2.1.2. Mac OS X] (http://bedops.readthedocs.org/en/latest/content/installation.html#mac-os-x) of the BEDOPS Installation document.

   d. Attach per-platform binaries to this release by dragging each of them into the field underneath the description text. It can take a few moments for the web browser to upload each binary into the release page, so be patient. There should be at least three binaries: two for Linux 64- and 32-bit, and one for (fat) Mac OS X.

   e. Click the *Publish Release* button.

3. After at least 5-10 minutes from pushing the development branch to the master branch, check the `BEDOPS documentation site <http://bedops.readthedocs.org/en/latest/>`_ to ensure that the "latest" or default documenation shown is for the new version. 

   If not, take a look at the `build <https://readthedocs.org/builds/bedops/>`_ page to manually trigger document rebuilds, or examine error logs, as necessary.

4. Push fixes to any documentation errors in the master branch. 

.. note:: We should aim to fix typos and other errors as soon after a new release as possible, because then shortly afterwards we can simply pull a new development branch off the current state of the master branch with minimal commit losses.

5. Visit the `BEDOPS documentation administration site <https://readthedocs.org/dashboard/bedops/edit/>`_ to disable documentation for the development branch. 

   Click on the `versions <https://readthedocs.org/dashboard/bedops/versions/>`_ tab to deactivate the old development branch. (Likewise, when adding a new development branch, add a active link here so that edits to documentation for the new development branch are available.)

=========
Celebrate
=========

At this point, we can email links to Linux packages to IT for updating the cluster BEDOPS module and make announcements on websites, mailing lists, etc.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
