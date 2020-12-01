#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "fugvenyek.h"

#define SORHOSSZ 250
#define MAX 250
#define MAX_NEHEZSEG 15

struct Kerdes {
	int nehezseg;
	char szoveg[MAX];
	char a[MAX];
	char b[MAX];
	char c[MAX];
	char d[MAX];
	char valasz;
	char kategoria[MAX];
	int index;

	struct Kerdes *kov;
};

struct Kerdes* kerdesek[16];

//aktu�lis �s a m�r a pont�bl�zatban l�v� j�t�kosok neve, pontja
typedef struct adat {
	char nev[3 + 1];
	int pont;
} adat;
//j�t�kosok pontja �s neve folyamatos t�rol�sa
adat aktjatekos;
adat eredmenyek[10];


//J�t�k men�je:
void menu() {
	FILE *fmenu = fopen("menu.txt", "rt");
	char sor[SORHOSSZ + 1];
	char s[2];
	system("cls");

	//Men� megjelen�t�se
	if (fmenu != NULL) {
		while (fgets(sor, SORHOSSZ, fmenu)) {
			printf("%s", sor);
		}
		fclose(fmenu);
		//Men�pont v�laszt�sa
		getline(s, 2);
		if (egesze(s) != 1 || atoi(s) < 1 || atoi(s) > 5 || strlen(s) != 1) {
			printf("Helytelen karaktert adott meg!\n");
			_sleep(1500);
			menu();
		}

		//�j j�t�k ind�t�sa
		if (atoi(s) == 1)
			ujjatek();

		//Gyakorl� j�t�k ind�t�sa
		else if (atoi(s) == 2)
			gyakorlas();

		//El�rt pontok megtekint�se
		else if (atoi(s) == 3)
			pontok();

		//S�g� megnyit�sa
		else if (atoi(s) == 4)
			sugo();

		//Program bez�r�sa
		else if (atoi(s) == 5) {
			setlocale(LC_ALL, "c");
			exit(0);
		}

	}
	else {
		printf("Sikertelen F�jlnyit�s!\n");
		getchar();
		exit(-1);
	}


}


int pontok() {
	FILE *fpontok = fopen("pontok.csv", "rt");
	char sor[SORHOSSZ];
	char *mezo;
	int i = 0;
	int db = 1;
	char s[2];

	system("cls");
	//pontok beolvas�sa a f�jlb�l
	if (fpontok != NULL) {
		while (fgets(sor, SORHOSSZ, fpontok)) {
			mezo = kovMezo(sor);
			while (mezo) {
				if ((db % 2) != 0) {
					strcpy(eredmenyek[i].nev, mezo);
					db++;
					mezo = kovMezo(NULL);
				}
				else {
					eredmenyek[i].pont = atoi(mezo);
					db++;
					i++;
					mezo = kovMezo(NULL);
				}
			}
		}
		fclose(fpontok);
		//pontok ki�r�sa
		printf("* * * * * * * * * * * * * * * *\n");
		printf("*                             *\n");
		printf("*           Pontok:           *\n");
		printf("*         -----------         *\n");
		printf("*                             *\n");

		for (i = 0; i < 10; i++) {
			if (eredmenyek[i].pont != 0) {
				if (eredmenyek[i].pont >= 10) {
					printf("*    %s        -        %d   *\n", eredmenyek[i].nev, eredmenyek[i].pont);
					printf("*                             *\n");
				}
				else{
					printf("*    %s        -        %d    *\n", eredmenyek[i].nev, eredmenyek[i].pont);
					printf("*                             *\n");
				}
			}
		}
		printf("*          0. Vissza          *\n");
		printf("*                             *\n");
		printf("* * * * * * * * * * * * * * * *\n");
		getline(s, 2);
		while (egesze(s) != 1 || atoi(s) != 0 || strlen(s) != 1) {
			printf("Helytelen karaktert adott meg!\n");
			getline(s, 2);
		}

		menu();
	}
	else printf("Sikertelen F�jlnyit�s!\n");
}

