IMAGE_INSTALL = "packagegroup-core-boot \
                 ${CORE_IMAGE_EXTRA_INSTALL} \
                 kernel-modules \
                 python3 \
                 "

IMAGE_LINGUAS = " "

LICENSE = "MIT"

VIRTUAL-RUNTIME_init_manager = "sysvinit"
VIRTUAL-RUNTIME_initscripts = "initscripts"

inherit core-image

IMAGE_ROOTFS_SIZE ?= "8192"
