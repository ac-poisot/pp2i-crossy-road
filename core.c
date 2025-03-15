#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "core.h"

int tick;


void free_obstacles(obstacle* first_obstacle) {
    /* frees recursively obstacles */
    obstacle* current_obstacle = first_obstacle;
    obstacle* next_obstacle;
    while (current_obstacle != NULL) {
        next_obstacle = current_obstacle->next;
        free(current_obstacle);
        current_obstacle = next_obstacle;
    }
}

void free_lanes(lane* first_lane) {
    /* frees recursively lanes */
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
    /* chooses randomly a number */
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
    /* weigth for probabilities depending on time */
    return (4-index) * tick;
}

int* probabilite_biomes(lane* l) {// prend les 4 lignes et renvoie les probabilité dans le panier
    /* creates the urn for the generation of biomes */
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
    /* creates an empty lane */
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
    new_lane->obst_size = 1+rand()%2;
    new_lane->coins = malloc(LANE_WIDTH * sizeof(bool));
    for (int i = 0; i < LANE_WIDTH; i++) {
        new_lane->coins[i] = false;
    }
    new_lane->type = type;
    new_lane->next = NULL;
    
    return new_lane;
}


lane* initialLanes(void) {
    /* initializes the four first lanes */
    lane* l = empty_lane(NULL, GRASS);
    lane* firstLane = l;
    for (int i = 0; i < 4; i++) {
        l = empty_lane(l, GRASS);
    }
    return firstLane;
}

lane* random_lane(lane* prev_lane) {
    lane* new_lane = malloc(sizeof(lane));
    if (prev_lane == NULL) {
        new_lane->y = 0;
    } else {
        prev_lane->next = new_lane;
        new_lane->y = prev_lane->y + 1;
    }
    new_lane->speed = 0;

    // Only one obstacle, for testing purposes
    new_lane->obstacles = malloc(sizeof(obstacle));
    new_lane->obstacles->next = NULL;
    new_lane->obstacles->prev = NULL;
    new_lane->obstacles->x = (rand() % (LANE_WIDTH - UNPLAYABLE_WIDTH * 2)) + UNPLAYABLE_WIDTH ;
    new_lane->type = (rand() % 4) + 1;

    if (new_lane->type == WATER) {
        new_lane->obst_size = 5;
    } else {
        new_lane->obst_size = 1;
    }


    new_lane->coins = malloc(LANE_WIDTH * sizeof(bool));
    for (int i = 0; i < LANE_WIDTH; i++) {
        new_lane->coins[i] = !(rand() % 30); // for each tile, 1/30 chance of having a coin
    }
    new_lane->prev = prev_lane;
    new_lane->next = NULL;
    return new_lane;
}

obstacle* generate_vehicles(lane* l) {
    /* generates vehicles */
    float x = - (float)(rand()%12);
    float current_x = x;
    obstacle* first_obst = (obstacle*)malloc(sizeof(obstacle));
    first_obst->x = current_x;
    first_obst->prev = NULL;
    first_obst->next = NULL;
    first_obst->size = l->obst_size;
    obstacle *last_generated = first_obst;
    while (current_x < LANE_WIDTH) {
        current_x += (float)(6*(rand()%2)+6);
        obstacle* obst = (obstacle*)malloc(sizeof(obstacle));
        obst->x = current_x;
        obst->next = NULL;
        obst->prev = last_generated;
        obst->size = l->obst_size;
        last_generated->next = obst;
        last_generated = obst;
    }
    return first_obst;
}

obstacle* generate_drowning_slots(void) {
    /* generates the slots where the player would drown */
    float x = (float)(rand()%5 - 8);
    float current_x = x;
    obstacle* first_obst = (obstacle*)malloc(sizeof(obstacle));
    first_obst->x = current_x;
    first_obst->prev = NULL;
    first_obst->next = NULL;
    first_obst->size = 1 + (rand()%3);
    obstacle *last_generated = first_obst;
    while(current_x < LANE_WIDTH) {
        current_x += (float)last_generated->size + (float)(rand()%5);
        obstacle* obst = (obstacle*)malloc(sizeof(obstacle));
        obst->x = current_x;
        obst->next = NULL;
        obst->prev = last_generated;
        obst->size = 1 + (rand()%3);
        last_generated-> next = obst;
        last_generated = obst;
    }
    return first_obst;
}

