#ifndef FILEIO_H
#define FILEIO_H

#include "models.h"

// create files if they don't exist
void ensure_file_exists(const char *fname);

// User storage in users.txt: each line "username password role"
int load_users(User **out);
int load_reps(User **outReps);  //* Load only representatives
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

// sync manifestos with representatives
void sync_manifestos_with_reps(void);

//! Display the current status of the election results
void display_result_status(void);

//! Mark the election results as published
void mark_results_published(void);

//! Append a vote update to the results file
void append_vote_update(const User *current);

#endif
