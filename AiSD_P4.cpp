#include <iostream>
#include <string>

using namespace std;

enum tryby {
	PRINT,
	SAVE
};

enum kierunki {
	LEWY,
	PRAWY,
	NONE
};

#define CYFRY_ASCII 48

class bWezel {
public:
	bool jestLisciem;
	int* klucze;
	bWezel** dzieci;
	int iloscKluczy;
	//pozycja na dysku

	bWezel(int t) {
		jestLisciem = true;
		klucze = new int[2 * t - 1];
		//for (int i = 0; i < t; i++) klucze[i] = 0;
		dzieci = new bWezel * [2 * t];
		for (int i = 0; i < 2 * t; i++) dzieci[i] = nullptr;
		iloscKluczy = 0;
	}

	~bWezel() {
		delete[] klucze;
		for (int i = 0; i <= iloscKluczy; i++) if (dzieci[i] != nullptr) delete dzieci[i];
		delete[] dzieci;
	}

	bool zawieraKlucz(int k) {
		for (int i = 0; i < iloscKluczy; i++) {
			if (klucze[i] == k) return true;
		}
		return false;
	}

	bWezel* dzieckoZKluczem(int k) {
		int i;
		for (i = 0; i < iloscKluczy; i++) {
			if (klucze[i] > k) {
				return dzieci[i];
			}
		}
		return dzieci[i];
	}
};

class bTree {
public:
	bWezel* root;
	int T;

	bTree(int t) {
		T = t;
		root = new bWezel(T);
	}

	bTree(string s, int t) {
		T = t;
		int i, nawiasy, last, liscie;
		i = nawiasy = liscie = 0;
		int l = s.length();

		for (int j = 2; j < l; j += 2) { //celowe pominiecie pierwszego nawiasu
			if (s[j] == '(') liscie++;
			else if (s[j] == ')') break;
		}

		for (int j = 2; j < l; j += 2) { //wyszukanie najmniejszego klucza
			if (s[j] - CYFRY_ASCII >= 0 && s[j] - CYFRY_ASCII <= 9) {
				last = wczytajLiczbe(s, l, j);
				break;
			}
		}

		root = new bWezel(T);
		if (liscie > 0) root->jestLisciem = false;
		wczytaj(root, s, l, i, nawiasy, liscie, last);
		if (nawiasy != -1) przerwij();
	}

	~bTree() {
		delete root;
	}

	int wczytajLiczbe(string s, int length, int& i) {
		int liczba = 0;
		int potega = 1;

		for (int j = i + 1; j < length; j++) {
			if (s[j] - CYFRY_ASCII >= 0 && s[j] - CYFRY_ASCII <= 9) potega *= 10;
			else if (s[j] == ' ') break;
			else przerwij();
		}

		for (; potega > 0; i++) {
			liczba += potega * (s[i] - CYFRY_ASCII);
			potega /= 10;
		}
		i--;
		return liczba;
	}

	void przerwij() {
		cout << "kontrolowane przerwanie";
		if (root != nullptr) delete root;
		delete this;
		exit(EXIT_SUCCESS);
	}

	void wczytaj(bWezel* curr, string s, int length, int& i, int& otwarteNawiasy, int nawiasyLisciaste, int& lastAdded) {
		if (s[i] != '(') {
			przerwij();
		}
		i += 2;
		for (; i < length; i += 2) {
			if (s[i] == '(') {
				if (curr->jestLisciem == true || curr->dzieci[curr->iloscKluczy] != nullptr) przerwij();
				otwarteNawiasy++;
				curr->dzieci[curr->iloscKluczy] = new bWezel(T);

				if (otwarteNawiasy != nawiasyLisciaste) curr->dzieci[curr->iloscKluczy]->jestLisciem = false;
				else if (otwarteNawiasy > nawiasyLisciaste) przerwij();
				wczytaj(curr->dzieci[curr->iloscKluczy], s, length, i, otwarteNawiasy, nawiasyLisciaste, lastAdded);
			}
			else if (s[i] == ')') {
				otwarteNawiasy--;
				return;
			}
			else if (s[i] - CYFRY_ASCII >= 0 && s[i] - CYFRY_ASCII <= 9) {
				if (curr->iloscKluczy == 2 * T - 1) przerwij();
				else {
					int liczba = wczytajLiczbe(s, length, i);
					if (liczba >= lastAdded) curr->klucze[curr->iloscKluczy++] = liczba;
					else przerwij();
					lastAdded = liczba;
				}
			}
			else przerwij();
		}
	}

