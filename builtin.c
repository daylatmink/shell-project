#include "cell.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utime.h>
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
int cell_pwd(char **args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        return 0;
    } else {
        perror("pwd");
        return 1;
    }
}

int cell_clear(char **args) {
    // Gọi lệnh clear của hệ thống
    int ret = system("clear");
    return ret;
}

// Lưu lịch sử lệnh (đơn giản, tối đa 100 lệnh)
#define HISTORY_SIZE 100
char *history[HISTORY_SIZE];
int history_count = 0;

void add_history(const char *cmd) {
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(cmd);
    } else {
        free(history[0]);
        memmove(history, history + 1, sizeof(char*) * (HISTORY_SIZE - 1));
        history[HISTORY_SIZE - 1] = strdup(cmd);
    }
}

int cell_help(char **args) {
    printf(
        "Các lệnh hỗ trợ trong tinyShell:\n\n"
        "  help, cellhelp      Hiển thị thông tin trợ giúp này\n"
        "  cd <dir>            Đổi thư mục làm việc hiện tại\n"
        "  exit                Thoát shell\n"
        "  jobs                Liệt kê các tiến trình nền\n"
        "  fg <pid>            Đưa tiến trình nền về foreground\n"
        "  kill <pid>          Kết thúc tiến trình theo pid\n"
        "  history             Hiển thị lịch sử lệnh\n"
        "  !<n>                Thực thi lại lệnh thứ n trong history\n"
        "  <lệnh> &            Chạy lệnh ở chế độ nền (background)\n"
        "  <lệnh1> | <lệnh2>   Kết hợp hai lệnh qua pipe\n"
        "  <lệnh> > file       Ghi output vào file\n"
        "  <lệnh> >> file      Ghi tiếp output vào file\n"
        "  <lệnh> < file       Đọc input từ file\n"
    );
    return 0;
}

int cell_history(char **args) {
    for (int i = 0; i < history_count; i++) {
        printf("%2d  %s\n", i+1, history[i]);
    }
    return 0;
}

int cell_date(char **args) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("%s\n", buf);
    return 0;
}

int cell_whoami(char **args) {
    struct passwd *pw = getpwuid(getuid());
    if (pw)
        printf("%s\n", pw->pw_name);
    else
        perror("whoami");
    return 0;
}

// Lệnh uptime: Đọc thông tin từ /proc/uptime (trên Linux)
int cell_uptime(char **args) {
    FILE *f = fopen("/proc/uptime", "r");
    if (!f) {
        perror("uptime");
        return 1;
    }
    double uptime_seconds;
    if (fscanf(f, "%lf", &uptime_seconds) != 1) {
        fprintf(stderr, "uptime: lỗi đọc dữ liệu\n");
        fclose(f);
        return 1;
    }
    fclose(f);
    int hours = (int)uptime_seconds / 3600;
    int minutes = ((int)uptime_seconds % 3600) / 60;
    int seconds = (int)uptime_seconds % 60;
    printf("Uptime: %dh %dm %ds\n", hours, minutes, seconds);
    return 0;
}

// Lệnh touch: Tạo file rỗng hoặc cập nhật thời gian sửa đổi
int cell_touch(char **args) {
    if (!args[1]) {
        fprintf(stderr, "touch: thiếu tên file\n");
        return 1;
    }
    int fd = open(args[1], O_CREAT | O_WRONLY, 0644);
    if (fd < 0) {
        perror("touch");
        return 1;
    }
    close(fd);
    // Cập nhật thời gian truy cập và sửa đổi
    struct utimbuf new_times;
    new_times.actime = time(NULL);
    new_times.modtime = time(NULL);
    utime(args[1], &new_times);
    return 0;
}
Alias alias_table[MAX_ALIAS];
int alias_count = 0;

// Hàm thêm alias
void add_alias(const char *name, const char *value) {
    if (!name || !value || !*name || !*value) return; // Không thêm alias rỗng
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(alias_table[i].name, name) == 0) {
            strncpy(alias_table[i].value, value, sizeof(alias_table[i].value) - 1);
            alias_table[i].value[sizeof(alias_table[i].value) - 1] = '\0';
            return;
        }
    }
    if (alias_count < MAX_ALIAS) {
        strncpy(alias_table[alias_count].name, name, sizeof(alias_table[alias_count].name) - 1);
        alias_table[alias_count].name[sizeof(alias_table[alias_count].name) - 1] = '\0';
        strncpy(alias_table[alias_count].value, value, sizeof(alias_table[alias_count].value) - 1);
        alias_table[alias_count].value[sizeof(alias_table[alias_count].value) - 1] = '\0';
        alias_count++;
    }
}

