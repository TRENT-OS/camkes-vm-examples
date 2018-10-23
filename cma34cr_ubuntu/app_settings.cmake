#
# Copyright 2018, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(DATA61_BSD)
#

cmake_minimum_required(VERSION 3.8.2)

# Define kernel config options
set(KernelX86Sel4Arch x86_64 CACHE STRING "" FORCE)
set(KernelMaxNumNodes 2 CACHE STRING "" FORCE)
set(KernelHugePage OFF CACHE BOOL "" FORCE)
set(KernelSupportPCID OFF CACHE BOOL "" FORCE)

set(KernelMultibootGFXMode linear CACHE STRING "" FORCE)
set(KernelMultibootGFXModeWidth 0 CACHE STRING "" FORCE)
set(KernelMultibootGFXModeDepth 0 CACHE STRING "" FORCE)
set(KernelMultibootGFXModeHeight 0 CACHE STRING "" FORCE)

# Define VMM configurations
set(LibSel4VMMVESAFrameBuffer ON CACHE BOOL "" FORCE)
