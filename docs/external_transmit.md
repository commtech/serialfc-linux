# External Transmit

###### Support
| Code         | Version
| ------------ | --------
| `serialfc-linux` | `v2.0.0`


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_EXTERNAL_TRANSMIT
```

###### Examples
```c
#include <serialfc.h>
...

unsigned status;

ioctl(fd, IOCTL_FASTCOM_GET_EXTERNAL_TRANSMIT, &status);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/external_transmit
```

###### Examples
```
cat /sys/class/serialfc/serialfc0/settings/external_transmit
```


## Enable
### IOCTL
```c
IOCTL_FASTCOM_ENABLE_EXTERNAL_TRANSMIT
```

###### Examples
Enable external transmit on 4 bytes.
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_ENABLE_EXTERNAL_TRANSMIT, 4);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/external_transmit
```

###### Examples
```
echo 1 > /sys/class/serialfc/serialfc0/settings/external_transmit
```


## Disable
### IOCTL
```c
IOCTL_FASTCOM_DISABLE_EXTERNAL_TRANSMIT
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_DISABLE_EXTERNAL_TRANSMIT);
```

### Sysfs
```
/sys/class/serialfc/serialfc*/settings/external_transmit
```

###### Examples
```
echo 0 > /sys/class/serialfc/serialfc0/settings/external_transmit
```


### Additional Resources
- Complete example: [`examples\external_transmit.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/external_transmit.c)
