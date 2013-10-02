# Write


###### Support
| Code             | Version
| ---------------- | --------
| `serialfc-linux` | `v2.0.0`


## Write
The Linux [`write`](http://linux.die.net/man/3/write)
is used to write data to the port.

###### Examples
### Function
```c
#include <unistd.h>
...

char odata[] = "Hello world!";
unsigned bytes_written;

bytes_read = write(fd, odata, sizeof(odata));
```

### Command Line
###### Examples
```
echo "Hello world!" > /dev/serialfc0
```


### Additional Resources
- Complete example: [`examples\tutorial.c`](https://github.com/commtech/serialfc-linux/blob/master/examples/tutorial.c)
