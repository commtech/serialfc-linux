# 9-Bit Protocol

Enabling 9-Bit protocol has a couple of effects.

- Transmitting with 9-bit protocol enabled automatically sets the 1st byte's 9th bit to MARK, and all remaining bytes's 9th bits to SPACE.
- Receiving with 9-bit protocol enabled will return two bytes per each 9-bits of data. The second of each byte-duo contains the 9th bit.

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


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_9BIT
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...

unsigned status;

ioctl(fd, IOCTL_FASTCOM_GET_9BIT, &status);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/nine_bit
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/nine_bit
```


## Enable
### IOCTL
```c
IOCTL_FASTCOM_ENABLE_9BIT
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_ENABLE_9BIT);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/nine_bit
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/nine_bit
```


## Disable
### IOCTL
```c
IOCTL_FASTCOM_DISABLE_9BIT
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_DISABLE_9BIT);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/nine_bit
```

###### Examples
```
echo 0 > /sys/class/serialfc/serialfc0/settings/nine_bit
```


### Additional Resources
- Complete example: [`examples/nine-bit.c`](../examples/nine-bit.c)
