#include <iostream>
#include <sstream>
#include <winsock2.h>
#include <conio.h>
#include <stdio.h>

#define LISTEN_PORT 9009

using namespace std;

char clntbuf[0xFFFF];



string plain_text_resp_create(string text)
{
   stringstream contentlen;
   contentlen << text.length();
   string resp =
   "HTTP/1.1 200 OK "
   "Content-Type: text/plain; charset=UTF-8 "
   "Content-Length: " + contentlen.str() + " "
   "Connection: close "
   "\n\n" +
   text;

   return resp;
}

string plain_text_resp_create_404(string text)
{
   stringstream contentlen;
   contentlen << text.length();
   string resp =
   "HTTP/1.1 404 ERROR "
   "Content-Type: text/plain; charset=UTF-8 "
   "Content-Length: " + contentlen.str() + " "
   "Connection: close "
   "\n\n" +
   text;

   return resp;
}


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
      cout <<"########## listening at " << LISTEN_PORT << "...\n";
      if((hClntSock = accept(hServSock, (SOCKADDR*) &clntaddr, &szClntAddr)) == INVALID_SOCKET) return 5;
      cout << "accept\n";

      int rlen = recv(hClntSock, clntbuf, sizeof(clntbuf), 0);
      clntbuf[rlen]=0;

      string clntbufstr(clntbuf);
      cout << clntbufstr <<endl;

      string resp = plain_text_resp_create("Hello thererer");

      int firstlineidx = clntbufstr.find("\n");
      if(firstlineidx!=-1)
      {
         string querystr = clntbufstr.substr(0,firstlineidx);
         querystr = querystr.substr(querystr.find(" ")+1);
         querystr = querystr.substr(0, querystr.find(" "));
         cout << "querystr : " << querystr << endl;

         if(querystr == "/favicon.ico")
         {
            resp = plain_text_resp_create_404("ERROR");
         }
         else if(querystr == "/hello")
         {
            FILE *file = fopen("875px-ASCII-Table-wide.jpeg", "rb");
            if(file != NULL)
            {
               fseek(file, 0, SEEK_END);
               int fsiz = ftell(file);
               fseek(file, 0, SEEK_SET);

               char txbuf[512];
               int rlen = 1;

               stringstream sizstr;
               sizstr << fsiz;
               string resp =
               "HTTP/1.1 200 OK "
               "Content-Type: image/jpeg "
               "Content-Length: " + sizstr.str() + " "
               "Connection: close "
               "\n\n";

               send(hClntSock, resp.c_str(), resp.length(), 0);

               while(rlen)
               {
                  rlen = fread(txbuf, 1, sizeof(txbuf), file);
                  send(hClntSock, txbuf, rlen, 0);
               }
            }
            else
            {
               resp = plain_text_resp_create_404("picture not available");
            }

            fclose(file);
         }
      }

      send(hClntSock, resp.c_str(), resp.length(), 0);
      cout << "client close\n\n";
      closesocket(hClntSock);
   }

//   closesocket(hServSock);
//   WSACleanup();
//   Sleep(500);
//   return 0;
}
