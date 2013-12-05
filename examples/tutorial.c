#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

/* #define ASYNC_PCIE */

int main(void)
{
    int fd = 0;
    char odata[] = "Hello world!";
    char idata[20];
    int file_status;

    /* Open port 0 (ttyS4) */
    fd = open("/dev/ttyS4", O_RDWR | O_NDELAY);

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    /* Turn off O_NDELAY now that we have the port open */
    file_status = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, file_status & ~O_NDELAY);

#ifdef ASYNC_PCIE
    {
        int status = 0;

        ioctl(fd, TIOCMGET, &status);
        status &= ~TIOCM_DTR; /* Set DTR to 1 (transmitter always on, 422) */
        ioctl(fd, TIOCMSET, &status);
    }
#endif

    /* Send "Hello world!" text */
    write(fd, odata, sizeof(odata));

    /* Read the data back in (with our loopback connector) */
    read(fd, idata, sizeof(idata));

    fprintf(stdout, "%s\n", idata);

    close(fd);

    return EXIT_SUCCESS;
}
