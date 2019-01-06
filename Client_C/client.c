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

char * fileName;

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

struct HostInfo hostInfo;
int sendSocket, recvSocket;
struct sockaddr_in sendSocket_addr, recvSocket_addr;
socklen_t sendSocket_len, recvSocket_len;

char * getListFile()
{
	char * temp = (char*)malloc(100 * sizeof(char*));
	struct dirent * de = NULL;
	DIR * d = NULL;

	d = opendir("Shared");
	if(d == NULL)
		error("Couldn't open directory");

	while(de = readdir(d))
	{
		if(de->d_name[0] != '.')
		{
			temp = strcat(strcat(temp, de->d_name), " ");
		}
	}
	closedir(d);
	return temp;
}

void sendHostInfoToServer(int sock)
{
    char * temp = (char*) malloc(100 * sizeof(char *));     // hostInfo Data
	hostInfo.hostName = (char*) malloc(100 * sizeof(char *));
	hostInfo.hostIPAddress = (char*) malloc(100 * sizeof(char *));
	hostInfo.listFile = (char*) malloc(100 * sizeof(char *));
    printf("Host Name: ");
    scanf("%s", hostInfo.hostName);
	temp = strcat(strcat(temp, "\r"), hostInfo.hostName);
    printf("Host IP Address: ");
    scanf("%s", hostInfo.hostIPAddress);
	temp = strcat(strcat(temp, ","), hostInfo.hostIPAddress);
	hostInfo.listFile = getListFile();
	temp = strcat(strcat(temp, ","), hostInfo.listFile);
	int dataLength = strlen(temp);
	printf("%d\n", dataLength);
	//write(sockfd, dataLength, sizeof(dataLength));
	write(sock, temp, dataLength);
	printf("%s\n",temp);
}

void downloadFile(int sock)
{
	printf("Input file name to download: ");
	scanf("%s", fileName);
	write(sock, fileName, sizeof(fileName));
	if(strcmp(fileName, "QUIT") == 0)
	{
		close(sock);
	}
	else
	{
		char * listHost = (char*) malloc(100*sizeof(char*));
		char * hostName = (char*) malloc(100*sizeof(char*));
		char * hostIPAddress = (char*) malloc(100*sizeof(char*));
		read(sock, listHost, 1024);
		printf("List Host: %s", listHost);
		printf("Input host name you want to download file: ");
		scanf("%s", hostName);
		printf("Host IP Address: ");
		scanf("%s", hostIPAddress);

		// recvSocket()
		recvSocket = socket(AF_INET, SOCK_STREAM, 0);
		if(recvSocket < 0)
			error("Error Opening RecvSocket\n");
		struct sockaddr_in recvSocket_addr;
		socklen_t recvSocket_len = sizeof(recvSocket_addr);
		bzero(&recvSocket_addr, recvSocket_len);

		recvSocket_addr.sin_family = AF_INET;
		recvSocket_addr.sin_port = htons(1234);
		recvSocket_addr.sin_addr.s_addr = inet_addr(hostIPAddress);

		if(connect(recvSocket, (struct sockaddr*)&recvSocket_addr, recvSocket_len))
			error("Error Connecting");
		write(sendSocket, hostName, strlen(hostName));
		char * temp = malloc(100*sizeof(char*));
		read(recvSocket, &temp, sizeof(temp));
		printf("%s\n", temp);
		downloadFile(sock);

	}
}

void initSendSocket()
{
	// sendSocket()
	sendSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(sendSocket < 0)
		error("Error Opening sendSocket\n");
	sendSocket_len = sizeof(sendSocket_addr);
	bzero((char*) &sendSocket_addr, sendSocket_len);

	// set sendSocket opt
	int optval = 1;
	if((setsockopt(sendSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval))) < 0)
		error("Error Set Opt SendSocket\n");

	// bind() sendSocket
	sendSocket_addr.sin_family = AF_INET;
	sendSocket_addr.sin_port = htons(1234);
	sendSocket_addr.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr(hostInfo.hostIPAddress);
	if(bind(sendSocket, (struct sockaddr*)&sendSocket_addr, sendSocket_len) < 0)
		error("Error Binding SendSocket");
	
	// listen() sendSocket
	if(listen(sendSocket, 5) < 0)
		error("Error Listening Socket\n");
}

// void sendFile(int sock)
// {
// 	FILE * file;
// 	file = fopen(fileName, "rb");
// }

int main()
{
    int sockfd, connfd;
    struct sockaddr_in serv_addr;
	fileName = (char*) malloc(100 * sizeof(char*));

    // socket()
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		error("Error Openning Socket\n");
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
        printf("Connected Success !\n");
		sendHostInfoToServer(sockfd);
		initSendSocket();
		downloadFile(sockfd);
    }

	// close()
	close(sockfd);
	printf("\nClosed Connection !\n");
	return 0;
}
