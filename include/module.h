
#ifndef CHEATER_MODULE_H
#define CHEATER_MODULE_H

/*

  Module functions (somes functions taken form psplink source by Tyranid)

  By MPH (mphtheone@hotmail.com)

*/


// *** INCLUDES ***

#include <pspkernel.h>
#include <pspsdk.h>
#include <psputilsforkernel.h>
#include <string.h>


// *** STRUCTURES ***

typedef struct ModuleSyscallHeader {
    void *unk;
    u32 basenum;
    u32 topnum;
    u32 size;
} ModuleSyscallHeader;

typedef struct ModuleFunc {
    u32 addr;
    u32 *sysaddr;
} ModuleFunc;


// *** FUNCTIONS DECLARATIONS ***

SceUID module_load(const char *, int, int);
u32 module_load_start(const char *, int, int);
u32 module_unload(const char *);
u32 module_patch_for_reload(const char *);

SceLibraryEntryTable *module_find_library(SceUID, const char *);
u32 module_find_proc(const char *sz_mod, const char *sz_lib, u32 nid);
u32 *module_find_func(SceLibraryEntryTable *, SceUID);
u32 *module_find_syscall_func(u32);
u32 module_hook_addr(u32 *, u32);
u32 module_hook_func(ModuleFunc *, SceUID, const char *, SceUID, void *);
u32 module_restore_func(ModuleFunc *);
u32 module_get_func(ModuleFunc *, SceUID, const char *, SceUID);

#endif