obstacle* generate_trains(void) {
    float x = -(float)(rand()%TRAIN_LENGTH + TRAIN_SPEED);
    float current_x = x;
    obstacle* first_obst = (obstacle*)malloc(sizeof(obstacle));
    first_obst->x = current_x;
    first_obst->prev = NULL;
    first_obst->next = NULL;
    first_obst->size = TRAIN_LENGTH;
    obstacle *last_generated = first_obst;
    while (current_x < LANE_WIDTH) {
        current_x += (float)(TRAIN_LENGTH + TRAIN_SPEED + rand()%TRAIN_SPEED);
        obstacle* obst = (obstacle*)malloc(sizeof(obstacle));
        obst->x = current_x;
        obst->next = NULL;
        obst->prev = last_generated;
        obst->size = TRAIN_LENGTH;
        last_generated->next = obst;
        last_generated = obst;
    }
    return first_obst;
}


lane* generate_lane(lane* prev_lane, int type) {
    /* creates an empty lane */
    lane* new_lane = malloc(sizeof(lane));
    if (prev_lane != NULL) {
        new_lane->y = prev_lane->y + 1;
        prev_lane->next = new_lane;
    } else {
        new_lane->y = 0;
    }
    new_lane->prev = prev_lane;
    new_lane->obst_size = 1+rand()%2;
    switch (type) {
        case GRASS:
        case WATER:
            new_lane->obstacles = generate_drowning_slots();
            new_lane->speed = 3;
            break;
        case TRACK:
            new_lane->obstacles = generate_trains();
            new_lane->speed = TRAIN_SPEED;
            break;
        case ROAD:
            new_lane->obstacles = generate_vehicles(new_lane);
            new_lane->speed = -2;
            break;
    }
    new_lane->coins = malloc(LANE_WIDTH * sizeof(bool));
    for (int i = 0; i < LANE_WIDTH; i++) {
        new_lane->coins[i] = false;
    }
    new_lane->type = type;
    new_lane->next = NULL;
    
    return new_lane;
}


void update_vehicles(lane* l) {
    /* updates the position of the vehicles and add a new one when the next displayed vehicle doesn't exist */
    if(l->obstacles == NULL) {
        return;
    }
    obstacle* c = l->obstacles;
    if (l->speed > 0 && c->x+l->speed+c->size > 6) {
        obstacle* new_obst = (obstacle*)malloc(sizeof(obstacle));
        float new_x = c->x - (float)(6*(rand()%2)+6);
        new_obst-> prev = NULL;
        new_obst-> next = c;
        new_obst->x = new_x;
        new_obst->size = l->obst_size;
        c->prev = new_obst;
        l->obstacles = new_obst;
        c = new_obst;
    }
    while (c != NULL) {
        if ((c->x + l->speed > LANE_WIDTH) && (l->speed > 0)){
            c-> prev ->next = NULL;
            if(c->next != NULL) {
                obstacle* current = c->next;
                while(current != NULL) {
                    obstacle* next = current->next;
                    free(current);
                    current = next;
                    if(next != NULL) {
                        next = next->next;
                    }
                }
            }
            free(c);
            break;
        } else if ((c->x + l->speed + l->obst_size < 1) && (l->speed < 0)){
            c->next->prev = NULL;
            obstacle* nc = c->next;
            free(c);
            c = nc;
            l->obstacles = c;
        } else if (c->next == NULL && l->speed < 0 && c->x+l->speed < LANE_WIDTH) {
            c->x = c->x + l->speed;
            obstacle* next_obst = (obstacle*)malloc(sizeof(obstacle));
            float current_x = c->x + (float)(6*(rand()%2)+6);
            next_obst->x = current_x;
            next_obst->size = l->obst_size;
            next_obst->next = NULL;
            next_obst->prev = c;
            c->next = next_obst;
            c = c->next;
            break;
        } else {
            c->x = c->x + l->speed;
            c = c ->next;
        }
    }
}

