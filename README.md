a. Full Name:XXXXXXXX <br>
b. Student ID:XXXXXXXX <br>
c. What I have done in the assignment:<br>
	In this assignment I finish a model of computational offloading and meet the requirements in the instructions. I actually use edge server to distribute the computation load from client on two bankend servers. In phase 1,I boot up all the servers and show the message on the screen. Then I using file name to read the file, getting data from file and seperate them into strings ,also count the name of lines. Then with TCP connection I send all of them to the edge server. In phase 2, edge server will seperate the data and send them to corresponding backend servers with UDP connections.Then two backend servers will finish their computations. Then in phase 3, two backend servers send their results back to edge server. I adjust the order and create an array to store all the results in correct order and then edge server send them back to client.

d. What your code files are and what each one of them does.<br> 
	client.c: This file gets the file name, then read the data from file including strings and numbers,count the number of lines and then use TCP connection to send them to the edge server. When the calculation is over, client will get the results and output on the screen. <br>
	edge.c: This file gets the detailed data from client with TCP connection and  separate them into two parts "and" and "or",then send data to corresponding backend servers with UDP connections. When the calculation in two backend servers is over,it will get the calculation result from them and show them on the screen. At last , it will adjust the order and send the results back to client in correct order.<br>
	server_and.c: This file gets the data that only have to operate "and" from edge server with UDP connections. It will finish the calculation and send the results back. Also it will show them on the screen. <br>
	server_or.c: This file gets the data that only have to operate "or" from edge server with UDP connections. It will finish the calculation and send the results back. Also it will show them on the screen. 

e. What the TA should do to run your programs. <br>
	(1)Open four different terminals. Use either one of them and type "make all" to compile all the files.<br>
	(2)Type "make edge" in one terminal to run the edge server.<br>
	(3)Type "make server_and" in one terminal to run the "and" backend server.<br>
	(3)Type "make server_or" in one terminal to run the "or" backend server.<br>
	(4)Type "./client <input_file_name>" (i.e. ./client job.txt) in the last one terminal to run the client.<br>

f. The format of all the messages exchanged.<br>
	With the example job.txt.     <br>
	(1)client:<br>
	The client is up and running. 
	The client has successfully finished sending 4 lines to the edge server.
	The client has successfully finished receiving all computation results from the edge server.
	The final computation results are:
	101
	1011
	10011
	1<br>
	(2)edge:<br>
	The edge server is up and running. 
	The edge server has received 4 lines from the client using TCP over port 23023
	The edge server has successfully sent 2 lines to Backend-Server OR.
	The edge server has successfully sent 2 lines to Backend-Server AND.
	The edge server start receiving the computation results from Backend-Server OR and Backend-Server AND using UDP port 24023.
	The computation results are:
	10 or 1011 = 1011
	11 or 10001 = 10011
	10111 and 101 = 101
	1001 and 11 = 1
	The edge server has successfully finished receiving all computation results from the Backend-Server OR and Backend-Server AND.
	The edge server has successfully finished sending all computation results to the client.
	(3)server_or<br>
	The Server OR is up and running using UDP on port 21023
	The Server OR start receiving lines from the edge server for OR computation.The computation results are:
	10 or 1011 is 1011
	11 or 10001 is 10011
	The Server OR has successfully received 2 lines from the edge server and finished all AND computations.
	The Server OR has successfully finished sending all computation results to the edge server.
	(4)server_and<br>
	The Server AND is up and running using UDP on port 22023
	The Server AND start receiving lines from the edge server for AND computation.The computation results are:
	10111 and 101 is 101
	1001 and 11 is 1
	The Server AND has successfully received 2 lines from the edge server and finished all AND computations.
	The Server AND has successfully finished sending all computation results to the edge server.

g. Any idiosyncrasy of your project. It should say under what conditions the project fails, if any.<br>
	I didn't found any fails with my test case. Make sure that you obey the rules in e and have the correct format input file.

h. Reused Code: Did you use code from anywhere for your project? If not, say
so. If so, say what functions and where they're from. (Also identify this with a comment in the source code.)
	I just read some codes in "Beej's Guide to Network Programming -- Using Internet Sockets" and I rewrote it by myself as required.They are all marked in my codes.
