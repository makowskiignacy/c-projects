/** @file
 * Interfejs klasy obsługującej interaktywny tryb gry.
 *
 * @author Ignacy Makowski
 */

#ifndef GAMMA_2_INTERACTIVE_H
#define GAMMA_2_INTERACTIVE_H

#include "gamma.h"

/**
 * @brief Funkcja odpowiada za obsługę trybu interaktywnego.
 *
 * Obsługuje sterowanie pozycją kursora przy użyciu strzałek,
 * polecenia zajęcia pola i złotego ruchu uruchamiane poleceniem ' ', 'g', 'G',
 * zatrzymanie programu wykorzystując control D.
 */

/**
 * @brief Funkcja odpowiada za obsługę trybu interaktywnego.
 * Obsługuje sterowanie pozycją kursora przy użyciu strzałek,
 * polecenia zajęcia pola i złotego ruchu uruchamiane poleceniem ' ', 'g', 'G',
 * zatrzymanie programu wykorzystując control D.
 * @param gamma - struktura przechowująca stan gry gamma
 * @param players - liczba graczy w trybie interaktywnym
 * @param x_max - makszymalna szerokość planszy
 * @param y_max - maksymalna wysokość planszy
 */
void interactive_mode(gamma_t *gamma, uint32_t players, uint32_t x_max, uint32_t y_max);

#endif //GAMMA_2_INTERACTIVE_H
