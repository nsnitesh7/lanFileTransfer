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

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
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
    while(true)
    {
        sin_size = sizeof(struct sockaddr_in);

        /* Wait a connection, and obtain a new socket file despriptor for single connection */
        
        /*Receive File from Client */
        
    	if ((nsockfd = accept(sockfd, (struct sockaddr *)&addr_remote, (socklen_t *)&sin_size)) == -1) {
	        fprintf(stderr, "ERROR: Obtaining new Socket Despcritor. (errno = %d)\n", errno);
	        exit(1);
	    }
	    else 
	        printf("[Server] Server has got connected from %s.\n", inet_ntoa(addr_remote.sin_addr));
    	int fr_block_sz = 0;
	    fr_block_sz = recv(nsockfd, revbuf, LENGTH, 0);
	    printf("Printing Revbuf : %s\n",revbuf);
	    
	    char * pch;
		//printf ("Splitting string \"%s\" into tokens:\n",input);
		pch = strtok(revbuf," ");
		printf("%s\n",pch);
		
		if(strcmp(pch,"upload")==0)
		{
			
			system("mkdir Downloads");
			getcwd(fr_name, sizeof(fr_name));
			printf("%s\n",fr_name);
			strcat(fr_name,"/Downloads/");
			//strcpy(fr_name,"/home/nitesh/Dropbox/Coding/fileTransfer/file/");
			//printf("%s\n",fr_name);
			pch=strtok (NULL, " ");
			printf("Printing Revbuf2 : %s\n",pch);
			strcat(fr_name,pch);
			printf("%s\n",fr_name);
		    printf("Data Received = %d\n",fr_block_sz);
//		    printf("%s\n",revbuf);
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
			printf("here\n");
		    bzero(revbuf, LENGTH); 
		    fr_block_sz = 0;
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
		}
		else if(strcmp(pch,"download")==0)
		{
			pch=strtok (NULL, " ");
		//printf("%s\n",pch);
			char *file_name=pch;
			printf("[Client] Sending %s to the Client... ", file_name);
			//exit(1);
			char fs_name[500]="Downloads/";
			strcat(fs_name,file_name);
			FILE *fs = fopen(fs_name, "r");
			if(fs == NULL) {
				printf("ERROR: File %s not found.\n", fs_name);
				exit(1);
			}

			/*send to server*/
			//char* fs_name = "/home/nitesh/Dropbox/Coding/fileTransfer/temp.txt";
		
			char sdbuf[LENGTH]; 
		
			bzero(sdbuf, LENGTH); 
			int fs_block_sz,i=0;
		
			//sending fileName
			fs_block_sz=strlen(file_name)+1;
			printf("%d \t %s\n",fs_block_sz,file_name);
				
			//printf("Data Sent %d = %d\n",i,fs_block_sz);
			if (sendall(nsockfd, file_name, &fs_block_sz) == -1) {
				perror("sendall");
				printf("We only sent %d bytes because of the error!\n", fs_block_sz);
			}
			//exit(1);
			/*
			if(send(sockfd, sdbuf, fs_block_sz, 0) < 0) {
				fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name,     errno);
				exit(1);
			}
			*/
			while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0) {
				
		//        printf("%s\n",sdbuf);
				
				//printf("Data Sent %d = %d\n",i,fs_block_sz);
				if (sendall(nsockfd, sdbuf, &fs_block_sz) == -1) {
					perror("sendall");
					printf("We only sent %d bytes because of the error!\n", fs_block_sz);
				} 
				/*
				if(send(sockfd, sdbuf, fs_block_sz, 0) < 0) {
				    fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name,     errno);
				    exit(1);
				}
				*/
				bzero(sdbuf, LENGTH);
				i++;
			}
			shutdown(nsockfd,2);
			printf("[Client] Connection lost.\n");
		}
		else if(strcmp(pch,"ls")==0)
		{
			//system("ls Downloads");
			FILE *fp;
			int status;
			char path[1035];
			char send1[5000]="";

			/* Open the command for reading. */
			fp = popen("ls Downloads", "r");
			if (fp == NULL) {
			printf("Failed to run command\n" );
			exit;
			}

			/* Read the output a line at a time - output it. */
			while (fgets(path, sizeof(path)-1, fp) != NULL) {
				strcat(send1,path);
				//printf("%s", path);
			}
			//printf("%s",send1);
			/* close */
			pclose(fp);
			int l=strlen(send1)+1;
			
			printf("%d\n",l);
			
			//printf("Data Sent %d = %d\n",i,fs_block_sz);
			send(nsockfd, send1, l, 0);
			//printf("sent\n");
//			if (sendall(sockfd, send, &l) == -1) {
//				perror("sendall");
//				printf("We only sent bytes because of the error!\n");
//			}
			
		}
    //    flag=0;
		    //success = 1;
    }
}   
