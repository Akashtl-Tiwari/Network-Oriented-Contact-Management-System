
#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 8080

void recvMessage(SOCKET sock) {
    char buffer[4096];
    int bytes;

    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);

        // Break when a full message or prompt ends
        if (bytes < (int)(sizeof(buffer) - 1)) break;
        if (strchr(buffer, '\n') || strchr(buffer, ':')) break;
    }
}

void sendMessage(SOCKET sock, const char *msg) {
    send(sock, msg, (int)strlen(msg), 0);
}

void sendUserInput(SOCKET sock) {
    char input[1024];
    if (fgets(input, sizeof(input), stdin) == NULL) return;
    input[strcspn(input, "\r\n")] = '\0';
    sendMessage(sock, input);
}

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serverAddr;
    char input[1024];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Could not create socket.\n");
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connection failed.\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    while (1) {
        printf("\n==========================\n");
        printf(" NetPhone Client Terminal \n");
        printf("==========================\n");

        recvMessage(sock);        // Server menu or prompt

        printf("\n>> ");
        sendUserInput(sock);      // User response

        // Optional: Exit condition check
        if (strcmp(input, "6") == 0 || strcmp(input, "exit") == 0)
            break;

        // Receive further prompts/data (like name/phone if adding)
        while (1) {
            recvMessage(sock);
            printf("\n>> ");
            if (fgets(input, sizeof(input), stdin) == NULL) break;
            input[strcspn(input, "\r\n")] = '\0';
            sendMessage(sock, input);

            if (strstr(input, "back") || strstr(input, "exit") || input[0] == '\0') break;
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
