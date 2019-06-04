#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
using namespace std;

int main(int argc, char* argv[]){
        msgbuf poruka;
        int duljinaPoruke = strlen(argv[1]);
        int pid = getpid();

        key_t key = atoi(getenv("MSG_KEY"));
        int msqid = msgget(key, 00600 | IPC_CREAT);
        cout << "[S] Nadjena varijabla okoline " << getenv("MSG_KEY") << endl;
        cout << "[Proizvodac] pid: " << pid << endl;

        for(int i=0; i<=duljinaPoruke; i++){
                poruka.mtype = pid;
                if(i!=duljinaPoruke) poruka.mtext[0] = argv[1][i];
                else poruka.mtext[0] = '\0';
                msgsnd(msqid, &poruka, 1, 0);
        }
        cout << "[Proizvodac] (pid=" << getpid() << ") zavrsio s radom!" << endl;

        return 0;
}
