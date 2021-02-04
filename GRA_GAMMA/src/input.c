/** @file
 * Implementacja funkcji parsujących dane, do interfejsu gry gamma.
 *
 * @author Ignacy Makowski
 */

#include "input.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

int list_string_length(list_string_t *l) {
  int result = 0;
  while (l) {
    result++;
    l = l->next;
  }
  return result;
}

/**
 * @brief Sprawdza czy wczytany wiersz składa się z wypisywalnych znaków.
 * @param line - wiersz
 * @param characters - liczba znaków w wierszu
 * @return
 */
static bool valid_line(char *line, ssize_t characters) {
  bool result = true;
  bool valid_character;
  if (characters > 0) {
    if (line[0] == '#') {
      return true;
    }
    result = isalpha(line[0]) || line[0] == '\n';
  }
  for (ssize_t i = 0; i < characters; ++i) {
    valid_character = isgraph(line[i]);
    valid_character = valid_character || isspace(line[i]);
    result = result && valid_character;
  }
  return result;
}

///@brief Wczytuje następny wiersz ze stanardowego wejścia.
static char *take_line(int *line_indeks) {
  size_t allocated_memory = 0;
  char *buffer = NULL;
  ssize_t length = getline(&buffer, &allocated_memory, stdin);
  if (length != -1 && valid_line(buffer, length)) {
    (*line_indeks)++;
    return buffer;
  } else if (length != -1) {
    //Przypadek z istnieniem niewypisywalnych znaków w wejściu
    (*line_indeks)++;
    fprintf(stderr, "ERROR %d\n", *line_indeks);
    free(buffer);
    return NULL;
  } else {
    free(buffer);
    return NULL;
  }
}

///@brief Przekształca ciąg znaków w listę słów.
static list_string_t *line_to_list(char *line) {
  char *word;
  list_string_t *first = NULL;
  list_string_t *last = NULL;
  if (line) {
    word = strtok(line, " \t\v\f\r\n");
  } else {
    word = NULL;
  }
  if (word) {
    first = malloc(sizeof(list_string_t));
    if (!first) {
      printf("Memory allocation failure\n");
      exit(1);
    }
    first->string = malloc((strlen(word) + 1) * sizeof(char));
    if (!first->string) {
      printf("Memory allocation failure\n");
      exit(1);
    }
    strcpy(first->string, word);
    first->next = NULL;
    last = first;
    word = strtok(NULL, " \t\v\f\r\n");
  }
  while (word) {
    last->next = malloc(sizeof(list_string_t));
    if (!last->next) {
      printf("Memory allocation failure\n");
      exit(1);
    }
    last = last->next;
    last->string = malloc((strlen(word) + 1) * sizeof(char));
    if (!last->string) {
      printf("Memory allocation failure\n");
      exit(1);
    }
    strcpy(last->string, word);
    last->next = NULL;
    word = strtok(NULL, " \t\v\f\r\n");
  }
  free(line);
  return first;
}

list_string_t *next_line(int *verse) {
  return line_to_list(take_line(verse));
}

void free_string_list(list_string_t *l) {
  if (l) {
    free_string_list(l->next);
    free(l->string);
    free(l);
  }
}

void free_uint_list(struct list_uint *l) {
  if (l) {
    free_uint_list(l->next);
    free(l);
  }
}

void free_command(command_t *c) {
  if (c) {
    free_uint_list(c->arguments);
    free(c);
  }
}
