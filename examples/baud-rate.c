#include <fcntl.h> /* open, O_RDWR */
#include <stdio.h> /* printf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* close */
#include <serialfc.h> /* IOCTL_FASTCOM_* */

int main(void)
{
    int fd = 0;
    unsigned type;
    int status = 0;
    unsigned long baud_rate = 0;
    unsigned long baud_rate2 = 0;

    fd = open("/dev/serialfc0", O_RDWR);

    ioctl(fd, IOCTL_FASTCOM_GET_CARD_TYPE, &type);

    status = ioctl(fd, IOCTL_FASTCOM_GET_BAUD_RATE, &baud_rate);
    if (status < 0)
    {
       printf("Error getting baud rate: %d", status);
       exit(status);
    }

    printf("Original Baud Rate = %lu\n", baud_rate);

    baud_rate = 8000000ul;
    printf("New Baud Rate = %lu\n", baud_rate);
    status = ioctl(fd, IOCTL_FASTCOM_SET_BAUD_RATE, baud_rate);
    if (status < 0)
    {
       printf("Error setting baud rate: %d", status);
       exit(status);
    }

    ioctl(fd, IOCTL_FASTCOM_GET_BAUD_RATE, &baud_rate2);
    printf("Actual achieved baudrate = %lu\n", baud_rate2);

    close(fd);

    return 0;
}
