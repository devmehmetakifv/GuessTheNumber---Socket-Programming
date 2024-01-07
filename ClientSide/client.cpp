#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 4337

int receive_generated_number_from_server(int, int*, unsigned long);
bool try_connect(int, struct sockaddr*, socklen_t);
int send_decision_to_server(int, char);

int main() {
    bool is_connection_established = false;
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t server_address_length = sizeof(server_address);

    while (true) {
        if (try_connect(client_socket, (struct sockaddr *)&server_address, server_address_length)) {
            is_connection_established = true;
            std::cout << "Connection established!" << std::endl;
            break;
        } else {
            std::cerr << "Connection couldn't be established! " << std::endl;
            std::cerr << "Error:" << errno << " Error Message: " << strerror(errno) << std::endl;
            sleep(1);
        }
    }

    while (is_connection_established) {
        int buffer;
        int received = receive_generated_number_from_server(client_socket, &buffer, sizeof(buffer));
        if (received > 0) {
            int lives = 3;
            bool game_loop = true;
            std::cout << "Game is on! " << std::endl;
            std::cout << "Guess the number between 0-10. You have 3 lives." << std::endl;
            
            while (game_loop) {
                int guess;
                std::cout << "Your guess: " << std::endl;
                std::cin >> guess;

                if (guess > buffer) {
                    lives--;
                    std::cout << "You have to guess lower! You have " << lives << " lives left!" << std::endl; 
                } else if (guess < buffer) {
                    lives--;
                    std::cout << "You have to guess higher! You have " << lives << " lives left!" << std::endl;
                } else {
                    std::cout << "Congratulations! Yes! " << guess << " was the correct answer!" << std::endl;
                    std::cout << "Wanna play again? (y / N): " << std::endl;
                    char answer;
                    std::cin >> answer;

                    if (answer == 'y') {
                        std::cout << "Setting up a new game, please be patient..." << std::endl;
                        int count = send_decision_to_server(client_socket, answer);
                        if (count > 0) {
                            std::cout << "Connected with server... New number is generating for you..." << std::endl;
                            sleep(3);
                            break;
                        } else {
                            std::cout << "Something was wrong with the connection. Server might be down." << std::endl;
                            std::cout << "Restart the server and try again!" << std::endl;
                        }
                    } else if (answer == 'N') {
                        std::cout << "Alrighty, thanks for playing!" << std::endl;
                        close(client_socket);
                        return 0;
                    }
                }
                if(lives == 0){
                    std::cout << "I'm sorry, you lost! " << buffer << " was the correct answer!" << std::endl;
                    std::cout << "Wanna play again? (y / N): " << std::endl;
                    char answer;
                    std::cin >> answer;

                    if (answer == 'y') {
                        std::cout << "Setting up a new game, please be patient..." << std::endl;
                        int count = send_decision_to_server(client_socket, answer);
                        if (count > 0) {
                            std::cout << "Connected with server... New number is generating for you..." << std::endl;
                            sleep(3);
                            break;
                        } else {
                            std::cout << "Something was wrong with the connection. Server might be down." << std::endl;
                            std::cout << "Restart the server and try again!" << std::endl;
                        }
                    } else if (answer == 'N') {
                        std::cout << "Alrighty, thanks for playing!" << std::endl;
                        close(client_socket);
                        return 0;
                    }
                }
            }
        } else {
            std::cerr << "Unable to receive data from the server. No messages received. The server might be down." << std::endl;
        }
    }
    return 0;
}

int receive_generated_number_from_server(int client_socket, int *buffer, unsigned long buffer_size) {
    int received_bytes = recv(client_socket, buffer, buffer_size, 0);
    return received_bytes;
}

int send_decision_to_server(int client_socket, char answer) {
    int sent_count = send(client_socket, &answer, sizeof(answer), 0);
    return sent_count;
}

bool try_connect(int client_socket, struct sockaddr * server_address, socklen_t socket_length) {
    int if_connected = connect(client_socket, server_address, socket_length);
    switch (if_connected) {
        case 0:
            return true;
        default:
            return false;
    }
}
