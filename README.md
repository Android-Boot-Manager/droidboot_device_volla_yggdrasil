# Android Boot Manager for Volla Phone (droidboot_device_volla_yggdrasil)

This repo contains a [lk](https://github.com/littlekernel)-based bootlader for the [Volla Phone](https://volla.online/de/)
to support dualbooting (booting of different operating systems on the same mobile device)
from the internal device storage or an inserted (fast) SD card.

**Current status: Pre-alpha (only for development, not for end-users)**

-------
Be sure to check out our [wiki](https://github.com/Android-Boot-Manager/App/wiki)
-------

## About Volla phone

The [Volla Phone](https://volla.online/de/) (code name `yggdrasil`) is a privacy-focused mobile device which supports
different operating systems (customized Android, [Ubuntu Touch](https://ubuntu-touch.io/) and [SailfishOS](https://sailfishos.org/))

It was successfully sponsored by a [kickstarter campaign in 2020](https://www.kickstarter.com/projects/volla/volla-phone-designed-with-simplicity-and-security-in-mind).

The phone hardware is “Made in Germany” from the experienced hardware partner [Gigaset](https://www.gigaset.com/hq_en/).

## How it works

The boot manager is based on the source code of the MediaTek [BSP (Board Support Package)](https://en.wikipedia.org/wiki/Board_support_package) contained in the [Google Android source code](https://android.googlesource.com/platform/hardware/bsp/) and uses a small [lk](https://github.com/littlekernel)-based kernel.

It displays a simple GUI using the graphics library [lvgl](https://lvgl.io/) and reads configurations from an `ext2` formated cache partition.

The first config entry is used to boot the operating system from the internal device storage (*boot_linux_from_storage*). 

## Compilation

For compilation you will need google's `gcc4.9` toolchain.

You can get it from: https://android.googlesource.com/platform/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9

As google deprecated `gcc` you will need to switch to the last commit before deprecation with this shell command from within the toolchain directory:

`git checkout 16dab5225bf4c95baae3733f05b6e4b0e1c9aae9`

To build the bootloader image file open a terminal (shell) and navigate to the `droidboot` directory with `cd`, then enter:

`make TOOLCHAIN_PREFIX=~/toolchain/bin/arm-linux-androideabi- k63v2_64_bsp`

The bootloader is then contained in the file `build-k63v2_64_bsp/lk.img`

## Install and use the boot manager

Install and use the [Android Boot Manager App](https://github.com/Android-Boot-Manager/App) (requires unlocked bootloader)

## Disclaimer

It may possible that you could [brick](https://en.wikipedia.org/wiki/Brick_(electronics)) your device.

Use this bootloader at your own risk.
