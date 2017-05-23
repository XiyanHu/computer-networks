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

#define TCP_PORT "23023"
#define UDP_PORT "24023"
#define HOST "localhost"
#define PORT_OR "21023"
#define PORT_AND  "22023"
#define BACKLOG 10 // how many pending connections queue will hold

//The following code are learned from Beej's tutorial
void sigchld_handler(int s){
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;
	while(waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}

//transfer bin strings to dec integers 
int bin_dec(char bin[]){
	int count;
	int temp = 0;
	int sum = 0;
	for (count = 0; count < strlen(bin); count++){
		temp = bin[count] - '0';
		sum += temp;
		sum *= 2;
	}
	sum /= 2;
	return sum;
}

// send data to the corresponding backend servers
int send_data(char data[][100],int num_of_cal,char *port,int first_time,char res_back[][100]){
		char* backend_port;
		int mysocket;
		struct addrinfo hints_b, *serv_binfo_b, *p_b;
		int rv_b;

		
		if (strcmp(port,"and") == 0){
			backend_port = PORT_AND;
		}else if (strcmp(port,"or") == 0){
			backend_port = PORT_OR;
		}

		//The following code are learned from Beej's tutorial
		memset(&hints_b, 0, sizeof hints_b);
		hints_b.ai_family = AF_UNSPEC;
		hints_b.ai_socktype = SOCK_DGRAM;
		if ((rv_b = getaddrinfo(HOST, backend_port, &hints_b, &serv_binfo_b)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_b));
			return 1;
		}
		// loop through all the results and make a socket
		for(p_b = serv_binfo_b; p_b != NULL; p_b = p_b->ai_next) {
			if ((mysocket = socket(p_b->ai_family, p_b->ai_socktype,
					p_b->ai_protocol)) == -1) {
				perror("talker: socket");
				continue;
			}

			break;
		}
		if (p_b == NULL) {
			fprintf(stderr, "talker: failed to create socket\n");
			return 2;
		} // end of the copied code from Beej's tutorial

		int data_sent[num_of_cal];
		int count0 = 0;
		for (count0 = 0; count0 < num_of_cal;count0 ++){
			data_sent[count0] = bin_dec(data[count0]);	
		}

		int data_size = num_of_cal/2;

		sendto(mysocket, &data_size ,sizeof data_size, 0,p_b->ai_addr, p_b->ai_addrlen);
		sendto(mysocket, data_sent, sizeof data_sent, 0,p_b->ai_addr, p_b->ai_addrlen);
		freeaddrinfo(serv_binfo_b);
		
		char res_str[data_size][20];
		int count = 0;

		if (first_time == 0){
			printf("The edge server start receiving the computation results from Backend-Server OR and Backend-Server AND using UDP port %s.\n",UDP_PORT );	
		}
		
		recvfrom(mysocket, (char *)& res_str[count], sizeof res_str, 0 , NULL, NULL);

		if (first_time == 0){
			printf("The computation results are:\n");	
		}
		
		//send the data back to seperate arrays in main function
		for (count=0;count<data_size;count++){
			strcpy(res_back[count],res_str[count]);
			if (strcmp(port,"and")==0){
			printf("%s and %s = %s\n",data[2*count],data[2*count+1],res_str[count]);
			}else if (strcmp(port,"or")==0){
				printf("%s or %s = %s\n",data[2*count],data[2*count+1],res_str[count]);	
			}	
		}
		return 0;	
}

int main(int argc, char *argv[])
{	
	//The following code are learned from Beej's tutorial
	int sockfd, new_fd;  // listen on sock_fd and new connection on new_fd
    struct addrinfo hints_edge, *servinfo_edge, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes = 1;
    int rv_edge;

    memset(&hints_edge, 0, sizeof hints_edge);
    hints_edge.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints_edge.ai_socktype = SOCK_STREAM; // TCP stream
    hints_edge.ai_flags = AI_PASSIVE; // use my IP

    rv_edge = getaddrinfo(HOST, TCP_PORT, &hints_edge, &servinfo_edge);
    if (rv_edge != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_edge));
        return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo_edge; p != NULL; p = p->ai_next) {
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd == -1) {
			perror("server: socket");
			continue;
		}
		//set various options for a socket
		int set_sockopt = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (set_sockopt == -1) {
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}
	freeaddrinfo(servinfo_edge); // all done with this structure

	//listen
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}// end of the referred code from Beej's tutorial

	printf( "The edge server is up and running. \n");

	while(1){
		//This first_time flag is set to prevent multiply prompt information. 
		int first_time = 0;
		sin_size = sizeof their_addr;	
		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");			
			exit(1);
		}

		// get the port of client
		struct sockaddr_in addrTheir;
		memset(&addrTheir, 0, sizeof(addrTheir));
		int len = sizeof(addrTheir);
		getpeername(new_fd, (struct sockaddr *) &addrTheir, (socklen_t *) &len);
		int client_port = addrTheir.sin_port;

		// transfer data
		int num_of_line;
		recv(new_fd, (char *)&num_of_line, sizeof num_of_line, 0);
		printf("The edge server has received %d lines from the client using TCP over port %s\n",num_of_line,TCP_PORT );

		char data[100][100];
		recv(new_fd, (char *)&data, sizeof data, 0);

		int line_count;
		int and_count = 0;
		int or_count = 0;
		for (line_count = 0;line_count < num_of_line;line_count ++){
			int key_index = 3 * line_count;
			if (strcmp(data[key_index],"and") == 0){
				and_count ++;
			}
			if (strcmp(data[key_index],"or") == 0){
				or_count ++;
			}
		}

		// seperate the data that are sent to different backend servers
		char data_and[and_count*2][100];
		char data_or[or_count*2][100];
		int index_and = 0;
		int index_or = 0;
		for (line_count = 0;line_count < num_of_line;line_count++){
			int key_index = 3 * line_count;
			if (strcmp(data[key_index],"and")==0){
				strcpy(data_and[index_and++],data[key_index+1]);
				strcpy(data_and[index_and++],data[key_index+2]);
				
			}
			if (strcmp(data[key_index],"or")==0){
				strcpy(data_or[index_or++],data[key_index+1]);
				strcpy(data_or[index_or++],data[key_index+2]);
			}
		}

		printf("The edge server has successfully sent %d lines to Backend-Server OR.\n",or_count);	
		printf("The edge server has successfully sent %d lines to Backend-Server AND.\n",and_count);
		
		char and_res[and_count][100];
		char or_res[or_count][100];

		//call the function to send the data.
		if (or_count > 0){
			send_data(data_or,or_count*2,"or",first_time,or_res);
			first_time = 1;	
		}
		if (and_count > 0){
			send_data(data_and,and_count*2,"and",first_time,and_res);
			first_time = 1;	
		}

		char res_sequence[num_of_line][100];
		int and_index = 0;
		int or_index = 0;
		
		// store the result in sequence
		for (line_count = 0;line_count < num_of_line;line_count ++){
			int key_index = 3*line_count;
			if (strcmp(data[key_index],"and")==0){
				strcpy(res_sequence[line_count],and_res[and_index]);
				and_index++;
			}
			if (strcmp(data[key_index],"or")==0){
				strcpy(res_sequence[line_count],or_res[or_index]);
				or_index++;
			}
		}
		
		printf("The edge server has successfully finished receiving all computation results from the Backend-Server OR and Backend-Server AND.\n");

		//send results back to client in right order
		send(new_fd, (char *)& res_sequence, sizeof res_sequence, 0);
		printf("The edge server has successfully finished sending all computation results to the client.\n");
		first_time = 0;
	}		
		return 0;
}
