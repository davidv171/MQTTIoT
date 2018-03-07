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
int sendPacketBuffer(int asocket, char* host, int port, unsigned char* buf, int buflen)
{
		//SEND OVER TCP/IP
		//ASOCKET = socket, hardcoded 127.0.0.1:1883
		// host = 127.0.0.1
		//port = 1883
		//buf = the buffer, the MQTT formatted packet!
		//buflen = size of buf,
		printf("Sending .... ");
    struct sockaddr_in cliaddr;
    int rc = 0;
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
		//socket()>connect()->send()<->recv()->close
		int statusConn = connect(asocket,(const struct sockaddr*)&cliaddr,sizeof(cliaddr));
		if(statusConn=0){
			printf("Success!");
		}
		if(statusConn=-1){
			printf("Connect unsuccessful!\n");
		}
		printf("Sent packets: ");
		//send buffer to asocket, countSend is the count of packets sent!
		int countSend = send(asocket,buf,buflen,0);
		printf("%d",countSend);
		printf("\n");
    return 0;
}
int main (void) {

		//currently unused, will be used later when we use the chip
    int i;
    float temperatura=30;
    unsigned char buffer[50];


    // inicializacija itd.

		//initialize MQTTPacket
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
		//status checking integer, currently unused
    int rc = 0;
		//charracter array, used for inputting the buffer
    char buf[200];
    int mysock=0;
    char *host = "127.0.0.1";
    int port = 1883;
    mysock = socket(AF_INET, SOCK_STREAM, 0);

		//length of buffer
    int buflen = sizeof(buf);
		//MQTTStrings is the type used for setting of topic! Topic is set as "ka" later
		//set MQTT packet format
    MQTTString topicString = MQTTString_initializer;
    char* payload = "I'm Alive";
    int payloadlen = strlen(payload);
    int len = 0;
    data.clientID.cstring = "TESTIRAM";
    data.keepAliveInterval = 20;
    data.cleansession=1;
    data.MQTTVersion=3;
    len = MQTTSerialize_connect(buf,buflen,&data);
    topicString.cstring="ka";
    len += MQTTSerialize_publish(buf + len, buflen - len, 0, 0, 0, 0, topicString, payload, payloadlen);


    while(1)
    {
			printf("Sending to hostname %s port %d\n", host, port);
			//infinite loop, sending packets to the specified ASOCKET
			//after sending sleep for 10 seconds
			//sleep uses unistd.h
			rc = sendPacketBuffer(mysock,host,port,buf,len);

			sleep(10);



    }
}
