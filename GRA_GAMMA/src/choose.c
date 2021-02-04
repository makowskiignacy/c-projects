/** @file
 * Implementacja funkcji do wyboru interfejsu gry gamma (tekstowy/interaktywny).
 *
 * @author Ignacy Makowski
 */

#include "choose.h"
#include "gamma.h"
#include "batch.h"
#include "interactive.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

///Prawidłowa liczba argumentów
#define ARGUMENTS 5

/**
 * Zamienia string składający się z cyfr na odpowiadającą liczbę
 * @param word - słowo do zamiany
 * @return - odpowiadająca liczba
 */
static uint32_t transform(char *word) {
  char *next_char;
  uint32_t result;
  int64_t temporary = strtoll(word, &next_char, 0);
  if (!word || word[0] > '9' || word[0] < '0' || temporary <= 0 || temporary > UINT32_MAX || (*next_char) != '\0') {
    result = 0;
  } else {
    result = (uint32_t) temporary;
  }
  return result;
}

/**
 * Funkcja zamienia listę stringów z weścia na gotową do zaaplikowania poprawną komendę
 * @param l - kolejny wiersz wejścia zamieniony w listę słów
 * @return komendę, która posiada listę uint oraz rodzaj lub NULL w przypadku braku możliwości poprawnej interpretacji
 */
static command_t *command_interpreter(list_string_t *l) {
  int length = list_string_length(l);
  if (length == 0) {
    return NULL;
  } else {
    if (l && l->string && l->string[0] == '#') {
      return NULL;
    }
    command_t *result = malloc(sizeof(command_t));
    result->type = l->string;
    result->arguments = NULL;
    l = l->next;
    struct list_uint *arg_first = malloc(sizeof(struct list_uint));
    struct list_uint **arg_last = &arg_first;
    while (l) {
      (*arg_last)->x = transform(l->string);
      (*arg_last)->next = malloc(sizeof(struct list_uint));
      arg_last = &(*arg_last)->next;
      l = l->next;
    }
    free(*arg_last);
    (*arg_last) = NULL;
    result->arguments = arg_first;
    bool correct_arguments = true;
    while (arg_first && correct_arguments) {
      correct_arguments = arg_first->x != 0;
      arg_first = arg_first->next;
    }
    if (correct_arguments && length == ARGUMENTS) {
      return result;
    } else {
      free_command(result);
      return NULL;
    }
  }
}

void choose_mode(void) {
  int verse = 0;
  list_string_t *temporary = NULL;
  command_t *command = NULL;
  gamma_t *gamma = NULL;
  uint32_t w;
  uint32_t h;
  uint32_t p;
  uint32_t a;
  while (!gamma && feof(stdin) == 0) {
    free_string_list(temporary);
    free_command(command);
    temporary = next_line(&verse);
    command = command_interpreter(temporary);
    if (command) {
      w = command->arguments->x;
      h = command->arguments->next->x;
      p = command->arguments->next->next->x;
      a = command->arguments->next->next->next->x;
      gamma = gamma_new(w, h, p, a);
      if (!gamma) {
        fprintf(stderr, "ERROR %d\n", verse);
      } else if (strcmp(command->type, "I") == 0) {
        fprintf(stdout, "OK %d\n", verse);
        interactive_mode(gamma, p, w, h);
      } else if (strcmp(command->type, "B") == 0) {
        fprintf(stdout, "OK %d\n", verse);
        batch_mode(gamma, &verse);
      } else {
        gamma_delete(gamma);
        gamma = NULL;
        fprintf(stderr, "ERROR %d\n", verse);
      }
    } else if (temporary && (temporary->string && temporary->string[0] != '#')) {
      fprintf(stderr, "ERROR %d\n", verse);
    }
  }
  free_command(command);
  free_string_list(temporary);
  gamma_delete(gamma);
}
