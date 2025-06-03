#include "cell.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define SPACE " \t\r\n"
/* Global status variable for tracking command execution results */
int	status = 0;

t_builtin	g_builtin[] = 
{
	{.builtin_name = "echo", .foo=cell_echo},
	{.builtin_name = "env", .foo=cell_env},
	{.builtin_name = "exit", .foo=cell_exit},
	{.builtin_name = "pwd", .foo=cell_pwd},
        {.builtin_name = "clear", .foo=cell_clear},
        {.builtin_name = "help", .foo=cell_help},
        {.builtin_name = "history", .foo=cell_history},
        {.builtin_name = "date", .foo=cell_date},
        {.builtin_name = "whoami", .foo=cell_whoami},
        {.builtin_name = "uptime", .foo=cell_uptime},
        {.builtin_name = "touch", .foo=cell_touch},
        {.builtin_name = "alias", .foo=cell_alias},
        {.builtin_name = "kill", .foo=cell_kill},
	{.builtin_name = NULL},
};

const char *builtin_cmds[] = {
	"echo", "env", "exit", "pwd", "clear", "help", "history", "date", "whoami", "uptime", "touch", NULL
};

char *command_generator(const char *text, int state) {
	static int list_index, len;
	const char *name;

	if (!state) {
		list_index = 0;
		len = strlen(text);
	}

	while ((name = builtin_cmds[list_index++])) {
		if (strncmp(name, text, len) == 0) {
			return strdup(name);
		}
	}
	return NULL;
}

char **cell_completion(const char *text, int start, int end) {
	(void)start; (void)end;
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, command_generator);
}

char *cell_read_line(void) {
    char cwd[BUFSIZ];
    char prompt[BUFSIZ + 64];
    char *line;
    if (status)
        snprintf(prompt, sizeof(prompt),
            "🦠"C"[%s]"RED"[%d]"RST"🦠 > ",
            Getcwd(cwd, BUFSIZ), status);
    else
        snprintf(prompt, sizeof(prompt),
            "🦠"C"[%s]"RST"🦠 > ",
            Getcwd(cwd, BUFSIZ));
    line = readline(prompt);
    if (line && *line)
        add_history(line);
    return line;
}

void cell_launch(char **args) {
    int in_redirect = -1, out_redirect = -1, append = 0;
    // Tìm redirect
    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], "<") == 0 && args[i+1]) {
            in_redirect = i;
        } else if (strcmp(args[i], ">") == 0 && args[i+1]) {
            out_redirect = i; append = 0;
        } else if (strcmp(args[i], ">>") == 0 && args[i+1]) {
            out_redirect = i; append = 1;
        }
    }

    pid_t pid = Fork();
    if (pid == 0) {
        if (in_redirect != -1) {
            int fd = open(args[in_redirect+1], O_RDONLY);
            if (fd == -1) { perror("open"); exit(1);}
            dup2(fd, STDIN_FILENO); close(fd);
            args[in_redirect] = NULL;
        }
        if (out_redirect != -1) {
            int fd;
            if (append)
                fd = open(args[out_redirect+1], O_WRONLY|O_CREAT|O_APPEND, 0644);
            else
                fd = open(args[out_redirect+1], O_WRONLY|O_CREAT|O_TRUNC, 0644);
            if (fd == -1) { perror("open"); exit(1);}
            dup2(fd, STDOUT_FILENO); close(fd);
            args[out_redirect] = NULL;
        }
        Execvp(args[0], args); // chỉ phần trước redirect
        perror("execvp"); exit(1);
    } else {
        Wait(&status);
    }
}
#define ALIAS_RECUR_LIMIT 10

void cell_execute(char **args) {
    static int alias_depth = 0;
    int i;
    const char *curr_builtin;

    if (!args || !args[0])
        return;

    const char* alias_value = get_alias(args[0]);
    if (alias_value && alias_value[0]) {
        if (alias_depth > ALIAS_RECUR_LIMIT) {
            fprintf(stderr, "Alias recursion too deep!\n");
            return;
        }
        alias_depth++;
        char new_cmd[512] = {0};
        snprintf(new_cmd, sizeof(new_cmd), "%s", alias_value);
        for (int j = 1; args[j]; j++) {
            strcat(new_cmd, " ");
            strcat(new_cmd, args[j]);
        }
        char **new_args = cell_split_line(new_cmd);
        cell_execute(new_args);
        for (int i = 0; new_args[i]; i++) free(new_args[i]);
        free(new_args);
        alias_depth--;
        return;
    }
    i = 0;
    while ((curr_builtin = g_builtin[i].builtin_name)) {
        if (!strcmp(args[0], curr_builtin)) {
            if ((status = (g_builtin[i].foo)(args)))
                p("%s failed\n", curr_builtin);
            return ;
        }
        i++;
    }
    cell_launch(args);
}

char **cell_split_line(char *line) {
    size_t bufsize = BUFSIZ;
    unsigned long position = 0;
    char **tokens = malloc(bufsize * sizeof *tokens);
    if (!tokens) { perror("malloc"); exit(EXIT_FAILURE); }

    char *p = line, *start;
    while (*p) {
        while (*p && strchr(SPACE, *p)) p++;
        if (!*p) break;
        if (*p == '|' || *p == '<' || *p == '>') {
            int len = 1;
            if (*p == '>' && *(p+1) == '>') len = 2; // phát hiện >>
            char op[3] = {0};
            strncpy(op, p, len);
            tokens[position++] = strdup(op);
            p += len;
        } else {
            start = p;
            while (*p && !strchr(SPACE, *p) && *p != '|' && *p != '<' && *p != '>') p++;
            size_t len = p - start;
            char *tok = malloc(len+1);
            strncpy(tok, start, len); tok[len] = 0;
            tokens[position++] = tok;
        }
        if (position >= bufsize) {
            bufsize *= 2;
            tokens = realloc(tokens, bufsize * sizeof *tokens);
            if (!tokens) { perror("realloc"); exit(EXIT_FAILURE); }
        }
    }
    tokens[position] = NULL;
    return tokens;
}
void cell_pipe(char **args) {
    int i = 0;
    while (args[i] && strcmp(args[i], "|") != 0) i++;
    if (!args[i]) { cell_launch(args); return; }

    args[i] = NULL; // tách hai lệnh
    char **args1 = args;
    char **args2 = &args[i+1];

    int fd[2];
    pipe(fd);
    pid_t pid1 = Fork();
    if (pid1 == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]); close(fd[1]);
        Execvp(args1[0], args1);
        perror("execvp"); exit(1);
    }
    pid_t pid2 = Fork();
    if (pid2 == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]); close(fd[1]);
        Execvp(args2[0], args2);
        perror("execvp"); exit(1);
    }
    close(fd[0]); close(fd[1]);
    Wait(&status); Wait(&status);
}
static inline int has_pipe(char **args) {
    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], "|") == 0)
            return 1;
    }
    return 0;
}
int main() {
    char *line;
    char **args;

    rl_attempted_completion_function = cell_completion;
    while ((line = cell_read_line())) {
        args = cell_split_line(line);
        if (args[0] && !strcmp(args[0], "cd")) {
            if (args[1]) {
                Chdir(args[1]);
            } else {
                fprintf(stderr, "cd: missing operand\n");
            }
        } else if (args[0] && has_pipe(args)) {
            // Nếu có pipe, gọi hàm xử lý pipe
            cell_pipe(args);
        } else {
            cell_execute(args);
        }
        for (int i = 0; args[i]; i++) free(args[i]);
        free(args);
        free(line);
    }
    return (EXIT_SUCCESS);
}
