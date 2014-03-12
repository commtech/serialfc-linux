# Termination

###### Code Support
| Code | Version |
| ---- | ------- |
| serialfc-linux | 2.0.0 |

###### Card Support
| Card Family | Supported |
| ----------- |:-----:|
| FSCC (16C950) | No |
| Async-335 (17D15X) | No |
| Async-PCIe (17V35X) | Yes |


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_TERMINATION
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...

unsigned status;

ioctl(fd, IOCTL_FASTCOM_GET_TERMINATION, &status);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/termination
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/termination
```


## Enable
### IOCTL
```c
IOCTL_FASTCOM_ENABLE_TERMINATION
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_ENABLE_TERMINATION);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/termination
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/termination
```


## Disable
### IOCTL
```c
IOCTL_FASTCOM_DISABLE_TERMINATION
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_DISABLE_TERMINATION);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/termination
```

###### Examples
```
echo 0 > /sys/class/serialfc/serialfc0/settings/termination
```


### Additional Resources
- Complete example: [`examples/termination.c`](../examples/termination.c)
