#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define RATE 200
struct sockaddr_in servaddr, cliaddr;
int sockfd;
long long packets = 0;
char app[]="1\n";

using namespace std;

int send_data(const boost::system::error_code& /*e*/,
		boost::asio::deadline_timer* t) {
	char sendline[]= "RANDOMDATA"; // 50 bytes payload
	char recvline[1000];
	app[0]=49+(packets%9);
	strcat(sendline,app);
	sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) &servaddr,
			sizeof(servaddr));


	printf("Packet Sent:%lld\n", packets++);

	t->expires_at(t->expires_at() + boost::posix_time::milliseconds(RATE));
	t->async_wait(boost::bind(send_data, boost::asio::placeholders::error, t));
}

int main(int argc, char**argv) {
	int n;

	if (argc != 2) {
		printf("usage:  udpcli <IP address>\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM for UDP

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(32000);

	boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::milliseconds(RATE));
	t.async_wait(boost::bind(send_data, boost::asio::placeholders::error, &t));

	io.run();

}

