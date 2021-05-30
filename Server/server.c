 
 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <WinSock2.h> 
 
#define PORT 8087 
#define SERVER_IP "127.0.0.1" 
#define BUFFER_SIZE 1024 
#define FILE_NAME_MAX_SIZE 512 
#pragma comment(lib, "-lws2_32")  // Please add Linker flag for this file.

 
int main() 
{ 
  //Declare and initialize a server-side (local) address structure
  struct sockaddr_in server_addr; 
  server_addr.sin_family = AF_INET; 
  server_addr.sin_addr.S_un.S_addr = INADDR_ANY; 
  server_addr.sin_port = htons(PORT); 
 
  //Initializes the socket DLL
  WSADATA wsaData; 
  WORD socketVersion = MAKEWORD(2, 0); 
  if(WSAStartup(socketVersion, &wsaData) != 0) 
  { 
    printf("Init socket dll error!\n"); 
    exit(1); 
  } 
 
  //Create a socket
  SOCKET m_Socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (SOCKET_ERROR == m_Socket) 
    { 
      printf("Create Socket Error!\n"); 
    exit(1); 
    } 
 
  //Bind socket and server (local) addresses
  if (SOCKET_ERROR == bind(m_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr))) 
  { 
    printf("Server Bind Failed: %d\n", WSAGetLastError()); 
    exit(1); 
  } 
 
  //Listening to the
  if (SOCKET_ERROR == listen(m_Socket, 10)) 
  { 
    printf("Server Listen Failed: %d\n", WSAGetLastError()); 
    exit(1); 
  } 
 
 
  while(1) 
  { 
    printf("Listening To Client...\n"); 
 
    struct sockaddr_in client_addr; 
    int client_addr_len = sizeof(client_addr); 
 
    SOCKET m_New_Socket = accept(m_Socket, (struct sockaddr *)&client_addr, &client_addr_len); 
    if (SOCKET_ERROR == m_New_Socket) 
    { 
      printf("Server Accept Failed: %d\n", WSAGetLastError()); 
      break; 
    } 
 
 	char buffer[BUFFER_SIZE]; 
	 memset(buffer, 0, BUFFER_SIZE); 
    if (recv(m_New_Socket, buffer, BUFFER_SIZE, 0) < 0) 
    { 
      printf("Server Receive Data Failed!\n"); 
      break; 
    } 
 
    char file_name[FILE_NAME_MAX_SIZE+1]; 
    memset(file_name, 0, FILE_NAME_MAX_SIZE+1); 
    strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE:strlen(buffer)); 
    printf("%s\n", file_name); 
 
    FILE * fp = fopen(file_name, "rb"); //Under Windows is "rb", which means to open a read-only binary
    if (NULL == fp) 
    { 
      printf("Image File: %s Not Found\n", file_name); 
    } 
        else 
    { 
      memset(buffer, 0, BUFFER_SIZE); 
      int length = 0; 
 
      while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) 
      { 
        if (send(m_New_Socket, buffer, length, 0) < 0) 
        { 
          printf("Send Image File: %s Failed\n", file_name); 
          break; 
        } 
        memset(buffer, 0, BUFFER_SIZE); 
      } 
 
      fclose(fp); 
      printf("Image File: %s Transfer Successful!\n", file_name); 
    } 
    closesocket(m_New_Socket); 
  } 
 
  closesocket(m_Socket); 
  //Release of the winsock library
  WSACleanup(); 
  return 0; 
} 
