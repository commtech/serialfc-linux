# RX Trigger

###### Support
| Code         | Version
| ------------ | --------
| `serialfc-linux` | `v2.0.0`


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_RX_TRIGGER
```

###### Examples
```c
#include <serialfc.h>
...

unsigned level;

ioctl(fd, IOCTL_FASTCOM_GET_RX_TRIGGER, &level);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/rx_trigger
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/rx_trigger
```


## Set
### IOCTL
```c
IOCTL_FASTCOM_SET_RX_TRIGGER
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_SET_RX_TRIGGER, 32);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/rx_trigger
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/rx_trigger
```


### Additional Resources
- Complete example: [`examples\rx_trigger.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/rx_trigger.c)
