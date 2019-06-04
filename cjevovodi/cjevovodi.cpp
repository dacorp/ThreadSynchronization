#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

#define READ 0
#define WRITE 1
#define ERROR 2

int main(int argc, char* argv[]){
        int citaj[2], pisi[2], rjesenje;
        char* bc = new char[100];
        char* zadatak = new char[100];

        if(argc != 2) {
                cout << "Neispravan unos\n";
                return 0;
        }

        ifstream datoteka(argv[1]);

        pipe(citaj);
        pipe(pisi);

        switch(fork()) {
                case 0:
                        close(pisi[WRITE]);
                        close(citaj[READ]);

                        dup2(pisi[READ], READ);
                        dup2(citaj[WRITE], WRITE);
                        dup2(citaj[WRITE], ERROR);

                        execlp("bc", "bc", NULL);
                        exit(0);
                        break;
                case -1:
                        cout << "Greska u kreiranju procesa\n";
                        break;
                default:
                        if(datoteka.is_open()) {
                                while(true) {
                                        datoteka.getline(zadatak, 20, '\n');
                                        if(datoteka.eof()) break;
                                        cout << zadatak << "=";
                                        cin >> rjesenje;

                                        zadatak[strlen(zadatak)] = '\n';

                                        write(pisi[WRITE], zadatak, strlen(zadatak));
                                        read(citaj[READ], bc, 100);

                                        if(!atoi(bc)) cout << "NEISPRAVAN IZRAZ\n";
                                        else if(atoi(bc) == rjesenje) cout << "ISPRAVNO\n";
                                        else cout << "NEISPRAVNO, tocan odgovor je " << atoi(bc) << endl;
                                }
                                datoteka.close();
                        }
                        else {
                                cout << "Otvaranje datoteke nije uspjelo\n";
                        }
                        break;
        }
        return 0;
}