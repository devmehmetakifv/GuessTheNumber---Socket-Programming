#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <random>
#include <time.h>

#define PORT 4337

void send_generated_number_to_client(int, int);
int receive_client_decision(int, int*, unsigned long);
int generate_random_number();
int main() {

    // Create the socket.
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Create endpoint.
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket with endpoint.
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    // Let's listen incoming connections.
    int listen_result = listen(server_socket, 5);
    if (listen_result == -1) {
        std::cerr << "An error occurred while listening incoming requests..." << errno << std::endl;
        return -1;
    }
    std::cout << "Server is listening on port 4337..." << std::endl;

    bool server_on = true;
    while (server_on) {
        int generated_number = generate_random_number();
        int buffer;
        
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);

        // Let's accept incoming connection.
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket == -1) {
            std::cerr << "We couldn't accept the incoming connection." << std::endl;
        } else {
            std::cout << "Player connected. Player socket: " << client_socket << std::endl;
            send_generated_number_to_client(client_socket, generated_number);

            while (true) {
                std::cout << "Waiting input from client..." << std::endl;
                int rcvd = receive_client_decision(client_socket, &buffer, sizeof(buffer));
                if (rcvd > 0) {
                    std::cout << "Player wants to play again." << std::endl;
                    generated_number = generate_random_number();
                    send_generated_number_to_client(client_socket, generated_number);
                } else if (rcvd == 0) {
                    std::cout << "Client disconnected." << std::endl;
                    close(server_socket);
                    exit(0);
                } else {
                    std::cout << "An unexpected error occured. Please restart the game." << std::endl;
                    close(server_socket);
                    exit(0);
                }
            }
        }
    }
    return 0;
}

void send_generated_number_to_client(int client_socket, int generated_number) {
    int numbers_sent = send(client_socket, &generated_number, sizeof(generated_number), 0);
    (numbers_sent > 0) ? std::cout << "Generated number has been sent to client." << std::endl : std::cerr << "Something wrong with sending the data!" << std::endl;
}

int receive_client_decision(int client_socket, int *buffer, unsigned long buffer_size) {
    int received_bytes = recv(client_socket, buffer, buffer_size, 0);
    return received_bytes;
}

int generate_random_number(){
    std::srand(static_cast<unsigned int>(time(nullptr)));
    int generated_number = (std::rand() % 10) + 1; // Generate number between 1 and 10
    return generated_number;
}