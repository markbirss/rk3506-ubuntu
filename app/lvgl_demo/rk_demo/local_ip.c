#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *get_local_ip(void)
{
    int fd, intrface, retn = 0;
    struct ifreq buf[INET_ADDRSTRLEN];
    struct ifconf ifc;
    char ip[INET_ADDRSTRLEN];
    char *ip_out = NULL;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;
        if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
        {
            intrface = ifc.ifc_len / sizeof(struct ifreq);
            ip_out = calloc(intrface, 20);
            snprintf(ip_out, intrface * 20, "IP:");
            while (intrface-- > 0)
            {
                if (!(ioctl(fd, SIOCGIFADDR, (char *)&buf[intrface])))
                {
                    inet_ntop(AF_INET,
                              &((struct sockaddr_in *)(&buf[intrface].ifr_addr))->sin_addr,
                              ip, INET_ADDRSTRLEN);
                    if (strcmp(ip, "127.0.0.1") != 0)
                    {
                        strcat(ip_out, ip);
                        if (intrface > 1)
                            strcat(ip_out, ",");
                    }
                }
            }
        }
        close(fd);
    }

    return ip_out;
}

