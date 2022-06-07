#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.8.2)

project(camkes-vm-virtio-net-2-VMs C)

include(${CAMKES_ARM_VM_HELPERS_PATH})

find_package(camkes-vm-linux REQUIRED)
include(${CAMKES_VM_LINUX_HELPERS_PATH})

set(dtb_file "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/linux-dtb")
set(rootfs_file "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/rootfs.cpio.gz")

if("${KernelARMPlatform}" STREQUAL "exynos5422")
    # Create our CPP Flags based on ARM VM config variables
    set(cpp_flags "-DKERNELARMPLATFORM_EXYNOS5422")
    set(rootfs_address "0x4d700000")

elseif("${KernelARMPlatform}" STREQUAL "tx2")
    set(cpp_flags "-DKERNELARMPLATFORM_TX2")
    set(rootfs_address "0xf7000000")
    set(dtb_file "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/linux-pci-dtb")
    set(rootfs_file "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/rootfs_crossvm.cpio.gz")
elseif("${KernelARMPlatform}" STREQUAL "qemu-arm-virt")
    # Create our CPP Flags based on ARM VM config variables
    set(cpp_flags "-DKERNELARMPLATFORM_QEMU-ARM-VIRT")
    set(SIMULATION ON CACHE BOOL "Generate simulation script to run qemu with the proper arguments")
    include(simulation)
    if(SIMULATION)
        GenerateSimulateScript()
    endif()
    set(rootfs_file "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/rootfs.cpio.gz")
endif()

# Generate overlayed rootfs VM0
AddFileToOverlayDir(
    "interfaces"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/network_interfaces/interfaces_vm0
    "etc/network"
    overlay_vm0
)

AddFileToOverlayDir(
    "S90ping_test"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/init_scripts/ping_test
    "etc/init.d"
    overlay_vm0
)

AddOverlayDirToRootfs(
    overlay_vm0
    ${rootfs_file}
    "buildroot"
    "rootfs_install"
    output_overlayed_rootfs_location_vm0
    rootfs_target
    GZIP
)


# Generate overlayed rootfs VM1
AddFileToOverlayDir(
    "interfaces"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/network_interfaces/interfaces_vm1
    "etc/network"
    overlay_vm1
)

AddFileToOverlayDir(
    "S90ping_test"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/init_scripts/ping_test
    "etc/init.d"
    overlay_vm1
)

AddOverlayDirToRootfs(
    overlay_vm1
    ${rootfs_file}
    "buildroot"
    "rootfs_install"
    output_overlayed_rootfs_location_vm1
    rootfs_target_vm1
    GZIP
)


# Updated dtb based on generated initrd
#UpdateDtbFromInitrd(
	#${dtb_file}
    #${output_overlayed_rootfs_location}
    #${rootfs_address}
    #dtb_gen_target
    #output_dtb_location
    #DEPENDS
    #rootfs_target
    #)

#AddToFileServer("linux-dtb" "${output_dtb_location}" DEPENDS dtb_gen_target)
AddToFileServer("linux-initrd" ${output_overlayed_rootfs_location_vm0} DEPENDS rootfs_target)
AddToFileServer("linux-initrd-vm1" ${output_overlayed_rootfs_location_vm1} DEPENDS rootfs_target_vm1)
AddToFileServer("linux" "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/linux")

AddCamkesCPPFlag(
    cpp_flags
    CONFIG_VARS
    VmEmmc2NoDMA
    VmVUSB
    Tk1DeviceFwd
    Tk1Insecure
    VmVirtioNetVirtqueue
)

DefineCAmkESVMFileServer()

CAmkESAddImportPath(${KernelARMPlatform})

# Declare root server
if(VIRTIO_NET_PING)
    DeclareCAmkESComponent(
        PingClient
        SOURCES
        components/PingClient/ping_client.c
        LIBS
        virtqueue
        sel4_autoconf
    )

    DeclareCAmkESRootserver(
        vm_echo_ping.camkes
        CPP_FLAGS
        ${cpp_flags}
        CPP_INCLUDES
        ${CAMKES_VM_DIR}/components/VM_Arm
    )
else()
    DeclareCAmkESRootserver(
        vm_virtio_net.camkes
        CPP_FLAGS
        ${cpp_flags}
        CPP_INCLUDES
        ${CAMKES_VM_DIR}/components/VM_Arm
    )
endif()
