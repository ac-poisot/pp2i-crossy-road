
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

obstacle* init_obst(obstacle* next, obstacle* prev, float x, int size) {
    obstacle* o = (obstacle*)malloc(sizeof(obstacle));
    o->next = next;
    if (next != NULL) {
        next->prev = o;
    }
    o->prev = prev;
    if (prev != NULL) {
        prev->next = o;
    }
    o->x = x;
    o->size = size;
    return o;
}

lane* init_lane(int y, float speed, obstacle* obst, int obst_size, int type, lane* prev, lane* next) {
    lane* l = (lane*)malloc(sizeof(lane));
    l->y = y;
    l->speed = speed;
    l->obst_size = obst_size;
    l->type = type;
    l->prev = prev;
    if (prev != NULL) {
        prev->next = l;
    }
    l->obstacles = obst;
    l->coins = (bool*)malloc(sizeof(bool)*24);
    for (int i=0; i<24; i=i+1) {
        l->coins[i] = false;
    }
    l->next = next;
    if (next != NULL) {
        next->prev = l;
    }
    return l;
}

void test_copy_obstacle(void) {
    obstacle* o1 = init_obst(NULL, NULL, 3, 2);
    obstacle* o2 = init_obst(NULL, o1, 6, 1);
    obstacle* o3 = init_obst(NULL, o2, 10, 3);
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

    obstacle* o1 = init_obst(NULL, NULL, LANE_WIDTH/2+1, 2);
    obstacle* o2 = init_obst(o1, NULL, 1, 2);
    lane* l1 = init_lane(4, -1, o2, 2, 4, NULL, NULL);

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
    obstacle* o1 = init_obst(NULL, NULL, LANE_WIDTH/2+1, 2);
    lane* l1 = init_lane(4, -1, o1, 2, 4, NULL, NULL);

    obstacle* o2 = init_obst(NULL, NULL, 1, 2);
    lane* l2 = init_lane(5, 1, o2, 2, 4, l1, NULL);

    update_lanes(l1);
    
    assert(l1->obstacles->x == LANE_WIDTH/2);
    assert(l2->obstacles->x == 2);

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

    obstacle* o1 = init_obst(NULL, NULL, LANE_WIDTH/2+1, 2);
    lane* l1 = init_lane(4, -1, o1, 2, 4, l->next->next->next, NULL);

    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c1 = minmax_rec(l1->prev,1,todo,p);
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

    obstacle* o1 = init_obst(NULL, NULL, LANE_WIDTH/2, 2);
    lane* l1 = init_lane(4, 1, o1, 2, 4, NULL, NULL);
    obstacle* o2 = init_obst(NULL, NULL, LANE_WIDTH/2-1, 2);
    lane* l2 = init_lane(4, 1, o2, 2, 4, l1, NULL);
    obstacle* o3 = init_obst(NULL, NULL, LANE_WIDTH/2, 2);
    lane* l3 = init_lane(4, 1, o3, 2, 4, l2, NULL);
    l2->next = l3;

    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c1 = minmax_rec(l2,1,todo,p);
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

    obstacle* o1 = init_obst(NULL, NULL, LANE_WIDTH/2+1, 2);
    lane* l1 = init_lane(4, -1, o1, 2, 4, l->next->next->next, NULL);

    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c2 = minmax_rec(l1->prev,2,todo,p);
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

    obstacle* o1 = init_obst(NULL, NULL, LANE_WIDTH/2, 2);
    lane* l1 = init_lane(4, -1, o1, 4, 4, l->next->next->next, NULL);

    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c3 = minmax_rec(l1->prev,3,todo,p);
    assert(o1->x == LANE_WIDTH/2); // les obstacles ne doivent pas avoir bouge
    assert(p.x == LANE_WIDTH/2); // le joueur ne doit pas avoir bouge
    assert(p.y == 3);
    assert(c3.score == 1);
    assert(c3.move == STAY);

    free_lanes(l);
}

void test_two_left(void) {
    // define the game
    player p;
    p.y=4;
    p.x=1;
    p.orientation=0;
    p.skin=0;
    lane* l = initialLanes();
    free_lanes(l->next->next->next->next);
    l->next->next->next->next = NULL;

    obstacle* o1 = init_obst(NULL, NULL, 2, 2);
    lane* l1 = init_lane(4, -1, o1, 2, 4, l->next->next->next, NULL);
    obstacle* o2 = init_obst(NULL, NULL, 1, 2);
    lane* l2 = init_lane(5, -1, o2, 2, 4, l1, NULL);
    
    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c4 = minmax_rec(l2->prev,2,todo,p);
    assert(o1->x == 2); // les obstacles ne doivent pas avoir bouge
    assert(p.x == 1); // le joueur ne doit pas avoir bouge
    assert(p.y == 4);
    assert(c4.score == -1);
    assert(c4.move == GO_LEFT);

    free_lanes(l);
}

void test_three_down(void) {
    // define the game
    player p;
    p.y=4;
    p.x=1;
    p.orientation=0;
    p.skin=0;
    lane* l = initialLanes();
    free_lanes(l->next->next->next->next);
    l->next->next->next->next = NULL;

    obstacle* o1 = init_obst(NULL, NULL, 2, 2);
    lane* l1 = init_lane(4, -1, o1, 2, 4, l->next->next->next, NULL);
    obstacle* o2 = init_obst(NULL, NULL, 1, 2);
    lane* l2 = init_lane(5, -1, o2, 2, 4, l1, NULL);
    
    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c4 = minmax_rec(l2->prev,3,todo,p);
    assert(o1->x == 2); // les obstacles ne doivent pas avoir bouge
    assert(p.x == 1); // le joueur ne doit pas avoir bouge
    assert(p.y == 4);
    assert(c4.score == 0);
    assert(c4.move == GO_DOWN);

    free_lanes(l);
}

void test_n_update(void) {
    obstacle* o1 = init_obst(NULL, NULL, LANE_WIDTH/2, 2);
    lane* l1 = init_lane(4, -1, o1, 2, 4, NULL, NULL);
    obstacle* o2 = init_obst(NULL, NULL, 1, 2);
    lane* l2 = init_lane(5, 1, o2, 2, 4, l1, NULL);
    l2->prev = l1;
    l1->next = l2;

    lane** tab = n_update(3, l1);
    assert(tab[0] != l1);
    assert(tab[1] != l1);
    assert(tab[2] != l1);
    assert(tab[2]->obstacles->x == LANE_WIDTH/2-1);
    assert(tab[2]->next->obstacles->x == 2);
    assert(tab[1]->obstacles->x == LANE_WIDTH/2-2);
    assert(tab[1]->next->obstacles->x == 3);
    assert(tab[0]->obstacles->x == LANE_WIDTH/2-3);
    assert(tab[0]->next->obstacles->x == 4);
    assert(l1->obstacles->x == LANE_WIDTH/2);
    assert(l2->obstacles->x == 1);
    free_update(tab, 3);
    free_lanes(l1);
}
 
int main(void) {
    //test_copy_obstacle();
    //test_copy_lane();
    //test_collide();
    //test_update_lanes();
    //test_minmax_rec_begining();
    //test_one_stay();
    //test_one_right();
    //test_two_rigth();
    //test_three_stay();
    //test_three_down();
    //test_two_left();
    test_n_update();
    return 0;
}