void update_drowning_slots(lane* l) {
    /* updates the position of the drowning slots*/
    if(l->obstacles == NULL) {
        return;
    }
    obstacle* c = l->obstacles;
    if (l->speed > 0 && c->x+l->speed +c->size> 0) {
        obstacle* new_obst = (obstacle*)malloc(sizeof(obstacle));
        new_obst->size = 1 + (rand()%3);
        float new_x = c->x - (float)(new_obst->size + (rand()%5));
        new_obst-> prev = NULL;
        new_obst-> next = c;
        new_obst->x = new_x;
        c->prev = new_obst;
        l->obstacles = new_obst;
        c = new_obst;
    }
    while (c != NULL) {
        if ((c->x + l->speed > LANE_WIDTH) && (l->speed > 0)){
            c-> prev ->next = NULL;
            if(c->prev != NULL) {
                obstacle* current = c->next;
                while(current != NULL) {
                    obstacle* next = current->next;
                    free(current);
                    current = next;
                    if(next != NULL) {
                        next = next->next;
                    }
                }
            }
            free(c);
            break;
        } else if ((c->x + l->speed + c->size < 1) && (l->speed < 0)){
            c->next->prev = NULL;
            obstacle* nc = c->next;
            free(c);
            c = nc;
            l->obstacles = c;
        } else if (c->next == NULL && l->speed < 0 && c->x+l->speed < LANE_WIDTH) {
            c->x = c->x + l->speed;
            obstacle* next_obst = (obstacle*)malloc(sizeof(obstacle));
            next_obst->size = 1 + (rand()%3);
            float current_x = c->x + (float)(next_obst->size + (rand()%5));
            next_obst->x = current_x;
            next_obst->next = NULL;
            next_obst->prev = c;
            c->next = next_obst;
            c = c->next;
            break;
        } else {
            c->x = c->x + l->speed;
            c = c ->next;
        }
    }
}

void update_trains(lane* l) {
    if(l->obstacles == NULL) {
        return;
    }
    obstacle* c = l->obstacles;
    if (l->speed > 0 && c->x+l->speed +c->size> 0) {
        obstacle* new_obst = (obstacle*)malloc(sizeof(obstacle));
        new_obst->size = TRAIN_LENGTH;
        float new_x = c->x - (float)(TRAIN_LENGTH + TRAIN_SPEED + rand()%TRAIN_SPEED);
        new_obst-> prev = NULL;
        new_obst-> next = c;
        new_obst->x = new_x;
        c->prev = new_obst;
        l->obstacles = new_obst;
        c = new_obst;
    }
    while (c != NULL) {
        if ((c->x + l->speed > LANE_WIDTH) && (l->speed > 0)){
            c-> prev ->next = NULL;
            if(c->prev != NULL) {
                obstacle* current = c->next;
                while(current != NULL) {
                    obstacle* next = current->next;
                    free(current);
                    current = next;
                    if(next != NULL) {
                        next = next->next;
                    }
                }
            }
            free(c);
            break;
        } else if ((c->x + l->speed + c->size < 1) && (l->speed < 0)){
            c->next->prev = NULL;
            obstacle* nc = c->next;
            free(c);
            c = nc;
            l->obstacles = c;
        } else if (c->next == NULL && l->speed < 0 && c->x+l->speed < LANE_WIDTH) {
            c->x = c->x + l->speed;
            obstacle* next_obst = (obstacle*)malloc(sizeof(obstacle));
            next_obst->size = TRAIN_LENGTH;
            float current_x = c->x + (float)(TRAIN_LENGTH + TRAIN_SPEED + rand()%TRAIN_SPEED);
            next_obst->x = current_x;
            next_obst->next = NULL;
            next_obst->prev = c;
            c->next = next_obst;
            c = c->next;
            break;
        } else {
            c->x = c->x + l->speed;
            c = c ->next;
        }
    }
}

