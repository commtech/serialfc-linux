# Isochronous

| Mode | Description
| ---- | -----------
|	0    | Transmit using external RI#
|	1    | Transmit using internal BRG
|	2    | Receive using external DSR#
|	3    | Transmit using external RI#, receive using external DSR#
|	4    | Transmit using internal BRG, receive using external DSR#
|	5    | Receive using internal BRG
|	6    | Transmit using external RI#, receive using internal BRG
|	7    | Transmit using internal BRG, receive using internal BRG
|	8    | Transmit and receive using external RI#
|	9    | Transmit clock is output on DTR#
|	10   | Transmit clock is output on DTR#, receive using external DSR#

###### Support
| Code         | Version
| ------------ | --------
| `serialfc-linux` | `v2.0.0`


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_ISOCHRONOUS
```

###### Examples
```c
#include <serialfc.h>
...

unsigned status;

ioctl(fd, IOCTL_FASTCOM_GET_ISOCHRONOUS, &status);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/isochronous
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/isochronous
```


## Enable
### IOCTL
```c
IOCTL_FASTCOM_ENABLE_ISOCHRONOUS
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_ENABLE_ISOCHRONOUS);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/isochronous
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/isochronous
```


## Disable
### IOCTL
```c
IOCTL_FASTCOM_DISABLE_ISOCHRONOUS
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_DISABLE_ISOCHRONOUS);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/isochronous
```

###### Examples
```
echo 0 > /sys/class/serialfc/serialfc0/settings/isochronous
```


### Additional Resources
- Complete example: [`examples\isochronous.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/isochronous.c)
