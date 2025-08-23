/* GPL-2.0 WITH Linux-syscall-note OR Apache 2.0 */
/* Copyright (c) 2023 Fuzhou Rockchip Electronics Co., Ltd */

#ifndef __RK_OSAL__
#define __RK_OSAL__

#define RK_OSAL_VERSION "1.0"
#include <stddef.h>
#include <stdarg.h>
#ifdef OS_LINUX
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>
#endif
#ifdef OS_RTT
#define _POSIX_TIMERS
#define _POSIX_MONOTONIC_CLOCK
#define loff_t off_t
#define ioctl rkos_dev_ioctl
#include "osal_ioctl.h"
#include "poll.h"
#include <sys/time.h>
#include <time.h>
#endif

typedef enum _RKChipType {
	RK_CHIP_UNKOWN = 0,

	//  2928 3036 3308 no iep
	RK_CHIP_2928,
	RK_CHIP_3036,
	RK_CHIP_3308,
	RK_CHIP_3506,

	RK_CHIP_3066,
	RK_CHIP_3188,

	//  iep
	RK_CHIP_3368H,
	RK_CHIP_3368A,
	RK_CHIP_3128H,
	RK_CHIP_3128M,
	RK_CHIP_312X,
	RK_CHIP_3326,

	//  support 10bit chips
	RK_CHIP_10BIT_SUPPORT_BEGIN,
	RK_CHIP_1109,
	RK_CHIP_1106,
	RK_CHIP_1103,
	RK_CHIP_1103B,
	RK_CHIP_1106B,
	//  3288 support max width to 3840
	RK_CHIP_3288,

	//  support 4k chips
	RK_CHIP_4K_SUPPORT_BEGIN,
	RK_CHIP_322X_SUPPORT_BEGIN,
	RK_CHIP_3228A,
	RK_CHIP_3228B,
	RK_CHIP_3228H,
	RK_CHIP_3328,
	RK_CHIP_3229,
	RK_CHIP_322X_SUPPORT_END,
	RK_CHIP_3399,
	RK_CHIP_1126,
	//  support 8k chips
	RK_CHIP_8K_SUPPORT_BEGIN,
	RK_CHIP_3566,
	RK_CHIP_3568,
	RK_CHIP_3576,
	RK_CHIP_3588,
	RK_CHIP_8K_SUPPORT_END,

	RK_CHIP_10BIT_SUPPORT_END,

	RK_CHIP_3368,
	RK_CHIP_4K_SUPPORT_END,
} RKChipType;

typedef struct {
	const char *name;
	RKChipType  type;
} RKChipInfo;

typedef struct rkos_dev {
	char name[48];
	void *dev;
	int minor;
	const struct file_operations *fops;
	struct platform_driver *driver;
	struct rkos_pmops *rkos_pmops;
} rkos_dev_t;

typedef struct rkos_poll {
	void *poll_table;
	void *data;
} rkos_poll_t;

typedef struct rkos_semaphore {
	void *sem;
} rkos_semaphore_t;

typedef struct rkos_mutex {
	void *mutex;
} rkos_mutex_t;

typedef struct rkos_wait {
	void *wait;
	int pipe_fd[2];
	int event;
} rkos_wait_t;

typedef int (*threadfn_t)(void *data);
typedef int (*fd_callback)(void *data);

typedef struct rkos_task {
	void *task_struct;
	void *data;
	threadfn_t func;
	int runing;
} rkos_task_t;

typedef struct rkos_timer {
	void *timer;
	void (*function)(void *);
	unsigned long data;
} rkos_timer_t;

typedef struct rkos_timeval {
	long tv_sec;
	long tv_usec;
} rkos_timeval_t;

typedef struct rkos_fd_arg {
	void *buf;
	int fd;
	fd_callback func;
}rkos_fd_arg_t;

