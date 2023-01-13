/** @file
 * Implementacja interfejsu charList.h za listy dwukierunkowej.
 * @author Artur Kamieniecki
 * @date 2022
 */

#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "charList.h"

/**
 * Struktura listy.
 */
struct charList {
    /**
     * Wskaźnik na początek listy, który jest strażnikiem.
     */
    ListNode *beginning;
    /**
     * Wskaźnik na koniec listy, który jest strażnikiem.
     */
    ListNode *end;
};

/**
 * Struktura elementu listy.
 */
struct listNode {
    /**
     * Wskaźnik na poprzedni element listy.
     */
    ListNode *previous;
    /**
     * Wskaźnik na następny element listy.
     */
    ListNode *next;
    /**
     * Wskaźnik na napis przechowywany w liście.
     */
    char *str;
};

CharList *createCharList(void) {
    CharList *list = malloc(sizeof(CharList));
    ListNode *beginning = malloc(sizeof(ListNode));
    ListNode *end = malloc(sizeof(ListNode));

    if (list != NULL && beginning != NULL && end != NULL) {
        beginning->next = end;
        beginning->previous = NULL;
        beginning->str = NULL;

        end->next = NULL;
        end->previous = beginning;
        end->str = NULL;

        list->beginning = beginning;
        list->end = end;

        return list;
    }
    else {
        if (list != NULL) {
            free(list);
        }
        if (beginning != NULL) {
            free(beginning);
        }
        if (end != NULL) {
            free(end);
        }

        return NULL;
    }
}

void freeCharList(CharList *charList) {
    if (charList != NULL) {
        ListNode *current = charList->beginning;
        ListNode *next = NULL;

        while (current != NULL) {
            next = current->next;
            freeListNode(current);
            current = next;
        }

        free(charList);
    }
}

void addCharList(CharList *charList, ListNode *node) {
    node->previous = charList->end->previous;
    node->next = charList->end;

    charList->end->previous->next = node;
    charList->end->previous = node;
}

void removeListNode(ListNode *node) {
    if (node != NULL) {
        if (node->previous != NULL && node->next != NULL) {
            node->previous->next = node->next;
            node->next->previous = node->previous;
        }

        freeListNode(node);
    }
}

bool isListEmpty(CharList *list) {
    if (list != NULL) {
        if (list->beginning->next == list->end) {
            return true;
        }
    }

    return false;
}

ListNode *createListNode(char const *str) {
    ListNode *node = malloc(sizeof(ListNode));

    if (node == NULL) {
        return NULL;
    }

    node->str = malloc(strlen(str) + 1);
    if (node->str == NULL) {
        free(node);
        return NULL;
    }

    strcpy(node->str, str);

    node->previous = NULL;
    node->next = NULL;

    return node;
}

void freeListNode(ListNode *node) {
    if (node != NULL) {
        free(node->str);
    }
    free(node);
}

char *getListNodeStr(ListNode *node) {
    if (node != NULL) {
        return node->str;
    }

    return NULL;
}

ListNode *nextListNode(ListNode *node) {
    if (node != NULL) {
        if (node->next->next == NULL) {
            return NULL;
        }
        return node->next;
    }

    return NULL;
}

ListNode *firstListNode(CharList *list) {
    if (list != NULL) {
        if (!isListEmpty(list)) {
            return list->beginning;
        }
    }

    return NULL;
}