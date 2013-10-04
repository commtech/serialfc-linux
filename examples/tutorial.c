#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

//#define ASYNC_PCIE

int main(void)
{
    int fd = 0;
    char odata[] = "Hello world!";
    char idata[20];

    /* Open port 0 (ttyS4) */
    fd = open("/dev/ttyS4", O_RDWR);

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

#ifdef ASYNC_PCIE
    {
        int status = 0;

        ioctl(fd, TIOCMGET, &status);
        status &= ~TIOCM_DTR; /* force DTR active = RS422 */
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
