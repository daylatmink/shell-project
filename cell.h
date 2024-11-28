#ifndef WRAPPERS_H
# define WRAPPERS_H

/*
** Standard library includes for core functionality:
** unistd.h    - POSIX operating system API (fork, exec, etc.)
** stdlib.h    - General utilities (malloc, free, exit)
** stdio.h     - Input/output operations
** errno.h     - System error numbers
** string.h    - String manipulation
** sys/wait.h  - Process control (wait, waitpid)
** sysexits.h  - System exit values
*/
# include <unistd.h>
# include <stdbool.h>
# include <stdlib.h>
# include <stdio.h>
# include <errno.h>
# include <string.h>
# include <sys/wait.h>
# include <sysexits.h>

/*
** ANSI Color codes for terminal output formatting:
** Y    - Yellow
** G    - Green
** C    - Cyan
** RED  - Red
** RST  - Reset to default color
*/
#define Y		"\033[1;33m"
#define G		"\033[1;32m"
#define C 		"\033[1;36m"
#define RED		"\033[1;31m"
#define RST 	"\033[0m"

/*
** Utility macros:
** p      - Shorthand for printf
** SPACE  - All whitespace characters for tokenization
** CELL_JR- Child process identifier (0)
*/
#define p(...)	printf(__VA_ARGS__)
#define ERROR(msg) fprintf(stderr, RED msg RST "\n")
#define SPACE	"\t\n\v\f\r "
#define CELL_JR	0

/*
** Status codes for shell operations
*/
enum{
	OK,     /* Operation succeeded */
	ERROR   /* Operation failed */
};

/*
** Structure for built-in command handling
** @builtin_name: Name of the built-in command
** @foo: Function pointer to the command implementation
*/
typedef struct s_builtin
{
    const char *builtin_name;
	int (*foo)(char **av);
} t_builtin;

/*
** Built-in command function prototypes
** Each returns 0 on success, non-zero on failure
*/
int	cell_echo(char **args);  /* Echo command implementation */
int	cell_env(char **args);   /* Environment variables display */
int	cell_exit(char **args);  /* Shell exit command */

/*
** Dragon Ball Z themed utility functions
*/
void 	dbzSpinnerLoading();  /* Animated loading spinner */
void	printbanner(void);    /* Shell banner display */

/*
** System call wrappers with error handling
** Each wrapper checks for errors and handles them appropriately
*/
void	Chdir(const char *path);      /* Change directory */
pid_t	Fork(void);                   /* Process creation */
void	Execvp(const char *file, char *const argv[]); /* Execute program */
pid_t	Wait(int *status);
pid_t	Waitpid(pid_t pid, int *status, int options); /* Wait for process */
void	*Malloc(size_t size);         /* Memory allocation */
void	*Realloc(void *ptr, size_t size); /* Memory reallocation */
char	*Getcwd(char *buf, size_t size); /* Get current directory */
void	Getline(char **lineptr, size_t *n, FILE *stream); /* Read line */

#endif
