/** @file
 * Klasa służąca do wyboru pomiędzy trybem wsadowym a interaktywnym interfejsu.
 *
 * @author Ignacy Makowski
 */

#ifndef GAMMA_2_CHOOSE_H
#define GAMMA_2_CHOOSE_H

#include "input.h"

/**
 * @brief Funkcja uruchamia interfejs wsadowy lub interaktywny.
 * Pobira ze stanardowego wejścia polecenie w postaci:
 * B/I szerokość wysokość ilość graczy maksymalna ilość obszarów.
 * W przypadku nieprawidłowego polecenia zwraca ERROR numer wersu na stderr.
 */
void choose_mode(void);

#endif //GAMMA_2_CHOOSE_H
