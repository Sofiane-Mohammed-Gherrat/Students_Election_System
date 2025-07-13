#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "models.h"
#include "fileio.h"
#include "utils.h"
#include "admin.h"
#include "rep.h"
#include "student.h"

//TODO: We could Add a hashing function for storing the PW more securely but needs installation for openssl 
//! the roles : 
    //? 0 == admin
    //? 1 == rep
    //? 2 == student

void Welcoming_message(){
    printf("=================================================\n");
    printf("🔎 Welcome to the student election system\n");
    printf("🔎 This system allows you to:\n    • Login as the admin.\n    • Register/login as a student.\n    • Register/login as representative.\n\n");
}


int main(void) {
    //* for initializin the admin account
    initial_admin_setup();
    
    Welcoming_message();
    while (1) {
        int opt = main_prompt();
        if (opt == 0) break;

        char uname[USERNAME_LEN], pass[PASS_LEN];
        
        //* Loging Flow 
        User current;
        if (opt == 2){
            printf("\nEnter The:\n");
            get_string("Username", uname, USERNAME_LEN);
            get_string("Password", pass, PASS_LEN);
        
            if (authenticate(uname, pass, &current)) {
                if (current.role == ROLE_ADMIN) {
                    printf("🔐 Admin login successful.\n");
                    admin_menu(&current);
                } else if (current.role == ROLE_REP) {
                    rep_menu(&current);
                } else {
                    student_menu(&current);
                }
            }
            else{
                printf("\n❌ Wrong credentials\nRetry\n\n");
            }
        }
        //* Registration
        else if (opt == 1) {

            // 1. Choose role
            printf("\nChoose to register as:\n 1. Student Representative\n 2. Student\nSelect (1–2): ");
            int role_choice = get_int(1, 2);

            // 2. Load existing users and add new one
            User *users = NULL;
            int cnt = load_users(&users);

            // 3. Get base credentials
            //! checking user name availibility in a loop
            bool taken = true;
            printf("\nEnter The:\n");
            while (taken){
                get_string("Username", uname, USERNAME_LEN);
                if (username_exists(users, cnt, uname)){
                    printf("❌ Username already exists!\n");
                    //! allowed chars ??
                    if (!valid_username(uname)){
                        username_guideline();
                        continue;
                    }
                }
                else
                    taken = false;
            }
            printf("\n✅ Username accepted\n");
            //! strong enough pw in a loop 
            bool strong = false;
            printf("\nEnter The:\n");
            while (!strong){
                get_string("Password", pass, PASS_LEN);
                if (!is_strong_password(pass)){
                    printf("\n❌ Weak password.\n");
                    password_guideline();
                }
                else
                    strong = true;
            }
            printf("\n✅ Password accepted\n");
            
            users = realloc(users, (cnt + 1) * sizeof *users);
            strcpy(users[cnt].username, uname);
            strcpy(users[cnt].password, pass);
            users[cnt].role = (role_choice == 1 ? ROLE_REP : ROLE_STUDENT);

            save_users(users, cnt + 1);
            free(users);

            printf("\n✅ Registration complete! You are now a %s.\n\n",
                role_choice == 1 ? "STUDENT REPRESENTATIVE" : "STUDENT");
        }

        //? How it works
        /* Register as:
            1) Student Representative
            2) Student
            Select (1–2): */
    }
    printf("\n🛑 Exiting, goodbye!\n");
    return 0;
}
