# Frame Length

The frame length specifies the number of bytes that get transmitted between the start and stop bits. The standard asynchronous serial communication protocol uses a frame length of one.

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
IOCTL_FASTCOM_GET_FRAME_LENGTH
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...

unsigned frame_length;

ioctl(fd, IOCTL_FASTCOM_GET_FRAME_LENGTH, &frame_length);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/frame_length
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/frame_length
```


## Enable
### IOCTL
```c
IOCTL_FASTCOM_ENABLE_FRAME_LENGTH
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EINVAL` | 22 (0x16) | Invalid parameter |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
Transmit 4 bytes per frame.
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_ENABLE_FRAME_LENGTH, 4);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/frame_length
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/frame_length
```


## Disable
### IOCTL
```c
IOCTL_FASTCOM_DISABLE_FRAME_LENGTH
```

| System Error | Value | Cause |
| ------------ | -----:| ----- |
| `EPROTONOSUPPORT` | 93 (0x5D) | Not supported on this family of cards |

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_DISABLE_FRAME_LENGTH);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/frame_length
```

###### Examples
```
echo 0 > /sys/class/serialfc/serialfc0/settings/frame_length
```


### Additional Resources
- Complete example: [`examples/frame-length.c`](../examples/frame-length.c)
