# RS485

###### Code Support
| Code | Version |
| ---- | ------- |
| serialfc-linux | 2.3.2 |

###### Card Support
| Card Family | Supported |
| ----------- |:-----:|
| FSCC (16C950) | Yes |
| Async-335 (17D15X) | No |
| Async-PCIe (17V35X) | No |


## Get
### IOCTL
```c
IOCTL_FASTCOM_GET_IDLE_LOW
```

###### Examples
```c
#include <serialfc.h>
...

unsigned status;

ioctl(fd, IOCTL_FASTCOM_GET_IDLE_LOW, &status);
```



## Enable
### IOCTL
```c
IOCTL_FASTCOM_ENABLE_IDLE_LOW
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_FASTCOM_ENABLE_IDLE_LOW);
```


## Disable
### IOCTL
```c
IOCTL_FASTCOM_ENABLE_IDLE_LOW
```

###### Examples
```c
#include <serialfc.h>
...

ioctl(fd, IOCTL_ENABLE_IDLE_LOW);
```


### Additional Resources
- Complete example: [`examples/idle_low.c`](../examples/idle_low.c)
