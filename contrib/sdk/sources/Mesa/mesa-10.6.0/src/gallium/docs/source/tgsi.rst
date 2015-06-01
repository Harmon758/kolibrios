TGSI
====

TGSI, Tungsten Graphics Shader Infrastructure, is an intermediate language
for describing shaders. Since Gallium is inherently shaderful, shaders are
an important part of the API. TGSI is the only intermediate representation
used by all drivers.

Basics
------

All TGSI instructions, known as *opcodes*, operate on arbitrary-precision
floating-point four-component vectors. An opcode may have up to one
destination register, known as *dst*, and between zero and three source
registers, called *src0* through *src2*, or simply *src* if there is only
one.

Some instructions, like :opcode:`I2F`, permit re-interpretation of vector
components as integers. Other instructions permit using registers as
two-component vectors with double precision; see :ref:`doubleopcodes`.

When an instruction has a scalar result, the result is usually copied into
each of the components of *dst*. When this happens, the result is said to be
*replicated* to *dst*. :opcode:`RCP` is one such instruction.

Modifiers
^^^^^^^^^^^^^^^

TGSI supports modifiers on inputs (as well as saturate modifier on instructions).

For inputs which have a floating point type, both absolute value and negation
modifiers are supported (with absolute value being applied first).
TGSI_OPCODE_MOV is considered to have float input type for applying modifiers.

For inputs which have signed or unsigned type only the negate modifier is
supported.

Instruction Set
---------------

Core ISA
^^^^^^^^^^^^^^^^^^^^^^^^^

These opcodes are guaranteed to be available regardless of the driver being
used.

.. opcode:: ARL - Address Register Load

.. math::

  dst.x = (int) \lfloor src.x\rfloor

  dst.y = (int) \lfloor src.y\rfloor

  dst.z = (int) \lfloor src.z\rfloor

  dst.w = (int) \lfloor src.w\rfloor


.. opcode:: MOV - Move

.. math::

  dst.x = src.x

  dst.y = src.y

  dst.z = src.z

  dst.w = src.w


.. opcode:: LIT - Light Coefficients

.. math::

  dst.x &= 1 \\
  dst.y &= max(src.x, 0) \\
  dst.z &= (src.x > 0) ? max(src.y, 0)^{clamp(src.w, -128, 128))} : 0 \\
  dst.w &= 1


.. opcode:: RCP - Reciprocal

This instruction replicates its result.

.. math::

  dst = \frac{1}{src.x}


.. opcode:: RSQ - Reciprocal Square Root

This instruction replicates its result. The results are undefined for src <= 0.

.. math::

  dst = \frac{1}{\sqrt{src.x}}


.. opcode:: SQRT - Square Root

This instruction replicates its result. The results are undefined for src < 0.

.. math::

  dst = {\sqrt{src.x}}


.. opcode:: EXP - Approximate Exponential Base 2

.. math::

  dst.x &= 2^{\lfloor src.x\rfloor} \\
  dst.y &= src.x - \lfloor src.x\rfloor \\
  dst.z &= 2^{src.x} \\
  dst.w &= 1


.. opcode:: LOG - Approximate Logarithm Base 2

.. math::

  dst.x &= \lfloor\log_2{|src.x|}\rfloor \\
  dst.y &= \frac{|src.x|}{2^{\lfloor\log_2{|src.x|}\rfloor}} \\
  dst.z &= \log_2{|src.x|} \\
  dst.w &= 1


.. opcode:: MUL - Multiply

.. math::

  dst.x = src0.x \times src1.x

  dst.y = src0.y \times src1.y

  dst.z = src0.z \times src1.z

  dst.w = src0.w \times src1.w


.. opcode:: ADD - Add

.. math::

  dst.x = src0.x + src1.x

  dst.y = src0.y + src1.y

  dst.z = src0.z + src1.z

  dst.w = src0.w + src1.w


.. opcode:: DP3 - 3-component Dot Product

This instruction replicates its result.

.. math::

  dst = src0.x \times src1.x + src0.y \times src1.y + src0.z \times src1.z


.. opcode:: DP4 - 4-component Dot Product

This instruction replicates its result.

.. math::

  dst = src0.x \times src1.x + src0.y \times src1.y + src0.z \times src1.z + src0.w \times src1.w


.. opcode:: DST - Distance Vector

.. math::

  dst.x &= 1\\
  dst.y &= src0.y \times src1.y\\
  dst.z &= src0.z\\
  dst.w &= src1.w


.. opcode:: MIN - Minimum

.. math::

  dst.x = min(src0.x, src1.x)

  dst.y = min(src0.y, src1.y)

  dst.z = min(src0.z, src1.z)

  dst.w = min(src0.w, src1.w)


.. opcode:: MAX - Maximum

.. math::

  dst.x = max(src0.x, src1.x)

  dst.y = max(src0.y, src1.y)

  dst.z = max(src0.z, src1.z)

  dst.w = max(src0.w, src1.w)


.. opcode:: SLT - Set On Less Than

.. math::

  dst.x = (src0.x < src1.x) ? 1.0F : 0.0F

  dst.y = (src0.y < src1.y) ? 1.0F : 0.0F

  dst.z = (src0.z < src1.z) ? 1.0F : 0.0F

  dst.w = (src0.w < src1.w) ? 1.0F : 0.0F


.. opcode:: SGE - Set On Greater Equal Than

.. math::

  dst.x = (src0.x >= src1.x) ? 1.0F : 0.0F

  dst.y = (src0.y >= src1.y) ? 1.0F : 0.0F

  dst.z = (src0.z >= src1.z) ? 1.0F : 0.0F

  dst.w = (src0.w >= src1.w) ? 1.0F : 0.0F


.. opcode:: MAD - Multiply And Add

.. math::

  dst.x = src0.x \times src1.x + src2.x

  dst.y = src0.y \times src1.y + src2.y

  dst.z = src0.z \times src1.z + src2.z

  dst.w = src0.w \times src1.w + src2.w


.. opcode:: SUB - Subtract

.. math::

  dst.x = src0.x - src1.x

  dst.y = src0.y - src1.y

  dst.z = src0.z - src1.z

  dst.w = src0.w - src1.w


.. opcode:: LRP - Linear Interpolate

.. math::

  dst.x = src0.x \times src1.x + (1 - src0.x) \times src2.x

  dst.y = src0.y \times src1.y + (1 - src0.y) \times src2.y

  dst.z = src0.z \times src1.z + (1 - src0.z) \times src2.z

  dst.w = src0.w \times src1.w + (1 - src0.w) \times src2.w


.. opcode:: FMA - Fused Multiply-Add

Perform a * b + c with no intermediate rounding step.

.. math::

  dst.x = src0.x \times src1.x + src2.x

  dst.y = src0.y \times src1.y + src2.y

  dst.z = src0.z \times src1.z + src2.z

  dst.w = src0.w \times src1.w + src2.w


.. opcode:: DP2A - 2-component Dot Product And Add

.. math::

  dst.x = src0.x \times src1.x + src0.y \times src1.y + src2.x

  dst.y = src0.x \times src1.x + src0.y \times src1.y + src2.x

  dst.z = src0.x \times src1.x + src0.y \times src1.y + src2.x

  dst.w = src0.x \times src1.x + src0.y \times src1.y + src2.x


.. opcode:: FRC - Fraction

.. math::

  dst.x = src.x - \lfloor src.x\rfloor

  dst.y = src.y - \lfloor src.y\rfloor

  dst.z = src.z - \lfloor src.z\rfloor

  dst.w = src.w - \lfloor src.w\rfloor


.. opcode:: CLAMP - Clamp

.. math::

  dst.x = clamp(src0.x, src1.x, src2.x)

  dst.y = clamp(src0.y, src1.y, src2.y)

  dst.z = clamp(src0.z, src1.z, src2.z)

  dst.w = clamp(src0.w, src1.w, src2.w)


.. opcode:: FLR - Floor

.. math::

  dst.x = \lfloor src.x\rfloor

  dst.y = \lfloor src.y\rfloor

  dst.z = \lfloor src.z\rfloor

  dst.w = \lfloor src.w\rfloor


.. opcode:: ROUND - Round

.. math::

  dst.x = round(src.x)

  dst.y = round(src.y)

  dst.z = round(src.z)

  dst.w = round(src.w)


.. opcode:: EX2 - Exponential Base 2

This instruction replicates its result.

.. math::

  dst = 2^{src.x}


.. opcode:: LG2 - Logarithm Base 2

This instruction replicates its result.

.. math::

  dst = \log_2{src.x}


.. opcode:: POW - Power

This instruction replicates its result.

.. math::

  dst = src0.x^{src1.x}

.. opcode:: XPD - Cross Product

.. math::

  dst.x = src0.y \times src1.z - src1.y \times src0.z

  dst.y = src0.z \times src1.x - src1.z \times src0.x

  dst.z = src0.x \times src1.y - src1.x \times src0.y

  dst.w = 1


.. opcode:: ABS - Absolute

.. math::

  dst.x = |src.x|

  dst.y = |src.y|

  dst.z = |src.z|

  dst.w = |src.w|


.. opcode:: DPH - Homogeneous Dot Product

This instruction replicates its result.

.. math::

  dst = src0.x \times src1.x + src0.y \times src1.y + src0.z \times src1.z + src1.w


.. opcode:: COS - Cosine

This instruction replicates its result.

.. math::

  dst = \cos{src.x}


.. opcode:: DDX, DDX_FINE - Derivative Relative To X

The fine variant is only used when ``PIPE_CAP_TGSI_FS_FINE_DERIVATIVE`` is
advertised. When it is, the fine version guarantees one derivative per row
while DDX is allowed to be the same for the entire 2x2 quad.

.. math::

  dst.x = partialx(src.x)

  dst.y = partialx(src.y)

  dst.z = partialx(src.z)

  dst.w = partialx(src.w)


.. opcode:: DDY, DDY_FINE - Derivative Relative To Y

The fine variant is only used when ``PIPE_CAP_TGSI_FS_FINE_DERIVATIVE`` is
advertised. When it is, the fine version guarantees one derivative per column
while DDY is allowed to be the same for the entire 2x2 quad.

.. math::

  dst.x = partialy(src.x)

  dst.y = partialy(src.y)

  dst.z = partialy(src.z)

  dst.w = partialy(src.w)


.. opcode:: PK2H - Pack Two 16-bit Floats

  TBD


.. opcode:: PK2US - Pack Two Unsigned 16-bit Scalars

  TBD


.. opcode:: PK4B - Pack Four Signed 8-bit Scalars

  TBD


