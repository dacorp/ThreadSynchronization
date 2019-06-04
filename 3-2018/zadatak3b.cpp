#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <cstdlib>
#include <ctime>
#include <signal.h>
using namespace std;
struct ZAP {
        int ULAZ;
        int IZLAZ;
        int M[5];
} *x;

int shmId;

int N;
int K;

const int PUN = 0;
const int PISI = 1;
const int PRAZAN = 2;

int SemId;

void SemGet(int n) {
        SemId = semget(IPC_PRIVATE, n, 0600);

        if(SemId == -1) {
                cout << "Nema semafora!" << endl;
                exit(1);
        }
}
int SemSetVal(int SemNum, int SemVal) {
        return semctl(SemId, SemNum, SETVAL, SemVal);
}

int SemOp(int SemNum, int SemOp) {

        struct sembuf SemBuf;

        SemBuf.sem_num = SemNum;
        SemBuf.sem_op = SemOp;
        SemBuf.sem_flg = 0;
        return semop(SemId, &SemBuf, 1);
}

void SemRemove(void) {
        (void) semctl(SemId, 0, IPC_RMID, 0);
}
void Proizvodac(int j) {
        srand(getpid());
        int i=0;
        for (i=0; i<K; i++) {
                SemOp(PUN,-1);
                SemOp(PISI,-1);
                x->M[x->ULAZ] = rand()%1000;
                cout << "Proizvodac " << j << " salje " << x->M[x->ULAZ] << endl;
                x->ULAZ = (x->ULAZ + 1) % 5;
                SemOp(PISI, 1);
                SemOp(PRAZAN, 1);
        }
        cout << "Proizvodac " << j << " zavrsio sa slanjem" << endl;
        exit(0);
}
void Potrosac() {
        int i = 0;
        int suma = 0;
        for (i=0; i<N*K; i++) {
                SemOp(PRAZAN,-1);
                cout << "Potrosac prima " << x->M[x->IZLAZ] << endl;
                suma += x->M[x->IZLAZ];
                x->IZLAZ = (x->IZLAZ + 1) % 5;
                SemOp(PUN, 1);
        }
        cout << "Potrosac - suma primljenih brojeva=" << suma << endl;
        exit(0);
}

void brisi(int sig) {
        (void) shmdt((char *) x);
        (void) shmctl(shmId, IPC_RMID, NULL);
        SemRemove();
        exit(0);
}
int main (int argc, char* argv[]) {
        if(argc != 3) {
                cout << "Krivi unos" << endl;
                return 0;
        }

        N = atoi(argv[1]);
        K = atoi(argv[2]);

        sigset(SIGINT, brisi);

        shmId = shmget(IPC_PRIVATE, sizeof(ZAP), 0600);
        if(shmId == -1) exit(1);

        x = (ZAP*) shmat(shmId, NULL, 0);
        x->ULAZ = 0;
        x->IZLAZ = 0;
        SemGet(3);

        SemSetVal(PISI, 1);
        SemSetVal(PUN, 4);
        SemSetVal(PRAZAN, 0);

        int k;
        for(k=0; k<=N; k++) {
                switch(fork()) {
                        case 0:{
                                if(k==0) Potrosac();
                                else Proizvodac(k);
                                break;
                        }
                        case -1:{
                                cout << "Neuspjelo stvaranje procesa!" << endl;
                                break;
                        }
                }
        }

        int i;
        for(i=0; i<=N; i++) wait(NULL);
        brisi(0);
        return 0;
}
