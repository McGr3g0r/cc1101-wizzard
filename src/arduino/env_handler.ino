//------------------------------------------------------------------------------------------------------------------
#include "settings.h"

//------------------------------------------------------------------------------------------------------------------
#include "CmdHandler.h"
#include "utils.h"
#if USE_FILE_SYSTEM == 1
#include "LittleFS.h"
#endif
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e env_main(void* parent,int argc, char* argv[]);
CmdStatus_e env_list(void* parent,int argc, char* argv[]);
CmdStatus_e env_set(void* parent,int argc, char* argv[]);
CmdStatus_e env_set_int(void* parent,const char* name, const char* value);
CmdStatus_e env_unset(void* parent,int argc, char* argv[]);
#if USE_FILE_SYSTEM == 1
CmdStatus_e env_save(void* parent,int argc, char* argv[]);
CmdStatus_e env_load(void* parent,int argc, char* argv[]);
#endif
CmdHandler* getRootCommandHandler(void);
//------------------------------------------------------------------------------------------------------------------
typedef struct env_item_s
{
  char name[ENV_NAME_MAXLEN+1];
  char value[ENV_VALUE_MAXLEN+1];
  bool used;
} env_item_t;
//------------------------------------------------------------------------------------------------------------------
env_item_t env[ENV_MAX_ITEMS];
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t env_handler = { &env_main, NULL, NULL, "env", "enviroment variables utils, use: 'env help'", 0, "", "env help" };
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t env_sub_cmd[] = {
   { &env_main,    &env_handler,   NULL, "help",  "this help", 0, "", "env help"},
   { &env_list,    &env_handler,   NULL, "list",  "list enviroment variables", 0, "", "env list"},
   { &env_set,     &env_handler,   NULL, "set",  "set variable <name> to <value>", 2, "tt", "env set kaboom<text> 25<text>"},
   { &env_unset,   &env_handler,   NULL, "unset", "unsets variable", 1, "t", "env unset kaboom<text>"},
   { &env_get,     &env_handler,   NULL, "get", "gets variable <name>", 1, "t", "env get kaboom<text>"},   
#if USE_FILE_SYSTEM == 1
   { &env_save,   &env_handler,   NULL, "save", "stores variables to /env.txt", 0, "", "env store"},
   { &env_load,    &env_handler,   NULL, "load", "reads variables from /env.txt", 0, "", "env load"},
#endif
   
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
void env_clear_all()
{
   memset(env, 0, sizeof(env));
}
//------------------------------------------------------------------------------------------------------------------
void env_cmd_init(void)
{ 
  env_clear_all();
  getRootCommandHandler()->registerHandler(&env_handler, env_sub_cmd); 
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e env_main(void* parent,int argc, char* argv[])
{
     STDOUT.println(env_handler.desc);

     int idx = 0 ;
     cmd_handler_t* item = &env_handler.sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &env_handler.sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e env_list(void* parent,int argc, char* argv[])
{
     STDOUT.println("env:");

     for (int idx = 0; idx < sizeof(env) / sizeof(env_item_t); idx++)
     {
        if (env[idx].used)
        {
            STDOUT.print(env[idx].name);
            STDOUT.print(" = ");
            STDOUT.println(env[idx].value);
        }
     }
     
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
int env_find_free_idx()
{
     int idx;

     for (idx = 0; idx < sizeof(env) / sizeof(env_item_t); idx++)
     {
        if (!env[idx].used)
            return idx;
     }
         
     return -1;
}
//------------------------------------------------------------------------------------------------------------------
int env_find_idx_by_name(const char* name)
{
     int idx;

     for (idx = 0; idx < sizeof(env) / sizeof(env_item_t); idx++)
     {
        if (env[idx].used && strcmp(env[idx].name, name) == 0)
            return idx;
     }
         
     return -1;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e env_set_int(void* parent,const char* name, const char* value)
{
    if (strlen(name) == 0 || strlen(value) == 0)
    {
         getRootCommandHandler()->setResultMessage("env name or value empty");
         return WRONG_PARAMS;
    }
    int idx = env_find_free_idx();

    if (idx == -1)
    {
         getRootCommandHandler()->setResultMessage("too much env variables, unset some variable");
         return WRONG_PARAMS;
    }   
        
   strncpy(env[idx].name,  name, min(strlen(name), ENV_NAME_MAXLEN));
   strncpy(env[idx].value, value, min(strlen(value), ENV_VALUE_MAXLEN));
   env[idx].used = true;

   return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e env_set(void* parent,int argc, char* argv[])
{
    char *ename = argv[2];
    char *eval = argv[3];

    return env_set_int(parent, ename, eval);
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e env_unset(void* parent,int argc, char* argv[])
{
    char *ename = argv[2];     
     
    if (strlen(ename) == 0 )
    {
         getRootCommandHandler()->setResultMessage("env name empty");
         return WRONG_PARAMS;
    }
    int idx = env_find_idx_by_name(ename);

    if (idx == -1)
    {
         getRootCommandHandler()->setResultMessage("variable not found");
         return WRONG_PARAMS;
    }   
   
   env[idx].used = false;

   return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e env_get(void* parent,int argc, char* argv[])
{
    char *ename = argv[2];
        
    if (strlen(ename) == 0 )
    {
         getRootCommandHandler()->setResultMessage("env name empty");
         return WRONG_PARAMS;
    }
    int idx = env_find_idx_by_name(ename);

    if (idx == -1)
    {
         getRootCommandHandler()->setResultMessage("variable not found");
         return WRONG_PARAMS;
    }   
   
   getRootCommandHandler()->setResultMessage(env[idx].value);
   return OK;
}
//------------------------------------------------------------------------------------------------------------------
#if USE_FILE_SYSTEM == 1
CmdStatus_e env_save(void* parent,int argc, char* argv[])
{
     char path[32];
     sprintf(path, "%s", "/env.txt");     
     
     File f = LittleFS.open(path, "w");  
     if (!f)
     {      
        getRootCommandHandler()->setHint("file /env.txt creation failed!.");
        return FILE_ERROR;      
     }
     
     for (int idx = 0; idx < sizeof(env) / sizeof(env_item_t); idx++)
     {
        if (env[idx].used)
        {
           f.print(env[idx].name);
           f.print(" ");
           f.println(env[idx].value);
        }
     }

     f.close();
     return OK;   
}
#endif
//------------------------------------------------------------------------------------------------------------------
#if USE_FILE_SYSTEM == 1
CmdStatus_e env_load(void* parent,int argc, char* argv[])
{
     char path[32];
     sprintf(path, "%s", "/env.txt");
     int idx;
     
     
     File f = LittleFS.open(path, "r");  
     if (!f)
     {      
        getRootCommandHandler()->setHint("file /env.txt read failed!.");
        return FILE_ERROR;      
     }
     while (f.available())
     {      
        String n = f.readStringUntil(' ');      
        String v = f.readStringUntil('\n');
        idx = env_find_free_idx();

        if (idx != -1)
        {
          strncpy(env[idx].name,  n.c_str(), min((unsigned int)n.length(), (unsigned int)ENV_NAME_MAXLEN));
          strncpy(env[idx].value, v.c_str(), min((unsigned int)v.length(), (unsigned int)ENV_VALUE_MAXLEN));
          env[idx].used = true;
        } else {
            getRootCommandHandler()->setHint("reading more items frin /env.txt failed!.");
            f.close();
            return FILE_ERROR;   
        }
     }

     f.close();
     return OK;   
}
#endif
//------------------------------------------------------------------------------------------------------------------
