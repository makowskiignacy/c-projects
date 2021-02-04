#include "przechowujaca.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//Zwraca fałsz, gdy nie jest możliwe dodanie elementu
bool dodaj(bst_t **wezel, char *nazwa) {
    if (wezel && !(*wezel) && ((*wezel) = malloc(sizeof(bst_t)))) {
        (*wezel)->nazwa = nazwa;
        (*wezel)->lsyn = NULL;
        (*wezel)->psyn = NULL;
        (*wezel)->nast = NULL;
        return true;
    } else {
        return false;
    }
}

void wyczysc(bst_t *wezel) {
    if (wezel) {
        wyczysc(wezel->lsyn);
        wyczysc(wezel->psyn);
        wyczysc(wezel->nast);
        free(wezel);
    }
}

//Zwraca wskaźnik na węzeł z maksymalnym argumentem
static bst_t **maksymalny_wezel(bst_t **d) {
    if (d && *d) {
        while ((*d)->psyn) {
            d = &(*d)->psyn;
        }
    }
    return d;
}

//Zwraca najbardziej prawy wezeł usuwając go równocześnie z drzewa
//Uwaga! Węzeł ma niezainicjalizowanych synów
static bst_t *odlacz_maksymalny(bst_t **d) {
    bst_t **maksymalny = maksymalny_wezel(d);
    if (maksymalny && *maksymalny) {
        bst_t *wynik = *maksymalny;
        *maksymalny = (*maksymalny)->lsyn;
        return wynik;
    } else {
        return NULL;
    }
}

void usun(bst_t **wezel) {
    if (wezel && *wezel) {
        if (!(*wezel)->lsyn && !(*wezel)->psyn) {
            wyczysc((*wezel)->nast);
            free(*wezel);
            *wezel = NULL;
        } else if (!(*wezel)->lsyn) {
            bst_t *pomocniczy = (*wezel)->psyn;
            wyczysc((*wezel)->nast);
            free(*wezel);
            *wezel = pomocniczy;
        } else if (!(*wezel)->psyn) {
            bst_t *pomocniczy = (*wezel)->lsyn;
            wyczysc((*wezel)->nast);
            free(*wezel);
            *wezel = pomocniczy;
        } else {
            bst_t **pomocniczy_lewy = &(*wezel)->lsyn;
            bst_t **pomocniczy_prawy = &(*wezel)->psyn;
            bst_t *do_zwolnienia = *wezel;
            (*wezel) = odlacz_maksymalny(pomocniczy_lewy);
            (*wezel)->lsyn = *pomocniczy_lewy;
            (*wezel)->psyn = *pomocniczy_prawy;
            wyczysc(do_zwolnienia->nast);
            free(do_zwolnienia);
        }
    }
}

bst_t **znajdz(bst_t **wezel, char *nazwa) {
    if (wezel && *wezel) {
        int porownanie = strcmp(nazwa, (*wezel)->nazwa);
        if (porownanie == 0) {
            return wezel;
        } else if (porownanie < 0) {
            return znajdz(&(*wezel)->lsyn, nazwa);
        } else {
            return znajdz(&(*wezel)->psyn, nazwa);
        }
    } else {
        return wezel;
    }
}
