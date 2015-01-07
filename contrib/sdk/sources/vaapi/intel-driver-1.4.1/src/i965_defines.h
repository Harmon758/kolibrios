#ifndef _I965_DEFINES_H_
#define _I965_DEFINES_H_

#define CMD(pipeline,op,sub_op)		((3 << 29) | \
                                           	((pipeline) << 27) | \
                                           	((op) << 24) | \
                                           	((sub_op) << 16))

#define CMD_URB_FENCE                           CMD(0, 0, 0)
#define CMD_CS_URB_STATE                        CMD(0, 0, 1)
#define CMD_CONSTANT_BUFFER                     CMD(0, 0, 2)
#define CMD_STATE_PREFETCH                      CMD(0, 0, 3)

#define CMD_STATE_BASE_ADDRESS                  CMD(0, 1, 1)
#define CMD_STATE_SIP                           CMD(0, 1, 2)
#define CMD_PIPELINE_SELECT                     CMD(1, 1, 4)
#define CMD_SAMPLER_PALETTE_LOAD                CMD(3, 1, 2)

#define CMD_MEDIA_STATE_POINTERS                CMD(2, 0, 0)
#define CMD_MEDIA_VFE_STATE                     CMD(2, 0, 0)
#define CMD_MEDIA_CURBE_LOAD                    CMD(2, 0, 1)
#define CMD_MEDIA_INTERFACE_LOAD                CMD(2, 0, 2)
#define CMD_MEDIA_OBJECT                        CMD(2, 1, 0)
#define CMD_MEDIA_OBJECT_EX                     CMD(2, 1, 1)

#define CMD_AVC_BSD_IMG_STATE                   CMD(2, 4, 0)
#define CMD_AVC_BSD_QM_STATE                    CMD(2, 4, 1)
#define CMD_AVC_BSD_SLICE_STATE                 CMD(2, 4, 2)
#define CMD_AVC_BSD_BUF_BASE_STATE              CMD(2, 4, 3)
#define CMD_BSD_IND_OBJ_BASE_ADDR               CMD(2, 4, 4)
#define CMD_AVC_BSD_OBJECT                      CMD(2, 4, 8)

#define CMD_MEDIA_VFE_STATE                     CMD(2, 0, 0)
#define CMD_MEDIA_CURBE_LOAD                    CMD(2, 0, 1)
#define CMD_MEDIA_INTERFACE_DESCRIPTOR_LOAD     CMD(2, 0, 2)
#define CMD_MEDIA_GATEWAY_STATE                 CMD(2, 0, 3)
#define CMD_MEDIA_STATE_FLUSH                   CMD(2, 0, 4)
#define CMD_MEDIA_OBJECT_WALKER                 CMD(2, 1, 3)

#define CMD_PIPELINED_POINTERS                  CMD(3, 0, 0)
#define CMD_BINDING_TABLE_POINTERS              CMD(3, 0, 1)
# define GEN6_BINDING_TABLE_MODIFY_PS           (1 << 12)/* for GEN6 */
# define GEN6_BINDING_TABLE_MODIFY_GS           (1 << 9) /* for GEN6 */
# define GEN6_BINDING_TABLE_MODIFY_VS           (1 << 8) /* for GEN6 */

#define CMD_VERTEX_BUFFERS                      CMD(3, 0, 8)
#define CMD_VERTEX_ELEMENTS                     CMD(3, 0, 9)
#define CMD_DRAWING_RECTANGLE                   CMD(3, 1, 0)
#define CMD_CONSTANT_COLOR                      CMD(3, 1, 1)
#define CMD_3DPRIMITIVE                         CMD(3, 3, 0)

#define CMD_DEPTH_BUFFER                        CMD(3, 1, 5)
# define CMD_DEPTH_BUFFER_TYPE_SHIFT            29
# define CMD_DEPTH_BUFFER_FORMAT_SHIFT          18

#define CMD_CLEAR_PARAMS                        CMD(3, 1, 0x10)
/* DW1 */
# define CMD_CLEAR_PARAMS_DEPTH_CLEAR_VALID     (1 << 15)

/* for GEN6+ */
#define GEN6_3DSTATE_SAMPLER_STATE_POINTERS	CMD(3, 0, 0x02)
# define GEN6_3DSTATE_SAMPLER_STATE_MODIFY_PS	(1 << 12)
# define GEN6_3DSTATE_SAMPLER_STATE_MODIFY_GS	(1 << 9)
# define GEN6_3DSTATE_SAMPLER_STATE_MODIFY_VS	(1 << 8)

#define GEN6_3DSTATE_URB			CMD(3, 0, 0x05)
/* DW1 */
# define GEN6_3DSTATE_URB_VS_SIZE_SHIFT		16
# define GEN6_3DSTATE_URB_VS_ENTRIES_SHIFT	0
/* DW2 */
# define GEN6_3DSTATE_URB_GS_ENTRIES_SHIFT	8
# define GEN6_3DSTATE_URB_GS_SIZE_SHIFT		0

#define GEN6_3DSTATE_VIEWPORT_STATE_POINTERS	CMD(3, 0, 0x0d)
# define GEN6_3DSTATE_VIEWPORT_STATE_MODIFY_CC		(1 << 12)
# define GEN6_3DSTATE_VIEWPORT_STATE_MODIFY_SF		(1 << 11)
# define GEN6_3DSTATE_VIEWPORT_STATE_MODIFY_CLIP	(1 << 10)

#define GEN6_3DSTATE_CC_STATE_POINTERS		CMD(3, 0, 0x0e)

#define GEN6_3DSTATE_VS				CMD(3, 0, 0x10)

#define GEN6_3DSTATE_GS				CMD(3, 0, 0x11)
/* DW4 */
# define GEN6_3DSTATE_GS_DISPATCH_START_GRF_SHIFT	0

#define GEN6_3DSTATE_CLIP			CMD(3, 0, 0x12)

#define GEN6_3DSTATE_SF				CMD(3, 0, 0x13)
/* DW1 on GEN6 */
# define GEN6_3DSTATE_SF_NUM_OUTPUTS_SHIFT              22
# define GEN6_3DSTATE_SF_URB_ENTRY_READ_LENGTH_SHIFT    11
# define GEN6_3DSTATE_SF_URB_ENTRY_READ_OFFSET_SHIFT    4
/* DW1 on GEN7 */
# define GEN7_SF_DEPTH_BUFFER_SURFACE_FORMAT_SHIFT      12


