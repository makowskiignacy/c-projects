#include <stdlib.h>
#include <stdio.h>

#define WIELKIE 'A' - 'a'

struct pozycja {
  int x_postaci;
  int y_postaci;
  char skrzynia;
  char kierunek;
  struct pozycja *nastepna;
};

void wstaw_pozycje(struct pozycja **pozycja, int x_postaci, int y_postaci, char skrzynia, char kierunek) {
  struct pozycja *pomocnicza = malloc(sizeof(struct pozycja));
  pomocnicza->x_postaci = x_postaci;
  pomocnicza->y_postaci = y_postaci;
  pomocnicza->skrzynia = skrzynia;
  pomocnicza->kierunek = kierunek;
  pomocnicza->nastepna = (*pozycja);
  (*pozycja) = pomocnicza;
}

struct mapa {
  /*punkt(0, 0) - lewy górny róg
  x - rosną w dół
  y - rosną w prawo*/
  int x;
  int x_pamiec;
  int *y;
  int *y_pamiec;
  char **pole;
  _Bool **odwiedzone;
  int *x_skrzyni;
  int *y_skrzyni;
  int x_postaci;
  int y_postaci;
  struct pozycja *pozycja;
};

void dopisz_y(struct mapa *gra, char wartosc) {
  if (gra->y[gra->x - 1] >= gra->y_pamiec[gra->x - 1]) {
    gra->y_pamiec[gra->x - 1] *= 2;
    gra->pole[gra->x - 1] = realloc(gra->pole[gra->x - 1], gra->y_pamiec[gra->x - 1] * sizeof(char));
  }
  gra->pole[gra->x - 1][gra->y[gra->x - 1]] = wartosc;
  gra->y[gra->x - 1]++;
}

void zwolnij_pozycje(struct pozycja **pozycja) {
  while ((*pozycja) != NULL) {
    struct pozycja *pomocnicza = (*pozycja)->nastepna;
    free((*pozycja));
    (*pozycja) = pomocnicza;
  }
}

void zwolnij_pole(struct mapa *gra) {
  if (gra->pole != NULL) {
    for (int i = 0; i < gra->x; i++) {
      free(gra->pole[i]);
      gra->pole[i] = NULL;
    }
    free(gra->pole);
    gra->pole = NULL;
  }
}

void zwolnij_odwiedzone(struct mapa *gra) {
  if (gra->odwiedzone != NULL) {
    for (int i = 0; i < gra->x; i++) {
      free(gra->odwiedzone[i]);
      gra->odwiedzone[i] = NULL;
    }
    free(gra->odwiedzone);
    gra->odwiedzone = NULL;
  }
}

void dopisz_x(struct mapa *gra) {
  if (gra->x >= gra->x_pamiec) {
    gra->x_pamiec *= 2;
    gra->pole = realloc(gra->pole, gra->x_pamiec * sizeof(char*));
    gra->y = realloc(gra->y, gra->x_pamiec * sizeof(int));
    gra->y_pamiec = realloc(gra->y_pamiec, gra->x_pamiec * sizeof(int));
  }
  gra->pole[gra->x] = malloc(sizeof(char));
  gra->y_pamiec[gra->x] = 1;
  gra->y[gra->x] = 0;
  gra->x++;
}

void wyczysc_odwiedzone(struct mapa *gra) {
  for (int i = 0; i < gra->x; i++) {
    for (int j = 0; j < gra->y[i]; j++) {
      gra->odwiedzone[i][j] = 0;
    }
  }
}

void drukuj(struct mapa *gra) {
  for (int i = 0; i < gra->x; i++) {
    for (int j = 0; j < gra->y[i]; j++) {
      printf("%c", gra->pole[i][j]);
    }
    if (i != gra->x - 1) {
      printf("\n");
    }
  }
}

