#include "hw4.h"

void initialize_game(ChessGame *game)
{
    game->moveCount = 0;
    game->capturedCount = 0;

    game->currentPlayer = WHITE_PLAYER;

    game->chessboard[0][0] = 'r';
    game->chessboard[0][1] = 'n';
    game->chessboard[0][2] = 'b';
    game->chessboard[0][3] = 'q';
    game->chessboard[0][4] = 'k';
    game->chessboard[0][5] = 'b';
    game->chessboard[0][6] = 'n';
    game->chessboard[0][7] = 'r';

    game->chessboard[1][0] = 'p';
    game->chessboard[1][1] = 'p';
    game->chessboard[1][2] = 'p';
    game->chessboard[1][3] = 'p';
    game->chessboard[1][4] = 'p';
    game->chessboard[1][5] = 'p';
    game->chessboard[1][6] = 'p';
    game->chessboard[1][7] = 'p';

    for (int i = 2; i < 6; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            game->chessboard[i][j] = '.';
        }
    }

    game->chessboard[7][0] = 'R';
    game->chessboard[7][1] = 'N';
    game->chessboard[7][2] = 'B';
    game->chessboard[7][3] = 'Q';
    game->chessboard[7][4] = 'K';
    game->chessboard[7][5] = 'B';
    game->chessboard[7][6] = 'N';
    game->chessboard[7][7] = 'R';

    game->chessboard[6][0] = 'P';
    game->chessboard[6][1] = 'P';
    game->chessboard[6][2] = 'P';
    game->chessboard[6][3] = 'P';
    game->chessboard[6][4] = 'P';
    game->chessboard[6][5] = 'P';
    game->chessboard[6][6] = 'P';
    game->chessboard[6][7] = 'P';

    for (int i = 0; i < MAX_CAPTURED_PIECES; i++)
    {
        game->capturedPieces[i] = 0;
    }

    for (int i = 0; i < MAX_MOVES; i++)
    {
        game->moves[i].startSquare[0] = '\0';
        game->moves[i].endSquare[0] = '\0';
    }
}

void chessboard_to_fen(char fen[], ChessGame *game)
{
    int fenIndex = 0;
    for (int row = 0; row < 8; row++)
    {
        int emptyCount = 0;
        for (int col = 0; col < 8; col++)
        {
            char piece = game->chessboard[row][col];
            if (piece == '.')
            {
                emptyCount++;
            }
            else
            {
                if (emptyCount > 0)
                {
                    fen[fenIndex++] = '0' + emptyCount;
                    emptyCount = 0;
                }
                fen[fenIndex++] = piece;
            }
        }
        if (emptyCount > 0)
        {
            fen[fenIndex++] = '0' + emptyCount;
        }
        if (row < 7)
        {
            fen[fenIndex++] = '/';
        }
    }
    fen[fenIndex++] = ' ';
    fen[fenIndex++] = (game->currentPlayer == WHITE_PLAYER) ? 'w' : 'b';

    fen[fenIndex] = '\0';
}

bool is_valid_pawn_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game)
{
    if (piece == 'P')
    {
        if (src_col == dest_col && game->chessboard[dest_row][dest_col] == '.')
        {
            if (src_row - 1 == dest_row)
            {
                return true;
            }
            else if (src_row == 6 && src_row - 2 == dest_row && game->chessboard[src_row - 1][src_col] == '.')
            {
                return true;
            }
        }
        else if (((src_col - dest_col == 1) || (dest_col - src_col == 1)) && src_row - 1 == dest_row && islower(game->chessboard[dest_row][dest_col]))
        {
            return true;
        }
    }
    else if (piece == 'p')
    {
        if (src_col == dest_col && game->chessboard[dest_row][dest_col] == '.')
        {
            if (src_row + 1 == dest_row)
            {
                return true;
            }
            else if (src_row == 1 && src_row + 2 == dest_row && game->chessboard[src_row + 1][src_col] == '.')
            {
                return true;
            }
        }
        else if (((src_col - dest_col == 1) || (dest_col - src_col == 1)) && src_row + 1 == dest_row && isupper(game->chessboard[dest_row][dest_col]))
        {
            return true;
        }
    }
    return false;
}