/* DW2 */
/* DW3 */
# define GEN6_3DSTATE_SF_CULL_BOTH			(0 << 29)
# define GEN6_3DSTATE_SF_CULL_NONE			(1 << 29)
# define GEN6_3DSTATE_SF_CULL_FRONT			(2 << 29)
# define GEN6_3DSTATE_SF_CULL_BACK			(3 << 29)
/* DW4 */
# define GEN6_3DSTATE_SF_TRI_PROVOKE_SHIFT		29
# define GEN6_3DSTATE_SF_LINE_PROVOKE_SHIFT		27
# define GEN6_3DSTATE_SF_TRIFAN_PROVOKE_SHIFT		25

#define GEN8_3DSTATE_RASTER			CMD(3, 0, 0x50)
# define GEN8_3DSTATE_RASTER_CULL_BOTH			(0 << 16)
# define GEN8_3DSTATE_RASTER_CULL_NONE			(1 << 16)
# define GEN8_3DSTATE_RASTER_CULL_FRONT			(2 << 16)
# define GEN8_3DSTATE_RASTER_CULL_BACK			(3 << 16)

#define GEN6_3DSTATE_WM				CMD(3, 0, 0x14)
/* DW2 */
# define GEN6_3DSTATE_WM_SAMPLER_COUNT_SHITF			27
# define GEN6_3DSTATE_WM_BINDING_TABLE_ENTRY_COUNT_SHIFT	18
/* DW4 */
# define GEN6_3DSTATE_WM_DISPATCH_START_GRF_0_SHIFT		16
/* DW5 */
# define GEN6_3DSTATE_WM_MAX_THREADS_SHIFT			25
# define GEN6_3DSTATE_WM_DISPATCH_ENABLE			(1 << 19)
# define GEN6_3DSTATE_WM_16_DISPATCH_ENABLE			(1 << 1)
# define GEN6_3DSTATE_WM_8_DISPATCH_ENABLE			(1 << 0)
/* DW6 */
# define GEN6_3DSTATE_WM_NUM_SF_OUTPUTS_SHIFT			20
# define GEN6_3DSTATE_WM_NONPERSPECTIVE_SAMPLE_BARYCENTRIC	(1 << 15)
# define GEN6_3DSTATE_WM_NONPERSPECTIVE_CENTROID_BARYCENTRIC	(1 << 14)
# define GEN6_3DSTATE_WM_NONPERSPECTIVE_PIXEL_BARYCENTRIC	(1 << 13)
# define GEN6_3DSTATE_WM_PERSPECTIVE_SAMPLE_BARYCENTRIC		(1 << 12)
# define GEN6_3DSTATE_WM_PERSPECTIVE_CENTROID_BARYCENTRIC	(1 << 11)
# define GEN6_3DSTATE_WM_PERSPECTIVE_PIXEL_BARYCENTRIC		(1 << 10)

/* 3DSTATE_WM on GEN7 */
/* DW1 */
# define GEN7_WM_STATISTICS_ENABLE                              (1 << 31)
# define GEN7_WM_DEPTH_CLEAR                                    (1 << 30)
# define GEN7_WM_DISPATCH_ENABLE                                (1 << 29)
# define GEN6_WM_DEPTH_RESOLVE                                  (1 << 28)
# define GEN7_WM_HIERARCHICAL_DEPTH_RESOLVE                     (1 << 27)
# define GEN7_WM_KILL_ENABLE                                    (1 << 25)
# define GEN7_WM_PSCDEPTH_OFF                                   (0 << 23)
# define GEN7_WM_PSCDEPTH_ON                                    (1 << 23)
# define GEN7_WM_PSCDEPTH_ON_GE                                 (2 << 23)
# define GEN7_WM_PSCDEPTH_ON_LE                                 (3 << 23)
# define GEN7_WM_USES_SOURCE_DEPTH                              (1 << 20)
# define GEN7_WM_USES_SOURCE_W                                  (1 << 19)
# define GEN7_WM_POSITION_ZW_PIXEL                              (0 << 17)
# define GEN7_WM_POSITION_ZW_CENTROID                           (2 << 17)
# define GEN7_WM_POSITION_ZW_SAMPLE                             (3 << 17)
# define GEN7_WM_NONPERSPECTIVE_SAMPLE_BARYCENTRIC              (1 << 16)
# define GEN7_WM_NONPERSPECTIVE_CENTROID_BARYCENTRIC            (1 << 15)
# define GEN7_WM_NONPERSPECTIVE_PIXEL_BARYCENTRIC               (1 << 14)
# define GEN7_WM_PERSPECTIVE_SAMPLE_BARYCENTRIC                 (1 << 13)
# define GEN7_WM_PERSPECTIVE_CENTROID_BARYCENTRIC               (1 << 12)
# define GEN7_WM_PERSPECTIVE_PIXEL_BARYCENTRIC                  (1 << 11)
# define GEN7_WM_USES_INPUT_COVERAGE_MASK                       (1 << 10)
# define GEN7_WM_LINE_END_CAP_AA_WIDTH_0_5                      (0 << 8)
# define GEN7_WM_LINE_END_CAP_AA_WIDTH_1_0                      (1 << 8)
# define GEN7_WM_LINE_END_CAP_AA_WIDTH_2_0                      (2 << 8)
# define GEN7_WM_LINE_END_CAP_AA_WIDTH_4_0                      (3 << 8)
# define GEN7_WM_LINE_AA_WIDTH_0_5                              (0 << 6)
# define GEN7_WM_LINE_AA_WIDTH_1_0                              (1 << 6)
# define GEN7_WM_LINE_AA_WIDTH_2_0                              (2 << 6)
# define GEN7_WM_LINE_AA_WIDTH_4_0                              (3 << 6)
# define GEN7_WM_POLYGON_STIPPLE_ENABLE                         (1 << 4)
# define GEN7_WM_LINE_STIPPLE_ENABLE                            (1 << 3)
# define GEN7_WM_POINT_RASTRULE_UPPER_RIGHT                     (1 << 2)
# define GEN7_WM_MSRAST_OFF_PIXEL                               (0 << 0)
# define GEN7_WM_MSRAST_OFF_PATTERN                             (1 << 0)
# define GEN7_WM_MSRAST_ON_PIXEL                                (2 << 0)
# define GEN7_WM_MSRAST_ON_PATTERN                              (3 << 0)
/* DW2 */
# define GEN7_WM_MSDISPMODE_PERPIXEL                            (1 << 31)

#define GEN6_3DSTATE_CONSTANT_VS		CMD(3, 0, 0x15)
#define GEN6_3DSTATE_CONSTANT_GS          	CMD(3, 0, 0x16)
#define GEN6_3DSTATE_CONSTANT_PS          	CMD(3, 0, 0x17)

