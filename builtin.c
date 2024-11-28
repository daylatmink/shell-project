#include "cell.h"

/**
 * cell_echo - Echo command implementation with optional newline suppression
 * @args: Command arguments (args[0] is "echo")
 * Return: 0 on success, 1 on failure
 */
int	cell_echo(char **args)
{
	int start = 1;
	bool newline = true;

	if (!args || !args[0])
		return (1);

	// Check for the -n option
	if (args[1] && !strcmp(args[1], "-n"))
	{
		newline = false;
		start = 2;
	}

	// Print each argument separated by a space
	for (int i = start; args[i]; i++)
	{
		p("%s", args[i]);
		if (args[i + 1])
			p(" ");
	}

	// Print newline if -n option is not present
	if (newline)
		p("\n");

	return (0);
}


/**
 * cell_env - Display all environment variables
 * @args: Command arguments (unused in this function)
 * 
 * This function prints all the environment variables available in the current
 * shell session. Environment variables are key-value pairs that provide
 * information about the system environment and user settings.
 * 
 * How it works:
 * - The function accesses the global variable `environ`, which is declared
 *   with the `extern` keyword. This indicates that `environ` is defined
 *   elsewhere, typically by the system's C library, and is available for use
 *   in this file.
 * - The `extern` keyword is used to declare a variable without defining it,
 *   allowing the variable to be shared across multiple files. In this case,
 *   `environ` is a pointer to an array of strings, where each string is an
 *   environment variable formatted as "KEY=VALUE".
 * - The function iterates over this array and prints each environment variable
 *   to the standard output, followed by a newline.
 * - If the `environ` array is NULL, indicating that no environment variables
 *   are available, the function returns 1 to signal failure.
 * - Otherwise, it returns 0 to indicate successful execution.
 * 
 * Note: The `args` parameter is not used in this function, but it is included
 * to maintain a consistent function signature for built-in shell commands.
 * 
 * Return: 0 on success, 1 on failure (if no environment variables are found).
 */
int	cell_env(char **args)
{
	// exter -> this variable exist but it's defined somewhere else!
	//Because environ is defined in the C runtime and not directly in your program, you need to declare it as extern to tell the compiler:
    //"This variable exists somewhere else, not defined here, but I want to use it.
	extern char	**environ;

	(void)args;
	if (!environ)
		return (1);
	for (int i = 0; environ[i]; i++)
		p("%s\n", environ[i]);
	return (0);
}

/**
 * cell_exit - Exit shell with status
 */
int	cell_exit(char **args)
{
	(void)args;
	dbzSpinnerLoading();
	exit(EX_OK);
} 
