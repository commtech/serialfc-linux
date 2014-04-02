# Echo Cancel

The echo cancel feature disables the receiver while transmitting. This is mainly used in RS485 mode when the transmit and receive lines are tied together.

_This is a board-wide (as opposed to port-by-port) setting on the Async-335 family of cards._

###### Code Support
| Code | Version |
| ---- | ------- |
| serialfc-linux | 2.0.0 |

###### Card Support
| Card Family | Supported |
| ----------- |:-----:|
| FSCC (16C950) | Yes |
| Async-335 (17D15X) | Yes |
| Async-PCIe (17V35X) | Yes |


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_ECHO_CANCEL
```

###### Examples
```c
#include <serialfc.h>
...

unsigned status;

ioctl(fd, IOCTL_FASTCOM_GET_ECHO_CANCEL, &status);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/echo_cancel
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/echo_cancel
```


## Enable
### IOCTL
```c
IOCTL_FASTCOM_ENABLE_ECHO_CANCEL
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_ENABLE_ECHO_CANCEL);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/echo_cancel
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/echo_cancel
```


## Disable
### IOCTL
```c
IOCTL_FASTCOM_DISABLE_ECHO_CANCEL
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_DISABLE_ECHO_CANCEL);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/echo_cancel
```

###### Examples
```
echo 0 > /sys/class/serialfc/serialfc0/settings/echo_cancel
```


### Additional Resources
- Complete example: [`examples/echo-cancel.c`](../examples/echo-cancel.c)
