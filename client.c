// Client side C/C++ program to demonstrate Socket programming
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
char* command = NULL;
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
	struct sockaddr_in address;
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char *hello = "Hello from client";
	char buffer[1024] = {0};
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
	// which is meant to be, and rest is defined below

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Converts an IP address in numbers-and-dots notation into either a
	// struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
	{
		printf("\nConnection Failed \n");
		return -1;
	}
	long long int filesize;
	char *BUFFER = (char *)malloc(4000 * sizeof(char));
	int fwr ;
	double pi, percentage;
	char message[100] = "";
	while (1)
	{
		command = NULL;
		char msg[1024];
		memset(msg, '\0', sizeof(msg));
		printf("client> ");
		fgets(msg, 1024, stdin);
		if (strlen(msg) == 1)
			continue;
		command = strtok(msg, " \n");
		// printf("%s\n", command);
		send(sock , command , strlen(command) , 0 );
		check2(sock);
		// sleep(1);
		if (strcmp(command, "get") == 0)
		{
			// printf("asd\n");
			command = strtok(NULL, " \n");
			while (command != NULL)
			{
				// printf("COMM%s", command);
				send(sock , command , strlen(command) , 0 );
				check2(sock);
				valread = recv( sock , buffer, 1024, 0);
				check1(sock);
				// printf("%s\n", buffer);
				filesize = (long long int)atol(buffer);
				// printf("%lld\n", filesize);
				if (filesize == -1)
				{
					printf("Error in opening %s!\n", command);
					command = strtok(NULL, " \n");
					memset(buffer, '\0', sizeof(buffer));
					continue;
				}
				pi = filesize / 4000.0;
				pi = 100 / pi;
				percentage = pi;
				fwr = open(command, O_CREAT | O_TRUNC | O_RDWR, 0644);
				while (filesize > 4000)
				{
					recv(sock, BUFFER, 4000, 0);
					check1(sock);
					// send(new_socket , BUFFER , strlen(BUFFER) , 0 );
					write(fwr, BUFFER, 4000);
					filesize -= 4000;
					sprintf(message, "Receiving %s.............%0.2f%% complete\r", command, percentage);
					percentage += pi;
					write(1, message, strlen(message));
				}
				recv(sock, BUFFER, filesize, 0);
				check1(sock);
				write(fwr, BUFFER, filesize);
				sprintf(message, "%s successfully Transferred.............100%% done\n", command);
				write(1, message, strlen(message));
				command = strtok(NULL, " \n");
				memset(buffer, '\0', sizeof(buffer));
				// send(new_socket , BUFFER , strlen(BUFFER) , 0 );
			}
			command = "-1";
			send(sock , command , strlen(command) , 0 );
			check2(sock);
			// printf("Hello message sent\n");
			// receive message back from server, into the buffer
			// printf("%s\n", buffer);
		}
		else if (strcmp(command, "exit") == 0)
		{
			break;
		}
		else
		{
			printf("Usage:- get,exit\n");
		}
		// printf("endofloop\n");
	}
	return 0;
}
