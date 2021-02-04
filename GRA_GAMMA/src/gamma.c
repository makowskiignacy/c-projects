/** @file
 * @brief Implementacja modułu "silnika" gry gamma.
 *
 * @author Ignacy Makowski
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "gamma.h"

/**
 * @brief Struktura przechowująca informacje o graczu.
 */
struct player {
  ///Ilość posiadanych przez gracza obszarów.
  uint32_t own_areas;
  ///Prawda, jeśli gracz może wykonać jeszcze złoty ruch.
  bool golden_move;
};
///Struktura przechowująca informacje o graczu.
typedef struct player player_t;

/**
 * @brief Struktura przechowująca stan gry.
 */
struct gamma {
  ///Szerokość planszy.
  uint32_t width;
  ///Wysokość planszy.
  uint32_t height;
  ///Ilość graczy.
  uint32_t players;
  ///Maksymalna ilość obszarów posiadanych przez gracza.
  uint32_t areas_limit;
  ///Tablica struktur player_t, informacje o graczach.
  player_t *player_overview;
  ///Główna plansza 0 - puste pole, numer gracza - pole zajęte.
  uint32_t **board;
  ///Pomocnicza plansza odwiedzonych.
  bool **visited;
};
///Struktura przechowująca stan gry.
typedef struct gamma gamma_t;

/** @brief Inicjalizuje strukturę przechowującą informacje o graczach.
 * Alokuje pamięć na strukturę.
 * Inicjuje ją tak, aby reprezentowała początkowy stan gry.
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry.
 */
static void init_player_overview(gamma_t *g) {
  g->player_overview = malloc(g->players * sizeof(player_t));
  if (g->player_overview) {
    for (uint32_t i = 0; i < g->players; i++) {
      g->player_overview[i].own_areas = 0;
      g->player_overview[i].golden_move = true;
    }
  }
}

/** @brief Inicjalizuje planszę, będącą częścią struktury gammma.
 * Alokuje pamięć na planszę.
 * Inicjuje ją zerami.
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry.
 */
static void init_board(gamma_t *g) {
  g->board = malloc(g->height * sizeof(uint32_t *));
  if (g->board) {
    bool successful_allocation = true;
    for (uint32_t i = 0; i < g->height; i++) {
      g->board[i] = calloc(g->width, sizeof(uint32_t));
      successful_allocation = successful_allocation && g->board[i];
    }
    if (!successful_allocation) {
      for (uint32_t i = 0; i < g->height; i++) {
        free(g->board[i]);
      }
      free(g->board);
      g->board = NULL;
    }
  }
}

/** @brief Inicjalizuje pomocniczą planszę odwiedzonych(część struktury @p gamma).
 * Alokuje pamięć na planszę.
 * Inicjuje ją wartością nie odwiedzone.
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry.
 */
static void init_visited(gamma_t *g) {
  g->visited = malloc(g->height * sizeof(bool *));
  if (g->visited) {
    bool successful_allocation = true;
    for (uint32_t i = 0; i < g->height; i++) {
      g->visited[i] = malloc(g->width * sizeof(bool));
      successful_allocation = successful_allocation && g->visited[i];
      if (g->visited[i]) {
        for (uint32_t j = 0; j < g->width; j++) {
          g->visited[i][j] = false;
        }
      }
    }
    if (!successful_allocation) {
      for (uint32_t i = 0; i < g->height; i++) {
        free(g->visited[i]);
      }
      free(g->visited);
      g->visited = NULL;
    }
  }
}

/** @brief Czyści pomocniczą planszę odwiedzonych(część struktury @p gamma).
 * Wypełnia ją wartością nie odwiedzone.
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry.
 */
static void reset_visited(gamma_t *g) {
  if (g) {
    for (uint32_t i = 0; i < g->height; i++) {
      for (uint32_t j = 0; j < g->width; j++) {
        g->visited[i][j] = false;
      }
    }
  }
}

