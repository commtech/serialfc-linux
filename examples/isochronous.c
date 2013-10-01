#include <fcntl.h> /* open, O_RDWR */
#include <unistd.h> /* close */
#include <serialfc.h> /* IOCTL_FASTCOM_* */

int main(void)
{
    int fd = 0;
    unsigned status = 0;

    fd = open("/dev/serialfc0", O_RDWR);

    ioctl(fd, IOCTL_FASTCOM_GET_ISOCHRONOUS, &status);

    ioctl(fd, IOCTL_FASTCOM_DISABLE_ISOCHRONOUS);

    /* Enable transmit using external RI# */
    ioctl(fd, IOCTL_FASTCOM_ENABLE_ISOCHRONOUS, 0);

    close(fd);

    return 0;
}
