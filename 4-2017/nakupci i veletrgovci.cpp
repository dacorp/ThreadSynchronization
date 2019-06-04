#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
using namespace std;

pthread_mutex_t m;
pthread_cond_t red;
pthread_mutexattr_t psharedm;
pthread_condattr_t psharedr;

bool prazan = true;
int stavi;

void *veletrgovac(void *x){
        while(1) {
                pthread_mutex_lock(&m);
                if(!prazan) pthread_cond_wait(&red, &m);
                srand(time(NULL) ^ (getpid()<<16));
                stavi = rand()%3;
                switch(stavi){
                        case 0:
                                cout << "Veletrgovac stavio monitor i racunalo\n";
                                break;
                        case 1:
                                cout << "Veletrgovac stavio racunalo i tipkovnicu\n";
                                break;
                        case 2:
                                cout << "Veletrgovac stavio monitor i tipkovnicu\n";
                                break;

                }
                prazan = false;
                pthread_mutex_unlock(&m);
        }
}
void nakupac(int i){
        while(1){
                pthread_mutex_lock(&m);
                if(!prazan && stavi==i){
                        switch(i){
                                case 0:
                                        cout << "Nakupac s tipkovnicama uzeo monitor i racunalo\n";
                                        break;
                                case 1:
                                        cout << "Nakupac s monitorima uzeo racunalo i tipkovnicu\n";
                                        break;
                                case 2:
                                        cout << "Nakupac s racunalima uzeo monitor i tipkovnicu\n";
                                        break;
                        }
                        sleep(1);
                        prazan = true;
                        pthread_cond_signal(&red);
                }
                pthread_mutex_unlock(&m);
        }
}
void *x(void *i){
        nakupac(*((int*)i));
}

void signal(int sig){
        pthread_mutexattr_destroy(&psharedm);
        pthread_condattr_destroy(&psharedr);
        pthread_mutex_destroy(&m);
        pthread_cond_destroy(&red);
        cout << endl;
        exit(0);
}

int main(){
        sigset(SIGINT,signal);

        pthread_mutexattr_init(&psharedm);
        pthread_condattr_init(&psharedr);

        pthread_mutexattr_setpshared(&psharedm, PTHREAD_PROCESS_SHARED);
        pthread_condattr_setpshared(&psharedr, PTHREAD_PROCESS_SHARED);

        pthread_mutex_init(&m, &psharedm);
        pthread_cond_init(&red, &psharedr);
        pthread_t id_veletrgovac, id_nakupac[3];

        int arg[3] = {0,1,2};

        pthread_create(&id_veletrgovac, NULL, veletrgovac, NULL);
        for(int i=0; i<3; i++) pthread_create(&id_nakupac[i], NULL, x, (void*)&arg[i]);
        pthread_join(id_veletrgovac, NULL);
        for(int i=0; i<3; i++) pthread_join(id_nakupac[i], NULL);
        return 0;
}