/** @brief Sprawdza czy współrzędne znajdują się na planszy.
 * Sprawdza czy struktura gamma nie jest NULL-em.
 * Porównuje współrzędne x, y z wartościami granicznymi dla strukturzy g.
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] x – współrzędna szerokości.
 * @param[in] y – współrzędna wysokości.
 * @return Prawda, jeśli współrzędne są poprawne.
 */
static bool valid_coordinates(gamma_t *g, uint32_t x, uint32_t y) {
  return g && (x < g->width) && (y < g->height);
}

/** @brief Sprawdza czy @p x, @p y, @p player są mają poprawne wartości dla danej gammy.
 * Sprawdza czy struktura gamma nie jest NULL-em.
 * Sprawdza czy gracz jest w zadeklarowanym przedziale.
 * Wykorzystuje funkcję valid_coordinates
 * do sprawdzenia poprawności współrzędnych.
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] player – numer gracza i jego pozycja w strukturze + 1;
 * @param[in] x – współrzędna szerokości.
 * @param[in] y – współrzędna wysokości.
 * @return Prawda, jeśli współrzędne i numer gracza są poprawne.
 */
static bool valid_input(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  return g && player > 0 && player <= g->players && valid_coordinates(g, x, y);
}

/** @brief Sprawdza początkowe warunki do rozpatyrywania złotego ruchu.
 * Sprawdza czy pole nie należy już do gracza @p player.
 * Sprawdza czy pole nie jest puste.
 * Sprawdza czy gracz nie wykonał już złotego ruchu.
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] player – numer gracza i jego pozycja w strukturze + 1;
 * @param[in] x – współrzędna szerokości.
 * @param[in] y – współrzędna wysokości.
 * @return Prawda, jeśli powyższe warunki są spełnione
 * @warning Należy używać razem z warunkiem valid_input
 */
static bool valid_input_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  if (g->board[y][x] == player) {
    return false;
  } else if (g->board[y][x] == 0) {
    return false;
  } else if (g->player_overview[player - 1].golden_move == false) {
    return false;
  } else {
    return true;
  }
}

/** @brief Sprawdza czy pole graniczy z posiadanym przez gracza obszarem.
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] player – numer gracza i jego pozycja w strukturze + 1;
 * @param[in] x – współrzędna szerokości.
 * @param[in] y – współrzędna wysokości.
 * @return Prawda, jeśli pole gracza @p player graniczy z jego obszarem
 */
static bool adjacent_to_area(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  bool outcome = false;
  if (valid_coordinates(g, x + 1, y) && g->board[y][x + 1] == player) {
    outcome = true;
  } else if (valid_coordinates(g, x, y + 1) && g->board[y + 1][x] == player) {
    outcome = true;
  } else if (valid_coordinates(g, x - 1, y) && g->board[y][x - 1] == player) {
    outcome = true;
  } else if (valid_coordinates(g, x, y - 1) && g->board[y - 1][x] == player) {
    outcome = true;
  }
  return outcome;
}

/** @brief Sprawdza czy pole należy do innego nieodwiedzonego obszaru.
 * Zaznacza wyszystkie pola należące do obszaru jako odwiedzone.
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] player – numer gracza i jego pozycja w strukturze + 1;
 * @param[in] x – współrzędna szerokości.
 * @param[in] y – współrzędna wysokości.
 * @return Prawda jeśli nie obszar nie został odwiedzony przy wywołaniu funkcji
 */
static bool separate_area(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  if (!valid_coordinates(g, x, y) || g->visited[y][x] || g->board[y][x] != player) {
    return false;
  } else {
    g->visited[y][x] = true;
    separate_area(g, player, x + 1, y);
    separate_area(g, player, x - 1, y);
    separate_area(g, player, x, y + 1);
    separate_area(g, player, x, y - 1);
    return true;
  }
}

