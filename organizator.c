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
    int N, M; // N-broj knjiga (aukcija), M-broj ucesnika na aukciji
    scanf("%d%d", &N, &M);
    printf("%d %d\n", N, M);
    key_t key = ftok("./organizator.c", 1);
    int id_reda = msgget(key, IPC_CREAT | 0666);
    ucesnik ucesnici[N * M];
    for (int i = 0; i < N * M; i++)
    {
        ucesnik u;

        msgrcv(id_reda, (void *)&u, sizeof(u) - sizeof(u.mtype), 1, 0); // prima prijavu, tj pid ucesnika i rbr koji trenutno je 0
        u.mtype = u.pid_ucesnika;                                       // setujemo u.mtype na pid ucesnika koji smo dobili odozgo
        u.rbr = N;                                                      // stavljamo rbr na N da bismo vratili ucesniku informaciju o tome kolko ima aukcija
        msgsnd(id_reda, (void *)&u, sizeof(u) - sizeof(u.mtype), 0);    // saljemo to N, preko mtype = pid, svaki ucesnik da prepozna poruku koja je njemu poslata
        u.rbr = i;                                                      // stavljamo sada rbr na i
        msgsnd(id_reda, (void *)&u, sizeof(u) - sizeof(u.mtype), 0);    // saljemo opet taj rbr kad je on upisan vec u ucesniku
        ucesnici[i] = u;
        printf("ucesnik: %ld sa rednim brojem %d\n", u.mtype, u.rbr);
    }

    // msgctl(id_reda, IPC_RMID, NULL);
    printf("--------------------\n");
    printf("Prijavili su se svi.\n");

    // aukcije
    for (int i = 0; i < N; i++)
    {
        pid_t aukcija = fork();

        if (aukcija == -1)
        {
            printf("ERROR\n");
            exit(1);
        }
        if (aukcija == 0)
        {
            ucesnik ucesnik_na_aukciji[M];
            for (int i = 0; i < M; i++)
            {
                ucesnik a;
                msgrcv(id_reda, (void *)&a, sizeof(a) - sizeof(a.mtype), i, 0);
                ucesnik_na_aukciji[i] = a;
                printf("%d %d na %d\n", ucesnik_na_aukciji[i].pid_ucesnika, ucesnik_na_aukciji[i].rbr, i);
            }

            return 0;
        }
    }

    return 0;
}