int sugo() {
	FILE *fsugo = fopen("sugo.txt", "rt");
	char sor[SORHOSSZ];
	char s[2];
	system("cls");
	//s�g� beolvas�sa a f�jlb�l �s ki�r�sa
	if (fsugo != NULL) {
		while (fgets(sor, SORHOSSZ, fsugo)) {
			printf("%s", sor);
		}
		fclose(fsugo);
		//visszal�p�s bek�r�se
		getline(s, 2);
		while (egesze(s) != 1 || atoi(s) != 0 || strlen(s) != 1) {
			printf("Helytelen karaktert adott meg!\n");
			getline(s, 2);
		}
		menu();
	}
	else printf("Sikertelen F�jlnyit�s!\n");
}

struct Kerdes *beszur(int nehezseg, char szoveg[], char a[], char b[],
	char c[], char d[], char valasz, char kategoria[], struct Kerdes *elozo) {
	struct Kerdes *uj;
	if (uj = (struct Kerdes *)malloc(sizeof(struct Kerdes))) {
		//adatok beilleszt�se
		uj->nehezseg = nehezseg;
		strcpy(uj->szoveg, szoveg);
		strcpy(uj->a, a);
		strcpy(uj->b, b);
		strcpy(uj->c, c);
		strcpy(uj->d, d);
		uj->valasz = valasz;
		strcpy(uj->kategoria, kategoria);

		if (elozo == NULL) {
			uj->index = 0;
		}
		else {
			uj->index = elozo->index;
			elozo->index++;
		}

		//lista logika
		if (elozo) {
			uj->kov = elozo->kov;
			elozo->kov = uj;
		}
		else {
			uj->kov = NULL;
		}
	}
	return uj;
}

void kerdesekBetoltese() {	//Kerdes* kerdesek[]
	char *p;
	char sor[MAX];
	int sordb = 0, tag;
	struct Kerdes *akt, *seged, *kezdopont;
	int i;

	FILE *f;

	//k�rd�sek bet�lt�se f�jlb�l
	if (!(f = fopen("loim.csv", "r"))) {
		printf("Hianyzo adatfajl!\n");
		getchar();
		exit(-1);
	}
	while (fgets(sor, MAX, f)) {
		sordb++;
		if (sordb != 1 && sor[0] != ';') {
			tag = 0;
			akt = (struct Kerdes *)malloc(sizeof(struct Kerdes));
			p = strtok(sor, ";");
			while (p) {
				tag++;
				switch (tag) {
				case 1:
					akt->nehezseg = atoi(p);
					break;
				case 2:
					strcpy(akt->szoveg, p);
					break;
				case 3:
					strcpy(akt->a, p);
					break;
				case 4:
					strcpy(akt->b, p);
					break;
				case 5:
					strcpy(akt->c, p);
					break;
				case 6:
					strcpy(akt->d, p);
					break;
				case 7:
					akt->valasz = p[0];
					break;
				case 8:
					strcpy(akt->kategoria, p);
					break;
				default:
					break;
				}
				p = strtok(NULL, ";");
			}


			//kerdesek[akt->nehezseg] : ez a kezd�pontja az adott neh�zs�gi szintnek
			if (kerdesek[akt->nehezseg] == NULL) {
				kerdesek[akt->nehezseg] = beszur(akt->nehezseg, akt->szoveg, akt->a, akt->b, akt->c, akt->d, akt->valasz, akt->kategoria, NULL);
			}
			else {
				beszur(akt->nehezseg, akt->szoveg, akt->a, akt->b, akt->c, akt->d, akt->valasz, akt->kategoria, kerdesek[akt->nehezseg]);
			}
		}
	}
}
//k�rd�s kisorsol�sa
struct Kerdes* sorsol(int nehezseg) {
	int maxRandom, idx;
	struct Kerdes *seged, *kezdopont;

