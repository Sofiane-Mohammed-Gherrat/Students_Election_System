#ifndef MODELS_H
#define MODELS_H

#define USERNAME_LEN 32
#define PASS_LEN 32
#define MANIFESTO_LEN 512
#define INIT_ADMIN_USERNAME "SCDS"
#define INIT_ADMIN_PASSWORD "202504"

#define Manifesto_Path "manifestos.txt"
#define Users_Path "users.txt"
#define Votes_Path "votes.txt"
#define Results_Path "results.txt"
#define Vote_Updates_Path "votes_updates.txt"

typedef enum {
    ROLE_ADMIN = 0,
    ROLE_REP,
    ROLE_STUDENT
} Role;

// User account information
typedef struct {
    char username[USERNAME_LEN];
    char password[PASS_LEN];
    Role role;
} User;

// Representative manifesto
typedef struct {
    char rep_username[USERNAME_LEN];
    char manifesto[MANIFESTO_LEN];
} Manifesto;

// A student’s vote record
typedef struct {
    char student_username[USERNAME_LEN];
    char rep_username[USERNAME_LEN];
} Vote;

#endif