#define rkos_gfp_kernel 0
#define rkos_gfp_atomic 1
extern void *rkos_vcalloc(unsigned int num, unsigned int size);
extern void *rkos_vmalloc(unsigned long size);
extern void *rkos_vcalloc_uncache(unsigned int num, unsigned int size);
extern void *rkos_vmalloc_uncache(unsigned long size);
extern void  rkos_vfree(const void *addr);
extern void *rkos_kmalloc(unsigned long size, unsigned int rkos_gfp_flag);
extern void *rkos_kzalloc(unsigned long size, unsigned int rkos_gfp_flag);
extern void  rkos_kfree(const void *addr);

extern int rkos_sema_init(rkos_semaphore_t *sem, int val);
extern int rkos_down(rkos_semaphore_t *sem);
extern int rkos_down_timeout(rkos_semaphore_t *sem, int timeout);
extern int rkos_down_interruptible(rkos_semaphore_t *sem);
extern int rkos_down_trylock(rkos_semaphore_t *sem);
extern void rkos_up(rkos_semaphore_t *sem);
extern void rkos_sema_destory(rkos_semaphore_t *sem);

extern int rkos_mutex_init(rkos_mutex_t *mutex);
extern int rkos_mutex_lock(rkos_mutex_t *mutex);
extern int rkos_mutex_lock_interruptible(rkos_mutex_t *mutex);
extern int rkos_mutex_trylock(rkos_mutex_t *mutex);
extern void rkos_mutex_unlock(rkos_mutex_t *mutex);
extern void rkos_mutex_destory(rkos_mutex_t *mutex); //notice:must be called when kmod exit, other wise will lead to memory leak;

extern int rkos_wait_init(rkos_wait_t *wait);
extern void rkos_wakeup(rkos_wait_t *wait);
extern void rkos_wakedown(rkos_wait_t *wait);
extern void rkos_wakereset(rkos_wait_t *wait, int len);
extern void rkos_wait_event(rkos_wait_t *wait, int event);
extern void rkos_poll_wait(void *flip, rkos_wait_t *wait, void *table);
extern void rkos_wakeup(rkos_wait_t *wait);  //same as wake_up_all
extern void rkos_wait_destory(rkos_wait_t *wait);

extern rkos_task_t *rkos_kthread_create(threadfn_t thread, void *data, const char *name, int stack_size, int priority);
extern void rkos_kthread_destory(rkos_task_t *task, unsigned int stop_flag);
extern int rkos_kthread_should_stop(void);
extern int rkos_kthread_get_name(char *name, int size);
extern int rkos_kthread_set_name(const char *name);

extern char *rkos_strcpy(char *s1, const char *s2);
extern char *rkos_strncpy(char *s1, const char *s2, int size);
extern int rkos_strlcpy(char *s1, const char *s2, int size);
extern char *rkos_strcat(char *s1, const char *s2);
extern char *rkos_strncat(char *s1, const char *s2, int size);
extern int rkos_strlcat(char *s1, const char *s2, int size);
extern int rkos_strcmp(const char *s1, const char *s2);
extern int rkos_strncmp(const char *s1, const char *s2, int size);
extern int rkos_strnicmp(const char *s1, const char *s2, int size);
extern int rkos_strcasecmp(const char *s1, const char *s2);
extern int rkos_strncasecmp(const char *s1, const char *s2, int n);
extern char *rkos_strchr(const char *s, int n);
extern char *rkos_strnchr(const char *s, int count, int c);
extern char *rkos_strrchr(const char *s, int c);
extern char *rkos_strstr(const char *s1, const char *s2);
extern char *rkos_strnstr(const char *s1, const char *s2, int n);
extern int rkos_strlen(const char *s);
extern int rkos_strnlen(const char *s, int size);
extern char *rkos_strpbrk(const char *s1, const char *s2);
extern char *rkos_strsep(char **s, const char *ct);
extern int rkos_strspn(const char *s, const char *accept);
extern int rkos_strcspn(const char *s, const char *reject);
extern void *rkos_memset(void *str, int c, int count);
extern void *rkos_memcpy(void *s1, const void *s2, int count);
extern void *rkos_memmove(void *s1, const void *s2, int count);
extern void *rkos_memscan(void *addr, int c, int size);
extern int rkos_memcmp(const void *cs, const void *ct, int count);
extern void *rkos_memchr(const void *s, int c, int n);
extern void *rkos_memchr_inv(const void *s, int c, int n);

extern unsigned long long rkos_strtoull(const char *cp, char **endp, unsigned int base);
extern unsigned long rkos_strtoul(const char *cp, char **endp, unsigned int base);
extern long rkos_strtol(const char *cp, char **endp, unsigned int base);
extern long long rkos_strtoll(const char *cp, char **endp, unsigned int base);
extern int rkos_snprintf(char *buf, int size, const char *fmt, ...);
extern int rkos_scnprintf(char *buf, int size, const char *fmt, ...);
extern int rkos_sprintf(char *buf, const char *fmt, ...);
extern int rkos_sscanf(const char *buf, const char *fmt, ...);
extern int rkos_printk(const char *fmt, ...);
extern int rkos_printf(const char *fmt, ...);
extern void rkos_panic(const char *fmt, const char *file, const char *fun, int line, const char *);

extern void *rkos_ioremap(unsigned long phys_addr, unsigned long size);
extern void *rkos_ioremap_nocache(unsigned long phys_addr, unsigned long size);
extern void *rkos_ioremap_cached(unsigned long phys_addr, unsigned long size);
extern void rkos_iounmap(void *addr);
extern void *rkos_arch_ioaddress(void *addr);
extern void *rkos_ioaddress(int addr);
extern void *rkos_mmap(int fd, int size);
extern int rkos_munmap(void *addr, int size);
extern int rkos_cache_flush(void *addr, int size);
extern int rkos_cache_invalid(void *addr, int size);
extern unsigned long rkos_copy_from_user(void *to, const void *from, unsigned long n);
extern unsigned long rkos_copy_to_user(void *to, const void *from, unsigned long n);

extern int rkos_get_select_fd(int fd);
extern int rkos_dev_open(const char *pathname, int flags);
extern int rkos_dev_close(int fd);
extern int rkos_dev_ioctl(int fd, int cmd, void *args);
extern int rkos_dev_read(int fd, void *buf, size_t count);
extern int rkos_new_fd(void);
extern void rkos_set_fd_arg(int fd, fd_callback func, void *arg);
extern int rkos_close(int fd);
extern int rkos_dup_fd(int fd);
extern void *rkos_fopen(const char *filename, int flags, int mode);
extern void rkos_fclose(void *filp);
extern int rkos_fwrite(char *buf, int len, void *filp);
extern int rkos_fread(char *buf, unsigned int len, void *filp);
extern void rkos_device_init(void);
extern void rkos_device_deinit(void);
extern rkos_dev_t *rkos_createdev(const char *name);
extern int rkos_destroydev(rkos_dev_t *pdev);
extern int rkos_registerdevice(rkos_dev_t *pdev);
extern void rkos_deregisterdevice(rkos_dev_t *pdev);
extern void rkos_device_add_driver(void *dev, void *drv);

extern int rkos_timer_init(rkos_timer_t *timer);
extern int rkos_set_timer(rkos_timer_t *timer, unsigned long interval);//ms
extern int rkos_del_timer(rkos_timer_t *timer);
extern int rkos_timer_destory(rkos_timer_t *timer);

extern unsigned long rkos_msleep(unsigned int msecs);
extern void rkos_udelay(unsigned int usecs);
extern void rkos_mdelay(unsigned int msecs);

extern long long rkos_time_after(unsigned long long end, unsigned long long start);
extern unsigned int rkos_get_tickcount(void);
extern unsigned long long rkos_sched_clock(void);
extern rkos_timeval_t rkos_time_sub(rkos_timeval_t end, rkos_timeval_t start);
extern unsigned long long rkos_getjiffies(void);
extern void rkos_gettime(rkos_timeval_t *tv);

extern RKChipInfo *getChipInfo(void);

extern void *rkos_dlopen(const char *libName, int mode);
extern void *rkos_dlsym(void *handler, const char *funcName);
extern int rkos_dlclose(void *handle);
extern const char *rkos_dlerror(void);
extern int rkos_access(char *libPath, int accessMode);
#endif
