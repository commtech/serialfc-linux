# Clock Rate

###### Support
| Code         | Version
| ------------ | --------
| `serialfc-linux` | `v2.0.0`


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_CLOCK_RATE
```

###### Examples
```c
#include <serialfc.h>
...

unsigned rate;

ioctl(fd, IOCTL_FASTCOM_GET_CLOCK_RATE, &rate);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/clock_rate
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/clock_rate
```


## Set
### IOCTL
```c
IOCTL_FASTCOM_SET_CLOCK_RATE
```

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
- Complete example: [`examples\clock_rate.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/clock_rate.c)
