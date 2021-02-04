/** @file
 * Implementacja interaktywnego trybu gry.
 *
 * @author Ignacy Makowski
 */

#include "interactive.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

///Czyści terminal
#define CLEAN_SCREEN "\e[2J"
///Czyści wiersz
#define EL "\e[2K"
///Przesuwa kursor do góry
#define CUU "\e[A"
///Przesuwa kursor w dół
#define CUD "\e[B"
///Przesuwa kursor w prawo
#define CUF "\e[C"
///Przesuwa kursor w lewo
#define CUB "\e[D"
///Standardowy początek kodu
#define CSI "\e["
///Ustawia kursor na początku poprzedniego wiersza
#define CPL "\e[F"
///Przesuwa kursor na pozycję (1, 1)
#define CUP "\e[;H"

/**
 * @brief Sprawdza czy ruch po strzałce nie wychodzi poza planszę.
 * @param vector - zmiana współrzędnej
 * @param a - aktualna wartość współrzędnej
 * @param a_max - maksymalna wartość współrzędnej
 * @return prawdę, gdy współrzędna po zmianie mieści się w zakresie, inaczej fałsz
 */
bool possible_move(int vector, uint32_t a, uint32_t a_max) {
  if (vector > 0) {
    return a < (a_max - (uint32_t) vector);
  } else if (vector == 0) {
    return true;
  } else {
    return (uint32_t) (-vector) <= a;
  }
}

/**
 * Funkcja wypisuje planszę do gry gamma na standardowe wyjście
 * Podkreśla pola należące do wskazanego gracza
 * @param g - struktura przechowująca aktualny stan gry
 * @param player - wskazany gracz (docelowo bieżący)
 */
void print_board(gamma_t *g, uint32_t player) {
  fprintf(stdout, CLEAN_SCREEN);
  fprintf(stdout, CUP);
  char *board = gamma_board(g);
  uint64_t i = 0;
  while (board[i] != '\0') {
    if ((uint64_t) board[i] == player + '0') {
      fprintf(stdout, "\e[7m");
      fprintf(stdout, "%c", board[i]);
      fprintf(stdout, "\e[27m");
    } else {
      fprintf(stdout, "%c", board[i]);
    }
    ++i;
  }
  free(board);
}

/**
 * @brief Wypisuje na standardowe wyjście planszę ze zmienionym polem na aktualnego gracza, podświetla posiadane pola
 * Podkreśla pola należące do wskazanego gracza
 * @param g - struktura przechowująca aktualny stan gry
 * @param player - wskazany gracz (docelowo bieżący)
 * @param x - pierwsza współrzędna do zmiany właściciela
 * @param y - druga współrzędna do zmiany właściciela
 * @param x_max - maksymalna wartość współrzędnej
 * @param y_max - maksymalna wartość współrzędnej
 */
void print_edited_board(gamma_t *g, uint32_t player, uint32_t *x, uint32_t *y, uint32_t x_max, uint32_t y_max) {
  fprintf(stdout, CUP);
  for (size_t i = 0; i < y_max; i++) {
    fprintf(stdout, EL);
  }
  char *board = gamma_board(g);
  board[(y_max - 1 - (*y)) * (x_max + 1) + (*x)] = '0' + player;
  uint64_t i = 0;
  while (board[i] != '\0') {
    if ((uint64_t) board[i] == player +'0') {
      fprintf(stdout, "\e[7m");
      fprintf(stdout, "%c", board[i]);
      fprintf(stdout, "\e[27m");
    } else {
      fprintf(stdout, "%c", board[i]);
    }
    ++i;
  }
  //fprintf(stdout, "%s", board);
  fprintf(stdout, CPL);
  if ((*x) > 0) {
    fprintf(stdout, "\e[%uC", (*x));
  }
  if ((*y) > 0) {
    fprintf(stdout, "\e[%uA", (*y));
  }
  free(board);
}

/**
 * Wypisuje przeunięcie kursora w zależności od zakończenia escape codu CSI i aktualizuje zmienne przechowujące pozycję
 * @param c - zakończenie kodu
 * @param x - wskażnik na współrzędną x (do zmiany)
 * @param y - wskażnik na współrzędną y (do zmiany)
 * @param x_max - maksymalna wartość danej współrzędnej
 * @param y_max - maksymalna wartość danej współrzędnej
 */
