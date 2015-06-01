/*
 Copyright (C) Intel Corp.  2006.  All Rights Reserved.
 Intel funded Tungsten Graphics to
 develop this 3D driver.

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice (including the
 next paragraph) shall be included in all copies or substantial
 portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE COPYRIGHT OWNER(S) AND/OR ITS SUPPLIERS BE
 LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 **********************************************************************/
 /*
  * Authors:
  *   Keith Whitwell <keithw@vmware.com>
  */

#define INTEL_MASK(high, low) (((1<<((high)-(low)+1))-1)<<(low))
/* Using the GNU statement expression extension */
#define SET_FIELD(value, field)                                         \
   ({                                                                   \
      uint32_t fieldval = (value) << field ## _SHIFT;                   \
      assert((fieldval & ~ field ## _MASK) == 0);                       \
      fieldval & field ## _MASK;                                        \
   })

#define GET_BITS(data, high, low) ((data & INTEL_MASK((high), (low))) >> (low))
#define GET_FIELD(word, field) (((word)  & field ## _MASK) >> field ## _SHIFT)

#ifndef BRW_DEFINES_H
#define BRW_DEFINES_H

/* 3D state:
 */
#define CMD_3D_PRIM                                 0x7b00 /* 3DPRIMITIVE */
/* DW0 */
# define GEN4_3DPRIM_TOPOLOGY_TYPE_SHIFT            10
# define GEN4_3DPRIM_VERTEXBUFFER_ACCESS_SEQUENTIAL (0 << 15)
# define GEN4_3DPRIM_VERTEXBUFFER_ACCESS_RANDOM     (1 << 15)
# define GEN7_3DPRIM_INDIRECT_PARAMETER_ENABLE      (1 << 10)
# define GEN7_3DPRIM_PREDICATE_ENABLE               (1 << 8)
/* DW1 */
# define GEN7_3DPRIM_VERTEXBUFFER_ACCESS_SEQUENTIAL (0 << 8)
# define GEN7_3DPRIM_VERTEXBUFFER_ACCESS_RANDOM     (1 << 8)

#define _3DPRIM_POINTLIST         0x01
#define _3DPRIM_LINELIST          0x02
#define _3DPRIM_LINESTRIP         0x03
#define _3DPRIM_TRILIST           0x04
#define _3DPRIM_TRISTRIP          0x05
#define _3DPRIM_TRIFAN            0x06
#define _3DPRIM_QUADLIST          0x07
#define _3DPRIM_QUADSTRIP         0x08
#define _3DPRIM_LINELIST_ADJ      0x09
#define _3DPRIM_LINESTRIP_ADJ     0x0A
#define _3DPRIM_TRILIST_ADJ       0x0B
#define _3DPRIM_TRISTRIP_ADJ      0x0C
#define _3DPRIM_TRISTRIP_REVERSE  0x0D
#define _3DPRIM_POLYGON           0x0E
#define _3DPRIM_RECTLIST          0x0F
#define _3DPRIM_LINELOOP          0x10
#define _3DPRIM_POINTLIST_BF      0x11
#define _3DPRIM_LINESTRIP_CONT    0x12
#define _3DPRIM_LINESTRIP_BF      0x13
#define _3DPRIM_LINESTRIP_CONT_BF 0x14
#define _3DPRIM_TRIFAN_NOSTIPPLE  0x15

/* We use this offset to be able to pass native primitive types in struct
 * _mesa_prim::mode.  Native primitive types are BRW_PRIM_OFFSET +
 * native_type, which should be different from all GL types and still fit in
 * the 8 bits avialable. */

#define BRW_PRIM_OFFSET           0x80

#define BRW_ANISORATIO_2     0
#define BRW_ANISORATIO_4     1
#define BRW_ANISORATIO_6     2
#define BRW_ANISORATIO_8     3
#define BRW_ANISORATIO_10    4
#define BRW_ANISORATIO_12    5
#define BRW_ANISORATIO_14    6
#define BRW_ANISORATIO_16    7

#define BRW_BLENDFACTOR_ONE                 0x1
#define BRW_BLENDFACTOR_SRC_COLOR           0x2
#define BRW_BLENDFACTOR_SRC_ALPHA           0x3
#define BRW_BLENDFACTOR_DST_ALPHA           0x4
#define BRW_BLENDFACTOR_DST_COLOR           0x5
#define BRW_BLENDFACTOR_SRC_ALPHA_SATURATE  0x6
#define BRW_BLENDFACTOR_CONST_COLOR         0x7
#define BRW_BLENDFACTOR_CONST_ALPHA         0x8
#define BRW_BLENDFACTOR_SRC1_COLOR          0x9
#define BRW_BLENDFACTOR_SRC1_ALPHA          0x0A
#define BRW_BLENDFACTOR_ZERO                0x11
#define BRW_BLENDFACTOR_INV_SRC_COLOR       0x12
#define BRW_BLENDFACTOR_INV_SRC_ALPHA       0x13
#define BRW_BLENDFACTOR_INV_DST_ALPHA       0x14
#define BRW_BLENDFACTOR_INV_DST_COLOR       0x15
#define BRW_BLENDFACTOR_INV_CONST_COLOR     0x17
#define BRW_BLENDFACTOR_INV_CONST_ALPHA     0x18
#define BRW_BLENDFACTOR_INV_SRC1_COLOR      0x19
#define BRW_BLENDFACTOR_INV_SRC1_ALPHA      0x1A

#define BRW_BLENDFUNCTION_ADD               0
#define BRW_BLENDFUNCTION_SUBTRACT          1
#define BRW_BLENDFUNCTION_REVERSE_SUBTRACT  2
#define BRW_BLENDFUNCTION_MIN               3
#define BRW_BLENDFUNCTION_MAX               4

#define BRW_ALPHATEST_FORMAT_UNORM8         0
#define BRW_ALPHATEST_FORMAT_FLOAT32        1

#define BRW_CHROMAKEY_KILL_ON_ANY_MATCH  0
#define BRW_CHROMAKEY_REPLACE_BLACK      1

#define BRW_CLIP_API_OGL     0
#define BRW_CLIP_API_DX      1

#define BRW_CLIPMODE_NORMAL              0
#define BRW_CLIPMODE_CLIP_ALL            1
#define BRW_CLIPMODE_CLIP_NON_REJECTED   2
#define BRW_CLIPMODE_REJECT_ALL          3
#define BRW_CLIPMODE_ACCEPT_ALL          4
#define BRW_CLIPMODE_KERNEL_CLIP         5

#define BRW_CLIP_NDCSPACE     0
#define BRW_CLIP_SCREENSPACE  1

#define BRW_COMPAREFUNCTION_ALWAYS       0
#define BRW_COMPAREFUNCTION_NEVER        1
#define BRW_COMPAREFUNCTION_LESS         2
#define BRW_COMPAREFUNCTION_EQUAL        3
#define BRW_COMPAREFUNCTION_LEQUAL       4
#define BRW_COMPAREFUNCTION_GREATER      5
#define BRW_COMPAREFUNCTION_NOTEQUAL     6
#define BRW_COMPAREFUNCTION_GEQUAL       7

#define BRW_COVERAGE_PIXELS_HALF     0
#define BRW_COVERAGE_PIXELS_1        1
#define BRW_COVERAGE_PIXELS_2        2
#define BRW_COVERAGE_PIXELS_4        3

#define BRW_CULLMODE_BOTH        0
#define BRW_CULLMODE_NONE        1
#define BRW_CULLMODE_FRONT       2
#define BRW_CULLMODE_BACK        3

#define BRW_DEFAULTCOLOR_R8G8B8A8_UNORM      0
#define BRW_DEFAULTCOLOR_R32G32B32A32_FLOAT  1

#define BRW_DEPTHFORMAT_D32_FLOAT_S8X24_UINT     0
#define BRW_DEPTHFORMAT_D32_FLOAT                1
#define BRW_DEPTHFORMAT_D24_UNORM_S8_UINT        2
#define BRW_DEPTHFORMAT_D24_UNORM_X8_UINT        3 /* GEN5 */
#define BRW_DEPTHFORMAT_D16_UNORM                5

#define BRW_FLOATING_POINT_IEEE_754        0
#define BRW_FLOATING_POINT_NON_IEEE_754    1

#define BRW_FRONTWINDING_CW      0
#define BRW_FRONTWINDING_CCW     1

#define BRW_SPRITE_POINT_ENABLE  16

#define BRW_CUT_INDEX_ENABLE     (1 << 10)

#define BRW_INDEX_BYTE     0
#define BRW_INDEX_WORD     1
#define BRW_INDEX_DWORD    2

#define BRW_LOGICOPFUNCTION_CLEAR            0
#define BRW_LOGICOPFUNCTION_NOR              1
#define BRW_LOGICOPFUNCTION_AND_INVERTED     2
#define BRW_LOGICOPFUNCTION_COPY_INVERTED    3
#define BRW_LOGICOPFUNCTION_AND_REVERSE      4
#define BRW_LOGICOPFUNCTION_INVERT           5
#define BRW_LOGICOPFUNCTION_XOR              6
#define BRW_LOGICOPFUNCTION_NAND             7
#define BRW_LOGICOPFUNCTION_AND              8
#define BRW_LOGICOPFUNCTION_EQUIV            9
#define BRW_LOGICOPFUNCTION_NOOP             10
#define BRW_LOGICOPFUNCTION_OR_INVERTED      11
#define BRW_LOGICOPFUNCTION_COPY             12
#define BRW_LOGICOPFUNCTION_OR_REVERSE       13
#define BRW_LOGICOPFUNCTION_OR               14
#define BRW_LOGICOPFUNCTION_SET              15

#define BRW_MAPFILTER_NEAREST        0x0
#define BRW_MAPFILTER_LINEAR         0x1
#define BRW_MAPFILTER_ANISOTROPIC    0x2

#define BRW_MIPFILTER_NONE        0
#define BRW_MIPFILTER_NEAREST     1
#define BRW_MIPFILTER_LINEAR      3

#define BRW_ADDRESS_ROUNDING_ENABLE_U_MAG	0x20
#define BRW_ADDRESS_ROUNDING_ENABLE_U_MIN	0x10
#define BRW_ADDRESS_ROUNDING_ENABLE_V_MAG	0x08
#define BRW_ADDRESS_ROUNDING_ENABLE_V_MIN	0x04
#define BRW_ADDRESS_ROUNDING_ENABLE_R_MAG	0x02
#define BRW_ADDRESS_ROUNDING_ENABLE_R_MIN	0x01

#define BRW_POLYGON_FRONT_FACING     0
#define BRW_POLYGON_BACK_FACING      1

#define BRW_PREFILTER_ALWAYS     0x0
#define BRW_PREFILTER_NEVER      0x1
#define BRW_PREFILTER_LESS       0x2
#define BRW_PREFILTER_EQUAL      0x3
#define BRW_PREFILTER_LEQUAL     0x4
#define BRW_PREFILTER_GREATER    0x5
#define BRW_PREFILTER_NOTEQUAL   0x6
#define BRW_PREFILTER_GEQUAL     0x7

#define BRW_PROVOKING_VERTEX_0    0
#define BRW_PROVOKING_VERTEX_1    1
#define BRW_PROVOKING_VERTEX_2    2

#define BRW_RASTRULE_UPPER_LEFT  0
#define BRW_RASTRULE_UPPER_RIGHT 1
/* These are listed as "Reserved, but not seen as useful"
 * in Intel documentation (page 212, "Point Rasterization Rule",
 * section 7.4 "SF Pipeline State Summary", of document
 * "Intel® 965 Express Chipset Family and Intel® G35 Express
 * Chipset Graphics Controller Programmer's Reference Manual,
 * Volume 2: 3D/Media", Revision 1.0b as of January 2008,
 * available at
 *     http://intellinuxgraphics.org/documentation.html
 * at the time of this writing).
 *
 * These appear to be supported on at least some
 * i965-family devices, and the BRW_RASTRULE_LOWER_RIGHT
 * is useful when using OpenGL to render to a FBO
 * (which has the pixel coordinate Y orientation inverted
 * with respect to the normal OpenGL pixel coordinate system).
 */
#define BRW_RASTRULE_LOWER_LEFT  2
#define BRW_RASTRULE_LOWER_RIGHT 3

#define BRW_RENDERTARGET_CLAMPRANGE_UNORM    0
#define BRW_RENDERTARGET_CLAMPRANGE_SNORM    1
#define BRW_RENDERTARGET_CLAMPRANGE_FORMAT   2

#define BRW_STENCILOP_KEEP               0
#define BRW_STENCILOP_ZERO               1
#define BRW_STENCILOP_REPLACE            2
#define BRW_STENCILOP_INCRSAT            3
#define BRW_STENCILOP_DECRSAT            4
#define BRW_STENCILOP_INCR               5
#define BRW_STENCILOP_DECR               6
#define BRW_STENCILOP_INVERT             7

/* Surface state DW0 */
#define GEN8_SURFACE_IS_ARRAY                       (1 << 28)
#define GEN8_SURFACE_VALIGN_4                       (1 << 16)
#define GEN8_SURFACE_VALIGN_8                       (2 << 16)
#define GEN8_SURFACE_VALIGN_16                      (3 << 16)
#define GEN8_SURFACE_HALIGN_4                       (1 << 14)
#define GEN8_SURFACE_HALIGN_8                       (2 << 14)
#define GEN8_SURFACE_HALIGN_16                      (3 << 14)
#define GEN8_SURFACE_TILING_NONE                    (0 << 12)
#define GEN8_SURFACE_TILING_W                       (1 << 12)
#define GEN8_SURFACE_TILING_X                       (2 << 12)
#define GEN8_SURFACE_TILING_Y                       (3 << 12)
#define BRW_SURFACE_RC_READ_WRITE	(1 << 8)
#define BRW_SURFACE_MIPLAYOUT_SHIFT	10
#define BRW_SURFACE_MIPMAPLAYOUT_BELOW   0
#define BRW_SURFACE_MIPMAPLAYOUT_RIGHT   1
#define BRW_SURFACE_CUBEFACE_ENABLES	0x3f
#define BRW_SURFACE_BLEND_ENABLED	(1 << 13)
#define BRW_SURFACE_WRITEDISABLE_B_SHIFT	14
#define BRW_SURFACE_WRITEDISABLE_G_SHIFT	15
#define BRW_SURFACE_WRITEDISABLE_R_SHIFT	16
#define BRW_SURFACE_WRITEDISABLE_A_SHIFT	17

#define BRW_SURFACEFORMAT_R32G32B32A32_FLOAT             0x000
#define BRW_SURFACEFORMAT_R32G32B32A32_SINT              0x001
#define BRW_SURFACEFORMAT_R32G32B32A32_UINT              0x002
#define BRW_SURFACEFORMAT_R32G32B32A32_UNORM             0x003
#define BRW_SURFACEFORMAT_R32G32B32A32_SNORM             0x004
#define BRW_SURFACEFORMAT_R64G64_FLOAT                   0x005
#define BRW_SURFACEFORMAT_R32G32B32X32_FLOAT             0x006
#define BRW_SURFACEFORMAT_R32G32B32A32_SSCALED           0x007
#define BRW_SURFACEFORMAT_R32G32B32A32_USCALED           0x008
#define BRW_SURFACEFORMAT_R32G32B32A32_SFIXED            0x020
#define BRW_SURFACEFORMAT_R64G64_PASSTHRU                0x021
#define BRW_SURFACEFORMAT_R32G32B32_FLOAT                0x040
#define BRW_SURFACEFORMAT_R32G32B32_SINT                 0x041
#define BRW_SURFACEFORMAT_R32G32B32_UINT                 0x042
#define BRW_SURFACEFORMAT_R32G32B32_UNORM                0x043
#define BRW_SURFACEFORMAT_R32G32B32_SNORM                0x044
#define BRW_SURFACEFORMAT_R32G32B32_SSCALED              0x045
#define BRW_SURFACEFORMAT_R32G32B32_USCALED              0x046
#define BRW_SURFACEFORMAT_R32G32B32_SFIXED               0x050
#define BRW_SURFACEFORMAT_R16G16B16A16_UNORM             0x080
#define BRW_SURFACEFORMAT_R16G16B16A16_SNORM             0x081
#define BRW_SURFACEFORMAT_R16G16B16A16_SINT              0x082
#define BRW_SURFACEFORMAT_R16G16B16A16_UINT              0x083
#define BRW_SURFACEFORMAT_R16G16B16A16_FLOAT             0x084
#define BRW_SURFACEFORMAT_R32G32_FLOAT                   0x085
#define BRW_SURFACEFORMAT_R32G32_SINT                    0x086
#define BRW_SURFACEFORMAT_R32G32_UINT                    0x087
#define BRW_SURFACEFORMAT_R32_FLOAT_X8X24_TYPELESS       0x088
#define BRW_SURFACEFORMAT_X32_TYPELESS_G8X24_UINT        0x089
#define BRW_SURFACEFORMAT_L32A32_FLOAT                   0x08A
#define BRW_SURFACEFORMAT_R32G32_UNORM                   0x08B
#define BRW_SURFACEFORMAT_R32G32_SNORM                   0x08C
#define BRW_SURFACEFORMAT_R64_FLOAT                      0x08D
#define BRW_SURFACEFORMAT_R16G16B16X16_UNORM             0x08E
#define BRW_SURFACEFORMAT_R16G16B16X16_FLOAT             0x08F
#define BRW_SURFACEFORMAT_A32X32_FLOAT                   0x090
#define BRW_SURFACEFORMAT_L32X32_FLOAT                   0x091
#define BRW_SURFACEFORMAT_I32X32_FLOAT                   0x092
#define BRW_SURFACEFORMAT_R16G16B16A16_SSCALED           0x093
#define BRW_SURFACEFORMAT_R16G16B16A16_USCALED           0x094
#define BRW_SURFACEFORMAT_R32G32_SSCALED                 0x095
#define BRW_SURFACEFORMAT_R32G32_USCALED                 0x096
#define BRW_SURFACEFORMAT_R32G32_FLOAT_LD                0x097
#define BRW_SURFACEFORMAT_R32G32_SFIXED                  0x0A0
#define BRW_SURFACEFORMAT_R64_PASSTHRU                   0x0A1
#define BRW_SURFACEFORMAT_B8G8R8A8_UNORM                 0x0C0
#define BRW_SURFACEFORMAT_B8G8R8A8_UNORM_SRGB            0x0C1
#define BRW_SURFACEFORMAT_R10G10B10A2_UNORM              0x0C2
#define BRW_SURFACEFORMAT_R10G10B10A2_UNORM_SRGB         0x0C3
#define BRW_SURFACEFORMAT_R10G10B10A2_UINT               0x0C4
#define BRW_SURFACEFORMAT_R10G10B10_SNORM_A2_UNORM       0x0C5
#define BRW_SURFACEFORMAT_R8G8B8A8_UNORM                 0x0C7
#define BRW_SURFACEFORMAT_R8G8B8A8_UNORM_SRGB            0x0C8
#define BRW_SURFACEFORMAT_R8G8B8A8_SNORM                 0x0C9
#define BRW_SURFACEFORMAT_R8G8B8A8_SINT                  0x0CA
#define BRW_SURFACEFORMAT_R8G8B8A8_UINT                  0x0CB
#define BRW_SURFACEFORMAT_R16G16_UNORM                   0x0CC
#define BRW_SURFACEFORMAT_R16G16_SNORM                   0x0CD
#define BRW_SURFACEFORMAT_R16G16_SINT                    0x0CE
#define BRW_SURFACEFORMAT_R16G16_UINT                    0x0CF
#define BRW_SURFACEFORMAT_R16G16_FLOAT                   0x0D0
#define BRW_SURFACEFORMAT_B10G10R10A2_UNORM              0x0D1
#define BRW_SURFACEFORMAT_B10G10R10A2_UNORM_SRGB         0x0D2
#define BRW_SURFACEFORMAT_R11G11B10_FLOAT                0x0D3
#define BRW_SURFACEFORMAT_R32_SINT                       0x0D6
#define BRW_SURFACEFORMAT_R32_UINT                       0x0D7
#define BRW_SURFACEFORMAT_R32_FLOAT                      0x0D8
#define BRW_SURFACEFORMAT_R24_UNORM_X8_TYPELESS          0x0D9
#define BRW_SURFACEFORMAT_X24_TYPELESS_G8_UINT           0x0DA
#define BRW_SURFACEFORMAT_L16A16_UNORM                   0x0DF
#define BRW_SURFACEFORMAT_I24X8_UNORM                    0x0E0
#define BRW_SURFACEFORMAT_L24X8_UNORM                    0x0E1
#define BRW_SURFACEFORMAT_A24X8_UNORM                    0x0E2
#define BRW_SURFACEFORMAT_I32_FLOAT                      0x0E3
#define BRW_SURFACEFORMAT_L32_FLOAT                      0x0E4
#define BRW_SURFACEFORMAT_A32_FLOAT                      0x0E5
#define BRW_SURFACEFORMAT_B8G8R8X8_UNORM                 0x0E9
#define BRW_SURFACEFORMAT_B8G8R8X8_UNORM_SRGB            0x0EA
#define BRW_SURFACEFORMAT_R8G8B8X8_UNORM                 0x0EB
#define BRW_SURFACEFORMAT_R8G8B8X8_UNORM_SRGB            0x0EC
#define BRW_SURFACEFORMAT_R9G9B9E5_SHAREDEXP             0x0ED
#define BRW_SURFACEFORMAT_B10G10R10X2_UNORM              0x0EE
#define BRW_SURFACEFORMAT_L16A16_FLOAT                   0x0F0
#define BRW_SURFACEFORMAT_R32_UNORM                      0x0F1
#define BRW_SURFACEFORMAT_R32_SNORM                      0x0F2
#define BRW_SURFACEFORMAT_R10G10B10X2_USCALED            0x0F3
#define BRW_SURFACEFORMAT_R8G8B8A8_SSCALED               0x0F4
#define BRW_SURFACEFORMAT_R8G8B8A8_USCALED               0x0F5
#define BRW_SURFACEFORMAT_R16G16_SSCALED                 0x0F6
#define BRW_SURFACEFORMAT_R16G16_USCALED                 0x0F7
#define BRW_SURFACEFORMAT_R32_SSCALED                    0x0F8
#define BRW_SURFACEFORMAT_R32_USCALED                    0x0F9
#define BRW_SURFACEFORMAT_B5G6R5_UNORM                   0x100
#define BRW_SURFACEFORMAT_B5G6R5_UNORM_SRGB              0x101
#define BRW_SURFACEFORMAT_B5G5R5A1_UNORM                 0x102
#define BRW_SURFACEFORMAT_B5G5R5A1_UNORM_SRGB            0x103
#define BRW_SURFACEFORMAT_B4G4R4A4_UNORM                 0x104
#define BRW_SURFACEFORMAT_B4G4R4A4_UNORM_SRGB            0x105
#define BRW_SURFACEFORMAT_R8G8_UNORM                     0x106
#define BRW_SURFACEFORMAT_R8G8_SNORM                     0x107
#define BRW_SURFACEFORMAT_R8G8_SINT                      0x108
#define BRW_SURFACEFORMAT_R8G8_UINT                      0x109
#define BRW_SURFACEFORMAT_R16_UNORM                      0x10A
#define BRW_SURFACEFORMAT_R16_SNORM                      0x10B
#define BRW_SURFACEFORMAT_R16_SINT                       0x10C
#define BRW_SURFACEFORMAT_R16_UINT                       0x10D
#define BRW_SURFACEFORMAT_R16_FLOAT                      0x10E
#define BRW_SURFACEFORMAT_A8P8_UNORM_PALETTE0            0x10F
#define BRW_SURFACEFORMAT_A8P8_UNORM_PALETTE1            0x110
#define BRW_SURFACEFORMAT_I16_UNORM                      0x111
#define BRW_SURFACEFORMAT_L16_UNORM                      0x112
#define BRW_SURFACEFORMAT_A16_UNORM                      0x113
#define BRW_SURFACEFORMAT_L8A8_UNORM                     0x114
#define BRW_SURFACEFORMAT_I16_FLOAT                      0x115
#define BRW_SURFACEFORMAT_L16_FLOAT                      0x116
#define BRW_SURFACEFORMAT_A16_FLOAT                      0x117
#define BRW_SURFACEFORMAT_L8A8_UNORM_SRGB                0x118
#define BRW_SURFACEFORMAT_R5G5_SNORM_B6_UNORM            0x119
#define BRW_SURFACEFORMAT_B5G5R5X1_UNORM                 0x11A
#define BRW_SURFACEFORMAT_B5G5R5X1_UNORM_SRGB            0x11B
#define BRW_SURFACEFORMAT_R8G8_SSCALED                   0x11C
#define BRW_SURFACEFORMAT_R8G8_USCALED                   0x11D
#define BRW_SURFACEFORMAT_R16_SSCALED                    0x11E
#define BRW_SURFACEFORMAT_R16_USCALED                    0x11F
#define BRW_SURFACEFORMAT_P8A8_UNORM_PALETTE0            0x122
#define BRW_SURFACEFORMAT_P8A8_UNORM_PALETTE1            0x123
#define BRW_SURFACEFORMAT_A1B5G5R5_UNORM                 0x124
#define BRW_SURFACEFORMAT_A4B4G4R4_UNORM                 0x125
#define BRW_SURFACEFORMAT_L8A8_UINT                      0x126
#define BRW_SURFACEFORMAT_L8A8_SINT                      0x127
#define BRW_SURFACEFORMAT_R8_UNORM                       0x140
#define BRW_SURFACEFORMAT_R8_SNORM                       0x141
#define BRW_SURFACEFORMAT_R8_SINT                        0x142
#define BRW_SURFACEFORMAT_R8_UINT                        0x143
#define BRW_SURFACEFORMAT_A8_UNORM                       0x144
#define BRW_SURFACEFORMAT_I8_UNORM                       0x145
#define BRW_SURFACEFORMAT_L8_UNORM                       0x146
#define BRW_SURFACEFORMAT_P4A4_UNORM                     0x147
#define BRW_SURFACEFORMAT_A4P4_UNORM                     0x148
#define BRW_SURFACEFORMAT_R8_SSCALED                     0x149
#define BRW_SURFACEFORMAT_R8_USCALED                     0x14A
#define BRW_SURFACEFORMAT_P8_UNORM_PALETTE0              0x14B
#define BRW_SURFACEFORMAT_L8_UNORM_SRGB                  0x14C
#define BRW_SURFACEFORMAT_P8_UNORM_PALETTE1              0x14D
#define BRW_SURFACEFORMAT_P4A4_UNORM_PALETTE1            0x14E
#define BRW_SURFACEFORMAT_A4P4_UNORM_PALETTE1            0x14F
#define BRW_SURFACEFORMAT_Y8_SNORM                       0x150
#define BRW_SURFACEFORMAT_L8_UINT                        0x152
#define BRW_SURFACEFORMAT_L8_SINT                        0x153
#define BRW_SURFACEFORMAT_I8_UINT                        0x154
#define BRW_SURFACEFORMAT_I8_SINT                        0x155
#define BRW_SURFACEFORMAT_DXT1_RGB_SRGB                  0x180
#define BRW_SURFACEFORMAT_R1_UINT                        0x181
#define BRW_SURFACEFORMAT_YCRCB_NORMAL                   0x182
#define BRW_SURFACEFORMAT_YCRCB_SWAPUVY                  0x183
#define BRW_SURFACEFORMAT_P2_UNORM_PALETTE0              0x184
#define BRW_SURFACEFORMAT_P2_UNORM_PALETTE1              0x185
#define BRW_SURFACEFORMAT_BC1_UNORM                      0x186
#define BRW_SURFACEFORMAT_BC2_UNORM                      0x187
#define BRW_SURFACEFORMAT_BC3_UNORM                      0x188
#define BRW_SURFACEFORMAT_BC4_UNORM                      0x189
#define BRW_SURFACEFORMAT_BC5_UNORM                      0x18A
#define BRW_SURFACEFORMAT_BC1_UNORM_SRGB                 0x18B
#define BRW_SURFACEFORMAT_BC2_UNORM_SRGB                 0x18C
#define BRW_SURFACEFORMAT_BC3_UNORM_SRGB                 0x18D
#define BRW_SURFACEFORMAT_MONO8                          0x18E
#define BRW_SURFACEFORMAT_YCRCB_SWAPUV                   0x18F
#define BRW_SURFACEFORMAT_YCRCB_SWAPY                    0x190
#define BRW_SURFACEFORMAT_DXT1_RGB                       0x191
#define BRW_SURFACEFORMAT_FXT1                           0x192
#define BRW_SURFACEFORMAT_R8G8B8_UNORM                   0x193
#define BRW_SURFACEFORMAT_R8G8B8_SNORM                   0x194
#define BRW_SURFACEFORMAT_R8G8B8_SSCALED                 0x195
#define BRW_SURFACEFORMAT_R8G8B8_USCALED                 0x196
#define BRW_SURFACEFORMAT_R64G64B64A64_FLOAT             0x197
#define BRW_SURFACEFORMAT_R64G64B64_FLOAT                0x198
#define BRW_SURFACEFORMAT_BC4_SNORM                      0x199
#define BRW_SURFACEFORMAT_BC5_SNORM                      0x19A
#define BRW_SURFACEFORMAT_R16G16B16_FLOAT                0x19B
#define BRW_SURFACEFORMAT_R16G16B16_UNORM                0x19C
#define BRW_SURFACEFORMAT_R16G16B16_SNORM                0x19D
#define BRW_SURFACEFORMAT_R16G16B16_SSCALED              0x19E
#define BRW_SURFACEFORMAT_R16G16B16_USCALED              0x19F
#define BRW_SURFACEFORMAT_BC6H_SF16                      0x1A1
#define BRW_SURFACEFORMAT_BC7_UNORM                      0x1A2
#define BRW_SURFACEFORMAT_BC7_UNORM_SRGB                 0x1A3
#define BRW_SURFACEFORMAT_BC6H_UF16                      0x1A4
#define BRW_SURFACEFORMAT_PLANAR_420_8                   0x1A5
#define BRW_SURFACEFORMAT_R8G8B8_UNORM_SRGB              0x1A8
#define BRW_SURFACEFORMAT_ETC1_RGB8                      0x1A9
#define BRW_SURFACEFORMAT_ETC2_RGB8                      0x1AA
#define BRW_SURFACEFORMAT_EAC_R11                        0x1AB
#define BRW_SURFACEFORMAT_EAC_RG11                       0x1AC
#define BRW_SURFACEFORMAT_EAC_SIGNED_R11                 0x1AD
#define BRW_SURFACEFORMAT_EAC_SIGNED_RG11                0x1AE
#define BRW_SURFACEFORMAT_ETC2_SRGB8                     0x1AF
#define BRW_SURFACEFORMAT_R16G16B16_UINT                 0x1B0
#define BRW_SURFACEFORMAT_R16G16B16_SINT                 0x1B1
#define BRW_SURFACEFORMAT_R32_SFIXED                     0x1B2
#define BRW_SURFACEFORMAT_R10G10B10A2_SNORM              0x1B3
#define BRW_SURFACEFORMAT_R10G10B10A2_USCALED            0x1B4
#define BRW_SURFACEFORMAT_R10G10B10A2_SSCALED            0x1B5
#define BRW_SURFACEFORMAT_R10G10B10A2_SINT               0x1B6
#define BRW_SURFACEFORMAT_B10G10R10A2_SNORM              0x1B7
#define BRW_SURFACEFORMAT_B10G10R10A2_USCALED            0x1B8
#define BRW_SURFACEFORMAT_B10G10R10A2_SSCALED            0x1B9
#define BRW_SURFACEFORMAT_B10G10R10A2_UINT               0x1BA
#define BRW_SURFACEFORMAT_B10G10R10A2_SINT               0x1BB
#define BRW_SURFACEFORMAT_R64G64B64A64_PASSTHRU          0x1BC
#define BRW_SURFACEFORMAT_R64G64B64_PASSTHRU             0x1BD
#define BRW_SURFACEFORMAT_ETC2_RGB8_PTA                  0x1C0
#define BRW_SURFACEFORMAT_ETC2_SRGB8_PTA                 0x1C1
#define BRW_SURFACEFORMAT_ETC2_EAC_RGBA8                 0x1C2
#define BRW_SURFACEFORMAT_ETC2_EAC_SRGB8_A8              0x1C3
#define BRW_SURFACEFORMAT_R8G8B8_UINT                    0x1C8
#define BRW_SURFACEFORMAT_R8G8B8_SINT                    0x1C9
#define BRW_SURFACEFORMAT_RAW                            0x1FF
#define BRW_SURFACE_FORMAT_SHIFT	18
#define BRW_SURFACE_FORMAT_MASK		INTEL_MASK(26, 18)

#define BRW_SURFACERETURNFORMAT_FLOAT32  0
#define BRW_SURFACERETURNFORMAT_S1       1

#define BRW_SURFACE_TYPE_SHIFT		29
#define BRW_SURFACE_TYPE_MASK		INTEL_MASK(31, 29)
#define BRW_SURFACE_1D      0
#define BRW_SURFACE_2D      1
#define BRW_SURFACE_3D      2
#define BRW_SURFACE_CUBE    3
#define BRW_SURFACE_BUFFER  4
#define BRW_SURFACE_NULL    7

#define GEN7_SURFACE_IS_ARRAY           (1 << 28)
#define GEN7_SURFACE_VALIGN_2           (0 << 16)
#define GEN7_SURFACE_VALIGN_4           (1 << 16)
#define GEN7_SURFACE_HALIGN_4           (0 << 15)
#define GEN7_SURFACE_HALIGN_8           (1 << 15)
#define GEN7_SURFACE_TILING_NONE        (0 << 13)
#define GEN7_SURFACE_TILING_X           (2 << 13)
#define GEN7_SURFACE_TILING_Y           (3 << 13)
#define GEN7_SURFACE_ARYSPC_FULL	(0 << 10)
#define GEN7_SURFACE_ARYSPC_LOD0	(1 << 10)

/* Surface state DW1 */
#define GEN8_SURFACE_MOCS_SHIFT         24
#define GEN8_SURFACE_MOCS_MASK          INTEL_MASK(30, 24)
#define GEN8_SURFACE_QPITCH_SHIFT       0
#define GEN8_SURFACE_QPITCH_MASK        INTEL_MASK(14, 0)

/* Surface state DW2 */
#define BRW_SURFACE_HEIGHT_SHIFT	19
#define BRW_SURFACE_HEIGHT_MASK		INTEL_MASK(31, 19)
#define BRW_SURFACE_WIDTH_SHIFT		6
#define BRW_SURFACE_WIDTH_MASK		INTEL_MASK(18, 6)
#define BRW_SURFACE_LOD_SHIFT		2
#define BRW_SURFACE_LOD_MASK		INTEL_MASK(5, 2)
#define GEN7_SURFACE_HEIGHT_SHIFT       16
#define GEN7_SURFACE_HEIGHT_MASK        INTEL_MASK(29, 16)
#define GEN7_SURFACE_WIDTH_SHIFT        0
#define GEN7_SURFACE_WIDTH_MASK         INTEL_MASK(13, 0)

/* Surface state DW3 */
#define BRW_SURFACE_DEPTH_SHIFT		21
#define BRW_SURFACE_DEPTH_MASK		INTEL_MASK(31, 21)
#define BRW_SURFACE_PITCH_SHIFT		3
#define BRW_SURFACE_PITCH_MASK		INTEL_MASK(19, 3)
#define BRW_SURFACE_TILED		(1 << 1)
#define BRW_SURFACE_TILED_Y		(1 << 0)
#define HSW_SURFACE_IS_INTEGER_FORMAT   (1 << 18)

/* Surface state DW4 */
#define BRW_SURFACE_MIN_LOD_SHIFT	28
#define BRW_SURFACE_MIN_LOD_MASK	INTEL_MASK(31, 28)
#define BRW_SURFACE_MIN_ARRAY_ELEMENT_SHIFT	17
#define BRW_SURFACE_MIN_ARRAY_ELEMENT_MASK	INTEL_MASK(27, 17)
#define BRW_SURFACE_RENDER_TARGET_VIEW_EXTENT_SHIFT	8
#define BRW_SURFACE_RENDER_TARGET_VIEW_EXTENT_MASK	INTEL_MASK(16, 8)
#define BRW_SURFACE_MULTISAMPLECOUNT_1  (0 << 4)
#define BRW_SURFACE_MULTISAMPLECOUNT_4  (2 << 4)
#define GEN7_SURFACE_MULTISAMPLECOUNT_1         (0 << 3)
#define GEN8_SURFACE_MULTISAMPLECOUNT_2         (1 << 3)
#define GEN7_SURFACE_MULTISAMPLECOUNT_4         (2 << 3)
#define GEN7_SURFACE_MULTISAMPLECOUNT_8         (3 << 3)
#define GEN8_SURFACE_MULTISAMPLECOUNT_16        (4 << 3)
#define GEN7_SURFACE_MSFMT_MSS                  (0 << 6)
#define GEN7_SURFACE_MSFMT_DEPTH_STENCIL        (1 << 6)
#define GEN7_SURFACE_MIN_ARRAY_ELEMENT_SHIFT	18
#define GEN7_SURFACE_MIN_ARRAY_ELEMENT_MASK     INTEL_MASK(28, 18)
#define GEN7_SURFACE_RENDER_TARGET_VIEW_EXTENT_SHIFT	7
#define GEN7_SURFACE_RENDER_TARGET_VIEW_EXTENT_MASK   INTEL_MASK(17, 7)

/* Surface state DW5 */
#define BRW_SURFACE_X_OFFSET_SHIFT		25
#define BRW_SURFACE_X_OFFSET_MASK		INTEL_MASK(31, 25)
#define BRW_SURFACE_VERTICAL_ALIGN_ENABLE	(1 << 24)
#define BRW_SURFACE_Y_OFFSET_SHIFT		20
#define BRW_SURFACE_Y_OFFSET_MASK		INTEL_MASK(23, 20)
#define GEN7_SURFACE_MIN_LOD_SHIFT              4
#define GEN7_SURFACE_MIN_LOD_MASK               INTEL_MASK(7, 4)
#define GEN8_SURFACE_Y_OFFSET_SHIFT		21
#define GEN8_SURFACE_Y_OFFSET_MASK		INTEL_MASK(23, 21)

#define GEN7_SURFACE_MOCS_SHIFT                 16
#define GEN7_SURFACE_MOCS_MASK                  INTEL_MASK(19, 16)

/* Surface state DW6 */
#define GEN7_SURFACE_MCS_ENABLE                 (1 << 0)
#define GEN7_SURFACE_MCS_PITCH_SHIFT            3
#define GEN7_SURFACE_MCS_PITCH_MASK             INTEL_MASK(11, 3)
#define GEN8_SURFACE_AUX_QPITCH_SHIFT           16
#define GEN8_SURFACE_AUX_QPITCH_MASK            INTEL_MASK(30, 16)
#define GEN8_SURFACE_AUX_PITCH_SHIFT            3
#define GEN8_SURFACE_AUX_PITCH_MASK             INTEL_MASK(11, 3)
#define GEN8_SURFACE_AUX_MODE_MASK              INTEL_MASK(2, 0)

#define GEN8_SURFACE_AUX_MODE_NONE              0
#define GEN8_SURFACE_AUX_MODE_MCS               1
#define GEN8_SURFACE_AUX_MODE_APPEND            2
#define GEN8_SURFACE_AUX_MODE_HIZ               3

/* Surface state DW7 */
#define GEN9_SURFACE_RT_COMPRESSION_SHIFT       30
#define GEN9_SURFACE_RT_COMPRESSION_MASK        INTEL_MASK(30, 30)
#define GEN7_SURFACE_CLEAR_COLOR_SHIFT		28
#define GEN7_SURFACE_SCS_R_SHIFT                25
#define GEN7_SURFACE_SCS_R_MASK                 INTEL_MASK(27, 25)
#define GEN7_SURFACE_SCS_G_SHIFT                22
#define GEN7_SURFACE_SCS_G_MASK                 INTEL_MASK(24, 22)
#define GEN7_SURFACE_SCS_B_SHIFT                19
#define GEN7_SURFACE_SCS_B_MASK                 INTEL_MASK(21, 19)
#define GEN7_SURFACE_SCS_A_SHIFT                16
#define GEN7_SURFACE_SCS_A_MASK                 INTEL_MASK(18, 16)

/* The actual swizzle values/what channel to use */
#define HSW_SCS_ZERO                     0
#define HSW_SCS_ONE                      1
#define HSW_SCS_RED                      4
#define HSW_SCS_GREEN                    5
#define HSW_SCS_BLUE                     6
#define HSW_SCS_ALPHA                    7

/* SAMPLER_STATE DW0 */
#define BRW_SAMPLER_DISABLE                     (1 << 31)
#define BRW_SAMPLER_LOD_PRECLAMP_ENABLE         (1 << 28)
#define GEN6_SAMPLER_MIN_MAG_NOT_EQUAL          (1 << 27) /* Gen6 only */
#define BRW_SAMPLER_BASE_MIPLEVEL_MASK          INTEL_MASK(26, 22)
#define BRW_SAMPLER_BASE_MIPLEVEL_SHIFT         22
#define BRW_SAMPLER_MIP_FILTER_MASK             INTEL_MASK(21, 20)
#define BRW_SAMPLER_MIP_FILTER_SHIFT            20
#define BRW_SAMPLER_MAG_FILTER_MASK             INTEL_MASK(19, 17)
#define BRW_SAMPLER_MAG_FILTER_SHIFT            17
#define BRW_SAMPLER_MIN_FILTER_MASK             INTEL_MASK(16, 14)
#define BRW_SAMPLER_MIN_FILTER_SHIFT            14
#define GEN4_SAMPLER_LOD_BIAS_MASK              INTEL_MASK(13, 3)
#define GEN4_SAMPLER_LOD_BIAS_SHIFT             3
#define GEN4_SAMPLER_SHADOW_FUNCTION_MASK       INTEL_MASK(2, 0)
#define GEN4_SAMPLER_SHADOW_FUNCTION_SHIFT      0

#define GEN7_SAMPLER_LOD_BIAS_MASK              INTEL_MASK(13, 1)
#define GEN7_SAMPLER_LOD_BIAS_SHIFT             1
#define GEN7_SAMPLER_EWA_ANISOTROPIC_ALGORITHM  (1 << 0)

/* SAMPLER_STATE DW1 */
#define GEN4_SAMPLER_MIN_LOD_MASK               INTEL_MASK(31, 22)
#define GEN4_SAMPLER_MIN_LOD_SHIFT              22
#define GEN4_SAMPLER_MAX_LOD_MASK               INTEL_MASK(21, 12)
#define GEN4_SAMPLER_MAX_LOD_SHIFT              12
#define GEN4_SAMPLER_CUBE_CONTROL_OVERRIDE      (1 << 9)
/* Wrap modes are in DW1 on Gen4-6 and DW3 on Gen7+ */
#define BRW_SAMPLER_TCX_WRAP_MODE_MASK          INTEL_MASK(8, 6)
#define BRW_SAMPLER_TCX_WRAP_MODE_SHIFT         6
#define BRW_SAMPLER_TCY_WRAP_MODE_MASK          INTEL_MASK(5, 3)
#define BRW_SAMPLER_TCY_WRAP_MODE_SHIFT         3
#define BRW_SAMPLER_TCZ_WRAP_MODE_MASK          INTEL_MASK(2, 0)
#define BRW_SAMPLER_TCZ_WRAP_MODE_SHIFT         0

#define GEN7_SAMPLER_MIN_LOD_MASK               INTEL_MASK(31, 20)
#define GEN7_SAMPLER_MIN_LOD_SHIFT              20
#define GEN7_SAMPLER_MAX_LOD_MASK               INTEL_MASK(19, 8)
#define GEN7_SAMPLER_MAX_LOD_SHIFT              8
#define GEN7_SAMPLER_SHADOW_FUNCTION_MASK       INTEL_MASK(3, 1)
#define GEN7_SAMPLER_SHADOW_FUNCTION_SHIFT      1
#define GEN7_SAMPLER_CUBE_CONTROL_OVERRIDE      (1 << 0)

/* SAMPLER_STATE DW2 - border color pointer */

/* SAMPLER_STATE DW3 */
#define BRW_SAMPLER_MAX_ANISOTROPY_MASK         INTEL_MASK(21, 19)
#define BRW_SAMPLER_MAX_ANISOTROPY_SHIFT        19
#define BRW_SAMPLER_ADDRESS_ROUNDING_MASK       INTEL_MASK(18, 13)
#define BRW_SAMPLER_ADDRESS_ROUNDING_SHIFT      13
#define GEN7_SAMPLER_NON_NORMALIZED_COORDINATES (1 << 10)
/* Gen7+ wrap modes reuse the same BRW_SAMPLER_TC*_WRAP_MODE enums. */
#define GEN6_SAMPLER_NON_NORMALIZED_COORDINATES (1 << 0)

enum brw_wrap_mode {
   BRW_TEXCOORDMODE_WRAP         = 0,
   BRW_TEXCOORDMODE_MIRROR       = 1,
   BRW_TEXCOORDMODE_CLAMP        = 2,
   BRW_TEXCOORDMODE_CUBE         = 3,
   BRW_TEXCOORDMODE_CLAMP_BORDER = 4,
   BRW_TEXCOORDMODE_MIRROR_ONCE  = 5,
   GEN8_TEXCOORDMODE_HALF_BORDER = 6,
};

#define BRW_THREAD_PRIORITY_NORMAL   0
#define BRW_THREAD_PRIORITY_HIGH     1

#define BRW_TILEWALK_XMAJOR                 0
#define BRW_TILEWALK_YMAJOR                 1

#define BRW_VERTEX_SUBPIXEL_PRECISION_8BITS  0
#define BRW_VERTEX_SUBPIXEL_PRECISION_4BITS  1

/* Execution Unit (EU) defines
 */

#define BRW_ALIGN_1   0
#define BRW_ALIGN_16  1

#define BRW_ADDRESS_DIRECT                        0
#define BRW_ADDRESS_REGISTER_INDIRECT_REGISTER    1

#define BRW_CHANNEL_X     0
#define BRW_CHANNEL_Y     1
#define BRW_CHANNEL_Z     2
#define BRW_CHANNEL_W     3

enum brw_compression {
   BRW_COMPRESSION_NONE       = 0,
   BRW_COMPRESSION_2NDHALF    = 1,
   BRW_COMPRESSION_COMPRESSED = 2,
};

#define GEN6_COMPRESSION_1Q		0
#define GEN6_COMPRESSION_2Q		1
#define GEN6_COMPRESSION_3Q		2
#define GEN6_COMPRESSION_4Q		3
#define GEN6_COMPRESSION_1H		0
#define GEN6_COMPRESSION_2H		2

enum PACKED brw_conditional_mod {
   BRW_CONDITIONAL_NONE = 0,
   BRW_CONDITIONAL_Z    = 1,
   BRW_CONDITIONAL_NZ   = 2,
   BRW_CONDITIONAL_EQ   = 1,	/* Z */
   BRW_CONDITIONAL_NEQ  = 2,	/* NZ */
   BRW_CONDITIONAL_G    = 3,
   BRW_CONDITIONAL_GE   = 4,
   BRW_CONDITIONAL_L    = 5,
   BRW_CONDITIONAL_LE   = 6,
   BRW_CONDITIONAL_R    = 7,    /* Gen <= 5 */
   BRW_CONDITIONAL_O    = 8,
   BRW_CONDITIONAL_U    = 9,
};

#define BRW_DEBUG_NONE        0
#define BRW_DEBUG_BREAKPOINT  1

#define BRW_DEPENDENCY_NORMAL         0
#define BRW_DEPENDENCY_NOTCLEARED     1
#define BRW_DEPENDENCY_NOTCHECKED     2
#define BRW_DEPENDENCY_DISABLE        3

enum PACKED brw_execution_size {
   BRW_EXECUTE_1  = 0,
   BRW_EXECUTE_2  = 1,
   BRW_EXECUTE_4  = 2,
   BRW_EXECUTE_8  = 3,
   BRW_EXECUTE_16 = 4,
   BRW_EXECUTE_32 = 5,
};

enum PACKED brw_horizontal_stride {
   BRW_HORIZONTAL_STRIDE_0 = 0,
   BRW_HORIZONTAL_STRIDE_1 = 1,
   BRW_HORIZONTAL_STRIDE_2 = 2,
   BRW_HORIZONTAL_STRIDE_4 = 3,
};

#define BRW_INSTRUCTION_NORMAL    0
#define BRW_INSTRUCTION_SATURATE  1

#define BRW_MASK_ENABLE   0
#define BRW_MASK_DISABLE  1

/** @{
 *
 * Gen6 has replaced "mask enable/disable" with WECtrl, which is
 * effectively the same but much simpler to think about.  Now, there
 * are two contributors ANDed together to whether channels are
 * executed: The predication on the instruction, and the channel write
 * enable.
 */
/**
 * This is the default value.  It means that a channel's write enable is set
 * if the per-channel IP is pointing at this instruction.
 */
#define BRW_WE_NORMAL		0
/**
 * This is used like BRW_MASK_DISABLE, and causes all channels to have
 * their write enable set.  Note that predication still contributes to
 * whether the channel actually gets written.
 */
#define BRW_WE_ALL		1
/** @} */

enum opcode {
   /* These are the actual hardware opcodes. */
   BRW_OPCODE_MOV =	1,
   BRW_OPCODE_SEL =	2,
   BRW_OPCODE_NOT =	4,
   BRW_OPCODE_AND =	5,
   BRW_OPCODE_OR =	6,
   BRW_OPCODE_XOR =	7,
   BRW_OPCODE_SHR =	8,
   BRW_OPCODE_SHL =	9,
   BRW_OPCODE_ASR =	12,
   BRW_OPCODE_CMP =	16,
   BRW_OPCODE_CMPN =	17,
   BRW_OPCODE_CSEL =	18,  /**< Gen8+ */
   BRW_OPCODE_F32TO16 = 19,  /**< Gen7 only */
   BRW_OPCODE_F16TO32 = 20,  /**< Gen7 only */
   BRW_OPCODE_BFREV =	23,  /**< Gen7+ */
   BRW_OPCODE_BFE =	24,  /**< Gen7+ */
   BRW_OPCODE_BFI1 =	25,  /**< Gen7+ */
   BRW_OPCODE_BFI2 =	26,  /**< Gen7+ */
   BRW_OPCODE_JMPI =	32,
   BRW_OPCODE_IF =	34,
   BRW_OPCODE_IFF =	35,  /**< Pre-Gen6 */
   BRW_OPCODE_ELSE =	36,
   BRW_OPCODE_ENDIF =	37,
   BRW_OPCODE_DO =	38,
   BRW_OPCODE_WHILE =	39,
   BRW_OPCODE_BREAK =	40,
   BRW_OPCODE_CONTINUE = 41,
   BRW_OPCODE_HALT =	42,
   BRW_OPCODE_MSAVE =	44,  /**< Pre-Gen6 */
   BRW_OPCODE_MRESTORE = 45, /**< Pre-Gen6 */
   BRW_OPCODE_PUSH =	46,  /**< Pre-Gen6 */
   BRW_OPCODE_GOTO =	46,  /**< Gen8+    */
   BRW_OPCODE_POP =	47,  /**< Pre-Gen6 */
   BRW_OPCODE_WAIT =	48,
   BRW_OPCODE_SEND =	49,
   BRW_OPCODE_SENDC =	50,
   BRW_OPCODE_MATH =	56,  /**< Gen6+ */
   BRW_OPCODE_ADD =	64,
   BRW_OPCODE_MUL =	65,
   BRW_OPCODE_AVG =	66,
   BRW_OPCODE_FRC =	67,
   BRW_OPCODE_RNDU =	68,
   BRW_OPCODE_RNDD =	69,
   BRW_OPCODE_RNDE =	70,
   BRW_OPCODE_RNDZ =	71,
   BRW_OPCODE_MAC =	72,
   BRW_OPCODE_MACH =	73,
   BRW_OPCODE_LZD =	74,
   BRW_OPCODE_FBH =	75,  /**< Gen7+ */
   BRW_OPCODE_FBL =	76,  /**< Gen7+ */
   BRW_OPCODE_CBIT =	77,  /**< Gen7+ */
   BRW_OPCODE_ADDC =	78,  /**< Gen7+ */
   BRW_OPCODE_SUBB =	79,  /**< Gen7+ */
   BRW_OPCODE_SAD2 =	80,
   BRW_OPCODE_SADA2 =	81,
   BRW_OPCODE_DP4 =	84,
   BRW_OPCODE_DPH =	85,
   BRW_OPCODE_DP3 =	86,
   BRW_OPCODE_DP2 =	87,
   BRW_OPCODE_LINE =	89,
   BRW_OPCODE_PLN =	90,  /**< G45+ */
   BRW_OPCODE_MAD =	91,  /**< Gen6+ */
   BRW_OPCODE_LRP =	92,  /**< Gen6+ */
   BRW_OPCODE_NENOP =	125, /**< G45 only */
   BRW_OPCODE_NOP =	126,

   /* These are compiler backend opcodes that get translated into other
    * instructions.
    */
   FS_OPCODE_FB_WRITE = 128,
   FS_OPCODE_BLORP_FB_WRITE,
   FS_OPCODE_REP_FB_WRITE,
   SHADER_OPCODE_RCP,
   SHADER_OPCODE_RSQ,
   SHADER_OPCODE_SQRT,
   SHADER_OPCODE_EXP2,
   SHADER_OPCODE_LOG2,
   SHADER_OPCODE_POW,
   SHADER_OPCODE_INT_QUOTIENT,
   SHADER_OPCODE_INT_REMAINDER,
   SHADER_OPCODE_SIN,
   SHADER_OPCODE_COS,

   SHADER_OPCODE_TEX,
   SHADER_OPCODE_TXD,
   SHADER_OPCODE_TXF,
   SHADER_OPCODE_TXL,
   SHADER_OPCODE_TXS,
   FS_OPCODE_TXB,
   SHADER_OPCODE_TXF_CMS,
   SHADER_OPCODE_TXF_UMS,
   SHADER_OPCODE_TXF_MCS,
   SHADER_OPCODE_LOD,
   SHADER_OPCODE_TG4,
   SHADER_OPCODE_TG4_OFFSET,

   /**
    * Combines multiple sources of size 1 into a larger virtual GRF.
    * For example, parameters for a send-from-GRF message.  Or, updating
    * channels of a size 4 VGRF used to store vec4s such as texturing results.
    *
    * This will be lowered into MOVs from each source to consecutive reg_offsets
    * of the destination VGRF.
    *
    * src[0] may be BAD_FILE.  If so, the lowering pass skips emitting the MOV,
    * but still reserves the first channel of the destination VGRF.  This can be
    * used to reserve space for, say, a message header set up by the generators.
    */
   SHADER_OPCODE_LOAD_PAYLOAD,

   SHADER_OPCODE_SHADER_TIME_ADD,

   SHADER_OPCODE_UNTYPED_ATOMIC,
   SHADER_OPCODE_UNTYPED_SURFACE_READ,
   SHADER_OPCODE_UNTYPED_SURFACE_WRITE,

   SHADER_OPCODE_TYPED_ATOMIC,
   SHADER_OPCODE_TYPED_SURFACE_READ,
   SHADER_OPCODE_TYPED_SURFACE_WRITE,

   SHADER_OPCODE_MEMORY_FENCE,

   SHADER_OPCODE_GEN4_SCRATCH_READ,
   SHADER_OPCODE_GEN4_SCRATCH_WRITE,
   SHADER_OPCODE_GEN7_SCRATCH_READ,

   SHADER_OPCODE_URB_WRITE_SIMD8,

   /**
    * Return the index of an arbitrary live channel (i.e. one of the channels
    * enabled in the current execution mask) and assign it to the first
    * component of the destination.  Expected to be used as input for the
    * BROADCAST pseudo-opcode.
    */
   SHADER_OPCODE_FIND_LIVE_CHANNEL,

   /**
    * Pick the channel from its first source register given by the index
    * specified as second source.  Useful for variable indexing of surfaces.
    */
   SHADER_OPCODE_BROADCAST,

   VEC4_OPCODE_MOV_BYTES,
   VEC4_OPCODE_PACK_BYTES,
   VEC4_OPCODE_UNPACK_UNIFORM,

   FS_OPCODE_DDX_COARSE,
   FS_OPCODE_DDX_FINE,
   /**
    * Compute dFdy(), dFdyCoarse(), or dFdyFine().
    * src1 is an immediate storing the key->render_to_fbo boolean.
    */
   FS_OPCODE_DDY_COARSE,
   FS_OPCODE_DDY_FINE,
   FS_OPCODE_CINTERP,
   FS_OPCODE_LINTERP,
   FS_OPCODE_PIXEL_X,
   FS_OPCODE_PIXEL_Y,
   FS_OPCODE_UNIFORM_PULL_CONSTANT_LOAD,
   FS_OPCODE_UNIFORM_PULL_CONSTANT_LOAD_GEN7,
   FS_OPCODE_VARYING_PULL_CONSTANT_LOAD,
   FS_OPCODE_VARYING_PULL_CONSTANT_LOAD_GEN7,
   FS_OPCODE_MOV_DISPATCH_TO_FLAGS,
   FS_OPCODE_DISCARD_JUMP,
   FS_OPCODE_SET_OMASK,
   FS_OPCODE_SET_SAMPLE_ID,
   FS_OPCODE_SET_SIMD4X2_OFFSET,
   FS_OPCODE_PACK_HALF_2x16_SPLIT,
   FS_OPCODE_UNPACK_HALF_2x16_SPLIT_X,
   FS_OPCODE_UNPACK_HALF_2x16_SPLIT_Y,
   FS_OPCODE_PLACEHOLDER_HALT,
   FS_OPCODE_INTERPOLATE_AT_CENTROID,
   FS_OPCODE_INTERPOLATE_AT_SAMPLE,
   FS_OPCODE_INTERPOLATE_AT_SHARED_OFFSET,
   FS_OPCODE_INTERPOLATE_AT_PER_SLOT_OFFSET,

   VS_OPCODE_URB_WRITE,
   VS_OPCODE_PULL_CONSTANT_LOAD,
   VS_OPCODE_PULL_CONSTANT_LOAD_GEN7,
   VS_OPCODE_SET_SIMD4X2_HEADER_GEN9,
   VS_OPCODE_UNPACK_FLAGS_SIMD4X2,

   /**
    * Write geometry shader output data to the URB.
    *
    * Unlike VS_OPCODE_URB_WRITE, this opcode doesn't do an implied move from
    * R0 to the first MRF.  This allows the geometry shader to override the
    * "Slot {0,1} Offset" fields in the message header.
    */
   GS_OPCODE_URB_WRITE,

   /**
    * Write geometry shader output data to the URB and request a new URB
    * handle (gen6).
    *
    * This opcode doesn't do an implied move from R0 to the first MRF.
    */
   GS_OPCODE_URB_WRITE_ALLOCATE,

   /**
    * Terminate the geometry shader thread by doing an empty URB write.
    *
    * This opcode doesn't do an implied move from R0 to the first MRF.  This
    * allows the geometry shader to override the "GS Number of Output Vertices
    * for Slot {0,1}" fields in the message header.
    */
   GS_OPCODE_THREAD_END,

   /**
    * Set the "Slot {0,1} Offset" fields of a URB_WRITE message header.
    *
    * - dst is the MRF containing the message header.
    *
    * - src0.x indicates which portion of the URB should be written to (e.g. a
    *   vertex number)
    *
    * - src1 is an immediate multiplier which will be applied to src0
    *   (e.g. the size of a single vertex in the URB).
    *
    * Note: the hardware will apply this offset *in addition to* the offset in
    * vec4_instruction::offset.
    */
   GS_OPCODE_SET_WRITE_OFFSET,

   /**
    * Set the "GS Number of Output Vertices for Slot {0,1}" fields of a
    * URB_WRITE message header.
    *
    * - dst is the MRF containing the message header.
    *
    * - src0.x is the vertex count.  The upper 16 bits will be ignored.
    */
   GS_OPCODE_SET_VERTEX_COUNT,

   /**
    * Set DWORD 2 of dst to the value in src.
    */
   GS_OPCODE_SET_DWORD_2,

   /**
    * Prepare the dst register for storage in the "Channel Mask" fields of a
    * URB_WRITE message header.
    *
    * DWORD 4 of dst is shifted left by 4 bits, so that later,
    * GS_OPCODE_SET_CHANNEL_MASKS can OR DWORDs 0 and 4 together to form the
    * final channel mask.
    *
    * Note: since GS_OPCODE_SET_CHANNEL_MASKS ORs DWORDs 0 and 4 together to
    * form the final channel mask, DWORDs 0 and 4 of the dst register must not
    * have any extraneous bits set prior to execution of this opcode (that is,
    * they should be in the range 0x0 to 0xf).
    */
   GS_OPCODE_PREPARE_CHANNEL_MASKS,

   /**
    * Set the "Channel Mask" fields of a URB_WRITE message header.
    *
    * - dst is the MRF containing the message header.
    *
    * - src.x is the channel mask, as prepared by
    *   GS_OPCODE_PREPARE_CHANNEL_MASKS.  DWORDs 0 and 4 are OR'ed together to
    *   form the final channel mask.
    */
   GS_OPCODE_SET_CHANNEL_MASKS,

   /**
    * Get the "Instance ID" fields from the payload.
    *
    * - dst is the GRF for gl_InvocationID.
    */
   GS_OPCODE_GET_INSTANCE_ID,

   /**
    * Send a FF_SYNC message to allocate initial URB handles (gen6).
    *
    * - dst will be used as the writeback register for the FF_SYNC operation.
    *
    * - src0 is the number of primitives written.
    *
    * - src1 is the value to hold in M0.0: number of SO vertices to write
    *   and number of SO primitives needed. Its value will be overwritten
    *   with the SVBI values if transform feedback is enabled.
    *
    * Note: This opcode uses an implicit MRF register for the ff_sync message
    * header, so the caller is expected to set inst->base_mrf and initialize
    * that MRF register to r0. This opcode will also write to this MRF register
    * to include the allocated URB handle so it can then be reused directly as
    * the header in the URB write operation we are allocating the handle for.
    */
   GS_OPCODE_FF_SYNC,

   /**
    * Move r0.1 (which holds PrimitiveID information in gen6) to a separate
    * register.
    *
    * - dst is the GRF where PrimitiveID information will be moved.
    */
   GS_OPCODE_SET_PRIMITIVE_ID,

   /**
    * Write transform feedback data to the SVB by sending a SVB WRITE message.
    * Used in gen6.
    *
    * - dst is the MRF register containing the message header.
    *
    * - src0 is the register where the vertex data is going to be copied from.
    *
    * - src1 is the destination register when write commit occurs.
    */
   GS_OPCODE_SVB_WRITE,

   /**
    * Set destination index in the SVB write message payload (M0.5). Used
    * in gen6 for transform feedback.
    *
    * - dst is the header to save the destination indices for SVB WRITE.
    * - src is the register that holds the destination indices value.
    */
   GS_OPCODE_SVB_SET_DST_INDEX,

   /**
    * Prepare Mx.0 subregister for being used in the FF_SYNC message header.
    * Used in gen6 for transform feedback.
    *
    * - dst will hold the register with the final Mx.0 value.
    *
    * - src0 has the number of vertices emitted in SO (NumSOVertsToWrite)
    *
    * - src1 has the number of needed primitives for SO (NumSOPrimsNeeded)
    *
    * - src2 is the value to hold in M0: number of SO vertices to write
    *   and number of SO primitives needed.
    */
   GS_OPCODE_FF_SYNC_SET_PRIMITIVES,

   /**
    * Terminate the compute shader.
    */
   CS_OPCODE_CS_TERMINATE,
};

enum brw_urb_write_flags {
   BRW_URB_WRITE_NO_FLAGS = 0,

   /**
    * Causes a new URB entry to be allocated, and its address stored in the
    * destination register (gen < 7).
    */
   BRW_URB_WRITE_ALLOCATE = 0x1,

   /**
    * Causes the current URB entry to be deallocated (gen < 7).
    */
   BRW_URB_WRITE_UNUSED = 0x2,

   /**
    * Causes the thread to terminate.
    */
   BRW_URB_WRITE_EOT = 0x4,

   /**
    * Indicates that the given URB entry is complete, and may be sent further
    * down the 3D pipeline (gen < 7).
    */
   BRW_URB_WRITE_COMPLETE = 0x8,

   /**
    * Indicates that an additional offset (which may be different for the two
    * vec4 slots) is stored in the message header (gen == 7).
    */
   BRW_URB_WRITE_PER_SLOT_OFFSET = 0x10,

   /**
    * Indicates that the channel masks in the URB_WRITE message header should
    * not be overridden to 0xff (gen == 7).
    */
   BRW_URB_WRITE_USE_CHANNEL_MASKS = 0x20,

   /**
    * Indicates that the data should be sent to the URB using the
    * URB_WRITE_OWORD message rather than URB_WRITE_HWORD (gen == 7).  This
    * causes offsets to be interpreted as multiples of an OWORD instead of an
    * HWORD, and only allows one OWORD to be written.
    */
   BRW_URB_WRITE_OWORD = 0x40,

   /**
    * Convenient combination of flags: end the thread while simultaneously
    * marking the given URB entry as complete.
    */
   BRW_URB_WRITE_EOT_COMPLETE = BRW_URB_WRITE_EOT | BRW_URB_WRITE_COMPLETE,

   /**
    * Convenient combination of flags: mark the given URB entry as complete
    * and simultaneously allocate a new one.
    */
   BRW_URB_WRITE_ALLOCATE_COMPLETE =
      BRW_URB_WRITE_ALLOCATE | BRW_URB_WRITE_COMPLETE,
};

#ifdef __cplusplus
/**
 * Allow brw_urb_write_flags enums to be ORed together.
 */
inline brw_urb_write_flags
operator|(brw_urb_write_flags x, brw_urb_write_flags y)
{
   return static_cast<brw_urb_write_flags>(static_cast<int>(x) |
                                           static_cast<int>(y));
}
#endif

enum PACKED brw_predicate {
   BRW_PREDICATE_NONE                =  0,
   BRW_PREDICATE_NORMAL              =  1,
   BRW_PREDICATE_ALIGN1_ANYV         =  2,
   BRW_PREDICATE_ALIGN1_ALLV         =  3,
   BRW_PREDICATE_ALIGN1_ANY2H        =  4,
   BRW_PREDICATE_ALIGN1_ALL2H        =  5,
   BRW_PREDICATE_ALIGN1_ANY4H        =  6,
   BRW_PREDICATE_ALIGN1_ALL4H        =  7,
   BRW_PREDICATE_ALIGN1_ANY8H        =  8,
   BRW_PREDICATE_ALIGN1_ALL8H        =  9,
   BRW_PREDICATE_ALIGN1_ANY16H       = 10,
   BRW_PREDICATE_ALIGN1_ALL16H       = 11,
   BRW_PREDICATE_ALIGN1_ANY32H       = 12,
   BRW_PREDICATE_ALIGN1_ALL32H       = 13,
   BRW_PREDICATE_ALIGN16_REPLICATE_X =  2,
   BRW_PREDICATE_ALIGN16_REPLICATE_Y =  3,
   BRW_PREDICATE_ALIGN16_REPLICATE_Z =  4,
   BRW_PREDICATE_ALIGN16_REPLICATE_W =  5,
   BRW_PREDICATE_ALIGN16_ANY4H       =  6,
   BRW_PREDICATE_ALIGN16_ALL4H       =  7,
};

#define BRW_ARCHITECTURE_REGISTER_FILE    0
#define BRW_GENERAL_REGISTER_FILE         1
#define BRW_MESSAGE_REGISTER_FILE         2
#define BRW_IMMEDIATE_VALUE               3

#define BRW_HW_REG_TYPE_UD  0
#define BRW_HW_REG_TYPE_D   1
#define BRW_HW_REG_TYPE_UW  2
#define BRW_HW_REG_TYPE_W   3
#define BRW_HW_REG_TYPE_F   7
#define GEN8_HW_REG_TYPE_UQ 8
#define GEN8_HW_REG_TYPE_Q  9

#define BRW_HW_REG_NON_IMM_TYPE_UB  4
#define BRW_HW_REG_NON_IMM_TYPE_B   5
#define GEN7_HW_REG_NON_IMM_TYPE_DF 6
#define GEN8_HW_REG_NON_IMM_TYPE_HF 10

#define BRW_HW_REG_IMM_TYPE_UV  4 /* Gen6+ packed unsigned immediate vector */
#define BRW_HW_REG_IMM_TYPE_VF  5 /* packed float immediate vector */
#define BRW_HW_REG_IMM_TYPE_V   6 /* packed int imm. vector; uword dest only */
#define GEN8_HW_REG_IMM_TYPE_DF 10
#define GEN8_HW_REG_IMM_TYPE_HF 11

/* SNB adds 3-src instructions (MAD and LRP) that only operate on floats, so
 * the types were implied. IVB adds BFE and BFI2 that operate on doublewords
 * and unsigned doublewords, so a new field is also available in the da3src
 * struct (part of struct brw_instruction.bits1 in brw_structs.h) to select
 * dst and shared-src types. The values are different from BRW_REGISTER_TYPE_*.
 */
#define BRW_3SRC_TYPE_F  0
#define BRW_3SRC_TYPE_D  1
#define BRW_3SRC_TYPE_UD 2
#define BRW_3SRC_TYPE_DF 3

#define BRW_ARF_NULL                  0x00
#define BRW_ARF_ADDRESS               0x10
#define BRW_ARF_ACCUMULATOR           0x20
#define BRW_ARF_FLAG                  0x30
#define BRW_ARF_MASK                  0x40
#define BRW_ARF_MASK_STACK            0x50
#define BRW_ARF_MASK_STACK_DEPTH      0x60
#define BRW_ARF_STATE                 0x70
#define BRW_ARF_CONTROL               0x80
#define BRW_ARF_NOTIFICATION_COUNT    0x90
#define BRW_ARF_IP                    0xA0
#define BRW_ARF_TDR                   0xB0
#define BRW_ARF_TIMESTAMP             0xC0

#define BRW_MRF_COMPR4			(1 << 7)

#define BRW_AMASK   0
#define BRW_IMASK   1
#define BRW_LMASK   2
#define BRW_CMASK   3



#define BRW_THREAD_NORMAL     0
#define BRW_THREAD_ATOMIC     1
#define BRW_THREAD_SWITCH     2

enum PACKED brw_vertical_stride {
   BRW_VERTICAL_STRIDE_0               = 0,
   BRW_VERTICAL_STRIDE_1               = 1,
   BRW_VERTICAL_STRIDE_2               = 2,
   BRW_VERTICAL_STRIDE_4               = 3,
   BRW_VERTICAL_STRIDE_8               = 4,
   BRW_VERTICAL_STRIDE_16              = 5,
   BRW_VERTICAL_STRIDE_32              = 6,
   BRW_VERTICAL_STRIDE_ONE_DIMENSIONAL = 0xF,
};

enum PACKED brw_width {
   BRW_WIDTH_1  = 0,
   BRW_WIDTH_2  = 1,
   BRW_WIDTH_4  = 2,
   BRW_WIDTH_8  = 3,
   BRW_WIDTH_16 = 4,
};

#define BRW_STATELESS_BUFFER_BOUNDARY_1K      0
#define BRW_STATELESS_BUFFER_BOUNDARY_2K      1
#define BRW_STATELESS_BUFFER_BOUNDARY_4K      2
#define BRW_STATELESS_BUFFER_BOUNDARY_8K      3
#define BRW_STATELESS_BUFFER_BOUNDARY_16K     4
#define BRW_STATELESS_BUFFER_BOUNDARY_32K     5
#define BRW_STATELESS_BUFFER_BOUNDARY_64K     6
#define BRW_STATELESS_BUFFER_BOUNDARY_128K    7
#define BRW_STATELESS_BUFFER_BOUNDARY_256K    8
#define BRW_STATELESS_BUFFER_BOUNDARY_512K    9
#define BRW_STATELESS_BUFFER_BOUNDARY_1M      10
#define BRW_STATELESS_BUFFER_BOUNDARY_2M      11

#define BRW_POLYGON_FACING_FRONT      0
#define BRW_POLYGON_FACING_BACK       1

/**
 * Message target: Shared Function ID for where to SEND a message.
 *
 * These are enumerated in the ISA reference under "send - Send Message".
 * In particular, see the following tables:
 * - G45 PRM, Volume 4, Table 14-15 "Message Descriptor Definition"
 * - Sandybridge PRM, Volume 4 Part 2, Table 8-16 "Extended Message Descriptor"
 * - Ivybridge PRM, Volume 1 Part 1, section 3.2.7 "GPE Function IDs"
 */
enum brw_message_target {
   BRW_SFID_NULL                     = 0,
   BRW_SFID_MATH                     = 1, /* Only valid on Gen4-5 */
   BRW_SFID_SAMPLER                  = 2,
   BRW_SFID_MESSAGE_GATEWAY          = 3,
   BRW_SFID_DATAPORT_READ            = 4,
   BRW_SFID_DATAPORT_WRITE           = 5,
   BRW_SFID_URB                      = 6,
   BRW_SFID_THREAD_SPAWNER           = 7,
   BRW_SFID_VME                      = 8,

   GEN6_SFID_DATAPORT_SAMPLER_CACHE  = 4,
   GEN6_SFID_DATAPORT_RENDER_CACHE   = 5,
   GEN6_SFID_DATAPORT_CONSTANT_CACHE = 9,

   GEN7_SFID_DATAPORT_DATA_CACHE     = 10,
   GEN7_SFID_PIXEL_INTERPOLATOR      = 11,
   HSW_SFID_DATAPORT_DATA_CACHE_1    = 12,
   HSW_SFID_CRE                      = 13,
};

#define GEN7_MESSAGE_TARGET_DP_DATA_CACHE     10

#define BRW_SAMPLER_RETURN_FORMAT_FLOAT32     0
#define BRW_SAMPLER_RETURN_FORMAT_UINT32      2
#define BRW_SAMPLER_RETURN_FORMAT_SINT32      3

#define BRW_SAMPLER_MESSAGE_SIMD8_SAMPLE              0
#define BRW_SAMPLER_MESSAGE_SIMD16_SAMPLE             0
#define BRW_SAMPLER_MESSAGE_SIMD16_SAMPLE_BIAS        0
#define BRW_SAMPLER_MESSAGE_SIMD8_KILLPIX             1
#define BRW_SAMPLER_MESSAGE_SIMD4X2_SAMPLE_LOD        1
#define BRW_SAMPLER_MESSAGE_SIMD16_SAMPLE_LOD         1
#define BRW_SAMPLER_MESSAGE_SIMD4X2_SAMPLE_GRADIENTS  2
#define BRW_SAMPLER_MESSAGE_SIMD8_SAMPLE_GRADIENTS    2
#define BRW_SAMPLER_MESSAGE_SIMD4X2_SAMPLE_COMPARE    0
#define BRW_SAMPLER_MESSAGE_SIMD16_SAMPLE_COMPARE     2
#define BRW_SAMPLER_MESSAGE_SIMD8_SAMPLE_BIAS_COMPARE 0
#define BRW_SAMPLER_MESSAGE_SIMD4X2_SAMPLE_LOD_COMPARE 1
#define BRW_SAMPLER_MESSAGE_SIMD8_SAMPLE_LOD_COMPARE  1
#define BRW_SAMPLER_MESSAGE_SIMD4X2_RESINFO           2
#define BRW_SAMPLER_MESSAGE_SIMD16_RESINFO            2
#define BRW_SAMPLER_MESSAGE_SIMD4X2_LD                3
#define BRW_SAMPLER_MESSAGE_SIMD8_LD                  3
#define BRW_SAMPLER_MESSAGE_SIMD16_LD                 3

#define GEN5_SAMPLER_MESSAGE_SAMPLE              0
#define GEN5_SAMPLER_MESSAGE_SAMPLE_BIAS         1
#define GEN5_SAMPLER_MESSAGE_SAMPLE_LOD          2
#define GEN5_SAMPLER_MESSAGE_SAMPLE_COMPARE      3
#define GEN5_SAMPLER_MESSAGE_SAMPLE_DERIVS       4
#define GEN5_SAMPLER_MESSAGE_SAMPLE_BIAS_COMPARE 5
#define GEN5_SAMPLER_MESSAGE_SAMPLE_LOD_COMPARE  6
#define GEN5_SAMPLER_MESSAGE_SAMPLE_LD           7
#define GEN7_SAMPLER_MESSAGE_SAMPLE_GATHER4      8
#define GEN5_SAMPLER_MESSAGE_LOD                 9
#define GEN5_SAMPLER_MESSAGE_SAMPLE_RESINFO      10
#define GEN7_SAMPLER_MESSAGE_SAMPLE_GATHER4_C    16
#define GEN7_SAMPLER_MESSAGE_SAMPLE_GATHER4_PO   17
#define GEN7_SAMPLER_MESSAGE_SAMPLE_GATHER4_PO_C 18
#define HSW_SAMPLER_MESSAGE_SAMPLE_DERIV_COMPARE 20
#define GEN7_SAMPLER_MESSAGE_SAMPLE_LD_MCS       29
#define GEN7_SAMPLER_MESSAGE_SAMPLE_LD2DMS       30
#define GEN7_SAMPLER_MESSAGE_SAMPLE_LD2DSS       31

/* for GEN5 only */
#define BRW_SAMPLER_SIMD_MODE_SIMD4X2                   0
#define BRW_SAMPLER_SIMD_MODE_SIMD8                     1
#define BRW_SAMPLER_SIMD_MODE_SIMD16                    2
#define BRW_SAMPLER_SIMD_MODE_SIMD32_64                 3

/* GEN9 changes SIMD mode 0 to mean SIMD8D, but lets us get the SIMD4x2
 * behavior by setting bit 22 of dword 2 in the message header. */
#define GEN9_SAMPLER_SIMD_MODE_SIMD8D                   0
#define GEN9_SAMPLER_SIMD_MODE_EXTENSION_SIMD4X2        (1 << 22)

#define BRW_DATAPORT_OWORD_BLOCK_1_OWORDLOW   0
#define BRW_DATAPORT_OWORD_BLOCK_1_OWORDHIGH  1
#define BRW_DATAPORT_OWORD_BLOCK_2_OWORDS     2
#define BRW_DATAPORT_OWORD_BLOCK_4_OWORDS     3
#define BRW_DATAPORT_OWORD_BLOCK_8_OWORDS     4

#define BRW_DATAPORT_OWORD_DUAL_BLOCK_1OWORD     0
#define BRW_DATAPORT_OWORD_DUAL_BLOCK_4OWORDS    2

#define BRW_DATAPORT_DWORD_SCATTERED_BLOCK_8DWORDS   2
#define BRW_DATAPORT_DWORD_SCATTERED_BLOCK_16DWORDS  3

/* This one stays the same across generations. */
#define BRW_DATAPORT_READ_MESSAGE_OWORD_BLOCK_READ          0
/* GEN4 */
#define BRW_DATAPORT_READ_MESSAGE_OWORD_DUAL_BLOCK_READ     1
#define BRW_DATAPORT_READ_MESSAGE_MEDIA_BLOCK_READ          2
#define BRW_DATAPORT_READ_MESSAGE_DWORD_SCATTERED_READ      3
/* G45, GEN5 */
#define G45_DATAPORT_READ_MESSAGE_RENDER_UNORM_READ	    1
#define G45_DATAPORT_READ_MESSAGE_OWORD_DUAL_BLOCK_READ     2
#define G45_DATAPORT_READ_MESSAGE_AVC_LOOP_FILTER_READ	    3
#define G45_DATAPORT_READ_MESSAGE_MEDIA_BLOCK_READ          4
#define G45_DATAPORT_READ_MESSAGE_DWORD_SCATTERED_READ      6
/* GEN6 */
#define GEN6_DATAPORT_READ_MESSAGE_RENDER_UNORM_READ	    1
#define GEN6_DATAPORT_READ_MESSAGE_OWORD_DUAL_BLOCK_READ     2
#define GEN6_DATAPORT_READ_MESSAGE_MEDIA_BLOCK_READ          4
#define GEN6_DATAPORT_READ_MESSAGE_OWORD_UNALIGN_BLOCK_READ  5
#define GEN6_DATAPORT_READ_MESSAGE_DWORD_SCATTERED_READ      6

#define BRW_DATAPORT_READ_TARGET_DATA_CACHE      0
#define BRW_DATAPORT_READ_TARGET_RENDER_CACHE    1
#define BRW_DATAPORT_READ_TARGET_SAMPLER_CACHE   2

#define BRW_DATAPORT_RENDER_TARGET_WRITE_SIMD16_SINGLE_SOURCE                0
#define BRW_DATAPORT_RENDER_TARGET_WRITE_SIMD16_SINGLE_SOURCE_REPLICATED     1
#define BRW_DATAPORT_RENDER_TARGET_WRITE_SIMD8_DUAL_SOURCE_SUBSPAN01         2
#define BRW_DATAPORT_RENDER_TARGET_WRITE_SIMD8_DUAL_SOURCE_SUBSPAN23         3
#define BRW_DATAPORT_RENDER_TARGET_WRITE_SIMD8_SINGLE_SOURCE_SUBSPAN01       4

#define BRW_DATAPORT_WRITE_MESSAGE_OWORD_BLOCK_WRITE                0
#define BRW_DATAPORT_WRITE_MESSAGE_OWORD_DUAL_BLOCK_WRITE           1
#define BRW_DATAPORT_WRITE_MESSAGE_MEDIA_BLOCK_WRITE                2
#define BRW_DATAPORT_WRITE_MESSAGE_DWORD_SCATTERED_WRITE            3
#define BRW_DATAPORT_WRITE_MESSAGE_RENDER_TARGET_WRITE              4
#define BRW_DATAPORT_WRITE_MESSAGE_STREAMED_VERTEX_BUFFER_WRITE     5
#define BRW_DATAPORT_WRITE_MESSAGE_FLUSH_RENDER_CACHE               7

/* GEN6 */
#define GEN6_DATAPORT_WRITE_MESSAGE_DWORD_ATOMIC_WRITE              7
#define GEN6_DATAPORT_WRITE_MESSAGE_OWORD_BLOCK_WRITE               8
#define GEN6_DATAPORT_WRITE_MESSAGE_OWORD_DUAL_BLOCK_WRITE          9
#define GEN6_DATAPORT_WRITE_MESSAGE_MEDIA_BLOCK_WRITE               10
#define GEN6_DATAPORT_WRITE_MESSAGE_DWORD_SCATTERED_WRITE           11
#define GEN6_DATAPORT_WRITE_MESSAGE_RENDER_TARGET_WRITE             12
#define GEN6_DATAPORT_WRITE_MESSAGE_STREAMED_VB_WRITE               13
#define GEN6_DATAPORT_WRITE_MESSAGE_RENDER_TARGET_UNORM_WRITE       14

/* GEN7 */
#define GEN7_DATAPORT_RC_MEDIA_BLOCK_READ                           4
#define GEN7_DATAPORT_RC_TYPED_SURFACE_READ                         5
#define GEN7_DATAPORT_RC_TYPED_ATOMIC_OP                            6
#define GEN7_DATAPORT_RC_MEMORY_FENCE                               7
#define GEN7_DATAPORT_RC_MEDIA_BLOCK_WRITE                          10
#define GEN7_DATAPORT_RC_RENDER_TARGET_WRITE                        12
#define GEN7_DATAPORT_RC_TYPED_SURFACE_WRITE                        13
#define GEN7_DATAPORT_DC_OWORD_BLOCK_READ                           0
#define GEN7_DATAPORT_DC_UNALIGNED_OWORD_BLOCK_READ                 1
#define GEN7_DATAPORT_DC_OWORD_DUAL_BLOCK_READ                      2
#define GEN7_DATAPORT_DC_DWORD_SCATTERED_READ                       3
#define GEN7_DATAPORT_DC_BYTE_SCATTERED_READ                        4
#define GEN7_DATAPORT_DC_UNTYPED_SURFACE_READ                       5
#define GEN7_DATAPORT_DC_UNTYPED_ATOMIC_OP                          6
#define GEN7_DATAPORT_DC_MEMORY_FENCE                               7
#define GEN7_DATAPORT_DC_OWORD_BLOCK_WRITE                          8
#define GEN7_DATAPORT_DC_OWORD_DUAL_BLOCK_WRITE                     10
#define GEN7_DATAPORT_DC_DWORD_SCATTERED_WRITE                      11
#define GEN7_DATAPORT_DC_BYTE_SCATTERED_WRITE                       12
#define GEN7_DATAPORT_DC_UNTYPED_SURFACE_WRITE                      13

#define GEN7_DATAPORT_SCRATCH_READ                            ((1 << 18) | \
                                                               (0 << 17))
#define GEN7_DATAPORT_SCRATCH_WRITE                           ((1 << 18) | \
                                                               (1 << 17))
