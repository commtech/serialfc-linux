#include <fcntl.h> /* open, O_RDWR */
#include <unistd.h> /* close */
#include <serialfc.h> /* IOCTL_FASTCOM_* */
#include "calculate-clock-bits.h"

int main(void)
{
    int fd = 0;
    unsigned type;

    fd = open("/dev/serialfc0", O_RDWR);

    ioctl(fd, IOCTL_FASTCOM_GET_CARD_TYPE, &type);

    if (type == SERIALFC_CARD_TYPE_FSCC) {
        unsigned char clock_bits[20];
        calculate_clock_bits(18432000, 10, clock_bits);
        ioctl(fd, IOCTL_FASTCOM_SET_CLOCK_BITS, &clock_bits);
    }
    else if (type == SERIALFC_CARD_TYPE_PCI) {
        uint32_t clock_bits = 0;
        calculate_clock_bits_335(18432000, &clock_bits);
        ioctl(fd, IOCTL_FASTCOM_SET_CLOCK_BITS, &clock_bits);
    }

    close(fd);

    return 0;
}
