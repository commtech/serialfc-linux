#include <fcntl.h> /* open, O_RDWR */
#include <unistd.h> /* close */
#include <serialfc.h> /* IOCTL_FASTCOM_* */

int main(void)
{
    int fd = 0;
    unsigned rate;

    fd = open("/dev/serialfc0", O_RDWR);

    ioctl(fd, IOCTL_FASTCOM_GET_SAMPLE_RATE, &rate);

    ioctl(fd, IOCTL_FASTCOM_SET_SAMPLE_RATE, 16);

    close(fd);

    return 0;
}
