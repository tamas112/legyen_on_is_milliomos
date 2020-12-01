#include "fugvenyek.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define ELVALASZTO ';'
#define SZOVEGHAT '"'


int getline(char*s, int n) {
	int c;
	char*t = s;
	while (n--> 0 && (c = getchar()) != EOF && c != '\n') *t++ = c;
	*t = '\0';
	while (c != EOF&&c != '\n')
		c = getchar();
	return(t - s);
}

#define HSZ sizeof(int)/sizeof(short)*5 /* Az int max. jegyeinek sz ma. */
int egesze(char s[]) {
	int i = 0;		/* Indexel‚s. */
	int kezd;		/* Az els‹ numerikus karakter indexe. */
					/* A karakterl nc elej‚n lev‹ feh‚r karakterek  tl‚p‚se: */
	while (isspace(s[i])) ++i;
	/* Az el‹jel  tl‚p‚se: */
	if (s[i] == '+' || s[i] == '-') ++i;
	kezd = i;		/* A sz mjegyek itt kezd‹dnek. */
					/* El‹rel‚p‚s a k”vetkez‹, els‹ nem numerikus karakterre: */
	while (isdigit(s[i]) && i - kezd < HSZ) ++i;
	/* D”nt‚s: */
	if (kezd == i || !isspace(s[i]) && s[i] != 0) return 0;
	else return 1;
}

char* kovMezo(char* sor) {
	static char* mm = NULL;
	char *eleje, *vege;
	char idezet = 0;
	if (sor) mm = sor;
	else if (!mm) return NULL;
	eleje = vege = mm;
	while (*mm && (*mm != ELVALASZTO || idezet)) {
		if (*mm == SZOVEGHAT)
			if (!idezet) idezet = 1;
			else if (*(mm + 1) == SZOVEGHAT) *vege++ = *mm++;
			else idezet = 0;
		else *vege++ = *mm;
		mm++;
	}
	if (*mm) mm++; else mm = NULL;
	*vege = '\0';
	return eleje;
}