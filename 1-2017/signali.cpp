#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
using namespace std;
int Y;
void y(int &Y, int n){
        Y=n;
}
void prekid1(int signum){
        y(Y,1);
}
void prekid2(int signum){
        y(Y,2);
}
void prekid3(int signum){
        y(Y,3);
}

int main(int argc, char *argv[]){
        int M=3,N;
        char X = 'A';
        if (argc==1) {
                cout << "Moras unijeti broj zigica preko naredbenog retka, ne ide drugacije!\n";
                return 0;
        }
        N = atoi(argv[1]);
        sigset(SIGINT, prekid1);
        sigset(SIGQUIT, prekid2);
        sigset(SIGTSTP, prekid3);
        cout << "M = " << M << ". N = " << N << ".\n";
        if(M>=N){
                cout << "Parametri nisu ispravni.\n";
                return 0;
        }
        else {
                cout << "Parametri su ispravni. Zapocinjem igru.\n";
                cout << "Pravila: ^C - 1\t ^Ž - 2\t^Z - 3\n\n";
                while(N>0){
                        cout << "Zigica na stolu: " << N << ". Na redu igrac: " << X << ".\n";
                        pause();
                        if(N<Y) cout << " Nema toliko zigica na stolu!\n";
                        else {
                                cout << " Zigica igrac " << X << " uzeo sa stola: " << Y << ".\n";
                                N -= Y;
                                X++;
                                if(X=='C') X='A';
                        }
                }
                cout << "Zigica na stolu: 0. Pobijedio igrac: " << X << ".\n";
        }

        return 0;
}