bool is_valid_rook_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game)
{
    if (src_row != dest_row && src_col != dest_col)
    {
        return false;
    }
    int stepRow = 0;
    int stepCol = 0;

    if (dest_row > src_row)
    {
        stepRow = 1;
    }
    else if (dest_row < src_row)
    {
        stepRow = -1;
    }

    if (dest_col > src_col)
    {
        stepCol = 1;
    }
    else if (dest_col < src_col)
    {
        stepCol = -1;
    }

    int curRow = src_row + stepRow;
    int curCol = src_col + stepCol;

    while (curRow != dest_row || curCol != dest_col)
    {
        if (game->chessboard[curRow][curCol] != '.')
        {
            return false;
        }
        curRow += stepRow;
        curCol += stepCol;
    }

    return true;
}

bool is_valid_knight_move(int src_row, int src_col, int dest_row, int dest_col)
{
    int rowDiff, colDiff;

    if (src_row > dest_row)
        rowDiff = src_row - dest_row;
    else
        rowDiff = dest_row - src_row;

    if (src_col > dest_col)
        colDiff = src_col - dest_col;
    else
        colDiff = dest_col - src_col;

    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

bool is_valid_bishop_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game)
{
    int rowDiff, colDiff;
    if (src_row > dest_row)
    {
        rowDiff = src_row - dest_row;
    }
    else
    {
        rowDiff = dest_row - src_row;
    }
    if (src_col > dest_col)
    {
        colDiff = src_col - dest_col;
    }
    else
    {
        colDiff = dest_col - src_col;
    }

    if (rowDiff != colDiff)
    {
        return false;
    }

    int rowStep = 0;
    int colStep = 0;
    if (dest_row > src_row)
    {
        rowStep = 1;
    }
    else if (dest_row < src_row)
    {
        rowStep = -1;
    }
    if (dest_col > src_col)
    {
        colStep = 1;
    }
    else if (dest_col < src_col)
    {
        colStep = -1;
    }

    int curRow = src_row + rowStep;
    int curCol = src_col + colStep;
    while (curRow != dest_row)
    {
        if (game->chessboard[curRow][curCol] != '.')
        {
            return false;
        }
        curRow += rowStep;
        curCol += colStep;
    }

    return true;
}

bool is_valid_queen_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game)
{
    if (src_row == dest_row || src_col == dest_col)
    {
        if (src_row != dest_row && src_col != dest_col)
        {
            return false;
        }
        int stepRow = 0;
        int stepCol = 0;

        if (dest_row > src_row)
        {
            stepRow = 1;
        }
        else if (dest_row < src_row)
        {
            stepRow = -1;
        }

        if (dest_col > src_col)
        {
            stepCol = 1;
        }
        else if (dest_col < src_col)
        {
            stepCol = -1;
        }

        int curRow = src_row + stepRow;
        int curCol = src_col + stepCol;

        while (curRow != dest_row || curCol != dest_col)
        {
            if (game->chessboard[curRow][curCol] != '.')
            {
                return false;
            }
            curRow += stepRow;
            curCol += stepCol;
        }
        return true;
    }

    int rowDiff;
    int colDiff;

    if (src_row > dest_row)
    {
        rowDiff = src_row - dest_row;
    }
    else
    {
        rowDiff = dest_row - src_row;
    }

    if (src_col > dest_col)
    {
        colDiff = src_col - dest_col;
    }
    else
    {
        colDiff = dest_col - src_col;
    }

    if (rowDiff == colDiff)
    {
        int rowStep = 0;
        int colStep = 0;

        if (dest_row > src_row)
        {
            rowStep = 1;
        }
        else if (dest_row < src_row)
        {
            rowStep = -1;
        }

        if (dest_col > src_col)
        {
            colStep = 1;
        }
        else if (dest_col < src_col)
        {
            colStep = -1;
        }

        int curRow = src_row + rowStep;
        int curCol = src_col + colStep;

        while (curRow != dest_row)
        {
            if (game->chessboard[curRow][curCol] != '.')
            {
                return false;
            }
            curRow += rowStep;
            curCol += colStep;
        }
        return true;
    }

    return false;
}

bool is_valid_king_move(int src_row, int src_col, int dest_row, int dest_col)
{
    int rowDiff;
    int colDiff;

    if (src_row > dest_row)
    {
        rowDiff = src_row - dest_row;
    }
    else
    {
        rowDiff = dest_row - src_row;
    }

    if (src_col > dest_col)
    {
        colDiff = src_col - dest_col;
    }
    else
    {
        colDiff = dest_col - src_col;
    }

    if (rowDiff <= 1 && colDiff <= 1)
    {
        if (src_row != dest_row || src_col != dest_col)
        {
            return true;
        }
    }

    return false;
}

bool is_valid_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game)
{
    char lowerPiece = tolower(piece);

    if (lowerPiece == 'p')
    {
        return is_valid_pawn_move(piece, src_row, src_col, dest_row, dest_col, game);
    }
    else if (lowerPiece == 'r')
    {
        return is_valid_rook_move(src_row, src_col, dest_row, dest_col, game);
    }
    else if (lowerPiece == 'n')
    {
        return is_valid_knight_move(src_row, src_col, dest_row, dest_col);
    }
    else if (lowerPiece == 'b')
    {
        return is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game);
    }
    else if (lowerPiece == 'q')
    {
        return is_valid_queen_move(src_row, src_col, dest_row, dest_col, game);
    }
    else if (lowerPiece == 'k')
    {
        return is_valid_king_move(src_row, src_col, dest_row, dest_col);
    }
    else
    {
        return false;
    }
}

void fen_to_chessboard(const char *fen, ChessGame *game)
{
    int row = 0;
    int col = 0;

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            game->chessboard[i][j] = '.';
        }
    }
    for (const char *p = fen; *p; ++p)
    {
        if (*p == '/')
        {
            row++;
            col = 0;
        }
        else if (*p >= '1' && *p <= '8')
        {
            int emptySquares = *p - '0';
            col += emptySquares;
        }
        else if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z'))
        {
            game->chessboard[row][col] = *p;
            col++;
        }
        else if (*p == ' ')
        {
            ++p;
            if (*p == 'w')
            {
                game->currentPlayer = WHITE_PLAYER;
            }
            else if (*p == 'b')
            {
                game->currentPlayer = BLACK_PLAYER;
            }
            break;
        }
    }
    game->moveCount = 0;
    game->capturedCount = 0;
}

int string_length(const char *str)
{
    int len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return len;
}

bool is_valid_column(char c)
{
    return c >= 'a' && c <= 'h';
}

bool is_valid_row(char c)
{
    return c >= '1' && c <= '8';
}

bool is_valid_promotion_char(char c)
{
    return c == 'q' || c == 'r' || c == 'b' || c == 'n';
}

