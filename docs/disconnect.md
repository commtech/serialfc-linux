# Disconnect


###### Support
| Code             | Version
| ---------------- | --------
| `serialfc-linux` | `v2.0.0`


## Disconnect
The Linux [`close`](http://linux.die.net/man/3/close)
is used to disconnect from the port.


###### Examples
```c
#include <unistd.h>
...

close(fd);
```


### Additional Resources
- Complete example: [`examples\tutorial.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/tutorial.c)
