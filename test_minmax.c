
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "minmax.h"


#define LANE_WIDTH 24 // width of the displayed area
#define UNPLAYABLE_WIDTH 2

#define NOT_POSSIBLE 0
#define GO_AHEAD 1
#define GO_DOWN  2
#define GO_RIGHT 3
#define GO_LEFT  4
#define STAY     5

void test_copy_obstacle(void) {
    obstacle* o1 = (obstacle*)malloc(sizeof(obstacle));
    o1->next = NULL;
    o1->prev = NULL;
    o1->x = 3;
    o1->size = 2;
    obstacle* o2 = (obstacle*)malloc(sizeof(obstacle));
    o2->next = NULL;
    o2->prev = o1;
    o2->x = 6;
    o2->size = 1;
    obstacle* o3 = (obstacle*)malloc(sizeof(obstacle));
    o3->next = NULL;
    o3->prev = o2;
    o3->x = 10;
    o3->size = 3;
    o2->next = o3;
    o1->next = o2;

    obstacle* o4 = copy_obstacle(o1);
    o4->next->x = 8;
    assert(o2->x==6);
    assert(o1->next->x==6);

    free_obstacles(o1);
    free_obstacles(o4);
}

void test_copy_lane(void) {
    lane* l1 = random_lane(NULL);
    int s1 = l1->speed;
    lane* l2 = random_lane(l1);
    int s2 = l2->speed;
    lane* l3 = random_lane(l2);
    int s3 = l3->speed;

    lane* l4 = copy_lane(l1,2);
    assert(l4->prev == NULL);
    assert(l4 != l1); // must have different pointers
    assert(l4->next != l2);
    assert(l4->next->next != l3);
    assert(l4->next->next->next == NULL);
    assert(l4->speed == s1);
    assert(l4->next->speed == s2);
    assert(l4->next->next->speed == s3);
    l4->speed = 4; // normally never obtains
    assert(l1->speed == s1);
    l4->next->speed = 5;
    assert(l1->next->speed == s2);
    assert(l2->speed == s2);
    assert(l3->speed == s3);
    assert(l1->next->next->speed == s3);

    lane* l5 = copy_lane(l3,2);
    assert(l5->prev != NULL);
    assert(l5 != l3); // must have different pointers
    assert(l5->next == NULL);
    assert(l5->prev != l2);
    assert(l5->prev->prev->prev == NULL);
    assert(l5->speed == s3);
    assert(l5->prev->speed == s2);
    assert(l5->prev->prev->speed == s1);
    l5->speed = 4; // normally never obtains
    assert(l1->speed == s1);
    l5->prev->speed = 5;
    assert(l1->next->speed == s2);
    assert(l2->speed == s2);
    assert(l3->speed == s3);
    assert(l1->next->next->speed == s3);

    free_lanes(l1);
    free_lanes(l4);
    free_lanes(l5->prev->prev);
}

void test_collide(void) {
    player p;
    p.y=0;
    p.x=0;
    p.orientation=0;
    p.skin=0;

    obstacle* o1 = (obstacle*)malloc(sizeof(obstacle));
    o1->next = NULL;
    o1->prev = NULL;
    o1->x = LANE_WIDTH/2+1;
    o1->size = 2;
    lane* l1 = (lane*)malloc(sizeof(lane));
    l1->y = 4;
    l1->speed = -1;
    l1->obst_size = 2;
    l1->type = 4;
    l1->prev = NULL;
    l1->coins = (bool*)malloc(sizeof(bool)*24);
    for (int i=0; i<24; i=i+1) {
        l1->coins[i] = false;
    }
    l1->next = NULL;

    obstacle* o2 = (obstacle*)malloc(sizeof(obstacle));
    o2->prev = NULL;
    o2->next= o1;
    o1->prev = o2;
    o2->x = 1;
    o2->size = 2;

    l1->obstacles = o2;

    assert(!collides_without_game(l1, p));
    p.x = 1;
    assert(collides_without_game(l1, p));
    p.x = 2;
    assert(collides_without_game(l1, p));
    p.x = 3; 
    assert(!collides_without_game(l1, p));
    p.x = LANE_WIDTH/2;
    assert(!collides_without_game(l1, p));
    p.x = LANE_WIDTH/2+2;
    assert(collides_without_game(l1, p));
    p.x = LANE_WIDTH/2+3;
    assert(!collides_without_game(l1, p));

    free_lanes(l1);
}

