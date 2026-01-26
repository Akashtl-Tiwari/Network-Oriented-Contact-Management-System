
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define MAX_CONTACTS 100

struct Contact {
    char name[50];
    char phone[15];
    char email[50];
    char address[100];
};

struct Contact contacts[MAX_CONTACTS];
int contactCount = 0;

void saveContactsToFile();
void loadContactsFromFile();
int isValidEmail(const char *email);
int isNumeric(const char *str);
int isBlank(const char *str);
void sendMessage(SOCKET client, const char *msg);
void recvMessage(SOCKET client, char *buffer);
void toLowerCase(char *str);
void addContact(SOCKET client);
void listContacts(SOCKET client);
void searchContact(SOCKET client);
void modifyContact(SOCKET client);
void deleteContact(SOCKET client);

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = (char)tolower(str[i]);
    }
}

void saveContactsToFile() {
    FILE *fp = fopen("contacts.csv", "w");
    if (fp == NULL) return;
    fprintf(fp, "Name,Phone,Email,Address\n");
    for (int i = 0; i < contactCount; i++) {
        fprintf(fp, "%s,%s,%s,%s\n",
                contacts[i].name,
                contacts[i].phone,
                contacts[i].email,
                contacts[i].address);
    }
    fclose(fp);
}

void loadContactsFromFile() {
    FILE *fp = fopen("contacts.csv", "r");
    if (fp == NULL) return;
    char line[256];
    fgets(line, sizeof(line), fp);
    contactCount = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (contactCount >= MAX_CONTACTS) break;
        char *token = strtok(line, ",");
        if (token) strcpy(contacts[contactCount].name, token);
        token = strtok(NULL, ",");
        if (token) strcpy(contacts[contactCount].phone, token);
        token = strtok(NULL, ",");
        if (token) strcpy(contacts[contactCount].email, token);
        token = strtok(NULL, "\n");
        if (token) strcpy(contacts[contactCount].address, token);
        contactCount++;
    }
    fclose(fp);
}

int isValidEmail(const char *email) {
    return strchr(email, '@') && strchr(email, '.');
}

int isNumeric(const char *str) {
    for (int i = 0; str[i]; i++)
        if (!isdigit(str[i])) return 0;
    return 1;
}

int isBlank(const char *str) {
    return str[0] == '\0' || strspn(str, " \n\t") == strlen(str);
}

void sendMessage(SOCKET client, const char *msg) {
    send(client, msg, (int)strlen(msg), 0);
}

void recvMessage(SOCKET client, char *buffer) {
    int bytes = recv(client, buffer, 1023, 0);
    if (bytes <= 0) {
        buffer[0] = '\0';
        return;
    }
    buffer[bytes] = '\0';
    buffer[strcspn(buffer, "\r\n")] = '\0';
}

void addContact(SOCKET client) {
    if (contactCount >= MAX_CONTACTS) {
        sendMessage(client, "Contact list full.\n");
        return;
    }

    struct Contact newContact;
    char buffer[1024];

    sendMessage(client, "Enter name: ");
    recvMessage(client, newContact.name);

    sendMessage(client, "Enter phone: ");
    recvMessage(client, buffer);
    if (!isNumeric(buffer)) {
        sendMessage(client, "Invalid phone.\n"); return;
    }
    strcpy(newContact.phone, buffer);

    sendMessage(client, "Enter email: ");
    recvMessage(client, buffer);
    if (!isValidEmail(buffer)) {
        sendMessage(client, "Invalid email.\n"); return;
    }
    strcpy(newContact.email, buffer);

    sendMessage(client, "Enter address: ");
    recvMessage(client, newContact.address);

    contacts[contactCount++] = newContact;
    saveContactsToFile();
    sendMessage(client, "Contact added successfully.\n");
}

void listContacts(SOCKET client) {
    char msg[1024];
    sprintf(msg, "\n--- Contact List (%d) ---\n", contactCount);
    sendMessage(client, msg);
    for (int i = 0; i < contactCount; i++) {
        sprintf(msg, "\nName: %s\nPhone: %s\nEmail: %s\nAddress: %s\n",
                contacts[i].name, contacts[i].phone,
                contacts[i].email, contacts[i].address);
        sendMessage(client, msg);
    }
    sendMessage(client, "\n[Press Enter to continue]");
    recvMessage(client, msg);
}

void searchContact(SOCKET client) {
    char buffer[1024];
    char search[1024];
    int found = 0;

    sendMessage(client, "Enter keyword to search (name, phone, email, address): ");
    recvMessage(client, buffer);
    strcpy(search, buffer);
    toLowerCase(search);

    char msg[1024];
    sprintf(msg, "\n--- Search Results for '%s' ---\n", buffer);
    sendMessage(client, msg);

    for (int i = 0; i < contactCount; i++) {
        char nameL[50], phoneL[15], emailL[50], addressL[100];
        strcpy(nameL, contacts[i].name);
        strcpy(phoneL, contacts[i].phone);
        strcpy(emailL, contacts[i].email);
        strcpy(addressL, contacts[i].address);

        toLowerCase(nameL);
        toLowerCase(phoneL);
        toLowerCase(emailL);
        toLowerCase(addressL);

        if (strstr(nameL, search) || strstr(phoneL, search) ||
            strstr(emailL, search) || strstr(addressL, search)) {

            sprintf(msg,
                "\nName: %s\nPhone: %s\nEmail: %s\nAddress: %s\n",
                contacts[i].name, contacts[i].phone,
                contacts[i].email, contacts[i].address);
            sendMessage(client, msg);
            found = 1;
        }
    }

    if (!found) {
        sendMessage(client, "\nNo matching contact found.\n");
    }

    sendMessage(client, "\n[Press Enter to continue]");
    recvMessage(client, buffer);
}

