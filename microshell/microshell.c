#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int ft_strlen(char *s) {
    int i = 0;
    while (s[i])
        i++;
    return i;
}
void put_err( char *s1,  char *s2) {
    write(2, s1, ft_strlen(s1));
    write(2, s2, ft_strlen(s2));
    write(2, "\n", 1);
}
void execute(char **argv, char **env, int i){
    argv[i] = NULL;
    execve(argv[0], argv, env);
    put_err("error: cannot execute ", argv[0]);
    exit(1);
}
int main(int argc, char **argv, char **env){
    int fd[2];
    int i = 0;
    if (argc == 1)
        return 0;
    while (argv[i]) {
        argv = &argv[i + 1];
        i = 0;
        while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
            i++;
        if (i != 0 && strcmp(argv[0], "cd") == 0) {
            if (i != 2)
                put_err("error: cd: bad arguments", "");
            else if (chdir(argv[1]) != 0)
                put_err("error: cd: cannot change directory to ", argv[1]);
        } else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0)) {
            if (fork() == 0)
                execute(argv, env, i);
            else
                wait(NULL);
        } else if (i != 0 && strcmp(argv[i], "|") == 0) {
            pipe(fd);
            if (fork() == 0) {
                dup2(fd[1], 1);
                close(fd[1]);
                close(fd[0]);
                execute(argv, env, i);
            } else {
                dup2(fd[0], 0);
                close(fd[1]);
                close(fd[0]);
                wait(NULL);
            }
        }
    }
    return 0;
}