#define GEN7_DATAPORT_SCRATCH_NUM_REGS_SHIFT                        12

#define GEN7_PIXEL_INTERPOLATOR_LOC_SHARED_OFFSET     0
#define GEN7_PIXEL_INTERPOLATOR_LOC_SAMPLE            1
#define GEN7_PIXEL_INTERPOLATOR_LOC_CENTROID          2
#define GEN7_PIXEL_INTERPOLATOR_LOC_PER_SLOT_OFFSET   3

/* HSW */
#define HSW_DATAPORT_DC_PORT0_OWORD_BLOCK_READ                      0
#define HSW_DATAPORT_DC_PORT0_UNALIGNED_OWORD_BLOCK_READ            1
#define HSW_DATAPORT_DC_PORT0_OWORD_DUAL_BLOCK_READ                 2
#define HSW_DATAPORT_DC_PORT0_DWORD_SCATTERED_READ                  3
#define HSW_DATAPORT_DC_PORT0_BYTE_SCATTERED_READ                   4
#define HSW_DATAPORT_DC_PORT0_MEMORY_FENCE                          7
#define HSW_DATAPORT_DC_PORT0_OWORD_BLOCK_WRITE                     8
#define HSW_DATAPORT_DC_PORT0_OWORD_DUAL_BLOCK_WRITE                10
#define HSW_DATAPORT_DC_PORT0_DWORD_SCATTERED_WRITE                 11
#define HSW_DATAPORT_DC_PORT0_BYTE_SCATTERED_WRITE                  12

