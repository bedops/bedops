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

   b. If any issues can't be closed out, rename the assigned version tag to the next anticipated release version (*e.g.*, *v2.4.39* to *v2p5p0*, etc.)

2. Pull the most recent commit for the development branch to a local folder on build hosts (Linux with sufficiently old kernel, current OS X, etc.).

   a. Follow the :ref:`Installation (via source code) <installation_via_source_code>` documentation to build BEDOPS for the given platform. 

      1) For Linux, we build a 64-bit version. It may help to use `VirtualBox <https://www.virtualbox.org>`_ or a similar virtualization host to set up and run different (and consistent) versions of Linux build hosts.

      2) For Mac OS X, we currently build the Mac target with whatever the modern Xcode and current OS X release happens to be (currently, command-line tools that ship with Xcode 9 and OS X High Sierra/10.13). If things work correctly, build flags generate 64-bit binaries that should run on 10.10 and newer OS releases.

   b. For all platforms, run test suites for various tools and conversion scripts; tests should pass on supported platforms. If not, add an Issue ticket, fix it, close it and start over with the build/test process.

   c. If things work properly, make a bzip2-compressed tarball from the compiled binaries. 

   The naming scheme we currently use for Linux packages is as follows:

   ::

     bedops_linux_x86_64-vX.Y.Z.tar.bz2 (64-bit)

   Run ``shasum -a 256`` on the tarball to get its SHA256 hash (store this SHA256 hash in a file for later retrieval).

   For the OS X Installer, use ``productsign`` per :ref:`OS X Installer <installation_os_x_installer_construction>` documentation to digitally sign the package. Compress the Installer with the Finder or `zip`:

   ::

     BEDOPS.X.Y.Z.pkg.zip

   The *X.Y.Z* scheme should follow the development branch name, *e.g.* 2.4.39, etc.

3. Collect tarballs and zipped Installer in one location for later addition with web browser, via BEDOPS Github web site.

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

   As before, the *X.Y.Z* scheme should follow the development branch name, *e.g.* 2.4.39, etc.