	maxRandom = kerdesek[nehezseg]->index + 1;
	idx = rand() % (maxRandom);

	kezdopont = kerdesek[nehezseg];
	seged = kezdopont;
	while (seged) {
		if (seged->index == idx) {
			return seged;
		}
		seged = seged->kov;
	}

	return kezdopont;
}

//k�z�ns�g seg�t�g�nek a random sz�mgener�l�sa
double kozonseg(struct Kerdes* aktKerdes, double kozons[4]) {
	int alap = 70;
	int rossz1Suly = 60;
	int rossz2Suly = 50;
	int rossz3Suly = 70;
	int joSuly = 100;
	int osszeg, joHely, i;
	double egySulyHanySzazalek, r1, r2, r3, jo;
	double csere;

	int rndRossz1, rndRossz2, rndRossz3, rndJo;

	rndRossz1 = rand() % rossz1Suly + alap;
	rndRossz2 = rand() % rossz2Suly + alap;
	rndRossz3 = rand() % rossz3Suly + alap;
	rndJo = rand() % joSuly + alap*1.5;

	osszeg = rndRossz1 + rndRossz2 + rndRossz3 + rndJo;

	egySulyHanySzazalek = 100.0 / osszeg;	

	r1 = rndRossz1 * egySulyHanySzazalek;
	r2 = rndRossz2 * egySulyHanySzazalek;
	r3 = rndRossz3 * egySulyHanySzazalek;
	jo = rndJo * egySulyHanySzazalek;



	kozons[0] = r1;
	kozons[1] = r2;
	kozons[2] = r3;
	kozons[3] = jo;

	joHely = aktKerdes->valasz - 'A';

	csere = kozons[joHely];
	kozons[joHely] = kozons[3];
	kozons[3] = csere;

	return(*kozons);
}

//telefonos seg�ts�g random sz�mok gener�l�sa
double telefon(struct Kerdes* aktKerdes, double telefon[4]) {
	int alap = 70;
	int rossz1Suly = 40;
	int rossz2Suly = 30;
	int rossz3Suly = 50;
	int joSuly = 100;
	int osszeg, joHely, i;
	double egySulyHanySzazalek, r1, r2, r3, jo;
	double csere;

	int rndRossz1, rndRossz2, rndRossz3, rndJo;

	rndRossz1 = rand() % rossz1Suly + alap;
	rndRossz2 = rand() % rossz2Suly + alap;
	rndRossz3 = rand() % rossz3Suly + alap;
	rndJo = rand() % joSuly + alap*1.5;

	osszeg = rndRossz1 + rndRossz2 + rndRossz3 + rndJo;

	egySulyHanySzazalek = 100.0 / osszeg;

	r1 = rndRossz1 * egySulyHanySzazalek;
	r2 = rndRossz2 * egySulyHanySzazalek;
	r3 = rndRossz3 * egySulyHanySzazalek;
	jo = rndJo * egySulyHanySzazalek;


	telefon[0] = r1;
	telefon[1] = r2;
	telefon[2] = r3;
	telefon[3] = jo;

	joHely = aktKerdes->valasz - 'A';

	csere = telefon[joHely];
	telefon[joHely] = telefon[3];
	telefon[3] = csere;

	return(*telefon);
}

//felez�s random 2 rossz v�lasz kiv�laszt�sa
void  felezes(struct Kerdes* aktKerdes, char ret[]) {

	ret[0] = aktKerdes->valasz;
	do {
		ret[1] = rand() % 4 + 'A';
	} while (ret[0] == ret[1]);
	ret[2] = '\0';
}

