#include "MirmecoFunciones.h"

CRITICAL_SECTION lock;//multiplayer data

int mazoDeRobo[52] = {0};
int cartasPorJugador = 0;
int cartasRepartidas = 0;

bool gameStarted = false;

void repartirCartas(int n);  // prototipo

DWORD WINAPI client_thread(LPVOID arg) {//multiplayer data

    if(gameStarted == true){
        printf("Un jugador intentó unirse con la partida empezada");
        skibidi
    }

    int idx = *(int*)arg;
    free(arg);

    SOCKET sock = players[idx].sock;

    // Recibir nombre del jugador al inicio
    char pname[NAME_LEN];
    int rec = recv(sock, pname, sizeof(pname), 0);
    if(rec > 0) {
        pname[NAME_LEN-1] = '\0'; // seguridad
        EnterCriticalSection(&lock);
        strncpy(players[idx].name, pname, NAME_LEN);
        LeaveCriticalSection(&lock);
        printf("Jugador %d conectado con nombre: %s\n", idx, players[idx].name);
    }

    while(true) {
        // option.a = 1 actualizar, 2 leer
        // option.b = x o índice
        // option.c = y (solo si actualizar)
        Sleep(100);
        int received = recv(sock, (char*)&buffer, sizeof(buffer), 0);//multiplayer data
        if(received <= 0) break;

        EnterCriticalSection(&lock);
        if(buffer.a == 1){
            // actualizar datos propios
            players[idx].x = buffer.b;
            players[idx].y = buffer.c;
            players[idx].z = buffer.d;
            strncpy(players[idx].lastmsg, buffer.msg, NAME_LEN);
            UpdateID();
        } 
        else if(buffer.a == 2)
        {
            int target = buffer.b;
            struct {
                char name[NAME_LEN];
                int x,y,z;
                char msg[NAME_LEN];
            } reply = {"",0,0,0,""};
            
            if(target>=0 && target<MAX_CLIENTS && players[target].active){

                if(players[target].active != 2){ //si no está conectado (active = 2 significa conectado (vete fuera))
                    strncpy(reply.name, "none", NAME_LEN);
                    reply.x = -1;
                    reply.y = -1;
                    reply.z = -1;
                    strncpy(reply.msg, "null", NAME_LEN);
                }
                else {
                    strncpy(reply.name, players[target].name, NAME_LEN);
                    reply.x = players[target].x;
                    reply.y = players[target].y;
                    reply.z = players[target].z;
                    strncpy(reply.msg, players[target].lastmsg, NAME_LEN);
                }

            }
            send(sock, (char*)&reply, sizeof(reply), 0); //quien lea esto es gay
            UpdateID();
        }
        else if(buffer.a == 3)
        { //recibir las cartas
            int a = GetRandomInteger(10, 7* jugadoresConectados);
            Sleep(200 + a);
            if(gameStarted == false){
                repartirCartas(52);
                gameStarted = true;
                printf("El juego ha comenzado!\n");
            }

            struct {
                int baraja[53];
                int ID;
                int numeroDeJugadores;
            } baraja;
            
            baraja.ID = idx;
            baraja.numeroDeJugadores = jugadoresConectados;
            //printf("%i bro", baraja.numeroDeJugadores);

            if(cartasRepartidas > 52) printf("No hay mas cartas para repartir!\n");

            for (int i = 0; i < cartasPorJugador; i++)
            {   
                if(cartasRepartidas > 52) break; //si ya no hay más cartas, se sale
                baraja.baraja[i] = mazoDeRobo[cartasRepartidas];
                cartasRepartidas++;
            }

            baraja.baraja[52] = 10099;

            send(sock, (char*)&baraja, sizeof(baraja), 0);
            UpdateID();
        }
        else if(buffer.a == 4){

            Player actualLastPlayer;
                   
            UpdateID();

            actualLastPlayer = players[getLastPlayerID(ActualID, jugadoresConectados)];

            send(sock, (char*)&actualLastPlayer, sizeof(actualLastPlayer), 0);
        }
        LeaveCriticalSection(&lock);
    }

    printf("Jugador %d desconectado\n", idx);

    EnterCriticalSection(&lock);
    players[idx].active = 0;
    LeaveCriticalSection(&lock);
    closesocket(sock); //no veas como me llora el compilador bro

    int a = GetRandomInteger(10, 7* jugadoresConectados);
    Sleep(200 + a);

    skibidi
}

//damn daniel

void repartirCartas(int n){
    //printf("Repartiendo cartas...\n");
    for (int i = 0; i < n; i++)
    {
        mazoDeRobo[i] = i + 1;
    }

    //barajar
    //printf("Barajando cartas...\n");
    
    for (int i = 0; i < n; i++)
    {
        int j = GetRandomInteger(i, 0);
        int temp = mazoDeRobo[i];
        mazoDeRobo[i] = mazoDeRobo[j];
        mazoDeRobo[j] = temp;
    }

    //printf("Calculando cartas por jugador...\n");
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        //printf("Jugador %d estado: %d\n", i, players[i].active);
        if(players[i].active == 2) jugadoresConectados++;
    }
    printf("Jugadores conectados: %d\n", jugadoresConectados);
    cartasPorJugador = n / jugadoresConectados;
    if(n % jugadoresConectados != 0) cartasPorJugador++; //si no es exacto, se reparte una carta más a cada uno
    printf("Cartas por jugador: %d\n", cartasPorJugador);
    
    cartasRepartidas=0;
}

int main() {
    ActualID = 0; 

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
    InitializeCriticalSection(&lock);

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);//multiplayer data
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 5);
    printf("Servidor esperando clientes...\n");

    for(int i=0;i<MAX_CLIENTS;i++) players[i].active=0;

    while(1) {
        if(gameStarted == true){
            printf("Un jugador intento unirse con la partida empezada");
            skibidi
        }

        SOCKET new_client = accept(server, NULL, NULL);//multiplayer data
        if(new_client != INVALID_SOCKET) {
            int idx=-1;
            EnterCriticalSection(&lock);
            for(int i=0;i<MAX_CLIENTS;i++){
                if(players[i].active==0 || players[i].active==3){
                    idx=i;
                    players[i].active=1;
                    players[i].sock=new_client;
                    players[i].x=0;
                    players[i].y=0;
                    players[i].z=0;
                    strcpy(players[i].name, "name");
                    strcpy(players[i].lastmsg, "msg");
                    break;
                }
            }
            LeaveCriticalSection(&lock);

            if(idx!=-1){
                int* pidx = malloc(sizeof(int)); //jaja malloc (es necesario porque si no la variable idx se sobreescribe en el siguiente cliente)
                *pidx = idx;
                CreateThread(NULL,0,client_thread,pidx,0,NULL); //el anterior comentario está financiado por copilot
                printf("Cliente %d conectado!\n", idx);
                players[idx].active=2; //activo y conectado
             } else {
                closesocket(new_client);
                printf("error: servidor lleno\n");
            }
        }

        Sleep(100);
    }

    closesocket(server);//esto si no lo pongo no va xd
    DeleteCriticalSection(&lock); //lo que tu digas bro
    WSACleanup(); //el compilador me está llorando mucho con este codigo xd
    skibidi
}
