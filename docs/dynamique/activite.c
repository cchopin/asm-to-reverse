#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int main(void){
    char buf[64] = {0};
    int fd = open("/etc/hostname", O_RDONLY);
    if (fd >= 0){ ssize_t r = read(fd, buf, sizeof(buf)-1); (void)r; close(fd); }
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in a; memset(&a,0,sizeof(a));
    a.sin_family=AF_INET; a.sin_port=htons(8080);
    inet_pton(AF_INET,"127.0.0.1",&a.sin_addr);
    connect(s,(struct sockaddr*)&a,sizeof(a)); close(s);
    printf("fini\n");
    return 0;
}
