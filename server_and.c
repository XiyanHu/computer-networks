#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MYPORT "22023"	// the port users will be connecting to
#define HOST "localhost"

// decimal number to binary string
char *dec_bin(long int num){
	if (num == 0) {
		return "0";
	}
	long int rem[50],i = 0,length = 0;
	char *str;
	str = (char*)malloc(length+1);
	while(num > 0){
 		rem[i] = num % 2;
 		num = num / 2;
 		i ++;
 		length ++;
 	}
	
    for (i = length - 1;i >= 0;i --){
        str[length-1-i] = rem[i]+'0';
    }
    str[length] = '\0';// This line is important. It prevents garbage characters in Ubuntu. But without it the program works fine in Mac.
	return str;
}

int main(void)
{	

	int sockfd;
	int rv_and;
	struct addrinfo hints_and, *servinfo_and, *p_and;
	struct sockaddr_storage their_addr;
	socklen_t addr_len;

	//The following code are learned from Beej's tutorial
	memset(&hints_and, 0, sizeof hints_and);
	hints_and.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints_and.ai_socktype = SOCK_DGRAM; // UDP datatgram
	hints_and.ai_flags = AI_PASSIVE; // use my IP

	rv_and = getaddrinfo(NULL, MYPORT, &hints_and, &servinfo_and);
	if (rv_and != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_and));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p_and = servinfo_and; p_and != NULL; p_and = p_and->ai_next) {
		sockfd = socket(p_and->ai_family, p_and->ai_socktype,p_and->ai_protocol);
		if (sockfd == -1) {
			perror("The Server AND: socket");
			continue;
		}
		// bind it to the port we passed in to getaddrinfo():
		if (bind(sockfd, p_and->ai_addr, p_and->ai_addrlen) == -1) {
			close(sockfd);
			perror("The Server AND: bind");
			continue;
		}
		break;
	}

	if (p_and == NULL) {
		fprintf(stderr, "The Server AND: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo_and);// end of the referred code from Beej's tutorial
	printf("The Server AND is up and running using UDP on port %s\n",MYPORT);
	
	while(1){
		addr_len = sizeof their_addr;

		int data_size;
		recvfrom(sockfd,&data_size,sizeof data_size,0,(struct sockaddr *)&their_addr, &addr_len);
		printf("The Server AND start receiving lines from the edge server for AND computation.");
		int data[100];
		int res[100];
		char res_str[data_size][20];
		int ct;

		recvfrom(sockfd,data,sizeof data,0,(struct sockaddr *)&their_addr, &addr_len);

		//transfer the result from dec to bin string and output the information
		printf("The computation results are:\n");
		for (ct = 0; ct < data_size; ct++){
			res[ct] = data[ct*2] & data[ct*2+1];
			if (res[ct] == 0){
				strcpy(res_str[ct], "0");
				printf("%s and %s = 0\n",dec_bin(data[ct*2]),dec_bin(data[ct*2+1]));	
			}
			else{
				strcpy(res_str[ct], dec_bin(res[ct]));
				printf("%s and %s = %s\n",dec_bin(data[ct*2]),dec_bin(data[ct*2+1]),res_str[ct]);	
			}	
		}
		printf("The Server AND has successfully received %d lines from the edge server and finished all AND computations.\n",data_size);

		//send the result to edge server.
		sendto(sockfd, res_str, sizeof res_str , 0,(struct sockaddr *) &their_addr, addr_len);
		printf("The Server AND has successfully finished sending all computation results to the edge server.\n");
		
	}
	
}