void test_update_lanes(void) {
    obstacle* o1 = (obstacle*)malloc(sizeof(obstacle));
    o1->next = NULL;
    o1->prev = NULL;
    o1->x = LANE_WIDTH/2+1;
    o1->size = 2;
    lane* l1 = (lane*)malloc(sizeof(lane));
    l1->y = 4;
    l1->speed = -1;
    l1->obst_size = 2;
    l1->type = 4;
    l1->prev = NULL;
    l1->obstacles = o1;
    l1->coins = (bool*)malloc(sizeof(bool)*24);
    for (int i=0; i<24; i=i+1) {
        l1->coins[i] = false;
    }
    l1->next = NULL;

    obstacle* o2 = (obstacle*)malloc(sizeof(obstacle));
    o2->next = NULL;
    o2->prev = NULL;
    o2->x = 1;
    o2->size = 2;
    lane* l2 = (lane*)malloc(sizeof(lane));
    l2->y = 5;
    l2->speed = -1;
    l2->obst_size = 2;
    l2->type = 4;
    l2->prev = l1;
    l1->next = l2;
    l2->obstacles = o2;
    l2->coins = (bool*)malloc(sizeof(bool)*24);
    for (int i=0; i<24; i=i+1) {
        l2->coins[i] = false;
    }
    l2->next = NULL;

    update_lanes(l1);
    
    assert(l1->obstacles->x == LANE_WIDTH/2);
    assert(l2->obstacles->x == 0);

    free_lanes(l1);
}

void test_minmax_rec_begining(void) {
    // define the game
    player p;
    p.y=0;
    p.x=LANE_WIDTH/2;
    p.orientation=0;
    p.skin=0;
    lane* l = initialLanes();
    
    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c = minmax_rec(l, 3, todo, p);
    printf("score : %d, move : %d\n", c.score, c.move);
    assert(c.score == 3);
    assert(c.move == GO_AHEAD);

    free_lanes(l);
}

void test_one_stay(void) {
    // define the game
    player p;
    p.y=3;
    p.x=LANE_WIDTH/2;
    p.orientation=0;
    p.skin=0;
    lane* l = initialLanes();

    free_lanes(l->next->next->next->next);
    l->next->next->next->next = NULL;

    obstacle* o1 = (obstacle*)malloc(sizeof(obstacle));
    o1->next = NULL;
    o1->prev = NULL;
    o1->x = LANE_WIDTH/2+1;
    o1->size = 2;
    lane* l1 = (lane*)malloc(sizeof(lane));
    l1->y = 4;
    l1->speed = -1;
    l1->obst_size = 2;
    l1->type = 4;
    l1->prev = l->next->next->next;
    l->next->next->next->next = l1;
    l1->obstacles = o1;
    l1->coins = (bool*)malloc(sizeof(bool)*24);
    for (int i=0; i<24; i=i+1) {
        l1->coins[i] = false;
    }
    l1->next = NULL;

    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c1 = minmax_rec(l->next->next->next,1,todo,p);
    printf("le score obtenu %d\n", c1.score);
    printf("le mouvement obtenu %d\n", c1.move);
    assert(o1->x == LANE_WIDTH/2 + 1); // les obstacles ne doivent pas avoir bouge
    assert(p.x == LANE_WIDTH/2); // le joueur ne doit pas avoir bouge
    assert(p.y == 3);
    assert(c1.score == 0);
    assert(c1.move == STAY);

    free_lanes(l);

}