struct mapa *wczytaj_mape(void) {
  struct mapa *gra = malloc(sizeof(struct mapa));
  gra->x = 1;
  gra->x_pamiec = 1;
  gra->y = malloc(sizeof(int));
  gra->y_pamiec = malloc(sizeof(int));
  gra->y[0] = 0;
  gra->y_pamiec[0] = 1;
  gra->pole = malloc(sizeof(char*));
  gra->pole[0] = malloc(sizeof(char));
  gra->x_skrzyni = malloc(('z' - 'a' + 1) * sizeof(int));
  gra->y_skrzyni = malloc(('z' - 'a' + 1) * sizeof(int));
  gra->pozycja = NULL;
  int c = getchar();
  while (c != EOF && (c != '\n' || gra->y[gra->x - 1] != 0)) {
    switch (c) {
      case '\n':
        dopisz_x(gra);
        break;
      default:
        dopisz_y(gra, c);
        if ('a' <= c && c <= 'z') {
          gra->x_skrzyni[c - 'a'] = gra->x - 1;
          gra->y_skrzyni[c - 'a'] = gra->y[gra->x - 1] - 1;
        } else if ('A' <= c && c <= 'Z') {
          gra->x_skrzyni[c - 'A'] = gra->x - 1;
          gra->y_skrzyni[c - 'A'] = gra->y[gra->x - 1] - 1;
        } else if (c == '*' || c == '@') {
          gra->x_postaci = gra->x - 1;
          gra->y_postaci = gra->y[gra->x - 1] - 1;
        }
    }
    c = getchar();
  }
  gra->odwiedzone = malloc(gra->x * sizeof(_Bool*));
  for (int i = 0; i < gra->x; i++) {
    gra->odwiedzone[i] = malloc(gra->y[i] * sizeof(_Bool));
  }
  wyczysc_odwiedzone(gra);
  drukuj(gra);
  return(gra);
}

_Bool wolne_pole(struct mapa *gra, int x_pola, int y_pola) {
  if (0 <= x_pola && x_pola < gra->x) {
    if (0 <= y_pola && y_pola < gra->y[x_pola]) {
      if (!gra->odwiedzone[x_pola][y_pola]) {
        switch (gra->pole[x_pola][y_pola]) {
          case '-':
          case '+':
          case '*':
          case '@':
            return 1;
            break;
        }
      }
    }
  }
  return 0;
}

_Bool sciezka(struct mapa *gra, int x, int y, int x_koncowe, int y_koncowe) {
  if (wolne_pole(gra, x, y)) {
    gra->odwiedzone[x][y] = 1;
    if (x == x_koncowe && y == y_koncowe) {
      return 1;
    } else {
      return(sciezka(gra, x + 1, y, x_koncowe, y_koncowe) || sciezka(gra, x, y + 1, x_koncowe, y_koncowe) || sciezka(gra, x - 1, y, x_koncowe, y_koncowe) || sciezka(gra, x, y - 1, x_koncowe, y_koncowe));
    }
  } else {
    return 0;
  }
}

void przesun_skrzynie(struct mapa *gra, char skrzynia, int x_poczatkowe, int y_poczatkowe, int x_koncowe, int y_koncowe) {
  switch (gra->pole[x_koncowe][y_koncowe]) {
    case '+':
    case '*':
      gra->pole[x_koncowe][y_koncowe] = WIELKIE + skrzynia;
      break;
    case '-':
    case '@':
      gra->pole[x_koncowe][y_koncowe] = skrzynia;
      break;
  }
  if ('a' <= gra->pole[x_poczatkowe][y_poczatkowe] && gra->pole[x_poczatkowe][y_poczatkowe] <= 'z') {
    gra->pole[x_poczatkowe][y_poczatkowe] = '-';
  } else if ('A' <= gra->pole[x_poczatkowe][y_poczatkowe] && gra->pole[x_poczatkowe][y_poczatkowe] <= 'Z') {
    gra->pole[x_poczatkowe][y_poczatkowe] = '+';
  }
  gra->x_skrzyni[skrzynia - 'a'] = x_koncowe;
  gra->y_skrzyni[skrzynia - 'a'] = y_koncowe;
}

