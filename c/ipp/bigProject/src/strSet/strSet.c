/** @file
 * Implementacja interfejsu strSetNode.h za pomocą drzewa BST.
 * @author Artur Kamieniecki
 * @date 2022
 */

#include <stdlib.h>
#include <string.h>
#include "strSet.h"

/**
 * Definicja węzłu struktury.
 */
typedef struct strSetNode StrSetNode;

/**
 * Struktura przechowująca dynamiczną liczbę napisów string.
 * Każdy z napisów jest reprezentowany jako null terminated string.
 */
struct strSetNode {
    /**
     * Wskaźnik na ojca węzła. Ma wartość @p NULL jeśli jest korzeniem.
     */
    StrSetNode *father;
    /**
     * Aktualna liczba napisów trzymanych w lewym poddrzewie.
     */
    size_t sizeLeft;
    /**
     * Aktualna liczba napisów trzymanych w prawym poddrzewie.
     */
    size_t sizeRight;
    /**
     * Wskaźnik do lewego syna.
     */
    StrSetNode *left;
    /**
    * Wskaźnik do prawego syna.
    */
    StrSetNode *right;
    /**
     * Wskaźnik na napis przechowywany w węźle.
     */
    char *str;
};

/**
 * Definicja struktury set.
 */
struct strSet {
    /**
     * Wskaźnik na korzeń drzewa.
     */
    StrSetNode *root;
};

StrSet *createStrSet() {
    StrSet *strSet = malloc(sizeof(StrSet));
    if (strSet != NULL) {
        strSet->root = NULL;
    }

    return strSet;
}

/**
 * @brief Znalezienie węzła bez liścia.
 * Znalezienie węzła bez dziecka na pozycji lewej wychodzącej z node.
 * @p node nie może być @p NULL.
 * @param[in] node wskaźnik do węzła
 * @return Wskaźnik do węzła który jest synem (lub tym samym węzłem) węzła @p
 *         node, nie wychodzi gałąź z pozycji lewej.
 */
static StrSetNode *findLeftLeaf(StrSetNode *node) {
    StrSetNode *current = node;
    while (current->left != NULL) {
        current = current->left;
    }

    return current;
}

/**
 * @brief Zwolnienie pamięci zajmowanej przez node i jego poddrzewa.
 * Zwolnienie pamięci zajmowanej przez node i jego poddrzewa. @p node nie może
 * być wartości @p NULL.
 * @param[in] node - wskaźnik na węzeł.
 */
static void freeNode(StrSetNode *node) {
    StrSetNode *leftLeaf = node;
    StrSetNode *current = node;
    StrSetNode *next;
    // Algorytm przekształca drzewo w taki sposób aby każda z gałęźi nie
    // wychodzącej z pozycji lewej została podpięta pod najbardziej lewy
    // węzeł nie posiadającejcy dziecka w lewej gałęzi.
    while (current != NULL) {
        if (current->right != NULL) {
            // Znajdowanie najbardziej lewego węzła które nie ma węzła na
            // pozycji zerowej.
            if (leftLeaf != NULL) {
                leftLeaf = findLeftLeaf(leftLeaf);
            }
            else {
                leftLeaf = findLeftLeaf(current);
            }
            leftLeaf->left = current->right;
            current->right = NULL;
        }
        free(current->str);
        next = current->left;
        // W przypadku gdy aktualny węzeł nie posiada syna w pozycji zerowej
        // to jest swoim własnym najbardziej lewym węzłem i należy go zresetować
        // ponieważ jest on usuwany.
        if (leftLeaf == current) {
            leftLeaf = NULL;
        }
        free(current);
        current = next;
    }
}

void freeStrSet(StrSet *set) {
    if (set == NULL) {
        return;
    }

    freeNode(set->root);
    free(set);
}

/**
 * @brief Zamienia @p char na cyfre
 * Zamienia @p char na cyfrę. Napisy od '0' do '9' zamieniane są odpowiednio na
 * 0 do 9. Napisy '*' i '#' zamieniane są odpowiednio na odpowowiednio 10 i 11.
 * Znak '\0' zamieniany jest na -1.
 * Inne znaki nie są obsługowane przez funkcję.
 * @param [in] chr - znak reprezentujący cyfrę.
 * @return Wartość @p int reprezentowaną przez znak.
 */
static int charToInt(char chr) {
    if (chr == '*') {
        return 10;
    }
    else if (chr == '#') {
        return 11;
    }
    else if (chr == '\0') {
        return -1;
    }
    else {
        return chr - '0';
    }
}

int strCompare(char const *a, char const *b) {
    int comparedValueA;
    int comparedValueB;

    char *aCpy = (char*) a;
    char *bCpy = (char*) b;

    while (true) {
        comparedValueA = charToInt(*aCpy);
        comparedValueB = charToInt(*bCpy);

        if (comparedValueA < comparedValueB) {
            return 1;
        }
        else if (comparedValueA > comparedValueB) {
            return -1;
        }
        else if (comparedValueA == -1 && comparedValueB == -1) {
            return 0;
        }
        aCpy++;
        bCpy++;
    }
}

/**
 * @brief Tworzy nowy węzeł.
 * Tworzy nowy węzeł o wartości @p str i ojcu @p father. @p str jest kopiowane.
 * @p str musi być napisem reprezentowanym za pomocą null terminated string.
 * Zwraca @p NULL w przypadku błędu alokacji pamięci.
 * @param [in] str - wskaźnik na napis.
 * @param [in] father - wskaźnik na ojca węzła.
 * @return Wskaźnik na nowy węzeł.
 */
