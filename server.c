#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<signal.h>
#include<ctype.h>          
#include<arpa/inet.h>
#include<netdb.h>
#include <unistd.h>
#define PORT 20000 
#define BACKLOG 5
#define LENGTH 512 

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main ()
{
    /* Defining Variables */
    int sockfd, nsockfd; 
    int num;
    int sin_size; 
    struct sockaddr_in addr_local; /* client addr */
    struct sockaddr_in addr_remote; /* server addr */
    char revbuf[LENGTH]; // Receiver buffer

    /* Get the Socket file descriptor */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
        fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor. (errno = %d)\n", errno);
        exit(1);
    }
    else 
        printf("[Server] Obtaining socket descriptor successfully.\n");

    /* Fill the client socket address struct */
    addr_local.sin_family = AF_INET; // Protocol Family
    addr_local.sin_port = htons(PORT); // Port number
    addr_local.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
    bzero(&(addr_local.sin_zero), 8); // Flush the rest of struct

    /* Bind a special Port */
    if( bind(sockfd, (struct sockaddr*)&addr_local, sizeof(struct sockaddr)) == -1 ) {
        fprintf(stderr, "ERROR: Failed to bind Port. (errno = %d)\n", errno);
        exit(1);
    }
    else 
        printf("[Server] Binded tcp port %d in addr 127.0.0.1 sucessfully.\n",PORT);

    /* Listen remote connect/calling */
    if(listen(sockfd,BACKLOG) == -1) {
        fprintf(stderr, "ERROR: Failed to listen Port. (errno = %d)\n", errno);
        exit(1);
    }
    else
        printf ("[Server] Listening the port %d successfully.\n", PORT);

    int success = 0;
    int flag=0;
    FILE *fr;
    char fr_name[500];
    while(success == 0)
    {
        sin_size = sizeof(struct sockaddr_in);

        /* Wait a connection, and obtain a new socket file despriptor for single connection */
        
        /*Receive File from Client */
        if(flag==0)
        {
        	if ((nsockfd = accept(sockfd, (struct sockaddr *)&addr_remote, (socklen_t *)&sin_size)) == -1) {
		        fprintf(stderr, "ERROR: Obtaining new Socket Despcritor. (errno = %d)\n", errno);
		        exit(1);
		    }
		    else 
		        printf("[Server] Server has got connected from %s.\n", inet_ntoa(addr_remote.sin_addr));
        	int fr_block_sz = 0;
		    fr_block_sz = recv(nsockfd, revbuf, LENGTH, 0);
		    printf("%s\n",revbuf);
		    system("mkdir Downloads");
		    getcwd(fr_name, sizeof(fr_name));
		    printf("%s\n",fr_name);
		    strcat(fr_name,"/Downloads/");
		    //strcpy(fr_name,"/home/nitesh/Dropbox/Coding/fileTransfer/file/");
		    //printf("%s\n",fr_name);
		    strcat(fr_name,revbuf);
		    printf("%s\n",fr_name);
	        printf("Data Received = %d\n",fr_block_sz);
	        printf("%s\n",revbuf);
	        bzero(revbuf, LENGTH);
		    //fr_name= "/home/nitesh/Dropbox/Coding/fileTransfer/file/temp.txt";
		    fr= fopen(fr_name, "a");
			flag=1;
			printf("gone\n");
			if(fr == NULL)
			{
		        printf("File %s Cannot be opened file on server.\n", fr_name);
				success = 1;
			}
			
		}
		else
		{
			printf("here\n");
	        bzero(revbuf, LENGTH); 
	        int fr_block_sz = 0;
	        int i=0;
	        while((fr_block_sz = recv(nsockfd, revbuf, LENGTH, 0)) > 0) {
	            printf("Data Received %d = %d\n",i,fr_block_sz);
//	            printf("%s\n",revbuf);
	            int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
	            if(write_sz < fr_block_sz)
	                error("File write failed on server.\n");
	            bzero(revbuf, LENGTH);
	            i++;
	        }
	        if(fr_block_sz < 0) {
	            if (errno == EAGAIN)
	                printf("recv() timed out.\n");
	            else {
	                fprintf(stderr, "recv() failed due to errno = %d\n", errno);
	                exit(1);
	            }
	        }
	        printf("Ok received from client!\n");
	        fclose(fr);
		    success = 1;
		 }
    }
}   
