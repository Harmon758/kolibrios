

#ifndef __DDK_H__
#define __DDK_H__

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/rwsem.h>
#include <linux/pci.h>


#define OS_BASE             0x80000000

#define PG_SW               0x003
#define PG_UW               0x007
#define PG_NOCACHE          0x018
#define PG_SHARED           0x200

#define MANUAL_DESTROY      0x80000000

#define ENTER()   dbgprintf("enter %s\n",__FUNCTION__)
#define LEAVE()   dbgprintf("leave %s\n",__FUNCTION__)
#define FAIL()    dbgprintf("fail %s\n",__FUNCTION__)

typedef struct
{
    u32  code;
    u32  data[5];
}kevent_t;

typedef union
{
    struct
    {
        u32 handle;
        u32 euid;
    };
    u64 raw;
}evhandle_t;

typedef struct
{
  u32      handle;
  u32      io_code;
  void       *input;
  int        inp_size;
  void       *output;
  int        out_size;
}ioctl_t;

typedef int ( __stdcall *srv_proc_t)(ioctl_t *);

#define ERR_OK       0
#define ERR_PARAM   -1


struct ddk_params;

int   ddk_init(struct ddk_params *params);

u32 drvEntry(int, char *)__asm__("_drvEntry");





#endif      /*    DDK_H    */
