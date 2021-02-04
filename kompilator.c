
#include <stdio.h>
#include <stdlib.h>

#define SKOK_PAMIECI 1000
#define PUSH_0 0
#define PUSH_1 1
#define OUTPUT_0 2
#define OUTPUT_1 3
#define POP_BRANCH 4
#define INPUT_BRANCH 5
#define JUMP 6
#define CALL 7
#define RETURN 8
#define HALT 9

struct kody {
  char *bazowy;
  int bazowy_indeks;
  int *maszynowy;
  int maszynowy_indeks;
  int maszynowy_wers;
  int *tablica_symboli;
  int zarezerwowana_pamiec;
};

void tresc(struct kody *kod);

void maszynowy_dopisz(int zmienna, struct kody *kod) {
  kod->maszynowy_indeks++;
  if ((kod->maszynowy_indeks + 1) > kod->zarezerwowana_pamiec) {
    kod->zarezerwowana_pamiec += SKOK_PAMIECI;
    kod->maszynowy = realloc(kod->maszynowy, kod->zarezerwowana_pamiec * sizeof(int));
  }
  kod->maszynowy[kod->maszynowy_indeks] = zmienna;
}

void odwolania(struct kody *kod) {
  int i = 0;
  while (i <= kod->maszynowy_indeks) {
    if (kod->maszynowy[i] < 0) {
      kod->maszynowy[i] = kod->tablica_symboli[-(kod->maszynowy[i]) - 1];
    }
    i++;
  }
}

void druk(int T[]) {
  int i = 0;
  while (T[i] != HALT) {
    switch (T[i]) {
      case PUSH_0: printf("0 %d\n", T[i + 1]);
        i++;
        break;
      case PUSH_1: printf("1 %d\n", T[i + 1]);
        i++;
        break;
      case OUTPUT_0: printf("2\n");
        break;
      case OUTPUT_1: printf("3\n");
        break;
      case POP_BRANCH: printf("4 %d ", T[i + 1]);
        printf("%d\n", T[i + 2]);
        i += 2;
        break;
      case INPUT_BRANCH: printf("5 %d\n", T[i + 1]);
        i++;
        break;
      case JUMP: printf("6 %d\n", T[i + 1]);
        i++;
        break;
      case CALL: printf("7 %d\n", T[i + 1]);
        i++;
        break;
      case RETURN: printf("8\n");
        break;
    }
    i++;
  }
  printf("%d\n", T[i]);
}

char *sama_procedura(void) {
  int i = 0;
  int zarezerwowany_rozmiar = SKOK_PAMIECI;
  char *T = malloc(zarezerwowany_rozmiar * sizeof(char));
  int c = getchar();
  while (c != EOF) {
    switch (c) {
      case ' ': break;
      case '\n': break;
      case '\t': break;
      case '\v': break;
      case '\f': break;
      case '\r': break;
      case ';':
        while (c != '\n') {
          c = getchar();
        }
        break;
      default:
        i++;
        if ((i + 1) > zarezerwowany_rozmiar) {
          zarezerwowany_rozmiar += SKOK_PAMIECI;
          T = realloc(T, zarezerwowany_rozmiar * sizeof(char));
        }
        T[i - 1] = c;
    }
    c = getchar();
  }
  return T;
}

void wybor(struct kody *kod) {
  int adres;
  if (kod->bazowy[kod->bazowy_indeks] == '$') {
    kod->bazowy_indeks++;
    maszynowy_dopisz(INPUT_BRANCH, kod);
    maszynowy_dopisz(-1, kod);
    adres = kod->maszynowy_indeks;
    kod->maszynowy_wers++;
    tresc(kod);
    kod->maszynowy[adres] = kod->maszynowy_wers + 2;
    maszynowy_dopisz(JUMP, kod);
    maszynowy_dopisz(-1, kod);
    adres = kod->maszynowy_indeks;
    kod->maszynowy_wers++;
    tresc(kod);
    kod->maszynowy[adres] = kod->maszynowy_wers + 1;
  }else {
    maszynowy_dopisz(POP_BRANCH, kod);
    maszynowy_dopisz(-1, kod);
    adres = kod->maszynowy_indeks;
    maszynowy_dopisz(kod->bazowy[kod->bazowy_indeks] -'a', kod);
    kod->bazowy_indeks++;
    kod->maszynowy_wers++;
    tresc(kod);
    kod->maszynowy[adres] = kod->maszynowy_wers + 2;
    maszynowy_dopisz(JUMP, kod);
    maszynowy_dopisz(-1, kod);
    adres = kod->maszynowy_indeks;
    kod->maszynowy_wers++;
    tresc(kod);
    kod->maszynowy[adres] = kod->maszynowy_wers + 1;
  }
}