.. opcode:: PK4UB - Pack Four Unsigned 8-bit Scalars

  TBD


.. opcode:: SEQ - Set On Equal

.. math::

  dst.x = (src0.x == src1.x) ? 1.0F : 0.0F

  dst.y = (src0.y == src1.y) ? 1.0F : 0.0F

  dst.z = (src0.z == src1.z) ? 1.0F : 0.0F

  dst.w = (src0.w == src1.w) ? 1.0F : 0.0F


.. opcode:: SGT - Set On Greater Than

.. math::

  dst.x = (src0.x > src1.x) ? 1.0F : 0.0F

  dst.y = (src0.y > src1.y) ? 1.0F : 0.0F

  dst.z = (src0.z > src1.z) ? 1.0F : 0.0F

  dst.w = (src0.w > src1.w) ? 1.0F : 0.0F


.. opcode:: SIN - Sine

This instruction replicates its result.

.. math::

  dst = \sin{src.x}


.. opcode:: SLE - Set On Less Equal Than

.. math::

  dst.x = (src0.x <= src1.x) ? 1.0F : 0.0F

  dst.y = (src0.y <= src1.y) ? 1.0F : 0.0F

  dst.z = (src0.z <= src1.z) ? 1.0F : 0.0F

  dst.w = (src0.w <= src1.w) ? 1.0F : 0.0F


.. opcode:: SNE - Set On Not Equal

.. math::

  dst.x = (src0.x != src1.x) ? 1.0F : 0.0F

  dst.y = (src0.y != src1.y) ? 1.0F : 0.0F

  dst.z = (src0.z != src1.z) ? 1.0F : 0.0F

  dst.w = (src0.w != src1.w) ? 1.0F : 0.0F


.. opcode:: TEX - Texture Lookup

  for array textures src0.y contains the slice for 1D,
  and src0.z contain the slice for 2D.

  for shadow textures with no arrays (and not cube map),
  src0.z contains the reference value.

  for shadow textures with arrays, src0.z contains
  the reference value for 1D arrays, and src0.w contains
  the reference value for 2D arrays and cube maps.

  for cube map array shadow textures, the reference value
  cannot be passed in src0.w, and TEX2 must be used instead.

.. math::

  coord = src0

  shadow_ref = src0.z or src0.w (optional)

  unit = src1

  dst = texture\_sample(unit, coord, shadow_ref)


.. opcode:: TEX2 - Texture Lookup (for shadow cube map arrays only)

  this is the same as TEX, but uses another reg to encode the
  reference value.

.. math::

  coord = src0

  shadow_ref = src1.x

  unit = src2

  dst = texture\_sample(unit, coord, shadow_ref)




.. opcode:: TXD - Texture Lookup with Derivatives

.. math::

  coord = src0

  ddx = src1

  ddy = src2

  unit = src3

  dst = texture\_sample\_deriv(unit, coord, ddx, ddy)


.. opcode:: TXP - Projective Texture Lookup

.. math::

  coord.x = src0.x / src0.w

  coord.y = src0.y / src0.w

  coord.z = src0.z / src0.w

  coord.w = src0.w

  unit = src1

  dst = texture\_sample(unit, coord)


.. opcode:: UP2H - Unpack Two 16-Bit Floats

  TBD

.. note::

   Considered for removal.

.. opcode:: UP2US - Unpack Two Unsigned 16-Bit Scalars

  TBD

.. note::

   Considered for removal.

.. opcode:: UP4B - Unpack Four Signed 8-Bit Values

  TBD

.. note::

   Considered for removal.

.. opcode:: UP4UB - Unpack Four Unsigned 8-Bit Scalars

  TBD

.. note::

   Considered for removal.


.. opcode:: ARR - Address Register Load With Round

.. math::

  dst.x = (int) round(src.x)

  dst.y = (int) round(src.y)

  dst.z = (int) round(src.z)

  dst.w = (int) round(src.w)


.. opcode:: SSG - Set Sign

.. math::

  dst.x = (src.x > 0) ? 1 : (src.x < 0) ? -1 : 0

  dst.y = (src.y > 0) ? 1 : (src.y < 0) ? -1 : 0

  dst.z = (src.z > 0) ? 1 : (src.z < 0) ? -1 : 0

  dst.w = (src.w > 0) ? 1 : (src.w < 0) ? -1 : 0


.. opcode:: CMP - Compare

.. math::

  dst.x = (src0.x < 0) ? src1.x : src2.x

  dst.y = (src0.y < 0) ? src1.y : src2.y

  dst.z = (src0.z < 0) ? src1.z : src2.z

  dst.w = (src0.w < 0) ? src1.w : src2.w


.. opcode:: KILL_IF - Conditional Discard

  Conditional discard.  Allowed in fragment shaders only.

.. math::

  if (src.x < 0 || src.y < 0 || src.z < 0 || src.w < 0)
    discard
  endif


.. opcode:: KILL - Discard

  Unconditional discard.  Allowed in fragment shaders only.


.. opcode:: SCS - Sine Cosine

.. math::

  dst.x = \cos{src.x}

  dst.y = \sin{src.x}

  dst.z = 0

  dst.w = 1


.. opcode:: TXB - Texture Lookup With Bias

  for cube map array textures and shadow cube maps, the bias value
  cannot be passed in src0.w, and TXB2 must be used instead.

  if the target is a shadow texture, the reference value is always
  in src.z (this prevents shadow 3d and shadow 2d arrays from
  using this instruction, but this is not needed).

.. math::

  coord.x = src0.x

  coord.y = src0.y

  coord.z = src0.z

  coord.w = none

  bias = src0.w

  unit = src1

  dst = texture\_sample(unit, coord, bias)


.. opcode:: TXB2 - Texture Lookup With Bias (some cube maps only)

  this is the same as TXB, but uses another reg to encode the
  lod bias value for cube map arrays and shadow cube maps.
  Presumably shadow 2d arrays and shadow 3d targets could use
  this encoding too, but this is not legal.

  shadow cube map arrays are neither possible nor required.

.. math::

  coord = src0

  bias = src1.x

  unit = src2

  dst = texture\_sample(unit, coord, bias)


.. opcode:: DIV - Divide

.. math::

  dst.x = \frac{src0.x}{src1.x}

  dst.y = \frac{src0.y}{src1.y}

  dst.z = \frac{src0.z}{src1.z}

  dst.w = \frac{src0.w}{src1.w}


.. opcode:: DP2 - 2-component Dot Product

This instruction replicates its result.

.. math::

  dst = src0.x \times src1.x + src0.y \times src1.y


.. opcode:: TXL - Texture Lookup With explicit LOD

  for cube map array textures, the explicit lod value
  cannot be passed in src0.w, and TXL2 must be used instead.

  if the target is a shadow texture, the reference value is always
  in src.z (this prevents shadow 3d / 2d array / cube targets from
  using this instruction, but this is not needed).

.. math::

  coord.x = src0.x

  coord.y = src0.y

  coord.z = src0.z

  coord.w = none

  lod = src0.w

  unit = src1

  dst = texture\_sample(unit, coord, lod)


.. opcode:: TXL2 - Texture Lookup With explicit LOD (for cube map arrays only)

  this is the same as TXL, but uses another reg to encode the
  explicit lod value.
  Presumably shadow 3d / 2d array / cube targets could use
  this encoding too, but this is not legal.

  shadow cube map arrays are neither possible nor required.

.. math::

  coord = src0

  lod = src1.x

  unit = src2

  dst = texture\_sample(unit, coord, lod)


.. opcode:: PUSHA - Push Address Register On Stack

  push(src.x)
  push(src.y)
  push(src.z)
  push(src.w)

.. note::

   Considered for cleanup.

.. note::

   Considered for removal.

.. opcode:: POPA - Pop Address Register From Stack

  dst.w = pop()
  dst.z = pop()
  dst.y = pop()
  dst.x = pop()

.. note::

   Considered for cleanup.

.. note::

   Considered for removal.


.. opcode:: CALLNZ - Subroutine Call If Not Zero

   TBD

.. note::

   Considered for cleanup.

.. note::

   Considered for removal.


Compute ISA
^^^^^^^^^^^^^^^^^^^^^^^^

These opcodes are primarily provided for special-use computational shaders.
Support for these opcodes indicated by a special pipe capability bit (TBD).

XXX doesn't look like most of the opcodes really belong here.

.. opcode:: CEIL - Ceiling

.. math::

  dst.x = \lceil src.x\rceil

  dst.y = \lceil src.y\rceil

  dst.z = \lceil src.z\rceil

  dst.w = \lceil src.w\rceil


.. opcode:: TRUNC - Truncate

.. math::

  dst.x = trunc(src.x)

  dst.y = trunc(src.y)

  dst.z = trunc(src.z)

  dst.w = trunc(src.w)


.. opcode:: MOD - Modulus

.. math::

  dst.x = src0.x \bmod src1.x

  dst.y = src0.y \bmod src1.y

  dst.z = src0.z \bmod src1.z

  dst.w = src0.w \bmod src1.w


.. opcode:: UARL - Integer Address Register Load

  Moves the contents of the source register, assumed to be an integer, into the
  destination register, which is assumed to be an address (ADDR) register.


.. opcode:: SAD - Sum Of Absolute Differences

.. math::

  dst.x = |src0.x - src1.x| + src2.x

  dst.y = |src0.y - src1.y| + src2.y

  dst.z = |src0.z - src1.z| + src2.z

  dst.w = |src0.w - src1.w| + src2.w


.. opcode:: TXF - Texel Fetch

  As per NV_gpu_shader4, extract a single texel from a specified texture
  image. The source sampler may not be a CUBE or SHADOW.  src 0 is a
  four-component signed integer vector used to identify the single texel
  accessed. 3 components + level.  Just like texture instructions, an optional
  offset vector is provided, which is subject to various driver restrictions
  (regarding range, source of offsets).
  TXF(uint_vec coord, int_vec offset).


.. opcode:: TXQ - Texture Size Query

  As per NV_gpu_program4, retrieve the dimensions of the texture depending on
  the target. For 1D (width), 2D/RECT/CUBE (width, height), 3D (width, height,
  depth), 1D array (width, layers), 2D array (width, height, layers).
  Also return the number of accessible levels (last_level - first_level + 1)
  in W.

  For components which don't return a resource dimension, their value
  is undefined.


.. math::

  lod = src0.x

  dst.x = texture\_width(unit, lod)

  dst.y = texture\_height(unit, lod)

  dst.z = texture\_depth(unit, lod)

  dst.w = texture\_levels(unit)

