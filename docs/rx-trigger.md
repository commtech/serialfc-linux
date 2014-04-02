# RX Trigger

The RX FIFO trigger level generates an interrupt whenever the receive FIFO level rises to this preset trigger level.

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

###### Operating Range
| Card Family | Range |
| ----------- | ----- |
| FSCC (16C950) | 0 - 127 |
| Async-335 (17D15X) | 0 - 64 |
| Async-PCIe (17V35X) | 0 - 255 |


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_RX_TRIGGER
```

###### Examples
```c
#include <serialfc.h>
...

unsigned level;

ioctl(fd, IOCTL_FASTCOM_GET_RX_TRIGGER, &level);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/rx_trigger
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/rx_trigger
```


## Set
### IOCTL
```c
IOCTL_FASTCOM_SET_RX_TRIGGER
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EINVAL` | 22 (0x16) | Invalid parameter |

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_SET_RX_TRIGGER, 32);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/rx_trigger
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/rx_trigger
```


### Additional Resources
- Complete example: [`examples/rx-trigger.c`](../examples/rx-trigger.c)
