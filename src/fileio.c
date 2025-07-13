#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"

// Load users
int load_users(User **out) {
    FILE *f = fopen("users.txt", "r");
    if (!f) { *out = NULL; return 0; }
    User *arr = NULL; int cap = 0, cnt = 0;
    while (!feof(f)) {
        User u;
        if (fscanf(f, "%31s %31s %d\n", u.username, u.password, (int*)&u.role) != 3)
            break;
        if (cnt == cap) arr = realloc(arr, (cap = cap ? cap*2 : 4) * sizeof *arr);
        arr[cnt++] = u;
    }
    fclose(f); *out = arr; return cnt;
}

// Save users
int save_users(const User *arr, int count) {
    FILE *f = fopen("users.txt", "w");
    if (!f) return -1;
    for (int i = 0; i < count; i++)
        fprintf(f, "%s %s %d\n", arr[i].username, arr[i].password, arr[i].role);
    fclose(f); return 0;
}

// Load manifestos
int load_manifestos(Manifesto **out) {
    FILE *f = fopen("manifestos.txt", "r");
    if (!f) { *out = NULL; return 0; }
    Manifesto *arr = NULL; int cap = 0, cnt = 0;
    char buf[600];
    while (fgets(buf, sizeof buf, f)) {
        char *p = strchr(buf, '|');
        if (!p) continue;
        *p = 0;
        if (cnt == cap) arr = realloc(arr, (cap = cap ? cap*2 : 4) * sizeof *arr);
        strcpy(arr[cnt].rep_username, buf);
        strcpy(arr[cnt].manifesto, p+1);
        arr[cnt].manifesto[strcspn(arr[cnt].manifesto, "\n")] = 0;
        cnt++;
    }
    fclose(f); *out = arr; return cnt;
}

// Save manifestos
int save_manifestos(const Manifesto *arr, int count) {
    FILE *f = fopen("manifestos.txt", "w");
    if (!f) return -1;
    for (int i = 0; i < count; i++)
        fprintf(f, "%s|%s\n", arr[i].rep_username, arr[i].manifesto);
    fclose(f); return 0;
}

// Load votes
int load_votes(Vote **out) {
    FILE *f = fopen("votes.txt", "r");
    if (!f) { *out = NULL; return 0; }
    Vote *arr = NULL; int cap = 0, cnt = 0;
    while (!feof(f)) {
        Vote v;
        if (fscanf(f, "%31s %31s\n", v.student_username, v.rep_username) != 2)
            break;
        if (cnt == cap) arr = realloc(arr, (cap = cap ? cap*2 : 4) * sizeof *arr);
        arr[cnt++] = v;
    }
    fclose(f); *out = arr; return cnt;
}

// Save votes
int save_votes(const Vote *arr, int count) {
    FILE *f = fopen("votes.txt", "w");
    if (!f) return -1;
    for (int i = 0; i < count; i++)
        fprintf(f, "%s %s\n", arr[i].student_username, arr[i].rep_username);
    fclose(f); return 0;
}

// Save results
int save_results(const Manifesto *mfs, const int *counts, int mfCount) {
    FILE *f = fopen("results.txt", "w");
    if (!f) return -1;
    for (int i = 0; i < mfCount; i++)
        fprintf(f, "%s %d\n", mfs[i].rep_username, counts[i]);
    fclose(f); return 0;
}

// Load results
int load_results(Manifesto **outMfs, int **outCounts) {
    FILE *f = fopen("results.txt", "r");
    //! correction 
    if (!f) {
    *outMfs = NULL;
    *outCounts = NULL;
    return 0;
    }
    Manifesto *mfs = NULL; int *cnts = NULL;
    int cap = 0, n = 0;
    while (!feof(f)) {
        char uname[32]; int c;
        if (fscanf(f, "%31s %d\n", uname, &c) != 2) break;
        if (n == cap) {
            cap = cap ? cap*2 : 4;
            mfs = realloc(mfs, cap * sizeof *mfs);
            cnts = realloc(cnts, cap * sizeof *cnts);
        }
        strcpy(mfs[n].rep_username, uname);
        cnts[n] = c;
        n++;
    }
    fclose(f); *outMfs = mfs; *outCounts = cnts; return n;
}