2. Add a `new release <https://github.com/bedops/bedops/releases/new>`_ via the Github site. Or click on the `Draft a new release <https://github.com/bedops/bedops/releases>`_ button from the Github Releases page.

   Fill out the resulting form, as described below:

   a. *Tag version* should be of the form *vX.Y.Z* (using the "semantic versioning" naming scheme triggers Github to set up useful and automatic package features). 

   Tags should be applied to the *master* branch, since we pushed the development branch up to the master branch.

   b. *Release title* can be of the form *BEDOPS vX.Y.Z*.

   c. *Describe this release* can be populated with the following Markdown-formatted boilerplate:

   ::

     Downloads are available at the bottom of this page. Please read the [BEDOPS vX.Y.Z revision history](http://bedops.readthedocs.io/en/latest/content/revision-history.html#vX-Y-Z), which summarizes new features and fixes in this release.

     ------

     ### Linux
     **bedops_linux_x86_64-vX.Y.Z.tar.bz2** (64-bit, SHA256: ``abcd1234``)
     This package of BEDOPS vX.Y.Z binaries is for Linux 64-bit (glibc v2.17) hosts. Those who require 32-bit or pre-2.17 glibc-compiled binaries will need to build binaries from source code; please read [§2.2. Via source-code] (http://bedops.readthedocs.io/en/latest/content/installation.html#via-source-code) of the BEDOPS Installation document for details.

     For installation instructions, please read [§2.1.1. Linux] (http://bedops.readthedocs.io/en/latest/content/installation.html#linux) of the BEDOPS Installation document.

     ------

     ### Mac OS X
     **BEDOPS.X.Y.Z.pkg.zip**
     This package of BEDOPS vX.Y.Z is a digitally-signed installer for 64-bit binaries that run under OS X (10.10 - 10.13) on Intel-based Macs.

     For installation instructions, please read [§2.1.2. Mac OS X] (http://bedops.readthedocs.io/en/latest/content/installation.html#mac-os-x) of the BEDOPS Installation document.

   d. Attach per-platform binaries to this release by dragging each of them into the field underneath the description text. It can take a few moments for the web browser to upload each binary into the release page, so be patient. There should be at least two binary packages: one for Linux 64-bit, and one for Mac OS X.

   e. Click the *Publish Release* button.

3. After at least 5-10 minutes from pushing the development branch to the master branch, check the `BEDOPS documentation site <http://bedops.readthedocs.io/en/latest/>`_ to ensure that the "latest" or default documenation shown is for the new version. 

   If not, take a look at the `build <https://readthedocs.org/builds/bedops/>`_ page to manually trigger document rebuilds, or examine error logs, if necessary.

4. Update the Github bedops/bedops master `README.md <https://github.com/bedops/bedops/blob/v2.4.39/README.md>`_ file to note the current version number, if necessary.

5. Push fixes to any documentation errors in the master branch. 

.. note:: We should aim to fix typos and other errors as soon after a new release as possible, because then shortly afterwards we can simply pull a new development branch off the current state of the master branch with minimal commit losses.

.. tip:: If we push any subsequent changes to the ``master`` branch, it's not the end of the world. However, it is recommended that the version tag is pushed forwards to the latest commit:

   ::

      $ git tag -f -a vX.Y.Z -m 'pushed current version tag forwards to latest commit'
      ...
      $ git push -f --tags
      ...

   This way, anyone who downloads source via GitHub will get the "freshest" code, with all the typo fixes and so forth.

6. Visit the `BEDOPS documentation administration site <https://readthedocs.org/dashboard/bedops/edit/>`_ to disable documentation for the development branch. 

   Specifically, click on the `versions <https://readthedocs.org/dashboard/bedops/versions/>`_ tab to deactivate the old development branch. (Likewise, when adding a new development branch, add an active link here, so that edits to the documentation folder in the new development branch are available.)

7. Update a local fork of `homebrew-science <https://github.com/Homebrew/homebrew-science>`_ with details for the BEDOPS `formula <https://github.com/Homebrew/homebrew-science/blob/master/bedops.rb>`_. Submit pull request to homebrew-science folks.

   a. After establishing a local fork, add the upstream remote so that you can fetch/pull updated formulas from Homebrew (if this is already done, this step can be skipped):

   ::

      $ git remote add upstream git://github.com/homebrew/homebrew-science.git

   b. Fetch and pull data to the master branch from the upstream remote:

   ::

      $ git checkout master
      $ git fetch
      $ git pull upstream master
      ...

   c. Make a branch of the master entitled *bedops-vXpYpZ* and check it out:

   ::

      $ git branch bedops-vXpYpZ
      $ git checkout bedops-vXpYpZ

   d. Edit changes to *bedops.rb* formula. Change the version number in the tarball download and remove the ``sha1`` line (you'll replace this later on).

   e. Test the new formula. Add the ``--build-from-source`` option to skip the per-platform bottle code:

   ::

      $ brew install ./bedops.rb --build-from-source

   f. If the installation is successful, there will be a SHA1 validation code that you can copy and paste into the formula with the ``sha1`` header (see step *d* |---| basically, you are updating the line you removed in that step).

   g. Add, commit and push the updated formula to the *bedops-vXpYpZ* branch:

   ::

      $ git add bedops.rb
      $ git commit -am 'BEDOPS X.Y.Z'
      $ git push origin bedops-vXpYpZ

   h. Visit the `homebrew-science <https://github.com/Homebrew/homebrew-science>`_ site and initiate a pull request from your local fork's newly pushed branch (there will be a big green button at the top of the GitHub site that asks you to start this pull request).

   i. Wait for success or failure; the homebrew-science people will indicate if there are any problems, usually within 48-72 hours.

8. Consider closing out or deleting the development branch, as well as setting up the next development branch.

=========
Celebrate
=========

At this point, we can email links to Linux packages to IT for updating the cluster BEDOPS module and make announcements on websites, mailing lists, etc.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:
