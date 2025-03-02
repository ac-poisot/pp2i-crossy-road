#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int tick;

int biome(int* boules){// Sa prend les boules de chaque couleur, et renvoit la couleur pioché
    int nbr_de_boule= boules[1] + boules[2] + boules[3] + boules[0];
    int boule_choisi =rand()%nbr_de_boule;
    //return boule_choisi;
    tick++;
    if (boule_choisi < boules[0]) {
        return 1;
    }
    else if (boule_choisi < boules[1]+boules[0]) {
        return 2;
    }
    else if (boule_choisi<boules[1]+boules[2]+boules[0]) {
        return 3;
    }
    else {
        return 4;
    }
}

int weight(int index) {
    return (4-index) * tick;
}

int* probabilite_biomes(int* var) {// prend les 4 lignes et renvoie les probabilité dans le panier
    int a = 1000;
    int b = 1000;
    int c = 1000;
    int d = 1000;
    for (int i=0;i<4;i++){
        if (var[i] == 1) {
            a += weight(i);
        }
        else if (var[i] == 2) {
            b += weight(i);
        }
        else if (var[i] == 3) {
            c += weight(i);
        }
        else if (var[i] == 4) {
            d += weight(i);
        }
    }
    int* boules = malloc(4*sizeof(int));
    boules[0] = a;
    boules[1] = b;
    boules[2] = c;
    boules[3] = d;
    return boules;
}// Cette version ne prend pas en compte l'avancée dans le temps

 

int main() {
    srand(time(NULL));
    int* lanes = malloc(2000 * sizeof(int));
    lanes[1999] = 0;
    lanes[0] = 1;
    lanes[1] = 1;
    lanes[2] = 1;
    lanes[3] = 1;
    tick = 3;
    int i=3;
    printf("🟩🟩🟩🟩");
    while (lanes[1999]==0) {
        i++;
        int* four_lane = malloc(4*sizeof(int));
        four_lane[0] = lanes[i-1];
        four_lane[1] = lanes[i-2];
        four_lane[2] = lanes[i-3];
        four_lane[3] = lanes[i-4];
        int* boules = probabilite_biomes(four_lane);
        lanes[i] = biome(boules);
        free(boules);
        if (lanes[i] == 1) {
            printf("🟩");
        }
        else if (lanes[i] == 2) {
            printf("🟦");
        }
        else if (lanes[i] == 3) {
            printf("⬛️");
        }
        else if (lanes[i] == 4) {
            printf("⬜️");
        }
        free(four_lane);
    }
    
    printf("%i\n", tick);
    return 0;
}