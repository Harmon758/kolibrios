#include "pci.h"
#include "rhd_regs.h"

enum RHD_CHIPSETS {
    RHD_UNKNOWN = 0,
    /* R500 */
    RHD_RV505,
    RHD_RV515,
    RHD_RV516,
    RHD_R520,
    RHD_RV530,
    RHD_RV535,
    RHD_RV550,
    RHD_RV560,
    RHD_RV570,
    RHD_R580,
    /* R500 Mobility */
    RHD_M52,
    RHD_M54,
    RHD_M56,
    RHD_M58,
    RHD_M62,
    RHD_M64,
    RHD_M66,
    RHD_M68,
    RHD_M71,
    /* R500 integrated */
    RHD_RS600,
    RHD_RS690,
    RHD_RS740,
    /* R600 */
    RHD_R600,
    RHD_RV610,
    RHD_RV630,
    /* R600 Mobility */
    RHD_M72,
    RHD_M74,
    RHD_M76,
    /* RV670 came into existence after RV6x0 and M7x */
    RHD_RV670,
    RHD_R680,
    RHD_RV620,
    RHD_M82,
    RHD_RV635,
    RHD_M86,
    RHD_CHIP_END
};

enum RHD_FAMILIES {
    RHD_FAMILY_UNKNOWN = 0,
    RHD_FAMILY_RV515,
    RHD_FAMILY_R520,
    RHD_FAMILY_RV530,
    RHD_FAMILY_RV560,
    RHD_FAMILY_RV570,
    RHD_FAMILY_R580,
    RHD_FAMILY_RS690,
    RHD_FAMILY_R600,
    RHD_FAMILY_RV610,
    RHD_FAMILY_RV630,
    RHD_FAMILY_RV670,
    RHD_FAMILY_RV620,
    RHD_FAMILY_RV635
};

#define RHD_FB_BAR         0
#define RHD_MMIO_BAR       2

#define RHD_MEM_GART       1
#define RHD_MEM_FB         2

typedef struct RHDRec
{
  CARD32            MMIOBase;
  CARD32            MMIOMapSize;
  CARD32            videoRam;

  CARD32            FbBase;            /* map base of fb   */
  CARD32            PhisBase;
  CARD32            FbIntAddress;      /* card internal address of FB */
  CARD32            FbMapSize;

  CARD32            FbFreeStart;
  CARD32            FbFreeSize;

  /* visible part of the framebuffer */
  unsigned int      FbScanoutStart;
  unsigned int      FbScanoutSize;

  enum RHD_CHIPSETS ChipSet;
  char              *ChipName;

  Bool              IsIGP;

  CARD32            bus;
  CARD32            devfn;

  PCITAG            PciTag;
  CARD16            PciDeviceID;

  CARD16            subvendor_id;
  CARD16            subdevice_id;

  CARD32            memBase[6];
  CARD32            ioBase[6];
  CARD32            memtype[6];
  CARD32            memsize[6];

  struct mem_block  *fb_heap;
  struct mem_block  *gart_heap;

  CARD32            displayWidth;
  CARD32            displayHeight;

  CARD32            __xmin;
  CARD32            __ymin;
  CARD32            __xmax;
  CARD32            __ymax;

  CARD32            gui_control;
  CARD32            dst_pitch_offset;
  CARD32            surface_cntl;

  u32               *ring_base;
  u32               ring_rp;
  u32               ring_wp;

  int               num_gb_pipes;
  Bool              has_tcl;
}RHD_t, *RHDPtr;

extern RHD_t rhd;

typedef struct
{
  int xmin;
  int ymin;
  int xmax;
  int ymax;
}clip_t, *PTRclip;

typedef struct {
    int			token;		/* id of the token */
    const char *	name;		/* token name */
} SymTabRec, *SymTabPtr;

extern inline CARD32 INREG(CARD16 offset)
{
  return *(volatile CARD32 *)((CARD8*)(rhd.MMIOBase + offset));
}

//#define INREG(offset) *(volatile CARD32 *)((CARD8*)(rhd.MMIOBase + (offset)))

extern inline void
OUTREG(CARD16 offset, CARD32 value)
{
  *(volatile CARD32 *)((CARD8 *)(rhd.MMIOBase + offset)) = value;
}

extern inline CARD32 _RHDRegRead(RHDPtr rhdPtr, CARD16 offset)
{
  return *(volatile CARD32 *)((CARD8*)(rhdPtr->MMIOBase + offset));
}

extern inline void
MASKREG(CARD16 offset, CARD32 value, CARD32 mask)
{
  CARD32 tmp;

  tmp = INREG(offset);
  tmp &= ~mask;
  tmp |= (value & mask);
  OUTREG(offset, tmp);
};

extern inline void
_RHDRegWrite(RHDPtr rhdPtr, CARD16 offset, CARD32 value)
{
  *(volatile CARD32 *)((CARD8 *)(rhdPtr->MMIOBase + offset)) = value;
}

extern inline void
_RHDRegMask(RHDPtr rhdPtr, CARD16 offset, CARD32 value, CARD32 mask)
{
  CARD32 tmp;

  tmp = _RHDRegRead(rhdPtr, offset);
  tmp &= ~mask;
  tmp |= (value & mask);
  _RHDRegWrite(rhdPtr, offset, tmp);
};

enum RHD_FAMILIES RHDFamily(enum RHD_CHIPSETS chipset);

#define RHDRegRead(ptr, offset) _RHDRegRead((ptr)->rhdPtr, (offset))
#define RHDRegWrite(ptr, offset, value) _RHDRegWrite((ptr)->rhdPtr, (offset), (value))
#define RHDRegMask(ptr, offset, value, mask) _RHDRegMask((ptr)->rhdPtr, (offset), (value), (mask))


RHDPtr FindPciDevice();

Bool RHDPreInit();
int rhdInitHeap(RHDPtr rhdPtr);

#define RHDFUNC(ptr)

#define DBG(x) x
//  #define DBG(x)

#pragma pack (push,1)
typedef struct s_cursor
{
   u32   magic;                           // 'CURS'
   void  (*destroy)(struct s_cursor*);    // destructor
   u32   fd;                              // next object in list
   u32   bk;                              // prev object in list
   u32   pid;                             // owner id

   void *base;                            // allocated memory
   u32   hot_x;                           // hotspot coords
   u32   hot_y;
}cursor_t;
#pragma pack (pop)

#define LOAD_FROM_FILE   0
#define LOAD_FROM_MEM    1
#define LOAD_INDIRECT    2

cursor_t *create_cursor(u32 pid, void *src, u32 flags);
void __stdcall copy_cursor(void *img, void *src);
void destroy_cursor(cursor_t *cursor);
void __destroy_cursor(cursor_t *cursor);                // wrap

void __stdcall r500_SelectCursor(cursor_t *cursor);
void __stdcall r500_SetCursor(cursor_t *cursor, int x, int y);
void __stdcall r500_CursorRestore(int x, int y);

void  R5xx2DInit();
