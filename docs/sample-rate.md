# Sample Rate

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
| FSCC (16C950) | 4 - 16 |
| Async-335 (17D15X) | 8, 16 |
| Async-PCIe (17V35X) | 4, 8, 16 |


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_SAMPLE_RATE
```

###### Examples
```c
#include <serialfc.h>
...

unsigned rate;

ioctl(fd, IOCTL_FASTCOM_GET_SAMPLE_RATE, &rate);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/sample_rate
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/sample_rate
```


## Set
### IOCTL
```c
IOCTL_FASTCOM_SET_SAMPLE_RATE
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EINVAL` | 22 (0x16) | Invalid parameter |

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_SET_SAMPLE_RATE, 32);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/sample_rate
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/sample_rate
```


### Additional Resources
- Complete example: [`examples/sample-rate.c`](../examples/sample-rate.c)
