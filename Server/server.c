// Group Name : Nâu Nêm
// BTL : Xây dựng ứng dụng chia sẻ file ngang hàng sử dụng mô hình lai

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
// Cac include danh cho AF_INET
#include <netinet/in.h>
#include <arpa/inet.h>

// Information of Host connected to server
typedef struct HostInfo
{
    char * hostName;
    char * hostIPAddress;
    char * listFile;
};

static int countHost = 0;
struct HostInfo * DATAHOST;

void error(const char * msg)
{
	perror(msg);
	exit(1);
}

// server có 1 socket lắng nghe, mỗi khi client kết nối đến thì
// server sẽ tạo ra 1 luồng để xử lí yêu cầu của client

struct HostInfo getHostInfo(int sock)
{
	struct HostInfo hostInfo;
    char buffer_recv[8192];
	char * dataHost = (char*) malloc(100 * sizeof(char *));
    bzero(buffer_recv, sizeof(buffer_recv));
	read(sock, buffer_recv, sizeof(buffer_recv));
	hostInfo.hostName = strtok(buffer_recv, ", ");
	hostInfo.hostIPAddress= strtok(NULL, ", ");
	hostInfo.listFile = strtok(NULL, ", ");
	while(hostInfo.listFile != NULL && hostInfo.listFile != EOF ){
		printf("\n%s",hostInfo.listFile);
		hostInfo.listFile = strtok(NULL, ", ");
	}
	
	return hostInfo;
}

void responseToHost(int sock)
{
	struct HostInfo * data = DATAHOST;
	char * listHost; // list host include file name download
	char * fileName;
	read(sock, fileName, sizeof(fileName));
	printf("%s\n", fileName);
}

static void * doit(void * arg)
{
    printf("\nThread ID: %d is created.\n", *(int*)arg);
	countHost++;
    int connfd;
    connfd = *((int*)arg);
    free(arg);
    pthread_detach(pthread_self());
	struct HostInfo hostInfo =  getHostInfo(connfd);
	DATAHOST = &hostInfo;
	printf("%s\n",DATAHOST->hostName);
	*DATAHOST++;
	responseToHost(connfd);
    close(connfd);
	//countHost--;
    return NULL;
}

int main()
{
    int listenfd;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t cli_len;
	int *iptr;
	int port = 9090;

	// socket()
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
		error("Error Opening socket");
	bzero((char*) &serv_addr, sizeof(serv_addr));

	// get socket opt
	int optval = 1;	
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0)
		error("Get OptSocket Error");

	// bind()
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int bindCheck = bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	if (bindCheck < 0) {
		perror("Error Binding Socket. \n");
		return 1;
	}
	printf("Server is listening at port %d . . .\n", ntohs(serv_addr.sin_port));
	
	// listen()
	if(listen(listenfd, 10) < 0)
		error("Error Listening Socket");

	// accept()
	for( ; ; )
	{
		cli_len = sizeof(cli_addr);
		iptr = (int*)malloc(100 * sizeof(int*));
		*iptr =  accept(listenfd, (struct sockaddr *) &cli_addr, &cli_len);
		pthread_t tid;
		pthread_create(&tid, NULL, &doit, iptr);
	}
	
	// close()
	close(listenfd);
	printf("Closed Connection !\n");
	return 0;
}
