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
    inet_pton(AF_INET, "127.0.0.1", &remote_addr.sin_addr); 
    //inet_pton(AF_INET, "192.168.103.179", &remote_addr.sin_addr);
    bzero(&(remote_addr.sin_zero), 8);

    /* Try to connect the remote */
    if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1) {
        fprintf(stderr, "ERROR: Failed to connect to the host! (errno = %d)\n",errno);
        exit(1);
    }
    else
        printf("[Client] Connected to server at port %d...ok!\n", PORT);

    /*send to server*/
    //char* fs_name = "/home/nitesh/Dropbox/Coding/fileTransfer/temp.txt";
    char *fs_name=argv[1];
    char sdbuf[LENGTH]; 
    printf("[Client] Sending %s to the Server... ", fs_name);
    FILE *fs = fopen(fs_name, "r");
    if(fs == NULL) {
        printf("ERROR: File %s not found.\n", fs_name);
        exit(1);
    }
    bzero(sdbuf, LENGTH); 
    int fs_block_sz,i=0;
    
    const char *fileName;
    fileName=SplitFilename(fs_name);
    
    char new_name[strlen(fileName)+1];
    strcpy(new_name,fileName);
    
    //sending fileName
    fs_block_sz=strlen(new_name)+1;
    printf("%d \t %s\n",fs_block_sz,new_name);
        
    //printf("Data Sent %d = %d\n",i,fs_block_sz);
    if (sendall(sockfd, new_name, &fs_block_sz) == -1) {
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
//    close (sockfd);
    printf("[Client] Connection lost.\n");
    return (0);
}
