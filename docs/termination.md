# Termination

###### Support
| Code         | Version
| ------------ | --------
| `serialfc-linux` | `v2.0.0`


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_TERMINATION
```

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
- Complete example: [`examples\termination.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/termination.c)
