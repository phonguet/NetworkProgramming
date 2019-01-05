// Group Name : Nâu Nêm
// BTL : Xây dựng ứng dụng chia sẻ file ngang hàng sử dụng mô hình lai

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<dirent.h>

char * fileName ;

void error(const char * msg)
{
	perror(msg);
	exit(1);
}

typedef struct HostInfo
{
    char * hostName;
    char * hostIPAddress;
    char * listFile;
};

char * getListFile()
{
	char * temp = (char*)malloc(100 * sizeof(char*));
	struct dirent * de = NULL;
	DIR * d = NULL;
	d = opendir("Share");
	if(d == NULL)
		error("Couldn't open directory.\n");
	while(de = readdir(d))
	{
		if(de->d_name[0] != '.'){
			temp = strcat(strcat(temp, de->d_name), " ");
		}
	}
	closedir(d);
	return temp;
}

void sendHostInfoToServer(int sock)
{
	struct HostInfo hostInfo;
    char * temp = (char*) malloc(100 * sizeof(char *));     // hostInfo Data
	hostInfo.hostName = (char*) malloc(100 * sizeof(char *));
	hostInfo.hostIPAddress = (char*) malloc(100 * sizeof(char *));
	hostInfo.listFile = (char*) malloc(100 * sizeof(char *));
    printf("Host Name: ");
    scanf("%s", hostInfo.hostName);
	// temp = strcat(strcat(temp, "\r"), hostInfo.hostName);
	temp = strcat(temp, hostInfo.hostName);
    printf("Host IP Address: ");
    scanf("%s", hostInfo.hostIPAddress);
	temp = strcat(strcat(temp, ","), hostInfo.hostIPAddress);
	hostInfo.listFile = getListFile();
	temp = strcat(strcat(temp, ", "), hostInfo.listFile);
	int dataLength = strlen(temp);
	printf("%d\n", dataLength);
	write(sock, temp, dataLength);
	printf("%s\n",temp);
}

void downloadFile(int sock)
{
	printf("Input file name to download: ");
	scanf("%s", fileName);
	write(sock, fileName, sizeof(fileName));
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in serv_addr;
	fileName = (char*) malloc(100 * sizeof(char*));

    // socket()
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		error("Error Openning Socket");
	bzero(&serv_addr, sizeof(serv_addr));

	// connect()
	char * serv_IPAddr = (char *) malloc(100 *sizeof(char*));
	printf("Server IP Address: ");
	scanf("%s",serv_IPAddr);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(9090);
	serv_addr.sin_addr.s_addr = inet_addr(serv_IPAddr);

	if((connfd = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr))) < 0)
		error("Error Connecting");	
    else
    {
		int route_send, route_down;
        printf("Connected Success !\n");
		printf("You have some files:\n");

		// Show cac file dang co trong thu muc
		DIR *d;
  		struct dirent *dir;
 		d = opendir("Share");
  		if (d) {
    		while ((dir = readdir(d)) != NULL) {
      			if(dir->d_name[0] != '.'){
					printf("%s, ", dir->d_name);
				  }
				  
    		}
    		closedir(d);
  		}

		printf("\nDo you want to send this list to Index Server?\n");
		printf("Type '1' if Yes, '0' if No.\n ");
		scanf("%d", &route_send);
		// printf("\n%d \n", route_send);
		if(route_send == 1){
			// printf("\n%d \n", route_send);
			sendHostInfoToServer(sockfd);
		}

		printf("\nDo you want to download any file?\n");
		printf("Type '1' if Yes, '0' if No.\n ");
		scanf("%d", &route_down);
		if(route_down == 1){
			downloadFile(sockfd);
		}
		
    }
    
	close(sockfd);
	printf("\nClosed Connection !\n");
	return 0;
}
