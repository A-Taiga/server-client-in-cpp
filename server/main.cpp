


#include <iostream>
#include <string>
#include <array>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


#define CLEAR   "\e[2J\e[3J\e[H"
#define BLACK   "\x1B[30;1m"
#define RED     "\x1B[31;1m"
#define GREEN   "\x1B[32;1m"
#define YELLOW  "\x1B[33;1m"
#define BLUE    "\x1B[34;1m"
#define MAGENTA "\x1B[35;1m"
#define CYAN    "\x1B[36;1m"
#define WHITE   "\x1B[37;1m"
#define RESET   "\x1B[0m"

void error(std::string&& msg)
{
    std::cerr << msg << '\n';
    std::exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    /*
        // file desciptors 
        array subscripts into the file descriptor table.
        these two variables store the valuse returned by 
        the socket system call and the accept system call
    */
    int socket_file_descriptor = 0;
    int new_socket_file_descriptor = 0;

    int port_num = 0;         // stores port number
    int client_length = 0;   // stores the size of the address of the client (needed to accept system calls)

    size_t n = 0;  // return value fo the read() and write() calls; contains the number of character read or written 
    // std::vector<char> buffer{0}; //read characters from socket connection into buffer
    std::array<std::byte, 256> buffer = {};
    // structers containing internet address for both server and client
    sockaddr_in server_address = {};
    sockaddr_in client_address = {};

    if(argc < 2)
    {
        std::cerr << "ERROR: NO PORT PROVIDED\n";
        std::exit(EXIT_FAILURE);
    }
    /*
        socket(int domain, int type, int protocol);
        
        domain
            AF_INET = internet domain
            AF_UNIX = local file system domain

        type 
            SOCK_STREAM = messages are read in a continuous stream
            SOCK_DGRAM =  messages are read in chunks 

        protocol
            always should be 0 (except for unusual circumstances) 
            OS will pick appropriate protocol
            TCP for stream sockets
            UDP for datagram sockets

    */

    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0); // return a small number to the file descriptor table
    port_num = std::stoi(argv[1]);
    server_address.sin_family = AF_INET;    // address family

    /*
    This is an IP address that is used when we don't want to 
    bind a socket to any specific IP. Basically, while implementing 
    communication, we need to bind our socket to an IP address. When we 
    don't know the IP address of our machine, we can use the special IP address INADDR_ANY
    */
    server_address.sin_addr.s_addr  = INADDR_ANY;
    // htons converts port number in host byte order to a port number in network byte order
    server_address.sin_port = htons(port_num);
    //binds a socket to an address
    /*
        second argument must be cast to sockaddr because server_address is of type sockaddr_in
    */

    int option = 1;
    n = setsockopt(socket_file_descriptor,SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if(bind(socket_file_descriptor,(sockaddr *) &server_address,sizeof(server_address)) < 0)
        error("ERROR ON BINDING");
    // allow socket to on the socket for conections
    // 2nd argunnet is the size of the backlog queue 
    // (number of connections that can be written while the process is handling a particular connection )
    // should be set to 5 which is the max on most systems

    listen(socket_file_descriptor, 5);

    client_length = sizeof(client_address);
    new_socket_file_descriptor = accept(socket_file_descriptor, (struct sockaddr *) &client_address, (socklen_t *)&client_length);

    if(new_socket_file_descriptor < 0)
        error("ERROR ON ACCEPT");

    while(true)
    {

    
    n = read(new_socket_file_descriptor, buffer.data(), buffer.size()); 

    if(n < 0) error("ERROR: READING FROM SOCKET");

    std::string s(reinterpret_cast<char*>(buffer.data()), buffer.size());
    std::cout << GREEN << s << RESET << std::endl;

    std::string m = GREEN;
    m += "nutz\n";
    n = write(new_socket_file_descriptor, &m, sizeof(m));

    }

    return 0;
}
