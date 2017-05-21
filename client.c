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
#include <signal.h>
#include <ctype.h> 

#define PORT_EDGE "23023"   //aws TCP port
#define HOST "localhost"

char data_sepr[100][100];

// read the file and seperately store the string and number and then return the number of file
int read_file(char* file_name){
	char buf[1024];  
	FILE *fp;            
	int len;            
	char dot[] = ",";
	int num_of_line;

	if((fp = fopen(file_name,"r")) == NULL){
		 perror("fail to read");
		 exit (1) ;
	 }
	 char *result = NULL;
	 int ct = 0;
	 while(fgets(buf,1024,fp) != NULL){
		 num_of_line ++;
		 len = strlen(buf);
		 buf[len-1] = '\0'; 
		 result = strtok(buf, dot);
		 while( result != NULL ) {
		 	strcpy(data_sepr[ct],result);
		  	ct ++;
	     	result = strtok( NULL, dot);
	    }
	 }
	 return num_of_line;	
}

int main(int argc, char *argv[])
{	
	int num_of_line;
	int sockfd; 
	int rv_c; 
	struct addrinfo hints_c, *servinfo_c, *p_c;
	
	//The following code are from Beej's tutorial
	memset(&hints_c, 0, sizeof hints_c);
	hints_c.ai_family = AF_UNSPEC;
	hints_c.ai_socktype = SOCK_STREAM;

	rv_c = getaddrinfo(HOST, PORT_EDGE, &hints_c, &servinfo_c);
	if (rv_c != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_c));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p_c = servinfo_c; p_c != NULL; p_c = p_c->ai_next) {
		sockfd = socket(p_c->ai_family, p_c->ai_socktype,p_c->ai_protocol);
		if (sockfd == -1) {
			perror("client: socket");
			continue;
		}
		if (connect(sockfd, p_c->ai_addr, p_c->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}
		break;
	}

	if (p_c == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	freeaddrinfo(servinfo_c); // all done with this structure
	// end of the referred code from Beej's tutorial
	printf("The client is up and running. \n");
	
	num_of_line = read_file(argv[1]);
	
	// send number of lines and seperate data to edge server
	send(sockfd, (char *)& num_of_line, sizeof num_of_line, 0);
	send(sockfd, (char *)& data_sepr, sizeof data_sepr, 0);
	printf("The client has successfully finished sending %d lines to the edge server.\n",num_of_line);

	char res_sequence[num_of_line][100];
	// receive the calculation result from edge server.
	recv(sockfd, (char *)&res_sequence, sizeof res_sequence, 0);
	printf("The client has successfully finished receiving all computation results from the edge server.\n");
	printf("The final computation results are:\n");

	//output the result
	int count;
	for (count = 0; count < num_of_line; count++){
		printf("%s\n",res_sequence[count]);
	} 
}