/* Gen8 WM_HZ_OP */
#define GEN8_3DSTATE_WM_HZ_OP			CMD(3, 0, 0x52)


# define GEN6_3DSTATE_CONSTANT_BUFFER_3_ENABLE  (1 << 15)
# define GEN6_3DSTATE_CONSTANT_BUFFER_2_ENABLE  (1 << 14)
# define GEN6_3DSTATE_CONSTANT_BUFFER_1_ENABLE  (1 << 13)
# define GEN6_3DSTATE_CONSTANT_BUFFER_0_ENABLE  (1 << 12)

#define GEN6_3DSTATE_SAMPLE_MASK		CMD(3, 0, 0x18)

#define GEN6_3DSTATE_MULTISAMPLE		CMD(3, 1, 0x0d)
/* DW1 */
# define GEN6_3DSTATE_MULTISAMPLE_PIXEL_LOCATION_CENTER         (0 << 4)
# define GEN6_3DSTATE_MULTISAMPLE_PIXEL_LOCATION_UPPER_LEFT     (1 << 4)
# define GEN6_3DSTATE_MULTISAMPLE_NUMSAMPLES_1                  (0 << 1)
# define GEN6_3DSTATE_MULTISAMPLE_NUMSAMPLES_4                  (2 << 1)
# define GEN6_3DSTATE_MULTISAMPLE_NUMSAMPLES_8                  (3 << 1)

#define GEN8_3DSTATE_MULTISAMPLE		CMD(3, 0, 0x0d)
#define GEN8_3DSTATE_SAMPLE_PATTERN		CMD(3, 1, 0x1C)

/* GEN7 */
#define GEN7_3DSTATE_CLEAR_PARAMS               CMD(3, 0, 0x04)
#define GEN7_3DSTATE_DEPTH_BUFFER               CMD(3, 0, 0x05)
#define GEN7_3DSTATE_HIER_DEPTH_BUFFER		CMD(3, 0, 0x07)

#define GEN7_3DSTATE_URB_VS                     CMD(3, 0, 0x30)
#define GEN7_3DSTATE_URB_HS                     CMD(3, 0, 0x31)
#define GEN7_3DSTATE_URB_DS                     CMD(3, 0, 0x32)
#define GEN7_3DSTATE_URB_GS                     CMD(3, 0, 0x33)
/* DW1 */
# define GEN7_URB_ENTRY_NUMBER_SHIFT            0
# define GEN7_URB_ENTRY_SIZE_SHIFT              16
# define GEN7_URB_STARTING_ADDRESS_SHIFT        25

#define GEN7_3DSTATE_PUSH_CONSTANT_ALLOC_VS     CMD(3, 1, 0x12)
#define GEN7_3DSTATE_PUSH_CONSTANT_ALLOC_PS     CMD(3, 1, 0x16)

#define GEN7_3DSTATE_PUSH_CONSTANT_ALLOC_DS     CMD(3, 1, 0x14)
#define GEN7_3DSTATE_PUSH_CONSTANT_ALLOC_HS     CMD(3, 1, 0x13)
#define GEN7_3DSTATE_PUSH_CONSTANT_ALLOC_GS     CMD(3, 1, 0x15)
/* DW1 */
# define GEN7_PUSH_CONSTANT_BUFFER_OFFSET_SHIFT 16
# define GEN8_PUSH_CONSTANT_BUFFER_OFFSET_SHIFT	16
# define GEN8_PUSH_CONSTANT_BUFFER_SIZE_SHIFT	0

#define GEN7_3DSTATE_CONSTANT_HS                CMD(3, 0, 0x19)
#define GEN7_3DSTATE_CONSTANT_DS                CMD(3, 0, 0x1a)

#define GEN7_3DSTATE_HS                         CMD(3, 0, 0x1b)
#define GEN7_3DSTATE_TE                         CMD(3, 0, 0x1c)
#define GEN7_3DSTATE_DS                         CMD(3, 0, 0x1d)
#define GEN7_3DSTATE_STREAMOUT                  CMD(3, 0, 0x1e)
#define GEN7_3DSTATE_SBE                        CMD(3, 0, 0x1f)

/* DW1 */
# define GEN7_SBE_SWIZZLE_CONTROL_MODE          (1 << 28)
# define GEN7_SBE_NUM_OUTPUTS_SHIFT             22
# define GEN7_SBE_SWIZZLE_ENABLE                (1 << 21)
# define GEN7_SBE_POINT_SPRITE_LOWERLEFT        (1 << 20)
# define GEN7_SBE_URB_ENTRY_READ_LENGTH_SHIFT   11
# define GEN7_SBE_URB_ENTRY_READ_OFFSET_SHIFT   4
# define GEN8_SBE_FORCE_URB_ENTRY_READ_LENGTH  (1 << 29)
# define GEN8_SBE_FORCE_URB_ENTRY_READ_OFFSET  (1 << 28)

# define GEN8_SBE_URB_ENTRY_READ_OFFSET_SHIFT   5
#define GEN8_3DSTATE_SBE_SWIZ                    CMD(3, 0, 0x51)

#define GEN7_3DSTATE_PS                                 CMD(3, 0, 0x20)
/* DW1: kernel pointer */
/* DW2 */
# define GEN7_PS_SPF_MODE                               (1 << 31)
# define GEN7_PS_VECTOR_MASK_ENABLE                     (1 << 30)
# define GEN7_PS_SAMPLER_COUNT_SHIFT                    27
# define GEN7_PS_BINDING_TABLE_ENTRY_COUNT_SHIFT        18
# define GEN7_PS_FLOATING_POINT_MODE_IEEE_754           (0 << 16)
# define GEN7_PS_FLOATING_POINT_MODE_ALT                (1 << 16)
/* DW3: scratch space */
/* DW4 */
# define GEN7_PS_MAX_THREADS_SHIFT_IVB                  24
# define GEN7_PS_MAX_THREADS_SHIFT_HSW                  23
# define GEN7_PS_SAMPLE_MASK_SHIFT_HSW                  12
# define GEN7_PS_PUSH_CONSTANT_ENABLE                   (1 << 11)
# define GEN7_PS_ATTRIBUTE_ENABLE                       (1 << 10)
# define GEN7_PS_OMASK_TO_RENDER_TARGET                 (1 << 9)
# define GEN7_PS_DUAL_SOURCE_BLEND_ENABLE               (1 << 7)
# define GEN7_PS_POSOFFSET_NONE                         (0 << 3)
# define GEN7_PS_POSOFFSET_CENTROID                     (2 << 3)
# define GEN7_PS_POSOFFSET_SAMPLE                       (3 << 3)
# define GEN7_PS_32_DISPATCH_ENABLE                     (1 << 2)
# define GEN7_PS_16_DISPATCH_ENABLE                     (1 << 1)
# define GEN7_PS_8_DISPATCH_ENABLE                      (1 << 0)
/* DW5 */
# define GEN7_PS_DISPATCH_START_GRF_SHIFT_0             16
# define GEN7_PS_DISPATCH_START_GRF_SHIFT_1             8
# define GEN7_PS_DISPATCH_START_GRF_SHIFT_2             0
/* DW6: kernel 1 pointer */
/* DW7: kernel 2 pointer */

