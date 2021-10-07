cmake_minimum_required(VERSION 3.7.2)

set(CAMKES_ARM_LINUX_DIR "${CMAKE_CURRENT_LIST_DIR}/linux" CACHE STRING "")

set(project_dir "${CMAKE_CURRENT_LIST_DIR}/../../")
set(supported "qemu-arm-virt")

file(GLOB project_modules ${project_dir}/projects/*)
list(
    APPEND
        CMAKE_MODULE_PATH
        ${project_dir}/kernel
        ${project_dir}/tools/seL4/cmake-tool/helpers/
        ${project_dir}/tools/seL4/elfloader-tool/
        ${project_modules}
)

set(SEL4_CONFIG_DEFAULT_ADVANCED ON)
set(CAMKES_CONFIG_DEFAULT_ADVANCED ON)
mark_as_advanced(CMAKE_INSTALL_PREFIX)
include(application_settings)

include(${CMAKE_CURRENT_LIST_DIR}/easy-settings.cmake)

# Kernel settings
set(KernelArch "arm" CACHE STRING "" FORCE)
if(AARCH64)
    set(KernelSel4Arch "aarch64" CACHE STRING "" FORCE)
else()
    set(KernelSel4Arch "arm_hyp" CACHE STRING "" FORCE)
    set(ARM_HYP ON CACHE INTERNAL "" FORCE)
endif()
set(KernelArmHypervisorSupport ON CACHE BOOL "" FORCE)
set(KernelRootCNodeSizeBits 18 CACHE STRING "" FORCE)
set(KernelArmVtimerUpdateVOffset OFF CACHE BOOL "" FORCE)
set(KernelArmDisableWFIWFETraps ON CACHE BOOL "" FORCE)

if(NOT "${PLATFORM}" IN_LIST supported)
    message(FATAL_ERROR "PLATFORM: ${PLATFORM} not supported.
         Supported: ${supported}")
endif()

set(VmPCISupport ON CACHE BOOL "" FORCE)
set(VmInitRdFile ON CACHE BOOL "" FORCE)

if(${PLATFORM} STREQUAL "qemu-arm-virt")
    set(KernelPlatform qemu-arm-virt CACHE STRING "" FORCE)
    set(KernelARMPlatform qemu-arm-virt CACHE STRING "" FORCE)
    set(QEMU_MEMORY "2048")
    set(KernelArmCPU cortex-a53 CACHE STRING "" FORCE)
endif()
correct_platform_strings()

# CAmkES Settings
set(CAmkESCPP ON CACHE BOOL "" FORCE)

# capDL settings
set(CapDLLoaderMaxObjects 90000 CACHE STRING "" FORCE)
set(LibUSB OFF CACHE BOOL "" FORCE)

find_package(seL4 REQUIRED)
sel4_configure_platform_settings()

# Set up elfloader for platform
ApplyData61ElfLoaderSettings(${KernelARMPlatform} ${KernelSel4Arch})

# Release settings
# ApplyCommonReleaseVerificationSettings(${RELEASE} FALSE)
