#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "admin.h"
#include "fileio.h"
#include "utils.h"


/**
 * ? Ensure an initial admin account is created on first run.
 *
 * If no users exist (count == 0 after loading), this function:
 *  - Prompts for creation of an admin account.
 *  - Uses predefined INIT_ADMIN_USERNAME and INIT_ADMIN_PASSWORD.
 *  - Allocates or reallocates the users array to length 1.
 *  - Saves the new user with role = ROLE_ADMIN.
 *
 * No parameters.
 * @return Nothing.
 *
 * *Usage:
 *  - Called at program startup.
 *  - Ensures there's always at least one admin.
 */
void initial_admin_setup() {
    User *users = NULL;
    int count = load_users(&users);
    if (count == 0) {
        printf("\n=== Initial Admin Setup ===\n");
        printf("Create Admin Account\n");
        char uname[USERNAME_LEN], pass[PASS_LEN];

        strncpy(uname, INIT_ADMIN_USERNAME, USERNAME_LEN);
        strncpy(pass, INIT_ADMIN_PASSWORD, PASS_LEN);

        users = realloc(users, sizeof(User));
        strcpy(users[0].username, uname);
        strcpy(users[0].password, pass);
        users[0].role = ROLE_ADMIN;


        save_users(users, 1);
        free(users);

        printf("[SUCCESS] Admin account initialized.\n\n");
    }
}

/**
 * ? Check if the first line of the users file contains the default admin credentials.
 *
 * Validates whether the first line in `Users_Path` matches:
 *   SCDS 202504 0
 *
 * @return `1` if the first line matches exactly (valid default admin),
 *         `0` otherwise (mismatch or file error).
 *
 * * Usage:
 *   - Called during login and registration to ensure the default admin is always present.
 * @note Doesn't allocate memory; uses file I/O to inspect only the first line.
 */
int check_default_admin_at_top(void) {
    FILE *f = fopen(Users_Path, "r");
    if (!f) return 0;  // cannot read => treat as missing or invalid

    char username[USERNAME_LEN], password[PASS_LEN];
    int role;
    int ret = 0;
    if (fscanf(f, "%31s %31s %d", username, password, &role) == 3) {
        if (strcmp(username, "SCDS") == 0 &&
            strcmp(password, "202504") == 0 &&
            role == 0) {
            ret = 1;
        }
    }
    fclose(f);
    return ret;
}

/**
 * ? Ensure default admin credentials are the first line in the users file.
 *
 * If `check_default_admin_at_top()` returns 0, this function:
 *   - Reads all user entries,
 *   - Inserts the default admin record at the beginning,
 *   - Removes other admin-role (0) users,
 *   - Writes the updated list back to `Users_Path`.
 *
 * * Usage:
 *   - Called during login and registration to ensure the default admin is always present.
 * @return `0` on success, `-1` on I/O or memory errors.
 *
 * @note Uses dynamic reloading of users via load_users() and save_users().
 */
int enforce_default_admin_top(void) {
    if (check_default_admin_at_top()) return 0;

    User *users = NULL;
    int count = load_users(&users);

    if (!users && count > 0) return -1;

    User *filtered = malloc((count + 1) * sizeof *filtered);
    if (!filtered) { free(users); return -1; }

    // Insert default admin first
    strcpy(filtered[0].username, "SCDS");
    strcpy(filtered[0].password, "202504");
    filtered[0].role = 0;

    int idx = 1;
    for (int i = 0; i < count; i++) {
        if (!(users[i].role == 0 &&
                strcmp(users[i].username, "SCDS") != 0)) {
            filtered[idx++] = users[i];
        }
    }

    int result = save_users(filtered, idx);
    free(users);
    free(filtered);
    return result;
}

/**
 * ? Ensure only one genuine default admin exists and is correctly positioned.
 *
 * 1. If the first line doesn't match the default admin, call enforce_default_admin_top()
 *    to insert or fix it at the top.
 * 2. Load all users.
 * 3. Filter the list:
 *      • Keep exactly one correct default admin ("SCDS", "202504", role 0),
 *      • Include all non-admin users,
 *      • Discard any extra/invalid admins.
 * 4. Save the updated list back to disk.
 *
 * @return 0 on success; -1 on failure (I/O or memory error).
 */
int verify_and_clean_admins(void) {
    // Step 1: Ensure default admin is at the top
    if (!check_default_admin_at_top()) {
        if (enforce_default_admin_top() != 0) {
            return -1;
        }
    }

    // Step 2: Load all users
    User *users = NULL;
    int count = load_users(&users);
    if (!users && count > 0) {
        return -1;
    }

    // Step 3: Filter users
    User *filtered = malloc(count * sizeof *filtered);
    if (!filtered) {
        free(users);
        return -1;
    }

    int keep = 0;
    bool keptAdmin = false;

    for (int i = 0; i < count; i++) {
        if (users[i].role == 0) {
            if (!keptAdmin
                && strcmp(users[i].username, "SCDS") == 0
                && strcmp(users[i].password, "202504") == 0) {
                filtered[keep++] = users[i];
                keptAdmin = true;
            }
            // else skip extra admin entries
        } else {
            filtered[keep++] = users[i];
        }
    }

    free(users);

    // Step 4: Save filtered list
    int result = save_users(filtered, keep);
    free(filtered);
    return result;
}

void admin_actions(){
    printf("\nAs an admin you can:\n");
    printf("  • View a list of registered student representatives.\n");
    printf("  • View the total number of votes each representative has received.\n");
    printf("  • Publish and display the final election results.\n\n");
}

