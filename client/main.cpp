#include <array>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error(std::string &&msg)
{
	std::cerr << "ERROR: " << msg << std::endl;
	std::exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{

	int socket_file_descriptor = 0;
	int port_num               = 0;
	int n                      = 0;

	sockaddr_in server_address        = {};
	hostent *server                   = {};
	std::array<std::byte, 256> buffer = {};

	if (argc < 3)
		error("ARGUMENT ERROR");

	port_num               = std::stoi(argv[2]);
	socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_file_descriptor < 0)
		error("OPEN SOCKET ERROR");

	server = gethostbyname(argv[1]);
	if (server == nullptr)
		error("NO SUCH HOST");

	server_address.sin_family = AF_INET;

	std::memcpy(server->h_addr, &server_address.sin_addr.s_addr,
	            server->h_length);
	server_address.sin_port = htons(port_num);

	if (connect(socket_file_descriptor, (sockaddr *)&server_address,
	            sizeof(server_address)) < 0)
		error("CONNECTION ERROR");
	while (true)
	{
		std::string msg = std::string(buffer.size(), '\0');

		std::cout << "Message: ";

		std::getline(std::cin, msg);

		std::memcpy(buffer.data(), msg.c_str(), buffer.size());

		n = write(socket_file_descriptor, buffer.data(), buffer.size());

		if (n < 0)
			error("WRITE ERROR");
	}

	return 0;
}