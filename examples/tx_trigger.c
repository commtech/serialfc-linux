#include <fcntl.h> /* open, O_RDWR */
#include <unistd.h> /* close */
#include <serialfc.h> /* IOCTL_FASTCOM_* */

int main(void)
{
    int fd = 0;
    unsigned level;

    fd = open("/dev/serialfc0", O_RDWR);

    ioctl(fd, IOCTL_FASTCOM_GET_TX_TRIGGER, &level);

    ioctl(fd, IOCTL_FASTCOM_SET_TX_TRIGGER, 32);

    close(fd);

    return 0;
}
