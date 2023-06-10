#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <vector>
#include <map>
#include <pthread.h>

// The port number on which the server listens for connections.
#define PORT 7654

// Map to store the client IDs associated with their sockets
std::map<int, int> clientIDs;
std::map<int, int> clientSockets;

/**

    @brief Handles the client connection and message handling.

    This function is executed in a separate thread for each client.

    @param clientSocket The socket file descriptor (fd) of the connected client.

    **/

void* handleClient(void* clientSocket) {

    int socket = *(int*)clientSocket;

    // Receive client ID from client
    int clientID;
    if (recv(socket, &clientID, sizeof(clientID), 0) <= 0) {
        std::cerr << "Failed to receive client ID.\n";
        close(socket);
        return nullptr;
    }

    clientIDs[socket] = clientID;
    clientSockets[clientID] = socket;

    std::cout << "Client " << clientID << " connected!\n";

    while (true) {
        // Receive message from client
        int destID, message;
        if (recv(socket, &destID, sizeof(destID), 0) <= 0 ||
            recv(socket, &message, sizeof(message), 0) <= 0) {
            break;
        }

        std::cout << "Client " << clientID << " sends message " << message
                  << " to client " << destID << ".\n";

        // Modify the message
        message += 1;

        // Check if the destination client exists
        if (clientSockets.find(destID) != clientSockets.end()) {
            // Send the modified message to the destination client
            int destSocket = clientSockets[destID];
            send(destSocket, &message, sizeof(message), 0);
        }
    }

    // Client disconnected, remove its data
    clientIDs.erase(socket);
    clientSockets.erase(clientID);

    std::cout << "Client " << clientID << " disconnected.\n";
    close(socket);

    return nullptr;
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Socket creation failed.\n";
        return -1;
    }

    // Set up server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the specified IP and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Binding failed.\n";
        return -1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listening failed.\n";
        return -1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        // Accept a new client connection
        clientAddrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket < 0) {
            std::cerr << "Acceptance failed.\n";
            return -1;
        }

        // Create a new thread to handle the client
        pthread_t thread;
        pthread_create(&thread, NULL, handleClient, &clientSocket);
        pthread_detach(thread);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