#define HSW_DATAPORT_DC_PORT1_UNTYPED_SURFACE_READ                  1
#define HSW_DATAPORT_DC_PORT1_UNTYPED_ATOMIC_OP                     2
#define HSW_DATAPORT_DC_PORT1_UNTYPED_ATOMIC_OP_SIMD4X2             3
#define HSW_DATAPORT_DC_PORT1_MEDIA_BLOCK_READ                      4
#define HSW_DATAPORT_DC_PORT1_TYPED_SURFACE_READ                    5
#define HSW_DATAPORT_DC_PORT1_TYPED_ATOMIC_OP                       6
#define HSW_DATAPORT_DC_PORT1_TYPED_ATOMIC_OP_SIMD4X2               7
#define HSW_DATAPORT_DC_PORT1_UNTYPED_SURFACE_WRITE                 9
#define HSW_DATAPORT_DC_PORT1_MEDIA_BLOCK_WRITE                     10
#define HSW_DATAPORT_DC_PORT1_ATOMIC_COUNTER_OP                     11
#define HSW_DATAPORT_DC_PORT1_ATOMIC_COUNTER_OP_SIMD4X2             12
#define HSW_DATAPORT_DC_PORT1_TYPED_SURFACE_WRITE                   13

/* dataport atomic operations. */
#define BRW_AOP_AND                   1
#define BRW_AOP_OR                    2
#define BRW_AOP_XOR                   3
#define BRW_AOP_MOV                   4
#define BRW_AOP_INC                   5
#define BRW_AOP_DEC                   6
#define BRW_AOP_ADD                   7
#define BRW_AOP_SUB                   8
#define BRW_AOP_REVSUB                9
#define BRW_AOP_IMAX                  10
#define BRW_AOP_IMIN                  11
#define BRW_AOP_UMAX                  12
#define BRW_AOP_UMIN                  13
#define BRW_AOP_CMPWR                 14
#define BRW_AOP_PREDEC                15

