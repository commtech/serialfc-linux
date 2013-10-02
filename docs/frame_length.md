# Frame Length

###### Support
| Code         | Version
| ------------ | --------
| `serialfc-linux` | `v2.0.0`


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_FRAME_LENGTH
```

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
- Complete example: [`examples\frame_length.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/frame_length.c)