//pontok bet�lt�se �s a j�t�kos �ltal el�rt pont behejez�se a t�bl�zatba
void pontrendez() {

	FILE *fpontok = fopen("pontok.csv", "w+t");
	char sor[SORHOSSZ];
	char *mezo;
	int i = 0, j;
	int db = 1;
	int bent = 0;
	int csp = 0, csk = 0;
	char csn[4];

	system("cls");

	if (fpontok != NULL) {
		while (fgets(sor, SORHOSSZ, fpontok)) {
			mezo = kovMezo(sor);
			while (mezo) {
				if ((db % 2) != 0) {
					strcpy(eredmenyek[i].nev, mezo);
					db++;
					mezo = kovMezo(NULL);
				}
				else {
					eredmenyek[i].pont = atoi(mezo);
					db++;
					i++;
					mezo = kovMezo(NULL);
				}
			}
		}
		fseek(fpontok, 0, SEEK_SET);
		while (eredmenyek[i].pont >= aktjatekos.pont) {
			i++;
		}
		csp = eredmenyek[i].pont;
		strcpy(csn, eredmenyek[i].nev);
		eredmenyek[i].pont = aktjatekos.pont;
		strcpy(eredmenyek[i].nev, aktjatekos.nev);

		for (j = 9; j < i - 1; j--) {
			eredmenyek[j].pont = eredmenyek[j - 1].pont;
			strcpy(eredmenyek[j].nev, eredmenyek[j - 1].nev);
		}
		if (i != 9) {
			eredmenyek[i + 1].pont = csp;
			strcpy(eredmenyek[i + 1].nev, csn);
		}
		for (i = 0; i < 10; i++) {
			fprintf(fpontok, "%s;%d\n", eredmenyek[i].nev, eredmenyek[i].pont);
		}


		fclose(fpontok);
	}
}

//gyakorl� m�d
int gyakorlas() {
	int nehezseg, feladatdb = 5, rndA, rndB, i = 1, j, maxnehezseg = 15;
	struct Kerdes *a[5];
	char s[1 + 1];
	int hiba = 1;
	int volt;
	int kov = 0;
	int akt;
	int joe;
	srand(time(NULL));

	kerdesekBetoltese();
	//kateg�ri�k random kiv�laszt�sa (5 db) �s azonos kateg�ri�k kisz�r�se �s �jrasorsol�sa
	while (i != 6) {
		volt = 0;
		nehezseg = rand() % maxnehezseg + 1;
		a[i] = sorsol(nehezseg);
		for (j = 1; j < i; j++) {
			if (strcmp(a[i]->kategoria, a[j]->kategoria) == 0) {
				volt = 1;
			}
		}
		if (volt == 0) {
			i++;
		}
	}

	//kateg�ri�k k�z�tti v�laszt�s inputtal, �s annak ellen�rz�se
	while (hiba == 1) {
		system("cls");
		printf("Gyakorl�s\n\n");
		printf("V�lasszon a t�mak�r�k k�z�l:\n");
		for (i = 1; i <= 5; i++) {
			printf("%d. %s\n", i, a[i]->kategoria);
		}
		printf("0. Men�\n\n");
		printf("Sz�m megad�s�val v�laszthat.\n");
		printf("Kateg�ria sz�ma: ");
		getline(s, 2);

		if (strlen(s) != 1 || s[0]<'0' || s[0]>'5') {
			printf("Helytelen karaktert adott meg.\n");
			_sleep(1500); //1.5 sec k�sleltet�s
		}
		else if (s[0] >= '1' && s[0] <= '5')
			hiba = 0;
		else if (s[0] == '0') {
			printf("Gyakolr�s v�ge.\n");
			printf("Visszat�r�s a men�be.\n");
			hiba = 0;
			menu();
		}
	}
	akt = atoi(s);
	//a kiv�lasztott kateg�ri�j� k�rd�s ki�r�sa �s a v�lasz bek�r�se
	while (kov != 1) {

		system("cls");
		printf("K�rd�s:\n");
		printf("\n--------\n%s\n", a[akt]->szoveg, a[akt]->index);
		printf("A: %s\n", a[akt]->a);
		printf("B: %s\n", a[akt]->b);
		printf("C: %s\n", a[akt]->c);
		printf("D: %s\n", a[akt]->d);

		printf("\n\n");

		printf("\nK�rem a v�laszt: ");
		getline(s, 2);
		if (strlen(s) == 1 && (toupper(s[0]) >= 'A' && toupper(s[0]) <= 'D')) {
			kov = 1;
			joe = 0;
			if (toupper(s[0]) == a[akt]->valasz) {
				joe = 1;
			}
			//visszat�r�s, �j kateg�ri�k sorsol�sa
			if (joe == 1) {
				printf("A v�lasz helyes.\n");
				_sleep(1500);
				gyakorlas();
			}
			else if (joe == 0) {
				printf("A v�lasz helytelen.\n");
				printf("%c volt a helyes v�lasz.\n", a[akt]->valasz);
				_sleep(3000);
				gyakorlas();
			}
		}
		else {
			printf("Helytelen karaktert adott meg.\n");
			_sleep(1500); //1.5 sec k�sleltet�s
		}
	}
}

