#ifndef _DEBUG_TAG_H_
#define _DEBUG_TAG_H_

#ifdef __cplusplus
	extern "C" {
#endif

#define FORMATLOG_FP stderr

void time_tag_warning();

void time_tag_error();

void format_log(const char *fmt, ...);

#ifdef __cplusplus
	}
#endif

#endif /* _DEBUG_TAG_H_ */