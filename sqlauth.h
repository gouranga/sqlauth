#ifndef SQLAUTH_H_33C2357DE768498D84AF322323BA8151
#define SQLAUTH_H_33C2357DE768498D84AF322323BA8151

#include <zdb.h>

#define MAX_DB_CONNECTIONS 100
#define RETRY_ATTEMPTS 2

typedef struct {
    char* dsn;
    int max_connections;
    int connection_timeout;
    int sweep_interval;
    char* query;
} config_t;

typedef struct {
    ConnectionPool_T pool;
    URL_T url;
} dbc_t;

int start_pool(dbc_t* dbc, config_t* config);
int auth_attempt(dbc_t* dbc, config_t* config, const char* username, const char* password/*, int attempt = RETRY_ATTEMPTS*/);

#endif // SQLAUTH_H_33C2357DE768498D84AF322323BA8151
