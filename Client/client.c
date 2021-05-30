#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <WinSock2.h> 
 
#define PORT 8087 
#define SERVER_IP "127.0.0.1" 
#define BUFFER_SIZE 1024 
#define FILE_NAME_MAX_SIZE 512 
#pragma comment(lib, "WS2_32")  // Please add Linker flag for this file.
 
int main() 
{ 
  //Initializes the socket DLL
  WSADATA wsaData; 
  WORD socketVersion = MAKEWORD(2, 0); 
  if(WSAStartup(socketVersion, &wsaData) != 0) 
  { 
    printf("Init socket dll error!\n"); 
    exit(1); 
  } 
 
  //Create a socket
  SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0); 
  if (SOCKET_ERROR == c_Socket) 
  { 
    printf("Create Socket Error!\n"); 
    system("pause"); 
    exit(1); 
  } 
 
  //Specify the address of the server
  struct sockaddr_in server_addr; 
  server_addr.sin_family = AF_INET; 
  server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP); 
  server_addr.sin_port = htons(PORT); 
 
  if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr))) 
  { 
    printf("Can Not Connect To Server IP!\n"); 
    system("pause"); 
    exit(1); 
  } 
 
  //Input file name
  char file_name[FILE_NAME_MAX_SIZE+1]; 
  memset(file_name, 0, FILE_NAME_MAX_SIZE+1); 
  printf("Please Input Image File Name On Server: \n"); 
  scanf("%s", &file_name); 
 
  char buffer[BUFFER_SIZE]; 
  memset(buffer, 0, BUFFER_SIZE); 
  strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE ? BUFFER_SIZE:strlen(file_name)); 
 
  //Send the file name to the server
  if(send(c_Socket, buffer, BUFFER_SIZE, 0) < 0) 
  { 
    printf("Send File Name Failed\n"); 
    system("pause"); 
    exit(1); 
  } 
 
  //Open the file and prepare to write
  FILE * fp = fopen(file_name, "wb"); //Under Windows is "wb", which means to open a write-only binary
  if(NULL == fp) 
  { 
    printf("Image File: %s Can Not Open To Writen\n", file_name); 
    system("pause"); 
    exit(1); 
  } 
  else 
  { 
    memset(buffer, 0, BUFFER_SIZE); 
    int length = 0; 
    while ((length = recv(c_Socket, buffer, BUFFER_SIZE, 0)) > 0) 
    { 
      if (fwrite(buffer, sizeof(char), length, fp) < length) 
      { 
        printf("Image File: %s Write Failed\n", file_name); 
        break; 
      } 
      memset(buffer, 0, BUFFER_SIZE); 
    } 
 
    printf("Received Image File: %s From Server Successful!\n", file_name); 
  } 
 
  fclose(fp); 
  closesocket(c_Socket); 
   
  //Release of the winsock library
  WSACleanup(); 
 
  system("pause"); 
  return 0; 
} 
