/** @file
 * Interfejs klasy przechowującej klucze i wartości napisów string.
 * @author Artur Kamieniecki
 * @date 2022
 */

#ifndef VOIDTRIE_H
#define VOIDTRIE_H

#include <stdbool.h>
#include <stddef.h>
#include "../strSet/strSet.h"

/**
 * Definicja obiektu mapy jako drzewo trie.
 */
typedef struct trie Trie;
/**
 * Definicja węzła drzewa trie.
 */
typedef struct node TrieNode;

/** @brief Tworzy nową strukturę.
 * Tworzy nową strukturę niezawierającą żadnych wartości i kluczy.
 * Zawiera zarówno drzewo mapujące oraz drzewo odwrotne.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 *         alokować pamięci.
 */
Trie *createTrie(void);

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p trie. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 * @param[in] trie – wskaźnik na usuwaną strukturę.
 */
void freeTrie(Trie *trie);

/** @brief Dodaje klucz i wartościowanie do struktury.
 * Dodaje klucz i jego wartościowanie do struktury @p trie. Zarówno @p key jak i
 * @p value nie mogą mieć wartości NULL oraz muszą być poprawną reprezentacją
 * napisu jako null terminated string. Przypisanie wartości kluczowi
 * istniejącemu w strukturze nadpisuje jego wartość.
 * W przypadku gdy @p trie jest @p NULL funkcja nic nie robi.
 * Dodawany klucz i wartość są kopiowane.
 * @param[in,out] trie - wskaźnik na strukturę przetrzymującą mapowanie;
 * @param[in] key     - wskaźnik na klucz reprezentowany jako null terminated
 *                      string;
 * @param[in] value   - wskaźnik na wartościowanie klucza reprezentowane jako
 *                      null terminated string;
 * @return Wartość @p true jeżeli funkcja zakończyła się sukcesem i dane
 *         mapowanie zostało dodane do struktury.
 *         Wartość @p false jeżeli nastąpił błąd pamięci.
 */
bool trieAdd(Trie *trie, char const *key, char const *value);

/**
 * @brief Znajduje wartość przypisaną do prefiksu klucza.
 * Znajduje wartość przypisaną do najdłuższego prefiksu klucza @p key.
 * Gdy @p trie jest równe @p NULL funkcja zwraca @p NULL.
 * @p prefLen reprezentuje długość znalezionego prefiksu.
 * @param[in] trie          - wskaźnik na strukturę zawierającą wartościowanie;
 * @param[in] key          - wskaźnik na klucz reprezentowany jako null
 *                           terminated string;
 * @param[in, out] prefLen - wskaźnik na liczbę typu @p size_t reprezentującą
 *                           długość znalezionego prefiksu;
 * @return Wskaźnik do napisu który jest przypisaniu do najdłuższego prefiksu
 *         klucza. W przypadku nie istnienia klucza w strukturze zwracana jest
 *         wartość @p NULL.
 */
char *trieFindPrefix(Trie *trie, char const *key, size_t *prefLen);

/**
 * @brief Usuwa wartościowania o danym prefiksie.
 * Usuwa wszystkie wartościowania i klucze ze struktury @p trie, których klucze
 * posiadają prefiks @p key.
 * @param[in,out] trie - wskaźnik na strukturę zawierającą wartościowanie;
 * @param[in] key     - wskaźnik na napis reprezentujący prefiks. Wskaźnik musi
 *                      reprezentować null terminated string;
 */
void trieRemovePrefix(Trie *trie, char const *key);

/**
 * @brief Znajduje wartości odwrotne do klucza.
 * Znajduje wartości odwrotne do do wartościowania w drzewie.
 * Jeżeli jakiś prefiks napisu został przekierowany i napis zaczynający
 * się przekierowaniem prefiksu i kończący resztą napisu jest równy @p key
 * to wtedy będzie on należał do @p StrSet zwracanego przez funkcję.
 * Numer key nie będzie należał do @p StrSet.
 * @param [in] trie - wskaźnik na strukturę drzewa. Nie może być wartośći NULL.
 * @param [in] key - wskaźnik na klucz którego odwrotności mają być znalezione.
 *                   Nie może być wartości NULL.
 * @return Wskaźnik na strukturę @p StrSet zawierającą odpowiednie numery.
 */
StrSet *trieReversePrefix(Trie *trie, char const *key);

#endif //VOIDTRIE_H