static StrSetNode *createStrSetNode(char *str, StrSetNode *father) {
    StrSetNode *node = calloc(1, sizeof(StrSetNode));

    if (node != NULL) {
        char *strCpy = (char*) str;
        node->str = malloc(strlen(strCpy) + 1);
        if (node->str == NULL) {
            free(node);
            return NULL;
        }

        strcpy(node->str, strCpy);
        node->father = father;

        return node;
    }

    return NULL;
}

bool strSetAdd(StrSet *set, char const *val) {
    if (set == NULL) {
        return false;
    }

    char* valCpy = (char*) val;

    if (set->root == NULL) {
        set->root = createStrSetNode(valCpy, NULL);
        if (set->root == NULL) {
            return false;
        }
        return true;
    }

    StrSetNode *current = set->root;

    while (true) {
        int cmp = strCompare(current->str, valCpy);
        if (cmp < 0) {
            if (current->left != NULL) {
                current = current->left;
            }
            else {
                current->left = createStrSetNode(valCpy, current);
                if (current->left == NULL) {
                    return false;
                }
                break;
            }
        }
        else if (cmp > 0) {
            if (current->right != NULL) {
                current = current->right;
            }
            else {
                current->right = createStrSetNode(valCpy, current);
                if (current->right == NULL) {
                    return false;
                }
                break;
            }
        }
        else {
            return true;
        }
    }

    size_t newSize;

    while (current != NULL) {
        newSize = 0;
        if (current->left != NULL) {
            newSize = current->left->sizeLeft + current->left->sizeRight + 1;
        }
        current->sizeLeft = newSize;
        newSize = 0;
        if (current->right != NULL) {
            newSize = current->right->sizeLeft + current->right->sizeRight + 1;
        }
        current->sizeRight = newSize;
        current = current->father;
    }

    return true;

}

char *strSetGet(StrSet *set, size_t index) {
    if (set->root == NULL) {
        return NULL;
    }

    StrSetNode *current = set->root;

    // Odpowiednie sprawdzanie ile elementów jest w lewym i prawym poddrzewie
    // pozwala na wyznaczenie elementu za pomocą indeksu.
    while (current != NULL) {
        if (current->sizeLeft == index) {
            return current->str;
        }
        else if (current->sizeLeft  > index) {
            current = current->left;
        }
        else {
            index -= current->sizeLeft + 1;
            current = current->right;
        }

    }

    return NULL;
}

/**
 * @brief Aktualizuje wielkości poddrzew w secie.
 * Aktualizuje wielkości poddrzew w secie.
 * @param [in,out] node - wskaźnik na node seta.
 */
static void updateSizes(StrSetNode *node) {
    if (node == NULL) {
        return;
    }


    while (node != NULL) {
        if (node->left == NULL) {
            node->sizeLeft = 0;
        } else {
            node->sizeLeft = node->left->sizeLeft + node->left->sizeRight + 1;
        }

        if (node->right == NULL) {
            node->sizeRight = 0;
        }
        else {
            node->sizeRight = node->right->sizeLeft + node->right->sizeRight + 1;
        }

        node = node->father;
    }
}

/**
 * @brief Dealokuje pamięć zajmowaną przez pojedyńczy node.
 * Dealokuje pamięć zajmowaną przez pojedyńczy node.
 * @param [in] node - wskaźnik na node seta. Nie może mieć wartości null.
 */
static void freeSingleNode(StrSetNode *node) {
    free(node->str);

    free(node);
}

/**
 * @brief Zmienia pozycję node w drzewie bst.
 * Zmienia pozycję node w drzewie bst. Funkcja służy do usuwanie node z drzewa.
 * W taki sposób aby drzewo dalej był bst.
 * @p set oraz @p toSwitch nie mogą mieć wartości null.
 * @param [in, out] set - wskaźnik na set.
 * @param [in] toSwitch - wskaźnik na wierzchołek za którego będzie podstawiony
 *                        inny.
 * @param [in, out] replacement - wskaźnik na wierzchołek którego pozycję
 *                                zmieniamy.
 */
static void switchPositions(StrSet *set, StrSetNode *toSwitch, StrSetNode *replacement) {
    if (toSwitch->father == NULL) {
        set->root = replacement;
    }
    else if (toSwitch == toSwitch->father->left) {
        toSwitch->father->left = replacement;
    }
    else {
        toSwitch->father->right = replacement;
    }

    if (replacement != NULL) {
        replacement->father = toSwitch->father;
    }
}

void strSetRemove(StrSet *set, char *val) {
    if (set == NULL) {
        return;
    }
    StrSetNode *currentNode = set->root;

    int compare;
    StrSetNode *updateFrom = NULL;

    while (true) {
        if (currentNode == NULL) {
            return;
        }
        compare = strCompare(currentNode->str, val);

        if (compare > 0) {
            currentNode = currentNode->right;
        }
        else if (compare < 0) {
            currentNode = currentNode->left;
        }
        else {
            if (currentNode->left == NULL) {
                switchPositions(set, currentNode, currentNode->right);
                updateFrom = currentNode->father;
            }
            else if (currentNode->right == NULL) {
                switchPositions(set, currentNode, currentNode->left);
                updateFrom = currentNode->father;
            }
            else {
                StrSetNode *successor = findLeftLeaf(currentNode->right);
                if (successor->father != currentNode) {
                    switchPositions(set, successor, successor->right);
                    successor->right = currentNode->right;
                    successor->right->father = successor;
                    updateFrom = successor->father;
                }
                else {
                    updateFrom = successor;
                }

                switchPositions(set, currentNode, successor);
                successor->left = currentNode->left;
                successor->left->father = successor;
            }
            updateSizes(updateFrom);
            freeSingleNode(currentNode);

            return;
        }
    }
}