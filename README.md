# Gossip-Server

This program consists of both a client and a server component. The server can handle multiple clients, allowing them to send messages to each other. Each client is assigned a unique identification number on the server, which serves as a means of recognition.

When a client sends a message to the server, the message includes the following information: the unique number of the source client, a numerical value, and the number of the destination client. The server receives the message and increments the numerical value inside it by one unit. The server then forwards the modified message to the destination client.

To visualize the flow of activities in the program, you can refer to the activity diagram provided below:

![activity](https://github.com/parisa-hr/Gossip-Server/blob/main/Doc/activity.svg)
