# Connect

Opening a handle using this API will only give you access to the
fastcom functionality of the card. You will need to use the ttyS ports
if you would like to use read/write.

###### Support
| Code             | Version
| ---------------- | --------
| `serialfc-linux` | `v2.0.0`


## Connect
The Linux [`open`](http://linux.die.net/man/3/open)
is used to connect to the port.

###### Examples
Connect to port 0.
```c
#include <fcntl.h>
...

int fd;

fd = open("/dev/serialfc0", O_RDWR);
```


### Additional Resources
- Complete example: [`examples\tutorial.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/tutorial.c)
