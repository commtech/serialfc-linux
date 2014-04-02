# External Transmit

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

###### Operating Range
| Card Family | Range |
| ----------- | ----- |
| FSCC (16C950) | 0 - 8191 |


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_EXTERNAL_TRANSMIT
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...

unsigned status;

ioctl(fd, IOCTL_FASTCOM_GET_EXTERNAL_TRANSMIT, &status);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/external_transmit
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/external_transmit
```


## Enable
### IOCTL
```c
IOCTL_FASTCOM_ENABLE_EXTERNAL_TRANSMIT
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EINVAL` | 22 (0x16) | Invalid parameter |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
Enable external transmit on 4 bytes.
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_ENABLE_EXTERNAL_TRANSMIT, 4);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/external_transmit
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/external_transmit
```


## Disable
### IOCTL
```c
IOCTL_FASTCOM_DISABLE_EXTERNAL_TRANSMIT
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_DISABLE_EXTERNAL_TRANSMIT);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/external_transmit
```

###### Examples
```
echo 0 > /sys/class/serialfc/serialfc0/settings/external_transmit
```


### Additional Resources
- Complete example: [`examples/external-transmit.c`](../examples/external-transmit.c)
