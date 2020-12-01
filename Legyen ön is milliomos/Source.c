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

//aktuális és a már a pontáblázatban lévõ játékosok neve, pontja
typedef struct adat {
	char nev[3 + 1];
	int pont;
} adat;
//játékosok pontja és neve folyamatos tárolása
adat aktjatekos;
adat eredmenyek[10];


//Játék menüje:
void menu() {
	FILE *fmenu = fopen("menu.txt", "rt");
	char sor[SORHOSSZ + 1];
	char s[2];
	system("cls");

	//Menü megjelenítése
	if (fmenu != NULL) {
		while (fgets(sor, SORHOSSZ, fmenu)) {
			printf("%s", sor);
		}
		fclose(fmenu);
		//Menüpont választása
		getline(s, 2);
		if (egesze(s) != 1 || atoi(s) < 1 || atoi(s) > 5 || strlen(s) != 1) {
			printf("Helytelen karaktert adott meg!\n");
			_sleep(1500);
			menu();
		}

		//Új játék indítása
		if (atoi(s) == 1)
			ujjatek();

		//Gyakorló játék indítása
		else if (atoi(s) == 2)
			gyakorlas();

		//Elért pontok megtekintése
		else if (atoi(s) == 3)
			pontok();

		//Súgó megnyitása
		else if (atoi(s) == 4)
			sugo();

		//Program bezárása
		else if (atoi(s) == 5) {
			setlocale(LC_ALL, "c");
			exit(0);
		}

	}
	else {
		printf("Sikertelen Fájlnyitás!\n");
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
	//pontok beolvasása a fájlból
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
		//pontok kiírása
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
	else printf("Sikertelen Fájlnyitás!\n");
}

int sugo() {
	FILE *fsugo = fopen("sugo.txt", "rt");
	char sor[SORHOSSZ];
	char s[2];
	system("cls");
	//súgó beolvasása a fájlból és kiírása
	if (fsugo != NULL) {
		while (fgets(sor, SORHOSSZ, fsugo)) {
			printf("%s", sor);
		}
		fclose(fsugo);
		//visszalépés bekérése
		getline(s, 2);
		while (egesze(s) != 1 || atoi(s) != 0 || strlen(s) != 1) {
			printf("Helytelen karaktert adott meg!\n");
			getline(s, 2);
		}
		menu();
	}
	else printf("Sikertelen Fájlnyitás!\n");
}

struct Kerdes *beszur(int nehezseg, char szoveg[], char a[], char b[],
	char c[], char d[], char valasz, char kategoria[], struct Kerdes *elozo) {
	struct Kerdes *uj;
	if (uj = (struct Kerdes *)malloc(sizeof(struct Kerdes))) {
		//adatok beillesztése
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

	//kérdések betõltése fájlból
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


			//kerdesek[akt->nehezseg] : ez a kezdõpontja az adott nehézségi szintnek
			if (kerdesek[akt->nehezseg] == NULL) {
				kerdesek[akt->nehezseg] = beszur(akt->nehezseg, akt->szoveg, akt->a, akt->b, akt->c, akt->d, akt->valasz, akt->kategoria, NULL);
			}
			else {
				beszur(akt->nehezseg, akt->szoveg, akt->a, akt->b, akt->c, akt->d, akt->valasz, akt->kategoria, kerdesek[akt->nehezseg]);
			}
		}
	}
}
//kérdés kisorsolása
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

//közönség segítégének a random számgenerálása
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

//telefonos segítség random számok generálása
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

//felezés random 2 rossz válasz kiválasztása
void  felezes(struct Kerdes* aktKerdes, char ret[]) {

	ret[0] = aktKerdes->valasz;
	do {
		ret[1] = rand() % 4 + 'A';
	} while (ret[0] == ret[1]);
	ret[2] = '\0';
}

//pontok betõltése és a játékos által elért pont behejezése a táblázatba
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

