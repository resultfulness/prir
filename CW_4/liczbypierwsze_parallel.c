#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 10000000
#define S (int)sqrt(N)
#define M N/10

int main(int argc, char **argv) {
    long int tmp[S + 1]; /*tablica pomocnicza*/
    long int pierwsze[M]; /*liczby pierwsze w przedziale 2..N*/
    long i, j, liczba, reszta;
    long int liczba_podzielników; /* l. podzielnikow*/
    long int liczba_liczb_pierwszych = 0; /*l. liczb pierwszych w tablicy pierwsze*/
    double czas; /*zmienna do  mierzenia czasu*/
    FILE *fp;

    /*wyznaczanie podzielnikow z przedzialow 2..S*/
    #pragma omp parallel for private(i)
    for (i = 2; i <= S; i++) {
        tmp[i] = 1; /*inicjowanie*/
    }

    for (i = 2; i <= S; i++) {
        if (tmp[i] == 1) {
            pierwsze[liczba_liczb_pierwszych++] = i; /*zapamietanie podzielnika*/

            /*wykreslanie liczb zlozonych bedacych wielokrotnosciami i*/
            #pragma omp parallel for private(j)
            for (j = i + i; j <= S; j += i) {
                tmp[j] = 0;
            }
        }
    }

    liczba_podzielników = liczba_liczb_pierwszych; /*zapamietanie liczby podzielnikow*/

    int is_prime;
    for (liczba = S + 1; liczba <= N; liczba++) {
        is_prime = 0;

        #pragma omp parallel for private(i, reszta)
        for (i = 0; i < liczba_podzielników; i++) {
            reszta = (liczba % pierwsze[i]);
            if (reszta == 0) { /* liczba zlozona */
                is_prime = 1;
            }
        }

        if (is_prime) {
            pierwsze[liczba_liczb_pierwszych] = liczba; /*zapamietanie liczby pierwszej*/
            liczba_liczb_pierwszych += 1;
        }
    }

    if ((fp = fopen("primes.txt", "w")) == NULL) {
        printf("Nie moge otworzyc pliku do zapisu\n");
        exit(1);
    }

    for (i = 0; i < liczba_liczb_pierwszych; i++) {
        fprintf(fp, "%ld ", pierwsze[i]);
    }

    fclose(fp);
    return 0;
}
