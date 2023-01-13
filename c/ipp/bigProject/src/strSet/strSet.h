/** @file
 * Interfejs klasy przechowującej napisy string jako set.
 * @author Artur Kamieniecki
 * @date 2022
 */

#ifndef STRSET_H
#define STRSET_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Struktura przechowująca dynamiczną liczbę napisów string.
 * Każdy z napisów jest reprezentowany jako null terminated string.
 * Napisy są posortowane.
 */
struct strSetNode;
/**
 * Definicja StrSet.
 */
typedef struct strSet StrSet;

/**
 * @brief Tworzy nową strukturę.
 * Tworzy nową strukturę przechowującą napisy. Zwraca @ NULL jeżeli nie
 * udało się zaalokować pamięci.
 * @return Wskaźnik na nową zaalokowaną strukturę.
 */
StrSet *createStrSet(void);

/**
 * @brief Usuwa strukturę.
 * Dealokuje pamięć zajmowaną przez strukturę. Funkcja nic nie robi jeżeli
 * @p set jest @p NULL.
 * @param[in] set - wskaźnik na strukturę;
 */
void freeStrSet(StrSet *set);

/**
 * @brief Dodaje napis do struktury.
 * Dodaje napis do struktury. Napis musi być reprezentowany jako null terminated
 * string. @p val oraz @p set nie mogą być wartości @p NULL. Dodawany napis jest
 * kopiowany. Ponowne dodanie napisu należącego do struktury nic nie zrobi.
 * @param[in,out] set - wskaźnik na strukturę;
 * @param[in,out] val - wskaźnik na napis reprezentowany jako null terminated
 *                      string;
 * @return Wartość @p true gdy funkcja zakończy działanie bez błędów.
 *         Wartość @p false gdy funkcja napotka jeden z błędów: brak możliwości
 *         alokacji pamięci, niepoprawna reprezentacja @p set.
 */
bool strSetAdd(StrSet *set, char const *val);

/**
 * @brief Zwraca wskaźnik do napisu.
 * Zwraca wskaźnik do napisu o odpowiednim indeksie trzymanego w strukturze.
 * Napisy w strukturze są posortowane.
 * @p set nie może mieć wartości @p NULL.
 * Jeżeli indeks jest zbyt duży zwracane jest @p NULL.
 * @param[in] set   - wskaźnik do struktury;
 * @param[in] index - indeks szukanego napisu;
 * @return Wskaźnik do napisu znajdującego się w strukturze. Jest wartości
 * @p NULL gdy wskaźnik o danym indeksie nie istnieje.
 */
char *strSetGet(StrSet *set, size_t index);

/**
 * @brief Usunięcie napisu z set.
 * Usuwa napis z set. Napis nie może mieć wartości @p null. W przypadku gdy
 * napisu nie ma w set czy @p set jest wartości @p null nic nie robi.
 * @param [in,out] set - wskaźnik na set.
 * @param [in] val - wskaźnik na napis do usunięcia.
 */
void strSetRemove(StrSet *set, char *val);

/**
 * @brief Porównanie dwóch numerów.
 * Porównanie dwóch numerów. Odpowiednie znaki są zamieniane na liczy według
 * funkcji @p charToInt. Oba napisy nie mogą być wartości @p NULL.
 * @param [in] a - napis 1.
 * @param [in] b - napis 2.
 * @return 1 gdy a < b;
 *         -1 gdy a > b;
 *         0 gdy a == b.
 */
int strCompare(char const *a, char const *b);

#endif //STRSET_H