void test_one_right(void) {
    // define the game
    player p;
    p.y=1;
    p.x=LANE_WIDTH/2+1;
    p.orientation=0;
    p.skin=0;

    obstacle* o1 = (obstacle*)malloc(sizeof(obstacle));
    o1->next = NULL;
    o1->prev = NULL;
    o1->x = LANE_WIDTH/2;
    o1->size = 2;
    lane* l1 = (lane*)malloc(sizeof(lane));
    l1->y = 4;
    l1->speed = 1;
    l1->obst_size = 2;
    l1->type = 4;
    l1->prev = NULL;
    l1->obstacles = o1;
    l1->coins = (bool*)malloc(sizeof(bool)*24);
    for (int i=0; i<24; i=i+1) {
        l1->coins[i] = false;
    }
    l1->next = NULL;

    obstacle* o2 = (obstacle*)malloc(sizeof(obstacle));
    o2->next = NULL;
    o2->prev = NULL;
    o2->x = LANE_WIDTH/2-1;
    o2->size = 2;
    lane* l2 = (lane*)malloc(sizeof(lane));
    l2->y = 4;
    l2->speed = 1;
    l2->obst_size = 2;
    l2->type = 4;
    l2->prev = l1;
    l2->obstacles = o2;
    l2->coins = (bool*)malloc(sizeof(bool)*24);
    for (int i=0; i<24; i=i+1) {
        l1->coins[i] = false;
    }
    l2->next = NULL;
    l1->next = l2;

    obstacle* o3 = (obstacle*)malloc(sizeof(obstacle));
    o3->next = NULL;
    o3->prev = NULL;
    o3->x = LANE_WIDTH/2;
    o3->size = 2;
    lane* l3 = (lane*)malloc(sizeof(lane));
    l3->y = 4;
    l3->speed = 1;
    l3->obst_size = 2;
    l3->type = 4;
    l3->prev = l2;
    l3->obstacles = o3;
    l3->coins = (bool*)malloc(sizeof(bool)*24);
    for (int i=0; i<24; i=i+1) {
        l1->coins[i] = false;
    }
    l3->next = NULL;
    l2->next = l3;

    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c1 = minmax_rec(l2,1,todo,p);
    printf("le score obtenu %d\n", c1.score);
    printf("le mouvement obtenu %d\n", c1.move);
    assert(c1.score == 0);
    assert(c1.move == GO_RIGHT);

    free_lanes(l1);

}

void test_two_rigth(void) {
    // define the game
    player p;
    p.y=3;
    p.x=LANE_WIDTH/2;
    p.orientation=0;
    p.skin=0;
    lane* l = initialLanes();
    free_lanes(l->next->next->next->next);
    l->next->next->next->next = NULL;

    obstacle* o1 = (obstacle*)malloc(sizeof(obstacle));
    o1->next = NULL;
    o1->prev = NULL;
    o1->x = LANE_WIDTH/2+1;
    o1->size = 2;
    lane* l1 = (lane*)malloc(sizeof(lane));
    l1->y = 4;
    l1->speed = -1;
    l1->obst_size = 2;
    l1->type = 4;
    l1->prev = l->next->next->next;
    l->next->next->next->next = l1;
    l1->obstacles = o1;
    l1->coins = (bool*)malloc(sizeof(bool)*24);
    for (int i=0; i<24; i=i+1) {
        l1->coins[i] = false;
    }
    l1->next = NULL;

    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c2 = minmax_rec(l->next->next->next,2,todo,p);
    printf("le score obtenu %d\n", c2.score);
    printf("le mouvement obtenu %d\n", c2.move);
    assert(o1->x == LANE_WIDTH/2 + 1); // les obstacles ne doivent pas avoir bouge
    assert(p.x == LANE_WIDTH/2); // le joueur ne doit pas avoir bouge
    assert(p.y == 3);
    assert(c2.score == 1);
    assert(c2.move == GO_RIGHT);

    free_lanes(l);
}

