# TX Trigger

The TX FIFO trigger level generates an interrupt whenever the data level in the transmit FIFO falls below this preset trigger level.

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
IOCTL_FASTCOM_GET_TX_TRIGGER
```

###### Examples
```c
#include <serialfc.h>
...

unsigned level;

ioctl(fd, IOCTL_FASTCOM_GET_TX_TRIGGER, &level);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/tx_trigger
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/tx_trigger
```


## Set
### IOCTL
```c
IOCTL_FASTCOM_SET_TX_TRIGGER
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EINVAL` | 22 (0x16) | Invalid parameter |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_SET_TX_TRIGGER, 32);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/tx_trigger
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/tx_trigger
```


### Additional Resources
- Complete example: [`examples/tx-trigger.c`](../examples/tx-trigger.c)
