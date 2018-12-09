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
//#include<libr/r_util/r_json.h>

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
	while(read(sock, buffer_recv, sizeof(buffer_recv)) > 0)
	{

	}
	hostInfo.hostName = strtok(buffer_recv, ",");
	hostInfo.hostIPAddress= strtok(NULL, ",");
	hostInfo.listFile = strtok(NULL, ",");
	printf("%s\n",hostInfo.listFile);
	return hostInfo;
}

void responseToHost()
{

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
	if(countHost == 1)
	{
		DATAHOST = &hostInfo;
		printf("%s\n",DATAHOST->hostName);
	}
	else
	{
		*DATAHOST++ = hostInfo;
		printf("%s\n",DATAHOST->hostName);
	}

    close(connfd);
	countHost--;
    return NULL;
}

int main()
{
    int listenfd;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t cli_len;
	pthread_t tid;
	int *iptr;

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
	serv_addr.sin_port = htons(9090);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
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