.. opcode:: TG4 - Texture Gather

  As per ARB_texture_gather, gathers the four texels to be used in a bi-linear
  filtering operation and packs them into a single register.  Only works with
  2D, 2D array, cubemaps, and cubemaps arrays.  For 2D textures, only the
  addressing modes of the sampler and the top level of any mip pyramid are
  used. Set W to zero.  It behaves like the TEX instruction, but a filtered
  sample is not generated. The four samples that contribute to filtering are
  placed into xyzw in clockwise order, starting with the (u,v) texture
  coordinate delta at the following locations (-, +), (+, +), (+, -), (-, -),
  where the magnitude of the deltas are half a texel.

  PIPE_CAP_TEXTURE_SM5 enhances this instruction to support shadow per-sample
  depth compares, single component selection, and a non-constant offset. It
  doesn't allow support for the GL independent offset to get i0,j0. This would
  require another CAP is hw can do it natively. For now we lower that before
  TGSI.

.. math::

   coord = src0

   component = src1

   dst = texture\_gather4 (unit, coord, component)

(with SM5 - cube array shadow)

.. math::

   coord = src0

   compare = src1

   dst = texture\_gather (uint, coord, compare)

.. opcode:: LODQ - level of detail query

   Compute the LOD information that the texture pipe would use to access the
   texture. The Y component contains the computed LOD lambda_prime. The X
   component contains the LOD that will be accessed, based on min/max lod's
   and mipmap filters.

.. math::

   coord = src0

   dst.xy = lodq(uint, coord);

Integer ISA
^^^^^^^^^^^^^^^^^^^^^^^^
These opcodes are used for integer operations.
Support for these opcodes indicated by PIPE_SHADER_CAP_INTEGERS (all of them?)


.. opcode:: I2F - Signed Integer To Float

   Rounding is unspecified (round to nearest even suggested).

.. math::

  dst.x = (float) src.x

  dst.y = (float) src.y

  dst.z = (float) src.z

  dst.w = (float) src.w


.. opcode:: U2F - Unsigned Integer To Float

   Rounding is unspecified (round to nearest even suggested).

.. math::

  dst.x = (float) src.x

  dst.y = (float) src.y

  dst.z = (float) src.z

  dst.w = (float) src.w


.. opcode:: F2I - Float to Signed Integer

   Rounding is towards zero (truncate).
   Values outside signed range (including NaNs) produce undefined results.

.. math::

  dst.x = (int) src.x

  dst.y = (int) src.y

  dst.z = (int) src.z

  dst.w = (int) src.w


.. opcode:: F2U - Float to Unsigned Integer

   Rounding is towards zero (truncate).
   Values outside unsigned range (including NaNs) produce undefined results.

.. math::

  dst.x = (unsigned) src.x

  dst.y = (unsigned) src.y

  dst.z = (unsigned) src.z

  dst.w = (unsigned) src.w


.. opcode:: UADD - Integer Add

   This instruction works the same for signed and unsigned integers.
   The low 32bit of the result is returned.

.. math::

  dst.x = src0.x + src1.x

  dst.y = src0.y + src1.y

  dst.z = src0.z + src1.z

  dst.w = src0.w + src1.w


.. opcode:: UMAD - Integer Multiply And Add

   This instruction works the same for signed and unsigned integers.
   The multiplication returns the low 32bit (as does the result itself).

.. math::

  dst.x = src0.x \times src1.x + src2.x

  dst.y = src0.y \times src1.y + src2.y

  dst.z = src0.z \times src1.z + src2.z

  dst.w = src0.w \times src1.w + src2.w


.. opcode:: UMUL - Integer Multiply

   This instruction works the same for signed and unsigned integers.
   The low 32bit of the result is returned.

.. math::

  dst.x = src0.x \times src1.x

  dst.y = src0.y \times src1.y

  dst.z = src0.z \times src1.z

  dst.w = src0.w \times src1.w


.. opcode:: IMUL_HI - Signed Integer Multiply High Bits

   The high 32bits of the multiplication of 2 signed integers are returned.

.. math::

  dst.x = (src0.x \times src1.x) >> 32

  dst.y = (src0.y \times src1.y) >> 32

  dst.z = (src0.z \times src1.z) >> 32

  dst.w = (src0.w \times src1.w) >> 32


.. opcode:: UMUL_HI - Unsigned Integer Multiply High Bits

   The high 32bits of the multiplication of 2 unsigned integers are returned.

.. math::

  dst.x = (src0.x \times src1.x) >> 32

  dst.y = (src0.y \times src1.y) >> 32

  dst.z = (src0.z \times src1.z) >> 32

  dst.w = (src0.w \times src1.w) >> 32


.. opcode:: IDIV - Signed Integer Division

   TBD: behavior for division by zero.

.. math::

  dst.x = src0.x \ src1.x

  dst.y = src0.y \ src1.y

  dst.z = src0.z \ src1.z

  dst.w = src0.w \ src1.w


.. opcode:: UDIV - Unsigned Integer Division

   For division by zero, 0xffffffff is returned.

.. math::

  dst.x = src0.x \ src1.x

  dst.y = src0.y \ src1.y

  dst.z = src0.z \ src1.z

  dst.w = src0.w \ src1.w


.. opcode:: UMOD - Unsigned Integer Remainder

   If second arg is zero, 0xffffffff is returned.

.. math::

  dst.x = src0.x \ src1.x

  dst.y = src0.y \ src1.y

  dst.z = src0.z \ src1.z

  dst.w = src0.w \ src1.w


.. opcode:: NOT - Bitwise Not

.. math::

  dst.x = \sim src.x

  dst.y = \sim src.y

  dst.z = \sim src.z

  dst.w = \sim src.w


.. opcode:: AND - Bitwise And

.. math::

  dst.x = src0.x \& src1.x

  dst.y = src0.y \& src1.y

  dst.z = src0.z \& src1.z

  dst.w = src0.w \& src1.w


.. opcode:: OR - Bitwise Or

.. math::

  dst.x = src0.x | src1.x

  dst.y = src0.y | src1.y

  dst.z = src0.z | src1.z

  dst.w = src0.w | src1.w


.. opcode:: XOR - Bitwise Xor

.. math::

  dst.x = src0.x \oplus src1.x

  dst.y = src0.y \oplus src1.y

  dst.z = src0.z \oplus src1.z

  dst.w = src0.w \oplus src1.w


.. opcode:: IMAX - Maximum of Signed Integers

.. math::

  dst.x = max(src0.x, src1.x)

  dst.y = max(src0.y, src1.y)

  dst.z = max(src0.z, src1.z)

  dst.w = max(src0.w, src1.w)


.. opcode:: UMAX - Maximum of Unsigned Integers

.. math::

  dst.x = max(src0.x, src1.x)

  dst.y = max(src0.y, src1.y)

  dst.z = max(src0.z, src1.z)

  dst.w = max(src0.w, src1.w)


.. opcode:: IMIN - Minimum of Signed Integers

.. math::

  dst.x = min(src0.x, src1.x)

  dst.y = min(src0.y, src1.y)

  dst.z = min(src0.z, src1.z)

  dst.w = min(src0.w, src1.w)


.. opcode:: UMIN - Minimum of Unsigned Integers

.. math::

  dst.x = min(src0.x, src1.x)

  dst.y = min(src0.y, src1.y)

  dst.z = min(src0.z, src1.z)

  dst.w = min(src0.w, src1.w)


.. opcode:: SHL - Shift Left

   The shift count is masked with 0x1f before the shift is applied.

.. math::

  dst.x = src0.x << (0x1f \& src1.x)

  dst.y = src0.y << (0x1f \& src1.y)

  dst.z = src0.z << (0x1f \& src1.z)

  dst.w = src0.w << (0x1f \& src1.w)


.. opcode:: ISHR - Arithmetic Shift Right (of Signed Integer)

   The shift count is masked with 0x1f before the shift is applied.

.. math::

  dst.x = src0.x >> (0x1f \& src1.x)

  dst.y = src0.y >> (0x1f \& src1.y)

  dst.z = src0.z >> (0x1f \& src1.z)

  dst.w = src0.w >> (0x1f \& src1.w)


.. opcode:: USHR - Logical Shift Right

   The shift count is masked with 0x1f before the shift is applied.

.. math::

  dst.x = src0.x >> (unsigned) (0x1f \& src1.x)

  dst.y = src0.y >> (unsigned) (0x1f \& src1.y)

  dst.z = src0.z >> (unsigned) (0x1f \& src1.z)

  dst.w = src0.w >> (unsigned) (0x1f \& src1.w)


.. opcode:: UCMP - Integer Conditional Move

.. math::

  dst.x = src0.x ? src1.x : src2.x

  dst.y = src0.y ? src1.y : src2.y

  dst.z = src0.z ? src1.z : src2.z

  dst.w = src0.w ? src1.w : src2.w



.. opcode:: ISSG - Integer Set Sign

.. math::

  dst.x = (src0.x < 0) ? -1 : (src0.x > 0) ? 1 : 0

  dst.y = (src0.y < 0) ? -1 : (src0.y > 0) ? 1 : 0

  dst.z = (src0.z < 0) ? -1 : (src0.z > 0) ? 1 : 0

  dst.w = (src0.w < 0) ? -1 : (src0.w > 0) ? 1 : 0



.. opcode:: FSLT - Float Set On Less Than (ordered)

   Same comparison as SLT but returns integer instead of 1.0/0.0 float

.. math::

  dst.x = (src0.x < src1.x) ? \sim 0 : 0

  dst.y = (src0.y < src1.y) ? \sim 0 : 0

  dst.z = (src0.z < src1.z) ? \sim 0 : 0

  dst.w = (src0.w < src1.w) ? \sim 0 : 0


.. opcode:: ISLT - Signed Integer Set On Less Than

.. math::

  dst.x = (src0.x < src1.x) ? \sim 0 : 0

  dst.y = (src0.y < src1.y) ? \sim 0 : 0

  dst.z = (src0.z < src1.z) ? \sim 0 : 0

  dst.w = (src0.w < src1.w) ? \sim 0 : 0


.. opcode:: USLT - Unsigned Integer Set On Less Than

.. math::

  dst.x = (src0.x < src1.x) ? \sim 0 : 0

  dst.y = (src0.y < src1.y) ? \sim 0 : 0

  dst.z = (src0.z < src1.z) ? \sim 0 : 0

  dst.w = (src0.w < src1.w) ? \sim 0 : 0


.. opcode:: FSGE - Float Set On Greater Equal Than (ordered)

   Same comparison as SGE but returns integer instead of 1.0/0.0 float

