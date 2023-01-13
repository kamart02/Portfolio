/** @file
 * Implementacja interfejsu voidTrie.h jako drzewo trie.
 * @author Artur Kamieniecki
 * @date 2022
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "voidTrie.h"
#include "../charList/charList.h"

/**
 * Stała informująca o ilości synów w drzewie trie.
 */
#define NUMOFTRIECHILDREN 12

/**
 * Struktura node reprezentującej węzeł drzewa.
 */
struct node {
    /**
     * Wskaźnik na ojca w drzewie.
     * W przypadku korzenia, wartość jest równa null.
     */
    TrieNode *father;
    /**
     * Ilość gałęzi wychodzących które kończą się wartością.
     */
    int activeBranches;
    /**
     * Tablica 12-to elementowa przechowująca wskaźniki do kolejnych węzłów
     * drzewa trie.
     */
    TrieNode *nodes[NUMOFTRIECHILDREN];
    /**
     * Wartość logiczna reprezentująca czy jest to węzeł końcowy (reprezentuje
     * dany klucz jakąś wartością)
     */
    bool isEnd;
    /**
     * Wskaźnik na listę (w przypadku drzewa root jest to wskaźnik na elementy
     * listy zawarte końcach drzewa reverse odpowiadajcych odpowiedniemu
     * przekierowaniu, w przypadku drzewa reverse jest to wskaźnik na listę
     * zawierającą prefiksy przekierowane na dany napis).
     */
    void *list;
    /**
     * Wskaźnik na napis na które jest przekierowanie w przypadku drzewa reverse.
     * W przypadku drzewa root jest to wskaźnik na odpowiedni @p node w drzewie
     * reverse, który odpowiada za dane przekierowanie)
     */
    void *val;
};

/**
 * Definicja struktury trie z możliwością odwracania.
 */
struct trie {
    /**
     * Wskaźnik na drzewo trie.
     */
    TrieNode *root;
    /**
     * Wskaźnik na drzewo z odwrotnościami.
     */
    TrieNode *reverse;
};

/**
 * @brief Zwraca liczbę reprezentowaną przez znak.
 * Zwraca liczbę reprezentowaną przez znak. Tylko znaki '0' - '9', '*', '#' są
 * dozwolone. W przeciwnym wypadku zwraca -1.
 * @param letter
 * @return
 */
static int toDigit(const char letter) {
    if (letter >= '0' && letter <= '9') {
        return letter - '0';
    }
    else if (letter == '#') {
        return 10;
    }
    else if (letter == '*') {
        return 11;
    }
    else {
        return -1;
    }

}

/**
 * @brief Funkcja tworząca węzeł.
 * Funkcja tworzy węzeł drzewa. Zwraca @p NULL w przypadku braku pamięci.
 * @return Wskaźnik do nowego węzła.
 */
static TrieNode *createNode(TrieNode* father) {
    TrieNode *node = calloc(1, sizeof(TrieNode));
    if (node != NULL) {
        node->isEnd = false;
        node->list = NULL;
        node->val = NULL;
        node->father = father;
        node->activeBranches = 0;
    }

    return node;
}

/**
 * @brief Znalezienie węzła bez liścia.
 * Znalezienie węzła bez dziecka na pozycji zerowej wychodzącej z node. Dziecko
 * znajduje się najbardziej na lewo, czyli zawsze wychodzi z gałęzi zerowej.
 * @p node nie może być @p NULL.
 * @param[in] node wskaźnik do węzła
 * @return Wskaźnik do węzła który jest synem (lub tym samym węzłem) węzła @p
 *         node, nie wychodzi gałąź z pozycji zerowej.
 */
static TrieNode *findLeftLeaf(TrieNode *node) {
    TrieNode *current = node;
    while (current->nodes[0] != NULL) {
        current = current->nodes[0];
    }

    return current;
}

/**
 * @brief Znajduje początek gałęzi.
 * Znajduje początek gałęzi w której zawiera się @p node. Koniec gałęzi
 * następuje w miejscu w którym węzeł rozdziela się na dwa lub więcej lub
 * gdy @p isEnd jest @p true.
 * @p node nie może być wartości @p NULL.
 * @param [in] node - wskaźnik na węzeł.
 * @return Wskaźnik na początek gałęzi.
 */
static TrieNode **findBranchBeginning(TrieNode *node) {
    TrieNode *current = node;
    TrieNode *next = node->father;

    while (next != NULL) {
        if (next->isEnd || next->activeBranches > 1 || next->father == NULL) {
            for (int i = 0; i < NUMOFTRIECHILDREN; i++) {
                if (next->nodes[i] == current) {
                    return &(next->nodes[i]);
                }
            }
            return NULL;
        }
        else {
            current = next;
            next = current->father;
        }
    }

    return NULL;
}