/**
 * ? List all registered student representatives.
 *
 * Loads representatives and displays their usernames. If none are found,
 * prints a warning and returns 0; otherwise prints count and returns repCount.
 *
 * @return The number of representatives (0 if none found).
 *
 * * Usage:
 *  - Called when admin selects "list representatives".
 *  - Helps admins verify which reps are registered.
 */
int Student_rep_list(void) {
    User *reps = NULL;
    int repCount = load_reps(&reps);  // load only reps

    if (repCount == 0) {
        printf("\n[WARNING] No registered student representatives found.\n");
        return 0;
    }
    else {
        printf("\n[SUCCESS] Representatives loaded successfully.\n");
        printf("=================================================\n\n");
        printf("Registered Student Representatives:\n");
        for (int i = 0; i < repCount; i++) {
            printf("  • %s\n", reps[i].username);
        }
    }

    free(reps);
    return repCount;
}

/**
 * ? Display the current vote count for each representative.
 *
 * Loads all reps and all votes, then:
 *  - If no reps exist: warns and exits.
 *  - Calculates max username length to align output.
 *  - Counts votes per rep and displays formatted results.
 *
 * * Usage:
 *  - Called when admin selects "view votes".
 *  - Provides a snapshot of ongoing vote tallies.
 */
void Display_votes(void) {
    User *reps = NULL;
    int repCount = load_reps(&reps);

    Vote *votes = NULL;
    int voteCount = load_votes(&votes);

    printf("\n\nCurrent vote counts:\n");
    if (repCount == 0) {
        printf("[WARNING] No representatives found.\n");
        free(reps); free(votes);
        return;
    }

    int maxNameLen = 0;
    for (int i = 0; i < repCount; i++)
        if ((int)strlen(reps[i].username) > maxNameLen)
            maxNameLen = strlen(reps[i].username);

    for (int i = 0; i < repCount; i++) {
        int cnt = 0;
        for (int j = 0; j < voteCount; j++)
            if (strcmp(votes[j].rep_username, reps[i].username) == 0)
                cnt++;

        printf(" • %-*s : %*d votes\n",
                maxNameLen,
                reps[i].username,
               4,  // lets assume vote counts won't exceed 9999
                cnt);
    }

    free(reps); free(votes);
}


/**
 * ? Tally votes and publish election results.
 *
 * @param mfs       Array of manifestos (one per candidate).
 * @param mfCount   Number of manifestos.
 * @param votes     Array of all votes cast.
 * @param voteCount Total number of votes.
 *
 * This function:
 *  - Allocates an integer tally array (initialized to zero).
 *  - Increments the count when `votes[i].rep_username` matches `mfs[j].rep_username`.
 *  - Saves results via save_results().
 *  - Prints success or error on memory failure.
 *
 * * Usage:
 *  - Called when admin chooses to publish results (opt == 3).
 *  - Outputs final vote counts to storage and informs the admin.
 */
void publish_results(Manifesto *mfs, int mfCount, Vote *votes, int voteCount) {
    int *counts = calloc(mfCount, sizeof *counts);
    if (!counts) {
        fprintf(stderr, "[ERROR] Out of memory during tally.\n");
        return;
    }

    for (int i = 0; i < voteCount; i++) {
        for (int j = 0; j < mfCount; j++) {
            if (strcmp(votes[i].rep_username, mfs[j].rep_username) == 0) {
                counts[j]++;
                break;  // Found the candidate; no need to check further
            }
        }
    }

    save_results(mfs, counts, mfCount);
    printf("\n[SUCCESS] Results published.\n");
    free(counts);
}

/**
 * ? Admin-level interactive menu loop.
 *
 * @param current  Pointer to the currently logged-in admin User.
 *
 * ! Displays a welcome banner, shows available actions, and enters a loop that:
 *  1. Prompts for choice via admin_prompt().
 *  2. On '0': logs out and exits loop.
 *  3. On '1': lists reps.
 *  4. On '2': displays vote counts.
 *  5. On '3': publishes results, marks them published, and displays status.
 *  6. On invalid choice: prints error and repeats.
 * Loads and frees manifestos and votes for options 1–3.
 *
 * * Usage:
 *  - Called once admin successfully logs in.
 *  - Drives all high-level admin interactions until logout.
 */
void admin_menu(const User *current) {
    printf("\n=================================");
    printf("\n      [Welcome] Admin: %s\n", current->username);
    printf("=================================\n");
    printf("[ADMIN] Admin menu loaded successfully.\n");

    //* what you can do
    admin_actions();

    while (1) {
        int opt = admin_prompt();
        if (opt == 0) {
            logging_out();
            break;
        }
        Manifesto *mfs; int mfCount = load_manifestos(&mfs);
        Vote *votes; int voteCount = load_votes(&votes);

        //! Rep list
        if (opt == 1) {
            Student_rep_list();
        } 
        //! vote count
        else if (opt == 2) {
            Display_votes();
        }
        //! publish results 
        else if (opt == 3 && 
                (check_default_admin_at_top() == 1)) {
            publish_results(mfs, mfCount, votes, voteCount);
            // Mark results as published
            mark_results_published();
            // Display the status of results
            display_result_status();
    } else {
        printf("[Error] Invalid option. Please try again.\n");
        continue;
    }
    // Cleanup
        free(mfs);
        free(votes);
    }
}
