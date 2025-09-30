//damn daniel
#include "MirmecoFunciones.h"

/*
El sistema para mandar información es el siguiente:
cada cliente tiene un ID (0-99), un nombre (sin importancia) y tres variables int (x,y,z) y un mensaje

la primera variable (x) es el estado actual, "100000000" un uno y 8 numeros diferentes, empezando por la izquierda y sin contar el primero:
1. el primero de los números indica la ID del jugador que es su turno(0-98)
2. el segundo de los números, junto al primero indica si el jugador ha terminado/ha ganado (99) o no (else)
3. los siguientes 2 números indican cuantas cartas tiene el jugador en su mano (max 52 (aunque es imposible porque descartaría todas xd))
4. los siguientes 2 números indican cuantas cartas ha descartado el jugador (max 52), recuerda que se descartan de 4 en 4
5. los últimos 2 números indican cuantas cartas ha jugado el jugador (max 99), si el jugador jugó más de 99 cartas en total, se mantiene en 99, por ser un perdedor jajaj

la segunda variable (y) es la acción actual, "100000000" un uno y 8 numeros diferentes, empezando por la izquierda y sin contar el primero:
1. el primero de los números indica si el jugador no ha jugado todavia (0), está jugando cartas (1), levantando cartas (2), ha descartado 4 iguales (3) o ha saltado turno (4), siendo esta última solo para debug
en caso de que no haya jugado cartas ni haya descartado 4, se omiten los siguientes 7 números con ceros
2. los siguientes 2 la carta que ha jugado / descartado el jugador (0-52) 
3. los siguientes 2 la cantidad de cartas que ha jugado el jugador (1-4) (si es descarte siempre es 4)
4. el siguiente si la jugada era mentira (1) o no (0) (no se miente al descartar)
5. el siguiente la cara que pone el jugador al jugar (0-9) (las caras las explico abajo)
6. el último, y más importante, si el jugador está listo, esto sirve para empezar la ronda (1) o no (0), cuando el juego está en marcha, se ignora

la tercera variable (z) es la jugada real del jugador, "100000000" un uno y 8 numeros diferentes, empezando por la izquierda y sin contar el primero:
1. los dos primeros la primera carta (0-52)
2. los dos siguientes la segunda carta (0-52) (si no hay, 0)
3. los dos siguientes la tercera carta (0-52) (si no hay, 0)
4. los dos últimos la cuarta carta (0-52) (si no hay, 0)

la tercera variable no está implementada todavia, pero pablo me la pidió
*/

Player myself;

int manoActual[52];

void setEstado(SOCKET sock, int x, int y, int z, char msg[NAME_LEN]){ //set manual

    myself.x = x;
    myself.y = y;
    myself.z = z;
    strcpy(myself.lastmsg, msg);

    buffer.b = x + (x < 100000000? 100000000 : 0);
    buffer.c = y + (y < 100000000? 100000000 : 0);
    buffer.d = z + (z < 100000000? 100000000 : 0);
    strcpy(buffer.msg, msg);
    
    send(sock,(char*)&buffer,sizeof(buffer),0);
}

void actualizarEstado(SOCKET sock, bool scan){ //scan es 1 o 0, si es 1 pide datos al usuario, si es 0 no
    if(scan != 0)
    {
        int x,y,z = 100000000;
        scanf("%d %d %d", &x, &y, &z); //pide datos al usuario
        buffer.b = x + (x < 100000000? 100000000 : 0);
        buffer.c = y + (y < 100000000? 100000000 : 0);
        buffer.d = z + (z < 100000000? 100000000 : 0);

        myself.x = buffer.b;
        myself.y = buffer.c;
        myself.z = buffer.d;

        printf("Introduce mensaje: ");
        scanf("%s", buffer.msg);
    }

    send(sock,(char*)&buffer,sizeof(buffer),0);
}