.. math::

  dst.x = (src0.x >= src1.x) ? \sim 0 : 0

  dst.y = (src0.y >= src1.y) ? \sim 0 : 0

  dst.z = (src0.z >= src1.z) ? \sim 0 : 0

  dst.w = (src0.w >= src1.w) ? \sim 0 : 0


.. opcode:: ISGE - Signed Integer Set On Greater Equal Than

.. math::

  dst.x = (src0.x >= src1.x) ? \sim 0 : 0

  dst.y = (src0.y >= src1.y) ? \sim 0 : 0

  dst.z = (src0.z >= src1.z) ? \sim 0 : 0

  dst.w = (src0.w >= src1.w) ? \sim 0 : 0


.. opcode:: USGE - Unsigned Integer Set On Greater Equal Than

.. math::

  dst.x = (src0.x >= src1.x) ? \sim 0 : 0

  dst.y = (src0.y >= src1.y) ? \sim 0 : 0

  dst.z = (src0.z >= src1.z) ? \sim 0 : 0

  dst.w = (src0.w >= src1.w) ? \sim 0 : 0


.. opcode:: FSEQ - Float Set On Equal (ordered)

   Same comparison as SEQ but returns integer instead of 1.0/0.0 float

.. math::

  dst.x = (src0.x == src1.x) ? \sim 0 : 0

  dst.y = (src0.y == src1.y) ? \sim 0 : 0

  dst.z = (src0.z == src1.z) ? \sim 0 : 0

  dst.w = (src0.w == src1.w) ? \sim 0 : 0


.. opcode:: USEQ - Integer Set On Equal

.. math::

  dst.x = (src0.x == src1.x) ? \sim 0 : 0

  dst.y = (src0.y == src1.y) ? \sim 0 : 0

  dst.z = (src0.z == src1.z) ? \sim 0 : 0

  dst.w = (src0.w == src1.w) ? \sim 0 : 0


.. opcode:: FSNE - Float Set On Not Equal (unordered)

   Same comparison as SNE but returns integer instead of 1.0/0.0 float

.. math::

  dst.x = (src0.x != src1.x) ? \sim 0 : 0

  dst.y = (src0.y != src1.y) ? \sim 0 : 0

  dst.z = (src0.z != src1.z) ? \sim 0 : 0

  dst.w = (src0.w != src1.w) ? \sim 0 : 0


.. opcode:: USNE - Integer Set On Not Equal

.. math::

  dst.x = (src0.x != src1.x) ? \sim 0 : 0

  dst.y = (src0.y != src1.y) ? \sim 0 : 0

  dst.z = (src0.z != src1.z) ? \sim 0 : 0

  dst.w = (src0.w != src1.w) ? \sim 0 : 0


.. opcode:: INEG - Integer Negate

  Two's complement.

.. math::

  dst.x = -src.x

  dst.y = -src.y

  dst.z = -src.z

  dst.w = -src.w


.. opcode:: IABS - Integer Absolute Value

.. math::

  dst.x = |src.x|

  dst.y = |src.y|

  dst.z = |src.z|

  dst.w = |src.w|

Bitwise ISA
^^^^^^^^^^^
These opcodes are used for bit-level manipulation of integers.

.. opcode:: IBFE - Signed Bitfield Extract

  See SM5 instruction of the same name. Extracts a set of bits from the input,
  and sign-extends them if the high bit of the extracted window is set.

  Pseudocode::

    def ibfe(value, offset, bits):
      offset = offset & 0x1f
      bits = bits & 0x1f
      if bits == 0: return 0
      # Note: >> sign-extends
      if width + offset < 32:
        return (value << (32 - offset - bits)) >> (32 - bits)
      else:
        return value >> offset

.. opcode:: UBFE - Unsigned Bitfield Extract

  See SM5 instruction of the same name. Extracts a set of bits from the input,
  without any sign-extension.

  Pseudocode::

    def ubfe(value, offset, bits):
      offset = offset & 0x1f
      bits = bits & 0x1f
      if bits == 0: return 0
      # Note: >> does not sign-extend
      if width + offset < 32:
        return (value << (32 - offset - bits)) >> (32 - bits)
      else:
        return value >> offset

.. opcode:: BFI - Bitfield Insert

  See SM5 instruction of the same name. Replaces a bit region of 'base' with
  the low bits of 'insert'.

  Pseudocode::

    def bfi(base, insert, offset, bits):
      offset = offset & 0x1f
      bits = bits & 0x1f
      mask = ((1 << bits) - 1) << offset
      return ((insert << offset) & mask) | (base & ~mask)

.. opcode:: BREV - Bitfield Reverse

  See SM5 instruction BFREV. Reverses the bits of the argument.

.. opcode:: POPC - Population Count

  See SM5 instruction COUNTBITS. Counts the number of set bits in the argument.

.. opcode:: LSB - Index of lowest set bit

  See SM5 instruction FIRSTBIT_LO. Computes the 0-based index of the first set
  bit of the argument. Returns -1 if none are set.

.. opcode:: IMSB - Index of highest non-sign bit

  See SM5 instruction FIRSTBIT_SHI. Computes the 0-based index of the highest
  non-sign bit of the argument (i.e. highest 0 bit for negative numbers,
  highest 1 bit for positive numbers). Returns -1 if all bits are the same
  (i.e. for inputs 0 and -1).

.. opcode:: UMSB - Index of highest set bit

  See SM5 instruction FIRSTBIT_HI. Computes the 0-based index of the highest
  set bit of the argument. Returns -1 if none are set.

Geometry ISA
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These opcodes are only supported in geometry shaders; they have no meaning
in any other type of shader.

.. opcode:: EMIT - Emit

  Generate a new vertex for the current primitive into the specified vertex
  stream using the values in the output registers.


.. opcode:: ENDPRIM - End Primitive

  Complete the current primitive in the specified vertex stream (consisting of
  the emitted vertices), and start a new one.


GLSL ISA
^^^^^^^^^^

These opcodes are part of :term:`GLSL`'s opcode set. Support for these
opcodes is determined by a special capability bit, ``GLSL``.
Some require glsl version 1.30 (UIF/BREAKC/SWITCH/CASE/DEFAULT/ENDSWITCH).

.. opcode:: CAL - Subroutine Call

  push(pc)
  pc = target


.. opcode:: RET - Subroutine Call Return

  pc = pop()


.. opcode:: CONT - Continue

  Unconditionally moves the point of execution to the instruction after the
  last bgnloop. The instruction must appear within a bgnloop/endloop.

.. note::

   Support for CONT is determined by a special capability bit,
   ``TGSI_CONT_SUPPORTED``. See :ref:`Screen` for more information.


.. opcode:: BGNLOOP - Begin a Loop

  Start a loop. Must have a matching endloop.


.. opcode:: BGNSUB - Begin Subroutine

  Starts definition of a subroutine. Must have a matching endsub.


.. opcode:: ENDLOOP - End a Loop

  End a loop started with bgnloop.


.. opcode:: ENDSUB - End Subroutine

  Ends definition of a subroutine.


.. opcode:: NOP - No Operation

  Do nothing.


.. opcode:: BRK - Break

  Unconditionally moves the point of execution to the instruction after the
  next endloop or endswitch. The instruction must appear within a loop/endloop
  or switch/endswitch.


.. opcode:: BREAKC - Break Conditional

  Conditionally moves the point of execution to the instruction after the
  next endloop or endswitch. The instruction must appear within a loop/endloop
  or switch/endswitch.
  Condition evaluates to true if src0.x != 0 where src0.x is interpreted
  as an integer register.

.. note::

   Considered for removal as it's quite inconsistent wrt other opcodes
   (could emulate with UIF/BRK/ENDIF). 


.. opcode:: IF - Float If

  Start an IF ... ELSE .. ENDIF block.  Condition evaluates to true if

    src0.x != 0.0

  where src0.x is interpreted as a floating point register.


.. opcode:: UIF - Bitwise If

  Start an UIF ... ELSE .. ENDIF block. Condition evaluates to true if

    src0.x != 0

  where src0.x is interpreted as an integer register.


.. opcode:: ELSE - Else

  Starts an else block, after an IF or UIF statement.


.. opcode:: ENDIF - End If

  Ends an IF or UIF block.


.. opcode:: SWITCH - Switch

   Starts a C-style switch expression. The switch consists of one or multiple
   CASE statements, and at most one DEFAULT statement. Execution of a statement
   ends when a BRK is hit, but just like in C falling through to other cases
   without a break is allowed. Similarly, DEFAULT label is allowed anywhere not
   just as last statement, and fallthrough is allowed into/from it.
   CASE src arguments are evaluated at bit level against the SWITCH src argument.

   Example::

     SWITCH src[0].x
     CASE src[0].x
     (some instructions here)
     (optional BRK here)
     DEFAULT
     (some instructions here)
     (optional BRK here)
     CASE src[0].x
     (some instructions here)
     (optional BRK here)
     ENDSWITCH


.. opcode:: CASE - Switch case

   This represents a switch case label. The src arg must be an integer immediate.


.. opcode:: DEFAULT - Switch default

   This represents the default case in the switch, which is taken if no other
   case matches.


.. opcode:: ENDSWITCH - End of switch

   Ends a switch expression.


Interpolation ISA
^^^^^^^^^^^^^^^^^

The interpolation instructions allow an input to be interpolated in a
different way than its declaration. This corresponds to the GLSL 4.00
interpolateAt* functions. The first argument of each of these must come from
``TGSI_FILE_INPUT``.

.. opcode:: INTERP_CENTROID - Interpolate at the centroid

   Interpolates the varying specified by src0 at the centroid

.. opcode:: INTERP_SAMPLE - Interpolate at the specified sample

   Interpolates the varying specified by src0 at the sample id specified by
   src1.x (interpreted as an integer)

.. opcode:: INTERP_OFFSET - Interpolate at the specified offset

   Interpolates the varying specified by src0 at the offset src1.xy from the
   pixel center (interpreted as floats)


.. _doubleopcodes:

Double ISA
^^^^^^^^^^^^^^^

The double-precision opcodes reinterpret four-component vectors into
two-component vectors with doubled precision in each component.

.. opcode:: DABS - Absolute

  dst.xy = |src0.xy|
  dst.zw = |src0.zw|

.. opcode:: DADD - Add

.. math::

  dst.xy = src0.xy + src1.xy

  dst.zw = src0.zw + src1.zw

.. opcode:: DSEQ - Set on Equal

