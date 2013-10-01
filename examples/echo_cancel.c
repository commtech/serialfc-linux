#include <fcntl.h> /* open, O_RDWR */
#include <unistd.h> /* close */
#include <serialfc.h> /* IOCTL_FASTCOM_* */

int main(void)
{
    int fd = 0;
    unsigned status = 0;

    fd = open("/dev/serialfc0", O_RDWR);

    ioctl(fd, IOCTL_FASTCOM_GET_ECHO_CANCEL, &status);

    ioctl(fd, IOCTL_FASTCOM_DISABLE_ECHO_CANCEL);
    ioctl(fd, IOCTL_FASTCOM_ENABLE_ECHO_CANCEL);

    close(fd);

    return 0;
}
