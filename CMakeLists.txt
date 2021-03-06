#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

#
# Modified by Olof Holmberg, 2021
#
# For licensing information see comment at the top of the file.
#

cmake_minimum_required(VERSION 3.16.3)
include(settings.cmake)

project(masterthesis C CXX ASM)

find_package(camkes-arm-vm REQUIRED)
camkes_arm_vm_setup_arm_vm_environment()
include(${CAMKES_ARM_VM_HELPERS_PATH})

find_package(camkes-vm-linux REQUIRED)
include(${CAMKES_VM_LINUX_HELPERS_PATH})
include(${CAMKES_VM_LINUX_MODULE_HELPERS_PATH})
include(${CAMKES_VM_LINUX_SOURCE_HELPERS_PATH})

find_package(master-thesis-vm-images REQUIRED)

include(ExternalProject)
include(external-project-helpers)

# Create our CPP Flags based on ARM VM config variables
if("${PLATFORM}" STREQUAL "qemu-arm-virt")
    set(cpp_flags "-DKERNELARMPLATFORM_QEMU-ARM-VIRT")
    set(linux_repo "https://git.yoctoproject.org/git/linux-yocto/")
    set(linux_tag "v5.10.25")
    set(linux_arch "arm64")
    set(linux_cross_compile "aarch64-linux-gnu-")
    include(simulation)
    set(SIMULATION ON CACHE BOOL "Generate simulation script to run qemu with the proper arguments")
    if(SIMULATION)
        GenerateSimulateScript()
    endif()
endif()

AddToFileServer("linux" "${MASTER_THESIS_VM_IMAGES_DIR}/work/build/tmp/deploy/images/qemuarm64/Image")
set(rootfs_file "${MASTER_THESIS_VM_IMAGES_DIR}/work/build/tmp/deploy/images/qemuarm64/vm-qemuarm64.cpio.gz")

# Checkout and configure linux to build crossvm module
ExternalProject_Add(
    checkout_linux
    GIT_REPOSITORY
    ${linux_repo}
    GIT_TAG
    ${linux_tag}
    GIT_SHALLOW
    1
    GIT_PROGRESS
    1
    BUILD_COMMAND
    ""
    INSTALL_COMMAND
    ""
    CONFIGURE_COMMAND
    ""
    SOURCE_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/linux_out
)

# Linux config and symvers are to be copied to unpacked archive
set(linux_config "${MASTER_THESIS_VM_IMAGES_DIR}/work/build/tmp/work-shared/qemuarm64/kernel-build-artifacts/.config")
set(linux_symvers "${MASTER_THESIS_VM_IMAGES_DIR}/work/build/tmp/work-shared/qemuarm64/kernel-build-artifacts/Module.symvers")

# Configure unpacked archive with config and symvers
ConfigureLinux(
    ${CMAKE_CURRENT_BINARY_DIR}/linux_out
    ${linux_config}
    ${linux_symvers}
    configure_vm_linux
    ARCH
    ${linux_arch}
    CROSS_COMPILE
    ${linux_cross_compile}
    DEPENDS
    checkout_linux
)

# Compile CrossVM Dataport Module
DefineLinuxModule(
    ${CAMKES_VM_LINUX_DIR}/camkes-linux-artifacts/camkes-linux-modules/camkes-connector-modules/connection
    output_module
    output_module_target
    KERNEL_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/linux_out
    ARCH
    ${linux_arch}
    CROSS_COMPILE
    ${linux_cross_compile}
    DEPENDS
    checkout_linux
    configure_vm_linux
)
AddFileToOverlayDir(
    "connection.ko"
    ${output_module}
    "lib/modules/5.10.25-yocto-standard/kernel/drivers/vmm"
    overlay
    DEPENDS
    output_module_target
)

# Complile CrossVM Dataport Apps
ExternalProject_Add(
    dataport-apps
    URL
    file:///${CAMKES_VM_LINUX_DIR}/camkes-linux-artifacts/camkes-linux-apps/camkes-connector-apps/pkgs/dataport
    BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/dataport_apps
    INSTALL_COMMAND
    ""
    BUILD_ALWAYS
    ON
    EXCLUDE_FROM_ALL
    CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)
AddExternalProjFilesToOverlay(
    dataport-apps
    ${CMAKE_CURRENT_BINARY_DIR}/dataport_apps
    overlay
    "usr/bin"
    FILES
    dataport_read
    dataport_write
)

# Complile CrossVM Event Apps
ExternalProject_Add(
    event-apps
    URL
    file:///${CAMKES_VM_LINUX_DIR}/camkes-linux-artifacts/camkes-linux-apps/camkes-connector-apps/pkgs/emits_event
    BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/emits_event_apps
    INSTALL_COMMAND
    ""
    BUILD_ALWAYS
    ON
    EXCLUDE_FROM_ALL
    CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)
AddExternalProjFilesToOverlay(
    event-apps
    ${CMAKE_CURRENT_BINARY_DIR}/emits_event_apps
    overlay
    "usr/bin"
    FILES
    emits_event_emit
)

# Complile CrossVM Consume Event Apps
ExternalProject_Add(
    consume-event-apps
    URL
    file:///${CAMKES_VM_LINUX_DIR}/camkes-linux-artifacts/camkes-linux-apps/camkes-connector-apps/pkgs/consumes_event
    BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/consume_event_apps
    INSTALL_COMMAND
    ""
    BUILD_ALWAYS
    ON
    EXCLUDE_FROM_ALL
    CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)
AddExternalProjFilesToOverlay(
    consume-event-apps
    ${CMAKE_CURRENT_BINARY_DIR}/consume_event_apps
    overlay
    "usr/bin"
    FILES
    consumes_event_wait
)

# Add script to initialise dataport module
AddFileToOverlayDir(
    "S90crossvm_module_init"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/init_scripts/cross_vm_module_init
    "etc/init.d"
    overlay
)

# Construct new rootfs
AddOverlayDirToRootfs(
    overlay
    ${rootfs_file}
    "buildroot"
    "rootfs_install"
    output_overlayed_rootfs_location
    rootfs_target
    GZIP
)
AddToFileServer("linux-initrd" ${output_overlayed_rootfs_location} DEPENDS rootfs_target)

AddCamkesCPPFlag(
    cpp_flags
    CONFIG_VARS
    VmEmmc2NoDMA
    VmVUSB
    VmVchan
    Tk1DeviceFwd
    Tk1Insecure
    VmVirtioNetVirtqueue
)

DefineCAmkESVMFileServer()

CAmkESAddImportPath(${KernelARMPlatform})

# Define our VM Component with out cross vm dataports glue code
DeclareCAmkESComponent(VM SOURCES src/cross_vm_connections.c)

# Define our demo component that shares a dataport with the VM
DeclareCAmkESComponent(VconnErrorEvaluator SOURCES components/VconnErrorEvaluator/vconn_error_evaluator.c)

# Declare root server
DeclareCAmkESRootserver(
    main.camkes
    CPP_FLAGS
    ${cpp_flags}
    CPP_INCLUDES
    ${CAMKES_VM_DIR}/components/VM_Arm
)

# Should be done adding targets, can now generate the root server and the global configuration
GenerateCAmkESRootserver()