.. math::

  dst.x = src0.xy == src1.xy ? \sim 0 : 0

  dst.z = src0.zw == src1.zw ? \sim 0 : 0

.. opcode:: DSNE - Set on Equal

.. math::

  dst.x = src0.xy != src1.xy ? \sim 0 : 0

  dst.z = src0.zw != src1.zw ? \sim 0 : 0

.. opcode:: DSLT - Set on Less than

.. math::

  dst.x = src0.xy < src1.xy ? \sim 0 : 0

  dst.z = src0.zw < src1.zw ? \sim 0 : 0

.. opcode:: DSGE - Set on Greater equal

.. math::

  dst.x = src0.xy >= src1.xy ? \sim 0 : 0

  dst.z = src0.zw >= src1.zw ? \sim 0 : 0

.. opcode:: DFRAC - Fraction

.. math::

  dst.xy = src.xy - \lfloor src.xy\rfloor

  dst.zw = src.zw - \lfloor src.zw\rfloor

.. opcode:: DTRUNC - Truncate

.. math::

  dst.xy = trunc(src.xy)

  dst.zw = trunc(src.zw)

.. opcode:: DCEIL - Ceiling

.. math::

  dst.xy = \lceil src.xy\rceil

  dst.zw = \lceil src.zw\rceil

.. opcode:: DFLR - Floor

.. math::

  dst.xy = \lfloor src.xy\rfloor

  dst.zw = \lfloor src.zw\rfloor

.. opcode:: DROUND - Fraction

.. math::

  dst.xy = round(src.xy)

  dst.zw = round(src.zw)

.. opcode:: DSSG - Set Sign

.. math::

  dst.xy = (src.xy > 0) ? 1.0 : (src.xy < 0) ? -1.0 : 0.0

  dst.zw = (src.zw > 0) ? 1.0 : (src.zw < 0) ? -1.0 : 0.0

.. opcode:: DFRACEXP - Convert Number to Fractional and Integral Components

Like the ``frexp()`` routine in many math libraries, this opcode stores the
exponent of its source to ``dst0``, and the significand to ``dst1``, such that
:math:`dst1 \times 2^{dst0} = src` .

.. math::

  dst0.xy = exp(src.xy)

  dst1.xy = frac(src.xy)

  dst0.zw = exp(src.zw)

  dst1.zw = frac(src.zw)

.. opcode:: DLDEXP - Multiply Number by Integral Power of 2

This opcode is the inverse of :opcode:`DFRACEXP`. The second
source is an integer.

.. math::

  dst.xy = src0.xy \times 2^{src1.x}

  dst.zw = src0.zw \times 2^{src1.y}

.. opcode:: DMIN - Minimum

.. math::

  dst.xy = min(src0.xy, src1.xy)

  dst.zw = min(src0.zw, src1.zw)

.. opcode:: DMAX - Maximum

.. math::

  dst.xy = max(src0.xy, src1.xy)

  dst.zw = max(src0.zw, src1.zw)

.. opcode:: DMUL - Multiply

.. math::

  dst.xy = src0.xy \times src1.xy

  dst.zw = src0.zw \times src1.zw


.. opcode:: DMAD - Multiply And Add

.. math::

  dst.xy = src0.xy \times src1.xy + src2.xy

  dst.zw = src0.zw \times src1.zw + src2.zw


.. opcode:: DFMA - Fused Multiply-Add

Perform a * b + c with no intermediate rounding step.

.. math::

  dst.xy = src0.xy \times src1.xy + src2.xy

  dst.zw = src0.zw \times src1.zw + src2.zw


.. opcode:: DRCP - Reciprocal

.. math::

   dst.xy = \frac{1}{src.xy}

   dst.zw = \frac{1}{src.zw}

.. opcode:: DSQRT - Square Root

.. math::

   dst.xy = \sqrt{src.xy}

   dst.zw = \sqrt{src.zw}

.. opcode:: DRSQ - Reciprocal Square Root

.. math::

   dst.xy = \frac{1}{\sqrt{src.xy}}

   dst.zw = \frac{1}{\sqrt{src.zw}}

.. opcode:: F2D - Float to Double

.. math::

   dst.xy = double(src0.x)

   dst.zw = double(src0.y)

.. opcode:: D2F - Double to Float

.. math::

   dst.x = float(src0.xy)

   dst.y = float(src0.zw)

.. opcode:: I2D - Int to Double

.. math::

   dst.xy = double(src0.x)

   dst.zw = double(src0.y)

.. opcode:: D2I - Double to Int

.. math::

   dst.x = int(src0.xy)

   dst.y = int(src0.zw)

.. opcode:: U2D - Unsigned Int to Double

.. math::

   dst.xy = double(src0.x)

   dst.zw = double(src0.y)

.. opcode:: D2U - Double to Unsigned Int

.. math::

   dst.x = unsigned(src0.xy)

   dst.y = unsigned(src0.zw)

.. _samplingopcodes:

Resource Sampling Opcodes
^^^^^^^^^^^^^^^^^^^^^^^^^

Those opcodes follow very closely semantics of the respective Direct3D
instructions. If in doubt double check Direct3D documentation.
Note that the swizzle on SVIEW (src1) determines texel swizzling
after lookup.

.. opcode:: SAMPLE

  Using provided address, sample data from the specified texture using the
  filtering mode identified by the gven sampler. The source data may come from
  any resource type other than buffers.

  Syntax: ``SAMPLE dst, address, sampler_view, sampler``

  Example: ``SAMPLE TEMP[0], TEMP[1], SVIEW[0], SAMP[0]``

