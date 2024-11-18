#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080        // Port number to connect to
#define BUFFER_SIZE 1024 // Buffer size for receiving data

int main(int argc, char *argv[])
{
    int status;

    // Create a socket for communication
    int sockfd = socket( // https://linux.die.net/man/2/socket
        AF_INET,         // Address family: IPv4
        SOCK_STREAM,     // Socket type: Stream (TCP)
        0                // Protocol: Default (TCP)
    );
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure the server address structure
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;         // Set the address family to IPv4
    server_address.sin_port = htons(PORT);       // Set the port number, converting to network byte order
    server_address.sin_addr.s_addr = INADDR_ANY; // Connect to localhost (use actual IP if needed)

    // Connect to the server
    status = connect( // https://linux.die.net/man/2/connect
        sockfd,                             // The socket file descriptor
        (struct sockaddr *)&server_address, // Pointer to the server address structure
        sizeof(server_address)              // The size of the address structure
    );
    if (status < 0) {
        perror("Connection to the server failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server on port %d\n", PORT);

    // Send a message to the server
    const char *message = "Hello from client"; // Message to send
    status = send(                             // https://linux.die.net/man/2/send
        sockfd,                                // Socket file descriptor
        message,                               // Pointer to the message to send
        strlen(message),                       // Length of the message
        0                                      // Flags: 0 for default behavior
    );
    if (status < 0) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
    printf("Message sent to the server: %s\n", message);

    // Receive a response from the server
    char buffer[BUFFER_SIZE] = {0}; // Buffer to store the response
    ssize_t valread = read(         // https://linux.die.net/man/2/read
        sockfd,                     // Socket file descriptor
        buffer,                     // Buffer to store the data
        BUFFER_SIZE - 1             // Maximum bytes to read
    );
    if (valread < 0) {
        perror("Read failed");
        exit(EXIT_FAILURE);
    }

    // Null-terminate the received string
    buffer[valread] = '\0';
    printf("Response from server: %s\n", buffer);

    // Close the socket
    close(sockfd);

    return 0;
}
