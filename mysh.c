#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAX_CMD_LEN 80
#define MAX_PATH_LEN 80
#define MAX_USERNAME_LEN 80

#define MAX_SYSPATH_NUM 40
#define MAX_ARGS_NUM 10

// todo: PATHを探索
// todo: statusによって色変更
// todo: history

int num_syspath;
char *syspath[MAX_SYSPATH_NUM];

void get_syspath();
void print_prompt();
int parse_input(char *cmd, char *args[]);
int split(char *input, char *output[], char *sep);

int main() {
    get_syspath();

    while (1) {
        char cmd[MAX_CMD_LEN];
        char *args[MAX_ARGS_NUM];
        for (int i = 0; i < MAX_ARGS_NUM; i++) {
            args[i] = NULL;
        }
        print_prompt();

        int num_args = parse_input(cmd, args);
        printf("before fork\n");
        printf("cmd: %s\n", cmd);
        for (int i = 0; i < num_args; i++) {
            printf("args[%d]: %s\n", i, args[i]);
        }

        pid_t pid = fork();
        int status;
        if (pid == 0) { // child process
            printf("after fork (child)\n");
            printf("cmd: %s\n", cmd);
            for (int i = 0; i < num_args; i++) {
                printf("args[%d]: %s\n", i, args[i]);
            }
            status = execv(cmd, args);
        } else {
            printf("after fork (parent)\n");
            printf("cmd: %s\n", cmd);
            for (int i = 0; i < num_args; i++) {
                printf("args[%d]: %s\n", i, args[i]);
            }
            wait(&status);
        }
    }
}

void get_syspath() {
    char *path_str = getenv("PATH");
    num_syspath = split(path_str, syspath, ":");
}

void print_prompt() {
    char username[MAX_USERNAME_LEN], *path, buf[MAX_PATH_LEN];
    getlogin_r(username, MAX_USERNAME_LEN);
    path = getcwd(buf, MAX_PATH_LEN);

    printf("%s@%s -> ", username, path);
}

int parse_input(char *cmd, char *args[]) {
    int num_args;
    char input[MAX_CMD_LEN];
    char input_cmd[MAX_CMD_LEN];

    fgets(input, MAX_CMD_LEN, stdin);
    num_args = split(input, args, " \n");

    sprintf(input_cmd, "%s", args[0]);

    for (int i = 0; i < num_syspath; i++) {
        char *cmd_cand[MAX_CMD_LEN];
        sprintf(cmd_cand, "%s/%s", syspath[i], input_cmd);

        if (access(cmd_cand, 0) == 0) {
            strcpy(cmd, cmd_cand);

            break;
        }
    }

    return num_args;
}

int split(char *input, char *output[], char *sep) {
    int index = 0;
    char *p;

    p = strtok(input, sep);
    while (p != NULL) {
        output[index++] = p;
        p = strtok(NULL, sep);
    }
    output[index] = NULL;

    return index;
}