#define BRW_MATH_FUNCTION_INV                              1
#define BRW_MATH_FUNCTION_LOG                              2
#define BRW_MATH_FUNCTION_EXP                              3
#define BRW_MATH_FUNCTION_SQRT                             4
#define BRW_MATH_FUNCTION_RSQ                              5
#define BRW_MATH_FUNCTION_SIN                              6
#define BRW_MATH_FUNCTION_COS                              7
#define BRW_MATH_FUNCTION_SINCOS                           8 /* gen4, gen5 */
#define BRW_MATH_FUNCTION_FDIV                             9 /* gen6+ */
#define BRW_MATH_FUNCTION_POW                              10
#define BRW_MATH_FUNCTION_INT_DIV_QUOTIENT_AND_REMAINDER   11
#define BRW_MATH_FUNCTION_INT_DIV_QUOTIENT                 12
#define BRW_MATH_FUNCTION_INT_DIV_REMAINDER                13
#define GEN8_MATH_FUNCTION_INVM                            14
#define GEN8_MATH_FUNCTION_RSQRTM                          15

#define BRW_MATH_INTEGER_UNSIGNED     0
#define BRW_MATH_INTEGER_SIGNED       1

#define BRW_MATH_PRECISION_FULL        0
#define BRW_MATH_PRECISION_PARTIAL     1

