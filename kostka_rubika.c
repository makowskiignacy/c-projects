#include <stdio.h>
#ifndef N
#define N 5
#endif

void druk_zewnetrznej(char A[N][N]) {
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N + 1; j++){
			printf(" ");
		}
		for(int k = 0; k < N; k++){
			printf("%c", A[i][k]);
		}
		printf("\n");
	}
}

void druk_srodkowych(char L[N][N], char F[N][N], char R[N][N], char B[N][N]) {
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			printf("%c", L[i][j]);
		}
		printf("|");
		for(int j = 0; j < N; j++){
			printf("%c", F[i][j]);
		}
		printf("|");
		for(int j = 0; j < N; j++){
			printf("%c", R[i][j]);
		}
		printf("|");
		for(int j = 0; j < N; j++){
			printf("%c", B[i][j]);
		}
		printf("\n");
	}
}

void druk(char U[N][N], char L[N][N], char F[N][N], char R[N][N], char B[N][N], char D[N][N]) {
	printf("\n");
	druk_zewnetrznej(U);
	druk_srodkowych(L, F, R, B);
	druk_zewnetrznej(D);
}

void obrot_wewnetrzny(char T[N][N]) {
	char zmienna;
	for (int i = 0; i < N/2; i++) {
		for (int j = i; j < N - 1 - i; j++) {
			zmienna = T[i][j];
			T[i][j] = T[N - 1 - j][i];
			T[N - 1 - j][i] = T[N - 1 - i][N - 1 - j];
			T[N - 1 - i][N - 1 - j] = T[j][N - 1 - i];
			T[j][N - 1 - i] = zmienna;
		}
	}
}

void obrot_gora(int wiersze, char U[N][N], char L[N][N], char F[N][N], char R[N][N], char B[N][N]) {
	char zmienna;
	obrot_wewnetrzny(U);
	for (int i = 0; i < wiersze; i++) {
		for (int j = 0; j < N; j++) {
			zmienna = L[i][j];
			L[i][j] = F[i][j];
			F[i][j] = R[i][j];
			R[i][j] = B[i][j];
			B[i][j] = zmienna;
		}
	}
}

void obrot_prawo(int wiersze, char R[N][N], char U[N][N], char F[N][N], char D[N][N], char B[N][N]) {
	char zmienna;
	obrot_wewnetrzny(R);
	for (int i = 0; i < wiersze; i++) {
		for (int j = 0; j < N; j++) {
			zmienna = U[N - 1 - j][N - 1 - i];
			U[N - 1 - j][N - 1 - i] = F[N - 1 - j][N - 1 - i];
			F[N - 1 - j][N - 1 - i] = D[N - 1 - j][N - 1 - i];
			D[N - 1 - j][N - 1 - i] = B[j][i];
			B[j][i] = zmienna;
		}
	}
}

void obrot_dol(int wiersze, char D[N][N], char L[N][N], char F[N][N], char R[N][N], char B[N][N]) {
	char zmienna;
	obrot_wewnetrzny(D);
	for (int i = 0; i < wiersze; i++) {
		for (int j = 0; j < N; j++) {
			zmienna = B[N - 1 - i][j];
			B[N - 1 - i][j] = R[N - 1 - i][j];
			R[N - 1 - i][j] = F[N - 1 - i][j];
			F[N - 1 - i][j] = L[N - 1 - i][j];
			L[N - 1 - i][j] = zmienna;
		}
	}
}

void obrot_przod(int wiersze, char F[N][N], char R[N][N], char D[N][N], char L[N][N], char U[N][N]) {
	char zmienna;
	obrot_wewnetrzny(F);
	for (int i = 0; i < wiersze; i++) {
		for (int j = 0; j < N; j++) {
			zmienna = U[N - 1 - i][j];
			U[N - 1 - i][j] = L[N - 1 - j][N - 1 - i];
			L[N - 1 - j][N - 1 - i] = D[i][N - 1 - j];
			D[i][N - 1 - j] = R[j][i];
			R[j][i] = zmienna;
		}
	}
}

void obrot_tyl(int wiersze, char B[N][N], char U[N][N], char L[N][N], char D[N][N], char R[N][N]) {
	char zmienna;
	obrot_wewnetrzny(B);
	for (int i = 0; i < wiersze; i++) {
		for (int j = 0; j < N; j++) {
			zmienna = U[i][N - 1 - j];
			U[i][N - 1 - j] = R[N - 1 - j][N - 1 - i];
			R[N - 1 - j][N - 1 - i] = D[N - 1 - i][j];
			D[N - 1 - i][j] = L[j][i];
			L[j][i] = zmienna;
		}
	}
}

