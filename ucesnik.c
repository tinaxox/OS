#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

typedef struct ucesnik
{
    long mtype;
    pid_t pid_ucesnika;
    int rbr;
} ucesnik;

int main()
{
    // prijave
    int broj_aukcija;
    ucesnik u;
    key_t key = ftok("./organizator.c", 1);
    int id_reda = msgget(key, IPC_CREAT | 0666);
    u.mtype = 1; // gde da trazi poruku
    u.pid_ucesnika = getpid();
    u.rbr = 0;
    msgsnd(id_reda, (void *)&u, sizeof(u) - sizeof(u.mtype), 0);                 // salje svoj pid
    msgrcv(id_reda, (void *)&u, sizeof(u) - sizeof(u.mtype), u.pid_ucesnika, 0); // prima broj aukcija
    broj_aukcija = u.rbr;                                                        // setuje br aukcija
    msgrcv(id_reda, (void *)&u, sizeof(u) - sizeof(u.mtype), u.pid_ucesnika, 0); // prima redni broj
    printf("ucesnik: %d sa rednim brojem %d ucestvovace na %d aukcija\n", u.pid_ucesnika, u.rbr, broj_aukcija);

    for (int i = 0; i <)
        // prijave na aukciju
        int aukcija;
    scanf("%d", &aukcija);
    u.mtype = aukcija;
    msgsnd(id_reda, (void *)&u, sizeof(u) - sizeof(u.mtype), 0);
    return 0;
}