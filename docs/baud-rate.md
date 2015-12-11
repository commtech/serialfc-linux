# Baud Rate

###### Code Support
| Code | Version |
| ---- | ------- |
| serialfc-linux | 2.3.0+ |

###### Card Support
| Card Family | Supported |
| ----------- |:-----:|
| FSCC (16C950) | No |
| Async-335 (17D15X) | No |
| Async-PCIe (17V35X) | Yes |


###### Operating Range
| Card Family | Range |
| ----------- | ----- |
| FSCC (16C950) | N/A |
| Async-335 (17D15X) | N/A |
| Async-PCIe (17V35X) | Not required |



## Set
### IOCTL
```c
IOCTL_FASTCOM_SET_BAUD_RATE
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EINVAL` | 22 (0x16) | Invalid parameter |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_SET_BAUD_RATE, 9600);

...
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/baud_rate
```

###### Examples
```
echo 9600 > /sys/class/serialfc/serialfc0/settings/baud_rate
```

## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_BAUD_RATE
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EINVAL` | 22 (0x16) | Invalid parameter |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...
unsigned long baud_rate = 0;

ioctl(fd, IOCTL_FASTCOM_GET_BAUD_RATE, &baud_rate);

...
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/baud_rate
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/baud_rate
```

## Additional Resources
- Complete example: [`examples/baud-rate.c`](../examples/baud-rate.c)