static void move_cursor(int c, uint32_t *x, uint32_t *y, uint32_t x_max, uint32_t y_max) {
  switch (c) {
    case 'A':
      if (possible_move(1, (*y), y_max)) {
        (*y)++;
        fprintf(stdout, CUU);
      }
      break;
    case 'B':
      if (possible_move(-1, (*y), y_max)) {
        (*y)--;
        fprintf(stdout, CUD);
      }
      break;
    case 'C':
      if (possible_move(1, (*x), x_max)) {
        (*x)++;
        fprintf(stdout, CUF);
      }
      break;
    case 'D':
      if (possible_move(-1, (*x), x_max)) {
        (*x)--;
        fprintf(stdout, CUB);
      }
      break;
  }
}

/**
 * @brief Pobiera jednoznakową komendę (g/G/ /koniec pliku), oraz obsługuje posruszanie po planszy.
 * @param last_command - wskaźnik na poprzednio wczytaną komendę, zostanie zwrócona w pierwszej kolejności
 * @param player - numer gracza rozgrywającego turę
 * @param x - aktualna współrzędna x
 * @param y - aktualna współrzędna y
 * @param x_max - maksymalna wartość współrzędnej
 * @param y_max - maksymalna wartość współrzędnej
 * @return znaki g/G/ /koniec pliku
 */
int get_command(int *last_command, uint32_t player, uint32_t *x, uint32_t *y, uint32_t x_max, uint32_t y_max) {
  int c = *last_command;
  int i = 0;
  while (c != VEOF && c != 'G' && c != 'g' && c != ' ' && i < 2) {
    if (c == CSI[i]) {
      i++;
    } else {
      i = 0;
    }
    c = getchar();
  }
  switch (c) {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
      move_cursor(c, x, y, x_max, y_max);
      break;
    case VEOF:
    case 'G':
    case 'g':
    case ' ':
      return c;
  }
  return get_command(last_command, player, x, y, x_max, y_max);
}

/**
 * @brief Rozpatruje turę pojedyńczego gracza
 * 1) Sprawdza czy gracz może wykonać ruch
 * 2) Wypisuje planszę, gracza, wolne i zajęte pola
 * 3) Pobiera komendę przy użyciu funkcji get_command
 * 4) Markuje wykonanie komendy
 * 5) Zapisuje komendę do wykonania na last_command
 * @param g - struktura przechowująca aktualny stan gry
 * @param last_command - komenda do rozpatrzenia w pierwszej kolejności, zmienna do zapisania komendy do potwierdzenia
 * @param player - gracz rozgrywający aktualnie turę
 * @param x - aktualna współrzędna
 * @param y - aktualna współrzędna
 * @param x_max - maksymalna wartość współrzędnej
 * @param y_max - maksymalna wartość współrzędnej
 * @param has_move - zmienna przechowująca prawdę, gdy którykolwiek z graczy miał ruch
 * @return fałsz, gdy napotkano znak końca danych w przeciwnym razie prawda
 */
bool turn(gamma_t *g, int *last_command, uint32_t player, uint32_t *x, uint32_t *y, uint32_t x_max, uint32_t y_max,
          bool *has_move) {
  uint64_t player_busy = gamma_busy_fields(g, player);
  uint64_t player_free = gamma_free_fields(g, player);
  bool player_golden = gamma_golden_possible(g, player);
  if (!player_golden && player_free == 0) {
    return true;
  } else {
    (*has_move) = true;
  }
  print_board(g, player);
  fprintf(stdout, "PLAYER %u %lu %lu", player, player_busy, player_free);
  if (player_golden) {
    fprintf(stdout, " G");
  }
  fprintf(stdout, CPL);
  if ((*x) > 0) {
    fprintf(stdout, "\e[%uC", (*x));
  }
  if ((*y) > 0) {
    fprintf(stdout, "\e[%uA", (*y));
  }
  int command = get_command(last_command, player, x, y, x_max, y_max);
  switch (command) {
    case VEOF:
      *last_command = '0';
      return false;
    case ' ':
      if (this_move_possible(g, player, *x, *y)) {
        *last_command = ' ';
        print_edited_board(g, player, x, y, x_max, y_max);
        return true;
      } else {
        *last_command = '0';
        return turn(g, last_command, player, x, y, x_max, y_max, has_move);
      }
    case 'g':
    case 'G':
      if (this_golden_move_possible(g, player, *x, *y)) {
        print_edited_board(g, player, x, y, x_max, y_max);
        *last_command = 'g';
        return true;
      } else {
        *last_command = '0';
        return turn(g, last_command, player, x, y, x_max, y_max, has_move);
      }
    default:
      *last_command = '0';
      return false;
  }
}

