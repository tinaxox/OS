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

typedef struct ponuda
{
    long mtype;
    int iznos;
    int rbr;
    pid_t pid_ucesnika;

} ponuda;

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

    // prijave na aukciju
    int aukcija;
    scanf("%d", &aukcija);
    u.mtype = aukcija + 1;
    msgsnd(id_reda, (void *)&u, sizeof(u) - sizeof(u.mtype), 0);
    printf("poslata prijava %ld\n", u.mtype);
    printf("cekam: %d\n", u.pid_ucesnika);
    msgrcv(id_reda, (void *)&u, sizeof(u) - sizeof(u.mtype), u.pid_ucesnika, 0); // ceka dok se ne prijave svi
    // krece aukcija nakon sto on primi ovu poruku
    printf("AUKCIJA moze da pocne.\n");

    for (int broj_kruga = 0; broj_kruga < 3; broj_kruga++)
    {
        int iznos;
        scanf("%d", &iznos);
        ponuda p = {aukcija + 1, iznos, u.rbr, u.pid_ucesnika};

        printf("broj kruga: %d\n", broj_kruga);
        msgsnd(id_reda, (void *)&p, sizeof(p) - sizeof(p.mtype), 0);
        ponuda max_runde;
        msgrcv(id_reda, (void *)&max_runde, sizeof(max_runde) - sizeof(max_runde.mtype), u.pid_ucesnika, 0);
        if (max_runde.pid_ucesnika == p.pid_ucesnika)
        {
            printf("Ja sam pobednik runde %d sa iznosom %d\n", broj_kruga, max_runde.iznos);
        }
        else
        {
            printf("Pobednik nakon runde %d je korisnik %d sa iznosom %d\n", broj_kruga, max_runde.rbr, max_runde.iznos);
        }
    }
    ponuda max;

    msgrcv(id_reda, (void *)&max, sizeof(max) - sizeof(max.mtype), u.pid_ucesnika, 0);
    printf("MAKSIMALNA PONUDA %d-%d %d\n", max.rbr, max.pid_ucesnika, max.iznos);

    if (max.pid_ucesnika == u.pid_ucesnika)
    {
        printf("Ja sam pobednik runde %d sa iznosom %d\n", aukcija, max.iznos);
    }
    else
    {
        printf("Pobednik nakon runde %d je korisnik %d sa iznosom %d\n", aukcija, max.rbr, max.iznos);
    }

    return 0;
}