/** @brief Liczy ilość sąsiadujących obszarów gracza @p player z polem.
 * Zaznacza pole początkowe jako odwiedzone.
 * Dla sąsiednich pól wykorzystuje funkcję separate_area.
 * Zlicza ilość niezależnych obszarów
 * @param[in] g – wskaźnik na strukturę przechowującą stan gry.
 * @param[in] player – numer gracza i jego pozycja w strukturze + 1;
 * @param[in] x – współrzędna szerokości.
 * @param[in] y – współrzędna wysokości.
 * @return Nieujemną liczbę obszarów nie większą niż 4.
 */
static uint32_t adjacent_areas(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint32_t outcome = 0;
  g->visited[y][x] = true;
  if (separate_area(g, player, x + 1, y)) {
    outcome++;
  }
  if (separate_area(g, player, x - 1, y)) {
    outcome++;
  }
  if (separate_area(g, player, x, y + 1)) {
    outcome++;
  }
  if (separate_area(g, player, x, y - 1)) {
    outcome++;
  }
  reset_visited(g);
  return outcome;
}

bool this_move_possible(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  if (!valid_input(g, player, x, y) || g->board[y][x] != 0) {
    return false;
  } else if (g->player_overview[player - 1].own_areas >= g->areas_limit
             && !adjacent_to_area(g, player, x, y)) {
    return false;
  } else {
    return true;
  }
}

bool this_golden_move_possible(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  if (!valid_input(g, player, x, y) || !valid_input_golden_move(g, player, x, y)) {
    return false;
  } else if (g->player_overview[player - 1].own_areas >= g->areas_limit
             && !adjacent_to_area(g, player, x, y)) {
    return false;
  } else {
    uint32_t target_player = g->board[y][x];
    uint32_t target_player_areas;
    //Posiadane przez wskazanego gracza obszary
    target_player_areas = g->player_overview[target_player - 1].own_areas;
    //Obszary dodane po rozdzieleniu złotym ruchem
    target_player_areas += adjacent_to_area(g, target_player, x, y);
    //Odjęcie policzonego dodatkowo
    --target_player_areas;
    if (target_player_areas > g->areas_limit) {
      return false;
    }
    return true;
  }
}

gamma_t *gamma_new(uint32_t width, uint32_t height, uint32_t players, uint32_t areas) {
  if (width == 0 || height == 0 || players == 0 || areas == 0) {
    return NULL;
  } else {
    gamma_t *g = malloc(sizeof(gamma_t));
    if (!g) {
      printf("Memory allocation failure\n");
      exit(1);
    }
    g->width = width;
    g->height = height;
    g->players = players;
    g->areas_limit = areas;
    init_player_overview(g);
    init_board(g);
    init_visited(g);
    if (g->player_overview && g->board && g->visited) {
      return g;
    } else {
      gamma_delete(g);
      return NULL;
    }
  }
}

