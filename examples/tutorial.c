#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <serialfc.h>

int main(void)
{
    int fd = 0, serialfc_fd = 0;
    char odata[] = "Hello world!";
    char idata[20];
    int file_status;
    unsigned type;
    struct termios tios;

    /* Open port 0 (ttyS4) */
    fd = open("/dev/ttyS4", O_RDWR | O_NDELAY);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    
    serialfc_fd = open("/dev/serialfc0", O_RDWR);
    if (serialfc_fd == -1) {
        perror("open");
        close(fd);
        return EXIT_FAILURE;
    }
    
    /* Turn off O_NDELAY now that we have the port open */
    file_status = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, file_status & ~O_NDELAY);

    ioctl(serialfc_fd, IOCTL_FASTCOM_GET_CARD_TYPE, &type);
    if (type == SERIALFC_CARD_TYPE_PCIE) {
        int status = 0;

        ioctl(fd, TIOCMGET, &status);
        status &= ~TIOCM_DTR; /* Set DTR to 1 (transmitter always on, 422) */
        ioctl(fd, TIOCMSET, &status);
    }


    /* Configure serial settings */
    tcgetattr(fd, &tios);

    tios.c_iflag = IGNBRK;
    tios.c_oflag = 0;
    tios.c_cflag = CS8 | CREAD | CLOCAL;
    tios.c_lflag &= ~(ICANON | ISIG | ECHO);

    cfsetospeed(&tios, B115200);
    cfsetispeed(&tios, 0); /* Set ispeed = ospeed */

    tcsetattr(fd, TCSANOW, &tios);


    /* Send "Hello world!" text */
    write(fd, odata, sizeof(odata));

    /* Read the data back in (with our loopback connector) */
    read(fd, idata, sizeof(idata));

    fprintf(stdout, "%s\n", idata);

    close(fd);

    return EXIT_SUCCESS;
}
