#include "cell.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <stdlib.h>

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

void	cell_launch(char **args) {
	if (Fork() == CELL_JR) {
		Execvp(args[0], args);
	} else {
		Wait(&status);
	}
}

void	cell_execute(char **args) {
	int			i;
	const char	*curr_builtin;

	if (!args || !args[0])
		return ;
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

    for (char *token = strtok(line, SPACE); token; token = strtok(NULL, SPACE)) {
        tokens[position++] = token;	
        if (position >= bufsize) {
            bufsize *= 2;
            tokens = realloc(tokens, bufsize * sizeof *tokens);
            if (!tokens) { perror("realloc"); exit(EXIT_FAILURE); }
        }
    }
    tokens[position] = NULL;
    return tokens;
}

int	main() {
	char	*line;
	char	**args;

	rl_attempted_completion_function = cell_completion;
	while ((line = cell_read_line())) {
		args = cell_split_line(line);
		if (args[0] && !strcmp(args[0], "cd")) {
			if (args[1]) Chdir(args[1]);
			else fprintf(stderr, "cd: missing operand\n");
		} else {
			cell_execute(args);
		}
		free(line);
		free(args);
	}
	return (EXIT_SUCCESS);
}