# define GEN8_PS_MAX_THREADS_SHIFT                      23

#define GEN8_3DSTATE_PSEXTRA				CMD(3, 0, 0x4f)
/* DW1 */
# define GEN8_PSX_PIXEL_SHADER_VALID                    (1 << 31)
# define GEN8_PSX_PSCDEPTH_OFF                          (0 << 26)
# define GEN8_PSX_PSCDEPTH_ON                           (1 << 26)
# define GEN8_PSX_PSCDEPTH_ON_GE                        (2 << 26)
# define GEN8_PSX_PSCDEPTH_ON_LE                        (3 << 26)
# define GEN8_PSX_ATTRIBUTE_ENABLE			(1 << 8)

#define GEN8_3DSTATE_PSBLEND				CMD(3, 0, 0x4d)
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


#define GEN7_3DSTATE_STENCIL_BUFFER			CMD(3, 0, 0x06)
#define GEN8_3DSTATE_WM_DEPTH_STENCIL			CMD(3, 0, 0x4e)

#define GEN7_3DSTATE_VIEWPORT_STATE_POINTERS_SF_CL      CMD(3, 0, 0x21)
#define GEN7_3DSTATE_VIEWPORT_STATE_POINTERS_CC         CMD(3, 0, 0x23)

#define GEN7_3DSTATE_BLEND_STATE_POINTERS               CMD(3, 0, 0x24)
#define GEN7_3DSTATE_DEPTH_STENCIL_STATE_POINTERS       CMD(3, 0, 0x25)

#define GEN7_3DSTATE_BINDING_TABLE_POINTERS_VS          CMD(3, 0, 0x26)
#define GEN7_3DSTATE_BINDING_TABLE_POINTERS_HS          CMD(3, 0, 0x27)
#define GEN7_3DSTATE_BINDING_TABLE_POINTERS_DS          CMD(3, 0, 0x28)
#define GEN7_3DSTATE_BINDING_TABLE_POINTERS_GS          CMD(3, 0, 0x29)
#define GEN7_3DSTATE_BINDING_TABLE_POINTERS_PS          CMD(3, 0, 0x2a)

#define GEN7_3DSTATE_SAMPLER_STATE_POINTERS_VS          CMD(3, 0, 0x2b)
#define GEN7_3DSTATE_SAMPLER_STATE_POINTERS_GS          CMD(3, 0, 0x2e)
#define GEN7_3DSTATE_SAMPLER_STATE_POINTERS_PS          CMD(3, 0, 0x2f)
#define GEN7_3DSTATE_SAMPLER_STATE_POINTERS_HS          CMD(3, 0, 0x2c)
#define GEN7_3DSTATE_SAMPLER_STATE_POINTERS_DS          CMD(3, 0, 0x2d)

#define MFX(pipeline, op, sub_opa, sub_opb)     \
    (3 << 29 |                                  \
     (pipeline) << 27 |                         \
     (op) << 24 |                               \
     (sub_opa) << 21 |                          \
     (sub_opb) << 16)

#define MFX_PIPE_MODE_SELECT                    MFX(2, 0, 0, 0)
#define MFX_SURFACE_STATE                       MFX(2, 0, 0, 1)
#define MFX_PIPE_BUF_ADDR_STATE                 MFX(2, 0, 0, 2)
#define MFX_IND_OBJ_BASE_ADDR_STATE             MFX(2, 0, 0, 3)
#define MFX_BSP_BUF_BASE_ADDR_STATE             MFX(2, 0, 0, 4)
#define MFX_AES_STATE                           MFX(2, 0, 0, 5)
#define MFX_STATE_POINTER                       MFX(2, 0, 0, 6)
#define MFX_QM_STATE                            MFX(2, 0, 0, 7)
#define MFX_FQM_STATE                           MFX(2, 0, 0, 8)

#define MFX_INSERT_OBJECT                       MFX(2, 0, 2, 8)

#define MFX_WAIT                                MFX(1, 0, 0, 0)

#define MFX_AVC_IMG_STATE                       MFX(2, 1, 0, 0)
#define MFX_AVC_QM_STATE                        MFX(2, 1, 0, 1)
#define MFX_AVC_DIRECTMODE_STATE                MFX(2, 1, 0, 2)
#define MFX_AVC_SLICE_STATE                     MFX(2, 1, 0, 3)
#define MFX_AVC_REF_IDX_STATE                   MFX(2, 1, 0, 4)
#define MFX_AVC_WEIGHTOFFSET_STATE              MFX(2, 1, 0, 5)

#define MFD_AVC_PICID_STATE                     MFX(2, 1, 1, 5)
#define MFD_AVC_BSD_OBJECT                      MFX(2, 1, 1, 8)

#define MFC_AVC_FQM_STATE                       MFX(2, 1, 2, 2)
#define MFC_AVC_INSERT_OBJECT                   MFX(2, 1, 2, 8)
#define MFC_AVC_PAK_OBJECT                      MFX(2, 1, 2, 9)

#define MFX_MPEG2_PIC_STATE                     MFX(2, 3, 0, 0)
#define MFX_MPEG2_QM_STATE                      MFX(2, 3, 0, 1)

#define MFD_MPEG2_BSD_OBJECT                    MFX(2, 3, 1, 8)

#define MFC_MPEG2_SLICEGROUP_STATE              MFX(2, 3, 2, 3)
#define MFC_MPEG2_PAK_OBJECT                    MFX(2, 3, 2, 9)

#define MFX_VC1_PIC_STATE                       MFX(2, 2, 0, 0)
#define MFX_VC1_PRED_PIPE_STATE                 MFX(2, 2, 0, 1)
#define MFX_VC1_DIRECTMODE_STATE                MFX(2, 2, 0, 2)

#define MFD_VC1_SHORT_PIC_STATE                 MFX(2, 2, 1, 0)
#define MFD_VC1_LONG_PIC_STATE                  MFX(2, 2, 1, 1)

