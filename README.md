- [Installation Instructions](#installation-instructions)
	- [Downloading Source Code](#downloading-source-code)
	- [Compiling Driver](#compiling-driver)
	- [Loading Driver](#loading-driver)
	- [Installing Driver](#installing-driver)

## Installation Instructions
### Downloading Source Code
The source code for the Fastcom serial driver is hosted on Github code hosting.
To check out the latest code you will need Git and to run the following in a
terminal.

```
git clone git://github.com/commtech/serialfc-linux.git serialfc
```

NOTE: We prefer you use the above method for downloading the driver (because it
      is the easiest way to stay up to date) but you can also get the driver
      from the
      [download page](https://github.com/commtech/serialfc-linux/tags/).

Now that you have the latest code checked out you will more than likely want
to switch to a stable version within the code directory. To do this browse
the various tags for one you would like to switch to. Version v2.0.3 is only
listed here as an example.

```
git tag
git checkout v1.1.0
```

### Compiling Driver
Compiling the driver is relatively simple assuming you have all of the
required dependencies. Typically you will need gcc, make and your kernel's
header files. After assembling all of these things you can build the driver by
simply running the make command from within the source code directory.

```
cd serialfc/
make
```

If the kernel header files you would like to build against are not in the
default location `/lib/modules/$(shell uname -r)/build` then you can specify
the location with the KDIR option while building the driver.

```
make KDIR="/location/to/kernel_headers/"
```

### Loading Driver
Assuming the driver has been successfully built in the previous step you are
now ready to load the driver so you can begin using it. To do this you insert
the driver's kernel object file (serialfc.ko) into the kernel.

```
insmod serialfc.ko
```

NOTE: You will more than likely need administrator privileges for this and
   the following commands.

You can verify that the new ports were detected by checking the message log.

```
tail /var/log/kern.log
```

```
ttys5 at MMIO 0xd0004800 (irq = 21) is a 16550A
ttyS6 at MMIO 0xfeafc000 (irq = 17) is a ST16650
```

If you do not see any ports or if you do not see enough ports, it may be
that your kernel's defaults are set to only detect a too-small number of
serial ports.  This can be verified by looking at your current configuration
file usually found at `/boot/config-*`. Look for the defines for:
`CONFIG_SERIAL_8250_NR_UARTS` & `CONFIG_SERIAL_8250_RUNTIME_UARTS`.

If either of these numbers is smaller than the total number of serial ports
in the system (including the reserved ttyS0-3) then you will have trouble.

You can change the number of UARTs by modifying your grub config's kernel
line to say: `8250.nr_uarts=16` (or whatever number you like).

Alternatively you could recompile your kernel with those two config
lines modified appropriately.

### Installing Driver
If you would like the driver to load automatically at boot use the included
installer.

```
make install
```

To uninstall, use the included uninstaller.

```
make uninstall
```

### FAQ

##### How do I give my user account permissions to touch the serial ports?
```
adduser <username> dialout
```

##### How do I prevent `setserial` from caching old serial port settings?
In Debian based distributions you can reconfigure the setserial package  and set the default option to `manual`.
```
dpkg-reconfigure setserial
```
