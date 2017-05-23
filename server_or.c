#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <sys/wait.h>

#define MYPORT "21023"	// the port users will be connecting to
#define HOST "localhost"

// transfer decimal number to binary string
char *dec_bin(long int num){
	if (num == 0) {
		return "0";
	}
	long int rem[50],i = 0,length =0;
	char *str;
	str = (char*)malloc(length+1);
	while(num > 0){
 		rem[i] = num % 2;
 		num = num / 2;
 		i ++;
 		length ++;
 	}
	
    for (i = length-1 ; i>=0 ; i--){
        str[length-1-i] = rem[i]+'0';
    }
    str[length] = '\0';// This line is important. It prevents garbage characters in Ubuntu. But without it the program works fine in Mac.
	return str;
}

int main(void)
{
	int sockfd;
	int rv_or;
	struct addrinfo hints_or, *servinfo_or, *p_or;
	struct sockaddr_storage their_addr;
	socklen_t addr_len;

	// the following code are learned from Beej's tutorial
	memset(&hints_or, 0, sizeof hints_or);
	hints_or.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints_or.ai_socktype = SOCK_DGRAM; // UDP datatgram
	hints_or.ai_flags = AI_PASSIVE; // use my IP

	rv_or = getaddrinfo(NULL, MYPORT, &hints_or, &servinfo_or);
	if (rv_or!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_or));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p_or = servinfo_or; p_or != NULL; p_or = p_or->ai_next) {
		sockfd = socket(p_or->ai_family, p_or->ai_socktype,p_or->ai_protocol);
		if (sockfd == -1) {
			perror("The Server OR: socket");
			continue;
		}
		if (bind(sockfd, p_or->ai_addr, p_or->ai_addrlen) == -1) {
			close(sockfd);
			perror("The Server OR: bind");
			continue;
		}
		break;
	}

	if (p_or == NULL) {
		fprintf(stderr, "The Server OR: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo_or);// end of the referred code from Beej's tutorial. I write it by myself.
	printf("The Server OR is up and running using UDP on port %s\n",MYPORT);
	
	while(1){
		addr_len = sizeof their_addr;

		int data_size;
		recvfrom(sockfd,&data_size,sizeof data_size,0,(struct sockaddr *)&their_addr, &addr_len);
		printf("The Server OR start receiving lines from the edge server for OR computation.");
		
		int data[100];
		int res[100];
		char res_str[data_size][20];
		int ct;

		//receive the data for calculation and store in an array
		recvfrom(sockfd,data,sizeof data,0,(struct sockaddr *)&their_addr, &addr_len);
		printf("The computation results are:\n");
		
		//transfer the result from dec to bin string and output the information
		for (ct = 0; ct < data_size; ct++){
			res[ct] = data[ct*2] | data[ct*2+1];
			if (res[ct] == 0){
				strcpy(res_str[ct], "0");
				printf("%s or %s = 0\n",dec_bin(data[ct*2]),dec_bin(data[ct*2+1]));	
			}
			else{
				strcpy(res_str[ct], dec_bin(res[ct]));
				printf("%s or %s = %s\n",dec_bin(data[ct*2]),dec_bin(data[ct*2+1]),res_str[ct]);	
			}	
		}
		printf("The Server OR has successfully received %d lines from the edge server and finished all AND computations.\n",data_size);

		//send the result to edge server.
		sendto(sockfd, res_str, sizeof res_str , 0,(struct sockaddr *) &their_addr, addr_len);
		printf("The Server OR has successfully finished sending all computation results to the edge server.\n");
			
		}
	
}
