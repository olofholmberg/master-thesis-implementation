cmake_minimum_required(VERSION 3.16.3)
include(settings.cmake)

project(masterthesis C CXX ASM)

include(ExternalProject)
include(external-project-helpers)

# Create our CPP Flags based on ARM VM config variables
if("${PLATFORM}" STREQUAL "qemu-arm-virt")
    set(cpp_flags "-DKERNELARMPLATFORM_QEMU-ARM-VIRT")
    set(linux_arch "arm64")
    set(linux_cross_compile "aarch64-linux-gnu-")
    include(simulation)
    set(SIMULATION ON CACHE BOOL "Generate simulation script to run qemu with the proper arguments")
    if(SIMULATION)
        GenerateSimulateScript()
    endif()
endif()

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

CAmkESAddImportPath(${KernelARMPlatform})

# The Client component that mimics the function call from the ovs-testcontroller in the VM
DeclareCAmkESComponent(Client SOURCES components/Client/client.c)

# The VEE Component that shares two dataports with the Client
DeclareCAmkESComponent(VconnErrorEvaluator SOURCES components/VconnErrorEvaluator/vconn_error_evaluator.c)

# Declare root server
DeclareCAmkESRootserver(
    main.camkes
    CPP_FLAGS
    ${cpp_flags}
)

# Should be done adding targets, can now generate the root server and the global configuration
GenerateCAmkESRootserver()
