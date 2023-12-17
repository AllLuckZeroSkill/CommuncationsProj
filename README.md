*** BEFORE STARTING THE PROJECT IN VISUAL STUDIOS MAKE SURE TO CONSIDER THESE PRECONDITIONS ***


If you get this error: Error	LNK2019	unresolved external symbol __imp__WSAStartup@8 referenced in function _init_winsock	
Follow these steps:

The problem is that winsock is not included by default in Visual Studios, so we have to link it.

-Right-click on your project in the Solution Explorer and select "Properties" from the context menu.

-In the Properties window, select "Linker" > "Input" from the left-hand menu.

-In the "Additional Dependencies" field, add "ws2_32.lib" (without quotes) to the list of libraries. (Should look similar to this: kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;ws2_32.lib;%(AdditionalDependencies))

- Click "APPLY" and "OK" to close the Properties window.

Rebuild your project.


# UDP File Transfer Application in C

This project presents a simple UDP file transfer application written in C, demonstrating a client-server architecture for sending and receiving files over a network. The project includes two main components: a server (`receive.c`) and a client (`send.c`), working together to transfer a file, specifically an image, from the client to the server.

## Features

- **UDP Protocol:** Utilizes User Datagram Protocol (UDP) for transferring files, offering an example of handling packet transmission without built-in reliability.
- **File Transfer:** Capable of sending and receiving binary files, specifically tailored for image files.
- **Error Handling:** Includes basic error handling for socket operations and file I/O.
- **Packet Sequencing:** Implements packet sequencing to ensure the order of received data and basic error handling for out-of-order packets.
- **Acknowledgements (ACKs):** Implements a simple acknowledgement system to confirm the receipt of packets.
- **Non-Blocking Sockets:** Utilizes non-blocking sockets on the client side for handling timeouts and retransmissions.
- **Performance Metrics:** Measures and prints the time taken to send each packet.

## Components

### `receive.c`

This is the server component of the application. It initializes a UDP socket, binds to a specified address and port, and then enters a loop to receive data packets. Each packet's sequence number is checked to ensure data integrity. The received data is written to a file, `received_image.jpg`.

Key functions:
- `create_and_bind_server_socket()`: Creates and binds the server socket.
- `server_receive_data()`: Receives data packets and writes them to a file.

### `send.c`

This file acts as the client. It reads an image file into a buffer and sends it to the server in packets. It also handles packet sequencing, acknowledgements, and retransmissions in case of timeouts.

Key functions:
- `read_image_file()`: Reads an image file into a buffer.
- `create_client_socket_and_send_data()`: Creates a client socket and sends data to the server.
- `send_packet()`: Sends individual packets and handles ACKs and timeouts.

## Getting Started

1. **Initial Setup**: Ensure that Winsock2 API is available on your system as the project uses Winsock2 for socket programming.
2. **Compile the Code**: Use a C compiler like GCC to compile `receive.c` and `send.c`. Example: `gcc receive.c -o receive -lws2_32` and `gcc send.c -o send -lws2_32`.
3. **Run the Server**: Start the server application (`receive`).
4. **Run the Client**: Execute the client application (`send`), pointing it to the image file you want to send.

## Note

- The IP address and port are hardcoded to `127.0.0.1` and `80`, respectively. Modify these values as needed to suit your network configuration.
- This application is a basic example and is not suitable for production use without further enhancements, particularly in terms of error handling and security.

## Contributing

Contributions to enhance features, improve error handling, or refine the code structure are welcome. Please feel free to submit pull requests or open issues for discussion.

---

*This project is for educational purposes and demonstrates basic socket programming and file transfer using UDP in C.*

