#include <stdio.h>
#include "main.h"
#include "session.h"
#include "database.h"
#include <stdbool.h>

void fflush_stdin()
{ int c; while ((c = getchar()) != '\n' && c != EOF); }

void start_db(Table *table, User* user){
    InputBuffer *input_buffer = new_input_buffer();
    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer, table, user)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_NEGATIVE_ID):
                printf("ID must be positive.\n");
                continue;
            case (PREPARE_STRING_TOO_LONG):
                printf("String is too long.\n");
                continue;
            case (PREPARE_SYNTAX_ERROR):
                printf("Syntax error. Could not parse statement.\n");
                continue;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n",
                       input_buffer->buffer);
                continue;
        }

        switch (execute_statement(&statement, table)) {
            case (EXECUTE_SUCCESS):
                printf("Executed.\n");
                break;
            case (EXECUTE_DUPLICATE_KEY):
                printf("Error: Duplicate key.\n");
                break;
        }
    }
}

bool isAuth(User* user){
    char *username = input_username();
    char *password = input_password();
    int result = authentication(user, username, password);
    if(result == 1){
        return true;
    }
    return false;
}

void print_welcome() {
    printf(
            "<<PassMan>> \n"
            "Welcome to PassMan \n"
            "Version 1.0 Filip Ivanusec \n"
            "\n"
            "\n"
            "Use .help for simple tutorial on how to use PassMann"
    );
}

int print_input(User *user) {
    int result = check_user_file();
    if (result == 1) {
        printf(
                "I see you have some users! \n"
                "Let's authenticate you \n"
        );
        load(user);
    } else {
        printf(
                "First timer eh? \n"
                "Let's get you setup \n"
        );
        input_user(user);
        save(user);
    }
}

int main() {
    User user;
    user.row = -1;

    print_welcome();
    print_input(&user);

    bool auth = isAuth(&user);

    if(auth) {
        Table *table = db_open(FILENAME);
        fflush_stdin();
        start_db(table, &user);
    }
}