#ifndef FILEIO_H
#define FILEIO_H

#include "models.h"

// User storage in users.txt: each line "username password role"
int load_users(User **out);
int save_users(const User *arr, int count);

// Manifesto in manifestos.txt: lines "rep_username|manifesto_text"
int load_manifestos(Manifesto **out);
int save_manifestos(const Manifesto *arr, int count);

// Votes in votes.txt: "student_username rep_username"
int load_votes(Vote **out);
int save_votes(const Vote *arr, int count);

// Results in results.txt: "rep_username vote_count"
int save_results(const Manifesto *mfs, const int *counts, int mfCount);
int load_results(Manifesto **outMfs, int **outCounts);

#endif
