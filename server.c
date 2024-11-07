#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080        // Port number the server will listen on
#define BUFFER_SIZE 1024 // The size of the buffer used for storing incoming data

int main(int argc, char *argv[])
{
    int status; // Declare `status` once here at the top for reuse in error checking

    // Create an endpoint for communication and returns a int descriptor
    int sockfd = socket( // https://linux.die.net/man/2/socket
        AF_INET,         // Address family: IPv4
        SOCK_STREAM,     // Socket type: Stream (TCP)
        0                // Protocol: Default (TCP)
    );
    if (sockfd < 0) { // Error handling
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Setting socket options
    int enable_reuse = 1;            // Option value to enable address reuse
    status = setsockopt(             // https://linux.die.net/man/2/setsockopt
        sockfd,                      // The socket file descriptor to configure
        SOL_SOCKET,                  // SOL_SOCKET applies options to the socket layer
        SO_REUSEADDR | SO_REUSEPORT, // The name of the option to set; SO_REUSEADDR
        &enable_reuse,               // A pointer to the option value
        sizeof(enable_reuse)         // The size of the option value; necessary to know how much memory to read from optval
    );
    if (status < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Configure the server address structure
    struct sockaddr_in address;
    address.sin_family = AF_INET;         // Set the address family to IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Bind the socket to all available network interfaces
    address.sin_port = htons(PORT);       // Set the port number, converting from host byte order to network byte order

    // Attempt to bind the socket to the specified address and port
    status = bind(                   // https://linux.die.net/man/2/bind
        sockfd,                      // The socket file descriptor created earlier
        (struct sockaddr *)&address, // Pointer to the sockaddr structure that holds the server's address information
        sizeof(address)              // The size of the address structure
    );
    if (status < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections on the socket we just created
    status = listen( // https://linux.die.net/man/2/listen
        sockfd,      // We know what this is by now
        3            // Maximum number of pending connections in the queue
    );
    if (status < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", PORT);

    // Accept incoming connections and wait for new client requests, which will create a new socket for communication while `sockfd` keeps listening
    int addrlen = sizeof(address);  // Length of the address structure
    int newsock = accept(           // https://linux.die.net/man/2/accept
        sockfd,                     // The listening socket file descriptor
        (struct sockaddr*)&address, // Pointer to the address structure to store client information
        (socklen_t*)&addrlen        // Pointer to the length of the address structure
    );
    if (newsock < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    // Read data from the client
    char buffer[BUFFER_SIZE] = {0}; // Allocated memory to store the received data, using a const to ensure the size matches the one in `read()` to avoid overflows
    ssize_t valread = read(         // https://linux.die.net/man/2/read
        newsock,                    // Socket file descriptor for the client connection
        buffer,                     // Buffer created above to hold the incoming data
        BUFFER_SIZE - 1             // Read up to buffer size minus 1 to leave space for the null terminator
    );
    if (valread < 0) {
        perror("read failed");
        exit(EXIT_FAILURE);
    }

    // Null-terminate the string to ensure safe printing
    // Without it functions like `printf()` may read beyond the buffer risking data leaks or crashes
    buffer[valread] = '\0';
    printf("Received from client: %s\n", buffer);

    // Send a response to the client
    char* hello = "Hello from server"; // Response message to be sent to the client
    status = send(                     // https://linux.die.net/man/2/send
        newsock,                       // Socket file descriptor for the client connection
        hello,                         // Pointer to the message to send
        strlen(hello),                 // Length of the message in bytes
        0                              // Flags: 0 for default behavior, the send operation is performed in a standard blocking manner
    );
    if (status < 0) {
        perror("send failed");
        exit(EXIT_FAILURE);
    }
    printf("Hello message sent\n");

    // Close the sockets
    close(newsock);
    close(sockfd);

    return 0;
}
