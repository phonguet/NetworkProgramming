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

char * DATAHOST;

void error(const char * msg)
{
	perror(msg);
	exit(1);
}

// server có 1 socket lắng nghe, mỗi khi client kết nối đến thì
// server sẽ tạo ra 1 luồng để xử lí yêu cầu của client

char * getHostInfo(int sock)
{
    char buffer_recv[8192];
	//int dataLength = 0;
	//read(sock, &dataLength, sizeof(dataLength));
	//printf("Data Length: %d\n", dataLength);
	char * dataHost = (char*) malloc(100 * sizeof(char *));
    bzero(buffer_recv, sizeof(buffer_recv));
	while(read(sock, buffer_recv, sizeof(buffer_recv)) > 0)
	{
		dataHost = strcat(strcat(dataHost, "\n"), buffer_recv);
	}
	printf(dataHost);
	return dataHost;
}

void responseToHost()
{

}

static void * doit(void * arg)
{
    printf("\nThread ID: %d is created.\n", *(int*)arg);
    int connfd;
    connfd = *((int*)arg);
    free(arg);
    pthread_detach(pthread_self());
	//DATAHOST = getHostInfo(connfd);
	DATAHOST = strcat(strcat(DATAHOST, "\n"), getHostInfo(connfd));
	//printf(DATAHOST);
    close(connfd);
    return NULL;
}

int main()
{
    int listenfd;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t cli_len;
	pthread_t tid;
	int *iptr;
	DATAHOST = (char *) malloc(100 * sizeof(char*));

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