void modifyContact(SOCKET client) {
    char buffer[1024], nameSearch[1024];
    int matches[100];
    int matchCount = 0;

    sendMessage(client, "Enter name or keyword of contact to modify: ");
    recvMessage(client, buffer);
    strcpy(nameSearch, buffer);
    toLowerCase(nameSearch);

    char msg[2048];
    for (int i = 0; i < contactCount; i++) {
        char nameL[50];
        strcpy(nameL, contacts[i].name);
        toLowerCase(nameL);

        if (strstr(nameL, nameSearch)) {
            matches[matchCount] = i;
            sprintf(msg, "\n[%d] Name: %s\nPhone: %s\nEmail: %s\nAddress: %s\n",
                    matchCount + 1,
                    contacts[i].name, contacts[i].phone,
                    contacts[i].email, contacts[i].address);
            sendMessage(client, msg);
            matchCount++;
        }
    }

    if (matchCount == 0) {
        sendMessage(client, "No matching contact found.\n[Press Enter to continue]");
        recvMessage(client, buffer);
        return;
    }

    sprintf(msg, "\nEnter the number of the contact to modify (1 to %d): ", matchCount);
    sendMessage(client, msg);
    recvMessage(client, buffer);
    int choice = atoi(buffer);
    if (choice < 1 || choice > matchCount) {
        sendMessage(client, "Invalid selection.\n[Press Enter to continue]");
        recvMessage(client, buffer);
        return;
    }

    int index = matches[choice - 1];

    sendMessage(client, "Enter new name: ");
    recvMessage(client, contacts[index].name);

    sendMessage(client, "Enter new phone: ");
    recvMessage(client, buffer);
    if (!isNumeric(buffer)) {
        sendMessage(client, "Invalid phone number! Update aborted.\n[Press Enter to continue]");
        recvMessage(client, buffer);
        return;
    }
    strcpy(contacts[index].phone, buffer);

    sendMessage(client, "Enter new email: ");
    recvMessage(client, buffer);
    if (!isValidEmail(buffer)) {
        sendMessage(client, "Invalid email format! Update aborted.\n[Press Enter to continue]");
        recvMessage(client, buffer);
        return;
    }
    strcpy(contacts[index].email, buffer);

    sendMessage(client, "Enter new address: ");
    recvMessage(client, contacts[index].address);

    saveContactsToFile();

    sendMessage(client, "\nContact updated successfully!\n[Press Enter to continue]");
    recvMessage(client, buffer);
}

void deleteContact(SOCKET client) {
    char buffer[1024];
    sendMessage(client, "Enter name of contact to delete: ");
    recvMessage(client, buffer);
    int found = 0;
    for (int i = 0; i < contactCount; i++) {
        if (strcmp(contacts[i].name, buffer) == 0) {
            for (int j = i; j < contactCount - 1; j++) {
                contacts[j] = contacts[j + 1];
            }
            contactCount--;
            found = 1;
            break;
        }
    }
    if (found) {
        saveContactsToFile();
        sendMessage(client, "Contact deleted successfully.\n");
    } else {
        sendMessage(client, "Contact not found.\n");
    }
    sendMessage(client, "[Press Enter to continue]");
    recvMessage(client, buffer);
}

int main() {
    WSADATA wsa;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientSize;

    WSAStartup(MAKEWORD(2,2), &wsa);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    printf("Server listening on port %d...\n", PORT);

    loadContactsFromFile();

    while (1) {
        clientSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientSize);

        while (1) {
            sendMessage(clientSocket,
                "\n--- NetPhone Menu ---\n"
                "1. Add Contact\n"
                "2. List Contacts\n"
                "3. Search Contact\n"
                "4. Modify Contact\n"
                "5. Delete Contact\n"
                "6. Exit\n"
                "Enter your choice: ");

            char choice[10];
            recvMessage(clientSocket, choice);

            if (strcmp(choice, "1") == 0) addContact(clientSocket);
            else if (strcmp(choice, "2") == 0) listContacts(clientSocket);
            else if (strcmp(choice, "3") == 0) searchContact(clientSocket);
            else if (strcmp(choice, "4") == 0) modifyContact(clientSocket);
            else if (strcmp(choice, "5") == 0) deleteContact(clientSocket);
            else if (strcmp(choice, "6") == 0) break;
            else sendMessage(clientSocket, "Invalid choice.\n");
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