int parse_move(const char *move, ChessMove *parsed_move)
{
    int len = string_length(move);

    if (len < 4 || len > 5)
    {
        return PARSE_MOVE_INVALID_FORMAT;
    }

    if (!is_valid_column(move[0]) || !is_valid_column(move[2]))
    {
        return PARSE_MOVE_INVALID_FORMAT;
    }

    if (!is_valid_row(move[1]) || !is_valid_row(move[3]))
    {
        return PARSE_MOVE_OUT_OF_BOUNDS;
    }

    parsed_move->startSquare[0] = move[0];
    parsed_move->startSquare[1] = move[1];
    parsed_move->startSquare[2] = '\0';

    if (len == 5)
    {
        if (!is_valid_promotion_char(move[4]))
        {
            return PARSE_MOVE_INVALID_PROMOTION;
        }
        if (!((move[3] == '8' && move[1] == '7') || (move[3] == '1' && move[1] == '2')))
        {
            return PARSE_MOVE_INVALID_DESTINATION;
        }
        parsed_move->endSquare[0] = move[2];
        parsed_move->endSquare[1] = move[3];
        parsed_move->endSquare[2] = move[4];
        parsed_move->endSquare[3] = '\0';
    }
    else
    {
        parsed_move->endSquare[0] = move[2];
        parsed_move->endSquare[1] = move[3];
        parsed_move->endSquare[2] = '\0';
    }

    return 0;
}

bool is_players_turn(bool is_client, int currentPlayer)
{
    return (currentPlayer == WHITE_PLAYER && is_client) || (currentPlayer == BLACK_PLAYER && !is_client);
}

bool is_piece_correct_color(char piece, bool is_client)
{
    return (is_client && isupper(piece)) || (!is_client && islower(piece));
}

int make_move(ChessGame *game, ChessMove *move, bool is_client, bool validate_move)
{

    int startCol = move->startSquare[0] - 'a';
    int startRow = 8 - (move->startSquare[1] - '0');
    int destCol = move->endSquare[0] - 'a';
    int destRow = 8 - (move->endSquare[1] - '0');

    char piece = game->chessboard[startRow][startCol];
    char targetPiece = game->chessboard[destRow][destCol];

    if (!is_players_turn(is_client, game->currentPlayer))
    {
        return MOVE_OUT_OF_TURN;
    }

    if (piece == '.')
    {
        return MOVE_NOTHING;
    }

    if (!is_piece_correct_color(piece, is_client))
    {
        return MOVE_WRONG_COLOR;
    }

    if (is_piece_correct_color(targetPiece, is_client) && targetPiece != '.')
    {
        return MOVE_SUS;
    }

    if (targetPiece != '.')
    {
        game->capturedPieces[game->capturedCount++] = targetPiece;
    }

    if (validate_move)
    {
        bool validMove = false;
        switch (tolower(piece))
        {
        case 'p':
            validMove = is_valid_pawn_move(piece, startRow, startCol, destRow, destCol, game);
            break;
        case 'r':
            validMove = is_valid_rook_move(startRow, startCol, destRow, destCol, game);
            break;
        case 'n':
            validMove = is_valid_knight_move(startRow, startCol, destRow, destCol);
            break;
        case 'b':
            validMove = is_valid_bishop_move(startRow, startCol, destRow, destCol, game);
            break;
        case 'q':
            validMove = is_valid_queen_move(startRow, startCol, destRow, destCol, game);
            break;
        case 'k':
            validMove = is_valid_king_move(startRow, startCol, destRow, destCol);
            break;
        }
        if (!validMove)
            return MOVE_WRONG;
    }

    if ((piece == 'P' && destRow == 0) || (piece == 'p' && destRow == 7))
    {
        char promotionPiece = move->endSquare[2];
        if (promotionPiece != '\0')
        {
            if (is_client)
            {
                piece = toupper(promotionPiece);
            }
            else
            {
                piece = tolower(promotionPiece);
            }
        }
    }

    game->chessboard[destRow][destCol] = piece;
    game->chessboard[startRow][startCol] = '.';

    if (game->moveCount < MAX_MOVES)
    {
        game->moves[game->moveCount++] = *move;
    }

    if (game->currentPlayer == WHITE_PLAYER)
    {
        game->currentPlayer = BLACK_PLAYER;
    }
    else
    {
        game->currentPlayer = WHITE_PLAYER;
    }

    return 0;
}

int count_characters(const char *str)
{
    int count = 0;
    while (*str != '\0')
    {
        str++;
        count++;
    }
    return count;
}

