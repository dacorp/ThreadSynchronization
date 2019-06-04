#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
using namespace std;

int main(int argc, char* argv[]){
        char kljuc[15];

        if(argc<2){
                cout << "Neispravan unos argumenata" << endl;
                return 0;
        }

        key_t key = getuid();
        sprintf(kljuc, "%d", key);
        setenv("MSG_KEY", kljuc, true);
        int msqid = msgget(key, 00600 | IPC_CREAT);

        for(int i = 1; i<=argc; i++) {
                switch(fork()) {
                        case 0:
                                if(i==argc) execl("./potrosac", "potrosac", NULL);
                                else execl("./proizvodac", "proizvodac", argv[i], NULL);
                                exit(0);
                                break;
                        case -1:
                                cout << "Ne mogu kreirati novi proces" << endl;
                                break;
                }
        }

        for(int i = 0; i<argc; i++) wait(NULL);
        return 0;
}