#define MFD_VC1_BSD_OBJECT                      MFX(2, 2, 1, 8)

#define MFX_JPEG_PIC_STATE                      MFX(2, 7, 0, 0)
#define MFX_JPEG_HUFF_TABLE_STATE               MFX(2, 7, 0, 2)

#define MFD_JPEG_BSD_OBJECT                     MFX(2, 7, 1, 8)

#define MFX_VP8_PIC_STATE                       MFX(2, 4, 0, 0)

#define MFD_VP8_BSD_OBJECT                      MFX(2, 4, 1, 8)


#define VEB(pipeline, op, sub_opa, sub_opb)     \
     (3 << 29 |                                 \
     (pipeline) << 27 |                         \
     (op) << 24 |                               \
     (sub_opa) << 21 |                          \
     (sub_opb) << 16)

#define VEB_SURFACE_STATE                       VEB(2, 4, 0, 0)
#define VEB_STATE                               VEB(2, 4, 0, 2)
#define VEB_DNDI_IECP_STATE                     VEB(2, 4, 0, 3)

#define I965_DEPTHFORMAT_D32_FLOAT              1

#define BASE_ADDRESS_MODIFY             (1 << 0)

#define PIPELINE_SELECT_3D              0
#define PIPELINE_SELECT_MEDIA           1


#define UF0_CS_REALLOC                  (1 << 13)
#define UF0_VFE_REALLOC                 (1 << 12)
#define UF0_SF_REALLOC                  (1 << 11)
#define UF0_CLIP_REALLOC                (1 << 10)
#define UF0_GS_REALLOC                  (1 << 9)
#define UF0_VS_REALLOC                  (1 << 8)
#define UF1_CLIP_FENCE_SHIFT            20
#define UF1_GS_FENCE_SHIFT              10
#define UF1_VS_FENCE_SHIFT              0
#define UF2_CS_FENCE_SHIFT              20
#define UF2_VFE_FENCE_SHIFT             10
#define UF2_SF_FENCE_SHIFT              0

#define VFE_GENERIC_MODE        0x0
#define VFE_VLD_MODE            0x1
#define VFE_IS_MODE             0x2
#define VFE_AVC_MC_MODE         0x4
#define VFE_AVC_IT_MODE         0x7

#define FLOATING_POINT_IEEE_754        0
#define FLOATING_POINT_NON_IEEE_754    1


#define I965_SURFACE_1D      0
#define I965_SURFACE_2D      1
#define I965_SURFACE_3D      2
#define I965_SURFACE_CUBE    3
#define I965_SURFACE_BUFFER  4
#define I965_SURFACE_NULL    7

