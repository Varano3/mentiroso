#include <stdio.h>
#include <math.h>
#include <windows.h> //esto es lo del multiplayer, pero por algún casual no es obligatorio
//#include <stdlib.h>

#include "mirmecocrimenesdeguerra.maven"

Buffer buffer = {0,0,0,0,"msg"};

typedef struct {
    int x, y, z;
    SOCKET sock;//multiplayer data
    int active;
    char name[NAME_LEN];
    char lastmsg[NAME_LEN];
} Player;


Player players[MAX_CLIENTS];

int GetRandomInteger(int a, int b) {
    int min = (a < b) ? a : b;
    int max = (a > b) ? a : b;
    return (rand() % (max - min + 1)) + min;
}

int jugadoresConectados = 0;

int ActualID = -1;

int getLastPlayerID(int ID, int n){
    int sol = ID - 1;
    if(sol < 0){
        sol = n - 1;
    }
    return sol;
}

int getNextPlayerID(int ID, int n){
    int sol = ID + 1;
    if(sol >= n){
        sol = 0;
    }

    return sol;
}

int getDigit(int number, int position){ //posición 0-8 (de izquierda a derecha)
    float a = (number / (int)(0.2 + pow(10, position))) % 10;
    return (int)a;
}

int setDigit(int number, int position, int newDigit) {
    int divisor = (int)(0.2 + pow(10, position));
    int currentDigit = (number / divisor) % 10;
    number -= currentDigit * divisor;     // quitar el dígito actual
    number += newDigit * divisor;         // poner el nuevo dígito
    return number;
}

void UpdateID(){
    
    if(getDigit(players[ActualID].y, 7) != 0) {
        printf("ID: %i ?? -> ", ActualID);
        ActualID = getDigit(players[ActualID].x, 1);
        players[ActualID].y = setDigit(players[ActualID].y, 7, 0);
        printf("ID: %i", ActualID);
    }
    
}