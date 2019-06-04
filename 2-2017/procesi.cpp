#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <signal.h>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <ctime>
using namespace std;
long double *a, *b;
int id;
void brisi(int sig){
        shmdt((long double*)b);
        shmctl(id,IPC_RMID,NULL);
        exit(0);
}
int main(int argc, char *argv[]){
        int l,m,n,broj_zp, zadatak,brojac,id;
        if(argc<4) {
                cout << "Krivi unos!" << endl;
                return 0;
        }
        l = atoi(argv[1]);
        m = atoi(argv[2]);
        n = atoi(argv[3]);

        if (!l || !m || !n) {
                cout << "Krivi unos!" << endl;
                return 0;
        }

        if(l<n) {
                cout << "varijabla l mora biti veca od varijable n!!\n";
                return 0;
        }
        a = (long double*)malloc(l);
        id = shmget(IPC_PRIVATE,sizeof(int)*100,0600);
        if(id==-1) exit(1);
        b = (long double*)shmat(id, NULL, 0);
        sigset(SIGINT, brisi);

        cout << "Eksponenti = \n";
        srand(time(0));
        rand();
        for(int i=0; i<l; i++){
                a[i] = 10*((double)rand()/(double)RAND_MAX);
                cout << setprecision(12) << fixed << a[i] << endl;
        }
        broj_zp = l/n;
        zadatak = 0;
        cout << "Eksponencijale = \n";
        for(int x=1;x<=n;x++) {
                brojac = zadatak;
                if(x!=n)zadatak += broj_zp;
                else zadatak = l;
                switch(fork()) {
                        case -1:
                                cerr<<"Pogreska"<<endl;
                                exit(-1);
                                break;
                        case 0: {
                                double brojnik, nazivnik;
                                for(int j=brojac; j<zadatak; j++){
                                        b[j]=0;
                                        for(int i=0;i<m;i++){
                                                brojnik = pow(a[j],i);
                                                nazivnik = 1;
                                                if(i>0) for(int k =1; k<=i; k++) nazivnik *= k;
                                                b[j] += (double)brojnik/nazivnik;
                                        }
                                        usleep(10000);
                                }
                                exit(0);
                                break;
                        }
                        default:
                                break;
                }
        }
        wait(NULL);
        for(int i=0;i<l;i++) cout << b[i] << endl;
        shmdt((long double*) b);
        shmctl(id,IPC_RMID,NULL);
        exit(0);
        return 0;
    }
