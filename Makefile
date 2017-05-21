all:
		gcc -o client client.c
		gcc -o edge edge.c
		gcc -o server_and server_and.c
		gcc -o server_or server_or.c
		
.PHONY: server_or
server_or:
		./server_or

.PHONY: server_and
server_and:
		./server_and

.PHONY: edge
edge:
		./edge
