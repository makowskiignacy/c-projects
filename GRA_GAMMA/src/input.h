/** @file
 * Interfejs klasy parsującej dane, dla interfejsu tekstowego.
 *
 * @author Ignacy Makowski
 */

#ifndef GAMMA_2_INPUT_H
#define GAMMA_2_INPUT_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Lista przechowująca słowa z wczytanych wersów.
 */
struct list_string {
  ///Argument listy
  char *string;
  ///Wskażnik na kolejny element listy.
  struct list_string *next;
};
///Struktura przechowująca stringi w formie listy
typedef struct list_string list_string_t;

/**
 * @brief Lista przechowująca argumenty poleceń.
 */
struct list_uint {
  ///Argument listy
  uint32_t x;
  ///Wskaźnik na kolejny element listy.
  struct list_uint *next;
};

/**
 * @brief Struktura przechowująca wczytane polecenie.
 */
struct command {
  ///Polecenie
  char *type;
  ///Lista argumentów
  struct list_uint *arguments;
};
///Struktura służąca do przekazywania ostatecznych poleceń.
typedef struct command command_t;

/**
 * @brief Funkcja zwracająca długość listy.
 * Przechodzi listę do NULL-a
 * @param l - lista wyrazów
 * @return długość listy od 0
 */
int list_string_length(list_string_t *l);

/**
 * @brief Funkcja zwraca listę wyrazów z wczytanego wersu.
 * 1) Wczytuje wiersz funkcją getline
 * 2) Aktualizuje numer wiersza (verse)
 * 3) Sprawdza popraność wczytanego wiersza
 * 4) Dzieli wiersz na słowa funkcją strtok
 * 5) Tworzy listę ze słów
 * @param verse - aktualny numer rozpatrywanego wiersza, jest zmieniany w trakcie działania programu
 * @return listę wyrazów z kolejnej lini standardowego wejścia
 */
list_string_t *next_line(int *verse);

/**
 * @brief Zwalnia pamięć po strukturze list_string_t.
 * @param l - lista do zwolnienia
 */
void free_string_list(list_string_t *l);

/**
 * /@brief Zwalnia pamięć zaalokowaną dla listy argumentów.
 * @param l - lista do zwolnienia
 */
void free_uint_list(struct list_uint *l);

/**
 * @brief Zwalnia zaalokowaną strukturę command_t, wyłączając type.
 * @param c - struktura do zwolnienia
 */
void free_command(command_t *c);

#endif //GAMMA_2_INPUT_H