.. opcode:: SAMPLE_I

  Simplified alternative to the SAMPLE instruction.  Using the provided
  integer address, SAMPLE_I fetches data from the specified sampler view
  without any filtering.  The source data may come from any resource type
  other than CUBE.

  Syntax: ``SAMPLE_I dst, address, sampler_view``

  Example: ``SAMPLE_I TEMP[0], TEMP[1], SVIEW[0]``

  The 'address' is specified as unsigned integers. If the 'address' is out of
  range [0...(# texels - 1)] the result of the fetch is always 0 in all
  components.  As such the instruction doesn't honor address wrap modes, in
  cases where that behavior is desirable 'SAMPLE' instruction should be used.
  address.w always provides an unsigned integer mipmap level. If the value is
  out of the range then the instruction always returns 0 in all components.
  address.yz are ignored for buffers and 1d textures.  address.z is ignored
  for 1d texture arrays and 2d textures.

  For 1D texture arrays address.y provides the array index (also as unsigned
  integer). If the value is out of the range of available array indices
  [0... (array size - 1)] then the opcode always returns 0 in all components.
  For 2D texture arrays address.z provides the array index, otherwise it
  exhibits the same behavior as in the case for 1D texture arrays.  The exact
  semantics of the source address are presented in the table below:

  +---------------------------+----+-----+-----+---------+
  | resource type             | X  |  Y  |  Z  |    W    |
  +===========================+====+=====+=====+=========+
  | ``PIPE_BUFFER``           | x  |     |     | ignored |
  +---------------------------+----+-----+-----+---------+
  | ``PIPE_TEXTURE_1D``       | x  |     |     |   mpl   |
  +---------------------------+----+-----+-----+---------+
  | ``PIPE_TEXTURE_2D``       | x  |  y  |     |   mpl   |
  +---------------------------+----+-----+-----+---------+
  | ``PIPE_TEXTURE_3D``       | x  |  y  |  z  |   mpl   |
  +---------------------------+----+-----+-----+---------+
  | ``PIPE_TEXTURE_RECT``     | x  |  y  |     |   mpl   |
  +---------------------------+----+-----+-----+---------+
  | ``PIPE_TEXTURE_CUBE``     | not allowed as source    |
  +---------------------------+----+-----+-----+---------+
  | ``PIPE_TEXTURE_1D_ARRAY`` | x  | idx |     |   mpl   |
  +---------------------------+----+-----+-----+---------+
  | ``PIPE_TEXTURE_2D_ARRAY`` | x  |  y  | idx |   mpl   |
  +---------------------------+----+-----+-----+---------+

  Where 'mpl' is a mipmap level and 'idx' is the array index.

.. opcode:: SAMPLE_I_MS

  Just like SAMPLE_I but allows fetch data from multi-sampled surfaces.

  Syntax: ``SAMPLE_I_MS dst, address, sampler_view, sample``

.. opcode:: SAMPLE_B

  Just like the SAMPLE instruction with the exception that an additional bias
  is applied to the level of detail computed as part of the instruction
  execution.

  Syntax: ``SAMPLE_B dst, address, sampler_view, sampler, lod_bias``

  Example: ``SAMPLE_B TEMP[0], TEMP[1], SVIEW[0], SAMP[0], TEMP[2].x``

.. opcode:: SAMPLE_C

  Similar to the SAMPLE instruction but it performs a comparison filter. The
  operands to SAMPLE_C are identical to SAMPLE, except that there is an
  additional float32 operand, reference value, which must be a register with
  single-component, or a scalar literal.  SAMPLE_C makes the hardware use the
  current samplers compare_func (in pipe_sampler_state) to compare reference
  value against the red component value for the surce resource at each texel
  that the currently configured texture filter covers based on the provided
  coordinates.

  Syntax: ``SAMPLE_C dst, address, sampler_view.r, sampler, ref_value``

  Example: ``SAMPLE_C TEMP[0], TEMP[1], SVIEW[0].r, SAMP[0], TEMP[2].x``

.. opcode:: SAMPLE_C_LZ

  Same as SAMPLE_C, but LOD is 0 and derivatives are ignored. The LZ stands
  for level-zero.

  Syntax: ``SAMPLE_C_LZ dst, address, sampler_view.r, sampler, ref_value``

  Example: ``SAMPLE_C_LZ TEMP[0], TEMP[1], SVIEW[0].r, SAMP[0], TEMP[2].x``


.. opcode:: SAMPLE_D

  SAMPLE_D is identical to the SAMPLE opcode except that the derivatives for
  the source address in the x direction and the y direction are provided by
  extra parameters.

  Syntax: ``SAMPLE_D dst, address, sampler_view, sampler, der_x, der_y``

  Example: ``SAMPLE_D TEMP[0], TEMP[1], SVIEW[0], SAMP[0], TEMP[2], TEMP[3]``

.. opcode:: SAMPLE_L

  SAMPLE_L is identical to the SAMPLE opcode except that the LOD is provided
  directly as a scalar value, representing no anisotropy.

  Syntax: ``SAMPLE_L dst, address, sampler_view, sampler, explicit_lod``

  Example: ``SAMPLE_L TEMP[0], TEMP[1], SVIEW[0], SAMP[0], TEMP[2].x``

.. opcode:: GATHER4

  Gathers the four texels to be used in a bi-linear filtering operation and
  packs them into a single register.  Only works with 2D, 2D array, cubemaps,
  and cubemaps arrays.  For 2D textures, only the addressing modes of the
  sampler and the top level of any mip pyramid are used. Set W to zero.  It
  behaves like the SAMPLE instruction, but a filtered sample is not
  generated. The four samples that contribute to filtering are placed into
  xyzw in counter-clockwise order, starting with the (u,v) texture coordinate
  delta at the following locations (-, +), (+, +), (+, -), (-, -), where the
  magnitude of the deltas are half a texel.


.. opcode:: SVIEWINFO

  Query the dimensions of a given sampler view.  dst receives width, height,
  depth or array size and number of mipmap levels as int4. The dst can have a
  writemask which will specify what info is the caller interested in.

  Syntax: ``SVIEWINFO dst, src_mip_level, sampler_view``

  Example: ``SVIEWINFO TEMP[0], TEMP[1].x, SVIEW[0]``

  src_mip_level is an unsigned integer scalar. If it's out of range then
  returns 0 for width, height and depth/array size but the total number of
  mipmap is still returned correctly for the given sampler view.  The returned
  width, height and depth values are for the mipmap level selected by the
  src_mip_level and are in the number of texels.  For 1d texture array width
  is in dst.x, array size is in dst.y and dst.z is 0. The number of mipmaps is
  still in dst.w.  In contrast to d3d10 resinfo, there's no way in the tgsi
  instruction encoding to specify the return type (float/rcpfloat/uint), hence
  always using uint. Also, unlike the SAMPLE instructions, the swizzle on src1
  resinfo allowing swizzling dst values is ignored (due to the interaction
  with rcpfloat modifier which requires some swizzle handling in the state
  tracker anyway).

.. opcode:: SAMPLE_POS

  Query the position of a given sample.  dst receives float4 (x, y, 0, 0)
  indicated where the sample is located. If the resource is not a multi-sample
  resource and not a render target, the result is 0.

.. opcode:: SAMPLE_INFO

  dst receives number of samples in x.  If the resource is not a multi-sample
  resource and not a render target, the result is 0.


.. _resourceopcodes:

Resource Access Opcodes
^^^^^^^^^^^^^^^^^^^^^^^

.. opcode:: LOAD - Fetch data from a shader resource

               Syntax: ``LOAD dst, resource, address``

               Example: ``LOAD TEMP[0], RES[0], TEMP[1]``

               Using the provided integer address, LOAD fetches data
               from the specified buffer or texture without any
               filtering.

               The 'address' is specified as a vector of unsigned
               integers.  If the 'address' is out of range the result
               is unspecified.

               Only the first mipmap level of a resource can be read
               from using this instruction.

               For 1D or 2D texture arrays, the array index is
               provided as an unsigned integer in address.y or
               address.z, respectively.  address.yz are ignored for
               buffers and 1D textures.  address.z is ignored for 1D
               texture arrays and 2D textures.  address.w is always
               ignored.

.. opcode:: STORE - Write data to a shader resource

               Syntax: ``STORE resource, address, src``

               Example: ``STORE RES[0], TEMP[0], TEMP[1]``

               Using the provided integer address, STORE writes data
               to the specified buffer or texture.

               The 'address' is specified as a vector of unsigned
               integers.  If the 'address' is out of range the result
               is unspecified.

               Only the first mipmap level of a resource can be
               written to using this instruction.

               For 1D or 2D texture arrays, the array index is
               provided as an unsigned integer in address.y or
               address.z, respectively.  address.yz are ignored for
               buffers and 1D textures.  address.z is ignored for 1D
               texture arrays and 2D textures.  address.w is always
               ignored.


.. _threadsyncopcodes:

Inter-thread synchronization opcodes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These opcodes are intended for communication between threads running
within the same compute grid.  For now they're only valid in compute
programs.

.. opcode:: MFENCE - Memory fence

  Syntax: ``MFENCE resource``

  Example: ``MFENCE RES[0]``

  This opcode forces strong ordering between any memory access
  operations that affect the specified resource.  This means that
  previous loads and stores (and only those) will be performed and
  visible to other threads before the program execution continues.


.. opcode:: LFENCE - Load memory fence

  Syntax: ``LFENCE resource``

  Example: ``LFENCE RES[0]``

  Similar to MFENCE, but it only affects the ordering of memory loads.


.. opcode:: SFENCE - Store memory fence

  Syntax: ``SFENCE resource``

  Example: ``SFENCE RES[0]``

  Similar to MFENCE, but it only affects the ordering of memory stores.


.. opcode:: BARRIER - Thread group barrier

  ``BARRIER``

  This opcode suspends the execution of the current thread until all
  the remaining threads in the working group reach the same point of
  the program.  Results are unspecified if any of the remaining
  threads terminates or never reaches an executed BARRIER instruction.


.. _atomopcodes:

Atomic opcodes
^^^^^^^^^^^^^^

These opcodes provide atomic variants of some common arithmetic and
logical operations.  In this context atomicity means that another
concurrent memory access operation that affects the same memory
location is guaranteed to be performed strictly before or after the
entire execution of the atomic operation.

For the moment they're only valid in compute programs.

.. opcode:: ATOMUADD - Atomic integer addition

  Syntax: ``ATOMUADD dst, resource, offset, src``

  Example: ``ATOMUADD TEMP[0], RES[0], TEMP[1], TEMP[2]``

  The following operation is performed atomically on each component:

.. math::

  dst_i = resource[offset]_i

  resource[offset]_i = dst_i + src_i


.. opcode:: ATOMXCHG - Atomic exchange

  Syntax: ``ATOMXCHG dst, resource, offset, src``

  Example: ``ATOMXCHG TEMP[0], RES[0], TEMP[1], TEMP[2]``

  The following operation is performed atomically on each component:

.. math::

  dst_i = resource[offset]_i

  resource[offset]_i = src_i


.. opcode:: ATOMCAS - Atomic compare-and-exchange

  Syntax: ``ATOMCAS dst, resource, offset, cmp, src``

  Example: ``ATOMCAS TEMP[0], RES[0], TEMP[1], TEMP[2], TEMP[3]``

  The following operation is performed atomically on each component:

.. math::

  dst_i = resource[offset]_i

  resource[offset]_i = (dst_i == cmp_i ? src_i : dst_i)


.. opcode:: ATOMAND - Atomic bitwise And

  Syntax: ``ATOMAND dst, resource, offset, src``

  Example: ``ATOMAND TEMP[0], RES[0], TEMP[1], TEMP[2]``

  The following operation is performed atomically on each component:

.. math::

  dst_i = resource[offset]_i

  resource[offset]_i = dst_i \& src_i


.. opcode:: ATOMOR - Atomic bitwise Or

  Syntax: ``ATOMOR dst, resource, offset, src``

  Example: ``ATOMOR TEMP[0], RES[0], TEMP[1], TEMP[2]``

  The following operation is performed atomically on each component:

.. math::

  dst_i = resource[offset]_i

  resource[offset]_i = dst_i | src_i


.. opcode:: ATOMXOR - Atomic bitwise Xor

  Syntax: ``ATOMXOR dst, resource, offset, src``

  Example: ``ATOMXOR TEMP[0], RES[0], TEMP[1], TEMP[2]``

  The following operation is performed atomically on each component:

.. math::

  dst_i = resource[offset]_i

  resource[offset]_i = dst_i \oplus src_i


.. opcode:: ATOMUMIN - Atomic unsigned minimum

  Syntax: ``ATOMUMIN dst, resource, offset, src``

  Example: ``ATOMUMIN TEMP[0], RES[0], TEMP[1], TEMP[2]``

  The following operation is performed atomically on each component:

.. math::

  dst_i = resource[offset]_i

  resource[offset]_i = (dst_i < src_i ? dst_i : src_i)


.. opcode:: ATOMUMAX - Atomic unsigned maximum

  Syntax: ``ATOMUMAX dst, resource, offset, src``

  Example: ``ATOMUMAX TEMP[0], RES[0], TEMP[1], TEMP[2]``

  The following operation is performed atomically on each component:

.. math::

  dst_i = resource[offset]_i

  resource[offset]_i = (dst_i > src_i ? dst_i : src_i)


.. opcode:: ATOMIMIN - Atomic signed minimum

  Syntax: ``ATOMIMIN dst, resource, offset, src``

  Example: ``ATOMIMIN TEMP[0], RES[0], TEMP[1], TEMP[2]``

  The following operation is performed atomically on each component:

.. math::

  dst_i = resource[offset]_i

  resource[offset]_i = (dst_i < src_i ? dst_i : src_i)


.. opcode:: ATOMIMAX - Atomic signed maximum

  Syntax: ``ATOMIMAX dst, resource, offset, src``

  Example: ``ATOMIMAX TEMP[0], RES[0], TEMP[1], TEMP[2]``

  The following operation is performed atomically on each component:

.. math::

  dst_i = resource[offset]_i

  resource[offset]_i = (dst_i > src_i ? dst_i : src_i)



Explanation of symbols used
------------------------------


Functions
^^^^^^^^^^^^^^


  :math:`|x|`       Absolute value of `x`.

  :math:`\lceil x \rceil` Ceiling of `x`.

  clamp(x,y,z)      Clamp x between y and z.
                    (x < y) ? y : (x > z) ? z : x

  :math:`\lfloor x\rfloor` Floor of `x`.

  :math:`\log_2{x}` Logarithm of `x`, base 2.

  max(x,y)          Maximum of x and y.
                    (x > y) ? x : y

  min(x,y)          Minimum of x and y.
                    (x < y) ? x : y

  partialx(x)       Derivative of x relative to fragment's X.

  partialy(x)       Derivative of x relative to fragment's Y.

  pop()             Pop from stack.

  :math:`x^y`       `x` to the power `y`.

  push(x)           Push x on stack.

  round(x)          Round x.

  trunc(x)          Truncate x, i.e. drop the fraction bits.


Keywords
^^^^^^^^^^^^^


  discard           Discard fragment.

  pc                Program counter.

  target            Label of target instruction.


Other tokens
---------------


Declaration
^^^^^^^^^^^


Declares a register that is will be referenced as an operand in Instruction
tokens.

File field contains register file that is being declared and is one
of TGSI_FILE.

UsageMask field specifies which of the register components can be accessed
and is one of TGSI_WRITEMASK.

The Local flag specifies that a given value isn't intended for
subroutine parameter passing and, as a result, the implementation
isn't required to give any guarantees of it being preserved across
subroutine boundaries.  As it's merely a compiler hint, the
implementation is free to ignore it.

If Dimension flag is set to 1, a Declaration Dimension token follows.

If Semantic flag is set to 1, a Declaration Semantic token follows.

If Interpolate flag is set to 1, a Declaration Interpolate token follows.

If file is TGSI_FILE_RESOURCE, a Declaration Resource token follows.

If Array flag is set to 1, a Declaration Array token follows.

Array Declaration
^^^^^^^^^^^^^^^^^^^^^^^^

Declarations can optional have an ArrayID attribute which can be referred by
indirect addressing operands. An ArrayID of zero is reserved and treaded as
if no ArrayID is specified.

If an indirect addressing operand refers to a specific declaration by using
an ArrayID only the registers in this declaration are guaranteed to be
accessed, accessing any register outside this declaration results in undefined
behavior. Note that for compatibility the effective index is zero-based and
not relative to the specified declaration

If no ArrayID is specified with an indirect addressing operand the whole
register file might be accessed by this operand. This is strongly discouraged
and will prevent packing of scalar/vec2 arrays and effective alias analysis.

Declaration Semantic
^^^^^^^^^^^^^^^^^^^^^^^^

Vertex and fragment shader input and output registers may be labeled
with semantic information consisting of a name and index.

Follows Declaration token if Semantic bit is set.

Since its purpose is to link a shader with other stages of the pipeline,
it is valid to follow only those Declaration tokens that declare a register
either in INPUT or OUTPUT file.

SemanticName field contains the semantic name of the register being declared.
There is no default value.

SemanticIndex is an optional subscript that can be used to distinguish
different register declarations with the same semantic name. The default value
is 0.

The meanings of the individual semantic names are explained in the following
sections.

TGSI_SEMANTIC_POSITION
""""""""""""""""""""""

For vertex shaders, TGSI_SEMANTIC_POSITION indicates the vertex shader
output register which contains the homogeneous vertex position in the clip
space coordinate system.  After clipping, the X, Y and Z components of the
vertex will be divided by the W value to get normalized device coordinates.

For fragment shaders, TGSI_SEMANTIC_POSITION is used to indicate that
fragment shader input contains the fragment's window position.  The X
component starts at zero and always increases from left to right.
The Y component starts at zero and always increases but Y=0 may either
indicate the top of the window or the bottom depending on the fragment
coordinate origin convention (see TGSI_PROPERTY_FS_COORD_ORIGIN).
The Z coordinate ranges from 0 to 1 to represent depth from the front
to the back of the Z buffer.  The W component contains the interpolated
reciprocal of the vertex position W component (corresponding to gl_Fragcoord,
but unlike d3d10 which interpolates the same 1/w but then gives back
the reciprocal of the interpolated value).

Fragment shaders may also declare an output register with
TGSI_SEMANTIC_POSITION.  Only the Z component is writable.  This allows
the fragment shader to change the fragment's Z position.



TGSI_SEMANTIC_COLOR
"""""""""""""""""""

For vertex shader outputs or fragment shader inputs/outputs, this
label indicates that the resister contains an R,G,B,A color.

Several shader inputs/outputs may contain colors so the semantic index
is used to distinguish them.  For example, color[0] may be the diffuse
color while color[1] may be the specular color.

This label is needed so that the flat/smooth shading can be applied
to the right interpolants during rasterization.



TGSI_SEMANTIC_BCOLOR
""""""""""""""""""""

Back-facing colors are only used for back-facing polygons, and are only valid
in vertex shader outputs. After rasterization, all polygons are front-facing
and COLOR and BCOLOR end up occupying the same slots in the fragment shader,
so all BCOLORs effectively become regular COLORs in the fragment shader.


TGSI_SEMANTIC_FOG
"""""""""""""""""

Vertex shader inputs and outputs and fragment shader inputs may be
labeled with TGSI_SEMANTIC_FOG to indicate that the register contains
a fog coordinate.  Typically, the fragment shader will use the fog coordinate
to compute a fog blend factor which is used to blend the normal fragment color
with a constant fog color.  But fog coord really is just an ordinary vec4
register like regular semantics.


TGSI_SEMANTIC_PSIZE
"""""""""""""""""""

Vertex shader input and output registers may be labeled with
TGIS_SEMANTIC_PSIZE to indicate that the register contains a point size
in the form (S, 0, 0, 1).  The point size controls the width or diameter
of points for rasterization.  This label cannot be used in fragment
shaders.

When using this semantic, be sure to set the appropriate state in the
:ref:`rasterizer` first.


TGSI_SEMANTIC_TEXCOORD
""""""""""""""""""""""

Only available if PIPE_CAP_TGSI_TEXCOORD is exposed !

Vertex shader outputs and fragment shader inputs may be labeled with
this semantic to make them replaceable by sprite coordinates via the
sprite_coord_enable state in the :ref:`rasterizer`.
The semantic index permitted with this semantic is limited to <= 7.

If the driver does not support TEXCOORD, sprite coordinate replacement
applies to inputs with the GENERIC semantic instead.

The intended use case for this semantic is gl_TexCoord.


TGSI_SEMANTIC_PCOORD
""""""""""""""""""""

Only available if PIPE_CAP_TGSI_TEXCOORD is exposed !

Fragment shader inputs may be labeled with TGSI_SEMANTIC_PCOORD to indicate
that the register contains sprite coordinates in the form (x, y, 0, 1), if
the current primitive is a point and point sprites are enabled. Otherwise,
the contents of the register are undefined.

The intended use case for this semantic is gl_PointCoord.


TGSI_SEMANTIC_GENERIC
"""""""""""""""""""""

All vertex/fragment shader inputs/outputs not labeled with any other
semantic label can be considered to be generic attributes.  Typical
uses of generic inputs/outputs are texcoords and user-defined values.


TGSI_SEMANTIC_NORMAL
""""""""""""""""""""

Indicates that a vertex shader input is a normal vector.  This is
typically only used for legacy graphics APIs.


TGSI_SEMANTIC_FACE
""""""""""""""""""

This label applies to fragment shader inputs only and indicates that
the register contains front/back-face information of the form (F, 0,
0, 1).  The first component will be positive when the fragment belongs
to a front-facing polygon, and negative when the fragment belongs to a
back-facing polygon.


TGSI_SEMANTIC_EDGEFLAG
""""""""""""""""""""""

For vertex shaders, this sematic label indicates that an input or
output is a boolean edge flag.  The register layout is [F, x, x, x]
where F is 0.0 or 1.0 and x = don't care.  Normally, the vertex shader
simply copies the edge flag input to the edgeflag output.

Edge flags are used to control which lines or points are actually
drawn when the polygon mode converts triangles/quads/polygons into
points or lines.


TGSI_SEMANTIC_STENCIL
"""""""""""""""""""""

For fragment shaders, this semantic label indicates that an output
is a writable stencil reference value. Only the Y component is writable.
This allows the fragment shader to change the fragments stencilref value.


TGSI_SEMANTIC_VIEWPORT_INDEX
""""""""""""""""""""""""""""

For geometry shaders, this semantic label indicates that an output
contains the index of the viewport (and scissor) to use.
This is an integer value, and only the X component is used.


TGSI_SEMANTIC_LAYER
"""""""""""""""""""

For geometry shaders, this semantic label indicates that an output
contains the layer value to use for the color and depth/stencil surfaces.
This is an integer value, and only the X component is used.
(Also known as rendertarget array index.)


TGSI_SEMANTIC_CULLDIST
""""""""""""""""""""""

Used as distance to plane for performing application-defined culling
of individual primitives against a plane. When components of vertex
elements are given this label, these values are assumed to be a
float32 signed distance to a plane. Primitives will be completely
discarded if the plane distance for all of the vertices in the
primitive are < 0. If a vertex has a cull distance of NaN, that
vertex counts as "out" (as if its < 0);
The limits on both clip and cull distances are bound
by the PIPE_MAX_CLIP_OR_CULL_DISTANCE_COUNT define which defines
the maximum number of components that can be used to hold the
distances and by the PIPE_MAX_CLIP_OR_CULL_DISTANCE_ELEMENT_COUNT
which specifies the maximum number of registers which can be
annotated with those semantics.


TGSI_SEMANTIC_CLIPDIST
""""""""""""""""""""""

When components of vertex elements are identified this way, these
values are each assumed to be a float32 signed distance to a plane.
Primitive setup only invokes rasterization on pixels for which
the interpolated plane distances are >= 0. Multiple clip planes
can be implemented simultaneously, by annotating multiple
components of one or more vertex elements with the above specified
semantic. The limits on both clip and cull distances are bound
by the PIPE_MAX_CLIP_OR_CULL_DISTANCE_COUNT define which defines
the maximum number of components that can be used to hold the
distances and by the PIPE_MAX_CLIP_OR_CULL_DISTANCE_ELEMENT_COUNT
which specifies the maximum number of registers which can be
annotated with those semantics.

TGSI_SEMANTIC_SAMPLEID
""""""""""""""""""""""

For fragment shaders, this semantic label indicates that a system value
contains the current sample id (i.e. gl_SampleID).
This is an integer value, and only the X component is used.

TGSI_SEMANTIC_SAMPLEPOS
"""""""""""""""""""""""

For fragment shaders, this semantic label indicates that a system value
contains the current sample's position (i.e. gl_SamplePosition). Only the X
and Y values are used.

TGSI_SEMANTIC_SAMPLEMASK
""""""""""""""""""""""""

For fragment shaders, this semantic label indicates that an output contains
the sample mask used to disable further sample processing
(i.e. gl_SampleMask). Only the X value is used, up to 32x MS.

TGSI_SEMANTIC_INVOCATIONID
""""""""""""""""""""""""""

For geometry shaders, this semantic label indicates that a system value
contains the current invocation id (i.e. gl_InvocationID).
This is an integer value, and only the X component is used.

TGSI_SEMANTIC_INSTANCEID
""""""""""""""""""""""""

For vertex shaders, this semantic label indicates that a system value contains
the current instance id (i.e. gl_InstanceID). It does not include the base
instance. This is an integer value, and only the X component is used.

TGSI_SEMANTIC_VERTEXID
""""""""""""""""""""""

For vertex shaders, this semantic label indicates that a system value contains
the current vertex id (i.e. gl_VertexID). It does (unlike in d3d10) include the
base vertex. This is an integer value, and only the X component is used.

TGSI_SEMANTIC_VERTEXID_NOBASE
"""""""""""""""""""""""""""""""

For vertex shaders, this semantic label indicates that a system value contains
the current vertex id without including the base vertex (this corresponds to
d3d10 vertex id, so TGSI_SEMANTIC_VERTEXID_NOBASE + TGSI_SEMANTIC_BASEVERTEX
== TGSI_SEMANTIC_VERTEXID). This is an integer value, and only the X component
is used.

TGSI_SEMANTIC_BASEVERTEX
""""""""""""""""""""""""

For vertex shaders, this semantic label indicates that a system value contains
the base vertex (i.e. gl_BaseVertex). Note that for non-indexed draw calls,
this contains the first (or start) value instead.
This is an integer value, and only the X component is used.

TGSI_SEMANTIC_PRIMID
""""""""""""""""""""

For geometry and fragment shaders, this semantic label indicates the value
contains the primitive id (i.e. gl_PrimitiveID). This is an integer value,
and only the X component is used.
FIXME: This right now can be either a ordinary input or a system value...


TGSI_SEMANTIC_PATCH
"""""""""""""""""""

For tessellation evaluation/control shaders, this semantic label indicates a
generic per-patch attribute. Such semantics will not implicitly be per-vertex
arrays.

TGSI_SEMANTIC_TESSCOORD
"""""""""""""""""""""""

For tessellation evaluation shaders, this semantic label indicates the
coordinates of the vertex being processed. This is available in XYZ; W is
undefined.

TGSI_SEMANTIC_TESSOUTER
"""""""""""""""""""""""

For tessellation evaluation/control shaders, this semantic label indicates the
outer tessellation levels of the patch. Isoline tessellation will only have XY
defined, triangle will have XYZ and quads will have XYZW defined. This
corresponds to gl_TessLevelOuter.

TGSI_SEMANTIC_TESSINNER
"""""""""""""""""""""""

For tessellation evaluation/control shaders, this semantic label indicates the
inner tessellation levels of the patch. The X value is only defined for
triangle tessellation, while quads will have XY defined. This is entirely
undefined for isoline tessellation.

TGSI_SEMANTIC_VERTICESIN
""""""""""""""""""""""""

For tessellation evaluation/control shaders, this semantic label indicates the
number of vertices provided in the input patch. Only the X value is defined.


Declaration Interpolate
^^^^^^^^^^^^^^^^^^^^^^^

This token is only valid for fragment shader INPUT declarations.

The Interpolate field specifes the way input is being interpolated by
the rasteriser and is one of TGSI_INTERPOLATE_*.

The Location field specifies the location inside the pixel that the
interpolation should be done at, one of ``TGSI_INTERPOLATE_LOC_*``. Note that
when per-sample shading is enabled, the implementation may choose to
interpolate at the sample irrespective of the Location field.

The CylindricalWrap bitfield specifies which register components
should be subject to cylindrical wrapping when interpolating by the
rasteriser. If TGSI_CYLINDRICAL_WRAP_X is set to 1, the X component
should be interpolated according to cylindrical wrapping rules.


Declaration Sampler View
^^^^^^^^^^^^^^^^^^^^^^^^

Follows Declaration token if file is TGSI_FILE_SAMPLER_VIEW.

DCL SVIEW[#], resource, type(s)

Declares a shader input sampler view and assigns it to a SVIEW[#]
register.

resource can be one of BUFFER, 1D, 2D, 3D, 1DArray and 2DArray.

type must be 1 or 4 entries (if specifying on a per-component
level) out of UNORM, SNORM, SINT, UINT and FLOAT.


Declaration Resource
^^^^^^^^^^^^^^^^^^^^

Follows Declaration token if file is TGSI_FILE_RESOURCE.

DCL RES[#], resource [, WR] [, RAW]

Declares a shader input resource and assigns it to a RES[#]
register.

resource can be one of BUFFER, 1D, 2D, 3D, CUBE, 1DArray and
2DArray.

If the RAW keyword is not specified, the texture data will be
subject to conversion, swizzling and scaling as required to yield
the specified data type from the physical data format of the bound
resource.

If the RAW keyword is specified, no channel conversion will be
performed: the values read for each of the channels (X,Y,Z,W) will
correspond to consecutive words in the same order and format
they're found in memory.  No element-to-address conversion will be
performed either: the value of the provided X coordinate will be
interpreted in byte units instead of texel units.  The result of
accessing a misaligned address is undefined.

Usage of the STORE opcode is only allowed if the WR (writable) flag
is set.


Properties
^^^^^^^^^^^^^^^^^^^^^^^^

Properties are general directives that apply to the whole TGSI program.

FS_COORD_ORIGIN
"""""""""""""""

Specifies the fragment shader TGSI_SEMANTIC_POSITION coordinate origin.
The default value is UPPER_LEFT.

If UPPER_LEFT, the position will be (0,0) at the upper left corner and
increase downward and rightward.
If LOWER_LEFT, the position will be (0,0) at the lower left corner and
increase upward and rightward.

OpenGL defaults to LOWER_LEFT, and is configurable with the
GL_ARB_fragment_coord_conventions extension.

DirectX 9/10 use UPPER_LEFT.

FS_COORD_PIXEL_CENTER
"""""""""""""""""""""

Specifies the fragment shader TGSI_SEMANTIC_POSITION pixel center convention.
The default value is HALF_INTEGER.

If HALF_INTEGER, the fractionary part of the position will be 0.5
If INTEGER, the fractionary part of the position will be 0.0

Note that this does not affect the set of fragments generated by
rasterization, which is instead controlled by half_pixel_center in the
rasterizer.

OpenGL defaults to HALF_INTEGER, and is configurable with the
GL_ARB_fragment_coord_conventions extension.

DirectX 9 uses INTEGER.
DirectX 10 uses HALF_INTEGER.

FS_COLOR0_WRITES_ALL_CBUFS
""""""""""""""""""""""""""
Specifies that writes to the fragment shader color 0 are replicated to all
bound cbufs. This facilitates OpenGL's fragColor output vs fragData[0] where
fragData is directed to a single color buffer, but fragColor is broadcast.

VS_PROHIBIT_UCPS
""""""""""""""""""""""""""
If this property is set on the program bound to the shader stage before the
fragment shader, user clip planes should have no effect (be disabled) even if
that shader does not write to any clip distance outputs and the rasterizer's
clip_plane_enable is non-zero.
This property is only supported by drivers that also support shader clip
distance outputs.
This is useful for APIs that don't have UCPs and where clip distances written
by a shader cannot be disabled.

GS_INVOCATIONS
""""""""""""""

Specifies the number of times a geometry shader should be executed for each
input primitive. Each invocation will have a different
TGSI_SEMANTIC_INVOCATIONID system value set. If not specified, assumed to
be 1.

VS_WINDOW_SPACE_POSITION
""""""""""""""""""""""""""
If this property is set on the vertex shader, the TGSI_SEMANTIC_POSITION output
is assumed to contain window space coordinates.
Division of X,Y,Z by W and the viewport transformation are disabled, and 1/W is
directly taken from the 4-th component of the shader output.
Naturally, clipping is not performed on window coordinates either.
The effect of this property is undefined if a geometry or tessellation shader
are in use.

TCS_VERTICES_OUT
""""""""""""""""

The number of vertices written by the tessellation control shader. This
effectively defines the patch input size of the tessellation evaluation shader
as well.

TES_PRIM_MODE
"""""""""""""

This sets the tessellation primitive mode, one of ``PIPE_PRIM_TRIANGLES``,
``PIPE_PRIM_QUADS``, or ``PIPE_PRIM_LINES``. (Unlike in GL, there is no
separate isolines settings, the regular lines is assumed to mean isolines.)

TES_SPACING
"""""""""""

This sets the spacing mode of the tessellation generator, one of
``PIPE_TESS_SPACING_*``.

TES_VERTEX_ORDER_CW
"""""""""""""""""""

This sets the vertex order to be clockwise if the value is 1, or
counter-clockwise if set to 0.

TES_POINT_MODE
""""""""""""""

If set to a non-zero value, this turns on point mode for the tessellator,
which means that points will be generated instead of primitives.


Texture Sampling and Texture Formats
------------------------------------

This table shows how texture image components are returned as (x,y,z,w) tuples
by TGSI texture instructions, such as :opcode:`TEX`, :opcode:`TXD`, and
:opcode:`TXP`. For reference, OpenGL and Direct3D conventions are shown as
well.

+--------------------+--------------+--------------------+--------------+
| Texture Components | Gallium      | OpenGL             | Direct3D 9   |
+====================+==============+====================+==============+
| R                  | (r, 0, 0, 1) | (r, 0, 0, 1)       | (r, 1, 1, 1) |
+--------------------+--------------+--------------------+--------------+
| RG                 | (r, g, 0, 1) | (r, g, 0, 1)       | (r, g, 1, 1) |
+--------------------+--------------+--------------------+--------------+
| RGB                | (r, g, b, 1) | (r, g, b, 1)       | (r, g, b, 1) |
+--------------------+--------------+--------------------+--------------+
| RGBA               | (r, g, b, a) | (r, g, b, a)       | (r, g, b, a) |
+--------------------+--------------+--------------------+--------------+
| A                  | (0, 0, 0, a) | (0, 0, 0, a)       | (0, 0, 0, a) |
+--------------------+--------------+--------------------+--------------+
| L                  | (l, l, l, 1) | (l, l, l, 1)       | (l, l, l, 1) |
+--------------------+--------------+--------------------+--------------+
| LA                 | (l, l, l, a) | (l, l, l, a)       | (l, l, l, a) |
+--------------------+--------------+--------------------+--------------+
| I                  | (i, i, i, i) | (i, i, i, i)       | N/A          |
+--------------------+--------------+--------------------+--------------+
| UV                 | XXX TBD      | (0, 0, 0, 1)       | (u, v, 1, 1) |
|                    |              | [#envmap-bumpmap]_ |              |
+--------------------+--------------+--------------------+--------------+
| Z                  | XXX TBD      | (z, z, z, 1)       | (0, z, 0, 1) |
|                    |              | [#depth-tex-mode]_ |              |
+--------------------+--------------+--------------------+--------------+
| S                  | (s, s, s, s) | unknown            | unknown      |
+--------------------+--------------+--------------------+--------------+

.. [#envmap-bumpmap] http://www.opengl.org/registry/specs/ATI/envmap_bumpmap.txt
.. [#depth-tex-mode] the default is (z, z, z, 1) but may also be (0, 0, 0, z)
   or (z, z, z, z) depending on the value of GL_DEPTH_TEXTURE_MODE.