int ujjatek() {
	char sor[SORHOSSZ + 1];
	char felezesvalasz[2];
	//struct Kerdes* kerdesek[MAX_NEHEZSEG+1];		//0. elem nincs, mert nincs 0. neh�zs�gi szint
	struct Kerdes* seged, *aktKerdes;
	int i = 1, j, hossz, joe;
	int kozsegitseg = 1;
	int telsegitseg = 1;
	int felsegitseg = 1;
	int kov;
	int hiba = 1;
	double kozons[4], tel[4];


	srand(time(NULL));

	kerdesekBetoltese();


	//1 - 15 k�rd�sig l�ptet�s(ha helyes a v�lasz)
	for (i = 1; i <= MAX_NEHEZSEG; i++) {
		//seg�ts�geket csak egyszer lehessen felhaszn�lni a j�t�k alatt
		if (kozsegitseg == 0) kozsegitseg = -1;

		if (telsegitseg == 0) telsegitseg = -1;

		if (felsegitseg == 0) felsegitseg = -1;

		aktKerdes = sorsol(i);

		kov = 0;
		while (kov != 1) {
			//k�rd�s ki�r�sa
			system("cls");
			printf("%d. K�rd�s:", i);
			printf("\n--------\n%s\n", aktKerdes->szoveg);
			printf("A: %s\n", aktKerdes->a);
			printf("B: %s\n", aktKerdes->b);
			printf("C: %s\n", aktKerdes->c);
			printf("D: %s\n", aktKerdes->d);

			printf("\n\n");
			//felez�s eset�n ki�r�s
			if (felsegitseg == 0) {
				system("cls");
				printf("%d. K�rd�s:\n", i);
				printf("\nFelez�s ut�n:");
				printf("\n--------\n%s\n", aktKerdes->szoveg);
				if (felezesvalasz[0] == 'A' || felezesvalasz[1] == 'A') {
					printf("A: %s\n", aktKerdes->a);
				}
				if (felezesvalasz[0] == 'B' || felezesvalasz[1] == 'B') {
					printf("B: %s\n", aktKerdes->b);
				}
				if (felezesvalasz[0] == 'C' || felezesvalasz[1] == 'C') {
					printf("C: %s\n", aktKerdes->c);
				}
				if (felezesvalasz[0] == 'D' || felezesvalasz[1] == 'D') {
					printf("D: %s\n", aktKerdes->d);
				}

				printf("\n");
			}
			//a m�g haszn�lhat� seg�ts�gek
			if (kozsegitseg == 1)
				printf("K�z�ns�g seg�ts�g: K\n\n");
			if (telsegitseg == 1)
				printf("Telefonos seg�ts�g: T\n\n");
			if (felsegitseg == 1)
				printf("Felez�s: F\n\n");
			//k�z�s�g seg�ts�g�nek megjelen�t�se
			if (kozsegitseg == 0) {
				printf("K�zns�g seg�ts�ge:\n");
				for (j = 0; j < 4; j++) {
					printf("%c: %3.2f\n", j + 'A', kozons[j]);
				}
				printf("\n");
			}
			//telefonos seg�ts�g megjelen�t�se
			if (telsegitseg == 0) {
				printf("Telefon�l� seg�ts�ge:\n");
				for (j = 0; j < 4; j++) {
					printf("%c: %3.2f\n", j + 'A', tel[j]);
				}
				printf("\n");
			}
			//v�lasz k�r�se
			printf("\nK�rem a v�laszt: ");
			hossz = getline(sor, MAX);
			//k�z�ns�g seg�ts�g (haszn�lhat�-e)
			if (hossz == 1 && toupper(sor[0]) == 'K' && kozsegitseg == 1 && (telsegitseg == 0 || felsegitseg == 0)){
				printf("Egy k�r�sben csak egy seg�ts�get haszn�lhat.\n");
				_sleep(1500); //1.5 sec k�sleltet�s
			}

			else if (hossz == 1 && toupper(sor[0]) == 'K' && kozsegitseg == 1) {
				kozonseg(aktKerdes, kozons);
				kozsegitseg = 0;
			}

			else if (hossz == 1 && toupper(sor[0]) == 'K' && kozsegitseg != 1) {
				printf("A k�z�ns�g seg�ts�g�t m�r haszn�lta.\n");
				_sleep(1500); //1.5 sec k�sleltet�s
			}
			//telefonos seg�ts�g (haszn�lhat�-e)
			else if (hossz == 1 && toupper(sor[0]) == 'T' && telsegitseg == 1 && (kozsegitseg == 0 || felsegitseg == 0)) {
				printf("Egy k�r�sben csak egy seg�ts�get haszn�lhat.\n");
				_sleep(1500); //1.5 sec k�sleltet�s
			}
			else if (hossz == 1 && toupper(sor[0]) == 'T' && telsegitseg == 1) {
				telefon(aktKerdes, tel);
				telsegitseg = 0;

			}

			else if (hossz == 1 && toupper(sor[0]) == 'T' && telsegitseg != 1) {
				printf("A telefonos seg�ts�get m�r haszn�ltad.\n");
				_sleep(1500); //1.5 sec k�sleltet�s
			}
			//felezes (haszn�lhat�-e)
			else if (hossz == 1 && toupper(sor[0]) == 'F' && felsegitseg == 1 && (kozsegitseg == 0 || telsegitseg == 0)) {
				printf("Egy k�r�sben csak egy seg�ts�get haszn�lhat.\n");
				_sleep(1500); //1.5 sec k�sleltet�s
			}
			else if (hossz == 1 && toupper(sor[0]) == 'F' && felsegitseg == 1) {
				felezes(aktKerdes, felezesvalasz);
				felsegitseg = 0;
			}

			else if (hossz == 1 && toupper(sor[0]) == 'F' && felsegitseg != 1) {
				printf("A felez�s seg�ts�get m�r haszn�ltad.\n");
				_sleep(1500); //1.5 sec k�sleltet�s
			}

			//valaszadas (ha megadott karakter A-D ig van)
			else if (hossz == 1 && felsegitseg == 0 && (!toupper(sor[0]) == felezesvalasz[0] && !toupper(sor[0]) == felezesvalasz[1])) {
				printf("Helytelen karakter adott meg.\n");
			}
			else if (hossz == 1 && (toupper(sor[0]) >= 'A' && toupper(sor[0]) <= 'D')) {

				kov = 1;
				joe = 0;
				//helyes v�lasz eset�n
				if (toupper(sor[0]) == aktKerdes->valasz) {
					joe = 1;
				}
				if (joe == 1) {
					printf("A v�lasz helyes.\n");
				}
				// 0 helyes v�lasz eset�n
				else if (joe == 0 && i == 1) {
					printf("A v�lasz helytelen.\n");
					printf("%c volt a helyes v�lasz.\n", aktKerdes->valasz);
					printf("Egy k�d�st sem tudt�l megv�laszolni, �gy nem ker�lsz fel a pont�bl�zatra.\n");
					_sleep(3000);
					menu();
				}
				// helytelen v�lasz (de egy k�rd�st siker�lt megv�laszolni) + j�t�kos nev�nek bek�r�se
				else {
					printf("A v�lasz helytelen.\n");
					printf("%c volt a helyes v�lasz.\n", aktKerdes->valasz);
					_sleep(1500);
					system("cls");
					printf("A j�t�k v�ge.\n");
					while (hiba == 1) {
						if (nev() == 0) {
							printf("Hib�s nevet adott meg!\n");
							_sleep(1500);
						}
						else hiba = 0;
					}
					aktjatekos.pont = i - 1;
					pontrendez();
					pontok();

				}
				_sleep(2000); // a v�lasz ut�ni v�rakoz�s most 2 sec
			}
			else {
				printf("Helytelen karakter adott meg.\n");
				_sleep(1500); //1.5 sec k�sleltet�s
			}
		}
	}
	//15 k�rd�s teljes�t�se + j�t�kos nev�nek bek�r�se
	aktjatekos.pont = i - 1;
	while (hiba == 1) {
		if (nev() == 0) {
			printf("Hib�s nevet adott meg!\n");
			_sleep(1500);
		}
		else hiba = 0;
	}
	pontrendez();
	pontok();
}
//a j�t�kos nev�t bek�r� �s ellen�rz� f�ggv�ny
int nev() {
	int i;

	system("cls");
	printf("Kerem adja meg nev�t,hogy felker�lhessen az eredm�nyt�bl�zatra.\n(3 karater� legyen)\nNeve:");
	getline(aktjatekos.nev, 3 + 1);

	if (strlen(aktjatekos.nev) != 3) return 0;

	else
		for (i = 0; i < 3; i++) {
			if (aktjatekos.nev[i]<'A' || aktjatekos.nev[i]>'z') return 0;
			else if (aktjatekos.nev[i]>'Z' && aktjatekos.nev[i] < 'a') return 0;
			aktjatekos.nev[i] = toupper(aktjatekos.nev[i]);
		}
	return 1;

}



