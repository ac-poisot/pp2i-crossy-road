#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "core.h"

int tick;


void free_obstacles(obstacle* first_obstacle) {
    obstacle* current_obstacle = first_obstacle;
    obstacle* next_obstacle;
    while (current_obstacle != NULL) {
        next_obstacle = current_obstacle->next;
        free(current_obstacle);
        current_obstacle = next_obstacle;
    }
}

void free_lanes(lane* first_lane) {
    lane* current_lane = first_lane;
    lane* next_lane;
    while (current_lane != NULL) {
        next_lane = current_lane->next;
        free(current_lane->coins);
        free_obstacles(current_lane->obstacles);
        free(current_lane);
        current_lane = next_lane;
    }
}


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

int* probabilite_biomes(lane* l) {// prend les 4 lignes et renvoie les probabilité dans le panier
    int a = 1000;
    int b = 1000;
    int c = 1000;
    int d = 1000;
    for (int i=0;i<4;i++){
        if (l->type == 1) {
            a += weight(i);
        }
        else if (l->type == 2) {
            b += weight(i);
        }
        else if (l->type == 3) {
            c += weight(i);
        }
        else if (l->type == 4) {
            d += weight(i);
        }
        l = l->next;
    }
    int* boules = malloc(4*sizeof(int));
    boules[0] = a;
    boules[1] = b;
    boules[2] = c;
    boules[3] = d;
    return boules;
}// Cette version ne prend pas en compte l'avancée dans le temps


lane* empty_lane(lane* prev_lane, int type) {
    lane* new_lane = malloc(sizeof(lane));
    if (prev_lane != NULL) {
        new_lane->y = prev_lane->y + 1;
        prev_lane->next = new_lane;
    } else {
        new_lane->y = 0;
    }
    new_lane->prev = prev_lane;
    new_lane->speed = 0;
    new_lane->obstacles = NULL;
    new_lane->coins = malloc(LANE_WIDTH * sizeof(bool));
    for (int i = 0; i < LANE_WIDTH; i++) {
        new_lane->coins[i] = false;
    }
    new_lane->type = type;
    new_lane->next = NULL;
    
    return new_lane;
}

lane* initialLanes(void) {
    lane* l = empty_lane(NULL, GRASS);
    lane* firstLane = l;
    for (int i = 0; i < 4; i++) {
        l = empty_lane(l, GRASS);
    }
    return firstLane;
}

void displayLanes(lane* l) {
    while (l != NULL) {
        for (int i = 0; i < LANE_WIDTH; i++) {
            
            switch (l->type) {
                case GRASS:
                    printf("🟩");
                    break;
                case WATER:
                    printf("🟦");
                    break;
                case TRACK:
                    printf("⬜️");
                    break;
                case ROAD:
                    printf("⬛️");
                    break;
            }

        }
        printf("\n");
        l = l->next;
    }
}

lane* fourLastLanes(lane* l) {
    lane* lastLane = l;
    for (int i = 0; i < 3; i++) {
        lastLane = lastLane->prev;
    }
    return lastLane;
}

void generateNNewLanes(lane* l, int n) {
    for (int i = 0; i < n; i++) {
        l = empty_lane(l, biome(probabilite_biomes(fourLastLanes(l))));
    }
}

int main(void) {
    srand(time(NULL));
    lane* firstLane = initialLanes();
    generateNNewLanes(firstLane->next->next->next, 100);
    
    displayLanes(firstLane);
    free_lanes(firstLane);
    return 0;
}