	bWezel* findKey(bWezel* w, int k) {
		if (w->zawieraKlucz(k) == true) {
			return w;
		}
		else if (w->jestLisciem == true) return nullptr;
		else return findKey(w->dzieckoZKluczem(k), k);
	}

	bWezel* findParent(bWezel* curr, bWezel* w, int k) {
		if (w == root || curr->jestLisciem == true) return nullptr;
		for (int i = 0; i < curr->iloscKluczy; i++) {
			if (curr->klucze[i] >= k) {
				if (curr->dzieci[i] == w) return curr;
				else {
					bWezel* potencjalny = findParent(curr->dzieci[i], w, k);
					if (potencjalny != nullptr) return potencjalny;
					else if (curr->klucze[i] > k) return nullptr;
				}
			}
		}
		if (curr->dzieci[curr->iloscKluczy] == w) return curr;
		else return findParent(curr->dzieci[curr->iloscKluczy], w, k);
	}

	void getSasiadow(bWezel* parent, bWezel* w, bWezel* &lewy, bWezel* &prawy) {
		for (int i = 0; i <= parent->iloscKluczy; i++) {
			prawy = lewy = nullptr;
			if (parent->dzieci[i] == w) {
				if (i > 0) lewy = parent->dzieci[i - 1];
				if (i < parent->iloscKluczy) prawy = parent->dzieci[i + 1];
				return;
			}
		}
	}

	bWezel* findToPush(bWezel* w, int k) {
		if (w->jestLisciem == false) {
			for (int i = 0; i <= w->iloscKluczy; i++) {
				if (i == w->iloscKluczy || w->klucze[i] >= k) return findToPush(w->dzieci[i], k);
			}
		}
		else {
			if (w->iloscKluczy == 2 * T - 1) {
				bWezel* parent = findParent(root, w, w->klucze[0]);
				podziel(parent, w);
				return findToPush(root, k);
			}
			else return w;
		}
		return nullptr;
	}

	bWezel* findToPop(bWezel* w, int x) {
		if (w->iloscKluczy == T - 1 && w != root) {
			return findToPop(powieksz(w), x);
			/*powieksz(w);
			return findToPop(root, x);*/
		}
		else if (w->zawieraKlucz(x) == true) return w;
		else if (w->jestLisciem == false) {
			for (int i = 0; i <= w->iloscKluczy; i++) {
				if (i == w->iloscKluczy || w->klucze[i] >= x) return findToPop(w->dzieci[i], x);
			}
		}
		return nullptr;
	}

	//dzielony = i-te dziecko parenta
	bWezel* podziel(bWezel* parent, bWezel* dzielony) {
		bWezel* nowy = new bWezel(T);
		int i = 0;
		if (parent == nullptr) { //dzielenie korzenia
			parent = new bWezel(T);
			parent->jestLisciem = false;
			root = parent;
			root->dzieci[0] = dzielony;
		}
		else if (parent->iloscKluczy == 2 * T - 1) {
			bWezel* potencjalnyRodzic = podziel(findParent(root, parent, parent->klucze[0]), parent);
			for (int j = 0; j <= potencjalnyRodzic->iloscKluczy; j++) if (potencjalnyRodzic->dzieci[j] == dzielony) parent = potencjalnyRodzic;
		}

		while (parent->dzieci[i] != dzielony) i++;
		nowy->jestLisciem = dzielony->jestLisciem;
		nowy->iloscKluczy = dzielony->iloscKluczy = T - 1;

		for (int j = parent->iloscKluczy; j > i; j--)
			parent->klucze[j] = parent->klucze[j - 1];
		for (int j = parent->iloscKluczy + 1; j > i + 1; j--)
			parent->dzieci[j] = parent->dzieci[j - 1];
		parent->klucze[i] = dzielony->klucze[T - 1];
		parent->dzieci[i + 1] = nowy;
		parent->iloscKluczy++;

		for (int j = 0; j < T - 1; j++) {
			nowy->klucze[j] = dzielony->klucze[T + j];
		}
		for (int j = 0; j < T; j++) {
			nowy->dzieci[j] = dzielony->dzieci[T + j];
			dzielony->dzieci[T + j] = nullptr;
		}
		return nowy;
	}

