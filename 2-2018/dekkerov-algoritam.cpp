#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
using namespace std;
int id;
struct ZAP{
        int ZASTAVICA[2];
        int PRAVO;
} *zajednicko;

void udji_KO(int i, int j) {
        (*zajednicko).ZASTAVICA[i] = 1;
        while((*zajednicko).ZASTAVICA[j] != 0) {
                if((*zajednicko).PRAVO == j) {
                        (*zajednicko).ZASTAVICA[i] = 0;
                        while((*zajednicko).PRAVO == j);
                        (*zajednicko).ZASTAVICA[i] = 1;
                }
        }

}
void izadji_KO(int i, int j){
        (*zajednicko).PRAVO = j;
        (*zajednicko).ZASTAVICA[i] = 0;
}
void proc(int i, int j){
        int k, m;
        for(k=1; k<=5; k++) {
                udji_KO(i,j);
                for(m = 1; m<=5; m++) {
                        cout << "Dretva: " << i+1 << ", K.O. broj: " << k << " (" << m << "/5)" << endl;
                        sleep(1);
                }
        }
        izadji_KO(i, j);
}
void brisi(int sig){
        shmdt((ZAP*)zajednicko);
        shmctl(id, IPC_RMID, NULL);
        exit(0);
}
int main(){
        sigset(SIGINT, brisi);

        id = shmget(IPC_PRIVATE, sizeof(int)*100, 0600);
        if(id==-1) exit(1);
        zajednicko = (ZAP*)shmat(id, NULL, 0);

        (*zajednicko).PRAVO = 0;
        (*zajednicko).ZASTAVICA[0] = 0;
        (*zajednicko).ZASTAVICA[1] = 0;

        for(int n = 0; n<=1; n++) {
                switch(fork()){
                        case -1:
                                cerr<<"Pogreska"<<endl;
                                exit(-1);
                                break;
                        case 0:
                                proc(n, 1-n);
                                exit(0);
                                break;
                        default:
                                break;
                }
        }

        for(int n = 0; n<=1; n++) wait(NULL);
        brisi(0);
        return 0;
}

