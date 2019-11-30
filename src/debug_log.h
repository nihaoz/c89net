#ifndef _DEBUG_TAG_H_
#define _DEBUG_TAG_H_

#ifdef __cplusplus
	extern "C" {
#endif

#define FORMATLOG_FP stderr

/* Action when format_log ERROR */
/* Abort when error */
#define ERR_ACT_ABORT       0
/* Print warning info only */
#define ERR_ACT_WARNING     1

#define LOG_ERR   0
#define LOG_WARN  1
#define LOG_INFO  2

#define QUICK_LOG_BAD_ARG(expr) \
	format_log(LOG_ERR,"Argument(s) illegal @"#expr " %s: %d",\
						 __FILE__, __LINE__);

#define QUICK_LOG_ERR_MEM_ALLOC(expr) \
	format_log(LOG_ERR,"Memory alloc failed @"#expr " %s: %d",\
						 __FILE__, __LINE__);

#define QUICK_LOG_ERR_DATATYPE(expr) \
	format_log(LOG_ERR,"Datatype unsupported or illegal @"#expr " %s: %d",\
						 __FILE__, __LINE__);


void set_log_error_action(int act);

void format_log(int logtype, const char *fmt, ...);

#ifdef __cplusplus
	}
#endif

#endif /* _DEBUG_TAG_H_ */