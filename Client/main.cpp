#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>

#define PORT 7654

int clientSocket;

void* receiveMessages(void*) {
    while (true) {
        int modifiedMessage;
        if (recv(clientSocket, &modifiedMessage, sizeof(modifiedMessage), 0) <= 0) {
            break;
        }

        std::cout << "\nModified Message Received: " << modifiedMessage << "\nEnter Destination ID (0 to exit): ";
    }
    return nullptr;
}

int main() {
    struct sockaddr_in serverAddr;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Socket creation failed.\n";
        return -1;
    }

    // Set up server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed.\n";
        return -1;
    }

    // Get client ID from user
    int clientID;
    std::cout << "Set ID: ";
    std::cin >> clientID;

    // Send the client ID to the server
    send(clientSocket, &clientID, sizeof(clientID), 0);

    // Create a thread to receive messages from the server
    pthread_t receiveThread;
    pthread_create(&receiveThread, NULL, receiveMessages, NULL);

    while (true) {
        // Get destination ID from user
        int destID;
        std::cout << "Enter Destination ID (0 to exit): ";
        std::cin >> destID;

        if (destID == 0) {
            break;
        }

        // Get message from user
        int message;
        std::cout << "Enter Message: ";
        std::cin >> message;

        // Send the destination ID and message to the server
        send(clientSocket, &destID, sizeof(destID), 0);
        send(clientSocket, &message, sizeof(message), 0);
    }

    // Close the client socket
    close(clientSocket);

    return 0;
}