void endAction(SOCKET sock, int action){
    myself.y = setDigit(myself.y, 7, action);

    Sleep(50);

    switch (action)
    {
    case 1:
        myself.x = setDigit(myself.x, 1, getNextPlayerID(ActualID, jugadoresConectados));
        //printf("-----%i------", myself.x);
        setEstado(sock, myself.x, myself.y, myself.z, "Played Card");
        break;
    case 2:
        myself.x = setDigit(myself.x, 1, getNextPlayerID(ActualID, jugadoresConectados));
        //printf("-----%i------", myself.x);
        setEstado(sock, myself.x, myself.y, myself.z, "Revealed");
        break;
    case 3:
        //printf("-----%i------", myself.x);
        setEstado(sock, myself.x, myself.y, myself.z, "discarted");
        break;
    default:
        break;
    }
}

void imprimirMano(bool ordenar){

    if(ordenar == true){

        qsort(manoActual, sizeof(manoActual) / sizeof(manoActual[0]), sizeof(int), comparar); // ordenar la mano
        for (int i = 0; i < 52; i++)
        {   
            int val = 0, times = 0;
            if(manoActual[i] == 0) {
                //if(val != 0){
                //    printf("%i cartas de %i", times, val);
                //}
                break; //si ya no hay más cartas, se sale
            }
            if((manoActual[i] + 3) / 4 == val){
                times++;
            }
            else {
                if(val != 0){
                    printf("%i cartas de %i \n", times, val);
                }
                val = (manoActual[i] + 3) / 4;
                times = 1;
            }
        }
    }

    printf("Tu mano actual es: \n");
    for (int i = 0; i < 52; i++)
    {
        if(manoActual[i] == 0 && (manoActual[i + 1] == 0 || i + 1 == 52)) break; //si ya no hay más cartas, se sale
        printf("%i ",manoActual[i]);
    }
    printf("\n");
}

void inicializar(SOCKET sock){ //no mires esto

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(PORT);

    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))!=0){
        printf("No se pudo conectar al servidor\n");    
    }
    else{
        // Enviar nombre del jugador
        printf("Introduce tu nombre: ");
        scanf("%s", myself.name);
        send(sock, myself.name, sizeof(myself.name), 0);

        printf("Conectado como %s!\n", myself.name);
    }
}

int playerTurn; //turno del jugador actual (su ID)

Player lastPlayer;

void descartar4(){
    Sleep(50);
    bool descartaste = false;
    qsort(manoActual, sizeof(manoActual) / sizeof(manoActual[0]), sizeof(int), comparar); //ordenar la mano por si acaso
    int val = 0, times = 0;
    for (int i = 0; i < 52; i++)
    {
        if(manoActual[i] == 0) break; //si ya no hay más cartas, se sale
        if((manoActual[i] + 3) / 4 == val){
            times++;
            if(times == 4){
                manoActual[i] = 0;
                manoActual[i - 1] = 0;
                manoActual[i - 2] = 0;
                manoActual[i - 3] = 0;
                printf("Has descartado los %i \n", val);
                val = 0;
                descartaste = true;
            }
        }
        else {
            val = (manoActual[i] + 3) / 4;
            //printf("Los %i ?", val);
            times = 1;
        }
    }
    if(descartaste == false) printf("No hay para descartar bro :c");
    imprimirMano(true);
}

int EmpiezaAJugar(SOCKET sock){
    Sleep(50);
    printf("Que quieres hacer? \n 0 = jugar carta \n 1 = descartar 4 iguales\n");
    int decision;
    scanf("%i", &decision);
    printf("%i",decision);
    if(decision == 1){
        descartar4();
        endAction(sock, 3);
    }
    else if(decision == 0){
        printf("no se ha implementado eso\n");
        
        endAction(sock, 1);
    }
    else endAction(sock, 4);
    skibidi
}

int ContinuaJugada(SOCKET sock){
    Sleep(50);
    printf("El jugador anterior jugo %i \n", lastPlayer.y);
    printf("Que quieres hacer? \n 0 = jugar carta \n 1 = descartar 4 iguales \n 2 = levantar carta\n");
    int decision;
    scanf("%i", &decision);
    printf("%i",decision);
    if(decision == 1){
        descartar4();
        endAction(sock, 3);
    }
    else if(decision == 0){
        printf("no se ha implementado eso\n");
            
        endAction(sock, 1);
    }
    else if(decision == 2){
        printf("no se ha implementado eso\n");
           
        endAction(sock, 2);
    }
    else endAction(sock, 4);
    skibidi
}

