# 9-Bit Protocol

###### Support
| Code         | Version
| ------------ | --------
| `serialfc-linux` | `v2.0.0`


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_9BIT
```

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
- Complete example: [`examples\nine_bit.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/nine_bit.c)