#define I965_SURFACEFORMAT_R32G32B32A32_FLOAT             0x000 
#define I965_SURFACEFORMAT_R32G32B32A32_SINT              0x001 
#define I965_SURFACEFORMAT_R32G32B32A32_UINT              0x002 
#define I965_SURFACEFORMAT_R32G32B32A32_UNORM             0x003 
#define I965_SURFACEFORMAT_R32G32B32A32_SNORM             0x004 
#define I965_SURFACEFORMAT_R64G64_FLOAT                   0x005 
#define I965_SURFACEFORMAT_R32G32B32X32_FLOAT             0x006 
#define I965_SURFACEFORMAT_R32G32B32A32_SSCALED           0x007
#define I965_SURFACEFORMAT_R32G32B32A32_USCALED           0x008
#define I965_SURFACEFORMAT_R32G32B32_FLOAT                0x040 
#define I965_SURFACEFORMAT_R32G32B32_SINT                 0x041 
#define I965_SURFACEFORMAT_R32G32B32_UINT                 0x042 
#define I965_SURFACEFORMAT_R32G32B32_UNORM                0x043 
#define I965_SURFACEFORMAT_R32G32B32_SNORM                0x044 
#define I965_SURFACEFORMAT_R32G32B32_SSCALED              0x045 
#define I965_SURFACEFORMAT_R32G32B32_USCALED              0x046 
#define I965_SURFACEFORMAT_R16G16B16A16_UNORM             0x080 
#define I965_SURFACEFORMAT_R16G16B16A16_SNORM             0x081 
#define I965_SURFACEFORMAT_R16G16B16A16_SINT              0x082 
#define I965_SURFACEFORMAT_R16G16B16A16_UINT              0x083 
#define I965_SURFACEFORMAT_R16G16B16A16_FLOAT             0x084 
#define I965_SURFACEFORMAT_R32G32_FLOAT                   0x085 
#define I965_SURFACEFORMAT_R32G32_SINT                    0x086 
#define I965_SURFACEFORMAT_R32G32_UINT                    0x087 
#define I965_SURFACEFORMAT_R32_FLOAT_X8X24_TYPELESS       0x088 
#define I965_SURFACEFORMAT_X32_TYPELESS_G8X24_UINT        0x089 
#define I965_SURFACEFORMAT_L32A32_FLOAT                   0x08A 
#define I965_SURFACEFORMAT_R32G32_UNORM                   0x08B 
#define I965_SURFACEFORMAT_R32G32_SNORM                   0x08C 
#define I965_SURFACEFORMAT_R64_FLOAT                      0x08D 
#define I965_SURFACEFORMAT_R16G16B16X16_UNORM             0x08E 
#define I965_SURFACEFORMAT_R16G16B16X16_FLOAT             0x08F 
#define I965_SURFACEFORMAT_A32X32_FLOAT                   0x090 
#define I965_SURFACEFORMAT_L32X32_FLOAT                   0x091 
#define I965_SURFACEFORMAT_I32X32_FLOAT                   0x092 
#define I965_SURFACEFORMAT_R16G16B16A16_SSCALED           0x093
#define I965_SURFACEFORMAT_R16G16B16A16_USCALED           0x094
#define I965_SURFACEFORMAT_R32G32_SSCALED                 0x095
#define I965_SURFACEFORMAT_R32G32_USCALED                 0x096
#define I965_SURFACEFORMAT_B8G8R8A8_UNORM                 0x0C0 
#define I965_SURFACEFORMAT_B8G8R8A8_UNORM_SRGB            0x0C1 
#define I965_SURFACEFORMAT_R10G10B10A2_UNORM              0x0C2 
#define I965_SURFACEFORMAT_R10G10B10A2_UNORM_SRGB         0x0C3 
#define I965_SURFACEFORMAT_R10G10B10A2_UINT               0x0C4 
#define I965_SURFACEFORMAT_R10G10B10_SNORM_A2_UNORM       0x0C5 
#define I965_SURFACEFORMAT_R8G8B8A8_UNORM                 0x0C7 
#define I965_SURFACEFORMAT_R8G8B8A8_UNORM_SRGB            0x0C8 
#define I965_SURFACEFORMAT_R8G8B8A8_SNORM                 0x0C9 
#define I965_SURFACEFORMAT_R8G8B8A8_SINT                  0x0CA 
#define I965_SURFACEFORMAT_R8G8B8A8_UINT                  0x0CB 
#define I965_SURFACEFORMAT_R16G16_UNORM                   0x0CC 
#define I965_SURFACEFORMAT_R16G16_SNORM                   0x0CD 
#define I965_SURFACEFORMAT_R16G16_SINT                    0x0CE 
#define I965_SURFACEFORMAT_R16G16_UINT                    0x0CF 
#define I965_SURFACEFORMAT_R16G16_FLOAT                   0x0D0 
#define I965_SURFACEFORMAT_B10G10R10A2_UNORM              0x0D1 
#define I965_SURFACEFORMAT_B10G10R10A2_UNORM_SRGB         0x0D2 
#define I965_SURFACEFORMAT_R11G11B10_FLOAT                0x0D3 
#define I965_SURFACEFORMAT_R32_SINT                       0x0D6 
#define I965_SURFACEFORMAT_R32_UINT                       0x0D7 
#define I965_SURFACEFORMAT_R32_FLOAT                      0x0D8 
#define I965_SURFACEFORMAT_R24_UNORM_X8_TYPELESS          0x0D9 
#define I965_SURFACEFORMAT_X24_TYPELESS_G8_UINT           0x0DA 
#define I965_SURFACEFORMAT_L16A16_UNORM                   0x0DF 
#define I965_SURFACEFORMAT_I24X8_UNORM                    0x0E0 
#define I965_SURFACEFORMAT_L24X8_UNORM                    0x0E1 
#define I965_SURFACEFORMAT_A24X8_UNORM                    0x0E2 
#define I965_SURFACEFORMAT_I32_FLOAT                      0x0E3 
#define I965_SURFACEFORMAT_L32_FLOAT                      0x0E4 
#define I965_SURFACEFORMAT_A32_FLOAT                      0x0E5 
#define I965_SURFACEFORMAT_B8G8R8X8_UNORM                 0x0E9 
#define I965_SURFACEFORMAT_B8G8R8X8_UNORM_SRGB            0x0EA 
#define I965_SURFACEFORMAT_R8G8B8X8_UNORM                 0x0EB 
#define I965_SURFACEFORMAT_R8G8B8X8_UNORM_SRGB            0x0EC 
#define I965_SURFACEFORMAT_R9G9B9E5_SHAREDEXP             0x0ED 
#define I965_SURFACEFORMAT_B10G10R10X2_UNORM              0x0EE 
#define I965_SURFACEFORMAT_L16A16_FLOAT                   0x0F0 
#define I965_SURFACEFORMAT_R32_UNORM                      0x0F1 
#define I965_SURFACEFORMAT_R32_SNORM                      0x0F2 
#define I965_SURFACEFORMAT_R10G10B10X2_USCALED            0x0F3
#define I965_SURFACEFORMAT_R8G8B8A8_SSCALED               0x0F4
#define I965_SURFACEFORMAT_R8G8B8A8_USCALED               0x0F5
#define I965_SURFACEFORMAT_R16G16_SSCALED                 0x0F6
#define I965_SURFACEFORMAT_R16G16_USCALED                 0x0F7
#define I965_SURFACEFORMAT_R32_SSCALED                    0x0F8
#define I965_SURFACEFORMAT_R32_USCALED                    0x0F9
#define I965_SURFACEFORMAT_B5G6R5_UNORM                   0x100 
#define I965_SURFACEFORMAT_B5G6R5_UNORM_SRGB              0x101 
#define I965_SURFACEFORMAT_B5G5R5A1_UNORM                 0x102 
#define I965_SURFACEFORMAT_B5G5R5A1_UNORM_SRGB            0x103 
#define I965_SURFACEFORMAT_B4G4R4A4_UNORM                 0x104 
#define I965_SURFACEFORMAT_B4G4R4A4_UNORM_SRGB            0x105 
#define I965_SURFACEFORMAT_R8G8_UNORM                     0x106 
#define I965_SURFACEFORMAT_R8G8_SNORM                     0x107 
#define I965_SURFACEFORMAT_R8G8_SINT                      0x108 
#define I965_SURFACEFORMAT_R8G8_UINT                      0x109 
#define I965_SURFACEFORMAT_R16_UNORM                      0x10A 
#define I965_SURFACEFORMAT_R16_SNORM                      0x10B 
#define I965_SURFACEFORMAT_R16_SINT                       0x10C 
#define I965_SURFACEFORMAT_R16_UINT                       0x10D 
#define I965_SURFACEFORMAT_R16_FLOAT                      0x10E 
#define I965_SURFACEFORMAT_I16_UNORM                      0x111 
#define I965_SURFACEFORMAT_L16_UNORM                      0x112 
#define I965_SURFACEFORMAT_A16_UNORM                      0x113 
#define I965_SURFACEFORMAT_L8A8_UNORM                     0x114 
#define I965_SURFACEFORMAT_I16_FLOAT                      0x115
#define I965_SURFACEFORMAT_L16_FLOAT                      0x116
#define I965_SURFACEFORMAT_A16_FLOAT                      0x117 
#define I965_SURFACEFORMAT_R5G5_SNORM_B6_UNORM            0x119 
#define I965_SURFACEFORMAT_B5G5R5X1_UNORM                 0x11A 
#define I965_SURFACEFORMAT_B5G5R5X1_UNORM_SRGB            0x11B
#define I965_SURFACEFORMAT_R8G8_SSCALED                   0x11C
#define I965_SURFACEFORMAT_R8G8_USCALED                   0x11D
#define I965_SURFACEFORMAT_R16_SSCALED                    0x11E
#define I965_SURFACEFORMAT_R16_USCALED                    0x11F
#define I965_SURFACEFORMAT_P8A8_UNORM                     0x122 
#define I965_SURFACEFORMAT_A8P8_UNORM                     0x123
#define I965_SURFACEFORMAT_R8_UNORM                       0x140 
#define I965_SURFACEFORMAT_R8_SNORM                       0x141 
#define I965_SURFACEFORMAT_R8_SINT                        0x142 
#define I965_SURFACEFORMAT_R8_UINT                        0x143 
#define I965_SURFACEFORMAT_A8_UNORM                       0x144 
#define I965_SURFACEFORMAT_I8_UNORM                       0x145 
#define I965_SURFACEFORMAT_L8_UNORM                       0x146 
#define I965_SURFACEFORMAT_P4A4_UNORM                     0x147 
#define I965_SURFACEFORMAT_A4P4_UNORM                     0x148
#define I965_SURFACEFORMAT_R8_SSCALED                     0x149
#define I965_SURFACEFORMAT_R8_USCALED                     0x14A
#define I965_SURFACEFORMAT_R1_UINT                        0x181 
#define I965_SURFACEFORMAT_YCRCB_NORMAL                   0x182 
#define I965_SURFACEFORMAT_YCRCB_SWAPUVY                  0x183 
#define I965_SURFACEFORMAT_BC1_UNORM                      0x186 
#define I965_SURFACEFORMAT_BC2_UNORM                      0x187 
#define I965_SURFACEFORMAT_BC3_UNORM                      0x188 
#define I965_SURFACEFORMAT_BC4_UNORM                      0x189 
#define I965_SURFACEFORMAT_BC5_UNORM                      0x18A 
#define I965_SURFACEFORMAT_BC1_UNORM_SRGB                 0x18B 
#define I965_SURFACEFORMAT_BC2_UNORM_SRGB                 0x18C 
#define I965_SURFACEFORMAT_BC3_UNORM_SRGB                 0x18D 
#define I965_SURFACEFORMAT_MONO8                          0x18E 
#define I965_SURFACEFORMAT_YCRCB_SWAPUV                   0x18F 
#define I965_SURFACEFORMAT_YCRCB_SWAPY                    0x190 
#define I965_SURFACEFORMAT_DXT1_RGB                       0x191 
#define I965_SURFACEFORMAT_FXT1                           0x192 
#define I965_SURFACEFORMAT_R8G8B8_UNORM                   0x193 
#define I965_SURFACEFORMAT_R8G8B8_SNORM                   0x194 
#define I965_SURFACEFORMAT_R8G8B8_SSCALED                 0x195 
#define I965_SURFACEFORMAT_R8G8B8_USCALED                 0x196 
#define I965_SURFACEFORMAT_R64G64B64A64_FLOAT             0x197 
#define I965_SURFACEFORMAT_R64G64B64_FLOAT                0x198 
#define I965_SURFACEFORMAT_BC4_SNORM                      0x199 
#define I965_SURFACEFORMAT_BC5_SNORM                      0x19A 
#define I965_SURFACEFORMAT_R16G16B16_UNORM                0x19C 
#define I965_SURFACEFORMAT_R16G16B16_SNORM                0x19D 
#define I965_SURFACEFORMAT_R16G16B16_SSCALED              0x19E 
#define I965_SURFACEFORMAT_R16G16B16_USCALED              0x19F

