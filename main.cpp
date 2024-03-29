#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>
#include <memory>
using namespace std;

class Obserwator {
    public:
        Obserwator(const char* s) : nazwa{s ? s : "nienazwany"} { /* nie robi nic */ }
        virtual ~Obserwator() { } // klasa bazowa - polimorfizm - virt. destr.
        const char* id() const { return nazwa.c_str(); }
        virtual void update() = 0; // metoda czysto wirtualna
    private:
        string nazwa;
};

class Manager {
    public:
        void dodaj(unique_ptr<Obserwator>& p);
        void usun(unique_ptr<Obserwator>& p);
        void powiadom();
        void obserwatorzy() const;
    private:
        list<unique_ptr<Obserwator>> klient;
};

void Manager::obserwatorzy() const {
    if (klient.empty()) cout << "Brak obserwatorow \n";
    auto it = klient.cbegin(); // cbegin zwraca staly iterator na poczatek
    // auto wyrecza nas w pisaniu: list<Obserwator*>::const_iterator
    while (it != klient.cend()) {
        cout << "Obserwator " << (*it)->id() << endl;
        ++it;
    }
}

void Manager::powiadom() {
    for (auto& p : klient) p->update();
}

void Manager::dodaj(unique_ptr<Obserwator>& p) {
    if (find(klient.begin(),klient.end(),p) == klient.end()) {
        klient.push_back(move
        
        ./pro   
        (p)); // dodanie na koniec listy
    } else {
        cout << "Obserwator " << p->id() << " juz jest na liscie \n";
    }
}

void Manager::usun(unique_ptr<Obserwator>& p) {
    klient.erase(find(klient.begin(),klient.end(),p));
}

class Meteo : public Manager {
    public:
        double getT() const { return t; }
        void operator()(size_t n); // n liczba losowan
    private:
        double t { 0. };
        double losuj(int a, int b);
        random_device rd;
};

double Meteo::losuj(int a, int b) {
    // return static_cast<double>( rand() % ( b - a + 1 ) + a );
    mt19937 gen(rd());
    uniform_real_distribution<> dis(a,b);
    return dis(gen);
}

void Meteo::operator()(size_t n) {
    while (n--) {
        t = losuj(5,25);
        powiadom();
    }
}

class TChwilowa : public Obserwator {
    public:
        TChwilowa(const char* s, const Meteo& r) : Obserwator(s),ref{r} { }
        void update() {
            t = ref.getT();
            cout << "Obserwator " << id() << " t = " << t << endl;
        }
    private:
        double t { 0. };
        const Meteo& ref;
};

class TSrednia : public Obserwator {
    public:
        TSrednia(const char* s, const Meteo& r) : Obserwator(s),ref{r} { }
        void update() {
            vec.push_back(ref.getT());
            tsr = 0.;
            for (const auto& d : vec) tsr += d;
            tsr /= vec.size();
            cout << "Obserwator " << id() << " t srednia = " << tsr << endl;
        }
    private:
        double tsr { 0. };
        vector<double> vec;
        const Meteo& ref;
};

int main() {
    //srand(time(0));
    Meteo stacja;
    unique_ptr<Obserwator> p1(new TChwilowa("MONITOR 1",stacja));
    unique_ptr<Obserwator> p2(new TChwilowa("MONITOR 2",stacja));
    unique_ptr<Obserwator> p3(new TSrednia("MONITOR SREDNI 3",stacja));
    unique_ptr<Obserwator> p4(new TSrednia("MONITOR SREDNI 4",stacja));
    stacja.obserwatorzy(); // pusta lista
    stacja.dodaj(p1);
    stacja.dodaj(p2);
    stacja.dodaj(p3);
    stacja.dodaj(p4);
    stacja.obserwatorzy();
    stacja(2); // dwa losowania, wywolanie stacja.operator()(2)
    stacja.dodaj(p1);
    stacja.usun(p3);
    stacja(2); // kolejne dwa losowania, juz bez p3
}