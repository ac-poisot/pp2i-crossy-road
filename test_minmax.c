
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "minmax.h"

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
    assert(l4->next->next == NULL);
    l4->speed = 4; // normally never obtains
    assert(l1->speed == s1);
    l4->next->speed = 4;
    assert(l1->next->speed == s2);
    assert(l2->speed == s2);
    assert(l3->speed == s3);
    assert(l1->next->next->speed == s3);

    free_lanes(l1);
    free_lanes(l4);
}

int main(void) {
    test_copy_obstacle();
    return 0;
}
