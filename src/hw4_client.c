#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "hw4.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int count_string_length(const char *str)
{
    int len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return len;
}

void delete_newline_character(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '\n')
        {
            str[i] = '\0';
            break;
        }
        i++;
    }
}

int compare_strings2(const char *str1, const char *str2)
{
    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

int receive_data_from_socket(int sockfd, char *buf, int len)
{
    int total_received = 0;
    int bytes_received;
    while (total_received < len)
    {
        bytes_received = recv(sockfd, buf + total_received, len - total_received, 0);
        if (bytes_received == -1)
            return -1;
        if (bytes_received == 0)
            break;
        total_received += bytes_received;
    }
    return total_received;
}

int main()
{
    ChessGame game;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    // Connect to the server
    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(connfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    initialize_game(&game);
    display_chessboard(&game);

    while (1)
    {

        printf("Enter a command: ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            printf("Error reading command.\n");
            continue;
        }

        delete_newline_character(buffer);

        if (send(connfd, buffer, count_string_length(buffer), 0) < 0)
        {
            break;
        }

        if (compare_strings2(buffer, "/forfeit") == 0)
        {
            printf("Forfeiting the game. \n");
            break;
        }

        int n = receive_data_from_socket(connfd, buffer, BUFFER_SIZE);
        if (n < 0)
        {
            break;
        }
        else if (n == 0)
        {
            printf("Server closed the connection.\n");
            break;
        }

        buffer[n] = '\0';
        printf("Server says: %s\n", buffer);

        display_chessboard(&game);
    }
    // Please ensure that the following lines of code execute just before your program terminates.
    // If necessary, copy and paste it to other parts of your code where you terminate your program.
    FILE *temp = fopen("./fen.txt", "w");
    char fen[200];
    chessboard_to_fen(fen, &game);
    fprintf(temp, "%s", fen);
    fclose(temp);
    close(connfd);
    return 0;
}
