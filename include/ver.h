#ifndef VER_H
#define VER_H

#include <stddef.h>

/* Version information structure */
typedef struct {
    int major;
    int minor;
    int patch;
    int build_number;
    const char* version_string;
    const char* build_date;
    const char* build_time;
    const char* description;
    const char* git_commit;
} version_info_t;

/* External version constants */
extern const char* SOFTWARE_VERSION;
extern const char* SOFTWARE_NAME;
extern const char* BUILD_DATE;
extern const char* BUILD_TIME;
extern const char* VERSION_DESCRIPTION;
extern const char* GIT_COMMIT_HASH;

/* External version structure */
extern version_info_t version_info;

/* Function prototypes */
const char* get_version_string(void);
const char* get_version_description(void);
const char* get_build_date(void);
const char* get_build_time(void);
const char* get_git_commit_hash(void);
const version_info_t* get_version_info(void);
void print_version_info(void);
int get_version_display_string(char* buffer, size_t buffer_size);
int get_version_commit_display_string(char* buffer, size_t buffer_size);
int get_smart_version_display_string(char* buffer, size_t buffer_size);

#endif /* VER_H */ 