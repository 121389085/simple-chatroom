#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "client.h"
#include "main_face.h"

int main()
{
    int sockfd = connect_server("192.168.10.128",8888);
    main_interface(sockfd);
    close(sockfd);
    return 0;
}