//gyakorló mód
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
	//kategóriák random kiválasztása (5 db) és azonos kategóriák kiszûrése és újrasorsolása
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

	//kategóriák közötti választás inputtal, és annak ellenõrzése
	while (hiba == 1) {
		system("cls");
		printf("Gyakorlás\n\n");
		printf("Válasszon a témakörök közül:\n");
		for (i = 1; i <= 5; i++) {
			printf("%d. %s\n", i, a[i]->kategoria);
		}
		printf("0. Menü\n\n");
		printf("Szám megadásával választhat.\n");
		printf("Kategória száma: ");
		getline(s, 2);

		if (strlen(s) != 1 || s[0]<'0' || s[0]>'5') {
			printf("Helytelen karaktert adott meg.\n");
			_sleep(1500); //1.5 sec késleltetés
		}
		else if (s[0] >= '1' && s[0] <= '5')
			hiba = 0;
		else if (s[0] == '0') {
			printf("Gyakolrás vége.\n");
			printf("Visszatérés a menübe.\n");
			hiba = 0;
			menu();
		}
	}
	akt = atoi(s);
	//a kiválasztott kategóriájú kérdés kiírása és a válasz bekérése
	while (kov != 1) {

		system("cls");
		printf("Kérdés:\n");
		printf("\n--------\n%s\n", a[akt]->szoveg, a[akt]->index);
		printf("A: %s\n", a[akt]->a);
		printf("B: %s\n", a[akt]->b);
		printf("C: %s\n", a[akt]->c);
		printf("D: %s\n", a[akt]->d);

		printf("\n\n");

		printf("\nKérem a választ: ");
		getline(s, 2);
		if (strlen(s) == 1 && (toupper(s[0]) >= 'A' && toupper(s[0]) <= 'D')) {
			kov = 1;
			joe = 0;
			if (toupper(s[0]) == a[akt]->valasz) {
				joe = 1;
			}
			//visszatérés, új kategóriák sorsolása
			if (joe == 1) {
				printf("A válasz helyes.\n");
				_sleep(1500);
				gyakorlas();
			}
			else if (joe == 0) {
				printf("A válasz helytelen.\n");
				printf("%c volt a helyes válasz.\n", a[akt]->valasz);
				_sleep(3000);
				gyakorlas();
			}
		}
		else {
			printf("Helytelen karaktert adott meg.\n");
			_sleep(1500); //1.5 sec késleltetés
		}
	}
}

