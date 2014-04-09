#include "connect_h.h"

void myConnect()
{
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed: " << iResult << "\n";
        return;
    }else
    {
        cout << "WSAStartup correct\n";
    }

    server = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(server == INVALID_SOCKET)
    {
        cout << "Error creating socket\n";
        closesocket(server);
        return;
    }
    else
    {
        // Si se creó correctamente el socket del servidor
        //ZeroMemory(&local, sizeof (local));
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(DEFAULT_PORT);

        if( bind(server, (sockaddr *)&local, sizeof(local)) == SOCKET_ERROR )
        {
            cout << "Error binding socket\n";
            closesocket(server);
            return;
        }
        else
        {
            // si se logró el binding con la dirección local
            cout << "Binding OK\n";
            if (listen(server,1) == SOCKET_ERROR)
            {
                cout << "Error listening on socket\n";
                closesocket(server);
            }
            else
            {
                // Si está escuchando...
                cout << "I'm Listening...\n";
                client = INVALID_SOCKET;
                client = accept(server,NULL,NULL);
                if (client == INVALID_SOCKET) {
                    printf("accept failed: %d\n", WSAGetLastError());
                    closesocket(server);
                    //WSACleanup();
                    //return 1;
                }
                cout << "Conectado\n";
                iResult = 0;
                do {
                    iResult = recv(client, recvbuf, recvbuflen, 0);
                    if (iResult > 0) {
                        printf("Bytes received: %d\n", iResult);
                        getAllObjects(recvbuf, misCuerpos, misAgentes);
                        for(int i=0; i<nobstaculos; i++)
                        {
                            misCuerpos[i].SetPosX(misCuerpos[i].GetPosX()+30);
                        }
                        strcpy(recvbuf, buildJSONpackage(misCuerpos, misAgentes));
                        // Echo the buffer back to the sender
                        iSendResult = send(client, recvbuf, recvbuflen, 0);
                        if (iSendResult == SOCKET_ERROR) {
                            printf("send failed: %d\n", WSAGetLastError());
                            closesocket(client);
                            //WSACleanup();
                            //return 1;
                        }
                        printf("Bytes sent: %d\n", iSendResult);
                        closesocket(client);
                        //WSACleanup();
                        //return 1;
                    } else if (iResult == 0)
                        printf("Connection closing...\n");
                    else {
                        printf("recv failed: %d\n", WSAGetLastError());
                        //closesocket(client);
                        //WSACleanup();
                        cout << iResult;
                        //return 1;
                    }
                } while (iResult > 0);
                //for(;;){}
                cout << "FIN\n";
                //closesocket(client);
                //WSACleanup();
            }
        }
    }
}
