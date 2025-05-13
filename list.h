#pragma once

typedef struct couple_s {
    int score;
    int move;
} couple;

struct sList {
    int id;
    float x;
    float y;
    couple cpl;
    struct sList *next;
};

typedef struct sList List;

List *create_list(int index, float x, float y, couple c, List *next);
void free_list(List *l);
int id(List *l);
float v_x(List* l);
float v_y(List* l);
List *next(List *list);
List *append(int index, float x, float y, couple c, List *list);
couple is_in(int i, float x, float y, List *list);
void print_list(List* l);
