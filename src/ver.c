#include "ver.h"
#include <stdio.h>
#include <string.h>

/* Version information */
const char* SOFTWARE_VERSION = "1.0.0";
const char* SOFTWARE_NAME = "VDU_ESP32";
const char* BUILD_DATE = __DATE__;
const char* BUILD_TIME = __TIME__;
const char* VERSION_DESCRIPTION = "Multi-Dashboard LCD Display System";
const char* GIT_COMMIT_HASH = "6bcb47a"; /* Current commit hash */

/* Check if we're on a tagged version (you can set this to 1 when creating a tag) */
#define IS_TAGGED_VERSION 0  /* Set to 1 when creating a release tag */

/* Version structure */
version_info_t version_info = {
    .major = 1,
    .minor = 0,
    .patch = 0,
    .build_number = 1,
    .version_string = "1.0.0",
    .build_date = __DATE__,
    .build_time = __TIME__,
    .description = "Multi-Dashboard LCD Display System",
    .git_commit = "6bcb47a"
};

/**
 * @brief Get the full version string
 * @return Pointer to version string
 */
const char* get_version_string(void)
{
    return version_info.version_string;
}

/**
 * @brief Get the version description
 * @return Pointer to version description
 */
const char* get_version_description(void)
{
    return version_info.description;
}

/**
 * @brief Get the build date
 * @return Pointer to build date string
 */
const char* get_build_date(void)
{
    return version_info.build_date;
}

/**
 * @brief Get the build time
 * @return Pointer to build time string
 */
const char* get_build_time(void)
{
    return version_info.build_time;
}

/**
 * @brief Get the git commit hash
 * @return Pointer to git commit hash string
 */
const char* get_git_commit_hash(void)
{
    return version_info.git_commit;
}

/**
 * @brief Get the complete version information structure
 * @return Pointer to version_info_t structure
 */
const version_info_t* get_version_info(void)
{
    return &version_info;
}

/**
 * @brief Print version information to console
 */
void print_version_info(void)
{
    printf("=== %s Version Information ===\n", SOFTWARE_NAME);
    if (IS_TAGGED_VERSION) {
        printf("Version: %s (Tagged Release)\n", version_info.version_string);
    } else {
        printf("Version: %s (Development)\n", version_info.version_string);
        printf("Git Commit: %s\n", version_info.git_commit);
    }
    printf("Description: %s\n", version_info.description);
    printf("Build Date: %s\n", version_info.build_date);
    printf("Build Time: %s\n", version_info.build_time);
    printf("Build Number: %d\n", version_info.build_number);
    printf("==============================\n");
}

/**
 * @brief Get version as formatted string for display
 * @param buffer Buffer to store formatted string
 * @param buffer_size Size of the buffer
 * @return Number of characters written
 */
int get_version_display_string(char* buffer, size_t buffer_size)
{
    return snprintf(buffer, buffer_size, "V%s", version_info.version_string);
}

/**
 * @brief Get version with commit hash for display
 * @param buffer Buffer to store formatted string
 * @param buffer_size Size of the buffer
 * @return Number of characters written
 */
int get_version_commit_display_string(char* buffer, size_t buffer_size)
{
    if (IS_TAGGED_VERSION) {
        /* Show version for tagged releases */
        return snprintf(buffer, buffer_size, "V%s", version_info.version_string);
    } else {
        /* Show commit hash for development builds */
        return snprintf(buffer, buffer_size, "%s", version_info.git_commit);
    }
}

/**
 * @brief Get smart version display string (auto-detects tag vs commit)
 * @param buffer Buffer to store formatted string
 * @param buffer_size Size of the buffer
 * @return Number of characters written
 */
int get_smart_version_display_string(char* buffer, size_t buffer_size)
{
    if (IS_TAGGED_VERSION) {
        /* Show version for tagged releases */
        return snprintf(buffer, buffer_size, "V%s", version_info.version_string);
    } else {
        /* Show commit hash for development builds */
        return snprintf(buffer, buffer_size, "%s", version_info.git_commit);
    }
} 