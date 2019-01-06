#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

pthread_mutex_t	mutex_ndone = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t	mutex_n_sending = PTHREAD_MUTEX_INITIALIZER;

int numberClient=0;

struct Client{
    int IpAddr;
    long port;
    char listFile[100][100];
    long listFileSize[100];
    int numberFile;
};

struct Client client[100];

void sendAccept(int *fd) {
    int message = 1;
    write(*fd, &message, sizeof(message));
}

void recvlistFile(int *fd) {
    int n_bytes;
    int nFile, fileLen;
    char fileName[100];
    long port;
    long fileSize;

    read(*fd,&port,sizeof(port));
    printf("Ket noi voi End Host tai cong: %ld\n",port);
    int flag = 1;
    for(int i=0; i<numberClient; i++){
        if(client[i].port==port){
            n_bytes = read(*fd, &nFile, sizeof(int));
            printf("So file hien co : %d\n", nFile);
            client[i].numberFile=nFile;
            for (int j = 0; j < nFile; j++) {
                int fileNameLength;
                read(*fd,&fileNameLength,sizeof(fileNameLength));
                n_bytes = read(*fd, &client[i].listFile[j], fileNameLength);
                client[i].listFile[j][n_bytes]=0;
                n_bytes = read(*fd, &fileSize, sizeof(fileSize));
                printf("File: %s :\t %d bytes.\n", client[i].listFile[j], client[i].listFileSize);
                client[i].listFileSize[j]=fileSize;
            }
            flag = 0;
            break;
        }
    }
    if(flag==1){
        client[numberClient-1].port=port;
        n_bytes = read(*fd, &nFile, sizeof(int));
        printf("So file hien co : %d\n", nFile);
        client[numberClient-1].numberFile=nFile;
        for (int i = 0; i < nFile; i++) {
            int fileNameLength;
            read(*fd,&fileNameLength,sizeof(fileNameLength));
            n_bytes = read(*fd, &client[numberClient-1].listFile[i], fileNameLength);
            client[numberClient-1].listFile[i][n_bytes]=0;
            n_bytes = read(*fd, &fileSize, sizeof(fileSize));
            printf("%d. %s :\t %ld bytes.\n", i+1, client[numberClient-1].listFile[i], fileSize);
            client[numberClient-1].listFileSize[i]=fileSize;
        }
    }
}

void recvRequest(int *fd){
    int fileNameLength;
    char fileName[100];
    int numberRightClient=0;
    int listIP[100];
    int port[100];
    long fileSize;
    read(*fd,&fileNameLength,sizeof(fileNameLength));
    read(*fd,&fileName,fileNameLength);
    fileName[fileNameLength]=0;
    // printf("file name length: %d",fileNameLength);
    printf("\nEnd Host muon down load file: %s\n",fileName);
    for(int i=0;i<numberClient;i++){
        printf("Danh sach file:\n");
        for(int j=0;j<client[i].numberFile;j++){
            printf("%s\n",client[i].listFile[j]);
            if(strcmp(fileName,client[i].listFile[j])==0){
                numberRightClient++;
                listIP[numberRightClient-1]=client[i].IpAddr;
                port[numberRightClient-1]=client[i].port;
                printf("port: %d %d\n",i,port[numberRightClient]);
                fileSize=client[i].listFileSize[j];
                break;
            }
        }
    }
    if(numberRightClient>0){
        int type = 4;
        write(*fd,&type,sizeof(type));
        write(*fd,&fileSize,sizeof(fileSize));
        write(*fd,&numberRightClient,sizeof(numberRightClient));
        for(int i=0;i<numberRightClient;i++){
            write(*fd,&listIP[i],sizeof(listIP[i]));
            write(*fd,&port[i],sizeof(port[i]));
        }
    }else{
        int type = 7;
        int errorCode = 1;
        write(*fd,&type,sizeof(type));
        write(*fd,&errorCode,sizeof(errorCode));
    }
}

void * process(void * file_description) {
    int fd = *((int *) file_description);
    int n_bytes, message;

    sendAccept(&fd);



    while(n_bytes = read(fd, &message, sizeof(int))>0){
    printf("new message, type = %d\n", message);
    if (message == -1 || n_bytes==0) {
        printf("Khong the ket noi voi EndHost. \n");
        return NULL;
    }  else
    if (message == 2) {
        printf("client want to send list file\n");

        recvlistFile(&fd);
        sendAccept(&fd);
    } else
    if (message == 6){
        // printf("Message : %d\n", message);
        recvRequest(&fd);
    } else {
        // printf("Message : %d\n", message);
    }
    }

    if(n_bytes==0){
        printf("one client has closed!\n");
    }


    return NULL;
}

int main(int argc, char **argv) {
    int server_sockfd;
    int *client_sockfd;
    int PORT = 1098;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

     // Tao socket
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0) {
		perror("Server socket error. \n");
		return 1;
	}

    // Dat ten va gan dia chi cho socket theo giao thuc Internet
    char str[100];
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int bindCheck = bind(server_sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (bindCheck < 0) {
		perror("Bind error. \n");
		return 1;
	}

    // Tao hang doi va cho phep toi da 100 ket noi
    int listenCheck = listen( server_sockfd, 100 );
	if (listenCheck < 0) {
		perror("Listen error");
		return 1;
	}
    printf("Index Server dang cho tai cong:  %d\n", PORT);

    socklen_t client_len;
    pthread_t tid;

    while (1) {
        client_sockfd = malloc(sizeof(int));
        client_len= sizeof(client_addr);
        *client_sockfd = accept(server_sockfd, (struct sockaddr*) &client_addr, &client_len);

        printf("\nREQUEST FROM %s PORT %d \n",inet_ntop(AF_INET,&client_addr.sin_addr,str,sizeof(str)),htons(client_addr.sin_port));
        numberClient++;
        client[numberClient-1].IpAddr=client_addr.sin_addr.s_addr;
        pthread_create(&tid, NULL, &process, (void *) client_sockfd);

    }

    return 0;
}