#define I965_CULLMODE_BOTH      0
#define I965_CULLMODE_NONE      1
#define I965_CULLMODE_FRONT     2
#define I965_CULLMODE_BACK      3

#define I965_MAPFILTER_NEAREST        0x0 
#define I965_MAPFILTER_LINEAR         0x1 
#define I965_MAPFILTER_ANISOTROPIC    0x2

#define I965_MIPFILTER_NONE        0   
#define I965_MIPFILTER_NEAREST     1   
#define I965_MIPFILTER_LINEAR      3

#define HSW_SCS_ZERO                      0
#define HSW_SCS_ONE                       1
#define HSW_SCS_RED                       4
#define HSW_SCS_GREEN                     5
#define HSW_SCS_BLUE                      6
#define HSW_SCS_ALPHA                     7

#define I965_TEXCOORDMODE_WRAP            0
#define I965_TEXCOORDMODE_MIRROR          1
#define I965_TEXCOORDMODE_CLAMP           2
#define I965_TEXCOORDMODE_CUBE            3
#define I965_TEXCOORDMODE_CLAMP_BORDER    4
#define I965_TEXCOORDMODE_MIRROR_ONCE     5

#define I965_BLENDFACTOR_ONE                 0x1
#define I965_BLENDFACTOR_SRC_COLOR           0x2
#define I965_BLENDFACTOR_SRC_ALPHA           0x3
#define I965_BLENDFACTOR_DST_ALPHA           0x4
#define I965_BLENDFACTOR_DST_COLOR           0x5
#define I965_BLENDFACTOR_SRC_ALPHA_SATURATE  0x6
#define I965_BLENDFACTOR_CONST_COLOR         0x7
#define I965_BLENDFACTOR_CONST_ALPHA         0x8
#define I965_BLENDFACTOR_SRC1_COLOR          0x9
#define I965_BLENDFACTOR_SRC1_ALPHA          0x0A
#define I965_BLENDFACTOR_ZERO                0x11
#define I965_BLENDFACTOR_INV_SRC_COLOR       0x12
#define I965_BLENDFACTOR_INV_SRC_ALPHA       0x13
#define I965_BLENDFACTOR_INV_DST_ALPHA       0x14
#define I965_BLENDFACTOR_INV_DST_COLOR       0x15
#define I965_BLENDFACTOR_INV_CONST_COLOR     0x17
#define I965_BLENDFACTOR_INV_CONST_ALPHA     0x18
#define I965_BLENDFACTOR_INV_SRC1_COLOR      0x19
#define I965_BLENDFACTOR_INV_SRC1_ALPHA      0x1A

#define I965_BLENDFUNCTION_ADD               0
#define I965_BLENDFUNCTION_SUBTRACT          1
#define I965_BLENDFUNCTION_REVERSE_SUBTRACT  2
#define I965_BLENDFUNCTION_MIN               3
#define I965_BLENDFUNCTION_MAX               4

#define I965_SURFACERETURNFORMAT_FLOAT32  0
#define I965_SURFACERETURNFORMAT_S1       1

#define I965_VFCOMPONENT_NOSTORE      0
#define I965_VFCOMPONENT_STORE_SRC    1
#define I965_VFCOMPONENT_STORE_0      2
#define I965_VFCOMPONENT_STORE_1_FLT  3
#define I965_VFCOMPONENT_STORE_1_INT  4
#define I965_VFCOMPONENT_STORE_VID    5
#define I965_VFCOMPONENT_STORE_IID    6
#define I965_VFCOMPONENT_STORE_PID    7

