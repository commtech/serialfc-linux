# serialfc-linux
This README file is best viewed [online](http://github.com/commtech/serialfc-linux/).

## Installing Driver

##### Downloading Driver Source Code
The source code for the SerialFC driver is hosted on Github code hosting. To check
out the latest code you will need Git and to run the following in a terminal.

```
git clone git://github.com/commtech/serialfc-linux.git serialfc
```

You can also download driver packages directly from our
[website](http://www.commtech-fastcom.com/CommtechSoftware.html).

##### Switch To Stable Version
Now that you have the latest code checked out, switch to the latest stable
version v2.0.1 is only listed here as an example.

```
git tag
git checkout v2.0.1
```

##### Build Source Code
Run the make command from within the source code directory to build the driver.

```
cd serialfc/
make
```

If you would like to enable debug prints within the driver you need to add
the DEBUG option while building the driver.

```
make DEBUG=1
```

Once debugging is enabled you will find extra kernel prints in the
/var/log/messages and /var/log/debug log files.

If the kernel header files you would like to build against are not in the
default location `/lib/modules/$(shell uname -r)/build` then you can specify
the location with the KDIR option while building the driver.

```
make KDIR="/location/to/kernel_headers/"
```

##### Loading Driver
Assuming the driver has been successfully built in the previous step you are
now ready to load the driver so you can begin using it. To do this you insert
the driver's kernel object file (serialfc.ko) into the kernel.

```
insmod serialfc.ko
```

_You will more than likely need administrator privileges for this and
the following commands._

If no cards are present you will see the following message.

```
insmod serialfc.ko
insmod: error inserting 'serialfc.ko': -1 No such device
```

If you are using an FSCC card for asynchronous communication only you can
enable asynchronous by default by specifying the `fscc_enable_async` option.

```
insmod serialfc.ko fscc_enable_async=1
```

_All driver load time options can be set in your modprobe.conf file for
using upon system boot_

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

More information on this can be found in the FAQ section below.


##### Installing Driver
If you would like the driver to automatically load at boot use the included
installer.

```
make install
```

This will also install the header (.h) files.

To uninstall, use the included uninstaller.

```
make uninstall
```

By default the FSCC driver boots up in synchronous communication mode. To
switch to the asynchronous mode you must modify the FSCC card's FCR register
to allow for asynchronous communication. There are multiple ways of doing
this. Possibly the simplest method is using sysfs and the command line.

```
echo 03000000 > /sys/class/fscc/fscc0/registers/fcr
```


## Quick Start Guide
There is documentation for each specific function listed below, but lets get started
with a quick programming example for fun.

_This tutorial has already been set up for you at_
[`serialfc/examples/tutorial.c`](https://github.com/commtech/serialfc-linux/tree/master/examples/tutorial.c).

Create a new C file (named tutorial.c) with the following code.

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <serialfc.h>

int main(void)
{
    int fd = 0;
    char odata[] = "Hello world!";
    char idata[20];
    int file_status;
    unsigned type;
    struct termios tios;

    /* Open port 0 (ttyS4) */
    fd = open("/dev/ttyS4", O_RDWR | O_NDELAY);

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    /* Turn off O_NDELAY now that we have the port open */
    file_status = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, file_status & ~O_NDELAY);

    ioctl(fd, IOCTL_FASTCOM_GET_CARD_TYPE, &type);

    if (type == SERIALFC_CARD_TYPE_PCIE) {
        int status = 0;

        ioctl(fd, TIOCMGET, &status);
        status &= ~TIOCM_DTR; /* Set DTR to 1 (transmitter always on, 422) */
        ioctl(fd, TIOCMSET, &status);
    }


    /* Configure serial settings */
    tcgetattr(fd, &tios);

    tios.c_iflag = IGNBRK;
    tios.c_oflag = 0;
    tios.c_cflag = CS8 | CREAD | CLOCAL;
    tios.c_lflag &= ~(ICANON | ISIG | ECHO);

    cfsetospeed(&tios, B921600);
    cfsetispeed(&tios, 0); /* Set ispeed = ospeed */

    tcsetattr(fd, TCSANOW, &tios);


    /* Send "Hello world!" text */
    write(fd, odata, sizeof(odata));

    /* Read the data back in (with our loopback connector) */
    read(fd, idata, sizeof(idata));

    fprintf(stdout, "%s\n", idata);

    close(fd);

    return EXIT_SUCCESS;
}
```

For this example I will use the gcc compiler, but you can use your compiler of
choice.

```
# gcc -I ../lib/raw/ tutorial.c
```

Now attach the included loopback connector.

```
# ./a.out
Hello world!
```

You have now transmitted and received an asynchronous frame!


## API Reference

There are likely other configuration options you will need to set up for your
own program. All of these options are described on their respective documentation page.

- [Baud Rate](docs/baud-rate.md)
- [Connect](docs/connect.md)
- [Card Type](docs/card-type.md)
- [Clock Rate](docs/clock-rate.md)
- [Echo Cancel](docs/echo-cancel.md)
- [External Transmit](docs/external-transmit.md)
- [Frame Length](docs/frame-length.md)
- [Isochronous](docs/isochronous.md)
- [9-Bit Protocol](docs/nine-bit.md)
- [Read](docs/read.md)
- [RS485](docs/rs485.md)
- [RX Trigger](docs/rx-trigger.md)
- [Sample Rate](docs/sample-rate.md)
- [Termination](docs/termination.md)
- [TX Trigger](docs/tx-trigger.md)
- [Write](docs/write.md)
- [Disconnect](docs/disconnect.md)

There are also multiple code libraries to make development easier.
- [C](https://github.com/commtech/cserialfc/)
- [C++](https://github.com/commtech/cppserialfc/)
- [.NET](https://github.com/commtech/netserialfc/)
- [Python](https://github.com/commtech/pyserialfc/)


### 422/X-PCIe Differences
The 422/X-PCIe family of cards use DTR to manage RS485. This causes complications when using the Linux
serial driver. There are a couple extra steps needed in your software to use the card correctly.

##### 422 Mode
```
int mode = 0;

ioctl(fd, TIOCMGET, &mode);
mode &= ~TIOCM_DTR; /* Set DTR to 0 (transmitter always on, 422) */
ioctl(fd, TIOCMSET, &mode);

// Other 422 initialization code

```

##### 485 Mode
```
int mode = 0;

ioctl(ttys_fd, TIOCMGET, &mode);
mode |= 0x2000; /* Use DTR signal for turning on transmitter */
ioctl(ttys_fd, TIOCMSET, &mode);

// Other 485 initialization code

```


### Custom Baud Rates
Using custom baud rates in Linux require a few modifications.

```
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
...

/* Set the speed to 38400 so that we can use the custom speed below */
tcgetattr(ttys_fd, &tios);

cfsetospeed(&tios, B38400);
cfsetispeed(&tios, B38400);

tcsetattr(ttys_fd, TCSANOW, &tios);


/* Set up our new baud base (clock frequency / sampling rate) and enable
 custom speed flag */
int sample_rate = 16; // This is used earlier to set the sample rate.
int baud = 7812500; // Desired baud rate.
int clock_speed = 125000000; // This is for the 422/x-PCIe
ioctl(ttys_fd, TIOCGSERIAL, &ss);
ss.baud_base = clock_speed / sample_rate; /* Requires admin rights */
ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
ss.custom_divisor = (ss.baud_base + (baud / 2)) / baud;

ioctl(ttys_fd, TIOCSSERIAL, &ss);
```

This can also be achieved using the `setserial` utlity and the `baud_base`,
`spd_cust` and `divisor` parameters.

Here is an example of setting a baud rate of 1 MHz assuming a clock frequency
of 16 MHz and a sampling rate of 16.
```
# setserial /dev/ttyS# baud_base 1000000   // clock frequency / sampling rate
# setserial /dev/ttyS# spd_cust            // turn the spd_cust flag ON
# setserial /dev/ttyS# divisor 1           // spd_cust will use a divisor of 1
```

### Arbitrary baud rates
In the case that completely arbitrary baud rates are required, and a
fractional divisor is needed, IOCTL / sysfs is provided to set a custom baud rate.
To set the custom baud rate, the serialfc driver must be used, instead of the
kernel provided serial/tty.

```
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <serialfc.h>
...

/* Set the speed to 38400 so that we can use the custom speed below */
tcgetattr(ttys_fd, &tios);
cfsetospeed(&tios, B38400);
cfsetispeed(&tios, B38400);
tcsetattr(ttys_fd, TCSANOW, &tios);

unsigned long desired_baud = 10000000ul;

ioctl(serialfc_fd, IOCTL_FASTCOM_SET_BAUD_RATE, desired_baud);
```

Note that as a side effect of setting the baud rate this way, the sampling rate
may also be changed.

Support for setting the baud rate in this way is limited. See [baud rate](docs/baud-rate.md)
documentation for more details.

### FAQ

##### Why am I not receiving all of my data while in RS485 mode?
Manually closing the port immediately after an RS485 transfer could cause the serial
driver to disable the transmitter. Either don't close the port (which will give it time
to clean itself up), run other code, or manually put in a sleep before closing

##### Why does my system not have enough /dev/ttyS nodes?
Some Linux distributions have the default number of serial ports that are
available at boot set to a small number (usually 4). The first four serial
ports are reserved so you will need to change this value to something larger
to be able to configure more serial ports.

There are a couple ways of doing this. The easiest method is by appending
'8250.nr_uarts=x' to your grub boot line. Something like this:

kernel /boot/vmlinuz-2.6.20-15-generic ro quiet splash 8250.nr_uarts=16

This can be done temporarily by pressing 'e' at the grub menu during boot or
by permanently modifying this value which is grub version specific. To do
this please search google for one of the numerous guides on the subject.

Another method is by editing the .config file of you kernel before compiling
it to allow for more serial ports. This is not preferred because you will
need to recompile the kernel for it to take effect. The line you need to
change in the .config file is SERIAL_8250_RUNTIME_UARTS.

##### The `open` call is hanging. What am I doing wrong?
There are a couple possibilities. If you are using an FSCC card, make sure
you can have asynchronous mode enabled in the FCR register. Also, some
of the older FSCC firmware versions require you to open the port in O_NONBLOCK
mode.

##### How do I give my user account permissions to touch the serial ports?
```
usermod -a -G dialout <username>
```

##### How do I prevent `setserial` from caching old serial port settings?
In Debian based distributions you can reconfigure the setserial package  and set the default option to `manual`.
```
dpkg-reconfigure setserial
```


## Build Dependencies
- Kernel Build Tools (GCC, make, kernel headers, etc)


## Run-time Dependencies
- OS: Linux
- Base Installation: >= 2.6.16 (might work with a lower version)
- Sysfs Support: >= 2.6.25


## API Compatibility
We follow [Semantic Versioning](http://semver.org/) when creating releases.


## License

Copyright (C) 2014 [Commtech, Inc.](http://commtech-fastcom.com)

Licensed under the [GNU General Public License v3](http://www.gnu.org/licenses/gpl.txt).
