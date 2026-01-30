# Network-Oriented-Contact-Management-System
This project is a Network-Based Phonebook Management System developed in C using TCP/IP socket programming. It follows a client–server architecture where operations like add, search, update, delete, list, and sort contacts are performed over a network. Contact data is stored persistently in a CSV file to ensure data consistency and easy access.

---

## 🏗️ Project Architecture
- Client–Server Architecture
- TCP/IP based socket communication
- Server handles data processing and storage
- Client sends requests and receives responses from the server

---

## ✨ Features
- Add new contacts
- Search contacts by name
- Update existing contact details
- Delete contacts
- Display all saved contacts
- Sort contacts alphabetically
- Persistent data storage using CSV file
- Real-time network communication

---

## 🛠️ Technologies Used
- Programming Language: C
- Networking: TCP/IP, Winsock API
- Compiler: GCC (MSYS2 MinGW64)
- Data Storage: CSV File
- Operating System: Windows

---

## 🧠 Data Structures & Algorithms

### Data Structures
- Structure (`struct`) to store contact details
- Array to manage multiple contacts

### Algorithms
- Linear Search for searching contacts
- Quick Sort using `qsort()` for sorting contacts

---

## 📂 Project Structure
PBL/
│
├── server.c # Server-side source code
├── client.c # Client-side source code
├── contacts.csv # Data storage file
└── README.md

---

## ▶️ How to Run the Project

### 🔹 Prerequisites
- Windows Operating System
- MSYS2 installed
- GCC compiler (MinGW64)

### 🔹 Compile and Run Server
gcc server.c -o server.exe -lws2_32
./server.exe

### 🔹 Compile and Run Client (open a new terminal)
gcc client.c -o client.exe -lws2_32
./client.exe

---

## 💻 Available Client Commands
add
list
search
modify
delete
sort
exit

---

## 👨‍💻 Developer Role
Akash Tiwari  
- Designed the client–server architecture
- Implemented TCP socket communication
- Developed CRUD operations for contact management
- Handled file operations using CSV storage
- Implemented searching and sorting logic

---

## 🎓 Academic Context
This project was developed as part of Project Based Learning (PBL) for the MCA program. It demonstrates practical knowledge of network programming, file handling, data structures, and algorithms using C.

---

## 📜 License
This project is created for educational purposes only.