int actualizarAnteriorJugada(SOCKET sock){

    Sleep(50);
    int received;
    buffer.a = 4; // para recibir toda la info de la jugada anterior

    actualizarEstado(sock, false);//pide el dato

    Player actualLastPlayer;
    
    received = recv(sock,(char*)&actualLastPlayer,sizeof(actualLastPlayer),0);//recibe respuesta
    if(received<=0) { //error?
        printf("Error de conexión o el servidor cerró la conexión.\n");
        skibidi
    }
    //printf("La x de %i, es %i \n", ActualID, myself.x);
    printf("La x de %i, es %i \n", getLastPlayerID(ActualID, jugadoresConectados), actualLastPlayer.x);
    if(actualLastPlayer.x != lastPlayer.x){ //el jugador no ha jugado
        lastPlayer = actualLastPlayer;
        myself.x = setDigit(myself.x, 1, getDigit(lastPlayer.x, 1));

        buffer.a = 1; //envia la info nueva

        buffer.b = myself.x;
        buffer.c = myself.y;
        buffer.d = myself.z;
        actualizarEstado(sock, false);
        int a = GetRandomInteger(10, 20);
        Sleep(100 + a); //estamos todos sincronizados (mas o menos xd)
    }

    Sleep(100); //estamos todos sincronizados (mas o menos xd)
    skibidi
}

void onGame(SOCKET sock){//----------------------------------------------------ON-GAME---------------------------------------------------------------

    actualizarAnteriorJugada(sock);

    Sleep(100);

    buffer.a = 1;

    if(getDigit(myself.x, 1) == 9 && getDigit(myself.x, 2) == 9) {
        return; //el jugador ya ha ganado
    }

    if(getDigit(myself.x, 1) == ActualID){
        printf("Es tu turno, jugador %i!\n", ActualID);
        imprimirMano(false);

        if(getDigit(lastPlayer.y, 7) == 1){
            ContinuaJugada(sock);
        }
        else{
            EmpiezaAJugar(sock);
        }
        //actualizarEstado(sock, true);
    }
    else if(false){
        buffer.a = 1; //envia la info nueva

        buffer.b = myself.x;
        buffer.c = myself.y;
        buffer.d = myself.z;
        actualizarEstado(sock, false);
        Sleep(100 + GetRandomInteger(50, 25)); //estamos todos sincronizados (mas o menos xd)
    }

    Sleep(200);
}

void atStartingGame(SOCKET sock){ //robar cartas, empieza el de indice 0, etc

    playerTurn = 0;

    int a = GetRandomInteger(90,0);
    Sleep(100 + a); //así le damos tiempo para procesar los datos

    int received;
    buffer.a = 3; // para recibir toda la info para empezar la partida

    actualizarEstado(sock, false);//pide el dato

    struct {
        int baraja[53];
        int ID;
        int numeroDeJugadores;
    } baraja;

    received = recv(sock,(char*)&baraja,sizeof(baraja),0);//recibe respuesta
    if(received<=0) { //error?
        printf("Error de conexión o el servidor cerró la conexión.\n");
        return;
    }

    jugadoresConectados = baraja.numeroDeJugadores;

    //Sleep(a + 100);

    if(baraja.baraja[52] == 10099){
        printf("Cartas recibidas!\n");
     
        for (int i = 0; i < 52; i++)
        {
            if(baraja.baraja[i] == 0 && (baraja.baraja[i + 1] == 0 || i == 52)){
                break; //si ya no hay más cartas, se sale
            }
            manoActual[i] = baraja.baraja[i];
        }
    }
    else{
        printf("Error al recibir las cartas!\n");
        return;
    }
    ActualID = baraja.ID;

    Sleep(400 - a); //estamos todos sincronizados (mas o menos xd)

    printf("La partida comienza, jugador %i!\n", ActualID);

    buffer.a = 1; //para actualizar el estado

    if(ActualID == 0){
        setEstado(sock, 100000000, 100000001, 100000000, "I dont played yet");
        qsort(manoActual, sizeof(manoActual) / sizeof(manoActual[0]), sizeof(int), comparar); //ordenar la mano
    }
    else{
        setEstado(sock, 100000000, 100000001 + GetRandomInteger(1,9) * 10, 100000000, "I dont played yet");
        imprimirMano(true); //ordenar la mano y imprimirla
    }
}