#define VE0_VERTEX_BUFFER_INDEX_SHIFT	27
#define GEN6_VE0_VERTEX_BUFFER_INDEX_SHIFT      26 /* for GEN6 */
#define VE0_VALID			(1 << 26)
#define GEN6_VE0_VALID                  (1 << 25) /* for GEN6 */
#define VE0_FORMAT_SHIFT		16
#define VE0_OFFSET_SHIFT		0
#define VE1_VFCOMPONENT_0_SHIFT		28
#define VE1_VFCOMPONENT_1_SHIFT		24
#define VE1_VFCOMPONENT_2_SHIFT		20
#define VE1_VFCOMPONENT_3_SHIFT		16
#define VE1_DESTINATION_ELEMENT_OFFSET_SHIFT	0
#define GEN8_VE0_VERTEX_BUFFER_INDEX_SHIFT      26 /* for GEN8 */
#define GEN8_VE0_VALID                  (1 << 25)  /* for GEN8 */

#define VB0_BUFFER_INDEX_SHIFT          27
#define GEN6_VB0_BUFFER_INDEX_SHIFT     26
#define VB0_VERTEXDATA                  (0 << 26)
#define VB0_INSTANCEDATA                (1 << 26)
#define GEN6_VB0_VERTEXDATA             (0 << 20)
#define GEN6_VB0_INSTANCEDATA           (1 << 20)
#define GEN7_VB0_ADDRESS_MODIFYENABLE   (1 << 14)
#define VB0_BUFFER_PITCH_SHIFT          0
#define GEN8_VB0_BUFFER_INDEX_SHIFT     26
#define GEN8_VB0_MOCS_SHIFT		16

#define _3DPRIMITIVE_VERTEX_SEQUENTIAL  (0 << 15)
#define _3DPRIMITIVE_VERTEX_RANDOM      (1 << 15)
#define _3DPRIMITIVE_TOPOLOGY_SHIFT     10
/* DW1 on GEN7*/
# define GEN7_3DPRIM_VERTEXBUFFER_ACCESS_SEQUENTIAL     (0 << 8)
# define GEN7_3DPRIM_VERTEXBUFFER_ACCESS_RANDOM         (1 << 8)

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

#define GEN8_3DSTATE_VF_TOPOLOGY	CMD(3, 0, 0x4b)

#define I965_TILEWALK_XMAJOR                 0
#define I965_TILEWALK_YMAJOR                 1

#define SCAN_RASTER_ORDER       0
#define SCAN_SPECIAL_ORDER      1

#define ENTROPY_CAVLD           0
#define ENTROPY_CABAC           1

#define SLICE_TYPE_P            0
#define SLICE_TYPE_B            1
#define SLICE_TYPE_I            2
#define SLICE_TYPE_SP           3
#define SLICE_TYPE_SI           4

#define PRESENT_REF_LIST0               (1 << 0)
#define PRESENT_REF_LIST1               (1 << 1)
#define PRESENT_WEIGHT_OFFSET_L0        (1 << 2)
#define PRESENT_WEIGHT_OFFSET_L1        (1 << 3)

#define RESIDUAL_DATA_OFFSET    48

#define PRESENT_NOMV            0
#define PRESENT_NOWO            1
#define PRESENT_MV_WO           3

#define SCOREBOARD_STALLING     0
#define SCOREBOARD_NON_STALLING 1

#define SURFACE_FORMAT_YCRCB_NORMAL     0
#define SURFACE_FORMAT_YCRCB_SWAPUVY    1
#define SURFACE_FORMAT_YCRCB_SWAPUV     2
#define SURFACE_FORMAT_YCRCB_SWAPY      3
#define SURFACE_FORMAT_PLANAR_420_8     4
#define SURFACE_FORMAT_PLANAR_411_8     5
#define SURFACE_FORMAT_PLANAR_422_8     6
#define SURFACE_FORMAT_STMM_DN_STATISTICS       7
#define SURFACE_FORMAT_R10G10B10A2_UNORM        8
#define SURFACE_FORMAT_R8G8B8A8_UNORM   9
#define SURFACE_FORMAT_R8B8_UNORM       10
#define SURFACE_FORMAT_R8_UNORM         11
#define SURFACE_FORMAT_Y8_UNORM         12

#define AVS_FILTER_ADAPTIVE_8_TAP       0
#define AVS_FILTER_NEAREST              1

#define IEF_FILTER_COMBO                0
#define IEF_FILTER_DETAIL               1

#define IEF_FILTER_SIZE_3X3             0
#define IEF_FILTER_SIZE_5X5             1

#define MFX_FORMAT_MPEG2        0
#define MFX_FORMAT_VC1          1
#define MFX_FORMAT_AVC          2
#define MFX_FORMAT_JPEG         3
#define MFX_FORMAT_SVC          4
#define MFX_FORMAT_VP8          5

#define MFX_SHORT_MODE          0
#define MFX_LONG_MODE           1

#define MFX_CODEC_DECODE        0
#define MFX_CODEC_ENCODE        1

#define MFX_QM_AVC_4X4_INTRA_MATRIX     0
#define MFX_QM_AVC_4X4_INTER_MATRIX     1
#define MFX_QM_AVC_8x8_INTRA_MATRIX     2
#define MFX_QM_AVC_8x8_INTER_MATRIX     3

#define MFX_QM_MPEG_INTRA_QUANTIZER_MATRIX      0
#define MFX_QM_MPEG_NON_INTRA_QUANTIZER_MATRIX  1

#define MFX_QM_JPEG_LUMA_Y_QUANTIZER_MATRIX     0
#define MFX_QM_JPEG_CHROMA_CB_QUANTIZER_MATRIX  1
#define MFX_QM_JPEG_CHROMA_CR_QUANTIZER_MATRIX  2
#define MFX_QM_JPEG_ALPHA_QUANTIZER_MATRIX      3 /* for new device */

#define MFX_HUFFTABLE_ID_Y                      0
#define MFX_HUFFTABLE_ID_UV                     1 /* UV on Ivybridge */

#define MFD_MODE_VLD            0
#define MFD_MODE_IT             1

#define MFX_SURFACE_PLANAR_420_8        4
#define MFX_SURFACE_PLANAR_411_8        5
#define MFX_SURFACE_PLANAR_422_8        6
#define MFX_SURFACE_MONOCHROME          12

#define MPEG_I_PICTURE          1
#define MPEG_P_PICTURE          2
#define MPEG_B_PICTURE          3

#define MPEG_TOP_FIELD		1
#define MPEG_BOTTOM_FIELD	2
#define MPEG_FRAME		3

#define SUBSAMPLE_YUV400        0
#define SUBSAMPLE_YUV420        1
#define SUBSAMPLE_YUV422H       2
#define SUBSAMPLE_YUV422V       3
#define SUBSAMPLE_YUV444        4
#define SUBSAMPLE_YUV411        5
#define SUBSAMPLE_RGBX          6

#endif /* _I965_DEFINES_H_ */
