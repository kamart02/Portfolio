/** @file
 * Interfejs klasy przechowującej listę napisów.
 * @author Artur Kamieniecki
 * @date 2022
 */
#ifndef CHARLIST_H
#define CHARLIST_H

#include <stdbool.h>

/**
 * Definicja listy napisów.
 */
typedef struct charList CharList;
/**
 * Definicja pojedynczego elementu listy.
 */
typedef struct listNode ListNode;

/**
 * @brief Tworzy nową listę.
 * Tworzy nową listę.
 * @return Wskaźnik do struktury. Wartość @p NULL w przypadku braku pamięci.
 */
CharList *createCharList(void);

/**
 * @brief Dealokuje listę.
 * Dealokuje całą listę napisów. Gdy @p charList jest @p NULL nic nie robi.
 * @param [in] charList - wskaźnik na strukturę listy.
 */
void freeCharList(CharList *charList);

/**
 * @brief Dodaje element do listy
 * Dodaje element typu @p ListNode do struktury. Zarówno @p charList i
 * @p node nie mogą mieć wartości @p NULL.
 * @param [in, out] charList - wskaźnik na listę.
 * @param [in, out] node - wskaźnik na element do dodania.
 */
void addCharList(CharList *charList, ListNode *node);

/**
 * @brief Usuwa @p ListNode z listy.
 * Usuwa @p ListNode z listy oraz dealokuje pamięć zajmowaną przez ten element.
 * Nic nie robi gdy @p node jest @p NULL.
 * @param [in] node - wskaźnik na @p ListNode.
 */
void removeListNode(ListNode *node);

/**
 * @brief Sprawdza czy lista jest pusta.
 * Sprawdza czy lista jest pusta. W przypadku gdy @p list jest @p NULL zwraca
 * @p false.
 * @param [in] list - wskaźnik na listę.
 * @return @p true jeżeli lista jest pusta.
 *         @p false jeżeli lista nie jest pusta.
 */
bool isListEmpty(CharList *list);

/**
 * @brief Tworzy element listy.
 * Tworzy element listy. Zwraca @p NULL w przypadku gdy zabraknie pamięci.
 * @p str nie może mieć wartości null. Napis @p str jest kopiowany.
 * @param [in] str - napis reprezentowany jako null terminated string.
 * @return Wskaźnik na listę.
 */
ListNode *createListNode(char const *str);

/**
 * @brief Dealokuje pamięć zajmowaną przez @p node.
 * Dealokuje pamięć zajmowaną przez @p node. Gdy @p node jest @p NULL nic nie
 * robi.
 * @param [in] node - Wskaźnik na element listy.
 */
void freeListNode(ListNode *node);

/**
 * @brief Zwraca napis zawarty w elemencie listy.
 * Zwraca napis zawarty w elemencie listy. Gdy @p node jest @p NULL zwraca @p
 * NULL.
 * @param [in] node - wskaźnik na element listy.
 * @return Wskaźnik na napis zawarty w elemencie listy.
 */
char *getListNodeStr(ListNode *node);

/**
 * @brief Zwraca następny element listy.
 * Zwraca następny element listy. Gdy funkcja wywołana jest na ostatnim
 * elemencie to zwraca @p NULL. Gdy @p node jest @p NULL zwraca @p NULL.
 * @param [in] node - wskaźnik na element listy.
 * @return Wskaźnik na następny element listy.
 */
ListNode *nextListNode(ListNode *node);

/**
 * @brief Zwraca pierwszy element listy.
 * Zwraca pierwszy element listy. Pierwszym elementem jest strażnik, który
 * nie ma napisu. Należy po wykonaniu tej funkcji użyć funkcji @p nextListNode.
 * Gdy @p list jest @p NULL zwraca @p NULL.
 * Gdy @p lesy jest pusta zwraca @p NULL.
 * @param [in] list - wskaźnik na listę.
 * @return Wskaźnik na pierwszy element w liście.
 */
ListNode *firstListNode(CharList *list);

#endif //CHARLIST_H