#define BRW_MATH_SATURATE_NONE         0
#define BRW_MATH_SATURATE_SATURATE     1

#define BRW_MATH_DATA_VECTOR  0
#define BRW_MATH_DATA_SCALAR  1

#define BRW_URB_OPCODE_WRITE_HWORD  0
#define BRW_URB_OPCODE_WRITE_OWORD  1
#define GEN8_URB_OPCODE_SIMD8_WRITE  7

#define BRW_URB_SWIZZLE_NONE          0
#define BRW_URB_SWIZZLE_INTERLEAVE    1
#define BRW_URB_SWIZZLE_TRANSPOSE     2

#define BRW_SCRATCH_SPACE_SIZE_1K     0
#define BRW_SCRATCH_SPACE_SIZE_2K     1
#define BRW_SCRATCH_SPACE_SIZE_4K     2
#define BRW_SCRATCH_SPACE_SIZE_8K     3
#define BRW_SCRATCH_SPACE_SIZE_16K    4
#define BRW_SCRATCH_SPACE_SIZE_32K    5
#define BRW_SCRATCH_SPACE_SIZE_64K    6
#define BRW_SCRATCH_SPACE_SIZE_128K   7
#define BRW_SCRATCH_SPACE_SIZE_256K   8
#define BRW_SCRATCH_SPACE_SIZE_512K   9
#define BRW_SCRATCH_SPACE_SIZE_1M     10
#define BRW_SCRATCH_SPACE_SIZE_2M     11


#define CMD_URB_FENCE                 0x6000
#define CMD_CS_URB_STATE              0x6001
#define CMD_CONST_BUFFER              0x6002

#define CMD_STATE_BASE_ADDRESS        0x6101
#define CMD_STATE_SIP                 0x6102
#define CMD_PIPELINE_SELECT_965       0x6104
#define CMD_PIPELINE_SELECT_GM45      0x6904

#define _3DSTATE_PIPELINED_POINTERS		0x7800
#define _3DSTATE_BINDING_TABLE_POINTERS		0x7801
# define GEN6_BINDING_TABLE_MODIFY_VS	(1 << 8)
# define GEN6_BINDING_TABLE_MODIFY_GS	(1 << 9)
# define GEN6_BINDING_TABLE_MODIFY_PS	(1 << 12)

#define _3DSTATE_BINDING_TABLE_POINTERS_VS	0x7826 /* GEN7+ */
#define _3DSTATE_BINDING_TABLE_POINTERS_HS	0x7827 /* GEN7+ */
#define _3DSTATE_BINDING_TABLE_POINTERS_DS	0x7828 /* GEN7+ */
#define _3DSTATE_BINDING_TABLE_POINTERS_GS	0x7829 /* GEN7+ */
#define _3DSTATE_BINDING_TABLE_POINTERS_PS	0x782A /* GEN7+ */

#define _3DSTATE_SAMPLER_STATE_POINTERS		0x7802 /* GEN6+ */
# define PS_SAMPLER_STATE_CHANGE				(1 << 12)
# define GS_SAMPLER_STATE_CHANGE				(1 << 9)
# define VS_SAMPLER_STATE_CHANGE				(1 << 8)
/* DW1: VS */
/* DW2: GS */
/* DW3: PS */

#define _3DSTATE_SAMPLER_STATE_POINTERS_VS	0x782B /* GEN7+ */
#define _3DSTATE_SAMPLER_STATE_POINTERS_GS	0x782E /* GEN7+ */
#define _3DSTATE_SAMPLER_STATE_POINTERS_PS	0x782F /* GEN7+ */

#define _3DSTATE_VERTEX_BUFFERS       0x7808
# define BRW_VB0_INDEX_SHIFT		27
# define GEN6_VB0_INDEX_SHIFT		26
# define BRW_VB0_ACCESS_VERTEXDATA	(0 << 26)
# define BRW_VB0_ACCESS_INSTANCEDATA	(1 << 26)
# define GEN6_VB0_ACCESS_VERTEXDATA	(0 << 20)
# define GEN6_VB0_ACCESS_INSTANCEDATA	(1 << 20)
# define GEN7_VB0_ADDRESS_MODIFYENABLE  (1 << 14)
# define BRW_VB0_PITCH_SHIFT		0

#define _3DSTATE_VERTEX_ELEMENTS      0x7809
# define BRW_VE0_INDEX_SHIFT		27
# define GEN6_VE0_INDEX_SHIFT		26
# define BRW_VE0_FORMAT_SHIFT		16
# define BRW_VE0_VALID			(1 << 26)
# define GEN6_VE0_VALID			(1 << 25)
# define GEN6_VE0_EDGE_FLAG_ENABLE	(1 << 15)
# define BRW_VE0_SRC_OFFSET_SHIFT	0
# define BRW_VE1_COMPONENT_NOSTORE	0
# define BRW_VE1_COMPONENT_STORE_SRC	1
# define BRW_VE1_COMPONENT_STORE_0	2
# define BRW_VE1_COMPONENT_STORE_1_FLT	3
# define BRW_VE1_COMPONENT_STORE_1_INT	4
# define BRW_VE1_COMPONENT_STORE_VID	5
# define BRW_VE1_COMPONENT_STORE_IID	6
# define BRW_VE1_COMPONENT_STORE_PID	7
# define BRW_VE1_COMPONENT_0_SHIFT	28
# define BRW_VE1_COMPONENT_1_SHIFT	24
# define BRW_VE1_COMPONENT_2_SHIFT	20
# define BRW_VE1_COMPONENT_3_SHIFT	16
# define BRW_VE1_DST_OFFSET_SHIFT	0

#define CMD_INDEX_BUFFER              0x780a
#define GEN4_3DSTATE_VF_STATISTICS		0x780b
#define GM45_3DSTATE_VF_STATISTICS		0x680b
#define _3DSTATE_CC_STATE_POINTERS		0x780e /* GEN6+ */
#define _3DSTATE_BLEND_STATE_POINTERS		0x7824 /* GEN7+ */
#define _3DSTATE_DEPTH_STENCIL_STATE_POINTERS	0x7825 /* GEN7+ */

#define _3DSTATE_URB				0x7805 /* GEN6 */
# define GEN6_URB_VS_SIZE_SHIFT				16
# define GEN6_URB_VS_ENTRIES_SHIFT			0
# define GEN6_URB_GS_ENTRIES_SHIFT			8
# define GEN6_URB_GS_SIZE_SHIFT				0

#define _3DSTATE_VF                             0x780c /* GEN7.5+ */
#define HSW_CUT_INDEX_ENABLE                            (1 << 8)

#define _3DSTATE_VF_INSTANCING                  0x7849 /* GEN8+ */
# define GEN8_VF_INSTANCING_ENABLE                      (1 << 8)

#define _3DSTATE_VF_SGVS                        0x784a /* GEN8+ */
# define GEN8_SGVS_ENABLE_INSTANCE_ID                   (1 << 31)
# define GEN8_SGVS_INSTANCE_ID_COMPONENT_SHIFT          29
# define GEN8_SGVS_INSTANCE_ID_ELEMENT_OFFSET_SHIFT     16
# define GEN8_SGVS_ENABLE_VERTEX_ID                     (1 << 15)
# define GEN8_SGVS_VERTEX_ID_COMPONENT_SHIFT            13
# define GEN8_SGVS_VERTEX_ID_ELEMENT_OFFSET_SHIFT       0

