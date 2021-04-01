#include <iostream>
#include <winsock2.h>
#include <conio.h>

#define LISTEN_PORT 9009

using namespace std;


char rbuf[512];

int main()
{
   WSADATA wsaData;
   SOCKET hServSock, hClntSock;
   SOCKADDR_IN servaddr, clntaddr;

   memset(&servaddr, 0, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   servaddr.sin_port = htons(LISTEN_PORT);

   if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0) return 1;
   if((hServSock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) return 2;
   if(bind(hServSock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) return 3;
   if(listen(hServSock, 5) == SOCKET_ERROR) return 4;

   int szClntAddr = sizeof(clntaddr);

   while(1)
   {
      cout <<"listening at " << LISTEN_PORT << "...\n";
      if((hClntSock = accept(hServSock, (SOCKADDR*) &clntaddr, &szClntAddr)) == INVALID_SOCKET) return 5;
      cout << "accept\n";

      int rlen = 1;
//      while(rlen)
      {
         rlen = recv(hClntSock, rbuf, sizeof(rbuf), 0);
         rbuf[rlen]=0;
         cout << rlen << ":" << rbuf <<endl;
      }

      cout << "send respond\n";

      string resp =
      "HTTP/1.1 200 OK\n"
      "Content-Type: text/plain; charset=UTF-8\n"
      "Content-Length: 10\n"
      "Connection: close\n"
      "\n\n"
      "abcdefghi";

      int wlen = send(hClntSock, resp.c_str(), resp.length(), 0);
      cout << "send " << wlen << endl;

      cout << "client close\n\n";
      closesocket(hClntSock);
   }

//   closesocket(hServSock);
//   WSACleanup();
//   Sleep(500);
//   return 0;
}
