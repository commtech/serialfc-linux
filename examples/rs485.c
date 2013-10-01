#include <fcntl.h> /* open, O_RDWR */
#include <unistd.h> /* close */
#include <serialfc.h> /* IOCTL_FASTCOM_* */

int main(void)
{
    int fd = 0;
    unsigned status = 0;

    fd = open("/dev/serialfc0", O_RDWR);

    ioctl(fd, IOCTL_FASTCOM_GET_RS485, &status);

    ioctl(fd, IOCTL_FASTCOM_DISABLE_RS485);
    ioctl(fd, IOCTL_FASTCOM_ENABLE_RS485);

    close(fd);

    return 0;
}
