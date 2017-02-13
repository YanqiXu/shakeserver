#MakeFile to build and deploy the Sample US CENSUS Name Data using ajax
# For CSC3004 Software Development

# Put your user name below:
USER= xuy

CC= g++

#For Optimization
#CFLAGS= -O2
#For debugging
CFLAGS= -g

RM= /bin/rm -f

all: shakeserver testclient shakefetchajax PutCGI PutHTML
#all: shakeserver testclient 

testclient.o: testclient.cpp fifo.h
	$(CC) -c $(CFLAGS) testclient.cpp

shakeserver.o: shakeserver.cpp fifo.h fifo.o
	$(CC) -c $(CFLAGS) shakeserver.cpp

shakefetchajax.o: shakefetchajax.cpp fifo.h fifo.o
	$(CC) -c $(CFLAGS) shakefetchajax.cpp

testclient: testclient.o fifo.o
	$(CC) testclient.o fifo.o -o testclient

shakeserver: shakeserver.o fifo.o
	$(CC) shakeserver.o  fifo.o -o shakeserver

fifo.o:		fifo.cpp fifo.h
		g++ -c fifo.cpp
shakefetchajax: shakefetchajax.o  fifo.h
	$(CC) shakefetchajax.o  fifo.o -o shakefetchajax -L/usr/local/lib -lcgicc

PutCGI: shakefetchajax
	chmod 757 shakefetchajax
	cp shakefetchajax /usr/lib/cgi-bin/$(USER)_shakefetchajax.cgi 

	echo "Current contents of your cgi-bin directory: "
	ls -l /usr/lib/cgi-bin/

PutHTML:
	cp shake_ajax.html /var/www/html/class/softdev/$(USER)
	cp shake_ajax.css /var/www/html/class/softdev/$(USER)

	echo "Current contents of your HTML directory: "
	ls -l /var/www/html/class/softdev/$(USER)

clean:
	rm -f *.o shake_ajax shakeserver testclient
