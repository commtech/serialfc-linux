#include <fcntl.h> /* open, O_RDWR */
#include <unistd.h> /* close */
#include <serialfc.h> /* IOCTL_FASTCOM_* */

/* #define ASYNC_PCIE */

int main(void)
{
    int ttys_fd = 0;
    int serialfc_fd = 0;
    unsigned status = 0;
#ifdef ASYNC_PCIE
    int mode = 0;
#endif

    ttys_fd = open("/dev/ttyS4", O_RDWR | O_NDELAY);
    serialfc_fd = open("/dev/serialfc0", O_RDWR);

    ioctl(serialfc_fd, IOCTL_FASTCOM_GET_RS485, &status);

    /* Enable RS485 */
#ifdef ASYNC_PCIE
    ioctl(ttys_fd, TIOCMGET, &mode);
    mode |= 0x2000;
    ioctl(ttys_fd, TIOCMSET, &mode);
#endif
    ioctl(serialfc_fd, IOCTL_FASTCOM_ENABLE_RS485);


    /* Disable RS485 */
#ifdef ASYNC_PCIE
    ioctl(ttys_fd, TIOCMGET, &mode);
    mode &= ~0x2000;
    ioctl(ttys_fd, TIOCMSET, &mode);
#endif
    ioctl(serialfc_fd, IOCTL_FASTCOM_DISABLE_RS485);

    close(serialfc_fd);
    close(ttys_fd);

    return 0;
}
