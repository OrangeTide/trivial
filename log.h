#ifndef LOG_H_
#define LOG_H_

enum log_level {
	LOG_EMERG,
	LOG_ALERT,
	LOG_CRIT,
	LOG_ERR,
	LOG_WARNING,
	LOG_NOTICE,
	LOG_INFO,
	LOG_DEBUG
};

void log_open(const char *ident, int option, int facility);
void log_close(void);
void log_generic(enum log_level level, const char *fmt, ...);

#define log_err(...) log_generic(LOG_ERR, __VA_ARGS__)
#define log_warning(...) log_generic(LOG_WARNING, __VA_ARGS__)
#define log_info(...) log_generic(LOG_INFO, __VA_ARGS__)
#define log_debug(f, ...) log_generic(LOG_ERR, "%s():%d:" f, ## __VA_ARGS__)
#endif
