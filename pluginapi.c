#include "../../structures.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "sqlauth.h"
#include "config.h"

int g_DBGLEVEL = 0;

static int _already_loaded = 0;
static struct pluginlink* _pluginlink;

config_t g_config;
dbc_t g_dbc;

struct commands sqlopts;
int sqlopts_commandhandler(int argc, unsigned char **argv)
{
    debug_print(" - sqlopts %s %s", (char *)argv[1], (char *)argv[2]);
    if (!strcmp((char *)argv[1], "max_connections"))
    {
        g_config.max_connections = atoi((char *)argv[2]);
        return 0;
    }
    if (!strcmp((char *)argv[1], "connection_timeout"))
    {
        g_config.connection_timeout = atoi((char *)argv[2]);
        return 0;
    }
    if (!strcmp((char *)argv[1], "sweep_interval"))
    {
        g_config.sweep_interval = atoi((char *)argv[2]);
        return 0;
    }
    if (!strcmp((char *)argv[1], "dsn"))
    {
        if (!Connection_isSupported((char *)argv[2]))
        {
            debug_print("<!> DSN not supported: %s", (char *)argv[2]);
            return ERROR_CONFIG;
        }
        g_config.dsn = (char*)_pluginlink->mystrdup((char*)argv[2]);
        return 0;
    }
    if (!strcmp((char *)argv[1], "query"))
    {
        g_config.query = (char*)_pluginlink->mystrdup((char*)argv[2]);
        return 0;
    }

    return ERROR_CONFIG;
}

struct commands sqlconnect;
int sqlconnect_commandhandler(int argc, unsigned char **argv)
{
    debug_print(" - sqlconnect");
    return start_pool(&g_dbc, &g_config);
}

static struct auth sqlauth;
static int sqlauth_handler(struct clientparam* param)
{
    int ret;
    char* username = "";
    char* password = "";

    if (param->username)
    {
        username = param->username;
    }
    if (param->password)
    {
        password = param->password;
    }

    debug_print("Login attempt with %s:%s", username, password);

    if (strlen(username) == 0 || strlen(password) == 0)
    {
        debug_print("Either username or password empty.");
        return ERROR_AUTH_USERNAME_PASSWORD_EMPTY;
    }

    ret = auth_attempt(&g_dbc, &g_config, username, password);
    return ret <= 10 ? ret : ERROR_AUTH_USER_FORBID;
}

PLUGINAPI int PLUGINCALL load(struct pluginlink* pluginlink, int argc, unsigned char** argv)
{
    if (argc > 1 && !strcmp((char*)argv[1], "debug"))
    {
        g_DBGLEVEL = 1;
        debug_print("Debug mode enabled.");
    }

    if (!__sync_bool_compare_and_swap(&_already_loaded, 0, 1))
    {
        debug_print("<!> Second call to load"); // todo: plugin unload?
        return 0;
    }

    //set default config
    g_config.max_connections = MAX_DB_CONNECTIONS;
    g_config.connection_timeout = 10;
    g_config.sweep_interval = 10;
    g_config.dsn = NULL;
    g_config.query = NULL;

    for (int i = 0; i < argc; i++)
    {
        debug_print(" - argv[%d]: %s", i, argv[i]);
    }

    _pluginlink = pluginlink;

    sqlopts.command = "sqlopts";
    sqlopts.handler = sqlopts_commandhandler;
    sqlopts.minargs = 3;
    sqlopts.maxargs = 3;
    sqlopts.next = _pluginlink->commandhandlers->next;
    _pluginlink->commandhandlers->next = &sqlopts;

    sqlconnect.command = "sqlconnect";
    sqlconnect.handler = sqlconnect_commandhandler;
    sqlconnect.next = _pluginlink->commandhandlers->next;
    _pluginlink->commandhandlers->next = &sqlconnect;

    sqlauth.authenticate = sqlauth_handler;
    sqlauth.authorize = _pluginlink->checkACL;
    sqlauth.desc = "sqlauth";
    sqlauth.next = _pluginlink->authfuncs->next;
    _pluginlink->authfuncs->next = &sqlauth;

    return 0;
}
