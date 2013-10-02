# Echo Cancel

###### Support
| Code         | Version
| ------------ | --------
| `serialfc-linux` | `v2.0.0`


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_ECHO_CANCEL
```

###### Examples
```c
#include <serialfc.h>
...

unsigned status;

ioctl(fd, IOCTL_FASTCOM_GET_ECHO_CANCEL, &status);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/echo_cancel
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/echo_cancel
```


## Enable
### IOCTL
```c
IOCTL_FASTCOM_ENABLE_ECHO_CANCEL
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_ENABLE_ECHO_CANCEL);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/echo_cancel
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/echo_cancel
```


## Disable
### IOCTL
```c
IOCTL_FASTCOM_DISABLE_ECHO_CANCEL
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_DISABLE_ECHO_CANCEL);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/echo_cancel
```

###### Examples
```
echo 0 > /sys/class/serialfc/serialfc0/settings/echo_cancel
```


### Additional Resources
- Complete example: [`examples\echo_cancel.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/echo_cancel.c)
