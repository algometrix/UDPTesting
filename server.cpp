#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define RATE 100
#define PRINT_RATE 1000

struct sockaddr_in servaddr, cliaddr;
int sockfd;
long long packets = 0;

void recv(const boost::system::error_code& /*e*/,
		boost::asio::deadline_timer* t) {

	int n;
	socklen_t len;
	char mesg[1000];
	len = sizeof(cliaddr);
	n = recvfrom(sockfd, mesg, 1000, MSG_DONTWAIT, (struct sockaddr *) &cliaddr,
			&len);
	if (n > 0) {

		packets++;

	}

	t->expires_at(t->expires_at() + boost::posix_time::milliseconds(RATE));
	t->async_wait(boost::bind(recv, boost::asio::placeholders::error, t));

}
void print_packets(const boost::system::error_code& /*e*/,
		boost::asio::deadline_timer* t) {


	printf("Received Packets:%lld\n", packets);

	t->expires_at(t->expires_at() + boost::posix_time::milliseconds(PRINT_RATE));
	t->async_wait(boost::bind(print_packets, boost::asio::placeholders::error, t));
}
int main(int argc, char**argv) {

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(32000);
	bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	boost::asio::io_service io;

	boost::asio::deadline_timer rec_data(io,
			boost::posix_time::milliseconds(RATE));
	boost::asio::deadline_timer print_data(io,
			boost::posix_time::milliseconds(PRINT_RATE));

	print_data.async_wait(
			boost::bind(print_packets, boost::asio::placeholders::error,
					&print_data));

	rec_data.async_wait(
			boost::bind(recv, boost::asio::placeholders::error, &rec_data));

	io.run();
}
