#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"


List *create_list(int index, float x, float y, couple cpl, List *next){
    /* creer la liste */
    List* new = (List*)malloc(sizeof(List));
    new->id = index;
    new->x = x;
    new->y = y;
    new->cpl = cpl;
    new->next = next;
    return new;
}

void free_list(List *l){
    /* libere la liste */
    if ((l==NULL) || (l->next == NULL)) {
        free(l);
    } else {
        free_list(l->next);
        free(l);
    }
}

int id(List *list) {
    /* revoie la valeur du premier elem de la liste */
    if (list==NULL) {
        printf("La liste vide n'a pas de valeur\n");
        return -1;
    } else {
        return list->id;
    }
}

float v_x(List *list) {
    /* revoie la valeur du premier elem de la liste */
    if (list==NULL) {
        printf("La liste vide n'a pas de valeur\n");
        return -1;
    } else {
        return list->x;
    }
}

float v_y(List *list) {
    /* revoie la valeur du premier elem de la liste */
    if (list==NULL) {
        printf("La liste vide n'a pas de valeur\n");
        return -1;
    } else {
        return list->y;
    }
}

List *next(List *list) {
    /* renvoie la fin de liste */
    if (list == NULL) {
        printf("La liste vide n'a pas de suivant\n");
        return NULL;
    } else {
        return list->next;
    }
}

List *append(int index, float x, float y, couple cpl, List *list) {
    /* ajoute un element en fin de liste */
    List *c = list;
    if (c == NULL) {
        return create_list(index, x, y, cpl ,NULL);
    } else {;
        c = create_list(id(c), v_x(c), v_y(c), c->cpl, append(index, x, y, cpl, c->next));
        return c;
    }
}

couple is_in(int i, float x, float y, List *list) {
    couple res;
    List *c = list;
    if (c == NULL) {
        res.move = -1;
        res.score = -1;
        return res;
    } else if (id(c)==i && v_x(c)==x && v_y(c)==y){
        res.move = c->cpl.move;
        res.score = c->cpl.move;
        return res;
    } else {
        return is_in(i, x, y, next(c));
    }
}
