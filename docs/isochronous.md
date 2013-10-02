# Isochronous

###### Support
| Code         | Version
| ------------ | --------
| `serialfc-linux` | `v2.0.0`


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_ISOCHRONOUS
```

###### Examples
```c
#include <serialfc.h>
...

unsigned status;

ioctl(fd, IOCTL_FASTCOM_GET_ISOCHRONOUS, &status);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/isochronous
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/isochronous
```


## Enable
### IOCTL
```c
IOCTL_FASTCOM_ENABLE_ISOCHRONOUS
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_ENABLE_ISOCHRONOUS);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/isochronous
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/isochronous
```


## Disable
### IOCTL
```c
IOCTL_FASTCOM_DISABLE_ISOCHRONOUS
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_DISABLE_ISOCHRONOUS);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/isochronous
```

###### Examples
```
echo 0 > /sys/class/serialfc/serialfc0/settings/isochronous
```


### Additional Resources
- Complete example: [`examples\isochronous.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/isochronous.c)
