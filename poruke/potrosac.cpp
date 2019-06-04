#include <iostream>
#include <map>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
using namespace std;

int msqid;

void brisi(int a) {
        msgctl(msqid, IPC_RMID, NULL);
        exit(0);
}

int main(){
        int cekanja = 0, zastavica = 0;
        msgbuf poruka;
        map<long, string> tekst;

        sigset(SIGINT, brisi);

        key_t key = atoi(getenv("MSG_KEY"));
        msqid = msgget(key, 00600 | IPC_CREAT);
        cout << "[R] Nadjena varijabla okoline " << getenv("MSG_KEY") << endl;

        while(true){
                if(msgrcv(msqid, &poruka, 1, 0, zastavica)!= -1){
                        cout << "[Potrosac] primio " << poruka.mtext[0] << " od: " << poruka.mtype << endl;
                        if(poruka.mtext[0] == 0) {
                                cout << tekst[poruka.mtype] << endl;
                                zastavica = IPC_NOWAIT;
                        }
                        else {
                                cekanja = 0;
                                zastavica = 0;
                                tekst[poruka.mtype] += poruka.mtext[0];
                        }
                }
                else {
                        cout << "[Potrosac] Cekam (" << cekanja << ")\n";
                        cekanja++;
                        if(cekanja == 10) break;
                        sleep(1);
                }
        }
        brisi(0);
        return 0;
}






