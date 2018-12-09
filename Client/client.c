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

int main()
{
    int sockfd;
    struct sockaddr_in serv_addr;

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

	if(connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
		error("Error Connecting");	
    else
    {
        printf("Connected Success !\n");
		struct HostInfo hostInfo;
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
		write(sockfd, temp, dataLength);
		printf(temp);
    }
    
	// while(1)
	// {
	// 	fprintf(stdout, "File name : ");
	// 	fscanf(stdin, "%s", file_name);
	// 	strcpy(buffer_sent, file_name);
	// 	write(sockfd, buffer_sent, sizeof(buffer_sent));
		
	// 	if(strcmp(file_name, "QUIT") == 0)
	// 		break;

	// 	read(sockfd, &f_size, sizeof(f_size));
	// 	if(f_size == 0)
	// 	{
	// 		printf("File Not Found\n");
	// 		continue;
	// 	}
	// 	buffer_recv[strlen(buffer_recv)-1] = '\0';
	// 	if(f_size > 0)
	// 	{
	// 			file = fopen(file_name,"wb");
	// 			while(count < f_size)
	// 			{
	// 				int nbytes = read(sockfd, buffer_recv, sizeof(buffer_recv));
	// 	 			fwrite(buffer_recv, 1, nbytes, file);
	// 				count += nbytes;
	// 	 		}
	// 	 		fclose(file);
	// 	 		count = 0;
	// 	}
	//}	

	// close()
	close(sockfd);
	printf("\nClosed Connection !\n");
	return 0;
}