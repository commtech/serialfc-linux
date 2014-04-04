# Card Type

###### Code Support
| Code | Version |
| ---- | ------- |
| serialfc-linux | 2.0.0 |

###### Card Support
| Card Family | Supported |
| ----------- |:-----:|
| FSCC (16C950) | Yes |
| Async-335 (17D15X) | Yes |
| Async-PCIe (17V35X) | Yes |


## Get
```c
IOCTL_FASTCOM_GET_CARD_TYPE
```

| Card Type | Value | Description |
| --------- | -----:| ----------- |
| `SERIALFC_CARD_TYPE_PCI` | 0 | Async-335 (17D15X) |
| `SERIALFC_CARD_TYPE_PCIE` | 1 | Async-PCIe (17V35X) |
| `SERIALFC_CARD_TYPE_FSCC` | 2 | FSCC (16C950) |
| `SERIALFC_CARD_TYPE_UNKNOWN` | 3 | Unknown |

###### Examples
```
#include <serialfc.h>
...

unsigned type;

ioctl(fd, IOCTL_FASTCOM_GET_CARD_TYPE, &type);
```


### Additional Resources
- Complete example: [`examples/card-type.c`](../examples/card-type.c)