void display_obstacles(obstacle* o) {
    /* displays the position of the vehicles -- debbug function */
    while (o != NULL) {
        printf("%f\n", o->x);
        o = o->next;
    }
}

void displayLanes(lane* l) {
    /* displays the lanes */
    int i = 0;
    while (l != NULL) {
        obstacle* current = NULL;
        if(l->obstacles != NULL) {
            current = l->obstacles;
            while(current != NULL && (int)current->x + current->size <= 0) {
                current = current->next;
            }
            if(current != NULL && current->x < 0) {
                for(int i = 0; i < current->x + current->size; i++) {
                    printf("🟥");
                }
                i = current->x + current->size;
                current = current->next;
            }
        }
        for (; i < LANE_WIDTH; i++) {
            if(current != NULL && current->x == (float)i) {
                for(int j = 0; j < current->size; j++) {
                    if(i >= LANE_WIDTH) {
                        break;
                    }
                    printf("🟥");
                    i++;
                }
                i--;
                current = current->next;
            } else {
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

        }
        printf("\n");
        l = l->next;
    }
}

lane* fourLastLanes(lane* l) {
    /* gets the four previous lanes */
    lane* lastLane = l;
    for (int i = 0; i < 3; i++) {
        lastLane = lastLane->prev;
    }
    return lastLane;
}

void generateNNewLanes(lane* l, int n) {
    /* generates n lanes based on the four previous lanes */
    for (int i = 0; i < n; i++) {
        l = generate_lane(l, biome(probabilite_biomes(fourLastLanes(l))));
    }
}

bool collides(lane *current_lane, displayedData game) {
    obstacle* current_obstacle = current_lane->obstacles;
    while (current_obstacle != NULL) {
        if (game.player.x >= current_obstacle->x && game.player.x < current_obstacle->x + current_lane->obst_size) {
            return true;
        }
        current_obstacle = current_obstacle->next;
    }
    return false;
}

displayedData move_camera(displayedData data) {
    // Move the camera
    data.cameraY++;
    data.camera_first_lane = data.camera_first_lane->next;

    // Add a new lane
    lane* current_lane = data.camera_first_lane;
    while (current_lane->next != NULL) {
        current_lane = current_lane->next;
    }
    random_lane(current_lane);

    return data;
}


displayedData init_game(int game_height) {
    // Initialize the game
    player p = {PLAYER_START_Y, LANE_WIDTH/2, 0, 0};

    // Initialize the first lane
    lane* l = empty_lane(NULL, GRASS);
    lane* first_lane = l;

    // Generate the first lanes

    for (int i = 0; i < 10; i++) {
         l = empty_lane(l, GRASS);
    }

    for (int i = 0; i < game_height + 4; i++) {
        l = random_lane(l);
    }

    displayedData res = {game_height, first_lane, first_lane, p, 0, 0};

    return res;
}


#ifdef TEST
int main(void) {
    srand(time(NULL));
    lane* firstLane = initialLanes();
    firstLane->speed = 2;
    //generateNNewLanes(firstLane->next->next->next, 100);
    
    //displayLanes(firstLane);

    obstacle* v1 = generate_vehicles(firstLane);
    firstLane->obstacles = v1;
    firstLane->obst_size = -2;
    display_obstacles(firstLane->obstacles);
    printf("\n");
    for(int i=0; i<5; i=i+1) {
        update_vehicles(firstLane);
        display_obstacles(firstLane->obstacles);
        printf("\n");
    }
    //free_obstacles(v1); // ne pas free si v1 lie a firstLane

    free_lanes(firstLane);
    return 0;
}
#endif
