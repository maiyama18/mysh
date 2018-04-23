#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD_LEN 80
#define MAX_PATH_LEN 80
#define MAX_USERNAME_LEN 80

#define MAX_ARGS_NUM 10

// todo: PATHを探索
// todo: statusによって色変更
// todo: history
void print_prompt();
int split(char *input, char *output[], char *sep);

int main() {
    while (1) {
        char *input;
        char *args[MAX_ARGS_NUM];

        print_prompt();

        fgets(input, MAX_CMD_LEN, stdin);
        split(input, args, " \n");

        pid_t pid = fork();
        int status;
        if (pid == 0) { // child process
            status = execv(args[0], args);
        } else {
            wait(&status);
        }
    }
}

void print_prompt() {
    char username[MAX_USERNAME_LEN], *path, buf[MAX_PATH_LEN];
    getlogin_r(username, MAX_USERNAME_LEN);
    path = getcwd(buf, MAX_PATH_LEN);

    printf("%s@%s -> ", username, path);
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