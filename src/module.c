
/*

  Module functions (somes functions taken form psplink source by Tyranid)

  By MPH (mphtheone@hotmail.com)

*/


// *** INCLUDES ***

#include "module.h"


// *** FUNCTIONS ***

SceUID module_load(const char *path, int flags, int type) {
    SceKernelLMOption option;
    SceUID mpid;


    // If the type is 0, then load the module in the kernel partition, otherwise load it in the user partition.
    mpid = (type) ? 2 : 1;

    // Initialize structure
    memset(&option, 0, sizeof(option));
    option.size = sizeof(option);
    option.mpidtext = mpid;
    option.mpiddata = mpid;
    option.position = 0;
    option.access = 1;

    return sceKernelLoadModule(path, flags, (type) ? &option : NULL);
}

u32 module_load_start(const char *path, int flags, int type) {
    SceUID load_result, start_result;
    int status;


    load_result = module_load(path, flags, type);
    if (load_result & 0x80000000) return 1;

    start_result = sceKernelStartModule(load_result, 0, NULL, &status, NULL);
    if (load_result != start_result) return 2;

    return 0;
}

u32 module_unload(const char *name) {
    SceModule *mod;
    SceUID modid;
    u32 ret;


    // Find module address
    mod = sceKernelFindModuleByName(name);

    if (mod) {
        // Stop module
        modid = mod->modid;

        ret = sceKernelStopModule(modid, 0, NULL, NULL, NULL);
        if (!(ret & 0x80000000)) ret = sceKernelUnloadModule(modid);
    } else
        ret = SCE_KERNEL_ERROR_UNKNOWN_MODULE;

    return ret;
}

u32 module_find_proc(const char *sz_mod, const char *sz_lib, u32 nid) {
    SceModule *mod_p = sceKernelFindModuleByName(sz_mod);
    if (mod_p == NULL) {
        return 0;
    }
    SceLibraryEntryTable *ent_p = (SceLibraryEntryTable *) mod_p->ent_top;
    while ((u32) ent_p < ((u32) mod_p->ent_top + mod_p->ent_size)) {
        if (ent_p->libname != NULL && strcmp(ent_p->libname, sz_lib) == 0) {
            // found lib
            int i;
            int count = ent_p->stubcount + ent_p->vstubcount;
            u32 *nidtable = (u32 *) ent_p->entrytable;
            for (i = 0; i < count; i++) {
                if (nidtable[i] == nid) {
                    u32 proc_addr = nidtable[count + i];
                    return proc_addr;
                }
            }
            return 0;
        }
        ent_p++;
    }
    return 0;
}

SceLibraryEntryTable *module_find_library(SceUID modid, const char *library) {
    SceModule *mod;
    SceLibraryEntryTable *entry_table, *entry_end;


    // Find memory of module
    mod = sceKernelFindModuleByUID(modid);

    // If bad module
    if ((((long) mod) & 0xFF000000) != 0x88000000) return NULL;
    if ((mod->stub_top - mod->ent_top) < 40) return NULL;

    // Find entry table
    entry_table = (SceLibraryEntryTable *) ((u32 *) mod->ent_top);
    entry_end = (SceLibraryEntryTable *) (((u8 *) mod->ent_top) + mod->ent_size);

    // Entry table loop
    while (entry_table < entry_end) {
        // Find name
        if (entry_table->libname)// first entry (module info) has name = NULL
        {
            if (!(strcmp(entry_table->libname, library))) return entry_table;
        }

        // Next entry
        entry_table = (SceLibraryEntryTable *) (((u32 *) entry_table) + entry_table->len);
    }

    // Not found
    return NULL;
}

u32 *module_find_func(SceLibraryEntryTable *entry_table, SceUID nid) {
    u32 *entry;
    int x;


    // Verify parameters
    if (!(entry_table)) return NULL;

    // Find entry table
    entry = (u32 *) entry_table->entrytable;

    // NID loop
    for (x = 0; x < entry_table->stubcount; x++) {
        // Find function address
        if (entry[x] == nid) return &entry[x + entry_table->stubcount + entry_table->vstubcount];
    }

    return NULL;
}

