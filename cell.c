#include "cell.h"

/* Global status variable for tracking command execution results */
int	status = 0;

/*
** Array of built-in commands with their handlers
** Terminated by a NULL sentinel for easy iteration
** Currently supports: echo, env, exit
*/
t_builtin	g_builtin[] = 
{
	{.builtin_name = "echo", .foo=cell_echo},   /* Echo text to stdout */
	{.builtin_name = "env", .foo=cell_env},     /* Print environment */
	{.builtin_name = "exit", .foo=cell_exit},  /* Exit the shell */
	{.builtin_name = NULL},                    /* Sentinel */
};


/**
 * cell_launch - Creates a child process to execute external commands
 * @args: Array of command arguments
 * Return: 1 on success, 0 on failure
 * Corner cases:
 * - NULL args: No action taken
 * - Fork failure: Error message printed, shell continues
 * - Execvp failure: Child process exits with failure status
 */
void	cell_launch(char **args)
{

	// Is fork returning 2 values?
	if (Fork() == CELL_JR)
	{
		// replace the current process image with a new process image
		//v for "vector", p for "path".
		//Takes an array of arguments and uses PATH to find the executable.
		//	char *args[] = {"ls", "-l", "-a", NULL};
		//	execvp("ls", args);
		Execvp(args[0], args);
	}
	else
	{
		//Automatically waits for any child process, 
		//which is often sufficient for beginner-level 
		//shells that only handle one child process at a time.
		// FINE for CELL v°1
		Wait(&status);

		/*
		Waitpid(cell_jr, &status, 0);
		*/
	}
}

/**
 * cell_execute - Executes built-in or external commands
 * @args: Array of command arguments
 * Return: 1 to continue shell, 0 to terminate
 */
void	cell_execute(char **args)
{
	int			i;
	const char	*curr_builtin;

	if (!args || !args[0])
		return ;
	i = 0;

	// 1) check if builtin requested
	while ((curr_builtin = g_builtin[i].builtin_name))
	{
		if (!strcmp(args[0], curr_builtin))
		{
			// 2) Run builtin
			if ((status = (g_builtin[i].foo)(args)))
				p("%s failed\n", curr_builtin);
			return ;
		}
		i++;
	}
	
	// 2) fork and launch 
	cell_launch(args);
}

/**
 * cell_read_line - Reads a line from standard input
 * Return: Pointer to the read line or NULL on failure
 */
char	*cell_read_line(void)
{
	char	*line;
	size_t	bufsize;
	char	cwd[BUFSIZ];

	line = NULL;
	bufsize = 0;
	// check if interactive mode 
	if (isatty(fileno(stdin)))
	{
		if (status)
			p("🦠"C"[%s]"RED"[%d]"RST"🦠 > ", 
					Getcwd(cwd, BUFSIZ), 
					status);
		else
			p("🦠"C"[%s]"RST"🦠 > ", 
					Getcwd(cwd, BUFSIZ));
	}

	Getline(&line, &bufsize, stdin);
	return (line);
}


/**
 * cell_split_line - Splits input line into tokens
 * @line: Input line to be tokenized
 * Return: Array of tokens or NULL on failure
 */
char	**cell_split_line(char *line)
{
	size_t			bufsize;
	unsigned long	position;
	char			**tokens;

	bufsize = BUFSIZ;
	position = 0;
	tokens = Malloc(bufsize * sizeof *tokens);

	for (char *token = strtok(line, SPACE); token; token = strtok(NULL, SPACE))
	{
		tokens[position++] = token;	
		if (position >= bufsize)
		{
			bufsize *= 2;
			tokens = Realloc(tokens, bufsize * sizeof *tokens);
		}
	}
	tokens[position] = NULL;
	return (tokens);
}


/**
 * main - Entry point for the shell program
 * @argc: Argument count (unused)
 * @argv: Argument vector (unused)
 * Return: EXIT_SUCCESS on normal termination
 */
int	main()
{
	// Load config files, if any, not in our Imperfect Cell
	// 💥 stuff for cell perfect
	
	//printbanner();
	
	char	*line;
	char	**args;

	// READ->EVAL->EXECUTE->LOOP
	// 1) Get the line, loop until there is one 
	// 		cell perfect use readline
	while ((line = cell_read_line()))
	{

		// 2) Tokens
		args = cell_split_line(line);	

		// 2.1) check if cd command
		if (args[0] && !strcmp(args[0], "cd"))
			Chdir(args[1]);
		
		// 3) Exec
		cell_execute(args);

		// 4) Clean🧹
		free(line);
		free(args);
	

	}

	return (EXIT_SUCCESS);
}
