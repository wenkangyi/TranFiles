tfc:tfclient.o protocol.o sstime.o
	gcc tfclient.o protocol.o sstime.o -o tfc -g

protocol.o:../sharesource/protocol.c
	gcc ../sharesource/protocol.c -c

tfclient.o:tfclient.c
	gcc tfclient.c -c

sstime.o:../sharesource/sstime.c
	gcc ../sharesource/sstime.c -c