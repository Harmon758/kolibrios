qemu-system-i386 -m 256 -fda ../../test_kos_images/kolibri.img -boot a -vga vmware -net nic,model=rtl8139 -net user -soundhw ac97 -usb -usbdevice tablet -drive file=fat:rw:.