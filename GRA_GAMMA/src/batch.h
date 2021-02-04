/** @file
 * Interfejs klasy obsługującej wsadowy tryb gry.
 *
 * @author Ignacy Makowski
 */

#ifndef GAMMA_2_BATCH_H
#define GAMMA_2_BATCH_H

#include "gamma.h"
#include <stdlib.h>
#include <string.h>

 /**
  * @brief Funkcja odpowiada za obsługę trybu wstadowego.
  * Błędne polecenia powodują zwrócenie ERROR numer wiersza na stderr.
  * Po poprawnym poleceniu zwracany jest wynik odpowiedającej mu funkcji z gamma.h
  * @param gamma - aktualny stan gry gamma w danej strukturze
  * @param verse - wskaźnik na numer aktualnej lini wejścia
  */
void batch_mode(gamma_t *gamma, int *verse);

#endif //GAMMA_2_BATCH_H
