#include <fcntl.h> /* open, O_RDWR */
#include <unistd.h> /* close */
#include <serialfc.h> /* IOCTL_FASTCOM_* */

int main(void)
{
    int fd = 0;
    unsigned length;

    fd = open("/dev/serialfc0", O_RDWR);

    ioctl(fd, IOCTL_FASTCOM_GET_FRAME_LENGTH, &length);

    ioctl(fd, IOCTL_FASTCOM_SET_FRAME_LENGTH, 1);

    close(fd);

    return 0;
}