#define _3DSTATE_VF_TOPOLOGY                    0x784b /* GEN8+ */

#define _3DSTATE_WM_CHROMAKEY			0x784c /* GEN8+ */

#define _3DSTATE_URB_VS                         0x7830 /* GEN7+ */
#define _3DSTATE_URB_HS                         0x7831 /* GEN7+ */
#define _3DSTATE_URB_DS                         0x7832 /* GEN7+ */
#define _3DSTATE_URB_GS                         0x7833 /* GEN7+ */
# define GEN7_URB_ENTRY_SIZE_SHIFT                      16
# define GEN7_URB_STARTING_ADDRESS_SHIFT                25

/* Gen7 "GS URB Entry Allocation Size" is a U9-1 field, so the maximum gs_size
 * is 2^9, or 512.  It's counted in multiples of 64 bytes.
 */
#define GEN7_MAX_GS_URB_ENTRY_SIZE_BYTES                (512*64)
/* Gen6 "GS URB Entry Allocation Size" is defined as a number of 1024-bit
 * (128 bytes) URB rows and the maximum allowed value is 5 rows.
 */
#define GEN6_MAX_GS_URB_ENTRY_SIZE_BYTES                (5*128)

#define _3DSTATE_PUSH_CONSTANT_ALLOC_VS         0x7912 /* GEN7+ */
#define _3DSTATE_PUSH_CONSTANT_ALLOC_GS         0x7915 /* GEN7+ */
#define _3DSTATE_PUSH_CONSTANT_ALLOC_PS         0x7916 /* GEN7+ */
# define GEN7_PUSH_CONSTANT_BUFFER_OFFSET_SHIFT         16

#define _3DSTATE_VIEWPORT_STATE_POINTERS	0x780d /* GEN6+ */
# define GEN6_CC_VIEWPORT_MODIFY			(1 << 12)
# define GEN6_SF_VIEWPORT_MODIFY			(1 << 11)
# define GEN6_CLIP_VIEWPORT_MODIFY			(1 << 10)
# define GEN6_NUM_VIEWPORTS				16

#define _3DSTATE_VIEWPORT_STATE_POINTERS_CC	0x7823 /* GEN7+ */
#define _3DSTATE_VIEWPORT_STATE_POINTERS_SF_CL	0x7821 /* GEN7+ */

#define _3DSTATE_SCISSOR_STATE_POINTERS		0x780f /* GEN6+ */

#define _3DSTATE_VS				0x7810 /* GEN6+ */
/* DW2 */
# define GEN6_VS_SPF_MODE				(1 << 31)
# define GEN6_VS_VECTOR_MASK_ENABLE			(1 << 30)
# define GEN6_VS_SAMPLER_COUNT_SHIFT			27
# define GEN6_VS_BINDING_TABLE_ENTRY_COUNT_SHIFT	18
# define GEN6_VS_FLOATING_POINT_MODE_IEEE_754		(0 << 16)
# define GEN6_VS_FLOATING_POINT_MODE_ALT		(1 << 16)
/* DW4 */
# define GEN6_VS_DISPATCH_START_GRF_SHIFT		20
# define GEN6_VS_URB_READ_LENGTH_SHIFT			11
# define GEN6_VS_URB_ENTRY_READ_OFFSET_SHIFT		4
/* DW5 */
# define GEN6_VS_MAX_THREADS_SHIFT			25
# define HSW_VS_MAX_THREADS_SHIFT			23
# define GEN6_VS_STATISTICS_ENABLE			(1 << 10)
# define GEN6_VS_CACHE_DISABLE				(1 << 1)
# define GEN6_VS_ENABLE					(1 << 0)
/* Gen8+ DW7 */
# define GEN8_VS_SIMD8_ENABLE                           (1 << 2)
/* Gen8+ DW8 */
# define GEN8_VS_URB_ENTRY_OUTPUT_OFFSET_SHIFT          21
# define GEN8_VS_URB_OUTPUT_LENGTH_SHIFT                16
# define GEN8_VS_USER_CLIP_DISTANCE_SHIFT               8

#define _3DSTATE_GS		      		0x7811 /* GEN6+ */
/* DW2 */
# define GEN6_GS_SPF_MODE				(1 << 31)
# define GEN6_GS_VECTOR_MASK_ENABLE			(1 << 30)
# define GEN6_GS_SAMPLER_COUNT_SHIFT			27
# define GEN6_GS_BINDING_TABLE_ENTRY_COUNT_SHIFT	18
# define GEN6_GS_FLOATING_POINT_MODE_IEEE_754		(0 << 16)
# define GEN6_GS_FLOATING_POINT_MODE_ALT		(1 << 16)
/* DW4 */
# define GEN7_GS_OUTPUT_VERTEX_SIZE_SHIFT		23
# define GEN7_GS_OUTPUT_TOPOLOGY_SHIFT			17
# define GEN6_GS_URB_READ_LENGTH_SHIFT			11
# define GEN7_GS_INCLUDE_VERTEX_HANDLES		        (1 << 10)
# define GEN6_GS_URB_ENTRY_READ_OFFSET_SHIFT		4
# define GEN6_GS_DISPATCH_START_GRF_SHIFT		0
/* DW5 */
# define GEN6_GS_MAX_THREADS_SHIFT			25
# define HSW_GS_MAX_THREADS_SHIFT			24
# define IVB_GS_CONTROL_DATA_FORMAT_SHIFT		24
# define GEN7_GS_CONTROL_DATA_FORMAT_GSCTL_CUT		0
# define GEN7_GS_CONTROL_DATA_FORMAT_GSCTL_SID		1
# define GEN7_GS_CONTROL_DATA_HEADER_SIZE_SHIFT		20
# define GEN7_GS_INSTANCE_CONTROL_SHIFT			15
# define GEN7_GS_DISPATCH_MODE_SINGLE			(0 << 11)
# define GEN7_GS_DISPATCH_MODE_DUAL_INSTANCE		(1 << 11)
# define GEN7_GS_DISPATCH_MODE_DUAL_OBJECT		(2 << 11)
# define GEN6_GS_STATISTICS_ENABLE			(1 << 10)
# define GEN6_GS_SO_STATISTICS_ENABLE			(1 << 9)
# define GEN6_GS_RENDERING_ENABLE			(1 << 8)
# define GEN7_GS_INCLUDE_PRIMITIVE_ID			(1 << 4)
# define GEN7_GS_REORDER_TRAILING			(1 << 2)
# define GEN7_GS_ENABLE					(1 << 0)
/* DW6 */
# define HSW_GS_CONTROL_DATA_FORMAT_SHIFT		31
# define GEN6_GS_REORDER				(1 << 30)
# define GEN6_GS_DISCARD_ADJACENCY			(1 << 29)
# define GEN6_GS_SVBI_PAYLOAD_ENABLE			(1 << 28)
# define GEN6_GS_SVBI_POSTINCREMENT_ENABLE		(1 << 27)
# define GEN6_GS_SVBI_POSTINCREMENT_VALUE_SHIFT		16
# define GEN6_GS_SVBI_POSTINCREMENT_VALUE_MASK		INTEL_MASK(25, 16)
# define GEN6_GS_ENABLE					(1 << 15)

/* Gen8+ DW9 */
# define GEN8_GS_URB_ENTRY_OUTPUT_OFFSET_SHIFT          21
# define GEN8_GS_URB_OUTPUT_LENGTH_SHIFT                16
# define GEN8_GS_USER_CLIP_DISTANCE_SHIFT               8

# define BRW_GS_EDGE_INDICATOR_0			(1 << 8)
# define BRW_GS_EDGE_INDICATOR_1			(1 << 9)

/* GS Thread Payload
 */
/* R0 */
# define GEN7_GS_PAYLOAD_INSTANCE_ID_SHIFT		27

/* 3DSTATE_GS "Output Vertex Size" has an effective maximum of 62.  It's
 * counted in multiples of 16 bytes.
 */
#define GEN7_MAX_GS_OUTPUT_VERTEX_SIZE_BYTES		(62*16)

#define _3DSTATE_HS                             0x781B /* GEN7+ */
#define _3DSTATE_TE                             0x781C /* GEN7+ */
#define _3DSTATE_DS                             0x781D /* GEN7+ */

#define _3DSTATE_CLIP				0x7812 /* GEN6+ */
/* DW1 */
# define GEN7_CLIP_WINDING_CW                           (0 << 20)
# define GEN7_CLIP_WINDING_CCW                          (1 << 20)
# define GEN7_CLIP_VERTEX_SUBPIXEL_PRECISION_8          (0 << 19)
# define GEN7_CLIP_VERTEX_SUBPIXEL_PRECISION_4          (1 << 19)
# define GEN7_CLIP_EARLY_CULL                           (1 << 18)
# define GEN7_CLIP_CULLMODE_BOTH                        (0 << 16)
# define GEN7_CLIP_CULLMODE_NONE                        (1 << 16)
# define GEN7_CLIP_CULLMODE_FRONT                       (2 << 16)
# define GEN7_CLIP_CULLMODE_BACK                        (3 << 16)
# define GEN6_CLIP_STATISTICS_ENABLE			(1 << 10)
/**
 * Just does cheap culling based on the clip distance.  Bits must be
 * disjoint with USER_CLIP_CLIP_DISTANCE bits.
 */
# define GEN6_USER_CLIP_CULL_DISTANCES_SHIFT		0
/* DW2 */
# define GEN6_CLIP_ENABLE				(1 << 31)
# define GEN6_CLIP_API_OGL				(0 << 30)
# define GEN6_CLIP_API_D3D				(1 << 30)
# define GEN6_CLIP_XY_TEST				(1 << 28)
# define GEN6_CLIP_Z_TEST				(1 << 27)
# define GEN6_CLIP_GB_TEST				(1 << 26)
/** 8-bit field of which user clip distances to clip aganist. */
# define GEN6_USER_CLIP_CLIP_DISTANCES_SHIFT		16
# define GEN6_CLIP_MODE_NORMAL				(0 << 13)
# define GEN6_CLIP_MODE_REJECT_ALL			(3 << 13)
# define GEN6_CLIP_MODE_ACCEPT_ALL			(4 << 13)
# define GEN6_CLIP_PERSPECTIVE_DIVIDE_DISABLE		(1 << 9)
# define GEN6_CLIP_NON_PERSPECTIVE_BARYCENTRIC_ENABLE	(1 << 8)
# define GEN6_CLIP_TRI_PROVOKE_SHIFT			4
# define GEN6_CLIP_LINE_PROVOKE_SHIFT			2
# define GEN6_CLIP_TRIFAN_PROVOKE_SHIFT			0
/* DW3 */
# define GEN6_CLIP_MIN_POINT_WIDTH_SHIFT		17
# define GEN6_CLIP_MAX_POINT_WIDTH_SHIFT		6
# define GEN6_CLIP_FORCE_ZERO_RTAINDEX			(1 << 5)
# define GEN6_CLIP_MAX_VP_INDEX_MASK			INTEL_MASK(3, 0)

#define _3DSTATE_SF				0x7813 /* GEN6+ */
/* DW1 (for gen6) */
# define GEN6_SF_NUM_OUTPUTS_SHIFT			22
# define GEN6_SF_SWIZZLE_ENABLE				(1 << 21)
# define GEN6_SF_POINT_SPRITE_UPPERLEFT			(0 << 20)
# define GEN6_SF_POINT_SPRITE_LOWERLEFT			(1 << 20)
# define GEN9_SF_LINE_WIDTH_SHIFT			12 /* U11.7 */
# define GEN6_SF_URB_ENTRY_READ_LENGTH_SHIFT		11
# define GEN6_SF_URB_ENTRY_READ_OFFSET_SHIFT		4
/* DW2 */
# define GEN6_SF_LEGACY_GLOBAL_DEPTH_BIAS		(1 << 11)
# define GEN6_SF_STATISTICS_ENABLE			(1 << 10)
# define GEN6_SF_GLOBAL_DEPTH_OFFSET_SOLID		(1 << 9)
# define GEN6_SF_GLOBAL_DEPTH_OFFSET_WIREFRAME		(1 << 8)
# define GEN6_SF_GLOBAL_DEPTH_OFFSET_POINT		(1 << 7)
# define GEN6_SF_FRONT_SOLID				(0 << 5)
# define GEN6_SF_FRONT_WIREFRAME			(1 << 5)
# define GEN6_SF_FRONT_POINT				(2 << 5)
# define GEN6_SF_BACK_SOLID				(0 << 3)
# define GEN6_SF_BACK_WIREFRAME				(1 << 3)
# define GEN6_SF_BACK_POINT				(2 << 3)
# define GEN6_SF_VIEWPORT_TRANSFORM_ENABLE		(1 << 1)
# define GEN6_SF_WINDING_CCW				(1 << 0)
/* DW3 */
# define GEN6_SF_LINE_AA_ENABLE				(1 << 31)
# define GEN6_SF_CULL_BOTH				(0 << 29)
# define GEN6_SF_CULL_NONE				(1 << 29)
# define GEN6_SF_CULL_FRONT				(2 << 29)
# define GEN6_SF_CULL_BACK				(3 << 29)
# define GEN6_SF_LINE_WIDTH_SHIFT			18 /* U3.7 */
# define GEN6_SF_LINE_END_CAP_WIDTH_0_5			(0 << 16)
# define GEN6_SF_LINE_END_CAP_WIDTH_1_0			(1 << 16)
# define GEN6_SF_LINE_END_CAP_WIDTH_2_0			(2 << 16)
# define GEN6_SF_LINE_END_CAP_WIDTH_4_0			(3 << 16)
# define GEN6_SF_SCISSOR_ENABLE				(1 << 11)
# define GEN6_SF_MSRAST_OFF_PIXEL			(0 << 8)
# define GEN6_SF_MSRAST_OFF_PATTERN			(1 << 8)
# define GEN6_SF_MSRAST_ON_PIXEL			(2 << 8)
# define GEN6_SF_MSRAST_ON_PATTERN			(3 << 8)
/* DW4 */
# define GEN6_SF_TRI_PROVOKE_SHIFT			29
# define GEN6_SF_LINE_PROVOKE_SHIFT			27
# define GEN6_SF_TRIFAN_PROVOKE_SHIFT			25
# define GEN6_SF_LINE_AA_MODE_MANHATTAN			(0 << 14)
# define GEN6_SF_LINE_AA_MODE_TRUE			(1 << 14)
# define GEN6_SF_VERTEX_SUBPIXEL_8BITS			(0 << 12)
# define GEN6_SF_VERTEX_SUBPIXEL_4BITS			(1 << 12)
# define GEN6_SF_USE_STATE_POINT_WIDTH			(1 << 11)
# define GEN6_SF_POINT_WIDTH_SHIFT			0 /* U8.3 */
/* DW5: depth offset constant */
/* DW6: depth offset scale */
/* DW7: depth offset clamp */
/* DW8 */
# define ATTRIBUTE_1_OVERRIDE_W				(1 << 31)
# define ATTRIBUTE_1_OVERRIDE_Z				(1 << 30)
# define ATTRIBUTE_1_OVERRIDE_Y				(1 << 29)
# define ATTRIBUTE_1_OVERRIDE_X				(1 << 28)
# define ATTRIBUTE_1_CONST_SOURCE_SHIFT			25
# define ATTRIBUTE_1_SWIZZLE_SHIFT			22
# define ATTRIBUTE_1_SOURCE_SHIFT			16
# define ATTRIBUTE_0_OVERRIDE_W				(1 << 15)
# define ATTRIBUTE_0_OVERRIDE_Z				(1 << 14)
# define ATTRIBUTE_0_OVERRIDE_Y				(1 << 13)
# define ATTRIBUTE_0_OVERRIDE_X				(1 << 12)
# define ATTRIBUTE_0_CONST_SOURCE_SHIFT			9
#  define ATTRIBUTE_CONST_0000				0
#  define ATTRIBUTE_CONST_0001_FLOAT			1
#  define ATTRIBUTE_CONST_1111_FLOAT			2
#  define ATTRIBUTE_CONST_PRIM_ID			3
# define ATTRIBUTE_0_SWIZZLE_SHIFT			6
# define ATTRIBUTE_0_SOURCE_SHIFT			0

# define ATTRIBUTE_SWIZZLE_INPUTATTR                    0
# define ATTRIBUTE_SWIZZLE_INPUTATTR_FACING             1
# define ATTRIBUTE_SWIZZLE_INPUTATTR_W                  2
# define ATTRIBUTE_SWIZZLE_INPUTATTR_FACING_W           3
# define ATTRIBUTE_SWIZZLE_SHIFT                        6

/* DW16: Point sprite texture coordinate enables */
/* DW17: Constant interpolation enables */
/* DW18: attr 0-7 wrap shortest enables */
/* DW19: attr 8-16 wrap shortest enables */

/* On GEN7, many fields of 3DSTATE_SF were split out into a new command:
 * 3DSTATE_SBE.  The remaining fields live in different DWords, but retain
 * the same bit-offset.  The only new field:
 */
/* GEN7/DW1: */
# define GEN7_SF_DEPTH_BUFFER_SURFACE_FORMAT_SHIFT	12
/* GEN7/DW2: */
# define HSW_SF_LINE_STIPPLE_ENABLE			(1 << 14)

# define GEN8_SF_SMOOTH_POINT_ENABLE                    (1 << 13)

#define _3DSTATE_SBE				0x781F /* GEN7+ */
/* DW1 */
# define GEN8_SBE_FORCE_URB_ENTRY_READ_LENGTH           (1 << 29)
# define GEN8_SBE_FORCE_URB_ENTRY_READ_OFFSET           (1 << 28)
# define GEN7_SBE_SWIZZLE_CONTROL_MODE			(1 << 28)
# define GEN7_SBE_NUM_OUTPUTS_SHIFT			22
# define GEN7_SBE_SWIZZLE_ENABLE			(1 << 21)
# define GEN7_SBE_POINT_SPRITE_LOWERLEFT		(1 << 20)
# define GEN7_SBE_URB_ENTRY_READ_LENGTH_SHIFT		11
# define GEN7_SBE_URB_ENTRY_READ_OFFSET_SHIFT		4
# define GEN8_SBE_URB_ENTRY_READ_OFFSET_SHIFT		5
/* DW2-9: Attribute setup (same as DW8-15 of gen6 _3DSTATE_SF) */
/* DW10: Point sprite texture coordinate enables */
/* DW11: Constant interpolation enables */
/* DW12: attr 0-7 wrap shortest enables */
/* DW13: attr 8-16 wrap shortest enables */

/* DW4-5: Attribute active components (gen9) */
#define GEN9_SBE_ACTIVE_COMPONENT_NONE			0
#define GEN9_SBE_ACTIVE_COMPONENT_XY			1
#define GEN9_SBE_ACTIVE_COMPONENT_XYZ			2
#define GEN9_SBE_ACTIVE_COMPONENT_XYZW			3

#define _3DSTATE_SBE_SWIZ                       0x7851 /* GEN8+ */

