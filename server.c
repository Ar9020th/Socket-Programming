#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define PORT 8000
void check1(int socket)
{
	char packet[50] = "Done";
	write(socket, packet, 5);
}
void check2(int socket)
{
	char packet[50];
	recv(socket, packet, 5, 0);
}
int main(int argc, char const *argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char *hello = "Hello from server";

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// This is to lose the pesky "Address already in use" error message
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
	               &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc.
	address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
	address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address,
	         sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Port bind is done. You want to wait for incoming connections and handle them in some way.
	// The process is two step: first you listen(), then you accept()
	if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	// returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
	                         (socklen_t*)&addrlen)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	char *BUFFER = (char *)malloc(4000 * sizeof(char));
	struct stat info;
	long long int fd;
	long long int filesize;
	char filesiz[20];
	while (1)
	{
		memset(buffer, '\0', sizeof(buffer));
		// printf("wq%s\n", buffer);
		valread = recv(new_socket , buffer, 1024, 0);
		check1(new_socket);
		// printf("%s\n", buffer);
		if (strcmp(buffer, "get") == 0)
		{
			// printf("adwd\n");
			// memset(buffer, '\0', sizeof(buffer));
			while (1)
			{
				memset(buffer, '\0', sizeof(buffer));
				// printf("%s\n", buffer);
				valread = recv(new_socket , buffer, 1024, 0);
				check1(new_socket);
				// check2(sock);
				// printf("%s\n", buffer);
				if (strcmp(buffer, "-1") == 0)
				{
					// printf("Chala jaa\n");
					break;
				}
				fd = open(buffer, O_RDONLY);
				if (fd < 0)
				{
					// itoa(-1, filesiz, 10);
					sprintf(filesiz, "%d", -1);
					send(new_socket , filesiz , strlen(filesiz) , 0 );
					check2(new_socket);
					continue;
				}
				stat(buffer, &info);
				filesize = info.st_size;
				// printf("%lld\n", filesize);
				// ltoa(filesize, filesiz, 10);
				sprintf(filesiz, "%lld", filesize);
				send(new_socket , filesiz , strlen(filesiz) , 0 );
				check2(new_socket);
				while (filesize > 4000)
				{
					read(fd, BUFFER, 4000);
					send(new_socket , BUFFER , strlen(BUFFER) , 0 );
					check2(new_socket);
					filesize -= 4000;
				}
				read(fd, BUFFER, filesize);
				send(new_socket , BUFFER , filesize , 0 );
				check2(new_socket);
			}
		}
		else if (strcmp(buffer, "exit") == 0)
		{
			break;
		}
		else
			continue;
	}
	// read infromation received into the buffer
	// printf("%s\n", buffer);
	send(new_socket , hello , strlen(hello) , 0 );  // use sendto() and recvfrom() for DGRAM
	printf("Hello message sent\n");
	return 0;
}