int ujjatek() {
	char sor[SORHOSSZ + 1];
	char felezesvalasz[2];
	//struct Kerdes* kerdesek[MAX_NEHEZSEG+1];		//0. elem nincs, mert nincs 0. nehézségi szint
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


	//1 - 15 kérdésig léptetés(ha helyes a válasz)
	for (i = 1; i <= MAX_NEHEZSEG; i++) {
		//segítségeket csak egyszer lehessen felhasználni a játék alatt
		if (kozsegitseg == 0) kozsegitseg = -1;

		if (telsegitseg == 0) telsegitseg = -1;

		if (felsegitseg == 0) felsegitseg = -1;

		aktKerdes = sorsol(i);

		kov = 0;
		while (kov != 1) {
			//kérdés kiírása
			system("cls");
			printf("%d. Kérdés:", i);
			printf("\n--------\n%s\n", aktKerdes->szoveg);
			printf("A: %s\n", aktKerdes->a);
			printf("B: %s\n", aktKerdes->b);
			printf("C: %s\n", aktKerdes->c);
			printf("D: %s\n", aktKerdes->d);

			printf("\n\n");
			//felezés esetén kiírás
			if (felsegitseg == 0) {
				system("cls");
				printf("%d. Kérdés:\n", i);
				printf("\nFelezés után:");
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
			//a még használható segítségek
			if (kozsegitseg == 1)
				printf("Közönség segítség: K\n\n");
			if (telsegitseg == 1)
				printf("Telefonos segítség: T\n\n");
			if (felsegitseg == 1)
				printf("Felezés: F\n\n");
			//közöség segítségének megjelenítése
			if (kozsegitseg == 0) {
				printf("Köznség segítsége:\n");
				for (j = 0; j < 4; j++) {
					printf("%c: %3.2f\n", j + 'A', kozons[j]);
				}
				printf("\n");
			}
			//telefonos segítség megjelenítése
			if (telsegitseg == 0) {
				printf("Telefonáló segítsége:\n");
				for (j = 0; j < 4; j++) {
					printf("%c: %3.2f\n", j + 'A', tel[j]);
				}
				printf("\n");
			}
			//válasz kérése
			printf("\nKérem a választ: ");
			hossz = getline(sor, MAX);
			//közönség segítség (használható-e)
			if (hossz == 1 && toupper(sor[0]) == 'K' && kozsegitseg == 1 && (telsegitseg == 0 || felsegitseg == 0)){
				printf("Egy kérésben csak egy segítséget használhat.\n");
				_sleep(1500); //1.5 sec késleltetés
			}

			else if (hossz == 1 && toupper(sor[0]) == 'K' && kozsegitseg == 1) {
				kozonseg(aktKerdes, kozons);
				kozsegitseg = 0;
			}

			else if (hossz == 1 && toupper(sor[0]) == 'K' && kozsegitseg != 1) {
				printf("A közönség segítségét már használta.\n");
				_sleep(1500); //1.5 sec késleltetés
			}
			//telefonos segítség (használható-e)
			else if (hossz == 1 && toupper(sor[0]) == 'T' && telsegitseg == 1 && (kozsegitseg == 0 || felsegitseg == 0)) {
				printf("Egy kérésben csak egy segítséget használhat.\n");
				_sleep(1500); //1.5 sec késleltetés
			}
			else if (hossz == 1 && toupper(sor[0]) == 'T' && telsegitseg == 1) {
				telefon(aktKerdes, tel);
				telsegitseg = 0;

			}

			else if (hossz == 1 && toupper(sor[0]) == 'T' && telsegitseg != 1) {
				printf("A telefonos segítséget már használtad.\n");
				_sleep(1500); //1.5 sec késleltetés
			}
			//felezes (használható-e)
			else if (hossz == 1 && toupper(sor[0]) == 'F' && felsegitseg == 1 && (kozsegitseg == 0 || telsegitseg == 0)) {
				printf("Egy kérésben csak egy segítséget használhat.\n");
				_sleep(1500); //1.5 sec késleltetés
			}
			else if (hossz == 1 && toupper(sor[0]) == 'F' && felsegitseg == 1) {
				felezes(aktKerdes, felezesvalasz);
				felsegitseg = 0;
			}

			else if (hossz == 1 && toupper(sor[0]) == 'F' && felsegitseg != 1) {
				printf("A felezés segítséget már használtad.\n");
				_sleep(1500); //1.5 sec késleltetés
			}

			//valaszadas (ha megadott karakter A-D ig van)
			else if (hossz == 1 && felsegitseg == 0 && (!toupper(sor[0]) == felezesvalasz[0] && !toupper(sor[0]) == felezesvalasz[1])) {
				printf("Helytelen karakter adott meg.\n");
			}
			else if (hossz == 1 && (toupper(sor[0]) >= 'A' && toupper(sor[0]) <= 'D')) {

				kov = 1;
				joe = 0;
				//helyes válasz esetén
				if (toupper(sor[0]) == aktKerdes->valasz) {
					joe = 1;
				}
				if (joe == 1) {
					printf("A válasz helyes.\n");
				}
				// 0 helyes válasz esetén
				else if (joe == 0 && i == 1) {
					printf("A válasz helytelen.\n");
					printf("%c volt a helyes válasz.\n", aktKerdes->valasz);
					printf("Egy kédést sem tudtál megválaszolni, így nem kerülsz fel a pontáblázatra.\n");
					_sleep(3000);
					menu();
				}
				// helytelen válasz (de egy kérdést sikerült megválaszolni) + játékos nevének bekérése
				else {
					printf("A válasz helytelen.\n");
					printf("%c volt a helyes válasz.\n", aktKerdes->valasz);
					_sleep(1500);
					system("cls");
					printf("A játék vége.\n");
					while (hiba == 1) {
						if (nev() == 0) {
							printf("Hibás nevet adott meg!\n");
							_sleep(1500);
						}
						else hiba = 0;
					}
					aktjatekos.pont = i - 1;
					pontrendez();
					pontok();

				}
				_sleep(2000); // a válasz utáni várakozás most 2 sec
			}
			else {
				printf("Helytelen karakter adott meg.\n");
				_sleep(1500); //1.5 sec késleltetés
			}
		}
	}
	//15 kérdés teljesítése + játékos nevének bekérése
	aktjatekos.pont = i - 1;
	while (hiba == 1) {
		if (nev() == 0) {
			printf("Hibás nevet adott meg!\n");
			_sleep(1500);
		}
		else hiba = 0;
	}
	pontrendez();
	pontok();
}
//a játékos nevét bekérõ és ellenõrzõ függvény
int nev() {
	int i;

	system("cls");
	printf("Kerem adja meg nevét,hogy felkerülhessen az eredménytáblázatra.\n(3 karaterû legyen)\nNeve:");
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
	//magyar ékezetes megjelenítés
	setlocale(LC_ALL, "");
	//a pontok betõltése
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
		printf("Sikertelen Fájlnyitás!\n");
		getchar();
		exit(-1);
	}
	//Kezdõképernyõ ("töltõképernyõ")
	fintro = fopen("intro.txt", "rt");
	if (fintro != NULL) {
		while (fgets(sor, SORHOSSZ, fintro)) {
			printf("%s", sor);
		}
		fclose(fintro);
		//3 sec Várakozás
		_sleep(3000);
		menu();
	}
	else {
		printf("Sikertelen Fájlnyitás!\n");
		getchar();
		exit(-1);
	}
}