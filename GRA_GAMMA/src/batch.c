/** @file
 * Implementacja wsadowego trybu gry.
 *
 * @author Ignacy Makowski
 */

#include "batch.h"
#include "input.h"
#include <stdio.h>

/**
 * Zamienia string składający się z cyfr na odpowiadającą liczbę
 * @param word - słowo do zamiany
 * @return - odpowiadająca liczba
 */
static uint32_t transform(char *word) {
  char *next_char;
  uint32_t result;
  int64_t temporary = strtoll(word, &next_char, 0);
  if (!word || word[0] > '9' || word[0] < '0' || temporary < 0 || temporary > UINT32_MAX || (*next_char) != '\0') {
    result = UINT32_MAX;
  } else if (temporary == 0 && strcmp("0", word) != 0) {
    result = UINT32_MAX;
  } else {
    result = (uint32_t) temporary;
  }
  return result;
}

/**
 * Funkcja zamienia listę stringów na strukturę odpowiadającą komendzie.
 * Ustawia parametr type na pierwszy char
 * Z pozostałych wyrazów tworzy list liczb, przy użyciu funkcji transform
 * Sprawdza sensowność komend i komunikuje o większości błędów
 * @param l - kolejny wiersz poleceń, zamieniony na listę stringów
 * @param verse - numer wiersza
 * @return poprawna komenda, gotowa do wykonania
 */
static command_t *command_interpreter(list_string_t *l, int *verse) {
  int length = list_string_length(l);
  command_t *result = malloc(sizeof(command_t));
  result->arguments = NULL;
  result->type = NULL;
  if (length > 0 && l->string[0] == '#') {
    free(result);
    return NULL;
  }
  switch (length) {
    case 0:
      free_command(result);
      return NULL;
    case 1:
      if (strcmp(l->string, "p") == 0) {
        result->type = l->string;
        result->arguments = NULL;
        return result;
      } else {
        free_command(result);
        fprintf(stderr, "ERROR %d\n", (*verse));
        return NULL;
      }
    case 2:
      if (strcmp(l->string, "b") == 0 || strcmp(l->string, "f") == 0 || strcmp(l->string, "q") == 0) {
        result->type = l->string;
        result->arguments = malloc(sizeof(struct list_uint));
        result->arguments->next = NULL;
        result->arguments->x = transform(l->next->string);
        if (result->arguments->x == UINT32_MAX) {
          free_command(result);
          fprintf(stderr, "ERROR %d\n", (*verse));
          return NULL;
        }
        return result;
      } else {
        free_command(result);
        fprintf(stderr, "ERROR %d\n", (*verse));
        return NULL;
      }
    case 4:
      if (strcmp(l->string, "m") == 0 || strcmp(l->string, "g") == 0) {
        result->type = l->string;
        result->arguments = malloc(sizeof(struct list_uint));
        list_string_t *temporary = l->next;
        struct list_uint **last = &(result->arguments);
        for (int i = 0; i < 3; ++i) {
          (*last)->x = transform(temporary->string);
          temporary = temporary->next;
          (*last)->next = malloc(sizeof(struct list_uint));
          last = &(*last)->next;
        }
        free(*last);
        (*last) = NULL;
        last = &(result->arguments);
        bool correct_arguments = true;
        while (*last && correct_arguments) {
          correct_arguments = (*last)->x != UINT32_MAX;
          last = &(*last)->next;
        }
        if (!correct_arguments) {
          free_command(result);
          fprintf(stderr, "ERROR %d\n", (*verse));
          return NULL;
        }
        return result;
      } else {
        free_command(result);
        fprintf(stderr, "ERROR %d\n", (*verse));
        return NULL;
      }
    default:
      free_command(result);
      fprintf(stderr, "ERROR %d\n", (*verse));
      return NULL;
  }
}

void batch_mode(gamma_t *gamma, int *verse) {
  list_string_t *temporary;
  command_t *execute;
  while (feof(stdin) == 0) {
    temporary = next_line(verse);
    execute = command_interpreter(temporary, verse);
    struct list_uint *listUint;
    if (execute && strcmp(execute->type, "m") == 0) {
      bool result;
      listUint = execute->arguments;
      result = gamma_move(gamma, listUint->x, listUint->next->x, listUint->next->next->x);
      if (result) {
        fprintf(stdout, "1\n");
      } else {
        fprintf(stdout, "0\n");
      }
    } else if (execute && strcmp(execute->type, "g") == 0) {
      bool result;
      listUint = execute->arguments;
      result = gamma_golden_move(gamma, listUint->x, listUint->next->x, listUint->next->next->x);
      if (result) {
        fprintf(stdout, "1\n");
      } else {
        fprintf(stdout, "0\n");
      }
    } else if (execute && strcmp(execute->type, "b") == 0) {
      uint64_t result;
      listUint = execute->arguments;
      result = gamma_busy_fields(gamma, listUint->x);
      fprintf(stdout, "%lu\n", result);
    } else if (execute && strcmp(execute->type, "f") == 0) {
      uint64_t result;
      listUint = execute->arguments;
      result = gamma_free_fields(gamma, listUint->x);
      fprintf(stdout, "%lu\n", result);
    } else if (execute && strcmp(execute->type, "q") == 0) {
      bool result;
      listUint = execute->arguments;
      result = gamma_golden_possible(gamma, listUint->x);
      if (result) {
        fprintf(stdout, "1\n");
      } else {
        fprintf(stdout, "0\n");
      }
    } else if (execute && strcmp(execute->type, "p") == 0) {
      char *result;
      result = gamma_board(gamma);
      fprintf(stdout, "%s", result);
      free(result);
    }
    free_command(execute);
    free_string_list(temporary);
  }
}