// Hàm lấy alias
const char* get_alias(const char* name) {
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(alias_table[i].name, name) == 0)
            return alias_table[i].value;
    }
    return NULL;
}

// Hàm built-in xử lý lệnh alias
int cell_alias(char **args) {
    if (!args[1]) {
        // Hiển thị toàn bộ alias
        for (int i = 0; i < alias_count; i++)
            printf("alias %s='%s'\n", alias_table[i].name, alias_table[i].value);
        return 0;
    }
    // alias name='value'
    char *eq = strchr(args[1], '=');
    if (eq) {
        *eq = '\0';
        char *name = args[1];
        char *value = eq + 1;
        // Chỉ bỏ dấu nháy chuẩn
        if ((value[0] == '\'' && value[strlen(value)-1] == '\'') ||
            (value[0] == '"' && value[strlen(value)-1] == '"')) {
            value++;
            size_t len = strlen(value);
            if (len > 0) value[len-1] = '\0';
        }
        add_alias(name, value);
        return 0;
    }
    // alias name: hiển thị giá trị alias
    const char* value = get_alias(args[1]);
    if (value)
        printf("alias %s='%s'\n", args[1], value);
    else
        printf("alias: %s: not found\n", args[1]);
    return 0;
}
int cell_kill(char **args) {
    if (!args[1]) {
        fprintf(stderr, "kill: missing PID\n");
        return 1;
    }
    pid_t pid = atoi(args[1]);
    int sig = SIGTERM; // hoặc cho phép chọn signal
    if (args[2]) sig = atoi(args[2]);
    if (kill(pid, sig) == -1) {
        perror("kill");
        return 1;
    }
    return 0;
}
int cell_jobs(char **args) {
    print_bg_list();
    return 0;
}

int cell_time(char **args) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "%H:%M:%S", tm_info);
    printf("Giờ hiện tại: %s\n", buf);
    return 0;
}

int cell_dir(char **args) {
    // Gọi trực tiếp lệnh ls -l
    int ret = system("ls -l");
    return ret;
}

int cell_stop(char **args) {
    if (!args[1]) {
        fprintf(stderr, "stop: thiếu PID\n");
        return 1;
    }
    pid_t pid = atoi(args[1]);
    if (kill(pid, SIGSTOP) == -1) {
        perror("stop");
        return 1;
    }
    set_bg_status(pid, STOPPED);  // Cập nhật trạng thái
    return 0;
}

int cell_fg(char **args) {
    if (!args[1]) {
        fprintf(stderr, "fg: thiếu PID\n");
        return 1;
    }
    pid_t pid = atoi(args[1]);
    if (kill(pid, SIGCONT) == -1) {
        perror("fg");
        return 1;
    }
    set_bg_status(pid, RUNNING);
    int status;
    Waitpid(pid, &status, 0);  // Đợi foreground hoàn thành
    return 0;
}

int cell_resume(char **args) {
    if (!args[1]) {
        fprintf(stderr, "resume: thiếu PID\n");
        return 1;
    }
    pid_t pid = atoi(args[1]);
    if (kill(pid, SIGCONT) == -1) {
        perror("resume");
        return 1;
    }
    set_bg_status(pid, RUNNING);
    return 0;
}

int cell_path(char **args) {
    (void)args;
    const char *path = getenv("PATH");
    if (path)
        printf("PATH=%s\n", path);
    else
        fprintf(stderr, "PATH chưa được đặt\n");
    return 0;
}

int cell_addpath(char **args) {
    if (!args[1]) {
        fprintf(stderr, "addpath: thiếu tham số thư mục\n");
        return 1;
    }
    const char *old_path = getenv("PATH");
    char new_path[1024];
    if (old_path)
        snprintf(new_path, sizeof(new_path), "%s:%s", old_path, args[1]);
    else
        snprintf(new_path, sizeof(new_path), "%s", args[1]);

    if (setenv("PATH", new_path, 1) != 0) {
        perror("addpath");
        return 1;
    }

    printf("PATH đã cập nhật: %s\n", new_path);
    return 0;
}