void gamma_delete(gamma_t *g) {
  if (g) {
    free(g->player_overview);
    if (g->board) {
      for (uint32_t i = 0; i < g->height; i++) {
        free(g->board[i]);
      }
    }
    if (g->visited) {
      for (uint32_t i = 0; i < g->height; i++) {
        free(g->visited[i]);
      }
    }
    free(g->board);
    free(g->visited);
    free(g);
  }
}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  if (!valid_input(g, player, x, y) || g->board[y][x] != 0) {
    return false;
  } else {
    uint32_t player_areas = g->player_overview[player - 1].own_areas;
    if (adjacent_to_area(g, player, x, y)) {
      g->board[y][x] = player;
      uint32_t connected_areas = adjacent_areas(g, player, x, y);
      g->player_overview[player - 1].own_areas = player_areas - connected_areas + 1;
      return true;
    } else if (player_areas < g->areas_limit) {
      g->player_overview[player - 1].own_areas++;
      g->board[y][x] = player;
      uint32_t connected_areas = adjacent_areas(g, player, x, y);
      g->player_overview[player - 1].own_areas = player_areas - connected_areas + 1;
      return true;
    } else {
      return false;
    }
  }
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  if (!valid_input(g, player, x, y) || !valid_input_golden_move(g, player, x, y)) {
    return false;
  } else {
    uint32_t target_player = g->board[y][x];
    uint32_t new_areas = adjacent_areas(g, target_player, x, y);
    uint32_t target_player_areas;
    target_player_areas = g->player_overview[target_player - 1].own_areas + new_areas - 1;
    if (target_player_areas <= g->areas_limit) {
      uint32_t player_areas = g->player_overview[player - 1].own_areas;
      if (adjacent_to_area(g, player, x, y)) {
        g->board[y][x] = player;
        g->player_overview[target_player - 1].own_areas = target_player_areas;
        uint32_t connected_areas = adjacent_areas(g, player, x, y);
        g->player_overview[player - 1].own_areas = player_areas - connected_areas + 1;
        g->player_overview[player - 1].golden_move = false;
        return true;
      } else if (player_areas < g->areas_limit) {
        g->player_overview[player - 1].own_areas++;
        g->board[y][x] = player;
        g->player_overview[target_player - 1].own_areas = target_player_areas;
        uint32_t connected_areas = adjacent_areas(g, player, x, y);
        g->player_overview[player - 1].own_areas = player_areas - connected_areas + 1;
        g->player_overview[player - 1].golden_move = false;
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  }
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
  uint64_t outcome = 0;
  if (g) {
    for (uint32_t i = 0; i < g->height; i++) {
      for (uint32_t j = 0; j < g->width; j++) {
        if (g->board[i][j] == player) {
          outcome++;
        }
      }
    }
  }
  return outcome;
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
  uint64_t outcome = 0;
  if (!g) {
    return outcome;
  }
  uint32_t player_areas = g->player_overview[player - 1].own_areas;
  if (player <= g->players && player_areas < g->areas_limit) {
    for (uint32_t i = 0; i < g->height; i++) {
      for (uint32_t j = 0; j < g->width; j++) {
        if (g->board[i][j] == 0) {
          outcome++;
        }
      }
    }
  } else if (player <= g->players && player_areas == g->areas_limit) {
    for (uint32_t i = 0; i < g->height; i++) {
      for (uint32_t j = 0; j < g->width; j++) {
        if (g->board[i][j] == 0 && adjacent_to_area(g, player, j, i)) {
          outcome++;
        }
      }
    }
  }
  return outcome;
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
  if (!g || player == 0 || g->player_overview[player - 1].golden_move == false) {
    return false;
  }
  bool additional_areas = g->player_overview[player - 1].own_areas < g->areas_limit;
  uint32_t i = 0;
  uint32_t j = 0;
  bool other_player = false;
  while (i < g->height && !other_player) {
    j = 0;
    while (j < g->width && !other_player) {
      if (g->board[i][j] != 0 && g->board[i][j] != player) {
        if (additional_areas) {
          other_player = this_golden_move_possible(g, player, j, i);
        } else {
          other_player = adjacent_areas(g, player, j, i) && this_golden_move_possible(g, player, j, i);
        }
      }
      j++;
    }
    i++;
  }
  return other_player;
}

char *gamma_board(gamma_t *g) {
  if (!g) {
    return NULL;
  } else {
    char *board_to_string = malloc((g->height * (g->width + 1) + 1) * sizeof(char));
    if (!board_to_string) {
      printf("Memory allocation failure\n");
      exit(1);
    }
    uint32_t c;
    uint32_t k = 0;
    for (uint32_t i = g->height; i > 0; i--) {
      for (uint32_t j = 0; j < g->width; j++) {
        c = g->board[i - 1][j];
        switch (c) {
          case 0:
            board_to_string[k] = '.';
            break;
          default:
            board_to_string[k] = '0' + c;
        }
        k++;
      }
      board_to_string[k] = '\n';
      k++;
    }
    board_to_string[k] = '\0';
    return board_to_string;
  }
}