bool playing = false;
Boolean ready = false;

void waitingForGame(SOCKET sock){ //esperando a que todos estén listos
    if(ready == true){
            setEstado(sock, 0, 1, 0, "Ready");

            Boolean allReady = true;

            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if(players[i].active != 3){ //si es 3, el jugador está desconectado, pero su ID está libre
                    //printf("%i", i);
                    if(players[i].active != 2) break; //si el jugador no está conectado, se ignora y no hay más comprobaciones

                    if(getDigit(players[i].y, 0) == 0){ //si el jugador no está listo
                        allReady = false;
                        //printf("%i", getDigit(players[i].y, 0));
                        break; //si hay alguien que no está listo, allReady = false y se va
                    }
                }
            }

            //printf("%i", allReady);

            if(allReady){ //todos ready? empezamos

                playing = true;

                atStartingGame(sock);                 
            }
        }
        else{
            printf("Ready? (yes = 1, no = 0): "); //everybody must be ready to start
            int a;
            scanf("%i", &a);
            ready = (BOOLEAN)a;
            if(ready != 1) ready = 0; //sanitización de input
        }
}

int OnLineMain(SOCKET sock){ //este es el main (se ejecuta una vez cada dos del main real)

    buffer.a = 1;

    if(playing){
        onGame(sock);
    }
    else{  
        waitingForGame(sock);
    }

    skibidi
}

int main() {
    lastPlayer.x = 0;

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    inicializar(sock);

    bool findingNewData = false; //esto es 1 o 0 alternativamente en cada vuelta 

    setEstado(sock, 100000000, 100000000, 100000000, "No MSG"); //inicializa el estado del jugador

    while(true){ //leyendo el input del usuario

        findingNewData = !findingNewData; //alternando entre 1 y 0

        if(findingNewData == true)//el jugador elige
        {
            OnLineMain(sock);
        } 
        else if(findingNewData == false)//se lee
        { 
            int received;
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                buffer.a = 2;
                buffer.b = i; //para cada ID
                actualizarEstado(sock, false);//pide el dato

                struct {
                    char name[NAME_LEN];
                    int x,y,z;
                    char msg[NAME_LEN];
                } reply;

                 received = recv(sock,(char*)&reply,sizeof(reply),0);//recibe respuesta
                if(received<=0) break; //error?

                if(reply.name[0]=='\0' || (reply.x == -1 && reply.y == -1)){ //si no hay más jugadores validos, sale (me gustan los break)

                    for (int j = i; j < MAX_CLIENTS; j++)
                    {
                        players[i].x = 0;
                        players[i].y = 0;
                        players[i].z = 0;
                        players[i].active = 0; //no activo
                        strncpy(players[i].name, reply.name, NAME_LEN);
                        strncpy(players[i].lastmsg, reply.msg, NAME_LEN);
                    }
                    break;
                } 
                else
                {
                    players[buffer.b].x = reply.x;
                    players[buffer.b].y = reply.y;
                    players[buffer.b].z = reply.z;
                    players[buffer.b].active = 2; //activo y conectado
                    strncpy(players[buffer.b].name, reply.name, NAME_LEN);
                    strncpy(players[buffer.b].lastmsg, reply.msg, NAME_LEN);
                    //if(getDigit(players[buffer.b].y, 0) == 1)printf("Player %s (ID %d) Val: (%d,%d,%d) with msg: %s\n", reply.name, buffer.b, reply.x, reply.y, reply.z, reply.msg);
                }
            }

            if (received<=0)
            {
                printf("Error de conexion o el servidor cerro la conexion.\n");
                break;
            }
            
           
        }
    }

    closesocket(sock); //esto si no lo pongo no va xd
    WSACleanup(); //same
    skibidi
}
