/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */
#include <autoconf.h>

#ifdef CONFIG_PLAT_TK1

#include "vmlinux.h"

#include <string.h>

#include <vka/capops.h>
#include <camkes.h>

#include <sel4arm-vmm/vm.h>
#include <sel4arm-vmm/images.h>
#include <sel4arm-vmm/plat/devices.h>
#include <sel4arm-vmm/devices/vgic.h>
#include <sel4arm-vmm/devices/vram.h>
#include <sel4utils/irq_server.h>
#include <cpio/cpio.h>

#include <sel4arm-vmm/devices/generic_forward.h>

#define ATAGS_ADDR        (LINUX_RAM_BASE + 0x100)

#define PAGE_SIZE_BITS 12

extern int start_extra_frame_caps;

static const struct device *linux_pt_devices[] = {
    &dev_usb1,
    &dev_usb3,
    &dev_sdmmc,
};

static const struct device *linux_ram_devices[] = {
#ifndef CONFIG_TK1_INSECURE
    &dev_rtc_kbc_pmc,
    &dev_data_memory,
    &dev_exception_vectors,
    &dev_system_registers,
    &dev_ictlr,
    &dev_apb_misc,
    &dev_fuse,
    &dev_gpios,
#endif /* CONFIG_TK1_INSECURE */
};



#ifdef CONFIG_TK1_DEVICE_FWD

struct generic_forward_cfg camkes_uart_d = {
  .read_fn = uartfwd_read,
  .write_fn = uartfwd_write
};

struct generic_forward_cfg camkes_clk_car =  {
  .read_fn = clkcarfwd_read,
  .write_fn = clkcarfwd_write
};

#endif
int
plat_install_linux_devices(vm_t* vm)
{
    int err;
    int i;

#ifdef CONFIG_TK1_DEVICE_FWD
    /* Configure UART forward device */
    err = vm_install_generic_forward_device(vm, &dev_vconsole, camkes_uart_d);
    assert(!err);

    /* Configure Clock and Reset forward device */
    err = vm_install_generic_forward_device(vm, &dev_clkcar, camkes_clk_car);
    assert(!err);
#endif // CONFIG_TK1_DEVICE_FWD

    err = vm_install_tk1_usb_passthrough_device(vm);
    assert(!err);

    /* Install pass through devices */
    /* In insecure mode TK1 passes through all devices at the moment by using on-demand device mapping */
    for (i = 0; i < sizeof(linux_pt_devices) / sizeof(*linux_pt_devices); i++) {
        err = vm_install_passthrough_device(vm, linux_pt_devices[i]);
        assert(!err);
    }

    /* Install ram backed devices */
    /* Devices that are just anonymous memory mappings */
    for (i = 0; i < sizeof(linux_ram_devices) / sizeof(*linux_ram_devices); i++) {
        err = vm_install_ram_only_device(vm, linux_ram_devices[i]);
        assert(!err);
    }

    /* hack to give access to other components
       see https://github.com/smaccm/vm_hack/blob/master/details.md for details */
    int offset = 0;
    for (i = 0; i < num_extra_frame_caps; i++) {
        err = vm_map_frame(vm, start_extra_frame_caps + i,
            extra_frame_map_address + offset, PAGE_SIZE_BITS, 1, seL4_AllRights);
        assert(!err);
        offset += PAGE_SIZE;
    }

    return 0;
}



#endif
