#ifndef _DEBUG_TAG_H_
#define _DEBUG_TAG_H_

#ifdef __cplusplus
	extern "C" {
#endif

#define FORMATLOG_FP stderr

#define LOG_ERR   0
#define LOG_WARN  1
#define LOG_INFO  2

#define QUICK_LOG_ERR_MEM_ALLOC() \
	format_log(LOG_ERR, "Memory alloc failed @ %s: %d",\
						 __FILE__, __LINE__);

void format_log(int logtype, const char *fmt, ...);

#ifdef __cplusplus
	}
#endif

#endif /* _DEBUG_TAG_H_ */