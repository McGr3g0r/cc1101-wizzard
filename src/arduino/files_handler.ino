//------------------------------------------------------------------------------------------------------------------
#include "settings.h"
//------------------------------------------------------------------------------------------------------------------
#include "CmdHandler.h"
#include "utils.h"
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e files_main(void* parent,int argc, char* argv[]);
CmdStatus_e files_list(void* parent,int argc, char* argv[]);
CmdStatus_e files_create(void* parent,int argc, char* argv[]);
CmdStatus_e files_remove(void* parent,int argc, char* argv[]);
CmdStatus_e files_append(void* parent,int argc, char* argv[]);
CmdStatus_e files_cat(void* parent,int argc, char* argv[]);
CmdStatus_e files_readhex(void* parent,int argc, char* argv[]);
CmdHandler* getRootCommandHandler(void);
//------------------------------------------------------------------------------------------------------------------
#if USE_FILE_SYSTEM == 1
#include "LittleFS.h"
LittleFSConfig cfg;
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t files_handler = { &files_main, NULL, NULL, "files", "Files utils, use: 'files help'", 0, "", "files help" };
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t files_sub_cmd[] = {
   { &files_main,    &files_handler,   NULL, "help",  "this help", 0, "", "files help"},
   { &files_list,    &files_handler,   NULL, "list",  "lists files in main folder", 0, "", "files list"},
   { &files_create,    &files_handler,   NULL, "create",  "crate file: <filename>", 1, "t", "files create name.txt"},
   { &files_remove,    &files_handler,   NULL, "remove",  "remove file: <filename> with pass code <aa55>", 2, "tx", "files remove name.txt aa55"},
   { &files_append,    &files_handler,   NULL, "append",  "append text line to file ", 2, "tt", "files append name.txt test2 test3 test4"},
   { &files_cat,    &files_handler,   NULL, "cat",  "concatenates file: <filename>", 1, "t", "files cat name.txt"},
   { &files_readhex,    &files_handler,   NULL, "readhex",  "readhex file: <filename> to <envname>", 2, "tt", "files readhex name.txt env1"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
void files_cmd_init(void)
{ 
  cfg.setAutoFormat(true);
  LittleFS.setConfig(cfg);
  LittleFS.begin();
  
  getRootCommandHandler()->registerHandler(&files_handler, files_sub_cmd); 
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e files_main(void* parent,int argc, char* argv[])
{
     STDOUT.println(files_handler.desc);

     int idx = 0 ;
     cmd_handler_t* item = &files_handler.sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &files_handler.sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e files_list(void* parent,int argc, char* argv[])
{
    Dir dir = LittleFS.openDir("/");
    STDOUT.println("/");
    while (dir.next()) {
      if(dir.isFile()) {
         STDOUT.println(dir.fileName());
      }
    }
  
  return OK;
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e files_create(void* parent,int argc, char* argv[])
{

    char *fname = argv[2];
    char path[32];
    CmdHandler* handler = (CmdHandler*) parent;
  
    if (strlen(fname) < 2 || strlen(fname) > 11)
    {
       handler->setHint("wrong file name use dos-like file names");
       return WRONG_PARAMS;
    }

    sprintf(path,"/%s", fname);
    
    File f = LittleFS.open(path, "w");
    if (f)
    {
        f.close();
        return OK;
    } else {
      handler->setHint("file creation failed!.");
      return FILE_ERROR;
    }
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e files_remove(void* parent,int argc, char* argv[])
{

    char path[32];
    char *fname = argv[2];
    int passcode =  strtoul(argv[3], 0, 16);

    CmdHandler* handler = (CmdHandler*) parent;
  
    if (strlen(fname) < 2 || strlen(fname) > 11)
    {
       handler->setHint("wrong file name use dos-like file names");
       return WRONG_PARAMS;
    }

    if (passcode != 0xaa55)
    {
       handler->setHint("wrong pass code");
       return WRONG_PARAMS;
    }

    sprintf(path,"/%s", fname);
    if (LittleFS.exists(path))
    {
         LittleFS.remove(path);
         return OK;
    } else {
      handler->setHint("file does not extists!.");
      return FILE_ERROR;
    }
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e files_append(void* parent,int argc, char* argv[])
{

    char *fname = argv[2];
    char path[32];
    CmdHandler* handler = (CmdHandler*) parent;
  
    if (strlen(fname) < 2 || strlen(fname) > 11)
    {
       handler->setHint("wrong file name use dos-like file names");
       return WRONG_PARAMS;
    }

    sprintf(path,"/%s", fname);
    
    File f = LittleFS.open(path, "a");
    if (f)
    {
        for (int pos = 3; pos < argc; pos++)
        {
          f.print(argv[pos]);
          if (pos < argc -1 )
             f.print(" ");
        }
        f.println();
        f.close();
        return OK;
    } else {
      handler->setHint("file append failed!.");
      return FILE_ERROR;
    }
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e files_cat(void* parent,int argc, char* argv[])
{

    char path[32];
    char *fname = argv[2];

    CmdHandler* handler = (CmdHandler*) parent;
  
    if (strlen(fname) < 2 || strlen(fname) > 11)
    {
       handler->setHint("wrong file name use dos-like file names");
       return WRONG_PARAMS;
    }

    sprintf(path,"/%s", fname);
    if (LittleFS.exists(path))
    {
         File f = LittleFS.open(path, "r");
         while (f.available())
         {
             String s = f.readStringUntil('\n');
             STDOUT.print("line: ");
             STDOUT.println(s);
         }

         f.close();        
         return OK;
    } else {
      handler->setHint("file does not extists!.");
      return FILE_ERROR;
    }
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e files_readhex(void* parent,int argc, char* argv[])
{
  return FILE_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
#endif //USE_FILES