void obrot_lewo(int wiersze, char L[N][N], char B[N][N], char D[N][N], char F[N][N], char U[N][N]) {
	char zmienna;
	obrot_wewnetrzny(L);
	for (int i = 0; i < wiersze; i++) {
		for (size_t j = 0; j < N; j++) {
			zmienna = B[N - 1 - j][N - 1 - i];
			B[N - 1 - j][N - 1 - i] = D[j][i];
			D[j][i] = F[j][i];
			F[j][i] = U[j][i];
			U[j][i] = zmienna;
		}
	}
}

void obrot(char sciana, int wiersze, int katobrotu, char U[N][N], char L[N][N], char F[N][N], char R[N][N], char B[N][N], char D[N][N]) {
	if (wiersze == 0) {
		wiersze++;
	}
	for (int i = 0; i < katobrotu; i++) {
		switch (sciana) {
			case 'u': obrot_gora(wiersze, U, L, F, R, B); break;
			case 'r': obrot_prawo(wiersze, R, U, F, D, B); break;
			case 'd': obrot_dol(wiersze, D, L, F, R, B); break;
			case 'f': obrot_przod(wiersze, F, R, D, L, U); break;
			case 'b': obrot_tyl(wiersze, B, U, L, D, R); break;
			case 'l': obrot_lewo(wiersze, L, B, D, F, U); break;
			case 0: break;
		}
	}
	if (wiersze == N) {
		for (int i = 0; i < (4 - katobrotu); i++) {
			switch (sciana) {
				case 'u': obrot_wewnetrzny(D); break;
				case 'r': obrot_wewnetrzny(L); break;
				case 'd': obrot_wewnetrzny(U); break;
				case 'f': obrot_wewnetrzny(B); break;
				case 'b': obrot_wewnetrzny(F); break;
				case 'l': obrot_wewnetrzny(R); break;
				case 0: break;
			}
		}
	}
}

void wczytaj_wywolaj(char U[N][N], char L[N][N], char F[N][N], char R[N][N], char B[N][N], char D[N][N]) {
	char sciana = 0;
	int wiersze = 0;
	int katobrotu = 1;
	int c = getchar();
	while (c != EOF && c != '.') {
		switch (c) {
			case 'u':
				obrot(sciana, wiersze, katobrotu, U, L, F, R, B, D);
				sciana = 'u';
				wiersze = 0;
				katobrotu = 1;
				break;
			case 'r':
				obrot(sciana, wiersze, katobrotu, U, L, F, R, B, D);
				sciana = 'r';
				wiersze = 0;
				katobrotu = 1;
				break;
			case 'd':
				obrot(sciana, wiersze, katobrotu, U, L, F, R, B, D);
				sciana = 'd';
				wiersze = 0;
				katobrotu = 1;
				break;
			case 'f':
				obrot(sciana, wiersze, katobrotu, U, L, F, R, B, D);
				sciana = 'f';
				wiersze = 0;
				katobrotu = 1;
				break;
			case 'b':
				obrot(sciana, wiersze, katobrotu, U, L, F, R, B, D);
				sciana = 'b';
				wiersze = 0;
				katobrotu = 1;
				break;
			case 'l':
				obrot(sciana, wiersze, katobrotu, U, L, F, R, B, D);
				sciana = 'l';
				wiersze = 0;
				katobrotu = 1;
				break;
			case '\'':
				katobrotu = 3;
				break;
			case '\"':
				katobrotu = 2;
				break;
			case '\n':
				obrot(sciana, wiersze, katobrotu, U, L, F, R, B, D);
				sciana = 0;
				wiersze = 0;
				katobrotu = 1;
			 	druk(U, L, F, R, B, D);
				break;
			case '.': break;
			default:
				wiersze = (10 * wiersze) + c - '0';
		}
		c = getchar();
	}
}

int main(void) {
	char U[N][N];
	char L[N][N];
	char F[N][N];
	char R[N][N];
	char B[N][N];
	char D[N][N];
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			U[i][j] = '0';
			L[i][j] = '1';
			F[i][j] = '2';
			R[i][j] = '3';
			B[i][j] = '4';
			D[i][j] = '5';
		}
	}
	wczytaj_wywolaj(U, L, F, R, B, D);
}