/**
 * @brief Sprawdza czy zostało wpisane w konsolę cofnięcie ruchu, jeśli nie aktualizuje planszę.
 * 1) Sprawdza czy następny wczytany znak, jest komendą inną niż cofnięcie ruch
 * 2a) Wtedy wykonuje komendę zapisaną na last_command i nadpisuje nową
 * 2b) W przeciwnym razie zwraca komendę 'c', co powoduje kontynuację tury gracza i nie wykonanie polecenia
 * @param g - struktura przechowująca aktualny stan gry
 * @param last_command - komenda do potwierdzenia i zaktualizowania planszy
 * @param player - gracz rozgrywający aktualnie turę
 * @param x - aktualna współrzędna
 * @param y - aktualna współrzędna
 * @param x_max - maksymalna wartość współrzędnej
 * @param y_max - maksymalna wartość współrzędnej
 * @return 'c' lub '0'
 */
int
confirm_turn(gamma_t *g, int *last_command, uint32_t player, uint32_t *x, uint32_t *y, uint32_t x_max, uint32_t y_max) {
  int c = getchar();
  int i = 0;
  while (c != VEOF && c != 'G' && c != 'g' && c != ' ' && c != 'C' && c != 'c' && i < 2) {
    if (c == CSI[i]) {
      i++;
    } else if (*last_command == ' ') {
      gamma_move(g, player, *x, *y);
      return '0';
    } else if (*last_command == 'g' || *last_command == 'G') {
      gamma_golden_move(g, player, *x, *y);
      return '0';
    } else {
      return '0';
    }
    c = getchar();
  }
  if ((c == 'c' || c == 'C') && i != 2) {
    return c;
  }
  if (*last_command == ' ') {
    gamma_move(g, player, *x, *y);
  } else {
    gamma_golden_move(g, player, *x, *y);
  }
  switch (c) {
    case 'G':
    case 'g':
    case ' ':
    case VEOF:
      return c;
  }
  if (i == 2) {
    move_cursor(c, x, y, x_max, y_max);
  }
  return '0';
}

void interactive_mode(gamma_t *gamma, uint32_t players, uint32_t x_max, uint32_t y_max) {
  //Ustawienie trybu dzaiłania terminala
  struct termios normal;
  struct termios config;
  if (tcgetattr(STDIN_FILENO, &normal) < 0) {
    fprintf(stderr, "TERMIOS ERROR\n");
    exit(1);
  }
  config = normal;
  config.c_lflag &= ~(ECHO | ICANON);
  if (tcsetattr(STDIN_FILENO, TCSANOW, &config) < 0) {
    fprintf(stderr, "TERMIOS ERROR\n");
    exit(1);
  }
  //Obsługa gry
  uint32_t x = 0;
  uint32_t y = 0;
  uint32_t player;
  int last_command = '0';
  bool in_game = true;
  bool any_player_has_move = true;
  fprintf(stdout, CLEAN_SCREEN);
  fprintf(stdout, CUP);
  while (in_game && any_player_has_move) {
    player = 1;
    any_player_has_move = false;
    while (player <= players && in_game) {
      in_game = turn(gamma, &last_command, player, &x, &y, x_max, y_max, &any_player_has_move);
      if (in_game && any_player_has_move) {
        last_command = confirm_turn(gamma, &last_command, player, &x, &y, x_max, y_max);
      }
      if (last_command == VEOF) {
        in_game = false;
      } else if (last_command == 'c' || last_command == 'C') {
        last_command = '0';
      } else {
        player++;
      }
    }
  }
  fprintf(stdout, CLEAN_SCREEN);
  fprintf(stdout, CUP);
  char *board = gamma_board(gamma);
  fprintf(stdout, "%s", board);
  free(board);
  fprintf(stdout, "RESULTS:\n");
  for (uint32_t i = 1; i <= players; ++i) {
    fprintf(stdout, "Player %u: %lu\n", i, gamma_busy_fields(gamma, i));
  }
  //Powrót do standardowego trybu działania terminala.
  if (tcsetattr(STDIN_FILENO, TCSANOW, &normal) < 0) {
    fprintf(stderr, "TERMIOS ERROR\n");
    exit(1);
  }
}
