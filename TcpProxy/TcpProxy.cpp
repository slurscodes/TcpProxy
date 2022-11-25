// TcpProxy.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<WS2tcpip.h>
#include<string>

#pragma comment(lib,"Ws2_32")
using namespace std;


char* ip_remote;
int port_remote;



void usage() {

    cout << "TcpProxy.exe ip_interface port remote_ip remote_port\n";

}
__forceinline int connect_target(char* ip, int port, SOCKET *remote) {

    *remote = socket(AF_INET, SOCK_STREAM, 0);
    if (*remote == -1) { return -1; }

    SOCKADDR_IN inf = { sizeof(inf) };
    inf.sin_family = AF_INET;
    inf.sin_port = htons(port);

    inet_pton(AF_INET, ip, &inf.sin_addr.s_addr);

    if (connect(*remote, (const sockaddr*)&inf, sizeof(inf)) == SOCKET_ERROR) { closesocket(*remote); return -1; }

    return 0;
}


void _stdcall tunnel(SOCKET client) {
    SOCKET remote;
    fd_set fd;
    FD_ZERO(&fd);
    if (connect_target(ip_remote, port_remote, &remote)) { return; }
    char buff[4096];
    int e;
    while (1) {
        FD_SET(remote,&fd);
        FD_SET(client,&fd);
        if (FD_ISSET(remote, &fd)) {
            
            e = recv(remote, buff, sizeof(buff), 0);
            send(client, buff, e, 0);
            if (e <= 0) { break; }

        }
        if (FD_ISSET(client, &fd)) {

            e = recv(client, buff, sizeof(buff), 0);
            send(remote, buff, e, 0);
            if (e <= 0) { break; }


        }



    }
    closesocket(client);
    closesocket(remote);
    return;





}



int main(int argc, char* argv[])
{
    cout << "TcpProxy By F3di006\n";
    if (argc < 2) { usage(); return 0; }


    ip_remote = argv[3];
    port_remote = stoi(argv[4]);



    WSADATA d;
    SOCKADDR_IN inf = { sizeof(inf) };
    inf.sin_port = htons((u_short)stoi(argv[2]));
    inf.sin_family = AF_INET;
    if (!inet_pton(AF_INET, argv[1], &inf.sin_addr.s_addr)) { return -3; }


    if (WSAStartup(MAKEWORD(2, 0), &d)) { return -1; }

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) { return -2; }

    if (bind(s, (const sockaddr*)&inf, sizeof(inf)) == -1) {
        return -3;;
    }

    if (listen(s, 5) == -1) {
        return -4;
    }

    HANDLE th;
    while (1) {
        cout << "waiting for client on localport\n";
        SOCKET client = accept(s, NULL, NULL);
        cout << "accepted!\nforwarding...\n";
        th = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)tunnel, (LPVOID)client, 0, 0);
        CloseHandle(th);




    }


    
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