void zapis(struct kody *kod) {
  if (kod->bazowy[kod->bazowy_indeks] == '$') {
    kod->bazowy_indeks++;
    while ((kod->bazowy[kod->bazowy_indeks] == '+') || (kod->bazowy[kod->bazowy_indeks] == '-')) {
      if (kod->bazowy[kod->bazowy_indeks] == '+') {
        maszynowy_dopisz(OUTPUT_1, kod);
        kod->maszynowy_wers++;
      }else {
        maszynowy_dopisz(OUTPUT_0, kod);
        kod->maszynowy_wers++;
      }
      kod->bazowy_indeks++;
    }
  }else {
    int stos = kod->bazowy[kod->bazowy_indeks] - 'a';
    kod->bazowy_indeks++;
    while ((kod->bazowy[kod->bazowy_indeks] == '+') || (kod->bazowy[kod->bazowy_indeks] == '-')) {
      if (kod->bazowy[kod->bazowy_indeks] == '+') {
        maszynowy_dopisz(PUSH_1, kod);
        maszynowy_dopisz(stos, kod);
        kod->maszynowy_wers++;
      }else {
        maszynowy_dopisz(PUSH_0, kod);
        maszynowy_dopisz(stos, kod);
        kod->maszynowy_wers++;
      }
      kod->bazowy_indeks++;
    }
  }
}

void wywolanie(struct kody *kod) {
  maszynowy_dopisz(CALL, kod);
  maszynowy_dopisz(-(kod->bazowy[kod->bazowy_indeks] + 1 - 'A'), kod);
  kod->bazowy_indeks++;
  kod->maszynowy_wers++;
}

void instrukcja(struct kody *kod) {
  if (kod->bazowy[kod->bazowy_indeks] != '}') {
    if ((kod->bazowy[kod->bazowy_indeks] >= 'A') && (kod->bazowy[kod->bazowy_indeks] <= 'Z')) {
      wywolanie(kod);
    }else if (((kod->bazowy[kod->bazowy_indeks] >= 'a') && (kod->bazowy[kod->bazowy_indeks] <= 'z')) || (kod->bazowy[kod->bazowy_indeks] == '$')) {
      if (kod->bazowy[kod->bazowy_indeks + 1] == '{') {
        wybor(kod);
      }else if ((kod->bazowy[kod->bazowy_indeks + 1] == '-') || (kod->bazowy[kod->bazowy_indeks + 1] == '+')) {
        zapis(kod);
      }
    }
  }
}

void ciag_instrucji(struct kody *kod) {
  instrukcja(kod);
  if (kod->bazowy[kod->bazowy_indeks] != '}') {
    ciag_instrucji(kod);
  }
}

void tresc(struct kody *kod) {
  if (kod->bazowy[kod->bazowy_indeks] == '{') {
    kod->bazowy_indeks++;
    ciag_instrucji(kod);
    kod->bazowy_indeks++;
  }
}

void procedura_glowna(struct kody *kod) {
  tresc(kod);
  maszynowy_dopisz(HALT, kod);
  kod->maszynowy_wers++;
}

void procedura(struct kody *kod) {
  kod->tablica_symboli[kod->bazowy[kod->bazowy_indeks] - 'A'] = kod->maszynowy_wers + 1;
  kod->bazowy_indeks++;
  tresc(kod);
  maszynowy_dopisz(RETURN, kod);
  kod->maszynowy_wers++;
}

void ciag_procedur(struct kody *kod) {
  if (kod->bazowy[kod->bazowy_indeks] >= 'A' && kod->bazowy[kod->bazowy_indeks] <= 'Z') {
    procedura(kod);
    ciag_procedur(kod);
  }
}

int *program(struct kody *kod) {
  ciag_procedur(kod);
  kod->maszynowy[1] = kod->maszynowy_wers + 1;
  procedura_glowna(kod);
  odwolania(kod);
  return kod->maszynowy;
}

int main(void) {
  struct kody (*kod);
  kod = malloc(sizeof(struct kody));
  kod->bazowy = sama_procedura();
  kod->bazowy_indeks = 0;
  kod->zarezerwowana_pamiec = 0;
  kod->maszynowy = malloc(0);
  kod->maszynowy_indeks = -1;
  kod->tablica_symboli = malloc(('Z'-'A' + 1)*sizeof(int));
  kod->maszynowy_wers = 0;
  maszynowy_dopisz(JUMP, kod);
  maszynowy_dopisz(-1, kod);
  int *wynik = program(kod);
  druk(wynik);
  free(kod->bazowy);
  free(kod->maszynowy);
  free(kod->tablica_symboli);
  free(kod);
  return 0;
}