int compare_strings(const char *str1, const char *str2, int limit)
{
    while (limit-- && *str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    if (limit < 0)
    {
        return 0;
    }
    else
    {
        return *str1 - *str2;
    }
}

bool strings_equal(const char *str1, const char *str2)
{
    while (*str1 && *str2 && *str1 == *str2)
    {
        str1++;
        str2++;
    }
    return *str1 == '\0' && *str2 == '\0';
}

int send_command(ChessGame *game, const char *message, int socketfd, bool is_client)
{
    if (!message || count_characters(message) < 1)
    {
        return COMMAND_ERROR;
    }

    if (compare_strings(message, "/move ", 6) == 0)
    {
        ChessMove move;
        if (parse_move(message + 6, &move) == 0)
        {
            if (make_move(game, &move, is_client, true) == 0)
            {
                return COMMAND_MOVE;
            }
            else
            {
                printf("Move execution failed\n");
                return COMMAND_ERROR;
            }
        }
        else
        {
            printf("Move parsing failed\n");
            return COMMAND_ERROR;
        }
    }
    else if (strings_equal(message, "/forfeit"))
    {
        send(socketfd, message, count_characters(message), 0);
        return COMMAND_FORFEIT;
    }
    else if (strings_equal(message, "/chessboard"))
    {
        display_chessboard(game);
        return COMMAND_DISPLAY;
    }
    else if (compare_strings(message, "/import ", 8) == 0 && !is_client)
    {
        fen_to_chessboard(message + 8, game);
        if (send(socketfd, message, count_characters(message), 0) < 0)
        {
            return COMMAND_ERROR;
        }
        return COMMAND_IMPORT;
    }
    else if (compare_strings(message, "/load ", 6) == 0)
    {
        char username[100];
        int save_number = 0;
        int index = 6;
        char *ptr = username;
        while (message[index] == ' ')
            index++;
        while (message[index] != ' ' && message[index] != '\0')
        {
            *ptr++ = message[index++];
        }
        *ptr = '\0';
        while (message[index] == ' ')
            index++;
        while (isdigit(message[index]))
        {
            save_number = save_number * 10 + (message[index] - '0');
            index++;
        }
        if (load_game(game, username, "game_database.txt", save_number) == 0)
        {
            send(socketfd, message, count_characters(message), 0);
            return COMMAND_LOAD;
        }
        else
        {
            return COMMAND_ERROR;
        }
    }
    else if (compare_strings(message, "/save ", 6) == 0)
    {
        char username[100];
        int index = 6;
        char *ptr = username;
        while (message[index] == ' ')
            index++;

        while (message[index] != ' ' && message[index] != '\0')
        {
            *ptr++ = message[index++];
        }

        *ptr = '\0';
        if (save_game(game, username, "game_database.txt") == 0)
        {
            send(socketfd, message, count_characters(message), 0);
            return COMMAND_SAVE;
        }
        else
        {
            return COMMAND_ERROR;
        }
    }

    return COMMAND_UNKNOWN;
}

void parse_username_and_save_number(const char *message, char *username, int *save_number)
{
    int index = 6;
    char *ptr = username;
    while (message[index] == ' ')
        index++;
    while (message[index] != ' ' && message[index] != '\0')
    {
        *ptr++ = message[index++];
    }
    *ptr = '\0';

    while (message[index] == ' ')
        index++;
    *save_number = 0;
    while (isdigit(message[index]))
    {
        *save_number = *save_number * 10 + (message[index] - '0');
        index++;
    }
}

int receive_command(ChessGame *game, const char *message, int socketfd, bool is_client)
{
    if (!message || message[0] == '\0')
    {
        return COMMAND_ERROR;
    }

    if (message[0] == '/' && message[1] == 'm' && message[2] == 'o' && message[3] == 'v' && message[4] == 'e' && message[5] == ' ')
    {
        ChessMove move;
        if (parse_move(message + 6, &move) == 0)
        {
            make_move(game, &move, is_client, false);
            return COMMAND_MOVE;
        }
        else
        {
            return COMMAND_ERROR;
        }
    }

    if (message[0] == '/' && message[1] == 'f' && message[2] == 'o' && message[3] == 'r' && message[4] == 'f' && message[5] == 'e' && message[6] == 'i' && message[7] == 't' && message[8] == '\0')
    {
        close(socketfd);
        return COMMAND_FORFEIT;
    }

    if (is_client && message[0] == '/' && message[1] == 'i' && message[2] == 'm' && message[3] == 'p' && message[4] == 'o' && message[5] == 'r' && message[6] == 't' && message[7] == ' ')
    {
        fen_to_chessboard(message + 8, game);
        return COMMAND_IMPORT;
    }

    if (message[0] == '/' && message[1] == 'l' && message[2] == 'o' && message[3] == 'a' && message[4] == 'd' && message[5] == ' ')
    {
        char username[100];
        int save_number;
        parse_username_and_save_number(message, username, &save_number);
        if (load_game(game, username, "game_database.txt", save_number) == 0)
        {
            return COMMAND_LOAD;
        }
        else
        {
            return COMMAND_ERROR;
        }
    }

    return COMMAND_ERROR;
}

int username_validity(const char *username)
{
    int index = 0;
    while (username[index] != '\0')
    {
        if (username[index] == ' ')
        {
            return 1; 
        }
        index++;
    }
    return 0; 
}

void game_to_file(FILE *file, const char *username, const char *fen)
{
    fprintf(file, "%s:%s\n", username, fen);
}

int save_game(ChessGame *game, const char *username, const char *db_filename)
{
    int usernameValidity = username_validity(username);
    if (usernameValidity)
    {
        return -1;
    }

    FILE *file = fopen(db_filename, "a");
    if (file == NULL)
    {
        return -1;
    }

    char fen[200];
    chessboard_to_fen(fen, game);

    int writeResult = fprintf(file, "%s:%s\n", username, fen);
    if (writeResult < 0)
    {
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0; 
}

int check_username_match(const char *line, const char *username, int delimiter_position) {
    for (int j = 0; j < delimiter_position; j++) {
        if (username[j] == '\0' || line[j] != username[j]) {
            return 0; 
        }
    }
    return username[delimiter_position] == '\0'; 
}

int parse_fen_and_load(ChessGame *game, const char *fen) {
    if (fen && *fen) {
        fen_to_chessboard(fen, game);
        return 1; 
    }
    return 0; 
}

int load_game(ChessGame *game, const char *username, const char *db_filename, int save_number) {
    FILE *file = fopen(db_filename, "r");
    if (!file) {
        return -1;
    }

    char line[300];
    int found = 0;
    int currentSave = 0;

    while (fgets(line, sizeof(line), file)) {
        int i = 0;
        while (line[i] != ':' && line[i] != '\0') {
            i++; 
        }

        if (line[i] != ':') {
            continue; 
        }

        int usernameMatches = check_username_match(line, username, i);
        if (usernameMatches) {
            currentSave++;
            if (currentSave == save_number) {
                found = 1;
                if (!parse_fen_and_load(game, line + i + 1)) { 
                    fclose(file);
                    return -1;
                }
                break;
            }
        }
    }

    fclose(file);

    if (!found) {
        return -1;
    }

    return 0; 
}


void display_chessboard(ChessGame *game)
{
    printf("\nChessboard:\n");
    printf("  a b c d e f g h\n");
    for (int i = 0; i < 8; i++)
    {
        printf("%d ", 8 - i);
        for (int j = 0; j < 8; j++)
        {
            printf("%c ", game->chessboard[i][j]);
        }
        printf("%d\n", 8 - i);
    }
    printf("  a b c d e f g h\n");
}