#define _3DSTATE_RASTER                         0x7850 /* GEN8+ */
/* DW1 */
# define GEN9_RASTER_VIEWPORT_Z_FAR_CLIP_TEST_ENABLE    (1 << 26)
# define GEN8_RASTER_FRONT_WINDING_CCW                  (1 << 21)
# define GEN8_RASTER_CULL_BOTH                          (0 << 16)
# define GEN8_RASTER_CULL_NONE                          (1 << 16)
# define GEN8_RASTER_CULL_FRONT                         (2 << 16)
# define GEN8_RASTER_CULL_BACK                          (3 << 16)
# define GEN8_RASTER_SMOOTH_POINT_ENABLE                (1 << 13)
# define GEN8_RASTER_API_MULTISAMPLE_ENABLE             (1 << 12)
# define GEN8_RASTER_LINE_AA_ENABLE                     (1 << 2)
# define GEN8_RASTER_SCISSOR_ENABLE                     (1 << 1)
# define GEN8_RASTER_VIEWPORT_Z_CLIP_TEST_ENABLE        (1 << 0)
# define GEN9_RASTER_VIEWPORT_Z_NEAR_CLIP_TEST_ENABLE   (1 << 0)

/* Gen8 BLEND_STATE */
/* DW0 */
#define GEN8_BLEND_ALPHA_TO_COVERAGE_ENABLE             (1 << 31)
#define GEN8_BLEND_INDEPENDENT_ALPHA_BLEND_ENABLE       (1 << 30)
#define GEN8_BLEND_ALPHA_TO_ONE_ENABLE                  (1 << 29)
#define GEN8_BLEND_ALPHA_TO_COVERAGE_DITHER_ENABLE      (1 << 28)
#define GEN8_BLEND_ALPHA_TEST_ENABLE                    (1 << 27)
#define GEN8_BLEND_ALPHA_TEST_FUNCTION_MASK             INTEL_MASK(26, 24)
#define GEN8_BLEND_ALPHA_TEST_FUNCTION_SHIFT            24
#define GEN8_BLEND_COLOR_DITHER_ENABLE                  (1 << 23)
#define GEN8_BLEND_X_DITHER_OFFSET_MASK                 INTEL_MASK(22, 21)
#define GEN8_BLEND_X_DITHER_OFFSET_SHIFT                21
#define GEN8_BLEND_Y_DITHER_OFFSET_MASK                 INTEL_MASK(20, 19)
#define GEN8_BLEND_Y_DITHER_OFFSET_SHIFT                19
/* DW1 + 2n */
#define GEN8_BLEND_COLOR_BUFFER_BLEND_ENABLE            (1 << 31)
#define GEN8_BLEND_SRC_BLEND_FACTOR_MASK                INTEL_MASK(30, 26)
#define GEN8_BLEND_SRC_BLEND_FACTOR_SHIFT               26
#define GEN8_BLEND_DST_BLEND_FACTOR_MASK                INTEL_MASK(25, 21)
#define GEN8_BLEND_DST_BLEND_FACTOR_SHIFT               21
#define GEN8_BLEND_COLOR_BLEND_FUNCTION_MASK            INTEL_MASK(20, 18)
#define GEN8_BLEND_COLOR_BLEND_FUNCTION_SHIFT           18
#define GEN8_BLEND_SRC_ALPHA_BLEND_FACTOR_MASK          INTEL_MASK(17, 13)
#define GEN8_BLEND_SRC_ALPHA_BLEND_FACTOR_SHIFT         13
#define GEN8_BLEND_DST_ALPHA_BLEND_FACTOR_MASK          INTEL_MASK(12, 8)
#define GEN8_BLEND_DST_ALPHA_BLEND_FACTOR_SHIFT         8
#define GEN8_BLEND_ALPHA_BLEND_FUNCTION_MASK            INTEL_MASK(7, 5)
#define GEN8_BLEND_ALPHA_BLEND_FUNCTION_SHIFT           5
#define GEN8_BLEND_WRITE_DISABLE_ALPHA                  (1 << 3)
#define GEN8_BLEND_WRITE_DISABLE_RED                    (1 << 2)
#define GEN8_BLEND_WRITE_DISABLE_GREEN                  (1 << 1)
#define GEN8_BLEND_WRITE_DISABLE_BLUE                   (1 << 0)
/* DW1 + 2n + 1 */
#define GEN8_BLEND_LOGIC_OP_ENABLE                      (1 << 31)
#define GEN8_BLEND_LOGIC_OP_FUNCTION_MASK               INTEL_MASK(30, 27)
#define GEN8_BLEND_LOGIC_OP_FUNCTION_SHIFT              27
#define GEN8_BLEND_PRE_BLEND_SRC_ONLY_CLAMP_ENABLE      (1 << 4)
#define GEN8_BLEND_COLOR_CLAMP_RANGE_RTFORMAT           (2 << 2)
#define GEN8_BLEND_PRE_BLEND_COLOR_CLAMP_ENABLE         (1 << 1)
#define GEN8_BLEND_POST_BLEND_COLOR_CLAMP_ENABLE        (1 << 0)

#define _3DSTATE_WM_HZ_OP                       0x7852 /* GEN8+ */
/* DW1 */
# define GEN8_WM_HZ_STENCIL_CLEAR                       (1 << 31)
# define GEN8_WM_HZ_DEPTH_CLEAR                         (1 << 30)
# define GEN8_WM_HZ_DEPTH_RESOLVE                       (1 << 28)
# define GEN8_WM_HZ_HIZ_RESOLVE                         (1 << 27)
# define GEN8_WM_HZ_PIXEL_OFFSET_ENABLE                 (1 << 26)
# define GEN8_WM_HZ_FULL_SURFACE_DEPTH_CLEAR            (1 << 25)
# define GEN8_WM_HZ_STENCIL_CLEAR_VALUE_MASK            INTEL_MASK(23, 16)
# define GEN8_WM_HZ_STENCIL_CLEAR_VALUE_SHIFT           16
# define GEN8_WM_HZ_NUM_SAMPLES_MASK                    INTEL_MASK(15, 13)
# define GEN8_WM_HZ_NUM_SAMPLES_SHIFT                   13
/* DW2 */
# define GEN8_WM_HZ_CLEAR_RECTANGLE_Y_MIN_MASK          INTEL_MASK(31, 16)
# define GEN8_WM_HZ_CLEAR_RECTANGLE_Y_MIN_SHIFT         16
# define GEN8_WM_HZ_CLEAR_RECTANGLE_X_MIN_MASK          INTEL_MASK(15, 0)
# define GEN8_WM_HZ_CLEAR_RECTANGLE_X_MIN_SHIFT         0
/* DW3 */
# define GEN8_WM_HZ_CLEAR_RECTANGLE_Y_MAX_MASK          INTEL_MASK(31, 16)
# define GEN8_WM_HZ_CLEAR_RECTANGLE_Y_MAX_SHIFT         16
# define GEN8_WM_HZ_CLEAR_RECTANGLE_X_MAX_MASK          INTEL_MASK(15, 0)
# define GEN8_WM_HZ_CLEAR_RECTANGLE_X_MAX_SHIFT         0
/* DW4 */
# define GEN8_WM_HZ_SAMPLE_MASK_MASK                    INTEL_MASK(15, 0)
# define GEN8_WM_HZ_SAMPLE_MASK_SHIFT                   0


#define _3DSTATE_PS_BLEND                       0x784D /* GEN8+ */
/* DW1 */
# define GEN8_PS_BLEND_ALPHA_TO_COVERAGE_ENABLE         (1 << 31)
# define GEN8_PS_BLEND_HAS_WRITEABLE_RT                 (1 << 30)
# define GEN8_PS_BLEND_COLOR_BUFFER_BLEND_ENABLE        (1 << 29)
# define GEN8_PS_BLEND_SRC_ALPHA_BLEND_FACTOR_MASK      INTEL_MASK(28, 24)
# define GEN8_PS_BLEND_SRC_ALPHA_BLEND_FACTOR_SHIFT     24
# define GEN8_PS_BLEND_DST_ALPHA_BLEND_FACTOR_MASK      INTEL_MASK(23, 19)
# define GEN8_PS_BLEND_DST_ALPHA_BLEND_FACTOR_SHIFT     19
# define GEN8_PS_BLEND_SRC_BLEND_FACTOR_MASK            INTEL_MASK(18, 14)
# define GEN8_PS_BLEND_SRC_BLEND_FACTOR_SHIFT           14
# define GEN8_PS_BLEND_DST_BLEND_FACTOR_MASK            INTEL_MASK(13, 9)
# define GEN8_PS_BLEND_DST_BLEND_FACTOR_SHIFT           9
# define GEN8_PS_BLEND_ALPHA_TEST_ENABLE                (1 << 8)
# define GEN8_PS_BLEND_INDEPENDENT_ALPHA_BLEND_ENABLE   (1 << 7)

#define _3DSTATE_WM_DEPTH_STENCIL               0x784E /* GEN8+ */
/* DW1 */
# define GEN8_WM_DS_STENCIL_FAIL_OP_SHIFT               29
# define GEN8_WM_DS_Z_FAIL_OP_SHIFT                     26
# define GEN8_WM_DS_Z_PASS_OP_SHIFT                     23
# define GEN8_WM_DS_BF_STENCIL_FUNC_SHIFT               20
# define GEN8_WM_DS_BF_STENCIL_FAIL_OP_SHIFT            17
# define GEN8_WM_DS_BF_Z_FAIL_OP_SHIFT                  14
# define GEN8_WM_DS_BF_Z_PASS_OP_SHIFT                  11
# define GEN8_WM_DS_STENCIL_FUNC_SHIFT                  8
# define GEN8_WM_DS_DEPTH_FUNC_SHIFT                    5
# define GEN8_WM_DS_DOUBLE_SIDED_STENCIL_ENABLE         (1 << 4)
# define GEN8_WM_DS_STENCIL_TEST_ENABLE                 (1 << 3)
# define GEN8_WM_DS_STENCIL_BUFFER_WRITE_ENABLE         (1 << 2)
# define GEN8_WM_DS_DEPTH_TEST_ENABLE                   (1 << 1)
# define GEN8_WM_DS_DEPTH_BUFFER_WRITE_ENABLE           (1 << 0)
/* DW2 */
# define GEN8_WM_DS_STENCIL_TEST_MASK_MASK              INTEL_MASK(31, 24)
# define GEN8_WM_DS_STENCIL_TEST_MASK_SHIFT             24
# define GEN8_WM_DS_STENCIL_WRITE_MASK_MASK             INTEL_MASK(23, 16)
# define GEN8_WM_DS_STENCIL_WRITE_MASK_SHIFT            16
# define GEN8_WM_DS_BF_STENCIL_TEST_MASK_MASK           INTEL_MASK(15, 8)
# define GEN8_WM_DS_BF_STENCIL_TEST_MASK_SHIFT          8
# define GEN8_WM_DS_BF_STENCIL_WRITE_MASK_MASK          INTEL_MASK(7, 0)
# define GEN8_WM_DS_BF_STENCIL_WRITE_MASK_SHIFT         0
/* DW3 */
# define GEN9_WM_DS_STENCIL_REF_MASK                    INTEL_MASK(15, 8)
# define GEN9_WM_DS_STENCIL_REF_SHIFT                   8
# define GEN9_WM_DS_BF_STENCIL_REF_MASK                 INTEL_MASK(7, 0)
# define GEN9_WM_DS_BF_STENCIL_REF_SHIFT                0

enum brw_pixel_shader_computed_depth_mode {
   BRW_PSCDEPTH_OFF   = 0, /* PS does not compute depth */
   BRW_PSCDEPTH_ON    = 1, /* PS computes depth; no guarantee about value */
   BRW_PSCDEPTH_ON_GE = 2, /* PS guarantees output depth >= source depth */
   BRW_PSCDEPTH_ON_LE = 3, /* PS guarantees output depth <= source depth */
};

#define _3DSTATE_PS_EXTRA                       0x784F /* GEN8+ */
/* DW1 */
# define GEN8_PSX_PIXEL_SHADER_VALID                    (1 << 31)
# define GEN8_PSX_PIXEL_SHADER_NO_RT_WRITE              (1 << 30)
# define GEN8_PSX_OMASK_TO_RENDER_TARGET                (1 << 29)
# define GEN8_PSX_KILL_ENABLE                           (1 << 28)
# define GEN8_PSX_COMPUTED_DEPTH_MODE_SHIFT             26
# define GEN8_PSX_FORCE_COMPUTED_DEPTH                  (1 << 25)
# define GEN8_PSX_USES_SOURCE_DEPTH                     (1 << 24)
# define GEN8_PSX_USES_SOURCE_W                         (1 << 23)
# define GEN8_PSX_ATTRIBUTE_ENABLE                      (1 << 8)
# define GEN8_PSX_SHADER_DISABLES_ALPHA_TO_COVERAGE     (1 << 7)
# define GEN8_PSX_SHADER_IS_PER_SAMPLE                  (1 << 6)
# define GEN8_PSX_SHADER_COMPUTES_STENCIL               (1 << 5)
# define GEN8_PSX_SHADER_HAS_UAV                        (1 << 2)
# define GEN8_PSX_SHADER_USES_INPUT_COVERAGE_MASK       (1 << 1)

enum brw_wm_barycentric_interp_mode {
   BRW_WM_PERSPECTIVE_PIXEL_BARYCENTRIC		= 0,
   BRW_WM_PERSPECTIVE_CENTROID_BARYCENTRIC	= 1,
   BRW_WM_PERSPECTIVE_SAMPLE_BARYCENTRIC	= 2,
   BRW_WM_NONPERSPECTIVE_PIXEL_BARYCENTRIC	= 3,
   BRW_WM_NONPERSPECTIVE_CENTROID_BARYCENTRIC	= 4,
   BRW_WM_NONPERSPECTIVE_SAMPLE_BARYCENTRIC	= 5,
   BRW_WM_BARYCENTRIC_INTERP_MODE_COUNT  = 6
};
#define BRW_WM_NONPERSPECTIVE_BARYCENTRIC_BITS \
   ((1 << BRW_WM_NONPERSPECTIVE_PIXEL_BARYCENTRIC) | \
    (1 << BRW_WM_NONPERSPECTIVE_CENTROID_BARYCENTRIC) | \
    (1 << BRW_WM_NONPERSPECTIVE_SAMPLE_BARYCENTRIC))

#define _3DSTATE_WM				0x7814 /* GEN6+ */
/* DW1: kernel pointer */
/* DW2 */
# define GEN6_WM_SPF_MODE				(1 << 31)
# define GEN6_WM_VECTOR_MASK_ENABLE			(1 << 30)
# define GEN6_WM_SAMPLER_COUNT_SHIFT			27
# define GEN6_WM_BINDING_TABLE_ENTRY_COUNT_SHIFT	18
# define GEN6_WM_FLOATING_POINT_MODE_IEEE_754		(0 << 16)
# define GEN6_WM_FLOATING_POINT_MODE_ALT		(1 << 16)
/* DW3: scratch space */
/* DW4 */
# define GEN6_WM_STATISTICS_ENABLE			(1 << 31)
# define GEN6_WM_DEPTH_CLEAR				(1 << 30)
# define GEN6_WM_DEPTH_RESOLVE				(1 << 28)
# define GEN6_WM_HIERARCHICAL_DEPTH_RESOLVE		(1 << 27)
# define GEN6_WM_DISPATCH_START_GRF_SHIFT_0		16
# define GEN6_WM_DISPATCH_START_GRF_SHIFT_1		8
# define GEN6_WM_DISPATCH_START_GRF_SHIFT_2		0
/* DW5 */
# define GEN6_WM_MAX_THREADS_SHIFT			25
# define GEN6_WM_KILL_ENABLE				(1 << 22)
# define GEN6_WM_COMPUTED_DEPTH				(1 << 21)
# define GEN6_WM_USES_SOURCE_DEPTH			(1 << 20)
# define GEN6_WM_DISPATCH_ENABLE			(1 << 19)
# define GEN6_WM_LINE_END_CAP_AA_WIDTH_0_5		(0 << 16)
# define GEN6_WM_LINE_END_CAP_AA_WIDTH_1_0		(1 << 16)
# define GEN6_WM_LINE_END_CAP_AA_WIDTH_2_0		(2 << 16)
# define GEN6_WM_LINE_END_CAP_AA_WIDTH_4_0		(3 << 16)
# define GEN6_WM_LINE_AA_WIDTH_0_5			(0 << 14)
# define GEN6_WM_LINE_AA_WIDTH_1_0			(1 << 14)
# define GEN6_WM_LINE_AA_WIDTH_2_0			(2 << 14)
# define GEN6_WM_LINE_AA_WIDTH_4_0			(3 << 14)
# define GEN6_WM_POLYGON_STIPPLE_ENABLE			(1 << 13)
# define GEN6_WM_LINE_STIPPLE_ENABLE			(1 << 11)
# define GEN6_WM_OMASK_TO_RENDER_TARGET			(1 << 9)
# define GEN6_WM_USES_SOURCE_W				(1 << 8)
# define GEN6_WM_DUAL_SOURCE_BLEND_ENABLE		(1 << 7)
# define GEN6_WM_32_DISPATCH_ENABLE			(1 << 2)
# define GEN6_WM_16_DISPATCH_ENABLE			(1 << 1)
# define GEN6_WM_8_DISPATCH_ENABLE			(1 << 0)
/* DW6 */
# define GEN6_WM_NUM_SF_OUTPUTS_SHIFT			20
# define GEN6_WM_POSOFFSET_NONE				(0 << 18)
# define GEN6_WM_POSOFFSET_CENTROID			(2 << 18)
# define GEN6_WM_POSOFFSET_SAMPLE			(3 << 18)
# define GEN6_WM_POSITION_ZW_PIXEL			(0 << 16)
# define GEN6_WM_POSITION_ZW_CENTROID			(2 << 16)
# define GEN6_WM_POSITION_ZW_SAMPLE			(3 << 16)
# define GEN6_WM_NONPERSPECTIVE_SAMPLE_BARYCENTRIC	(1 << 15)
# define GEN6_WM_NONPERSPECTIVE_CENTROID_BARYCENTRIC	(1 << 14)
# define GEN6_WM_NONPERSPECTIVE_PIXEL_BARYCENTRIC	(1 << 13)
# define GEN6_WM_PERSPECTIVE_SAMPLE_BARYCENTRIC		(1 << 12)
# define GEN6_WM_PERSPECTIVE_CENTROID_BARYCENTRIC	(1 << 11)
# define GEN6_WM_PERSPECTIVE_PIXEL_BARYCENTRIC		(1 << 10)
# define GEN6_WM_BARYCENTRIC_INTERPOLATION_MODE_SHIFT   10
# define GEN6_WM_POINT_RASTRULE_UPPER_RIGHT		(1 << 9)
# define GEN6_WM_MSRAST_OFF_PIXEL			(0 << 1)
# define GEN6_WM_MSRAST_OFF_PATTERN			(1 << 1)
# define GEN6_WM_MSRAST_ON_PIXEL			(2 << 1)
# define GEN6_WM_MSRAST_ON_PATTERN			(3 << 1)
# define GEN6_WM_MSDISPMODE_PERSAMPLE			(0 << 0)
# define GEN6_WM_MSDISPMODE_PERPIXEL			(1 << 0)
/* DW7: kernel 1 pointer */
/* DW8: kernel 2 pointer */

#define _3DSTATE_CONSTANT_VS		      0x7815 /* GEN6+ */
#define _3DSTATE_CONSTANT_GS		      0x7816 /* GEN6+ */
#define _3DSTATE_CONSTANT_PS		      0x7817 /* GEN6+ */
# define GEN6_CONSTANT_BUFFER_3_ENABLE			(1 << 15)
# define GEN6_CONSTANT_BUFFER_2_ENABLE			(1 << 14)
# define GEN6_CONSTANT_BUFFER_1_ENABLE			(1 << 13)
# define GEN6_CONSTANT_BUFFER_0_ENABLE			(1 << 12)

