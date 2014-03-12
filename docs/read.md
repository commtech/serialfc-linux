# Read

###### Code Support
| Code | Version |
| ---- | ------- |
| serialfc-linux | 2.0.0 |


## Read
### Function
The Linux [`read`](http://linux.die.net/man/3/read) is used to read data from the port.

###### Examples
```c
#include <unistd.h>
...

char idata[20] = {0};
unsigned bytes_read;

bytes_read = read(fd, idata, sizeof(idata));
```

### Command Line
###### Examples
```
cat /dev/serialfc0
```


### Additional Resources
- Complete example: [`examples/tutorial.c`](../examples/tutorial.c)
