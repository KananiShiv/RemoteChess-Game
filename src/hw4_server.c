#include "hw4.h"

int main()
{
    int listenfd, connfd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char client_input[BUFFER_SIZE];
    char server_input[BUFFER_SIZE];

    // Create socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set options to reuse the IP address and IP port if either is already in use
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))");
        return -1;
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))");
        return -1;
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(listenfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(listenfd, 1) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    INFO("Server listening on port %d", PORT);
    // Accept incoming connection
    if ((connfd = accept(listenfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    INFO("Server accepted connection");

    ChessGame game;
    initialize_game(&game);
    display_chessboard(&game);

    while (1)
    {
        int read_size = read(connfd, client_input, sizeof(client_input) - 1);
        if (read_size < 0)
        {
            break;
        }
        else if (read_size == 0)
        {
            INFO("Client disconnected");
            break;
        }
        client_input[read_size] = '\0';

        int command_result = receive_command(&game, client_input, connfd, false);
        if (command_result == COMMAND_FORFEIT)
        {
            INFO("Client has forfeited the game");
            break;
        }

        printf("Enter your move: ");
        if (fgets(server_input, sizeof(server_input), stdin) == NULL)
        {
            INFO("Error reading input");
            continue;
        }

        int send_result;
        do
        {
            send_result = send_command(&game, server_input, connfd, false);
            if (send_result == COMMAND_ERROR || send_result == COMMAND_UNKNOWN)
            {
                printf("Invalid command, please try again: ");
                if (fgets(server_input, sizeof(server_input), stdin) == NULL)
                {
                    INFO("Error reading input");
                    continue;
                }
            }
            else if (send_result == COMMAND_FORFEIT)
            {
                INFO("Server has forfeited the game");
                break;
            }
        } while (send_result == COMMAND_ERROR || send_result == COMMAND_UNKNOWN);
    }

    close(connfd);
    close(listenfd);
    INFO("Server terminated");
    return 0;
}