void test_three_stay(void) {
    // define the game
    player p;
    p.y=3;
    p.x=LANE_WIDTH/2;
    p.orientation=0;
    p.skin=0;
    lane* l = initialLanes();
    free_lanes(l->next->next->next->next);
    l->next->next->next->next = NULL;

    obstacle* o1 = (obstacle*)malloc(sizeof(obstacle));
    o1->next = NULL;
    o1->prev = NULL;
    o1->x = LANE_WIDTH/2;
    o1->size = 2;
    lane* l1 = (lane*)malloc(sizeof(lane));
    l1->y = 4;
    l1->speed = -1;
    l1->obst_size = 4;
    l1->type = 4;
    l1->prev = l->next->next->next;
    l->next->next->next->next = l1;
    l1->obstacles = o1;
    l1->coins = (bool*)malloc(sizeof(bool)*24);
    for (int i=0; i<24; i=i+1) {
        l1->coins[i] = false;
    }
    l1->next = NULL;

    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c3 = minmax_rec(l->next->next->next,3,todo,p);
    printf("le score obtenu %d\n", c3.score);
    printf("le mouvement obtenu %d\n", c3.move);
    assert(o1->x == LANE_WIDTH/2); // les obstacles ne doivent pas avoir bouge
    assert(p.x == LANE_WIDTH/2); // le joueur ne doit pas avoir bouge
    assert(p.y == 3);
    assert(c3.score == 1);
    assert(c3.move == STAY);

    free_lanes(l);
}

void test_two_down(void) {
    // define the game
    player p;
    p.y=4;
    p.x=1;
    p.orientation=0;
    p.skin=0;
    lane* l = initialLanes();
    free_lanes(l->next->next->next->next);
    l->next->next->next->next = NULL;

    obstacle* o1 = (obstacle*)malloc(sizeof(obstacle));
    o1->next = NULL;
    o1->prev = NULL;
    o1->x = 2;
    o1->size = 2;
    lane* l1 = (lane*)malloc(sizeof(lane));
    l1->y = 4;
    l1->speed = -1;
    l1->obst_size = 2;
    l1->type = 4;
    l1->prev = l->next->next->next;
    l->next->next->next->next = l1;
    l1->obstacles = o1;
    l1->coins = (bool*)malloc(sizeof(bool)*24);
    for (int i=0; i<24; i=i+1) {
        l1->coins[i] = false;
    }
    l1->next = NULL;

    obstacle* o2 = (obstacle*)malloc(sizeof(obstacle));
    o2->next = NULL;
    o2->prev = NULL;
    o2->x = 1;
    o2->size = 2;
    lane* l2 = (lane*)malloc(sizeof(lane));
    l2->y = 5;
    l2->speed = -1;
    l2->obst_size = 2;
    l2->type = 4;
    l2->prev = l1;
    l1->next = l2;
    l2->obstacles = o2;
    l2->coins = (bool*)malloc(sizeof(bool)*24);
    for (int i=0; i<24; i=i+1) {
        l2->coins[i] = false;
    }
    l2->next = NULL;
    
    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c4 = minmax_rec(l->next->next->next->next,2,todo,p);
    printf("le score obtenu %d\n", c4.score);
    printf("le mouvement obtenu %d\n", c4.move);
    assert(o1->x == 2); // les obstacles ne doivent pas avoir bouge
    assert(p.x == 1); // le joueur ne doit pas avoir bouge
    assert(p.y == 4);
    assert(c4.score == -1);
    assert(c4.move == GO_DOWN);

    free_lanes(l);
}

int main(void) {
    //test_copy_obstacle();
    //test_copy_lane();
    //test_collide();
    //test_update_lanes();
    //test_minmax_rec_begining();
    test_one_stay();
    test_one_right();
    test_two_rigth();
    test_three_stay();
    //test_two_down();
    return 0;
}
