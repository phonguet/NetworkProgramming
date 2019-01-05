// Group Name : Nâu Nêm
// BTL : Xây dựng ứng dụng chia sẻ file ngang hàng sử dụng mô hình lai

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>          // close(), read(), write()
#include<sys/socket.h>      // socket(), bind(), listen(), accept()
#include<arpa/inet.h>       // htonl()
#include<netinet/in.h>
#include<pthread.h>
#include<errno.h>
#include<ctype.h>
#include<sys/wait.h>
//#include<libr/r_util/r_json.h>

#define MAX_HOST 10

// Information of Host connected to server
typedef struct HostInfo
{
    char * hostName;
    char * hostIPAddress;
    char * listFile;
};

static int countHost = 0;
struct HostInfo DATAHOST[MAX_HOST];
char * nullptr = NULL;

void error(const char * msg)
{
	perror(msg);
	exit(1);
}

// server có 1 socket lắng nghe, mỗi khi client kết nối đến thì
// server sẽ tạo ra 1 luồng để xử lí yêu cầu của client

void getHostInfo(int sock)
{
	struct HostInfo hostInfo;
    char buffer_recv[8192];
	char * dataHost = (char*) malloc(100 * sizeof(char *));
    bzero(buffer_recv, sizeof(buffer_recv));
	read(sock, buffer_recv, sizeof(buffer_recv));
	hostInfo.hostName = strtok(buffer_recv, ",");
	hostInfo.hostIPAddress= strtok(NULL, ",");
	hostInfo.listFile = strtok(NULL, ",");
	printf("List file: %s\n",hostInfo.listFile);
	//printf("Host Name: %s\n", hostInfo.hostName);
	DATAHOST[countHost++] = hostInfo;
	//printf("Host Name: %s\n", DATAHOST[countHost++].hostName);
	//printf("%p\n", &hostInfo[countHost]);
}

void responseToHost(int sock)
{
	char * fileName = (char*) malloc(100 * sizeof(char*));
	read(sock, fileName, sizeof(fileName));
	if(strcmp(fileName, "QUIT") == 0)
		close(sock);

	char * listHost = NULL;
	listHost = (char*) malloc(100 * sizeof(char*)); // list host include file name download

	// return list host have file request
	for(int i=0;i<countHost;i++)
	{
		if(strstr(DATAHOST[i].listFile, fileName) != nullptr)
		{
			listHost = strcat(strcat(listHost, "\n"), DATAHOST[i].hostName);
		}
	}

	//printf("ListHost: %s\n", listHost);
	write(sock, listHost, strlen(listHost));
}

static void * doit(void * arg)
{
    printf("\nThread ID: %d is created.\n", *(int*)arg);
    int connfd;
    connfd = *((int*)arg);
    free(arg);

	// Xoa vung bo nho cua luong con ra khoi he thong
	// sau khi ket thuc xu ly
    pthread_detach(pthread_self());
	getHostInfo(connfd);
	// for(int i=0;i<countHost;i++)
	// {
	// 	printf("%s\n", DATAHOST[i].hostName);
	// }
	printf("Length DATAHOST: %d\n",countHost);
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
	pthread_t tid;
	int *iptr;
	int serv_len;

	// socket()
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
		error("Error Opening socket");
	serv_len = sizeof(serv_addr);
	bzero((char*) &serv_addr, serv_len);

	// set socket opt
	int optval = 1;	
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0)
		error("Set OptSocket Error");

	// bind()
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(9090);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(listenfd, (struct sockaddr*) &serv_addr, serv_len) < 0)
		error("Error Binding Socket");

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
		pthread_create(&tid, NULL, &doit, (void*)iptr);
	}
	
	// close()
	close(listenfd);
	printf("Closed Connection !\n");
	return 0;
}
