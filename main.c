#include "hw_util.h"
#include "MQTTPacket.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int initialize_conn(int asocket, char* host, int port){

	//SEND OVER TCP/IP
	//ASOCKET = socket, hardcoded 127.0.0.1:1883
	// host = 127.0.0.1
	//port = 1883
	//buf = the buffer, the MQTT formatted packet!
	//buflen = size of buf,
	printf("Init .... ");
	printf("\n");
	struct sockaddr_in cliaddr;
	//initialize the host address

	memset(&cliaddr, 0, sizeof(cliaddr));
	//specify IPv4 protocol
	/*following includes are necessary for this:#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>*/
	cliaddr.sin_family = AF_INET;
	//intialize the IP address of the host as "127.0.0.1"
	cliaddr.sin_addr.s_addr = inet_addr(host);
	//initialize the port, 1883
	cliaddr.sin_port = htons(port);
	//client has no bind
	//socket()>connect()->send()
	int statusConn = connect(asocket,(const struct sockaddr*)&cliaddr,sizeof(cliaddr));
	if(statusConn=0){
		printf("Success!");
	}
	if(statusConn=-1){
		printf("Connect unsuccessful!\n");
	}
}
int sendPacketBuffer(int asocket, unsigned char* buf, int buflen)
{

		printf("Sent packets: ");
		//send buffer to asocket, countSend is the count of packets sent!
		//MSG_NOSIGNAL IS A CONSTANT THAT DOESNT SEND THE SIGNAL, WH0EN THE SEND FAILS!

		int countSend = send(asocket,buf,buflen,MSG_NOSIGNAL);
		printf("%d",countSend);

		printf("\n");
    return countSend;
}
int main (void) {
	int i;
	float temperatura=30;
	char buf[200];



    // inicializacija itd.

		//initialize MQTTPacket
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
		//status checking integer, currently unused
    int rc = 0;
		//charracter array, used for inputting the buffer
    int mysock=0;
    char *host = "127.0.0.1";
    int port = 1883;
    mysock = socket(AF_INET, SOCK_STREAM, 0);
		initialize_conn(mysock,host,port);

		//MQTTStrings is the type used for setting of topic! Topic is set as "ka" later
		//set MQTT packet format
    MQTTString topicString = MQTTString_initializer;
    char* payload = "I'm alive!";
    int payloadlen = strlen(payload);
    int len = 0;
    data.clientID.cstring = "TESTIRAM";
    data.keepAliveInterval = 20;
    data.cleansession=1;
    data.MQTTVersion=3;


    while(1)
    {


			//TODO: WHY EVERY 5TH PACKET FAILS TO SEND? CONNECTED TO BUFFER SIZE!
			//currently unused, will be used later when we use the chip

			int buflen = sizeof(buf);

			len = MQTTSerialize_connect(buf,buflen,&data);

			topicString.cstring="Topic";
			len += MQTTSerialize_publish(buf + len, buflen - len, 0, 0, 0, 0, topicString, payload, payloadlen);
			len += MQTTSerialize_disconnect(buf + len, buflen - len);

			printf("Sending to hostname %s port %d\n", host, port);
			//infinite loop, sending packets to the specified ASOCKET
			//after sending sleep for X seconds
			//sleep uses unistd.h

			rc = sendPacketBuffer(mysock,buf,len);
			//IF NO PACKETS WERE SENT REINITIALIZE THE CONNECTION
			//RESTART SOCKET
			if(rc==-1){
				printf("Send error, retrying....");
				printf("\n");
				mysock = socket(AF_INET, SOCK_STREAM, 0);
				initialize_conn(mysock,host,port);
			}
			sleep(10);



    }
}