#define _3DSTATE_CONSTANT_HS                  0x7819 /* GEN7+ */
#define _3DSTATE_CONSTANT_DS                  0x781A /* GEN7+ */

#define _3DSTATE_STREAMOUT                    0x781e /* GEN7+ */
/* DW1 */
# define SO_FUNCTION_ENABLE				(1 << 31)
# define SO_RENDERING_DISABLE				(1 << 30)
/* This selects which incoming rendering stream goes down the pipeline.  The
 * rendering stream is 0 if not defined by special cases in the GS state.
 */
# define SO_RENDER_STREAM_SELECT_SHIFT			27
# define SO_RENDER_STREAM_SELECT_MASK			INTEL_MASK(28, 27)
/* Controls reordering of TRISTRIP_* elements in stream output (not rendering).
 */
# define SO_REORDER_TRAILING				(1 << 26)
/* Controls SO_NUM_PRIMS_WRITTEN_* and SO_PRIM_STORAGE_* */
# define SO_STATISTICS_ENABLE				(1 << 25)
# define SO_BUFFER_ENABLE(n)				(1 << (8 + (n)))
/* DW2 */
# define SO_STREAM_3_VERTEX_READ_OFFSET_SHIFT		29
# define SO_STREAM_3_VERTEX_READ_OFFSET_MASK		INTEL_MASK(29, 29)
# define SO_STREAM_3_VERTEX_READ_LENGTH_SHIFT		24
# define SO_STREAM_3_VERTEX_READ_LENGTH_MASK		INTEL_MASK(28, 24)
# define SO_STREAM_2_VERTEX_READ_OFFSET_SHIFT		21
# define SO_STREAM_2_VERTEX_READ_OFFSET_MASK		INTEL_MASK(21, 21)
# define SO_STREAM_2_VERTEX_READ_LENGTH_SHIFT		16
# define SO_STREAM_2_VERTEX_READ_LENGTH_MASK		INTEL_MASK(20, 16)
# define SO_STREAM_1_VERTEX_READ_OFFSET_SHIFT		13
# define SO_STREAM_1_VERTEX_READ_OFFSET_MASK		INTEL_MASK(13, 13)
# define SO_STREAM_1_VERTEX_READ_LENGTH_SHIFT		8
# define SO_STREAM_1_VERTEX_READ_LENGTH_MASK		INTEL_MASK(12, 8)
# define SO_STREAM_0_VERTEX_READ_OFFSET_SHIFT		5
# define SO_STREAM_0_VERTEX_READ_OFFSET_MASK		INTEL_MASK(5, 5)
# define SO_STREAM_0_VERTEX_READ_LENGTH_SHIFT		0
# define SO_STREAM_0_VERTEX_READ_LENGTH_MASK		INTEL_MASK(4, 0)

/* 3DSTATE_WM for Gen7 */
/* DW1 */
# define GEN7_WM_STATISTICS_ENABLE			(1 << 31)
# define GEN7_WM_DEPTH_CLEAR				(1 << 30)
# define GEN7_WM_DISPATCH_ENABLE			(1 << 29)
# define GEN7_WM_DEPTH_RESOLVE				(1 << 28)
# define GEN7_WM_HIERARCHICAL_DEPTH_RESOLVE		(1 << 27)
# define GEN7_WM_KILL_ENABLE				(1 << 25)
# define GEN7_WM_COMPUTED_DEPTH_MODE_SHIFT              23
# define GEN7_WM_USES_SOURCE_DEPTH			(1 << 20)
# define GEN7_WM_USES_SOURCE_W			        (1 << 19)
# define GEN7_WM_POSITION_ZW_PIXEL			(0 << 17)
# define GEN7_WM_POSITION_ZW_CENTROID			(2 << 17)
# define GEN7_WM_POSITION_ZW_SAMPLE			(3 << 17)
# define GEN7_WM_BARYCENTRIC_INTERPOLATION_MODE_SHIFT   11
# define GEN7_WM_USES_INPUT_COVERAGE_MASK	        (1 << 10)
# define GEN7_WM_LINE_END_CAP_AA_WIDTH_0_5		(0 << 8)
# define GEN7_WM_LINE_END_CAP_AA_WIDTH_1_0		(1 << 8)
# define GEN7_WM_LINE_END_CAP_AA_WIDTH_2_0		(2 << 8)
# define GEN7_WM_LINE_END_CAP_AA_WIDTH_4_0		(3 << 8)
# define GEN7_WM_LINE_AA_WIDTH_0_5			(0 << 6)
# define GEN7_WM_LINE_AA_WIDTH_1_0			(1 << 6)
# define GEN7_WM_LINE_AA_WIDTH_2_0			(2 << 6)
# define GEN7_WM_LINE_AA_WIDTH_4_0			(3 << 6)
# define GEN7_WM_POLYGON_STIPPLE_ENABLE			(1 << 4)
# define GEN7_WM_LINE_STIPPLE_ENABLE			(1 << 3)
# define GEN7_WM_POINT_RASTRULE_UPPER_RIGHT		(1 << 2)
# define GEN7_WM_MSRAST_OFF_PIXEL			(0 << 0)
# define GEN7_WM_MSRAST_OFF_PATTERN			(1 << 0)
# define GEN7_WM_MSRAST_ON_PIXEL			(2 << 0)
# define GEN7_WM_MSRAST_ON_PATTERN			(3 << 0)
/* DW2 */
# define GEN7_WM_MSDISPMODE_PERSAMPLE			(0 << 31)
# define GEN7_WM_MSDISPMODE_PERPIXEL			(1 << 31)

#define _3DSTATE_PS				0x7820 /* GEN7+ */
/* DW1: kernel pointer */
/* DW2 */
# define GEN7_PS_SPF_MODE				(1 << 31)
# define GEN7_PS_VECTOR_MASK_ENABLE			(1 << 30)
# define GEN7_PS_SAMPLER_COUNT_SHIFT			27
# define GEN7_PS_SAMPLER_COUNT_MASK                     INTEL_MASK(29, 27)
# define GEN7_PS_BINDING_TABLE_ENTRY_COUNT_SHIFT	18
# define GEN7_PS_FLOATING_POINT_MODE_IEEE_754		(0 << 16)
# define GEN7_PS_FLOATING_POINT_MODE_ALT		(1 << 16)
/* DW3: scratch space */
/* DW4 */
# define IVB_PS_MAX_THREADS_SHIFT			24
# define HSW_PS_MAX_THREADS_SHIFT			23
# define HSW_PS_SAMPLE_MASK_SHIFT		        12
# define HSW_PS_SAMPLE_MASK_MASK			INTEL_MASK(19, 12)
# define GEN7_PS_PUSH_CONSTANT_ENABLE		        (1 << 11)
# define GEN7_PS_ATTRIBUTE_ENABLE		        (1 << 10)
# define GEN7_PS_OMASK_TO_RENDER_TARGET			(1 << 9)
# define GEN7_PS_RENDER_TARGET_FAST_CLEAR_ENABLE	(1 << 8)
# define GEN7_PS_DUAL_SOURCE_BLEND_ENABLE		(1 << 7)
# define GEN7_PS_RENDER_TARGET_RESOLVE_ENABLE		(1 << 6)
# define GEN7_PS_POSOFFSET_NONE				(0 << 3)
# define GEN7_PS_POSOFFSET_CENTROID			(2 << 3)
# define GEN7_PS_POSOFFSET_SAMPLE			(3 << 3)
# define GEN7_PS_32_DISPATCH_ENABLE			(1 << 2)
# define GEN7_PS_16_DISPATCH_ENABLE			(1 << 1)
# define GEN7_PS_8_DISPATCH_ENABLE			(1 << 0)
/* DW5 */
# define GEN7_PS_DISPATCH_START_GRF_SHIFT_0		16
# define GEN7_PS_DISPATCH_START_GRF_SHIFT_1		8
# define GEN7_PS_DISPATCH_START_GRF_SHIFT_2		0
/* DW6: kernel 1 pointer */
/* DW7: kernel 2 pointer */

#define _3DSTATE_SAMPLE_MASK			0x7818 /* GEN6+ */

#define _3DSTATE_DRAWING_RECTANGLE		0x7900
#define _3DSTATE_BLEND_CONSTANT_COLOR		0x7901
#define _3DSTATE_CHROMA_KEY			0x7904
#define _3DSTATE_DEPTH_BUFFER			0x7905 /* GEN4-6 */
#define _3DSTATE_POLY_STIPPLE_OFFSET		0x7906
#define _3DSTATE_POLY_STIPPLE_PATTERN		0x7907
#define _3DSTATE_LINE_STIPPLE_PATTERN		0x7908
#define _3DSTATE_GLOBAL_DEPTH_OFFSET_CLAMP	0x7909
#define _3DSTATE_AA_LINE_PARAMETERS		0x790a /* G45+ */

#define _3DSTATE_GS_SVB_INDEX			0x790b /* CTG+ */
/* DW1 */
# define SVB_INDEX_SHIFT				29
# define SVB_LOAD_INTERNAL_VERTEX_COUNT			(1 << 0) /* SNB+ */
/* DW2: SVB index */
/* DW3: SVB maximum index */

#define _3DSTATE_MULTISAMPLE			0x790d /* GEN6+ */
#define GEN8_3DSTATE_MULTISAMPLE		0x780d /* GEN8+ */
/* DW1 */
# define MS_PIXEL_LOCATION_CENTER			(0 << 4)
# define MS_PIXEL_LOCATION_UPPER_LEFT			(1 << 4)
# define MS_NUMSAMPLES_1				(0 << 1)
# define MS_NUMSAMPLES_2				(1 << 1)
# define MS_NUMSAMPLES_4				(2 << 1)
# define MS_NUMSAMPLES_8				(3 << 1)
# define MS_NUMSAMPLES_16				(4 << 1)

#define _3DSTATE_SAMPLE_PATTERN                 0x791c

#define _3DSTATE_STENCIL_BUFFER			0x790e /* ILK, SNB */
#define _3DSTATE_HIER_DEPTH_BUFFER		0x790f /* ILK, SNB */

#define GEN7_3DSTATE_CLEAR_PARAMS		0x7804
#define GEN7_3DSTATE_DEPTH_BUFFER		0x7805
#define GEN7_3DSTATE_STENCIL_BUFFER		0x7806
# define HSW_STENCIL_ENABLED                            (1 << 31)
#define GEN7_3DSTATE_HIER_DEPTH_BUFFER		0x7807

#define _3DSTATE_CLEAR_PARAMS			0x7910 /* ILK, SNB */
# define GEN5_DEPTH_CLEAR_VALID				(1 << 15)
/* DW1: depth clear value */
/* DW2 */
# define GEN7_DEPTH_CLEAR_VALID				(1 << 0)

#define _3DSTATE_SO_DECL_LIST			0x7917 /* GEN7+ */
/* DW1 */
# define SO_STREAM_TO_BUFFER_SELECTS_3_SHIFT		12
# define SO_STREAM_TO_BUFFER_SELECTS_3_MASK		INTEL_MASK(15, 12)
# define SO_STREAM_TO_BUFFER_SELECTS_2_SHIFT		8
# define SO_STREAM_TO_BUFFER_SELECTS_2_MASK		INTEL_MASK(11, 8)
# define SO_STREAM_TO_BUFFER_SELECTS_1_SHIFT		4
# define SO_STREAM_TO_BUFFER_SELECTS_1_MASK		INTEL_MASK(7, 4)
# define SO_STREAM_TO_BUFFER_SELECTS_0_SHIFT		0
# define SO_STREAM_TO_BUFFER_SELECTS_0_MASK		INTEL_MASK(3, 0)
/* DW2 */
# define SO_NUM_ENTRIES_3_SHIFT				24
# define SO_NUM_ENTRIES_3_MASK				INTEL_MASK(31, 24)
# define SO_NUM_ENTRIES_2_SHIFT				16
# define SO_NUM_ENTRIES_2_MASK				INTEL_MASK(23, 16)
# define SO_NUM_ENTRIES_1_SHIFT				8
# define SO_NUM_ENTRIES_1_MASK				INTEL_MASK(15, 8)
# define SO_NUM_ENTRIES_0_SHIFT				0
# define SO_NUM_ENTRIES_0_MASK				INTEL_MASK(7, 0)

/* SO_DECL DW0 */
# define SO_DECL_OUTPUT_BUFFER_SLOT_SHIFT		12
# define SO_DECL_OUTPUT_BUFFER_SLOT_MASK		INTEL_MASK(13, 12)
# define SO_DECL_HOLE_FLAG				(1 << 11)
# define SO_DECL_REGISTER_INDEX_SHIFT			4
# define SO_DECL_REGISTER_INDEX_MASK			INTEL_MASK(9, 4)
# define SO_DECL_COMPONENT_MASK_SHIFT			0
# define SO_DECL_COMPONENT_MASK_MASK			INTEL_MASK(3, 0)

#define _3DSTATE_SO_BUFFER                    0x7918 /* GEN7+ */
/* DW1 */
# define GEN8_SO_BUFFER_ENABLE                          (1 << 31)
# define SO_BUFFER_INDEX_SHIFT				29
# define SO_BUFFER_INDEX_MASK				INTEL_MASK(30, 29)
# define GEN8_SO_BUFFER_OFFSET_WRITE_ENABLE             (1 << 21)
# define GEN8_SO_BUFFER_OFFSET_ADDRESS_ENABLE           (1 << 20)
# define SO_BUFFER_PITCH_SHIFT				0
# define SO_BUFFER_PITCH_MASK				INTEL_MASK(11, 0)
/* DW2: start address */
/* DW3: end address. */

#define CMD_MI_FLUSH                  0x0200

# define BLT_X_SHIFT					0
# define BLT_X_MASK					INTEL_MASK(15, 0)
# define BLT_Y_SHIFT					16
# define BLT_Y_MASK					INTEL_MASK(31, 16)

#define GEN5_MI_REPORT_PERF_COUNT ((0x26 << 23) | (3 - 2))
/* DW0 */
# define GEN5_MI_COUNTER_SET_0      (0 << 6)
# define GEN5_MI_COUNTER_SET_1      (1 << 6)
/* DW1 */
# define MI_COUNTER_ADDRESS_GTT     (1 << 0)
/* DW2: a user-defined report ID (written to the buffer but can be anything) */

#define GEN6_MI_REPORT_PERF_COUNT ((0x28 << 23) | (3 - 2))

/* Bitfields for the URB_WRITE message, DW2 of message header: */
#define URB_WRITE_PRIM_END		0x1
#define URB_WRITE_PRIM_START		0x2
#define URB_WRITE_PRIM_TYPE_SHIFT	2


/* Maximum number of entries that can be addressed using a binding table
 * pointer of type SURFTYPE_BUFFER
 */
#define BRW_MAX_NUM_BUFFER_ENTRIES	(1 << 27)

/* Memory Object Control State:
 * Specifying zero for L3 means "uncached in L3", at least on Haswell
 * and Baytrail, since there are no PTE flags for setting L3 cacheability.
 * On Ivybridge, the PTEs do have a cache-in-L3 bit, so setting MOCS to 0
 * may still respect that.
 */
#define GEN7_MOCS_L3                    1

/* Ivybridge only: cache in LLC.
 * Specifying zero here means to use the PTE values set by the kernel;
 * non-zero overrides the PTE values.
 */
#define IVB_MOCS_LLC                    (1 << 1)

/* Baytrail only: snoop in CPU cache */
#define BYT_MOCS_SNOOP                  (1 << 1)

/* Haswell only: LLC/eLLC controls (write-back or uncached).
 * Specifying zero here means to use the PTE values set by the kernel,
 * which is useful since it offers additional control (write-through
 * cacheing and age).  Non-zero overrides the PTE values.
 */
#define HSW_MOCS_UC_LLC_UC_ELLC         (1 << 1)
#define HSW_MOCS_WB_LLC_WB_ELLC         (2 << 1)
#define HSW_MOCS_UC_LLC_WB_ELLC         (3 << 1)

/* Broadwell: these defines always use all available caches (L3, LLC, eLLC),
 * and let you force write-back (WB) or write-through (WT) caching, or leave
 * it up to the page table entry (PTE) specified by the kernel.
 */
#define BDW_MOCS_WB  0x78
#define BDW_MOCS_WT  0x58
#define BDW_MOCS_PTE 0x18

/* Skylake: MOCS is now an index into an array of 64 different configurable
 * cache settings.  We still use only either write-back or write-through; and
 * rely on the documented default values.
 */
#define SKL_MOCS_WB 9
#define SKL_MOCS_WT 5

#define MEDIA_VFE_STATE                         0x7000
/* GEN7 DW2, GEN8+ DW3 */
# define MEDIA_VFE_STATE_MAX_THREADS_SHIFT      16
# define MEDIA_VFE_STATE_MAX_THREADS_MASK       INTEL_MASK(31, 16)
# define MEDIA_VFE_STATE_URB_ENTRIES_SHIFT      8
# define MEDIA_VFE_STATE_URB_ENTRIES_MASK       INTEL_MASK(15, 8)
# define MEDIA_VFE_STATE_RESET_GTW_TIMER_SHIFT  7
# define MEDIA_VFE_STATE_RESET_GTW_TIMER_MASK   INTEL_MASK(7, 7)
# define MEDIA_VFE_STATE_BYPASS_GTW_SHIFT       6
# define MEDIA_VFE_STATE_BYPASS_GTW_MASK        INTEL_MASK(6, 6)
# define GEN7_MEDIA_VFE_STATE_GPGPU_MODE_SHIFT  2
# define GEN7_MEDIA_VFE_STATE_GPGPU_MODE_MASK   INTEL_MASK(2, 2)
/* GEN7 DW4, GEN8+ DW5 */
# define MEDIA_VFE_STATE_URB_ALLOC_SHIFT        16
# define MEDIA_VFE_STATE_URB_ALLOC_MASK         INTEL_MASK(31, 16)
# define MEDIA_VFE_STATE_CURBE_ALLOC_SHIFT      0
# define MEDIA_VFE_STATE_CURBE_ALLOC_MASK       INTEL_MASK(15, 0)

#define MEDIA_INTERFACE_DESCRIPTOR_LOAD         0x7002
#define MEDIA_STATE_FLUSH                       0x7004
#define GPGPU_WALKER                            0x7105
/* GEN8+ DW2 */
# define GPGPU_WALKER_INDIRECT_LENGTH_SHIFT     0
# define GPGPU_WALKER_INDIRECT_LENGTH_MASK      INTEL_MASK(15, 0)
/* GEN7 DW2, GEN8+ DW4 */
# define GPGPU_WALKER_SIMD_SIZE_SHIFT           30
# define GPGPU_WALKER_SIMD_SIZE_MASK            INTEL_MASK(31, 30)
# define GPGPU_WALKER_THREAD_DEPTH_MAX_SHIFT    16
# define GPGPU_WALKER_THREAD_DEPTH_MAX_MASK     INTEL_MASK(21, 16)
# define GPGPU_WALKER_THREAD_HEIGHT_MAX_SHIFT   8
# define GPGPU_WALKER_THREAD_HEIGHT_MAX_MASK    INTEL_MASK(31, 8)
# define GPGPU_WALKER_THREAD_WIDTH_MAX_SHIFT    0
# define GPGPU_WALKER_THREAD_WIDTH_MAX_MASK     INTEL_MASK(5, 0)

#endif