	bWezel* powieksz(bWezel* w) {
		bWezel* parent = findParent(root, w, w->klucze[0]);
		if (parent == nullptr) przerwij();
		bWezel* sasiedzi[2];
		getSasiadow(parent, w, sasiedzi[LEWY], sasiedzi[PRAWY]);
		if (sasiedzi[LEWY] == nullptr && sasiedzi[PRAWY] == nullptr) przerwij();
		else if (sasiedzi[LEWY] != nullptr && sasiedzi[LEWY]->iloscKluczy > T - 1) {
			pozycz(w, sasiedzi[LEWY], parent);
			return w;
		}
		else if (sasiedzi[PRAWY] != nullptr && sasiedzi[PRAWY]->iloscKluczy > T - 1) {
			pozycz(w, sasiedzi[PRAWY], parent);
			return w;
		}
		else if (sasiedzi[LEWY] != nullptr)
			return merge(w, sasiedzi[LEWY], parent);
		else return merge(w, sasiedzi[PRAWY], parent);
		return nullptr;
	}

	bWezel* merge(bWezel* jeden, bWezel* drugi, bWezel* parent) {
		if (jeden->iloscKluczy != T - 1 || drugi->iloscKluczy != T - 1 || parent == nullptr) przerwij();
		if (parent != root && parent->iloscKluczy == T - 1) powieksz(parent);

		bWezel* zmergowany = new bWezel(T);
		bWezel* lewy;
		bWezel* prawy;
		int indeksLewego;
		indeksLewego = 0;
		while (parent->dzieci[indeksLewego] != jeden && parent->dzieci[indeksLewego] != drugi) indeksLewego++;
		if (parent->dzieci[indeksLewego] == jeden) {
			lewy = jeden;
			prawy = drugi;
		}
		else {
			lewy = drugi;
			prawy = jeden;
		}
		zmergowany->jestLisciem = jeden->jestLisciem;
		for (int j = 0; j < lewy->iloscKluczy; j++) {
			zmergowany->dzieci[zmergowany->iloscKluczy] = lewy->dzieci[j];
			lewy->dzieci[j] = nullptr;
			zmergowany->klucze[zmergowany->iloscKluczy++] = lewy->klucze[j];
		}
		zmergowany->dzieci[zmergowany->iloscKluczy] = lewy->dzieci[lewy->iloscKluczy];
		lewy->dzieci[lewy->iloscKluczy] = nullptr;
		zmergowany->klucze[zmergowany->iloscKluczy++] = parent->klucze[indeksLewego];
		for (int j = 0; j < prawy->iloscKluczy; j++) {
			zmergowany->dzieci[zmergowany->iloscKluczy] = prawy->dzieci[j];
			prawy->dzieci[j] = nullptr;
			zmergowany->klucze[zmergowany->iloscKluczy++] = prawy->klucze[j];
		}
		zmergowany->dzieci[zmergowany->iloscKluczy] = prawy->dzieci[prawy->iloscKluczy];
		prawy->dzieci[prawy->iloscKluczy] = nullptr;
		if (parent == root && parent->iloscKluczy == 1) {
			root = zmergowany;
			delete parent;
		}
		else {
			parent->dzieci[indeksLewego] = zmergowany;
			parent->klucze[indeksLewego] = parent->klucze[indeksLewego + 1];
			parent->iloscKluczy--;
			for (int j = indeksLewego + 1; j < parent->iloscKluczy; j++) {
				parent->dzieci[j] = parent->dzieci[j + 1];
				parent->klucze[j] = parent->klucze[j + 1];
			}
			parent->dzieci[parent->iloscKluczy] = parent->dzieci[parent->iloscKluczy + 1];
			parent->dzieci[parent->iloscKluczy + 1] = nullptr;
			delete lewy;
			delete prawy;
		}
		return zmergowany;
	}

	void pozycz(bWezel* pozyczajacy, bWezel* dawca, bWezel* parent) {
		if (pozyczajacy->iloscKluczy != T - 1 || dawca->iloscKluczy == T - 1 || parent == nullptr) przerwij();
		int indeksPozyczajacego = 0;
		int indeksDawcy = 0;
		while (indeksPozyczajacego < 2 * T && parent->dzieci[indeksPozyczajacego] != pozyczajacy && parent->dzieci[indeksPozyczajacego] != nullptr) indeksPozyczajacego++;
		if (parent->dzieci[indeksPozyczajacego] == nullptr) przerwij();
		while (indeksDawcy < 2 * T && parent->dzieci[indeksDawcy] != dawca && parent->dzieci[indeksDawcy] != nullptr) indeksDawcy++;
		if (parent->dzieci[indeksDawcy] == nullptr) przerwij();

		if (indeksDawcy > indeksPozyczajacego) {
			pozyczajacy->klucze[pozyczajacy->iloscKluczy] = parent->klucze[indeksPozyczajacego];
			pozyczajacy->iloscKluczy++;
			pozyczajacy->dzieci[pozyczajacy->iloscKluczy] = dawca->dzieci[0];
			parent->klucze[indeksPozyczajacego] = dawca->klucze[0];
			for (int i = 0; i < dawca->iloscKluczy - 1; i++) {
				dawca->dzieci[i] = dawca->dzieci[i + 1];
				dawca->klucze[i] = dawca->klucze[i + 1];
			}
			dawca->dzieci[dawca->iloscKluczy - 1] = dawca->dzieci[dawca->iloscKluczy];
		}

		else {
			pozyczajacy->dzieci[pozyczajacy->iloscKluczy + 1] = pozyczajacy->dzieci[pozyczajacy->iloscKluczy];
			for (int i = pozyczajacy->iloscKluczy; i > 0; i--) {
				pozyczajacy->klucze[i] = pozyczajacy->klucze[i - 1];
				pozyczajacy->dzieci[i] = pozyczajacy->dzieci[i - 1];
			}
			pozyczajacy->klucze[0] = parent->klucze[indeksDawcy];
			pozyczajacy->dzieci[0] = dawca->dzieci[dawca->iloscKluczy];
			parent->klucze[indeksDawcy] = dawca->klucze[dawca->iloscKluczy - 1];
			pozyczajacy->iloscKluczy++;
		}

		dawca->dzieci[dawca->iloscKluczy] = nullptr;
		dawca->iloscKluczy--;
	}

