#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if.h> /* for struct ifreq */
#define MAXLINE 50
 
typedef struct sockaddr SA;
 
int main(int argc, char *argv)
{
	int sockfd,ret;
	struct sockaddr_in targetaddr;
	char buf_snd[MAXLINE] = "Hello, I am 48!";
	struct ifreq interface;
	int broadcast;

	/* create the socket */
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket failed!");
		exit(-1);
	}
	
	/* set the socket as broadcast */
	broadcast = 1;
	
	if((ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast))) < 0)
	{
		perror("setsockopt failed");
		exit(-1);
	}
	
	/* init the Network information architecture  */
	bzero(&targetaddr, sizeof(targetaddr));
	targetaddr.sin_family = AF_INET;
	targetaddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	targetaddr.sin_port = htons(53);/* dport */
	
	/*
	 SO_BINDTODEVICE
              Bind this socket to a particular device like “eth0”, as specified in the passed interface name.  If  the
              name  is an empty string or the option length is zero, the socket device binding is removed.  The passed
              option is a variable-length null-terminated interface name string with the maximum size of IFNAMSIZ.  If
              a socket is bound to an interface, only packets received from that particular interface are processed by
              the socket.  Note that this works only for some socket types, particularly AF_INET sockets.  It  is  not
              supported for packet sockets (use normal bind(2) there).

              Before  Linux  3.8,  this socket option could be set, but could not retrieved with getsockopt(2).  Since
              Linux 3.8, it is readable.  The optlen argument should contain the buffer size available to receive  the
              device name and is recommended to be IFNAMSZ bytes.  The real device name length is reported back in the
              optlen argument.

	*/	
	strncpy(interface.ifr_name, "lo", IFNAMSIZ);	
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface)) < 0) 
	{
		/*error handling*/
		printf("bind device failed\n");
		return -1;
     	}
	

	while(1)
	{
		sleep(1);
		if((ret = sendto(sockfd, buf_snd, sizeof(buf_snd), 0, (SA *)&targetaddr, sizeof(targetaddr))) < 0)
		{
			perror("sendto failed");
			exit(-1);
		}
		printf("sendto (broadcast) successed\n");
	}

	return 0;
}
 

