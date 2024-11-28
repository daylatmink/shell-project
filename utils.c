#include "cell.h"

/**
 * Chdir - Changes current working directory with error handling
 * @path: Directory path to change to
 * Corner cases:
 * - NULL path: prints error
 * - Non-existent path: prints error
 * - Permission denied: prints error
 */
void	Chdir(const char *path)
{
	if (!path)
	{
		fprintf(stderr, RED"cd: path argument required\n"RST);
		return ;
	}
	if (chdir(path) == -1)
		perror(RED"cd failed"RST);
}

/**
 * Fork - Creates a new process with error handling
 * Return: PID of child on success, -1 on failure
 * Corner cases:
 * - System resource limits: prints error and exits
 * - Process table full: prints error and exits
 */
pid_t	Fork(void)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror(RED"Fork failed"RST);
		exit(EX_OSERR);
	}
	return (pid);
}

/**
 * Execvp - Executes a program with error handling
 * @file: Name of the program to execute
 * @argv: Array of arguments
 * Corner cases:
 * - NULL file/argv: prints error and exits
 * - Command not found: prints error and exits
 * - Permission denied: prints error and exits
 */
void	Execvp(const char *file, char *const argv[])
{
	if (!file || !argv)
	{
		fprintf(stderr, RED"Execvp: invalid arguments\n"RST);
		exit(EXIT_FAILURE);
	}
	if (execvp(file, argv) == -1)
	{
		perror(RED"💥CELL_Jr failed💥"RST);
		exit(EX_UNAVAILABLE);
	}
}


/**
 * Wait - Waits for any child process to terminate with error handling
 * @status: Location to store status information
 * Return: PID of the terminated child on success, -1 on failure
 * Corner cases:
 * - NULL status: prints error and returns -1
 * - No child processes: prints error
 */
pid_t	Wait(int *status)
{
	pid_t	result;

	if (!status)
	{
		fprintf(stderr, RED"Wait: status argument required\n"RST);
		return (-1);
	}
	result = wait(status);
	if (result == -1)
		perror(RED"Wait failed"RST);
	if (WIFEXITED(*status))
		*status = WEXITSTATUS(*status); 
	return (result);
}

/**
 * Waitpid - Waits for process termination with error handling
 * @pid: Process ID to wait for
 * @status: Location to store status information
 * @options: Options for waiting
 * Return: PID of the child on success, -1 on failure
 * Corner cases:
 * - Invalid PID: prints error
 * - No child processes: prints error
 */
pid_t	Waitpid(pid_t pid, int *status, int options)
{
	pid_t	result;

	if (!status)
		return (-1);
	result = waitpid(pid, status, options);
	if (result == -1)
		perror(RED"Waitpid failed"RST);
	if (WIFEXITED(*status))
		*status = WEXITSTATUS(*status); 
	return (result);
}

/**
 * Malloc - Allocates memory with error handling
 * @size: Number of bytes to allocate
 * Return: Pointer to allocated memory
 * Corner cases:
 * - Zero size: returns NULL
 * - Allocation failure: prints error and exits
 */
void	*Malloc(size_t size)
{
	void	*ptr;

	if (size == 0)
		return (NULL);
	ptr = malloc(size);
	if (!ptr)
	{
		perror(RED"Malloc failed"RST);
		exit(EXIT_FAILURE);
	}
	return (ptr);
}


/**
 * Realloc - Reallocates memory with error handling
 * @ptr: Pointer to previously allocated memory
 * @size: New size in bytes
 * Return: Pointer to reallocated memory
 * Corner cases:
 * - NULL ptr: acts as malloc
 * - Zero size: acts as free
 * - Allocation failure: prints error and exits
 */
void	*Realloc(void *ptr, size_t size)
{
	void	*new_ptr;

	new_ptr = realloc(ptr, size);
	if (!new_ptr && size != 0)
	{
		perror(RED"Realloc failed"RST);
		exit(EXIT_FAILURE);
	}
	return (new_ptr);
}

/**
 * Getcwd - Gets current working directory with error handling
 * @buf: Buffer to store path
 * @size: Size of buffer
 * Return: Pointer to buf on success, NULL on failure
 * Corner cases:
 * - NULL buffer with zero size: allocates buffer
 * - Buffer too small: prints error
 * - Permission denied: prints error
 */
char	*Getcwd(char *buf, size_t size)
{
	char	*result;

	result = getcwd(buf, size);
	if (!result)
		perror(RED"Getcwd failed"RST);
	return (result);
}

/**
 * Getline - Reads a line from stream with error handling
 * @lineptr: Pointer to buffer containing the line
 * @n: Pointer to size of buffer
 * @stream: Input stream to read from
 * Corner cases:
 * - NULL arguments: prints error
 * - EOF reached: prints EOF message
 * - Read error: prints error message
 */
void	Getline(char **lineptr, size_t *n, FILE *stream)
{
	if (!lineptr || !stream)
	{
		fprintf(stderr, RED"Getline: invalid arguments\n"RST);
		return ;
	}
	if (getline(lineptr, n, stream) == -1)
	{
		free(*lineptr);
		*lineptr = NULL;
		if (feof(stream))
			printf(RED"[EOF]"RST"\n");
		else
			perror(RED"Getline failed"RST);
	}
}

/**
 * Dragon Ball Z themed loading animation
 * @charging: Array of animation frames
 * @frames: Number of frames in the animation
 * Uses ANSI escape sequences for colors and cursor control
 * Animation runs for approximately 3 seconds
 */
void dbzSpinnerLoading() 
{
    const char *charging[] = {
        "[            ]",
        "[=           ]",
        "[==          ]",
        "[===         ]",
        "[====        ]",
        "[=====       ]",
        "[======      ]",
        "[=======     ]",
        "[========    ]",
        "[=========   ]",
        "[==========  ]",
        "[=========== ]",
        "[===========💥]",
    };
    const int frames = sizeof(charging) / sizeof(charging[0]);

	p(RED"Shutting down...\n"RST);

    // Loop through the "charging" animation for 3 seconds
    for (int i = 0; i < frames; i++) {
        p("\r" Y "%s" RST, charging[i]);
        fflush(stdout);  // Force update the console
        usleep(421337);  
    }
	p(C"\n✅ EXIT ✅\n"RST);
	exit(EX_OK);
}

/**
 * Shell banner display function
 * Uses ANSI escape sequences for colors
 * Banner created using TAAG (Text to ASCII Art Generator) with ANSI_shadow font
 * Displays the "CELL" logo in ASCII art
 */
void printbanner(void)
{
	/*
	p(G"\n=== CELL TERMINAL INITIALIZED ===\n");
	p("Imperfect Shell | v1.0 | the basic/simple idea of a shell\n");
	p("Read-Evaluate/parse-Print/execute-Loop\n\n");
	*/

	// TAAG (Text to ASCII Art Generator). ANSI_shadow
    p(G" ██████╗███████╗██╗     ██╗         ██╗   ██╗  ██╗\n"
      "██╔════╝██╔════╝██║     ██║         ██║   ██║ ███║\n"
      "██║     █████╗  ██║     ██║         ██║   ██║ ╚██║\n"
      "██║     ██╔══╝  ██║     ██║         ╚██╗ ██╔╝  ██║\n"
      "╚██████╗███████╗███████╗███████╗     ╚████╔╝██╗██║\n"
      " ╚═════╝╚══════╝╚══════╝╚══════╝      ╚═══╝ ╚═╝╚═╝\n"
      "                                                  \n"RST);
}