	void push(int x) {
		bWezel* wzbogacany = findToPush(root, x); //ewentualne rozbicie wezla nastepilo juz w tej funkcji
		for (int i = 0; i < wzbogacany->iloscKluczy; i++) {
			if (wzbogacany->klucze[i] >= x) {
				for (int j = wzbogacany->iloscKluczy; j > i; j--) wzbogacany->klucze[j] = wzbogacany->klucze[j - 1];
				wzbogacany->klucze[i] = x;
				wzbogacany->iloscKluczy++;
				return;
			}
		}
		wzbogacany->klucze[wzbogacany->iloscKluczy++] = x;
	}
	
	void usun(bWezel* curr, int x) {
		bWezel* w = findToPop(curr, x);
		if (w == nullptr) return;
		else if (w->jestLisciem == true) {
			for (int i = 0; i < w->iloscKluczy; i++) {
				if (w->klucze[i] == x) {
					for (; i < w->iloscKluczy - 1; i++) {
						w->klucze[i] = w->klucze[i + 1];
					}
					w->iloscKluczy--;
					break;
				}
			}
		}
		else {
			bWezel* przet, * po;
			int i;
			for (i = 0; i < w->iloscKluczy; i++) {
				if (w->klucze[i] == x) {
					przet = w->dzieci[i];
					po = w->dzieci[i + 1];
					break;
				}
			}
			if (przet->iloscKluczy == T - 1 && po->iloscKluczy == T - 1) {
				usun(merge(przet, po, w), x);
				return;
			}
			else if (przet->iloscKluczy > T - 1) {
				w->klucze[i] = przet->klucze[przet->iloscKluczy - 1];
				przet->klucze[przet->iloscKluczy - 1] = x;
				usun(przet, x);
				return;
			}
			else {
				w->klucze[i] = po->klucze[0];
				po->klucze[0] = x;
				usun(po, x);
				return;
			}
		}
	}

	void wypisz(bWezel* curr, int tryb) {
		int i;
		if (tryb == SAVE) cout << "( ";
		for (i = 0; i < curr->iloscKluczy; i++) {
			if (curr->jestLisciem == false) wypisz(curr->dzieci[i], tryb);
			cout << curr->klucze[i] << " ";
		}
		if (curr->jestLisciem == false) wypisz(curr->dzieci[i], tryb);
		if (tryb == SAVE) cout << ") ";
	}
};

int main() {
	char input;
	int x;
	bTree* t;

	cin >> input;
	if (input == 'I') {
		cin >> x;
		t = new bTree(x);
	}
	else if (input == 'L') {
		string s;
		cin >> x;
		getline(cin, s);
		getline(cin, s);
		t = new bTree(s, x);
	}
	else {
		cout << "invalid input";
		return 0;
	}

	while (input != 'X') {
		cin >> input;
		if (input == 'A') {
			cin >> x;
			t->push(x);
		}
		else if (input == 'R') {
			cin >> x;
			t->usun(t->root, x);
		}
		else if (input == '?') {
			cin >> x;
			if (t->findKey(t->root, x)) cout << x << " +\n";
			else cout << x << " -\n";
		}
		else if (input == 'P') {
			t->wypisz(t->root, PRINT);
			cout << endl;
		}
		else if (input == 'S') {
			cout << t->T << endl;
			t->wypisz(t->root, SAVE);
			cout << endl;
		}
	}
	delete t;
	return 0;
}