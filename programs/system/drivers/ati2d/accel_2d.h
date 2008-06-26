
#define FILL_RECT  1
#define DRAW_RECT  2
#define LINE_2P    3

typedef unsigned int color_t;
typedef unsigned int u32_t;

typedef struct
{
  int x;
  int y;
  int w;
  int h;
  u32 color;
}draw_t;

typedef struct
{
  int x;
  int y;
  int w;
  int h;

  color_t bkcolor;
  color_t fcolor;

  u32_t   bmp0;
  u32_t   bmp1;
}fill_t;

typedef struct
{
  int x0;
  int y0;
  int x1;
  int y1;
  u32 color;
}line2p_t;

int LineClip( int *x1, int *y1, int *x2, int *y2 );
int BlockClip( int *x1, int *y1, int *x2, int* y2);

int DrawRect(draw_t * draw);
int FillRect(fill_t * fill);

int Line2P(line2p_t *draw);


# define RADEON_GMC_SRC_PITCH_OFFSET_CNTL (1 << 0)
#	define RADEON_GMC_DST_PITCH_OFFSET_CNTL	(1 << 1)
#	define RADEON_GMC_BRUSH_SOLID_COLOR	(13 << 4)
#	define RADEON_GMC_BRUSH_NONE		(15 << 4)
#	define RADEON_GMC_DST_16BPP		(4 << 8)
#	define RADEON_GMC_DST_24BPP		(5 << 8)
#	define RADEON_GMC_DST_32BPP		(6 << 8)
#	define RADEON_GMC_DST_DATATYPE_SHIFT	8
#	define RADEON_GMC_SRC_DATATYPE_COLOR	(3 << 12)
#	define RADEON_DP_SRC_SOURCE_MEMORY	(2 << 24)
#	define RADEON_DP_SRC_SOURCE_HOST_DATA	(3 << 24)
#	define RADEON_GMC_CLR_CMP_CNTL_DIS	(1 << 28)
#	define RADEON_GMC_WR_MSK_DIS		(1 << 30)
#	define RADEON_ROP3_S			0x00cc0000
# define RADEON_ROP3_P      0x00f00000

#define RADEON_CP_PACKET3              0xC0000000

# define RADEON_CNTL_PAINT             0x00009100
# define RADEON_CNTL_PAINT_POLYLINE    0x00009500
# define RADEON_CNTL_PAINT_MULTI       0x00009A00

#define CP_PACKET3( pkt, n )            \
	(RADEON_CP_PACKET3 | (pkt) | ((n) << 16))

#define BEGIN_RING( n ) do {            \
  ring = rhd.ring_base;                 \
  write = rhd.ring_wp;                  \
} while (0)

#define OUT_RING( x ) do {        \
	ring[write++] = (x);						\
} while (0)

#define DRM_MEMORYBARRIER()  __asm volatile("lock; addl $0,0(%%esp)" : : : "memory");

#define COMMIT_RING() do {                            \
  rhd.ring_wp = write & 0x1FFF;                       \
  /* Flush writes to ring */                          \
  DRM_MEMORYBARRIER();                                \
  /*GET_RING_HEAD( dev_priv );          */            \
  OUTREG( RADEON_CP_RB_WPTR, rhd.ring_wp);            \
	/* read from PCI bus to ensure correct posting */		\
  INREG( RADEON_CP_RB_RPTR );                         \
} while (0)

