#ifndef RKBT_UTILITY_H
#define RKBT_UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

pid_t rk_gettid(void);
size_t exec_command(const char *command, char *buffer, size_t buffer_size);
int exec_command_system(const char *cmd);
int kill_task(const char *process_name);
int run_task(char *name, char *cmd);

/**
 * @brief Get the PID of a process using the process name
 *
 * @param[in] name The name of the process to find the PID for
 *
 * @return The PID of the process if found, or 0 if not found
 */
unsigned int get_ps_pid(const char *process_name);

#define msleep(x) usleep(x * 1000)

#ifdef __cplusplus
}
#endif

#endif //RKBT_UTILITY_H