void przesun_postac(struct mapa *gra, int x_poczatkowe, int y_poczatkowe, int x_koncowe, int y_koncowe) {
  if (gra->pole[x_poczatkowe][y_poczatkowe] == '*') {
    gra->pole[x_poczatkowe][y_poczatkowe] = '+';
  } else if (gra->pole[x_poczatkowe][y_poczatkowe] == '@') {
    gra->pole[x_poczatkowe][y_poczatkowe] = '-';
  }
  if (gra->pole[x_koncowe][y_koncowe] == '+') {
    gra->pole[x_koncowe][y_koncowe] = '*';
  } else if (gra->pole[x_koncowe][y_koncowe] == '-') {
    gra->pole[x_koncowe][y_koncowe] = '@';
  }
  gra->x_postaci = x_koncowe;
  gra->y_postaci = y_koncowe;
}

void wez_kierunek(char kierunek, int *dx, int *dy) {
  switch (kierunek) {
    case '2':
      (*dx) = 1;
      (*dy) = 0;
      break;
    case '4':
      (*dx) = 0;
      (*dy) = (-1);
      break;
    case '6':
      (*dx) = 0;
      (*dy) = 1;
      break;
    case '8':
      (*dx) = (-1);
      (*dy) = 0;
      break;
  }
}

void ruch(struct mapa *gra, char skrzynia, char kierunek) {
  int dx;
  int dy;
  wez_kierunek(kierunek, &dx, &dy);
  if (wolne_pole(gra, gra->x_skrzyni[skrzynia - 'a'] + dx, gra->y_skrzyni[skrzynia - 'a'] + dy)) {
    if (sciezka(gra, gra->x_postaci, gra->y_postaci, gra->x_skrzyni[skrzynia - 'a'] - dx, gra->y_skrzyni[skrzynia - 'a'] - dy)) {
      wyczysc_odwiedzone(gra);
      przesun_skrzynie(gra, skrzynia, gra->x_skrzyni[skrzynia - 'a'], gra->y_skrzyni[skrzynia - 'a'], gra->x_skrzyni[skrzynia - 'a'] + dx, gra->y_skrzyni[skrzynia - 'a'] + dy);
      wstaw_pozycje(&(gra->pozycja), gra->x_postaci, gra->y_postaci, skrzynia, kierunek);
      przesun_postac(gra, gra->x_postaci, gra->y_postaci, gra->x_skrzyni[skrzynia - 'a'] - dx, gra->y_skrzyni[skrzynia - 'a'] - dy);
    } else {
      wyczysc_odwiedzone(gra);
    }
  }
  drukuj(gra);
}

void cofnij_ruch(struct mapa *gra) {
  if (gra->pozycja != NULL) {
    int dx;
    int dy;
    wez_kierunek(gra->pozycja->kierunek, &dx, &dy);
    przesun_skrzynie(gra, gra->pozycja->skrzynia, gra->x_skrzyni[gra->pozycja->skrzynia - 'a'], gra->y_skrzyni[gra->pozycja->skrzynia - 'a'], gra->x_skrzyni[gra->pozycja->skrzynia - 'a'] - dx, gra->y_skrzyni[gra->pozycja->skrzynia - 'a'] - dy);
    przesun_postac(gra, gra->x_postaci, gra->y_postaci, gra->pozycja->x_postaci, gra->pozycja->y_postaci);
    struct pozycja *pomocnicza;
    pomocnicza = gra->pozycja;
    gra->pozycja = gra->pozycja->nastepna;
    free(pomocnicza);
  }
  drukuj(gra);
}

void wczytaj_polecenie(struct mapa *gra) {
  int polecenie = getchar();
  int kierunek;
  while (polecenie != '.' && polecenie != EOF) {
    if (polecenie == '0') {
      cofnij_ruch(gra);
    } else if ('a' <= polecenie && polecenie <= 'z') {
      kierunek = getchar();
      ruch(gra, polecenie, kierunek);
    }
    polecenie = getchar();
  }
}

void wczytaj(void) {
  struct mapa *gra;
  gra = wczytaj_mape();
  wczytaj_polecenie(gra);
  zwolnij_pozycje(&(gra->pozycja));
  zwolnij_pole(gra);
  zwolnij_odwiedzone(gra);
  free(gra->y);
  free(gra->y_pamiec);
  free(gra->x_skrzyni);
  free(gra->y_skrzyni);
  free(gra);
}

int main(void) {
  wczytaj();
  return 0;
}
