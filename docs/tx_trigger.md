# TX Trigger

###### Support
| Code         | Version
| ------------ | --------
| `serialfc-linux` | `v2.0.0`


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_TX_TRIGGER
```

###### Examples
```c
#include <serialfc.h>
...

unsigned level;

ioctl(fd, IOCTL_FASTCOM_GET_TX_TRIGGER, &level);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/tx_trigger
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/tx_trigger
```


## Set
### IOCTL
```c
IOCTL_FASTCOM_SET_TX_TRIGGER
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_SET_TX_TRIGGER, 32);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/tx_trigger
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/tx_trigger
```


### Additional Resources
- Complete example: [`examples\tx_trigger.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/tx_trigger.c)
