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
#include <string>
#include <iostream>
#define PORT 20000
#define LENGTH 512
#include <unistd.h>
using namespace std;

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

const char *SplitFilename (const string& str)
{
  cout << "Splitting: " << str << '\n';
  unsigned found = str.find_last_of("/\\");
//  cout << " path: " << str.substr(0,found) << '\n';
 return (str.substr(found+1)).c_str();
}

int main(int argc, char *argv[])
{
    /* Variable Definition */
    while(true)
    {
    	char input[100];
    	//scanf("%s",input);
    	cin.getline(input,100);
    	char * pch;
		//printf ("Splitting string \"%s\" into tokens:\n",input);
		pch = strtok(input," ");
		printf("%s\n",pch);
		
		int sockfd, nsockfd;
		char revbuf[LENGTH]; 
		struct sockaddr_in remote_addr;

		/* Get the Socket file descriptor */
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor! (errno = %d)\n",errno);
			exit(1);
		}

		/* Fill the socket address struct */
		remote_addr.sin_family = AF_INET; 
		remote_addr.sin_port = htons(PORT); 
		inet_pton(AF_INET, "172.16.27.78", &remote_addr.sin_addr); 
		//inet_pton(AF_INET, "192.168.103.179", &remote_addr.sin_addr);
		bzero(&(remote_addr.sin_zero), 8);

		/* Try to connect the remote */
		if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1) {
			fprintf(stderr, "ERROR: Failed to connect to the host! (errno = %d)\n",errno);
			exit(1);
		}
		else
			printf("[Client] Connected to server at port %d...ok!\n", PORT);
		
		if(strcmp(pch,"upload")==0)
		{
		
			pch=strtok (NULL, " ");
		//printf("%s\n",pch);
			char *fs_name=pch;
			printf("[Client] Sending %s to the Server... ", fs_name);
			//exit(1);
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
		
			const char *fileName;
			fileName=SplitFilename(fs_name);
		
			char new_name[strlen(fileName)+1];
			strcpy(new_name,fileName);
		
			char sendingString[500]="upload ";
			strcat(sendingString,new_name);
		
			//sending fileName
			fs_block_sz=strlen(sendingString)+1;
			printf("%d \t %s\n",fs_block_sz,new_name);
			
			
				
			//printf("Data Sent %d = %d\n",i,fs_block_sz);
			if (sendall(sockfd, sendingString, &fs_block_sz) == -1) {
				perror("sendall");
				printf("We only sent %d bytes because of the error!\n", fs_block_sz);
			}
			/*
			if(send(sockfd, sdbuf, fs_block_sz, 0) < 0) {
				fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name,     errno);
				exit(1);
			}
			*/
			//bzero(sdbuf, LENGTH);
			//i++;
			while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0) {
				
		//        printf("%s\n",sdbuf);
				
				//printf("Data Sent %d = %d\n",i,fs_block_sz);
				if (sendall(sockfd, sdbuf, &fs_block_sz) == -1) {
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
			shutdown(sockfd,2);
			printf("[Client] Connection lost.\n");
		}
		else if(strcmp(pch,"download")==0)
		{
			pch=strtok (NULL, " ");
			//printf("%s\n",pch);
			char *fs_name=pch;
			
			char sendingString[500]="download ";
			strcat(sendingString,fs_name);
			
			printf("[Client] Sending %s to the Server... ", fs_name);
			int fs_block_sz=strlen(sendingString)+1;
			if (sendall(sockfd, sendingString, &fs_block_sz) == -1) {
				perror("sendall");
				printf("We only sent %d bytes because of the error!\n", fs_block_sz);
			}
			int fr_block_sz = 0;
			fr_block_sz = recv(sockfd, revbuf, LENGTH, 0);
			printf("Printing Revbuf : %s\n",revbuf);
			
			char fr_name[500];
			system("mkdir ClientDownloads");
			getcwd(fr_name, sizeof(fr_name));
			printf("%s\n",fr_name);
			strcat(fr_name,"/ClientDownloads/");
			//strcpy(fr_name,"/home/nitesh/Dropbox/Coding/fileTransfer/file/");
			//printf("%s\n",fr_name);
			fr_block_sz = 0;
	    	//fr_block_sz = recv(sockfd, revbuf, LENGTH, 0);
			//char revbuf[LENGTH]; // Receiver buffer
			strcat(fr_name,revbuf);
			
			printf("%s\n",fr_name);
		    //printf("Data Received = %d\n",fr_block_sz);
		    //printf("%s\n",revbuf);
		    bzero(revbuf, LENGTH);
			//fr_name= "/home/nitesh/Dropbox/Coding/fileTransfer/file/temp.txt";
			FILE *fr;
			fr= fopen(fr_name, "a");
			//flag=1;
			printf("gone\n");
			if(fr == NULL)
			{
			    printf("File %s Cannot be opened file on server.\n", fr_name);
				//success = 1;
			}
			printf("here\n");
		    bzero(revbuf, LENGTH); 
		    fr_block_sz = 0;
		    int i=0;
		    while((fr_block_sz = recv(sockfd, revbuf, LENGTH, 0)) > 0) {
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
		else if(strcmp(pch,"ls")==0)
		{
			
			char new_name[100]="ls";
			/*send to server*/
			//char* fs_name = "/home/nitesh/Dropbox/Coding/fileTransfer/temp.txt";
		
			char sdbuf[LENGTH]; 
		
			bzero(sdbuf, LENGTH); 
			int fs_block_sz;
			int i=0;
		
			//sending fileName
			fs_block_sz=strlen(new_name)+1;
			printf("%d \t %s\n",fs_block_sz,new_name);
				
			//printf("Data Sent %d = %d\n",i,fs_block_sz);
			if (sendall(sockfd, new_name, &fs_block_sz) == -1) {
				perror("sendall");
				printf("We only sent %d bytes because of the error!\n", fs_block_sz);
			}
			
			bzero(revbuf, LENGTH); 
		    int fr_block_sz = 0;
		    i=0;
		    char server_message[5000];
		    int length=sizeof(struct sockaddr_in);
		   	int n = recvfrom(sockfd,&server_message,5000,0,(struct sockaddr *)&remote_addr, (socklen_t*)&length);
		   	
		   	printf("%s",server_message);
		   	
		   	//printf("received %d bytes\n",n);
		   	//shutdown(sockfd,2);
		    printf("Ok received from server!\n");
			
		}
		else
		{
			printf("Please input right command.\n");
		}
    }
    return (0);
}