u32 *module_find_syscall_func(u32 func) {
    u8 **syscall;
    ModuleSyscallHeader *sysheader;
    u32 *systable;
    int size, x;


    // Get syscall table
    asm("cfc0 %0, $12\n" : "=r"(syscall));

    // Exit if failed
    if (!(syscall)) return NULL;

    // Get syscall header
    sysheader = (ModuleSyscallHeader *) *syscall;

    // Get syscall table
    systable = (u32 *) ((*syscall) + sizeof(ModuleSyscallHeader));

    // Get syscall size
    size = (sysheader->size - sizeof(ModuleSyscallHeader)) / sizeof(u32);

    // Search function
    for (x = 0; x < size; x++) {
        if (systable[x] == func) return &systable[x];
    }

    return NULL;
}

u32 module_hook_addr(u32 *addr, u32 func) {
    int x;


    // Verify parameters
    if (!(addr)) return 1;

    // Disable interrupts
    x = pspSdkDisableInterrupts();

    // Patch address
    *addr = func;

    // Apply to cache
    sceKernelDcacheWritebackInvalidateRange(addr, sizeof(addr));
    sceKernelIcacheInvalidateRange(addr, sizeof(addr));

    // Enable interrupts
    pspSdkEnableInterrupts(x);

    return 0;
}

u32 module_hook_func(ModuleFunc *modfunc, SceUID modid, const char *library, SceUID nid, void *func) {
    u32 *addr;


    // Verify parameters
    if ((!(modfunc)) || (!(library)) || (!(func))) return 1;

    // Find address of function in entry table and get pointer in entry table
    addr = module_find_func(module_find_library(modid, library), nid);

    // If not found
    if (!(addr)) return 2;

    // Copy address of function in structure
    modfunc->addr = *addr;

    // Find address of function in syscall table and get pointer in syscall table
    modfunc->sysaddr = module_find_syscall_func(modfunc->addr);

    // If not found
    if (!(modfunc->sysaddr)) return 3;

    // Hook function (copy func address to syscall table, overwrite old func)
    return module_hook_addr(modfunc->sysaddr, (u32) func);
}

u32 module_restore_func(ModuleFunc *modfunc) {
    // Verify parameters
    if (!(modfunc)) return 1;

    // Restore func
    return module_hook_addr(modfunc->sysaddr, modfunc->addr);
}

u32 module_get_func(ModuleFunc *modfunc, SceUID modid, const char *library, SceUID nid) {
    u32 *addr;


    // Verify parameters
    if ((!(modfunc)) || (!(library))) return 1;

    // Find address of function in entry table and get pointer in entry table
    addr = module_find_func(module_find_library(modid, library), nid);

    // If not found
    if (!(addr)) return 2;

    // Copy address of function in structure
    modfunc->addr = *addr;
    modfunc->sysaddr = 0x0;

    return 0;
}

u32 module_patch_for_reload(const char *name) {
    SceModule *mod;
    SceLibraryEntryTable *entry_table, *entry_end;


    // Find module address
    mod = sceKernelFindModuleByName(name);

    // If bad module
    if ((((long) mod) & 0xFF000000) != 0x88000000) return 1;
    if ((mod->stub_top - mod->ent_top) < 40) return 1;

    // Patch name and attributes
    mod->attribute = 0x1006;
    mod->modname[0] = '*';

    // Find entry table info
    entry_table = (SceLibraryEntryTable *) ((u32 *) mod->ent_top);
    entry_end = (SceLibraryEntryTable *) (((u8 *) mod->ent_top) + mod->ent_size);

    // Entry loop
    while (entry_table < entry_end) {
        // Patch lib name
        if (entry_table->libname) ((char *) entry_table->libname)[0] = '*';

        // Next entry
        entry_table = (SceLibraryEntryTable *) (((u32 *) entry_table) + entry_table->len);
    }

    return 0;
}
