#include <fcntl.h> /* open, O_RDWR */
#include <unistd.h> /* close */
#include <serialfc.h> /* IOCTL_FASTCOM_* */

int main(void)
{
    int serialfc_fd = 0;
    unsigned status = 0;

    serialfc_fd = open("/dev/serialfc0", O_RDWR);

    ioctl(serialfc_fd, IOCTL_FASTCOM_GET_IDLE_LOW, &status);
	
    ioctl(serialfc_fd, IOCTL_FASTCOM_ENABLE_IDLE_LOW);

    ioctl(serialfc_fd, IOCTL_FASTCOM_DISABLE_IDLE_LOW);

    close(serialfc_fd);

    return 0;
}
