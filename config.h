#ifndef CONFIG_H_33C2357DE768498D84AF322323BA8151
#define CONFIG_H_33C2357DE768498D84AF322323BA8151

#include <stdarg.h>
#include <stdio.h>

// 3proxy auth errors (via devref)
#define ERROR_AUTH_SUCCESS 0
#define ERROR_AUTH_USER_FORBID 1
#define ERROR_AUTH_USERNAME_PASSWORD_EMPTY 4
#define ERROR_AUTH_USERNAME_PASSWORD_MISMATCH 5

// custom errors
#define ERROR_CONFIG 1024
#define ERROR_SQLPOOL_ERROR 2048
#define ERROR_RETRY_LIMIT_REACHED ERROR_AUTH_USER_FORBID

extern int g_DBGLEVEL;

#define debug_print(fmt, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        if (g_DBGLEVEL)                                                                                                \
            fprintf(stderr, "sqlauth :: " fmt "\n", ##__VA_ARGS__);                                                    \
    } while (0)
#define debug_exprint(fmt, ...)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        if (g_DBGLEVEL)                                                                                                \
            fprintf(stderr, "sqlauth :: " "%s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__);       \
    } while (0)


#endif // CONFIG_H_33C2357DE768498D84AF322323BA8151
