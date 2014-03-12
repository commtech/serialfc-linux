# Isochronous

###### Code Support
| Code | Version |
| ---- | ------- |
| serialfc-linux | 2.0.0 |

###### Card Support
| Card Family | Supported |
| ----------- |:-----:|
| FSCC (16C950) | Yes |
| Async-335 (17D15X) | No |
| Async-PCIe (17V35X) | No |

| Mode | Description |
| ----:| ----------- |
| 0 | Transmit using external RI# |
| 1 | Transmit using internal BRG |
| 2 | Receive using external DSR# |
| 3 | Transmit using external RI#, receive using external DSR# |
| 4 | Transmit using internal BRG, receive using external DSR# |
| 5 | Receive using internal BRG |
| 6 | Transmit using external RI#, receive using internal BRG |
| 7 | Transmit using internal BRG, receive using internal BRG |
| 8 | Transmit and receive using external RI# |
| 9 | Transmit clock is output on DTR# |
| 10 | Transmit clock is output on DTR#, receive using external DSR# |


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_ISOCHRONOUS
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

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

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EINVAL` | 22 (0x16) | Invalid parameter |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

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

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

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
- Complete example: [`examples/isochronous.c`](../examples/isochronous.c)