/**
 * @brief Usuwa węzeł i jego dzieci.
 * Funkcja usuwa iteracyjnie węzeł wraz z jego wszystkimi dziećmi.
 * @param[in] node - wskaźnik na węzeł do usunięcia.
 * @param[in] isReverse - wartośc bool informująca o typie drzewa przetwarzanego.
 *                      @p false dla drzewa root
 *                      @p true dla drzewa reverse
 */
static void freeNode(TrieNode *node, bool isReverse) {
    TrieNode *leftLeaf = node;
    TrieNode *current = node;
    TrieNode *next;
    // Algorytm przekształca drzewo w taki sposób aby każda z gałęźi nie
    // wychodzącej z pozycji zerowej została podpięta pod najbardziej lewy
    // węzeł nie posiadającejcy dziecka w gałęzi o pozycji zerowej.
    while (current != NULL) {
        for (int i = 1; i < NUMOFTRIECHILDREN; i++) {
            if (current->nodes[i] != NULL) {
                // Znajdowanie najbardziej lewego węzła które nie ma węzła na
                // pozycji zerowej.
                if (leftLeaf != NULL) {
                    leftLeaf = findLeftLeaf(leftLeaf);
                }
                else {
                    leftLeaf = findLeftLeaf(current);
                }
                leftLeaf->nodes[0] = current->nodes[i];
                current->nodes[i] = NULL;
            }
        }
        if (!isReverse) {
            removeListNode(current->list);
            current->list = NULL;
            TrieNode *reverseNode = (TrieNode*) current->val;
            if (reverseNode != NULL) {
                if (isListEmpty(reverseNode->list)) {
                    if (reverseNode->activeBranches == 0) {
                        TrieNode **branchBeginning = findBranchBeginning(reverseNode);
                        (*branchBeginning)->father->activeBranches--;
                        freeNode(*branchBeginning, true);
                        *branchBeginning = NULL;
                    }
                    else {
                        freeCharList(reverseNode->list);
                        reverseNode->list = NULL;
                        free(reverseNode->val);
                        reverseNode->val = NULL;
                        reverseNode->isEnd = false;
                    }
                }
            }
        }
        else {
            freeCharList(current->list);
            current->list = NULL;
            free(current->val);
            current->val = NULL;
        }
        next = current->nodes[0];
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

Trie *createTrie() {
    Trie *trie = malloc(sizeof(Trie));
    TrieNode *root = createNode(NULL);
    TrieNode *reverse = createNode(NULL);

    if (trie != NULL && root != NULL && reverse != NULL) {
        trie->root = root;
        trie->reverse = reverse;

        return trie;
    }
    else {
        if (trie != NULL) {
            free(trie);
        }
        if (root != NULL) {
            free(root);
        }
        if (reverse != NULL) {
            free(reverse);
        }

        return NULL;
    }
}

void freeTrie(Trie *trie) {
    if (trie != NULL) {
        freeNode(trie->root, false);
        freeNode(trie->reverse, true);
    }
    free(trie);
}

/**
 * @brief Funkcja pomocnicza do funkcji @p trieAdd.
 * Funkcja pomocnicza do funkcji @ref trieAdd. Ma podobną sygnaturę. Jedyna
 * różnica to @p isReverse który reprezentuje czy funkcja zajmuje się drzewem
 * root czy reverse.
 *  * @param[in,out] trie - wskaźnik na strukturę przetrzymującą mapowanie;
 * @param[in] key     - wskaźnik na klucz reprezentowany jako null terminated
 *                      string;
 * @param[in] value   - wskaźnik na wartościowanie klucza reprezentowane jako
 *                      null terminated string;
 * @param[in] isReverse - bool oznajmiający typ drzewa.
 * @return Wskaźnik na ostatni węzeł dodany do drzewa.
 *         Wartość @p NULL jeżeli nastąpił błąd pamięci.
 */
static void *trieAddHelper(Trie *trie, char const *key, char const *value, bool isReverse) {
    if (trie == NULL) {
        return NULL;
    }
    TrieNode *currentNode = NULL;
    TrieNode **currentNodeAddr = NULL;
    if (!isReverse) {
        currentNode = trie->root;
    }
    else {
        currentNode = trie->reverse;
    }
    char *keyCpy = (char*) key;

    // Przejście po całym kluczu aż do momentu dojścia do węzła który go
    // reprezentuje.
    while (*keyCpy) {
        // W przypadku braku dziecka jest ono tworzone.
        if (currentNode->nodes[toDigit(*keyCpy)] == NULL) {
            currentNode->nodes[toDigit(*keyCpy)] = createNode(currentNode);
            if (currentNode->nodes[toDigit(*keyCpy)] == NULL) {
                return NULL;
            }
            else {
                currentNode->activeBranches++;
            }
        }

        currentNodeAddr = &currentNode->nodes[toDigit(*keyCpy)];
        currentNode = currentNode->nodes[toDigit(*keyCpy)];
        keyCpy++;
    }
    if (!isReverse) {
        ListNode *listNode = createListNode(key);
        if (listNode == NULL) {
            return NULL;
        }

        if (currentNode->isEnd) {
            removeListNode(currentNode->list);
        }

        currentNode->list = listNode;
        currentNode->isEnd = true;

        currentNode->val = trieAddHelper(trie, value, key, true);
        if (currentNode->val == NULL) {
            freeNode(currentNode, false);
            *currentNodeAddr = NULL;
            return NULL;
        }

        addCharList(((TrieNode*) currentNode->val)->list, listNode);

    }
    else {
        if (!currentNode->isEnd) {
            currentNode->list = createCharList();
            if (currentNode->list == NULL) {
                return NULL;
            }
            currentNode->val = realloc(currentNode->val, strlen(key) + 1);
            if (currentNode->val == NULL) {
                freeNode(currentNode, true);
                *currentNodeAddr = NULL;
                return NULL;
            }
            strcpy(currentNode->val, key);
            currentNode->isEnd = true;
        }
    }


    return currentNode;
}

bool trieAdd(Trie *trie, char const *key, char const *value) {
    return trieAddHelper(trie, key, value, false) != NULL;
}

char *trieFindPrefix(Trie *trie, char const *key, size_t *prefLen) {
    TrieNode *currentNode = trie->root;
    char *value = NULL;
    char *keyCpy = (char*) key;
    size_t len = 0;

    // Przejście po kluczu aż do znalezienia odpowiadającemu mu węzła oraz
    // zapamiętanie węzłów końcowych które reprezentują prefiks klucza.
    while (*keyCpy && currentNode != NULL) {
        if (currentNode->isEnd) {
            value = ((TrieNode*) currentNode->val)->val;
            *prefLen = len;
        }
        currentNode = currentNode->nodes[toDigit(*keyCpy)];

        len++;
        keyCpy++;
    }

    if (*keyCpy == '\0' && currentNode != NULL) {
        if (currentNode->isEnd) {
            value = ((TrieNode*) currentNode->val)->val;
            *prefLen = len;
        }
    }

    return value;
}

void trieRemovePrefix(Trie *trie, char const *key) {
    char *keyCpy = (char*) key;

    TrieNode *currentNode = trie->root->nodes[toDigit(*keyCpy)];

    TrieNode *branchBeginning = currentNode;
    TrieNode **branchBeginningAddr = &trie->root->nodes[toDigit(*keyCpy)];

    keyCpy++;

    // Znalezienie węzła odpowiadającemu kluczowi i usunięcie go wraz z dziećmi.
    while (*keyCpy && currentNode != NULL) {
        // Sprawdzenie szukające początku gałęzi.
        if (currentNode->activeBranches > 1 || currentNode->isEnd) {
            branchBeginning = currentNode->nodes[toDigit(*keyCpy)];
            branchBeginningAddr = &currentNode->nodes[toDigit(*keyCpy)];
        }
        currentNode = currentNode->nodes[toDigit(*keyCpy)];
        keyCpy++;
    }

    // Węzły są usuwane tylko gdy dane przekierowanie istnieje.
    if (branchBeginning != NULL && currentNode != NULL) {
        if (branchBeginning->father != NULL) {
            branchBeginning->father->activeBranches--;
            freeNode(branchBeginning, false);
            *branchBeginningAddr = NULL;
        }
    }
}

StrSet *trieReversePrefix(Trie *trie, char const *key) {
    char *keyCpy = (char*) key;

    TrieNode *currentNode = trie->reverse->nodes[toDigit(*keyCpy)];
    keyCpy++;
    ListNode *listNode = NULL;

    size_t prefixLen = 1;
    size_t keyLen = strlen(key);
    size_t newStringLen;

    StrSet *set = createStrSet();
    if (set == NULL) {
        return NULL;
    }

    char *newString = NULL;
    char *beginString = NULL;

    while (currentNode != NULL) {
        if (currentNode->isEnd) {
            listNode = firstListNode(currentNode->list);

            while ((listNode = nextListNode(listNode)) != NULL) {
                beginString = getListNodeStr(listNode);
                newStringLen = strlen(beginString);
                newStringLen += keyLen - prefixLen;
                newStringLen++;
                newString = malloc(sizeof(char) * newStringLen);

                if (newString == NULL) {
                    freeStrSet(set);
                    return NULL;
                }

                strcpy(newString, beginString);
                strcpy(newString + strlen(beginString), key + prefixLen);

                if (strSetAdd(set, newString) == false) {
                    freeStrSet(set);
                    free(newString);
                    return NULL;
                }

                free(newString);
            }
        }
        currentNode = currentNode->nodes[toDigit(*keyCpy)];
        prefixLen++;
        keyCpy++;
    }

    return set;
}