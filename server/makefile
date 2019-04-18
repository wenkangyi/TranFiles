tfs:tfserver.o protocol.o sstime.o
	gcc tfserver.o protocol.o sstime.o -o tfs -g

tfserver.o:tfserver.c
	gcc tfserver.c -c

protocol.o:../sharesource/protocol.c
	gcc ../sharesource/protocol.c -c

sstime.o:../sharesource/sstime.c
	gcc ../sharesource/sstime.c -c