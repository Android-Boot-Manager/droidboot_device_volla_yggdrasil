###droidboot_device_volla_yggdrasil
This repo contains lk based bootlader for the Volla Phone. It is based on sources from BSP. Created for dualbooting. It can display a simple GUI with lvgl, and read configs from an ext2 formated cache partion.
First entry will call boot_linux_from_storage. 

##Compilation
For compilation you will need google's gcc4.9 toolchain. Get it from: ttps://android.googlesource.com/platform/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9
As google deprecated gcc you will need to switch to commit before deprecation, do that with: git checkout 16dab5225bf4c95baae3733f05b6e4b0e1c9aae9 in toolchain
directory. Navigate to droidboot directory with cd. Run make TOOLCHAIN_PREFIX=~/toolchain/bin/arm-linux-androideabi- k63v2_64_bsp. build-k63v2_64_bsp/lk.img is the bootloader.
##Installation
On a phone with an unlocked bootloader: fastboot flash lk lk.img
