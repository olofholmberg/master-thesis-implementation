Usage
=====
Run `./build.sh` to
1. Build a Docker container with Yocto dependencies
1. Clone the Yocto top level git repo ("poky") into the `work` folder - this is were the build will take place
1. Initialize the build environment in the work folder
1. Copy Yocto config files and the meta-sel4 layer into the work folder
1. Run the build inside the previously built container

Output
======
After completed build, artefacts can be found in `work/build/tmp/deploy/images/qemuarm64/`:
* Kernel image: `Image`
* Root filesystem: `vm-qemuarm64.cpio.gz`

Updating
========
To add packages to the final image, edit `meta-sel4/recipes-core/images/vm.bb`.
