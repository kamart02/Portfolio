/** @file
 * Implementacja interfejsu phone_forward.h
 * @author Artur Kamieniecki
 * @date 2022
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "phone_forward.h"
#include "voidTrie/voidTrie.h"
#include "strSet/strSet.h"

/**
 * @brief Sprawdzenie czy napis jest numerem.
 * Sprawdzenie czy dany napis jest numerem oraz jest null terminated.
 * @param[in] str - wskaźnik na potencjalny napis.
 * @return Wartość @p true jeżeli @p str reprezentuje napis.
 *         Wartość @p false jeżeli @p str nie reprezentuje napisu.
 */
static bool isNumber(char const *str) {
    // Test wartości NULL.
    if (str == NULL) {
        return false;
    }
    // Sprawdzenie czy napis nie jest pusty.
    if (*str == '\0') {
        return false;
    }

    char* strCpy = (char*) str;

    // Przejście przez napis i sprawdzenie czy każdy znak to liczba.
    while (*strCpy != '\0') {
        if (!isdigit(*strCpy) && *strCpy != '*' && *strCpy != '#') {
            return false;
        }
        strCpy++;
    }

    return true;
}

/**
 * Struktura PhoneForward przechowująca przekierowania.
 */
struct PhoneForward {
    /**
     * Wskaźnik na trie.
     */
    Trie *trie;
};

/**
 * Struktura PhoneNumbers przechowująca dynamiczna liczbę napisów.
 */
struct PhoneNumbers {
    /**
     * Wskaźnik na set numerów.
     */
    StrSet *set;
};

PhoneForward *phfwdNew() {
    PhoneForward *phfwd = malloc(sizeof(PhoneForward));
    if (phfwd == NULL) {
        return NULL;
    }

    phfwd->trie = createTrie();
    if (phfwd->trie == NULL) {
        free(phfwd);
        return NULL;
    }

    return phfwd;
}

void phfwdDelete(PhoneForward *pf) {
    if (pf != NULL) {
        freeTrie(pf->trie);
        free(pf);
    }
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if (pf != NULL) {
        if (isNumber(num1) && isNumber(num2)) {
            if (strcmp((char *) num1, num2) != 0) {
                return trieAdd(pf->trie, num1, num2);
            }
        }
    }
    return false;
}

void phfwdRemove(PhoneForward *pf, char const *num) {
    if (pf != NULL) {
        if (isNumber(num)) {
            trieRemovePrefix(pf->trie, num);
        }
    }
}

char const *phnumGet(PhoneNumbers const *pnum, size_t idx) {
    if (pnum == NULL) {
        return NULL;
    }

    return strSetGet(pnum->set, idx);
}

void phnumDelete(PhoneNumbers *pnum) {
    if (pnum != NULL) {
        freeStrSet(pnum->set);
        free(pnum);
    }
}

PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num) {
    if (pf == NULL) {
        return NULL;
    }

    // Sprawdzenie poprawności alokacji.
    PhoneNumbers *phnum = malloc(sizeof(PhoneNumbers));
    if (phnum == NULL) {
        return NULL;
    }

    phnum->set = createStrSet();
    if (phnum->set == NULL) {
        free(phnum);
        return NULL;
    }

    // Gdy num nie jest numerem zwracana jest pusta struktura.
    if (!isNumber(num)) {
        return phnum;
    }
    // Długość prefiksu który jest zastępowany w danym numerze.
    size_t numPrefLen = 0;
    char *prefix = trieFindPrefix(pf->trie, num, &numPrefLen);
    if (prefix == NULL) {
        strSetAdd(phnum->set, num);
        return phnum;
    }

    // Długość nowego prefiksu
    size_t prefixLen = strlen(prefix);
    // Nowa długość całego napisu.
    size_t newLen = strlen(num) - numPrefLen + prefixLen + 1;
    char *newNum = malloc(newLen * sizeof(char));
    if (newNum == NULL) {
        freeStrSet(phnum->set);
        free(phnum);
        return NULL;
    }

    strcpy(newNum, prefix);
    strcpy(newNum + prefixLen, num + numPrefLen);

    if (strSetAdd(phnum->set, newNum)) {
        free(newNum);
        return phnum;
    }
    else {
        free(newNum);
        freeStrSet(phnum->set);
        free(phnum);
        return NULL;
    }

}

/**
 * @brief Wyznacza przekierowania na dany numer.
 * Wyznacza przekierowania na dany numer. Działanie analogiczne jak funkcja
 * @p phfwdReverse, ale zamiast zwracać PhoneNumbers zwraca strSet
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na set przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
static StrSet *phfwdReverseSet(PhoneForward const *pf, char const *num) {
    if (pf == NULL) {
        return NULL;
    }

    StrSet *set;

    if (!isNumber(num)) {
        set = createStrSet();
        if (set == NULL) {
            return NULL;
        }

        return set;
    }

    set = trieReversePrefix(pf->trie, num);
    if (set == NULL) {
        return NULL;
    }

    if (strSetAdd(set, num) == false) {
        freeStrSet(set);
        return NULL;
    }

    return set;
}

PhoneNumbers *phfwdReverse(PhoneForward const *pf, char const *num) {
    PhoneNumbers *numbers = malloc(sizeof(PhoneNumbers));
    if (numbers == NULL) {
        return NULL;
    }

    StrSet *set = phfwdReverseSet(pf, num);

    if (!isNumber(num)) {
        numbers->set = set;
        if (numbers->set == NULL) {
            free(numbers);
            return NULL;
        }

        return numbers;
    }

    numbers->set = set;
    if (numbers->set == NULL) {
        free(numbers);
        return NULL;
    }

    return numbers;
}

PhoneNumbers * phfwdGetReverse(PhoneForward const *pf, char const *num) {
    if (pf == NULL) {
        return NULL;
    }

    StrSet *reverseNumbers = NULL;
    PhoneNumbers *get = NULL;
    reverseNumbers = phfwdReverseSet(pf, num);

    if (reverseNumbers == NULL) {
        return NULL;
    }

    char *number = NULL;
    size_t i = 0;

    while ((number = (char *) strSetGet(reverseNumbers, i)) != NULL) {
        get = phfwdGet(pf, number);
        if (get == NULL) {
            freeStrSet(reverseNumbers);
            return NULL;
        }
        else {
            if (strCompare(phnumGet(get, 0), num) != 0) {
                strSetRemove(reverseNumbers, number);
                i--;
            }
            phnumDelete(get);
        }

        i++;
    }

    PhoneNumbers *numbers = malloc(sizeof(PhoneNumbers));
    if (numbers == NULL) {
        freeStrSet(reverseNumbers);
        return NULL;
    }

    numbers->set = reverseNumbers;

    return numbers;
}