void main() {
	FILE *fintro;
	char sor[SORHOSSZ + 1];
	FILE *fpontok = fopen("pontok.csv", "rt");
	char *mezo;
	int i = 0;
	int db = 1;
	//magyar �kezetes megjelen�t�s
	setlocale(LC_ALL, "");
	//a pontok bet�lt�se
	if (fpontok != NULL) {
		while (fgets(sor, SORHOSSZ, fpontok)) {
			mezo = kovMezo(sor);
			while (mezo) {
				if ((db % 2) != 0) {
					strcpy(eredmenyek[i].nev, mezo);
					db++;
					mezo = kovMezo(NULL);
				}
				else {
					eredmenyek[i].pont = atoi(mezo);
					db++;
					i++;
					mezo = kovMezo(NULL);
				}
			}
		}
		fclose(fpontok);
	}
	else if (fpontok == NULL) {
		printf("Sikertelen F�jlnyit�s!\n");
		getchar();
		exit(-1);
	}
	//Kezd�k�perny� ("t�lt�k�perny�")
	fintro = fopen("intro.txt", "rt");
	if (fintro != NULL) {
		while (fgets(sor, SORHOSSZ, fintro)) {
			printf("%s", sor);
		}
		fclose(fintro);
		//3 sec V�rakoz�s
		_sleep(3000);
		menu();
	}
	else {
		printf("Sikertelen F�jlnyit�s!\n");
		getchar();
		exit(-1);
	}
}