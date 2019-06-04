#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
using namespace std;
int Y;
char X = 'A';
void odigraj(int &Y, char &X){
        cout << "\nZigica igrac " << X << " uzeo sa stola: \n";
        cin >> Y;
}
void IgracA(int signum){
        X = 'A';
        odigraj(Y,X);
}
void IgracB(int signum){
        X = 'B';
        odigraj(Y,X);
}
void UbijProgram(int signum){
        raise(SIGKILL);
}

int main(int argc, char *argv[]){
        int M=3,N;
        if (argc==1) {
                cout << "Moras unijeti broj zigica preko naredbenog retka, ne ide drugacije!\n";
                return 0;
        }
        N = atoi(argv[1]);
        sigset(SIGINT, IgracA);
        sigset(SIGQUIT, IgracB);
        sigset(SIGTSTP, UbijProgram);
        cout << "M = " << M << ". N = " << N << ".\n";
        if(M>=N){
                cout << "Parametri nisu ispravni. (N mora biti cijeli broj veci od M)\n";
                return 0;
        }
        else {
                cout << "Parametri su ispravni. Zapocinjem igru.\n";
                while(N>0){
                        cout << "Zigica na stolu: " << N << ". Na redu igrac: " << X << ".\n";
                        pause();
                        if(N<Y) cout << "Nema toliko zigica na stolu!\n";
                        else if(Y<1 || Y>M) cout << "Krivi unos!! Moras uzeti cijeli broj zigica izmedju 1 i M.\n";
						else {
                                N -= Y;
                                X++;
                                if(X=='C') X='A';
                        }
                }
                cout << "Zigica na stolu: 0. Pobijedio igrac: " << X << ".\n";
        }

        return 0;
}
