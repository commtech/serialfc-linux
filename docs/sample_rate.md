# Sample Rate

###### Support
| Code         | Version
| ------------ | --------
| `serialfc-linux` | `v2.0.0`


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
- Complete example: [`examples\sample_rate.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/sample_rate.c)
