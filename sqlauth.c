#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
//#include <pthread.h>
#include "sqlauth.h"
#include "config.h"

static int _pool_started = 0;
//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void on_abort(const char *error)
{
    fprintf(stderr, "Error: %s\n", error);
    exit(1);
}

int start_pool(dbc_t* dbc, config_t* config)
{
    debug_print("Start pool for %s", config->dsn);
    if (!__sync_bool_compare_and_swap(&_pool_started, 0, 1))
    {
        debug_print("<!> start_pool called more than once. Check your config.");
        return ERROR_CONFIG;
    }

    int ret = 0;
    dbc->url = URL_new(config->dsn);
    dbc->pool = ConnectionPool_new(dbc->url);
    if (!dbc->pool)
    {
        debug_print("<!> Could not create pool for %s", config->dsn);
        return ERROR_SQLPOOL_ERROR;
    }
    TRY
    {
        ConnectionPool_setMaxConnections(dbc->pool, config->max_connections);
        ConnectionPool_setConnectionTimeout(dbc->pool, config->connection_timeout);
        ConnectionPool_setReaper(dbc->pool, config->sweep_interval);
        ConnectionPool_setAbortHandler(dbc->pool, on_abort);
        ConnectionPool_start(dbc->pool);
    }
    CATCH(SQLException)
    {
        debug_print("<!> Could not connect to %s, got message: %s", config->dsn, Exception_frame.message);
        ret = ERROR_SQLPOOL_ERROR;
    }
    END_TRY;

    return ret;
}

int auth_attempt(dbc_t* dbc, config_t* config, const char* username, const char* password/*, int attempt*/)
{
    if (!_pool_started)
    {
        debug_print("<!> Pool was not started for %s", config->dsn);
        return ERROR_SQLPOOL_ERROR;
    }

    Connection_T conn = ConnectionPool_getConnection(dbc->pool);
    if (!conn)
    {
        // todo: sleep and retry
        debug_print("<!> Could not get connection from the pool, %d of %d connections.", ConnectionPool_active(dbc->pool), ConnectionPool_size(dbc->pool));
        return ERROR_AUTH_USERNAME_PASSWORD_MISMATCH;
    }

    int ret = 0;
    bool match;
    TRY
    {
        PreparedStatement_T p = Connection_prepareStatement(conn, config->query);
        PreparedStatement_setString(p, 1, username);
        ResultSet_T r = PreparedStatement_executeQuery(p);
        if (ResultSet_next(r))
        {
            const char* db_password = ResultSet_getString(r, 1);
            debug_print("Fetched password for %s:%s", username, db_password);
            match = strcmp(db_password, password) == 0;
        }
    }
    CATCH(SQLException)
    {
        debug_print("<!> Error executing pass query \"%s\" as prepared statement, got message: %s", config->query, Exception_frame.message);
        ret = ERROR_SQLPOOL_ERROR;
    }
    FINALLY
    {
        Connection_clear(conn); // free prepared statements
        Connection_close(conn); // <-- return to pool
    }
    END_TRY;

    if (!ret)
    {
        if (match)
        {
           debug_print("Passwords match, auth ok.");
           return ERROR_AUTH_SUCCESS;
        }
        debug_print("Passwords mismatch.");
        return ERROR_AUTH_USERNAME_PASSWORD_MISMATCH;
    }
    return ret;
}
