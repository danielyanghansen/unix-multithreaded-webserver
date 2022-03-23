//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
	
#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAX_PENDING_CONNECTIONS 3
#define MAX_CLIENTS 30

char send_buffer[1000];

int parseGET(const char* line, int socket)
{

    /* Find out where everything is */
    const char *start_of_path = strchr(line, ' ') + 1;
    const char *start_of_query = strchr(start_of_path, '?');
    const char *end_of_query = strchr(start_of_query, ' ');
    /* Get the right amount of memory */
    char path[start_of_query - start_of_path];
    char query[end_of_query - start_of_query];
    /* Copy the strings into our memory */
    strncpy(path, start_of_path,  start_of_query - start_of_path);
    strncpy(query, start_of_query, end_of_query - start_of_query);
    /* Null terminators (because strncpy does not provide them) */
    path[sizeof(path)] = 0;
    query[sizeof(query)] = 0;
    char* filepath = strcat(path, "\0");

    printf("Filepath: %s\n", filepath);
    //char* filepath = "./doc/index.html\0"; 
    //then open the file.
    //For GET requests
    FILE *file = fopen(&filepath, "r");
	char parser_send_buffer[1000];

    while (!feof(filepath)) {
        int numread = fread(parser_send_buffer, sizeof(unsigned char), 1000, filepath);
        int numsent = send(socket, *parser_send_buffer, numread, 0);
	}
    return 0;
	
}
	
int main(int argc , char *argv[])
{
	int opt = TRUE;
	int master_socket , addrlen , new_socket , client_socket[MAX_CLIENTS] ,
		max_clients = MAX_CLIENTS , activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;
		
	char buffer[1025]; //data buffer of 1K
	//set of socket descriptors
	fd_set readfds;
	
	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}
	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("failed to create master socket...\n");
		exit(EXIT_FAILURE);
	}
	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt: failed to set up multiple connections...\n");
		exit(EXIT_FAILURE);
	}
	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
		
	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("failed to bind socket to port...\n");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, MAX_PENDING_CONNECTIONS) < 0)
	{
		perror("failed to set up listening...\n");
		exit(EXIT_FAILURE);
	}
	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...\n");
	while(TRUE)
	{
		//clear the socket set
		FD_ZERO(&readfds);
		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++)
		{
			//socket descriptor
			sd = client_socket[i];
				
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);
				
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}
		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			
			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" ,new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
				
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);
						
					break;
				}
			}
		}
			
		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
				
			if (FD_ISSET( sd , &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread = read( sd , buffer, 1024)) == 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" ,
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i] = 0;
				}
					
				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					buffer[valread] = '\0';
                    
                    //TESTING SENDING FILE:
                    int running = parseGET( &buffer, sd);
                    /*
                    FILE *testFILE = fopen("./doc/index.html", "r");
					char *send_buffer_ptr = send_buffer;
                    while (!feof(testFILE)) {
                        int numread = fread(send_buffer, sizeof(unsigned char), 1000, testFILE);
                        int numsent = send(sd, send_buffer_ptr, numread, 0);
                    }
                    */
                    //char* responseMessage = "Functions for response message go here";
                    //write(sd, responseMessage, sizeof(responseMessage));
					//send(sd , buffer , strlen(buffer) , 0 );

	
				}
			}
		}
	}
		
	return 0;
}
