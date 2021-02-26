/**
 * Session
 * 
 * Contains all the manipulations and data fro creating a session and authenticating a user
 * into the session.
 *
 * */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "session.h"
#include "hashing.h"

char username[USERNAME_MAX_CHAR];
char password[PASSWORD_MAX_CHAR];

void strip_newline(char *s) {
    while (*s != '\0') {
        if (*s == '\r' || *s == '\n') {
            *s = '\0';
        }
        s++;
    }
}

char *input_username() {
    fflush(stdin);
    printf("Input username> ");
    fgets(username, USERNAME_MAX_CHAR + 1, stdin);
    strip_newline(username);
    return username;
}

char *input_password() {
    fflush(stdin);
    printf("Input password> ");
    system("stty -echo");
    fgets(password, PASSWORD_MAX_CHAR + 1, stdin);
    strip_newline(password);
    char *hashed = getHash(password);
    system("stty echo");
    printf("\n");
    return hashed;
}

void set_session(Session *session, Userdata data, int pos) {
    int position;
    if (session->last >= MAX_USER_SESSION) {
        printf("User is already in session!\n");
        exit(EXIT_FAILURE);
    } else if ((position > session->last + 1) || (position < 0)) {
        printf("Position is full!\n");
        exit(EXIT_FAILURE);
    } else {
        for (position = session->last; position >= pos; position--)
            session->user[position + 1] = session->user[position];
        session->last++;
        session->user[pos] = data;
    }
}

void insert_session(User *user, int pos, Session *session, char *username, char *password) {
    for (int index = 0; index <= user->row; index++) {
        if (strcmp(username, user->user[index].username) == 0 && strcmp(password, user->user[index].password) == 0) {
            set_session(session, user->user[index], pos);
            break;
        }
    }
}

int authentication(User *user, char *username, char *password) {
    Session session;
    session.last = -1;
    int num = 0;
    do {
        int result = search(user, username, password);
        if (result == 0) {
            printf("Wrong input!! \n");
            username = input_username();
            password = input_password();
            num++;
            continue;
        } else if (result == 1) {
            printf("Hello %s \n", username);
            insert_session(user, session.last + 1, user, username, password);
            return 1;
        } else if (num == 3) {
            printf("Too many tries try again!");
            break;
        }
    } while (1);
    return 0;
}

int search(User *user, char *username, char *password) {
    for (int index = 0; index <= user->row; index++) {
        if ((strcmp(user->user[index].username, username) == 0) &&
            (strcmp(user->user[index].password, password) == 0)) {
            return 1;
        }
    }
    return 0;
}

void show_user_session(Session *session) {
    for (int index = 0; index < MAX_USER_SESSION; index++) {
        printf("User in session: %s\n", session->user[index].username);
    }
}