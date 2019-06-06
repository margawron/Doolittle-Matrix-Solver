#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


/*
 * Brzydkie zmienne globalne
 */
double** macierz_L;
double** macierz_U;
double** macierz_A;
double* macierz_B;			// wektor B
double* macierz_Y;			// wektor Y
double* macierz_X;			// wektor X

int n;					// rozmiar macierzy


void wypisz_macierze(std::ostream& stream) {
	stream << "Macierz A\n";
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			stream << macierz_A[i][j] << " ";
		}
		stream << "\n";
	}

	stream << "Macierz B\n";
	for (int i = 0; i < n; i++) {
		stream << macierz_B[i] << " ";

	}
	stream << "\n";
	stream << "Macierz L\n";
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			stream << macierz_L[i][j] << " ";
		}
		stream << "\n";
	}
	stream << "Macierz U\n";
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			stream << macierz_U[i][j] << " ";
		}
		stream << "\n";
	}
	stream << "Macierz Y\n";
	for (int i = 0; i < n; i++) {
		stream << macierz_Y[i] << " ";
	}

}

void wypisz_macierz_X(std::ostream & stream) {
	stream << "\nMacierz X:\n";
	for (int i = 0; i < n; i++) {
		stream << macierz_X[i] << " ";
	}
}

void wyznacz_macierze_LU() {
	// allokuj pamięć dla macierzy L
	macierz_L = new double* [n];  //utwórz wiersze L
	for (int i = 0; i < n; i++)
		macierz_L[i] = new double[n]; // utwórz kolumny L

	macierz_U = new double*[n]; // utwórz wiersze dla U
	for (int i = 0; i < n; i++)
		macierz_U[i] = new double[n]; // utwórz kolumny dla U

	// wyzeruj macierz L i ustaw na głównej przekątnej jedynki
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			macierz_L[i][j] = 0;
			if (i == j) macierz_L[i][j] = 1;
		}
	}

	// wyzeruj macierz U
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			macierz_U[i][j] = 0;
		}
	}


	// WZÓR 20
	// przepisz pierwszy wiersz macierzy A do macierzy U oraz przepisz elementy pierwszej kolumny macierzy A
	// podzielone przez lewy górny element macierzy U do macierzy L
	for (int j = 0; j < n; j++) {
		macierz_U[0][j] = macierz_A[0][j];
		if (macierz_U[0][0] == 0) {
			std::cout << "Napotkano dzielenie przez zero podczas dzielenia elementów macierzy A przez U[1]" <<
				"[" << j << "]\n";
			return;
		}
		if (j > 0) {
			macierz_L[j][0] = macierz_A[j][0] / macierz_U[0][0];
		}
	}
	// WZÓR 21
	//
	for (int i = 1; i < n; i++) {
		// u[i][j] = a[i][j] - SUMA od k = 1 do i - 1(L[i][k]*U[k][j])
		for (int j = i; j < n; j++) {
			double suma = 0;
			for (int k = 0; k < i; k++) {
				suma += macierz_L[i][k] * macierz_U[k][j];
			}
			macierz_U[i][j] = macierz_A[i][j] - suma;
		}
		// L[i][j] = (a[i][j] - SUMA od k = 1 do i - 1(L[i][k]*U[k][j]))/u[i][i];
		if (macierz_U[i][i] == 0) {
			std::cout << "Napotkano dzielenie przez zero podczas obliczania wartości dla macierz L przez U"
				<< "[" << i << "]"
				<< "[" << i << "]\n";
			return;
		}
		for (int j = i + 1; j < n; j++) {
			double suma = 0;
			for (int k = 0; k < i; k++) {
				suma += macierz_L[j][k] * macierz_U[k][i];
			}
			macierz_L[j][i] = (macierz_A[j][i] - suma) / macierz_U[i][i];
		}
	}


}

/*
 * AX = B
 * LUX = B
 * LY=B && UX=Y
 *
 * Oblicza macierz Y, a następnie
 * macierz X
 */
void oblicz_X() {
	macierz_Y = new double[n];

	// WZÓR 16
	macierz_Y[0] = macierz_B[0]; // początkowy element wektora B
	for (int i = 1; i < n; i++) {
		double suma = 0;
		for (int k = 0; k < i; k++) {
			suma += macierz_L[i][k] * macierz_Y[k];
		}
		macierz_Y[i] = macierz_B[i] - suma;
	}

	macierz_X = new double[n];
	for (int i = 0; i < n; i++)
		macierz_X[i] = 0;


	// WZÓR 17
	macierz_X[n - 1] = macierz_Y[n - 1] / macierz_U[n - 1][n - 1]; // n - 1 ponieważ korzystamy z numeracji od 0 do n-1
	for (int i = n - 2; i >= 0; i--) {
		double suma = 0;
		for (int k = i + 1; k < n; k++) {
			suma += macierz_U[i][k] * macierz_X[k];
		}
		macierz_X[i] = (macierz_Y[i] - suma) / macierz_U[i][i];
	}
}

void wczytaj_dane() {
	std::cout << "Podaj rozmiar macierzy ktora ma zostac wczytana z pliku macierz.txt:\n";
	std::cin >> n;
	std::fstream pWejsciowy("macierz.txt", std::ios::in);
	macierz_A = new double* [n];
	// Alokacja miejsca na macierz A
	for (int i = 0; i < n; i++) {
		macierz_A[i] = new double[n];
	}
	// Alokacja miejsca na macierz B
	macierz_B = new double[n];

	std::string tmp;
	for (int i = 0; i < n; i++) { // Wpisz wartości do macierzy A
		std::getline(pWejsciowy, tmp);
		std::stringstream ss(tmp);
		for (int j = 0; j < n; j++) {
			ss >> macierz_A[i][j];
		}
	}

	std::getline(pWejsciowy, tmp);
	std::stringstream ss(tmp);
	for (int i = 0; i < n; i++) { // Wpisz wartości do macierzy B
		ss >> macierz_B[i];
	}
	pWejsciowy.close();
}

void zapisz_dane() {
	std::fstream pWyjsciowy("raport.txt", std::ios::out);
	wypisz_macierze(pWyjsciowy);
	wypisz_macierz_X(pWyjsciowy);
	pWyjsciowy.close();
}

int main(int argc, char** argv) {
	wczytaj_dane();
    wyznacz_macierze_LU();
    oblicz_X();
	zapisz_dane();
	wypisz_macierze(std::cout);
	wypisz_macierz_X(std::cout);
	std::cout << "\nRaport zostal zapisany w pliku raport.txt\n";
	return 0;
}
