# Clock Rate

###### Code Support
| Code | Version |
| ---- | ------- |
| serialfc-linux | 2.0.0 |

###### Card Support
| Card Family | Supported |
| ----------- |:-----:|
| FSCC (16C950) | Yes |
| Async-335 (17D15X) | Yes |
| Async-PCIe (17V35X) | See [Baud Rate Documentation](baud-rate.md) |


###### Operating Range
| Card Family | Range |
| ----------- | ----- |
| FSCC (16C950) | 200 Hz - 270 MHz |
| Async-335 (17D15X) | 6 Mhz - 180 Mhz |
| Async-PCIe (17V35X) | Not required |



## Set
### IOCTL
```c
IOCTL_FASTCOM_SET_CLOCK_RATE
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EINVAL` | 22 (0x16) | Invalid parameter |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_SET_CLOCK_RATE, 32);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/clock_rate
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/clock_rate
```


### Additional Resources
- Complete example: [`examples/clock-rate.c`](../examples/clock-rate.c)
