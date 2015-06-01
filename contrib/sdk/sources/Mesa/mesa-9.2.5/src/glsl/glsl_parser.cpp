/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7.12-4996"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         _mesa_glsl_parse
#define yylex           _mesa_glsl_lex
#define yyerror         _mesa_glsl_error
#define yylval          _mesa_glsl_lval
#define yychar          _mesa_glsl_char
#define yydebug         _mesa_glsl_debug
#define yynerrs         _mesa_glsl_nerrs
#define yylloc          _mesa_glsl_lloc

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 1 "glsl_parser.yy"

/*
 * Copyright © 2008, 2009 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ast.h"
#include "glsl_parser_extras.h"
#include "glsl_types.h"
#include "main/context.h"

#undef yyerror

static void yyerror(YYLTYPE *loc, _mesa_glsl_parse_state *st, const char *msg)
{
   _mesa_glsl_error(loc, st, "%s", msg);
}

static int
_mesa_glsl_lex(YYSTYPE *val, YYLTYPE *loc, _mesa_glsl_parse_state *state)
{
   return _mesa_glsl_lexer_lex(val, loc, state->scanner);
}

/* Line 371 of yacc.c  */
#line 123 "glsl_parser.cpp"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "glsl_parser.h".  */
#ifndef YY__MESA_GLSL_SRC_GLSL_GLSL_PARSER_H_INCLUDED
# define YY__MESA_GLSL_SRC_GLSL_GLSL_PARSER_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int _mesa_glsl_debug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ATTRIBUTE = 258,
     CONST_TOK = 259,
     BOOL_TOK = 260,
     FLOAT_TOK = 261,
     INT_TOK = 262,
     UINT_TOK = 263,
     BREAK = 264,
     CONTINUE = 265,
     DO = 266,
     ELSE = 267,
     FOR = 268,
     IF = 269,
     DISCARD = 270,
     RETURN = 271,
     SWITCH = 272,
     CASE = 273,
     DEFAULT = 274,
     BVEC2 = 275,
     BVEC3 = 276,
     BVEC4 = 277,
     IVEC2 = 278,
     IVEC3 = 279,
     IVEC4 = 280,
     UVEC2 = 281,
     UVEC3 = 282,
     UVEC4 = 283,
     VEC2 = 284,
     VEC3 = 285,
     VEC4 = 286,
     CENTROID = 287,
     IN_TOK = 288,
     OUT_TOK = 289,
     INOUT_TOK = 290,
     UNIFORM = 291,
     VARYING = 292,
     NOPERSPECTIVE = 293,
     FLAT = 294,
     SMOOTH = 295,
     MAT2X2 = 296,
     MAT2X3 = 297,
     MAT2X4 = 298,
     MAT3X2 = 299,
     MAT3X3 = 300,
     MAT3X4 = 301,
     MAT4X2 = 302,
     MAT4X3 = 303,
     MAT4X4 = 304,
     SAMPLER1D = 305,
     SAMPLER2D = 306,
     SAMPLER3D = 307,
     SAMPLERCUBE = 308,
     SAMPLER1DSHADOW = 309,
     SAMPLER2DSHADOW = 310,
     SAMPLERCUBESHADOW = 311,
     SAMPLER1DARRAY = 312,
     SAMPLER2DARRAY = 313,
     SAMPLER1DARRAYSHADOW = 314,
     SAMPLER2DARRAYSHADOW = 315,
     SAMPLERCUBEARRAY = 316,
     SAMPLERCUBEARRAYSHADOW = 317,
     ISAMPLER1D = 318,
     ISAMPLER2D = 319,
     ISAMPLER3D = 320,
     ISAMPLERCUBE = 321,
     ISAMPLER1DARRAY = 322,
     ISAMPLER2DARRAY = 323,
     ISAMPLERCUBEARRAY = 324,
     USAMPLER1D = 325,
     USAMPLER2D = 326,
     USAMPLER3D = 327,
     USAMPLERCUBE = 328,
     USAMPLER1DARRAY = 329,
     USAMPLER2DARRAY = 330,
     USAMPLERCUBEARRAY = 331,
     SAMPLER2DRECT = 332,
     ISAMPLER2DRECT = 333,
     USAMPLER2DRECT = 334,
     SAMPLER2DRECTSHADOW = 335,
     SAMPLERBUFFER = 336,
     ISAMPLERBUFFER = 337,
     USAMPLERBUFFER = 338,
     SAMPLER2DMS = 339,
     ISAMPLER2DMS = 340,
     USAMPLER2DMS = 341,
     SAMPLER2DMSARRAY = 342,
     ISAMPLER2DMSARRAY = 343,
     USAMPLER2DMSARRAY = 344,
     SAMPLEREXTERNALOES = 345,
     STRUCT = 346,
     VOID_TOK = 347,
     WHILE = 348,
     IDENTIFIER = 349,
     TYPE_IDENTIFIER = 350,
     NEW_IDENTIFIER = 351,
     FLOATCONSTANT = 352,
     INTCONSTANT = 353,
     UINTCONSTANT = 354,
     BOOLCONSTANT = 355,
     FIELD_SELECTION = 356,
     LEFT_OP = 357,
     RIGHT_OP = 358,
     INC_OP = 359,
     DEC_OP = 360,
     LE_OP = 361,
     GE_OP = 362,
     EQ_OP = 363,
     NE_OP = 364,
     AND_OP = 365,
     OR_OP = 366,
     XOR_OP = 367,
     MUL_ASSIGN = 368,
     DIV_ASSIGN = 369,
     ADD_ASSIGN = 370,
     MOD_ASSIGN = 371,
     LEFT_ASSIGN = 372,
     RIGHT_ASSIGN = 373,
     AND_ASSIGN = 374,
     XOR_ASSIGN = 375,
     OR_ASSIGN = 376,
     SUB_ASSIGN = 377,
     INVARIANT = 378,
     LOWP = 379,
     MEDIUMP = 380,
     HIGHP = 381,
     SUPERP = 382,
     PRECISION = 383,
     VERSION_TOK = 384,
     EXTENSION = 385,
     LINE = 386,
     COLON = 387,
     EOL = 388,
     INTERFACE = 389,
     OUTPUT = 390,
     PRAGMA_DEBUG_ON = 391,
     PRAGMA_DEBUG_OFF = 392,
     PRAGMA_OPTIMIZE_ON = 393,
     PRAGMA_OPTIMIZE_OFF = 394,
     PRAGMA_INVARIANT_ALL = 395,
     LAYOUT_TOK = 396,
     ASM = 397,
     CLASS = 398,
     UNION = 399,
     ENUM = 400,
     TYPEDEF = 401,
     TEMPLATE = 402,
     THIS = 403,
     PACKED_TOK = 404,
     GOTO = 405,
     INLINE_TOK = 406,
     NOINLINE = 407,
     VOLATILE = 408,
     PUBLIC_TOK = 409,
     STATIC = 410,
     EXTERN = 411,
     EXTERNAL = 412,
     LONG_TOK = 413,
     SHORT_TOK = 414,
     DOUBLE_TOK = 415,
     HALF = 416,
     FIXED_TOK = 417,
     UNSIGNED = 418,
     INPUT_TOK = 419,
     OUPTUT = 420,
     HVEC2 = 421,
     HVEC3 = 422,
     HVEC4 = 423,
     DVEC2 = 424,
     DVEC3 = 425,
     DVEC4 = 426,
     FVEC2 = 427,
     FVEC3 = 428,
     FVEC4 = 429,
     SAMPLER3DRECT = 430,
     SIZEOF = 431,
     CAST = 432,
     NAMESPACE = 433,
     USING = 434,
     COHERENT = 435,
     RESTRICT = 436,
     READONLY = 437,
     WRITEONLY = 438,
     RESOURCE = 439,
     ATOMIC_UINT = 440,
     PATCH = 441,
     SAMPLE = 442,
     SUBROUTINE = 443,
     ERROR_TOK = 444,
     COMMON = 445,
     PARTITION = 446,
     ACTIVE = 447,
     FILTER = 448,
     IMAGE1D = 449,
     IMAGE2D = 450,
     IMAGE3D = 451,
     IMAGECUBE = 452,
     IMAGE1DARRAY = 453,
     IMAGE2DARRAY = 454,
     IIMAGE1D = 455,
     IIMAGE2D = 456,
     IIMAGE3D = 457,
     IIMAGECUBE = 458,
     IIMAGE1DARRAY = 459,
     IIMAGE2DARRAY = 460,
     UIMAGE1D = 461,
     UIMAGE2D = 462,
     UIMAGE3D = 463,
     UIMAGECUBE = 464,
     UIMAGE1DARRAY = 465,
     UIMAGE2DARRAY = 466,
     IMAGE1DSHADOW = 467,
     IMAGE2DSHADOW = 468,
     IMAGEBUFFER = 469,
     IIMAGEBUFFER = 470,
     UIMAGEBUFFER = 471,
     IMAGE1DARRAYSHADOW = 472,
     IMAGE2DARRAYSHADOW = 473,
     ROW_MAJOR = 474,
     THEN = 475
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 65 "glsl_parser.yy"

   int n;
   float real;
   const char *identifier;

   struct ast_type_qualifier type_qualifier;

   ast_node *node;
   ast_type_specifier *type_specifier;
   ast_fully_specified_type *fully_specified_type;
   ast_function *function;
   ast_parameter_declarator *parameter_declarator;
   ast_function_definition *function_definition;
   ast_compound_statement *compound_statement;
   ast_expression *expression;
   ast_declarator_list *declarator_list;
   ast_struct_specifier *struct_specifier;
   ast_declaration *declaration;
   ast_switch_body *switch_body;
   ast_case_label *case_label;
   ast_case_label_list *case_label_list;
   ast_case_statement *case_statement;
   ast_case_statement_list *case_statement_list;
   ast_interface_block *interface_block;

   struct {
      ast_node *cond;
      ast_expression *rest;
   } for_rest_statement;

   struct {
      ast_node *then_statement;
      ast_node *else_statement;
   } selection_rest_statement;


/* Line 387 of yacc.c  */
#line 423 "glsl_parser.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int _mesa_glsl_parse (void *YYPARSE_PARAM);
#else
int _mesa_glsl_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int _mesa_glsl_parse (struct _mesa_glsl_parse_state *state);
#else
int _mesa_glsl_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY__MESA_GLSL_SRC_GLSL_GLSL_PARSER_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 463 "glsl_parser.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif


/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3639

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  245
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  107
/* YYNRULES -- Number of rules.  */
#define YYNRULES  340
/* YYNRULES -- Number of states.  */
#define YYNSTATES  500

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   475

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   229,     2,     2,     2,   233,   236,     2,
     221,   222,   231,   227,   226,   228,   225,   232,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   240,   242,
     234,   241,   235,   239,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   223,     2,   224,   237,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   243,   238,   244,   230,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     9,    10,    14,    19,    22,    25,
      28,    31,    34,    35,    38,    40,    42,    44,    50,    52,
      55,    57,    59,    61,    63,    65,    67,    69,    73,    75,
      80,    82,    86,    89,    92,    94,    96,    98,   102,   105,
     108,   111,   113,   116,   120,   123,   125,   127,   129,   132,
     135,   138,   140,   143,   147,   150,   152,   155,   158,   161,
     163,   165,   167,   169,   171,   175,   179,   183,   185,   189,
     193,   195,   199,   203,   205,   209,   213,   217,   221,   223,
     227,   231,   233,   237,   239,   243,   245,   249,   251,   255,
     257,   261,   263,   267,   269,   275,   277,   281,   283,   285,
     287,   289,   291,   293,   295,   297,   299,   301,   303,   305,
     309,   311,   314,   317,   322,   324,   327,   329,   331,   334,
     338,   342,   345,   351,   354,   357,   358,   361,   364,   367,
     369,   371,   373,   375,   377,   381,   387,   394,   402,   411,
     417,   419,   422,   427,   433,   440,   448,   453,   456,   458,
     461,   466,   468,   472,   474,   476,   478,   482,   484,   486,
     488,   490,   492,   494,   496,   498,   500,   502,   504,   506,
     509,   512,   515,   518,   521,   524,   526,   528,   530,   532,
     534,   536,   538,   540,   544,   549,   551,   553,   555,   557,
     559,   561,   563,   565,   567,   569,   571,   573,   575,   577,
     579,   581,   583,   585,   587,   589,   591,   593,   595,   597,
     599,   601,   603,   605,   607,   609,   611,   613,   615,   617,
     619,   621,   623,   625,   627,   629,   631,   633,   635,   637,
     639,   641,   643,   645,   647,   649,   651,   653,   655,   657,
     659,   661,   663,   665,   667,   669,   671,   673,   675,   677,
     679,   681,   683,   685,   687,   689,   691,   693,   695,   701,
     706,   708,   711,   715,   717,   721,   723,   728,   730,   734,
     739,   741,   745,   747,   749,   751,   753,   755,   757,   759,
     761,   763,   766,   767,   772,   774,   776,   779,   783,   785,
     788,   790,   793,   799,   803,   805,   807,   812,   818,   821,
     825,   829,   832,   834,   837,   840,   843,   845,   848,   854,
     862,   869,   871,   873,   875,   876,   879,   883,   886,   889,
     892,   896,   899,   901,   903,   905,   907,   910,   912,   915,
     923,   925,   927,   929,   930,   932,   937,   941,   943,   946,
     950
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     246,     0,    -1,    -1,   248,   250,   247,   253,    -1,    -1,
     129,    98,   133,    -1,   129,    98,   251,   133,    -1,   136,
     133,    -1,   137,   133,    -1,   138,   133,    -1,   139,   133,
      -1,   140,   133,    -1,    -1,   250,   252,    -1,    94,    -1,
      95,    -1,    96,    -1,   130,   251,   132,   251,   133,    -1,
     343,    -1,   253,   343,    -1,    94,    -1,    96,    -1,   254,
      -1,    98,    -1,    99,    -1,    97,    -1,   100,    -1,   221,
     285,   222,    -1,   255,    -1,   256,   223,   257,   224,    -1,
     258,    -1,   256,   225,   251,    -1,   256,   104,    -1,   256,
     105,    -1,   285,    -1,   259,    -1,   260,    -1,   256,   225,
     265,    -1,   262,   222,    -1,   261,   222,    -1,   263,    92,
      -1,   263,    -1,   263,   283,    -1,   262,   226,   283,    -1,
     264,   221,    -1,   309,    -1,   254,    -1,   101,    -1,   267,
     222,    -1,   266,   222,    -1,   268,    92,    -1,   268,    -1,
     268,   283,    -1,   267,   226,   283,    -1,   254,   221,    -1,
     256,    -1,   104,   269,    -1,   105,   269,    -1,   270,   269,
      -1,   227,    -1,   228,    -1,   229,    -1,   230,    -1,   269,
      -1,   271,   231,   269,    -1,   271,   232,   269,    -1,   271,
     233,   269,    -1,   271,    -1,   272,   227,   271,    -1,   272,
     228,   271,    -1,   272,    -1,   273,   102,   272,    -1,   273,
     103,   272,    -1,   273,    -1,   274,   234,   273,    -1,   274,
     235,   273,    -1,   274,   106,   273,    -1,   274,   107,   273,
      -1,   274,    -1,   275,   108,   274,    -1,   275,   109,   274,
      -1,   275,    -1,   276,   236,   275,    -1,   276,    -1,   277,
     237,   276,    -1,   277,    -1,   278,   238,   277,    -1,   278,
      -1,   279,   110,   278,    -1,   279,    -1,   280,   112,   279,
      -1,   280,    -1,   281,   111,   280,    -1,   281,    -1,   281,
     239,   285,   240,   283,    -1,   282,    -1,   269,   284,   283,
      -1,   241,    -1,   113,    -1,   114,    -1,   116,    -1,   115,
      -1,   122,    -1,   117,    -1,   118,    -1,   119,    -1,   120,
      -1,   121,    -1,   283,    -1,   285,   226,   283,    -1,   282,
      -1,   288,   242,    -1,   297,   242,    -1,   128,   312,   309,
     242,    -1,   345,    -1,   289,   222,    -1,   291,    -1,   290,
      -1,   291,   293,    -1,   290,   226,   293,    -1,   299,   254,
     221,    -1,   309,   251,    -1,   309,   251,   223,   286,   224,
      -1,   294,   292,    -1,   294,   296,    -1,    -1,     4,   294,
      -1,   295,   294,    -1,   312,   294,    -1,    33,    -1,    34,
      -1,    35,    -1,   309,    -1,   298,    -1,   297,   226,   251,
      -1,   297,   226,   251,   223,   224,    -1,   297,   226,   251,
     223,   286,   224,    -1,   297,   226,   251,   223,   224,   241,
     318,    -1,   297,   226,   251,   223,   286,   224,   241,   318,
      -1,   297,   226,   251,   241,   318,    -1,   299,    -1,   299,
     251,    -1,   299,   251,   223,   224,    -1,   299,   251,   223,
     286,   224,    -1,   299,   251,   223,   224,   241,   318,    -1,
     299,   251,   223,   286,   224,   241,   318,    -1,   299,   251,
     241,   318,    -1,   123,   254,    -1,   309,    -1,   306,   309,
      -1,   141,   221,   301,   222,    -1,   303,    -1,   301,   226,
     303,    -1,    98,    -1,    99,    -1,   251,    -1,   251,   241,
     302,    -1,   304,    -1,   219,    -1,   149,    -1,    40,    -1,
      39,    -1,    38,    -1,   123,    -1,   307,    -1,   308,    -1,
     305,    -1,   300,    -1,   312,    -1,   123,   306,    -1,   305,
     306,    -1,   300,   306,    -1,   307,   306,    -1,   308,   306,
      -1,   312,   306,    -1,    32,    -1,     4,    -1,     3,    -1,
      37,    -1,    33,    -1,    34,    -1,    36,    -1,   310,    -1,
     310,   223,   224,    -1,   310,   223,   286,   224,    -1,   311,
      -1,   313,    -1,    95,    -1,    92,    -1,     6,    -1,     7,
      -1,     8,    -1,     5,    -1,    29,    -1,    30,    -1,    31,
      -1,    20,    -1,    21,    -1,    22,    -1,    23,    -1,    24,
      -1,    25,    -1,    26,    -1,    27,    -1,    28,    -1,    41,
      -1,    42,    -1,    43,    -1,    44,    -1,    45,    -1,    46,
      -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,    51,
      -1,    77,    -1,    52,    -1,    53,    -1,    90,    -1,    54,
      -1,    55,    -1,    80,    -1,    56,    -1,    57,    -1,    58,
      -1,    59,    -1,    60,    -1,    81,    -1,    61,    -1,    62,
      -1,    63,    -1,    64,    -1,    78,    -1,    65,    -1,    66,
      -1,    67,    -1,    68,    -1,    82,    -1,    69,    -1,    70,
      -1,    71,    -1,    79,    -1,    72,    -1,    73,    -1,    74,
      -1,    75,    -1,    83,    -1,    76,    -1,    84,    -1,    85,
      -1,    86,    -1,    87,    -1,    88,    -1,    89,    -1,   126,
      -1,   125,    -1,   124,    -1,    91,   251,   243,   314,   244,
      -1,    91,   243,   314,   244,    -1,   315,    -1,   314,   315,
      -1,   299,   316,   242,    -1,   317,    -1,   316,   226,   317,
      -1,   251,    -1,   251,   223,   286,   224,    -1,   283,    -1,
     243,   319,   244,    -1,   243,   319,   226,   244,    -1,   318,
      -1,   319,   226,   318,    -1,   287,    -1,   323,    -1,   322,
      -1,   320,    -1,   328,    -1,   329,    -1,   332,    -1,   338,
      -1,   342,    -1,   243,   244,    -1,    -1,   243,   324,   327,
     244,    -1,   326,    -1,   322,    -1,   243,   244,    -1,   243,
     327,   244,    -1,   321,    -1,   327,   321,    -1,   242,    -1,
     285,   242,    -1,    14,   221,   285,   222,   330,    -1,   321,
      12,   321,    -1,   321,    -1,   285,    -1,   299,   251,   241,
     318,    -1,    17,   221,   285,   222,   333,    -1,   243,   244,
      -1,   243,   337,   244,    -1,    18,   285,   240,    -1,    19,
     240,    -1,   334,    -1,   335,   334,    -1,   335,   321,    -1,
     336,   321,    -1,   336,    -1,   337,   336,    -1,    93,   221,
     331,   222,   325,    -1,    11,   321,    93,   221,   285,   222,
     242,    -1,    13,   221,   339,   341,   222,   325,    -1,   328,
      -1,   320,    -1,   331,    -1,    -1,   340,   242,    -1,   340,
     242,   285,    -1,    10,   242,    -1,     9,   242,    -1,    16,
     242,    -1,    16,   285,   242,    -1,    15,   242,    -1,   344,
      -1,   287,    -1,   249,    -1,   351,    -1,   288,   326,    -1,
     346,    -1,   300,   346,    -1,   347,    96,   243,   349,   244,
     348,   242,    -1,    33,    -1,    34,    -1,    36,    -1,    -1,
      96,    -1,    96,   223,   286,   224,    -1,    96,   223,   224,
      -1,   350,    -1,   350,   349,    -1,   299,   316,   242,    -1,
     300,    36,   242,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   263,   263,   262,   274,   276,   283,   293,   294,   295,
     296,   297,   310,   312,   316,   317,   318,   322,   331,   339,
     350,   351,   355,   362,   369,   376,   383,   390,   397,   398,
     404,   408,   415,   421,   430,   434,   438,   439,   448,   449,
     453,   454,   458,   464,   476,   480,   486,   493,   503,   504,
     508,   509,   513,   519,   531,   542,   543,   549,   555,   565,
     566,   567,   568,   572,   573,   579,   585,   594,   595,   601,
     610,   611,   617,   626,   627,   633,   639,   645,   654,   655,
     661,   670,   671,   680,   681,   690,   691,   700,   701,   710,
     711,   720,   721,   730,   731,   740,   741,   750,   751,   752,
     753,   754,   755,   756,   757,   758,   759,   760,   764,   768,
     784,   788,   793,   797,   802,   809,   813,   814,   818,   823,
     831,   845,   855,   870,   875,   888,   891,   899,   911,   924,
     929,   934,   943,   947,   948,   958,   968,   978,   993,  1008,
    1026,  1033,  1042,  1051,  1060,  1074,  1088,  1100,  1114,  1121,
    1132,  1139,  1140,  1150,  1151,  1155,  1231,  1280,  1302,  1307,
    1315,  1320,  1325,  1334,  1339,  1340,  1341,  1342,  1343,  1361,
    1374,  1402,  1425,  1440,  1460,  1474,  1482,  1487,  1492,  1497,
    1502,  1507,  1515,  1516,  1522,  1531,  1537,  1543,  1552,  1553,
    1554,  1555,  1556,  1557,  1558,  1559,  1560,  1561,  1562,  1563,
    1564,  1565,  1566,  1567,  1568,  1569,  1570,  1571,  1572,  1573,
    1574,  1575,  1576,  1577,  1578,  1579,  1580,  1581,  1582,  1583,
    1584,  1585,  1586,  1587,  1588,  1589,  1590,  1591,  1592,  1593,
    1594,  1595,  1596,  1597,  1598,  1599,  1600,  1601,  1602,  1603,
    1604,  1605,  1606,  1607,  1608,  1609,  1610,  1611,  1612,  1613,
    1614,  1615,  1616,  1617,  1618,  1622,  1627,  1632,  1640,  1648,
    1657,  1662,  1670,  1689,  1694,  1702,  1708,  1717,  1718,  1722,
    1729,  1736,  1743,  1749,  1750,  1754,  1755,  1756,  1757,  1758,
    1759,  1763,  1770,  1769,  1783,  1784,  1788,  1794,  1803,  1813,
    1825,  1831,  1840,  1849,  1854,  1862,  1866,  1884,  1892,  1897,
    1905,  1910,  1918,  1926,  1934,  1942,  1950,  1958,  1966,  1973,
    1980,  1990,  1991,  1995,  1997,  2003,  2008,  2017,  2023,  2029,
    2035,  2041,  2050,  2051,  2052,  2053,  2057,  2071,  2075,  2086,
    2183,  2188,  2193,  2202,  2206,  2211,  2216,  2227,  2232,  2240,
    2264
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ATTRIBUTE", "CONST_TOK", "BOOL_TOK",
  "FLOAT_TOK", "INT_TOK", "UINT_TOK", "BREAK", "CONTINUE", "DO", "ELSE",
  "FOR", "IF", "DISCARD", "RETURN", "SWITCH", "CASE", "DEFAULT", "BVEC2",
  "BVEC3", "BVEC4", "IVEC2", "IVEC3", "IVEC4", "UVEC2", "UVEC3", "UVEC4",
  "VEC2", "VEC3", "VEC4", "CENTROID", "IN_TOK", "OUT_TOK", "INOUT_TOK",
  "UNIFORM", "VARYING", "NOPERSPECTIVE", "FLAT", "SMOOTH", "MAT2X2",
  "MAT2X3", "MAT2X4", "MAT3X2", "MAT3X3", "MAT3X4", "MAT4X2", "MAT4X3",
  "MAT4X4", "SAMPLER1D", "SAMPLER2D", "SAMPLER3D", "SAMPLERCUBE",
  "SAMPLER1DSHADOW", "SAMPLER2DSHADOW", "SAMPLERCUBESHADOW",
  "SAMPLER1DARRAY", "SAMPLER2DARRAY", "SAMPLER1DARRAYSHADOW",
  "SAMPLER2DARRAYSHADOW", "SAMPLERCUBEARRAY", "SAMPLERCUBEARRAYSHADOW",
  "ISAMPLER1D", "ISAMPLER2D", "ISAMPLER3D", "ISAMPLERCUBE",
  "ISAMPLER1DARRAY", "ISAMPLER2DARRAY", "ISAMPLERCUBEARRAY", "USAMPLER1D",
  "USAMPLER2D", "USAMPLER3D", "USAMPLERCUBE", "USAMPLER1DARRAY",
  "USAMPLER2DARRAY", "USAMPLERCUBEARRAY", "SAMPLER2DRECT",
  "ISAMPLER2DRECT", "USAMPLER2DRECT", "SAMPLER2DRECTSHADOW",
  "SAMPLERBUFFER", "ISAMPLERBUFFER", "USAMPLERBUFFER", "SAMPLER2DMS",
  "ISAMPLER2DMS", "USAMPLER2DMS", "SAMPLER2DMSARRAY", "ISAMPLER2DMSARRAY",
  "USAMPLER2DMSARRAY", "SAMPLEREXTERNALOES", "STRUCT", "VOID_TOK", "WHILE",
  "IDENTIFIER", "TYPE_IDENTIFIER", "NEW_IDENTIFIER", "FLOATCONSTANT",
  "INTCONSTANT", "UINTCONSTANT", "BOOLCONSTANT", "FIELD_SELECTION",
  "LEFT_OP", "RIGHT_OP", "INC_OP", "DEC_OP", "LE_OP", "GE_OP", "EQ_OP",
  "NE_OP", "AND_OP", "OR_OP", "XOR_OP", "MUL_ASSIGN", "DIV_ASSIGN",
  "ADD_ASSIGN", "MOD_ASSIGN", "LEFT_ASSIGN", "RIGHT_ASSIGN", "AND_ASSIGN",
  "XOR_ASSIGN", "OR_ASSIGN", "SUB_ASSIGN", "INVARIANT", "LOWP", "MEDIUMP",
  "HIGHP", "SUPERP", "PRECISION", "VERSION_TOK", "EXTENSION", "LINE",
  "COLON", "EOL", "INTERFACE", "OUTPUT", "PRAGMA_DEBUG_ON",
  "PRAGMA_DEBUG_OFF", "PRAGMA_OPTIMIZE_ON", "PRAGMA_OPTIMIZE_OFF",
  "PRAGMA_INVARIANT_ALL", "LAYOUT_TOK", "ASM", "CLASS", "UNION", "ENUM",
  "TYPEDEF", "TEMPLATE", "THIS", "PACKED_TOK", "GOTO", "INLINE_TOK",
  "NOINLINE", "VOLATILE", "PUBLIC_TOK", "STATIC", "EXTERN", "EXTERNAL",
  "LONG_TOK", "SHORT_TOK", "DOUBLE_TOK", "HALF", "FIXED_TOK", "UNSIGNED",
  "INPUT_TOK", "OUPTUT", "HVEC2", "HVEC3", "HVEC4", "DVEC2", "DVEC3",
  "DVEC4", "FVEC2", "FVEC3", "FVEC4", "SAMPLER3DRECT", "SIZEOF", "CAST",
  "NAMESPACE", "USING", "COHERENT", "RESTRICT", "READONLY", "WRITEONLY",
  "RESOURCE", "ATOMIC_UINT", "PATCH", "SAMPLE", "SUBROUTINE", "ERROR_TOK",
  "COMMON", "PARTITION", "ACTIVE", "FILTER", "IMAGE1D", "IMAGE2D",
  "IMAGE3D", "IMAGECUBE", "IMAGE1DARRAY", "IMAGE2DARRAY", "IIMAGE1D",
  "IIMAGE2D", "IIMAGE3D", "IIMAGECUBE", "IIMAGE1DARRAY", "IIMAGE2DARRAY",
  "UIMAGE1D", "UIMAGE2D", "UIMAGE3D", "UIMAGECUBE", "UIMAGE1DARRAY",
  "UIMAGE2DARRAY", "IMAGE1DSHADOW", "IMAGE2DSHADOW", "IMAGEBUFFER",
  "IIMAGEBUFFER", "UIMAGEBUFFER", "IMAGE1DARRAYSHADOW",
  "IMAGE2DARRAYSHADOW", "ROW_MAJOR", "THEN", "'('", "')'", "'['", "']'",
  "'.'", "','", "'+'", "'-'", "'!'", "'~'", "'*'", "'/'", "'%'", "'<'",
  "'>'", "'&'", "'^'", "'|'", "'?'", "':'", "'='", "';'", "'{'", "'}'",
  "$accept", "translation_unit", "$@1", "version_statement",
  "pragma_statement", "extension_statement_list", "any_identifier",
  "extension_statement", "external_declaration_list",
  "variable_identifier", "primary_expression", "postfix_expression",
  "integer_expression", "function_call", "function_call_or_method",
  "function_call_generic", "function_call_header_no_parameters",
  "function_call_header_with_parameters", "function_call_header",
  "function_identifier", "method_call_generic",
  "method_call_header_no_parameters", "method_call_header_with_parameters",
  "method_call_header", "unary_expression", "unary_operator",
  "multiplicative_expression", "additive_expression", "shift_expression",
  "relational_expression", "equality_expression", "and_expression",
  "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_xor_expression",
  "logical_or_expression", "conditional_expression",
  "assignment_expression", "assignment_operator", "expression",
  "constant_expression", "declaration", "function_prototype",
  "function_declarator", "function_header_with_parameters",
  "function_header", "parameter_declarator", "parameter_declaration",
  "parameter_qualifier", "parameter_direction_qualifier",
  "parameter_type_specifier", "init_declarator_list", "single_declaration",
  "fully_specified_type", "layout_qualifier", "layout_qualifier_id_list",
  "integer_constant", "layout_qualifier_id",
  "interface_block_layout_qualifier", "interpolation_qualifier",
  "type_qualifier", "auxiliary_storage_qualifier", "storage_qualifier",
  "type_specifier", "type_specifier_nonarray",
  "basic_type_specifier_nonarray", "precision_qualifier",
  "struct_specifier", "struct_declaration_list", "struct_declaration",
  "struct_declarator_list", "struct_declarator", "initializer",
  "initializer_list", "declaration_statement", "statement",
  "simple_statement", "compound_statement", "$@2",
  "statement_no_new_scope", "compound_statement_no_new_scope",
  "statement_list", "expression_statement", "selection_statement",
  "selection_rest_statement", "condition", "switch_statement",
  "switch_body", "case_label", "case_label_list", "case_statement",
  "case_statement_list", "iteration_statement", "for_init_statement",
  "conditionopt", "for_rest_statement", "jump_statement",
  "external_declaration", "function_definition", "interface_block",
  "basic_interface_block", "interface_qualifier", "instance_name_opt",
  "member_list", "member_declaration", "layout_defaults", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,    40,    41,    91,    93,    46,    44,    43,    45,    33,
     126,    42,    47,    37,    60,    62,    38,    94,   124,    63,
      58,    61,    59,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   245,   247,   246,   248,   248,   248,   249,   249,   249,
     249,   249,   250,   250,   251,   251,   251,   252,   253,   253,
     254,   254,   255,   255,   255,   255,   255,   255,   256,   256,
     256,   256,   256,   256,   257,   258,   259,   259,   260,   260,
     261,   261,   262,   262,   263,   264,   264,   264,   265,   265,
     266,   266,   267,   267,   268,   269,   269,   269,   269,   270,
     270,   270,   270,   271,   271,   271,   271,   272,   272,   272,
     273,   273,   273,   274,   274,   274,   274,   274,   275,   275,
     275,   276,   276,   277,   277,   278,   278,   279,   279,   280,
     280,   281,   281,   282,   282,   283,   283,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   285,   285,
     286,   287,   287,   287,   287,   288,   289,   289,   290,   290,
     291,   292,   292,   293,   293,   294,   294,   294,   294,   295,
     295,   295,   296,   297,   297,   297,   297,   297,   297,   297,
     298,   298,   298,   298,   298,   298,   298,   298,   299,   299,
     300,   301,   301,   302,   302,   303,   303,   303,   304,   304,
     305,   305,   305,   306,   306,   306,   306,   306,   306,   306,
     306,   306,   306,   306,   306,   307,   308,   308,   308,   308,
     308,   308,   309,   309,   309,   310,   310,   310,   311,   311,
     311,   311,   311,   311,   311,   311,   311,   311,   311,   311,
     311,   311,   311,   311,   311,   311,   311,   311,   311,   311,
     311,   311,   311,   311,   311,   311,   311,   311,   311,   311,
     311,   311,   311,   311,   311,   311,   311,   311,   311,   311,
     311,   311,   311,   311,   311,   311,   311,   311,   311,   311,
     311,   311,   311,   311,   311,   311,   311,   311,   311,   311,
     311,   311,   311,   311,   311,   312,   312,   312,   313,   313,
     314,   314,   315,   316,   316,   317,   317,   318,   318,   318,
     319,   319,   320,   321,   321,   322,   322,   322,   322,   322,
     322,   323,   324,   323,   325,   325,   326,   326,   327,   327,
     328,   328,   329,   330,   330,   331,   331,   332,   333,   333,
     334,   334,   335,   335,   336,   336,   337,   337,   338,   338,
     338,   339,   339,   340,   340,   341,   341,   342,   342,   342,
     342,   342,   343,   343,   343,   343,   344,   345,   345,   346,
     347,   347,   347,   348,   348,   348,   348,   349,   349,   350,
     351
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     4,     0,     3,     4,     2,     2,     2,
       2,     2,     0,     2,     1,     1,     1,     5,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     4,
       1,     3,     2,     2,     1,     1,     1,     3,     2,     2,
       2,     1,     2,     3,     2,     1,     1,     1,     2,     2,
       2,     1,     2,     3,     2,     1,     2,     2,     2,     1,
       1,     1,     1,     1,     3,     3,     3,     1,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     3,     1,     3,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     5,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     2,     2,     4,     1,     2,     1,     1,     2,     3,
       3,     2,     5,     2,     2,     0,     2,     2,     2,     1,
       1,     1,     1,     1,     3,     5,     6,     7,     8,     5,
       1,     2,     4,     5,     6,     7,     4,     2,     1,     2,
       4,     1,     3,     1,     1,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     2,     2,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     5,     4,
       1,     2,     3,     1,     3,     1,     4,     1,     3,     4,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     0,     4,     1,     1,     2,     3,     1,     2,
       1,     2,     5,     3,     1,     1,     4,     5,     2,     3,
       3,     2,     1,     2,     2,     2,     1,     2,     5,     7,
       6,     1,     1,     1,     0,     2,     3,     2,     2,     2,
       3,     2,     1,     1,     1,     1,     2,     1,     2,     7,
       1,     1,     1,     0,     1,     4,     3,     1,     2,     3,
       3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       4,     0,     0,    12,     0,     1,     2,    14,    15,    16,
       5,     0,     0,     0,    13,     6,     0,   177,   176,   192,
     189,   190,   191,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   193,   194,   195,   175,   179,   180,   181,   178,
     162,   161,   160,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   217,   218,   220,   221,   223,   224,
     225,   226,   227,   229,   230,   231,   232,   234,   235,   236,
     237,   239,   240,   241,   243,   244,   245,   246,   248,   216,
     233,   242,   222,   228,   238,   247,   249,   250,   251,   252,
     253,   254,   219,     0,   188,   187,   163,   257,   256,   255,
       0,     0,     0,     0,     0,     0,     0,   324,     3,   323,
       0,     0,   117,   125,     0,   133,   140,   167,   166,     0,
     164,   165,   148,   182,   185,   168,   186,    18,   322,   114,
     327,     0,   325,     0,     0,     0,   179,   180,   181,    20,
      21,   163,   147,   167,   169,     0,     7,     8,     9,    10,
      11,     0,    19,   111,     0,   326,   115,   125,   125,   129,
     130,   131,   118,     0,   125,   125,     0,   112,    14,    16,
     141,     0,   181,   171,   328,   170,   149,   172,   173,     0,
     174,     0,     0,     0,     0,   260,     0,     0,   159,   158,
     155,     0,   151,   157,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    25,    23,    24,    26,    47,     0,     0,
       0,    59,    60,    61,    62,   290,   282,   286,    22,    28,
      55,    30,    35,    36,     0,     0,    41,     0,    63,     0,
      67,    70,    73,    78,    81,    83,    85,    87,    89,    91,
      93,    95,   108,     0,   272,     0,   167,   148,   275,   288,
     274,   273,     0,   276,   277,   278,   279,   280,   119,   126,
     123,   124,   132,   127,   128,   134,     0,     0,   120,   340,
     183,    63,   110,     0,    45,     0,    17,   265,     0,   263,
     259,   261,     0,   113,     0,   150,     0,   318,   317,     0,
       0,     0,   321,   319,     0,     0,     0,    56,    57,     0,
     281,     0,    32,    33,     0,     0,    39,    38,     0,   188,
      42,    44,    98,    99,   101,   100,   103,   104,   105,   106,
     107,   102,    97,     0,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   291,   287,   289,   121,
       0,     0,   142,     0,     0,   267,   146,   184,     0,     0,
     337,     0,     0,   262,   258,   153,   154,   156,   152,     0,
     312,   311,   314,     0,   320,     0,   295,     0,     0,    27,
       0,     0,    34,    31,     0,    37,     0,     0,    51,    43,
      96,    64,    65,    66,    68,    69,    71,    72,    76,    77,
      74,    75,    79,    80,    82,    84,    86,    88,    90,    92,
       0,   109,     0,   135,     0,   139,     0,   143,   270,     0,
       0,   333,   338,     0,   264,     0,   313,     0,     0,     0,
       0,     0,     0,   283,    29,    54,    49,    48,     0,   188,
      52,     0,     0,     0,   136,   144,     0,     0,   268,   339,
     334,     0,   266,     0,   315,     0,   294,   292,     0,   297,
       0,   285,   308,   284,    53,    94,   122,   137,     0,   145,
     269,   271,     0,   329,     0,   316,   310,     0,     0,     0,
     298,   302,     0,   306,     0,   296,   138,   336,     0,   309,
     293,     0,   301,   304,   303,   305,   299,   307,   335,   300
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,    13,     3,   107,     6,   277,    14,   108,   218,
     219,   220,   381,   221,   222,   223,   224,   225,   226,   227,
     385,   386,   387,   388,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   323,
     243,   273,   244,   245,   111,   112,   113,   260,   162,   163,
     164,   261,   114,   115,   116,   143,   191,   367,   192,   193,
     118,   119,   120,   121,   274,   123,   124,   125,   126,   184,
     185,   278,   279,   356,   419,   248,   249,   250,   251,   301,
     462,   463,   252,   253,   254,   457,   378,   255,   459,   481,
     482,   483,   484,   256,   372,   427,   428,   257,   127,   128,
     129,   130,   131,   451,   359,   360,   132
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -401
static const yytype_int16 yypact[] =
{
    -110,   -39,    82,  -401,   -21,  -401,   -22,  -401,  -401,  -401,
    -401,   -23,    -5,  3372,  -401,  -401,   -16,  -401,  -401,  -401,
    -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,
    -401,  -401,  -401,  -401,  -401,  -401,    23,    34,    50,  -401,
    -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,
    -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,
    -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,
    -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,
    -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,
    -401,  -401,  -401,   -71,  -401,  -401,   289,  -401,  -401,  -401,
     134,     5,    15,    36,    41,    43,   -43,  -401,  3372,  -401,
     -61,   -38,   -36,     2,  -197,  -401,   170,   215,   312,   835,
     312,   312,  -401,    -6,  -401,   312,  -401,  -401,  -401,  -401,
    -401,   129,  -401,    -5,  3498,   -10,  -401,  -401,  -401,  -401,
    -401,   312,  -401,   312,  -401,   835,  -401,  -401,  -401,  -401,
    -401,   -42,  -401,  -401,   484,  -401,  -401,    37,    37,  -401,
    -401,  -401,  -401,   835,    37,    37,    -5,  -401,     6,     7,
    -190,     9,   -95,  -401,  -401,  -401,  -401,  -401,  -401,  2511,
    -401,    -3,   105,    -5,  1095,  -401,  3498,     3,  -401,  -401,
      26,  -142,  -401,  -401,     8,    14,  1589,    47,    49,    29,
    2393,    55,    59,  -401,  -401,  -401,  -401,  -401,  1939,  1939,
    1939,  -401,  -401,  -401,  -401,  -401,     0,  -401,    60,  -401,
     -83,  -401,  -401,  -401,    62,  -121,  3144,    65,   360,  1939,
      42,  -171,    57,   -80,    56,    53,    58,    40,   188,   187,
     -81,  -401,  -401,  -165,  -401,    63,   583,    80,  -401,  -401,
    -401,  -401,   726,  -401,  -401,  -401,  -401,  -401,  -401,  -401,
    -401,  -401,    -5,  -401,  -401,  -177,  2832,  2285,  -401,  -401,
    -401,  -401,  -401,    83,  -401,  3498,  -401,    81,  -159,  -401,
    -401,  -401,  1219,  -401,    94,  -401,   -42,  -401,  -401,   216,
    2058,  1939,  -401,  -401,  -157,  1939,  2621,  -401,  -401,  -113,
    -401,  1589,  -401,  -401,  1939,   170,  -401,  -401,  1939,    86,
    -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,
    -401,  -401,  -401,  1939,  -401,  1939,  1939,  1939,  1939,  1939,
    1939,  1939,  1939,  1939,  1939,  1939,  1939,  1939,  1939,  1939,
    1939,  1939,  1939,  1939,  1939,  1939,  -401,  -401,  -401,    87,
    2933,  2285,    71,    89,  2285,  -401,  -401,  -401,    -5,    70,
    3498,  1939,    -5,  -401,  -401,  -401,  -401,  -401,  -401,    97,
    -401,  -401,  2621,  -104,  -401,   -97,    93,    -5,    98,  -401,
     968,   100,    93,  -401,   110,  -401,   111,   -91,  3254,  -401,
    -401,  -401,  -401,  -401,    42,    42,  -171,  -171,    57,    57,
      57,    57,   -80,   -80,    56,    53,    58,    40,   188,   187,
    -178,  -401,  1939,    91,   112,  -401,  2285,   102,  -401,  -176,
    -150,   238,  -401,   130,  -401,  1939,  -401,   113,   131,  1589,
      92,   116,  1830,  -401,  -401,  -401,  -401,  -401,  1939,   136,
    -401,  1939,   137,  2285,   119,  -401,  2285,  2167,  -401,  -401,
     139,   121,  -401,   -69,  1939,  1830,   352,  -401,   -15,  -401,
    2285,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  2285,  -401,
    -401,  -401,  3043,  -401,   124,    93,  -401,  1589,  1939,   128,
    -401,  -401,  1348,  1589,    -7,  -401,  -401,  -401,   146,  -401,
    -401,  -140,  -401,  -401,  -401,  -401,  -401,  1589,  -401,  -401
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -401,  -401,  -401,  -401,  -401,  -401,     1,  -401,  -401,   -82,
    -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,  -401,
    -401,  -401,  -401,  -401,  -130,  -401,  -114,  -109,  -146,  -101,
      33,    38,    32,    35,    44,    30,  -401,  -170,   -51,  -401,
    -193,  -246,    25,    31,  -401,  -401,  -401,  -401,   222,   -77,
    -401,  -401,  -401,  -401,  -126,   -11,  -401,  -401,   103,  -401,
    -401,   -78,  -401,  -401,   -13,  -401,  -401,   217,  -401,   194,
    -168,    39,    22,  -323,  -401,   101,  -186,  -400,  -401,  -401,
     -67,   283,    99,   104,  -401,  -401,    27,  -401,  -401,   -84,
    -401,   -79,  -401,  -401,  -401,  -401,  -401,  -401,   293,  -401,
    -401,  -102,  -401,  -401,    46,  -401,  -401
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -333
static const yytype_int16 yytable[] =
{
     122,  -332,   117,   478,   479,    11,   158,   294,   183,   272,
     289,   478,   479,    16,   142,   174,   281,   299,   144,     1,
     353,   302,   303,     7,     8,     9,   332,   333,   415,   166,
     343,   418,   461,   266,   171,   159,   160,   161,   109,   173,
     175,   158,   177,   178,   110,   167,   350,   180,   345,   271,
     447,   267,     7,     8,     9,   461,   328,   329,   183,     4,
     183,   345,   441,   144,   351,   173,   348,   362,   448,   345,
     159,   160,   161,     7,     8,     9,   362,   346,   297,   298,
     285,   259,     5,   363,   286,   374,   345,   263,   264,     7,
       8,     9,   449,   445,   135,   122,   272,   117,   373,   324,
     499,   307,   375,   376,   414,   308,   176,   188,    12,   379,
      15,   382,    10,   345,   281,   423,   133,   170,   429,  -330,
     467,   122,   345,   469,   471,   430,    97,    98,    99,   345,
    -331,   437,   187,   109,   182,   438,   271,   485,   146,   110,
     304,   247,   305,   246,   174,   486,  -332,   269,   147,   358,
     262,   410,   190,   474,   334,   335,   183,   345,   344,   330,
     331,    97,    98,    99,   336,   337,   442,   265,   173,   148,
     377,   122,   134,   122,   149,   310,   150,   189,   151,   376,
     272,   153,   154,   247,   156,   246,   398,   399,   400,   401,
     157,   272,   365,   366,   348,   391,   392,   393,   271,   271,
     271,   271,   271,   271,   271,   271,   271,   271,   271,   271,
     271,   271,   271,   271,   394,   395,   355,   179,    17,    18,
     271,   396,   397,   384,  -116,   181,   488,   -20,   -21,   480,
     268,   271,   453,   186,   358,   402,   403,   496,   276,   247,
     275,   246,   272,   456,   300,   283,   377,    35,    36,    37,
     287,   172,    39,    40,    41,    42,   288,   389,    97,    98,
      99,   475,   122,   349,   168,     8,   169,   284,   290,   122,
     291,   292,   390,   325,   326,   327,   295,   247,   340,   246,
     296,   -46,   271,   247,   306,   491,   311,   190,   247,   338,
     246,   490,    17,    18,   411,   339,   493,   495,   341,   342,
     355,   -45,   272,   355,   361,   153,   383,   357,   -40,   369,
     412,   495,   416,   417,   421,    17,    18,   145,   425,   345,
     432,    35,   136,   137,   434,   138,    39,    40,    41,    42,
     165,   435,   443,   436,   450,   458,   444,   440,   141,    97,
      98,    99,   271,   446,    35,   136,   137,   122,   138,    39,
      40,    41,    42,   455,   452,   454,   106,   460,   -50,   247,
     468,   466,   472,   473,   477,   355,   489,   247,   492,   246,
     498,   404,   406,   409,   165,   165,   407,   405,   431,   258,
     282,   165,   165,   139,   424,   140,   408,   464,   476,   368,
     465,   370,   355,   155,   371,   355,   355,   420,   494,   426,
     380,   152,     0,     0,     0,   497,   422,     0,     0,   355,
       0,     0,   141,    97,    98,    99,   247,   355,   246,   247,
       0,   246,     0,     0,     0,     0,     0,     0,     0,     0,
     106,     0,     0,     0,     0,   141,    97,    98,    99,     0,
       0,     0,   247,     0,   246,     0,     0,     0,     0,     0,
       0,     0,     0,   106,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   247,     0,   246,     0,     0,   247,
     247,   246,   246,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,     0,   247,     0,   246,    17,    18,    19,
      20,    21,    22,   194,   195,   196,     0,   197,   198,   199,
     200,   201,     0,     0,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,     0,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,   202,   139,    95,
     140,   203,   204,   205,   206,   207,    17,    18,   208,   209,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   322,     0,     0,     0,     0,     0,    96,    97,    98,
      99,     0,   100,     0,     0,    35,    36,    37,     0,    38,
      39,    40,    41,    42,     0,   106,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   210,   141,    97,    98,    99,
       0,   211,   212,   213,   214,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   106,     0,   215,   216,   217,    17,
      18,    19,    20,    21,    22,   194,   195,   196,     0,   197,
     198,   199,   200,   201,     0,     0,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,     0,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,   202,
     139,    95,   140,   203,   204,   205,   206,   207,     0,     0,
     208,   209,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    20,    21,    22,     0,     0,     0,     0,     0,    96,
      97,    98,    99,     0,   100,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,   106,     0,     0,
       0,     0,     0,     0,     0,     0,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,     0,     0,
      95,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   210,     0,     0,
       0,     0,     0,   211,   212,   213,   214,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   215,   216,
     347,    17,    18,    19,    20,    21,    22,   194,   195,   196,
       0,   197,   198,   199,   200,   201,     0,     0,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,     0,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,   202,   139,    95,   140,   203,   204,   205,   206,   207,
       0,     0,   208,   209,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    96,    97,    98,    99,     0,   100,     0,    17,    18,
      19,    20,    21,    22,     0,     0,     0,     0,     0,   106,
       0,     0,     0,     0,     0,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,   136,   137,
       0,   138,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,     0,   210,
      95,     0,     0,     0,     0,   211,   212,   213,   214,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     215,   216,   433,     0,     0,     0,     0,     0,   141,    97,
      98,    99,    17,    18,    19,    20,    21,    22,     0,     0,
       0,     0,     0,     0,     0,     0,   106,     0,     0,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,   136,   137,     0,   138,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,     0,     0,    95,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   280,
       0,     0,   141,    97,    98,    99,     0,     0,     0,     0,
       0,    17,    18,    19,    20,    21,    22,   194,   195,   196,
     106,   197,   198,   199,   200,   201,   478,   479,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,     0,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,   202,   139,    95,   140,   203,   204,   205,   206,   207,
       0,     0,   208,   209,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   364,     0,     0,     0,     0,     0,     0,
       0,    96,    97,    98,    99,     0,   100,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   106,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   210,
       0,     0,     0,     0,     0,   211,   212,   213,   214,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     215,   216,    17,    18,    19,    20,    21,    22,   194,   195,
     196,     0,   197,   198,   199,   200,   201,     0,     0,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,     0,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,   202,   139,    95,   140,   203,   204,   205,   206,
     207,     0,     0,   208,   209,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    96,    97,    98,    99,     0,   100,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     106,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     210,     0,     0,     0,     0,     0,   211,   212,   213,   214,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   215,   216,    17,    18,    19,    20,    21,    22,   194,
     195,   196,     0,   197,   198,   199,   200,   201,     0,     0,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,     0,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,   202,   139,    95,   140,   203,   204,   205,
     206,   207,     0,     0,   208,   209,     0,     0,     0,     0,
       0,     0,     0,     0,    19,    20,    21,    22,     0,     0,
       0,     0,     0,    96,    97,    98,    99,     0,   100,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,   106,     0,     0,     0,     0,     0,     0,     0,     0,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,     0,   139,    95,   140,   203,   204,   205,   206,
     207,     0,     0,   208,   209,     0,     0,     0,     0,     0,
       0,   210,     0,     0,     0,     0,     0,   211,   212,   213,
     214,    17,    18,    19,    20,    21,    22,     0,     0,     0,
       0,     0,   215,   154,     0,     0,     0,     0,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,     0,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,     0,   139,    95,   140,   203,   204,   205,   206,   207,
     210,     0,   208,   209,     0,     0,   211,   212,   213,   214,
       0,     0,    19,    20,    21,    22,     0,     0,     0,     0,
       0,    96,    97,    98,    99,     0,   100,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,   106,
       0,     0,     0,     0,     0,     0,     0,     0,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
       0,   139,    95,   140,   203,   204,   205,   206,   207,     0,
       0,   208,   209,     0,     0,     0,     0,     0,     0,   210,
       0,     0,     0,     0,     0,   211,   212,   213,   214,     0,
      19,    20,    21,    22,     0,     0,     0,     0,     0,     0,
     215,     0,     0,     0,     0,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,     0,   139,
      95,   140,   203,   204,   205,   206,   207,     0,   210,   208,
     209,     0,     0,     0,   211,   212,   213,   214,    19,    20,
      21,    22,     0,     0,     0,     0,     0,     0,     0,     0,
     354,   470,     0,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,     0,   139,    95,   140,
     203,   204,   205,   206,   207,     0,     0,   208,   209,     0,
       0,     0,     0,     0,     0,     0,   210,     0,     0,     0,
       0,     0,   211,   212,   213,   214,    19,    20,    21,    22,
       0,     0,     0,     0,     0,     0,     0,     0,   354,     0,
       0,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,     0,   139,    95,   140,   203,   204,
     205,   206,   207,     0,   210,   208,   209,     0,     0,     0,
     211,   212,   213,   214,    17,    18,    19,    20,    21,    22,
       0,     0,     0,     0,     0,   293,     0,     0,     0,     0,
       0,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,   136,   137,     0,   138,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,     0,   139,    95,   140,   203,   204,
     205,   206,   207,     0,     0,   208,   209,     0,     0,     0,
       0,     0,   210,     0,     0,   270,     0,     0,   211,   212,
     213,   214,     0,     0,   141,    97,    98,    99,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   106,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    19,    20,    21,
      22,     0,   210,     0,     0,     0,     0,     0,   211,   212,
     213,   214,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,     0,   139,    95,   140,   203,
     204,   205,   206,   207,     0,     0,   208,   209,    19,    20,
      21,    22,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,     0,   139,    95,   140,
     203,   204,   205,   206,   207,     0,     0,   208,   209,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    19,    20,
      21,    22,     0,   210,     0,     0,   352,     0,     0,   211,
     212,   213,   214,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,     0,   139,    95,   140,
     203,   204,   205,   206,   207,     0,     0,   208,   209,    19,
      20,    21,    22,     0,   210,     0,     0,   413,     0,     0,
     211,   212,   213,   214,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,   309,     0,   139,    95,
     140,   203,   204,   205,   206,   207,     0,     0,   208,   209,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    19,
      20,    21,    22,     0,   210,     0,     0,   487,     0,     0,
     211,   212,   213,   214,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,   439,     0,   139,    95,
     140,   203,   204,   205,   206,   207,     0,     0,   208,   209,
       0,     0,     0,     0,     0,   210,     0,     0,     0,     0,
       0,   211,   212,   213,   214,    17,    18,    19,    20,    21,
      22,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,     0,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,     0,     0,    95,     0,     0,
       0,     0,     0,     0,     0,   210,     0,     0,     0,     0,
       0,   211,   212,   213,   214,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    96,    97,    98,    99,     0,
     100,    17,    18,    19,    20,    21,    22,     0,   101,   102,
     103,   104,   105,   106,     0,     0,     0,     0,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,   136,   137,     0,   138,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,     0,     0,    95,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   141,    97,    98,    99,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   106
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-401)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      13,    96,    13,    18,    19,     4,     4,   200,   134,   179,
     196,    18,    19,    12,    96,   117,   184,   210,    96,   129,
     266,   104,   105,    94,    95,    96,   106,   107,   351,   226,
     111,   354,   432,   223,   116,    33,    34,    35,    13,   117,
     118,     4,   120,   121,    13,   242,   223,   125,   226,   179,
     226,   241,    94,    95,    96,   455,   227,   228,   184,    98,
     186,   226,   240,   141,   241,   143,   252,   226,   244,   226,
      33,    34,    35,    94,    95,    96,   226,   242,   208,   209,
     222,   158,     0,   242,   226,   242,   226,   164,   165,    94,
      95,    96,   242,   416,    93,   108,   266,   108,   291,   229,
     240,   222,   295,   296,   350,   226,   119,   149,   130,   222,
     133,   304,   133,   226,   282,   361,   132,   116,   222,    96,
     443,   134,   226,   446,   447,   222,   124,   125,   126,   226,
      96,   222,   145,   108,   133,   226,   266,   460,   133,   108,
     223,   154,   225,   154,   246,   468,    96,   242,   133,   275,
     163,   344,   151,   222,   234,   235,   282,   226,   239,   102,
     103,   124,   125,   126,   108,   109,   412,   166,   246,   133,
     296,   184,   243,   186,   133,   226,   133,   219,   221,   372,
     350,   242,   243,   196,   222,   196,   332,   333,   334,   335,
     226,   361,    98,    99,   380,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   342,   343,   328,   329,   267,   223,     3,     4,
     350,   330,   331,   305,   222,    96,   472,   221,   221,   244,
     221,   361,   425,   243,   360,   336,   337,   244,   133,   252,
     243,   252,   412,   429,   244,   242,   372,    32,    33,    34,
     242,    36,    37,    38,    39,    40,   242,   308,   124,   125,
     126,   454,   275,   262,    94,    95,    96,   241,   221,   282,
     221,   242,   323,   231,   232,   233,   221,   290,   238,   290,
     221,   221,   412,   296,   222,   478,   221,   286,   301,   236,
     301,   477,     3,     4,   345,   237,   482,   483,   110,   112,
     351,   221,   472,   354,   223,   242,   305,   224,   222,    93,
     223,   497,   241,   224,   244,     3,     4,   100,   221,   226,
     222,    32,    33,    34,   224,    36,    37,    38,    39,    40,
     113,   221,   241,   222,    96,   243,   224,   388,   123,   124,
     125,   126,   472,   241,    32,    33,    34,   360,    36,    37,
      38,    39,    40,   222,   224,   242,   141,   241,   222,   372,
     241,   224,   223,   242,    12,   416,   242,   380,   240,   380,
     224,   338,   340,   343,   157,   158,   341,   339,   377,   157,
     186,   164,   165,    94,   362,    96,   342,   438,   455,   286,
     441,   290,   443,   110,   290,   446,   447,   358,   482,   372,
     301,   108,    -1,    -1,    -1,   484,   360,    -1,    -1,   460,
      -1,    -1,   123,   124,   125,   126,   429,   468,   429,   432,
      -1,   432,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     141,    -1,    -1,    -1,    -1,   123,   124,   125,   126,    -1,
      -1,    -1,   455,    -1,   455,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   141,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   477,    -1,   477,    -1,    -1,   482,
     483,   482,   483,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,    -1,   497,    -1,   497,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    -1,    13,    14,    15,
      16,    17,    -1,    -1,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    -1,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,     3,     4,   104,   105,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   241,    -1,    -1,    -1,    -1,    -1,   123,   124,   125,
     126,    -1,   128,    -1,    -1,    32,    33,    34,    -1,    36,
      37,    38,    39,    40,    -1,   141,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   221,   123,   124,   125,   126,
      -1,   227,   228,   229,   230,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   141,    -1,   242,   243,   244,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    -1,    13,
      14,    15,    16,    17,    -1,    -1,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    -1,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,    -1,    -1,
     104,   105,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,   123,
     124,   125,   126,    -1,   128,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,   141,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    -1,    -1,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   221,    -1,    -1,
      -1,    -1,    -1,   227,   228,   229,   230,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   242,   243,
     244,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      -1,    13,    14,    15,    16,    17,    -1,    -1,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
      -1,    -1,   104,   105,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   123,   124,   125,   126,    -1,   128,    -1,     3,     4,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,   141,
      -1,    -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      -1,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    -1,   221,
      95,    -1,    -1,    -1,    -1,   227,   228,   229,   230,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     242,   243,   244,    -1,    -1,    -1,    -1,    -1,   123,   124,
     125,   126,     3,     4,     5,     6,     7,     8,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   141,    -1,    -1,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   244,
      -1,    -1,   123,   124,   125,   126,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
     141,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
      -1,    -1,   104,   105,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   123,   124,   125,   126,    -1,   128,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   141,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   221,
      -1,    -1,    -1,    -1,    -1,   227,   228,   229,   230,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     242,   243,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    -1,    13,    14,    15,    16,    17,    -1,    -1,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,    -1,    -1,   104,   105,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   123,   124,   125,   126,    -1,   128,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     141,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     221,    -1,    -1,    -1,    -1,    -1,   227,   228,   229,   230,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   242,   243,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    -1,    13,    14,    15,    16,    17,    -1,    -1,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    -1,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,    -1,    -1,   104,   105,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     5,     6,     7,     8,    -1,    -1,
      -1,    -1,    -1,   123,   124,   125,   126,    -1,   128,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,   141,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    -1,    94,    95,    96,    97,    98,    99,   100,
     101,    -1,    -1,   104,   105,    -1,    -1,    -1,    -1,    -1,
      -1,   221,    -1,    -1,    -1,    -1,    -1,   227,   228,   229,
     230,     3,     4,     5,     6,     7,     8,    -1,    -1,    -1,
      -1,    -1,   242,   243,    -1,    -1,    -1,    -1,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    -1,    94,    95,    96,    97,    98,    99,   100,   101,
     221,    -1,   104,   105,    -1,    -1,   227,   228,   229,   230,
      -1,    -1,     5,     6,     7,     8,    -1,    -1,    -1,    -1,
      -1,   123,   124,   125,   126,    -1,   128,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,   141,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      -1,    94,    95,    96,    97,    98,    99,   100,   101,    -1,
      -1,   104,   105,    -1,    -1,    -1,    -1,    -1,    -1,   221,
      -1,    -1,    -1,    -1,    -1,   227,   228,   229,   230,    -1,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,    -1,
     242,    -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    -1,    94,
      95,    96,    97,    98,    99,   100,   101,    -1,   221,   104,
     105,    -1,    -1,    -1,   227,   228,   229,   230,     5,     6,
       7,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     243,   244,    -1,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    -1,    94,    95,    96,
      97,    98,    99,   100,   101,    -1,    -1,   104,   105,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   221,    -1,    -1,    -1,
      -1,    -1,   227,   228,   229,   230,     5,     6,     7,     8,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   243,    -1,
      -1,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    -1,    94,    95,    96,    97,    98,
      99,   100,   101,    -1,   221,   104,   105,    -1,    -1,    -1,
     227,   228,   229,   230,     3,     4,     5,     6,     7,     8,
      -1,    -1,    -1,    -1,    -1,   242,    -1,    -1,    -1,    -1,
      -1,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    -1,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    -1,    94,    95,    96,    97,    98,
      99,   100,   101,    -1,    -1,   104,   105,    -1,    -1,    -1,
      -1,    -1,   221,    -1,    -1,   224,    -1,    -1,   227,   228,
     229,   230,    -1,    -1,   123,   124,   125,   126,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   141,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,     6,     7,
       8,    -1,   221,    -1,    -1,    -1,    -1,    -1,   227,   228,
     229,   230,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    -1,    94,    95,    96,    97,
      98,    99,   100,   101,    -1,    -1,   104,   105,     5,     6,
       7,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    -1,    94,    95,    96,
      97,    98,    99,   100,   101,    -1,    -1,   104,   105,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,     6,
       7,     8,    -1,   221,    -1,    -1,   224,    -1,    -1,   227,
     228,   229,   230,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    -1,    94,    95,    96,
      97,    98,    99,   100,   101,    -1,    -1,   104,   105,     5,
       6,     7,     8,    -1,   221,    -1,    -1,   224,    -1,    -1,
     227,   228,   229,   230,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    -1,    94,    95,
      96,    97,    98,    99,   100,   101,    -1,    -1,   104,   105,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,
       6,     7,     8,    -1,   221,    -1,    -1,   224,    -1,    -1,
     227,   228,   229,   230,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    -1,    94,    95,
      96,    97,    98,    99,   100,   101,    -1,    -1,   104,   105,
      -1,    -1,    -1,    -1,    -1,   221,    -1,    -1,    -1,    -1,
      -1,   227,   228,   229,   230,     3,     4,     5,     6,     7,
       8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    -1,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    -1,    -1,    95,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   221,    -1,    -1,    -1,    -1,
      -1,   227,   228,   229,   230,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   123,   124,   125,   126,    -1,
     128,     3,     4,     5,     6,     7,     8,    -1,   136,   137,
     138,   139,   140,   141,    -1,    -1,    -1,    -1,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   123,   124,   125,   126,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   141
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   129,   246,   248,    98,     0,   250,    94,    95,    96,
     133,   251,   130,   247,   252,   133,   251,     3,     4,     5,
       6,     7,     8,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    95,   123,   124,   125,   126,
     128,   136,   137,   138,   139,   140,   141,   249,   253,   287,
     288,   289,   290,   291,   297,   298,   299,   300,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   343,   344,   345,
     346,   347,   351,   132,   243,   251,    33,    34,    36,    94,
      96,   123,   254,   300,   306,   312,   133,   133,   133,   133,
     133,   221,   343,   242,   243,   326,   222,   226,     4,    33,
      34,    35,   293,   294,   295,   312,   226,   242,    94,    96,
     251,   254,    36,   306,   346,   306,   309,   306,   306,   223,
     306,    96,   251,   299,   314,   315,   243,   309,   149,   219,
     251,   301,   303,   304,     9,    10,    11,    13,    14,    15,
      16,    17,    93,    97,    98,    99,   100,   101,   104,   105,
     221,   227,   228,   229,   230,   242,   243,   244,   254,   255,
     256,   258,   259,   260,   261,   262,   263,   264,   269,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   280,
     281,   282,   283,   285,   287,   288,   300,   309,   320,   321,
     322,   323,   327,   328,   329,   332,   338,   342,   293,   294,
     292,   296,   309,   294,   294,   251,   223,   241,   221,   242,
     224,   269,   282,   286,   309,   243,   133,   251,   316,   317,
     244,   315,   314,   242,   241,   222,   226,   242,   242,   321,
     221,   221,   242,   242,   285,   221,   221,   269,   269,   285,
     244,   324,   104,   105,   223,   225,   222,   222,   226,    92,
     283,   221,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   241,   284,   269,   231,   232,   233,   227,   228,
     102,   103,   106,   107,   234,   235,   108,   109,   236,   237,
     238,   110,   112,   111,   239,   226,   242,   244,   321,   251,
     223,   241,   224,   286,   243,   283,   318,   224,   299,   349,
     350,   223,   226,   242,   244,    98,    99,   302,   303,    93,
     320,   328,   339,   285,   242,   285,   285,   299,   331,   222,
     327,   257,   285,   251,   254,   265,   266,   267,   268,   283,
     283,   269,   269,   269,   271,   271,   272,   272,   273,   273,
     273,   273,   274,   274,   275,   276,   277,   278,   279,   280,
     285,   283,   223,   224,   286,   318,   241,   224,   318,   319,
     316,   244,   349,   286,   317,   221,   331,   340,   341,   222,
     222,   251,   222,   244,   224,   221,   222,   222,   226,    92,
     283,   240,   286,   241,   224,   318,   241,   226,   244,   242,
      96,   348,   224,   285,   242,   222,   321,   330,   243,   333,
     241,   322,   325,   326,   283,   283,   224,   318,   241,   318,
     244,   318,   223,   242,   222,   285,   325,    12,    18,    19,
     244,   334,   335,   336,   337,   318,   318,   224,   286,   242,
     321,   285,   240,   321,   334,   321,   244,   336,   224,   240
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, state, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (YYID (N))                                                     \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (YYID (0))
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

__attribute__((__unused__))
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
#else
static unsigned
yy_location_print_ (yyo, yylocp)
    FILE *yyo;
    YYLTYPE const * const yylocp;
#endif
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += fprintf (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += fprintf (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += fprintf (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += fprintf (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += fprintf (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, state)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, state); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, struct _mesa_glsl_parse_state *state)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, state)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    struct _mesa_glsl_parse_state *state;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (state);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, struct _mesa_glsl_parse_state *state)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, state)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    struct _mesa_glsl_parse_state *state;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, state);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, struct _mesa_glsl_parse_state *state)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, state)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    struct _mesa_glsl_parse_state *state;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , state);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, state); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, struct _mesa_glsl_parse_state *state)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, state)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    struct _mesa_glsl_parse_state *state;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (state);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YYUSE (yytype);
}




/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (struct _mesa_glsl_parse_state *state)
#else
int
yyparse (state)
    struct _mesa_glsl_parse_state *state;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;


#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
static YYSTYPE yyval_default;
# define YY_INITIAL_VALUE(Value) = Value
#endif
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc = yyloc_default;


    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

/* User initialization code.  */
/* Line 1570 of yacc.c  */
#line 54 "glsl_parser.yy"
{
   yylloc.first_line = 1;
   yylloc.first_column = 1;
   yylloc.last_line = 1;
   yylloc.last_column = 1;
   yylloc.source = 0;
}
/* Line 1570 of yacc.c  */
#line 2845 "glsl_parser.cpp"
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1787 of yacc.c  */
#line 263 "glsl_parser.yy"
    {
      _mesa_glsl_initialize_types(state);
   }
    break;

  case 3:
/* Line 1787 of yacc.c  */
#line 267 "glsl_parser.yy"
    {
      delete state->symbols;
      state->symbols = new(ralloc_parent(state)) glsl_symbol_table;
      _mesa_glsl_initialize_types(state);
   }
    break;

  case 5:
/* Line 1787 of yacc.c  */
#line 277 "glsl_parser.yy"
    {
      state->process_version_directive(&(yylsp[(2) - (3)]), (yyvsp[(2) - (3)].n), NULL);
      if (state->error) {
         YYERROR;
      }
   }
    break;

  case 6:
/* Line 1787 of yacc.c  */
#line 284 "glsl_parser.yy"
    {
      state->process_version_directive(&(yylsp[(2) - (4)]), (yyvsp[(2) - (4)].n), (yyvsp[(3) - (4)].identifier));
      if (state->error) {
         YYERROR;
      }
   }
    break;

  case 11:
/* Line 1787 of yacc.c  */
#line 298 "glsl_parser.yy"
    {
      if (!state->is_version(120, 100)) {
         _mesa_glsl_warning(& (yylsp[(1) - (2)]), state,
                            "pragma `invariant(all)' not supported in %s "
                            "(GLSL ES 1.00 or GLSL 1.20 required).",
                            state->get_version_string());
      } else {
         state->all_invariant = true;
      }
   }
    break;

  case 17:
/* Line 1787 of yacc.c  */
#line 323 "glsl_parser.yy"
    {
      if (!_mesa_glsl_process_extension((yyvsp[(2) - (5)].identifier), & (yylsp[(2) - (5)]), (yyvsp[(4) - (5)].identifier), & (yylsp[(4) - (5)]), state)) {
         YYERROR;
      }
   }
    break;

  case 18:
/* Line 1787 of yacc.c  */
#line 332 "glsl_parser.yy"
    {
      /* FINISHME: The NULL test is required because pragmas are set to
       * FINISHME: NULL. (See production rule for external_declaration.)
       */
      if ((yyvsp[(1) - (1)].node) != NULL)
         state->translation_unit.push_tail(& (yyvsp[(1) - (1)].node)->link);
   }
    break;

  case 19:
/* Line 1787 of yacc.c  */
#line 340 "glsl_parser.yy"
    {
      /* FINISHME: The NULL test is required because pragmas are set to
       * FINISHME: NULL. (See production rule for external_declaration.)
       */
      if ((yyvsp[(2) - (2)].node) != NULL)
         state->translation_unit.push_tail(& (yyvsp[(2) - (2)].node)->link);
   }
    break;

  case 22:
/* Line 1787 of yacc.c  */
#line 356 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_identifier, NULL, NULL, NULL);
      (yyval.expression)->set_location(yylloc);
      (yyval.expression)->primary_expression.identifier = (yyvsp[(1) - (1)].identifier);
   }
    break;

  case 23:
/* Line 1787 of yacc.c  */
#line 363 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_int_constant, NULL, NULL, NULL);
      (yyval.expression)->set_location(yylloc);
      (yyval.expression)->primary_expression.int_constant = (yyvsp[(1) - (1)].n);
   }
    break;

  case 24:
/* Line 1787 of yacc.c  */
#line 370 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_uint_constant, NULL, NULL, NULL);
      (yyval.expression)->set_location(yylloc);
      (yyval.expression)->primary_expression.uint_constant = (yyvsp[(1) - (1)].n);
   }
    break;

  case 25:
/* Line 1787 of yacc.c  */
#line 377 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_float_constant, NULL, NULL, NULL);
      (yyval.expression)->set_location(yylloc);
      (yyval.expression)->primary_expression.float_constant = (yyvsp[(1) - (1)].real);
   }
    break;

  case 26:
/* Line 1787 of yacc.c  */
#line 384 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_bool_constant, NULL, NULL, NULL);
      (yyval.expression)->set_location(yylloc);
      (yyval.expression)->primary_expression.bool_constant = (yyvsp[(1) - (1)].n);
   }
    break;

  case 27:
/* Line 1787 of yacc.c  */
#line 391 "glsl_parser.yy"
    {
      (yyval.expression) = (yyvsp[(2) - (3)].expression);
   }
    break;

  case 29:
/* Line 1787 of yacc.c  */
#line 399 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_array_index, (yyvsp[(1) - (4)].expression), (yyvsp[(3) - (4)].expression), NULL);
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 30:
/* Line 1787 of yacc.c  */
#line 405 "glsl_parser.yy"
    {
      (yyval.expression) = (yyvsp[(1) - (1)].expression);
   }
    break;

  case 31:
/* Line 1787 of yacc.c  */
#line 409 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_field_selection, (yyvsp[(1) - (3)].expression), NULL, NULL);
      (yyval.expression)->set_location(yylloc);
      (yyval.expression)->primary_expression.identifier = (yyvsp[(3) - (3)].identifier);
   }
    break;

  case 32:
/* Line 1787 of yacc.c  */
#line 416 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_post_inc, (yyvsp[(1) - (2)].expression), NULL, NULL);
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 33:
/* Line 1787 of yacc.c  */
#line 422 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_post_dec, (yyvsp[(1) - (2)].expression), NULL, NULL);
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 37:
/* Line 1787 of yacc.c  */
#line 440 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_field_selection, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression), NULL);
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 42:
/* Line 1787 of yacc.c  */
#line 459 "glsl_parser.yy"
    {
      (yyval.expression) = (yyvsp[(1) - (2)].expression);
      (yyval.expression)->set_location(yylloc);
      (yyval.expression)->expressions.push_tail(& (yyvsp[(2) - (2)].expression)->link);
   }
    break;

  case 43:
/* Line 1787 of yacc.c  */
#line 465 "glsl_parser.yy"
    {
      (yyval.expression) = (yyvsp[(1) - (3)].expression);
      (yyval.expression)->set_location(yylloc);
      (yyval.expression)->expressions.push_tail(& (yyvsp[(3) - (3)].expression)->link);
   }
    break;

  case 45:
/* Line 1787 of yacc.c  */
#line 481 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_function_expression((yyvsp[(1) - (1)].type_specifier));
      (yyval.expression)->set_location(yylloc);
      }
    break;

  case 46:
/* Line 1787 of yacc.c  */
#line 487 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_expression *callee = new(ctx) ast_expression((yyvsp[(1) - (1)].identifier));
      (yyval.expression) = new(ctx) ast_function_expression(callee);
      (yyval.expression)->set_location(yylloc);
      }
    break;

  case 47:
/* Line 1787 of yacc.c  */
#line 494 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_expression *callee = new(ctx) ast_expression((yyvsp[(1) - (1)].identifier));
      (yyval.expression) = new(ctx) ast_function_expression(callee);
      (yyval.expression)->set_location(yylloc);
      }
    break;

  case 52:
/* Line 1787 of yacc.c  */
#line 514 "glsl_parser.yy"
    {
      (yyval.expression) = (yyvsp[(1) - (2)].expression);
      (yyval.expression)->set_location(yylloc);
      (yyval.expression)->expressions.push_tail(& (yyvsp[(2) - (2)].expression)->link);
   }
    break;

  case 53:
/* Line 1787 of yacc.c  */
#line 520 "glsl_parser.yy"
    {
      (yyval.expression) = (yyvsp[(1) - (3)].expression);
      (yyval.expression)->set_location(yylloc);
      (yyval.expression)->expressions.push_tail(& (yyvsp[(3) - (3)].expression)->link);
   }
    break;

  case 54:
/* Line 1787 of yacc.c  */
#line 532 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_expression *callee = new(ctx) ast_expression((yyvsp[(1) - (2)].identifier));
      (yyval.expression) = new(ctx) ast_function_expression(callee);
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 56:
/* Line 1787 of yacc.c  */
#line 544 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_pre_inc, (yyvsp[(2) - (2)].expression), NULL, NULL);
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 57:
/* Line 1787 of yacc.c  */
#line 550 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_pre_dec, (yyvsp[(2) - (2)].expression), NULL, NULL);
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 58:
/* Line 1787 of yacc.c  */
#line 556 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression((yyvsp[(1) - (2)].n), (yyvsp[(2) - (2)].expression), NULL, NULL);
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 59:
/* Line 1787 of yacc.c  */
#line 565 "glsl_parser.yy"
    { (yyval.n) = ast_plus; }
    break;

  case 60:
/* Line 1787 of yacc.c  */
#line 566 "glsl_parser.yy"
    { (yyval.n) = ast_neg; }
    break;

  case 61:
/* Line 1787 of yacc.c  */
#line 567 "glsl_parser.yy"
    { (yyval.n) = ast_logic_not; }
    break;

  case 62:
/* Line 1787 of yacc.c  */
#line 568 "glsl_parser.yy"
    { (yyval.n) = ast_bit_not; }
    break;

  case 64:
/* Line 1787 of yacc.c  */
#line 574 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_mul, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 65:
/* Line 1787 of yacc.c  */
#line 580 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_div, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 66:
/* Line 1787 of yacc.c  */
#line 586 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_mod, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 68:
/* Line 1787 of yacc.c  */
#line 596 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_add, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 69:
/* Line 1787 of yacc.c  */
#line 602 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_sub, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 71:
/* Line 1787 of yacc.c  */
#line 612 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_lshift, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 72:
/* Line 1787 of yacc.c  */
#line 618 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_rshift, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 74:
/* Line 1787 of yacc.c  */
#line 628 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_less, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 75:
/* Line 1787 of yacc.c  */
#line 634 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_greater, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 76:
/* Line 1787 of yacc.c  */
#line 640 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_lequal, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 77:
/* Line 1787 of yacc.c  */
#line 646 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_gequal, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 79:
/* Line 1787 of yacc.c  */
#line 656 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_equal, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 80:
/* Line 1787 of yacc.c  */
#line 662 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_nequal, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 82:
/* Line 1787 of yacc.c  */
#line 672 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_bit_and, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 84:
/* Line 1787 of yacc.c  */
#line 682 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_bit_xor, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 86:
/* Line 1787 of yacc.c  */
#line 692 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_bit_or, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 88:
/* Line 1787 of yacc.c  */
#line 702 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_logic_and, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 90:
/* Line 1787 of yacc.c  */
#line 712 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_logic_xor, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 92:
/* Line 1787 of yacc.c  */
#line 722 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression_bin(ast_logic_or, (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 94:
/* Line 1787 of yacc.c  */
#line 732 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression(ast_conditional, (yyvsp[(1) - (5)].expression), (yyvsp[(3) - (5)].expression), (yyvsp[(5) - (5)].expression));
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 96:
/* Line 1787 of yacc.c  */
#line 742 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_expression((yyvsp[(2) - (3)].n), (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression), NULL);
      (yyval.expression)->set_location(yylloc);
   }
    break;

  case 97:
/* Line 1787 of yacc.c  */
#line 750 "glsl_parser.yy"
    { (yyval.n) = ast_assign; }
    break;

  case 98:
/* Line 1787 of yacc.c  */
#line 751 "glsl_parser.yy"
    { (yyval.n) = ast_mul_assign; }
    break;

  case 99:
/* Line 1787 of yacc.c  */
#line 752 "glsl_parser.yy"
    { (yyval.n) = ast_div_assign; }
    break;

  case 100:
/* Line 1787 of yacc.c  */
#line 753 "glsl_parser.yy"
    { (yyval.n) = ast_mod_assign; }
    break;

  case 101:
/* Line 1787 of yacc.c  */
#line 754 "glsl_parser.yy"
    { (yyval.n) = ast_add_assign; }
    break;

  case 102:
/* Line 1787 of yacc.c  */
#line 755 "glsl_parser.yy"
    { (yyval.n) = ast_sub_assign; }
    break;

  case 103:
/* Line 1787 of yacc.c  */
#line 756 "glsl_parser.yy"
    { (yyval.n) = ast_ls_assign; }
    break;

  case 104:
/* Line 1787 of yacc.c  */
#line 757 "glsl_parser.yy"
    { (yyval.n) = ast_rs_assign; }
    break;

  case 105:
/* Line 1787 of yacc.c  */
#line 758 "glsl_parser.yy"
    { (yyval.n) = ast_and_assign; }
    break;

  case 106:
/* Line 1787 of yacc.c  */
#line 759 "glsl_parser.yy"
    { (yyval.n) = ast_xor_assign; }
    break;

  case 107:
/* Line 1787 of yacc.c  */
#line 760 "glsl_parser.yy"
    { (yyval.n) = ast_or_assign; }
    break;

  case 108:
/* Line 1787 of yacc.c  */
#line 765 "glsl_parser.yy"
    {
      (yyval.expression) = (yyvsp[(1) - (1)].expression);
   }
    break;

  case 109:
/* Line 1787 of yacc.c  */
#line 769 "glsl_parser.yy"
    {
      void *ctx = state;
      if ((yyvsp[(1) - (3)].expression)->oper != ast_sequence) {
         (yyval.expression) = new(ctx) ast_expression(ast_sequence, NULL, NULL, NULL);
         (yyval.expression)->set_location(yylloc);
         (yyval.expression)->expressions.push_tail(& (yyvsp[(1) - (3)].expression)->link);
      } else {
         (yyval.expression) = (yyvsp[(1) - (3)].expression);
      }

      (yyval.expression)->expressions.push_tail(& (yyvsp[(3) - (3)].expression)->link);
   }
    break;

  case 111:
/* Line 1787 of yacc.c  */
#line 789 "glsl_parser.yy"
    {
      state->symbols->pop_scope();
      (yyval.node) = (yyvsp[(1) - (2)].function);
   }
    break;

  case 112:
/* Line 1787 of yacc.c  */
#line 794 "glsl_parser.yy"
    {
      (yyval.node) = (yyvsp[(1) - (2)].declarator_list);
   }
    break;

  case 113:
/* Line 1787 of yacc.c  */
#line 798 "glsl_parser.yy"
    {
      (yyvsp[(3) - (4)].type_specifier)->default_precision = (yyvsp[(2) - (4)].n);
      (yyval.node) = (yyvsp[(3) - (4)].type_specifier);
   }
    break;

  case 114:
/* Line 1787 of yacc.c  */
#line 803 "glsl_parser.yy"
    {
      (yyval.node) = (yyvsp[(1) - (1)].node);
   }
    break;

  case 118:
/* Line 1787 of yacc.c  */
#line 819 "glsl_parser.yy"
    {
      (yyval.function) = (yyvsp[(1) - (2)].function);
      (yyval.function)->parameters.push_tail(& (yyvsp[(2) - (2)].parameter_declarator)->link);
   }
    break;

  case 119:
/* Line 1787 of yacc.c  */
#line 824 "glsl_parser.yy"
    {
      (yyval.function) = (yyvsp[(1) - (3)].function);
      (yyval.function)->parameters.push_tail(& (yyvsp[(3) - (3)].parameter_declarator)->link);
   }
    break;

  case 120:
/* Line 1787 of yacc.c  */
#line 832 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.function) = new(ctx) ast_function();
      (yyval.function)->set_location(yylloc);
      (yyval.function)->return_type = (yyvsp[(1) - (3)].fully_specified_type);
      (yyval.function)->identifier = (yyvsp[(2) - (3)].identifier);

      state->symbols->add_function(new(state) ir_function((yyvsp[(2) - (3)].identifier)));
      state->symbols->push_scope();
   }
    break;

  case 121:
/* Line 1787 of yacc.c  */
#line 846 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.parameter_declarator) = new(ctx) ast_parameter_declarator();
      (yyval.parameter_declarator)->set_location(yylloc);
      (yyval.parameter_declarator)->type = new(ctx) ast_fully_specified_type();
      (yyval.parameter_declarator)->type->set_location(yylloc);
      (yyval.parameter_declarator)->type->specifier = (yyvsp[(1) - (2)].type_specifier);
      (yyval.parameter_declarator)->identifier = (yyvsp[(2) - (2)].identifier);
   }
    break;

  case 122:
/* Line 1787 of yacc.c  */
#line 856 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.parameter_declarator) = new(ctx) ast_parameter_declarator();
      (yyval.parameter_declarator)->set_location(yylloc);
      (yyval.parameter_declarator)->type = new(ctx) ast_fully_specified_type();
      (yyval.parameter_declarator)->type->set_location(yylloc);
      (yyval.parameter_declarator)->type->specifier = (yyvsp[(1) - (5)].type_specifier);
      (yyval.parameter_declarator)->identifier = (yyvsp[(2) - (5)].identifier);
      (yyval.parameter_declarator)->is_array = true;
      (yyval.parameter_declarator)->array_size = (yyvsp[(4) - (5)].expression);
   }
    break;

  case 123:
/* Line 1787 of yacc.c  */
#line 871 "glsl_parser.yy"
    {
      (yyval.parameter_declarator) = (yyvsp[(2) - (2)].parameter_declarator);
      (yyval.parameter_declarator)->type->qualifier = (yyvsp[(1) - (2)].type_qualifier);
   }
    break;

  case 124:
/* Line 1787 of yacc.c  */
#line 876 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.parameter_declarator) = new(ctx) ast_parameter_declarator();
      (yyval.parameter_declarator)->set_location(yylloc);
      (yyval.parameter_declarator)->type = new(ctx) ast_fully_specified_type();
      (yyval.parameter_declarator)->type->qualifier = (yyvsp[(1) - (2)].type_qualifier);
      (yyval.parameter_declarator)->type->specifier = (yyvsp[(2) - (2)].type_specifier);
   }
    break;

  case 125:
/* Line 1787 of yacc.c  */
#line 888 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
   }
    break;

  case 126:
/* Line 1787 of yacc.c  */
#line 892 "glsl_parser.yy"
    {
      if ((yyvsp[(2) - (2)].type_qualifier).flags.q.constant)
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "duplicate const qualifier.\n");

      (yyval.type_qualifier) = (yyvsp[(2) - (2)].type_qualifier);
      (yyval.type_qualifier).flags.q.constant = 1;
   }
    break;

  case 127:
/* Line 1787 of yacc.c  */
#line 900 "glsl_parser.yy"
    {
      if (((yyvsp[(1) - (2)].type_qualifier).flags.q.in || (yyvsp[(1) - (2)].type_qualifier).flags.q.out) && ((yyvsp[(2) - (2)].type_qualifier).flags.q.in || (yyvsp[(2) - (2)].type_qualifier).flags.q.out))
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "duplicate in/out/inout qualifier\n");

      if (!state->ARB_shading_language_420pack_enable && (yyvsp[(2) - (2)].type_qualifier).flags.q.constant)
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "const must be specified before "
                          "in/out/inout.\n");

      (yyval.type_qualifier) = (yyvsp[(1) - (2)].type_qualifier);
      (yyval.type_qualifier).merge_qualifier(&(yylsp[(1) - (2)]), state, (yyvsp[(2) - (2)].type_qualifier));
   }
    break;

  case 128:
/* Line 1787 of yacc.c  */
#line 912 "glsl_parser.yy"
    {
      if ((yyvsp[(2) - (2)].type_qualifier).precision != ast_precision_none)
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "Duplicate precision qualifier.\n");

      if (!state->ARB_shading_language_420pack_enable && (yyvsp[(2) - (2)].type_qualifier).flags.i != 0)
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "Precision qualifiers must come last.\n");

      (yyval.type_qualifier) = (yyvsp[(2) - (2)].type_qualifier);
      (yyval.type_qualifier).precision = (yyvsp[(1) - (2)].n);
   }
    break;

  case 129:
/* Line 1787 of yacc.c  */
#line 925 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.in = 1;
   }
    break;

  case 130:
/* Line 1787 of yacc.c  */
#line 930 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.out = 1;
   }
    break;

  case 131:
/* Line 1787 of yacc.c  */
#line 935 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.in = 1;
      (yyval.type_qualifier).flags.q.out = 1;
   }
    break;

  case 134:
/* Line 1787 of yacc.c  */
#line 949 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(3) - (3)].identifier), false, NULL, NULL);
      decl->set_location(yylloc);

      (yyval.declarator_list) = (yyvsp[(1) - (3)].declarator_list);
      (yyval.declarator_list)->declarations.push_tail(&decl->link);
      state->symbols->add_variable(new(state) ir_variable(NULL, (yyvsp[(3) - (3)].identifier), ir_var_auto));
   }
    break;

  case 135:
/* Line 1787 of yacc.c  */
#line 959 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(3) - (5)].identifier), true, NULL, NULL);
      decl->set_location(yylloc);

      (yyval.declarator_list) = (yyvsp[(1) - (5)].declarator_list);
      (yyval.declarator_list)->declarations.push_tail(&decl->link);
      state->symbols->add_variable(new(state) ir_variable(NULL, (yyvsp[(3) - (5)].identifier), ir_var_auto));
   }
    break;

  case 136:
/* Line 1787 of yacc.c  */
#line 969 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(3) - (6)].identifier), true, (yyvsp[(5) - (6)].expression), NULL);
      decl->set_location(yylloc);

      (yyval.declarator_list) = (yyvsp[(1) - (6)].declarator_list);
      (yyval.declarator_list)->declarations.push_tail(&decl->link);
      state->symbols->add_variable(new(state) ir_variable(NULL, (yyvsp[(3) - (6)].identifier), ir_var_auto));
   }
    break;

  case 137:
/* Line 1787 of yacc.c  */
#line 979 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(3) - (7)].identifier), true, NULL, (yyvsp[(7) - (7)].expression));
      decl->set_location(yylloc);

      (yyval.declarator_list) = (yyvsp[(1) - (7)].declarator_list);
      (yyval.declarator_list)->declarations.push_tail(&decl->link);
      state->symbols->add_variable(new(state) ir_variable(NULL, (yyvsp[(3) - (7)].identifier), ir_var_auto));
      if ((yyvsp[(7) - (7)].expression)->oper == ast_aggregate) {
         ast_aggregate_initializer *ai = (ast_aggregate_initializer *)(yyvsp[(7) - (7)].expression);
         ast_type_specifier *type = new(ctx) ast_type_specifier((yyvsp[(1) - (7)].declarator_list)->type->specifier, true, NULL);
         _mesa_ast_set_aggregate_type(type, ai, state);
      }
   }
    break;

  case 138:
/* Line 1787 of yacc.c  */
#line 994 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(3) - (8)].identifier), true, (yyvsp[(5) - (8)].expression), (yyvsp[(8) - (8)].expression));
      decl->set_location(yylloc);

      (yyval.declarator_list) = (yyvsp[(1) - (8)].declarator_list);
      (yyval.declarator_list)->declarations.push_tail(&decl->link);
      state->symbols->add_variable(new(state) ir_variable(NULL, (yyvsp[(3) - (8)].identifier), ir_var_auto));
      if ((yyvsp[(8) - (8)].expression)->oper == ast_aggregate) {
         ast_aggregate_initializer *ai = (ast_aggregate_initializer *)(yyvsp[(8) - (8)].expression);
         ast_type_specifier *type = new(ctx) ast_type_specifier((yyvsp[(1) - (8)].declarator_list)->type->specifier, true, (yyvsp[(5) - (8)].expression));
         _mesa_ast_set_aggregate_type(type, ai, state);
      }
   }
    break;

  case 139:
/* Line 1787 of yacc.c  */
#line 1009 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(3) - (5)].identifier), false, NULL, (yyvsp[(5) - (5)].expression));
      decl->set_location(yylloc);

      (yyval.declarator_list) = (yyvsp[(1) - (5)].declarator_list);
      (yyval.declarator_list)->declarations.push_tail(&decl->link);
      state->symbols->add_variable(new(state) ir_variable(NULL, (yyvsp[(3) - (5)].identifier), ir_var_auto));
      if ((yyvsp[(5) - (5)].expression)->oper == ast_aggregate) {
         ast_aggregate_initializer *ai = (ast_aggregate_initializer *)(yyvsp[(5) - (5)].expression);
         _mesa_ast_set_aggregate_type((yyvsp[(1) - (5)].declarator_list)->type->specifier, ai, state);
      }
   }
    break;

  case 140:
/* Line 1787 of yacc.c  */
#line 1027 "glsl_parser.yy"
    {
      void *ctx = state;
      /* Empty declaration list is valid. */
      (yyval.declarator_list) = new(ctx) ast_declarator_list((yyvsp[(1) - (1)].fully_specified_type));
      (yyval.declarator_list)->set_location(yylloc);
   }
    break;

  case 141:
/* Line 1787 of yacc.c  */
#line 1034 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(2) - (2)].identifier), false, NULL, NULL);

      (yyval.declarator_list) = new(ctx) ast_declarator_list((yyvsp[(1) - (2)].fully_specified_type));
      (yyval.declarator_list)->set_location(yylloc);
      (yyval.declarator_list)->declarations.push_tail(&decl->link);
   }
    break;

  case 142:
/* Line 1787 of yacc.c  */
#line 1043 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(2) - (4)].identifier), true, NULL, NULL);

      (yyval.declarator_list) = new(ctx) ast_declarator_list((yyvsp[(1) - (4)].fully_specified_type));
      (yyval.declarator_list)->set_location(yylloc);
      (yyval.declarator_list)->declarations.push_tail(&decl->link);
   }
    break;

  case 143:
/* Line 1787 of yacc.c  */
#line 1052 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(2) - (5)].identifier), true, (yyvsp[(4) - (5)].expression), NULL);

      (yyval.declarator_list) = new(ctx) ast_declarator_list((yyvsp[(1) - (5)].fully_specified_type));
      (yyval.declarator_list)->set_location(yylloc);
      (yyval.declarator_list)->declarations.push_tail(&decl->link);
   }
    break;

  case 144:
/* Line 1787 of yacc.c  */
#line 1061 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(2) - (6)].identifier), true, NULL, (yyvsp[(6) - (6)].expression));

      (yyval.declarator_list) = new(ctx) ast_declarator_list((yyvsp[(1) - (6)].fully_specified_type));
      (yyval.declarator_list)->set_location(yylloc);
      (yyval.declarator_list)->declarations.push_tail(&decl->link);
      if ((yyvsp[(6) - (6)].expression)->oper == ast_aggregate) {
         ast_aggregate_initializer *ai = (ast_aggregate_initializer *)(yyvsp[(6) - (6)].expression);
         ast_type_specifier *type = new(ctx) ast_type_specifier((yyvsp[(1) - (6)].fully_specified_type)->specifier, true, NULL);
         _mesa_ast_set_aggregate_type(type, ai, state);
      }
   }
    break;

  case 145:
/* Line 1787 of yacc.c  */
#line 1075 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(2) - (7)].identifier), true, (yyvsp[(4) - (7)].expression), (yyvsp[(7) - (7)].expression));

      (yyval.declarator_list) = new(ctx) ast_declarator_list((yyvsp[(1) - (7)].fully_specified_type));
      (yyval.declarator_list)->set_location(yylloc);
      (yyval.declarator_list)->declarations.push_tail(&decl->link);
      if ((yyvsp[(7) - (7)].expression)->oper == ast_aggregate) {
         ast_aggregate_initializer *ai = (ast_aggregate_initializer *)(yyvsp[(7) - (7)].expression);
         ast_type_specifier *type = new(ctx) ast_type_specifier((yyvsp[(1) - (7)].fully_specified_type)->specifier, true, (yyvsp[(4) - (7)].expression));
         _mesa_ast_set_aggregate_type(type, ai, state);
      }
   }
    break;

  case 146:
/* Line 1787 of yacc.c  */
#line 1089 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(2) - (4)].identifier), false, NULL, (yyvsp[(4) - (4)].expression));

      (yyval.declarator_list) = new(ctx) ast_declarator_list((yyvsp[(1) - (4)].fully_specified_type));
      (yyval.declarator_list)->set_location(yylloc);
      (yyval.declarator_list)->declarations.push_tail(&decl->link);
      if ((yyvsp[(4) - (4)].expression)->oper == ast_aggregate) {
         _mesa_ast_set_aggregate_type((yyvsp[(1) - (4)].fully_specified_type)->specifier, (yyvsp[(4) - (4)].expression), state);
      }
   }
    break;

  case 147:
/* Line 1787 of yacc.c  */
#line 1101 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(2) - (2)].identifier), false, NULL, NULL);

      (yyval.declarator_list) = new(ctx) ast_declarator_list(NULL);
      (yyval.declarator_list)->set_location(yylloc);
      (yyval.declarator_list)->invariant = true;

      (yyval.declarator_list)->declarations.push_tail(&decl->link);
   }
    break;

  case 148:
/* Line 1787 of yacc.c  */
#line 1115 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.fully_specified_type) = new(ctx) ast_fully_specified_type();
      (yyval.fully_specified_type)->set_location(yylloc);
      (yyval.fully_specified_type)->specifier = (yyvsp[(1) - (1)].type_specifier);
   }
    break;

  case 149:
/* Line 1787 of yacc.c  */
#line 1122 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.fully_specified_type) = new(ctx) ast_fully_specified_type();
      (yyval.fully_specified_type)->set_location(yylloc);
      (yyval.fully_specified_type)->qualifier = (yyvsp[(1) - (2)].type_qualifier);
      (yyval.fully_specified_type)->specifier = (yyvsp[(2) - (2)].type_specifier);
   }
    break;

  case 150:
/* Line 1787 of yacc.c  */
#line 1133 "glsl_parser.yy"
    {
      (yyval.type_qualifier) = (yyvsp[(3) - (4)].type_qualifier);
   }
    break;

  case 152:
/* Line 1787 of yacc.c  */
#line 1141 "glsl_parser.yy"
    {
      (yyval.type_qualifier) = (yyvsp[(1) - (3)].type_qualifier);
      if (!(yyval.type_qualifier).merge_qualifier(& (yylsp[(3) - (3)]), state, (yyvsp[(3) - (3)].type_qualifier))) {
         YYERROR;
      }
   }
    break;

  case 153:
/* Line 1787 of yacc.c  */
#line 1150 "glsl_parser.yy"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 154:
/* Line 1787 of yacc.c  */
#line 1151 "glsl_parser.yy"
    { (yyval.n) = (yyvsp[(1) - (1)].n); }
    break;

  case 155:
/* Line 1787 of yacc.c  */
#line 1156 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));

      /* Layout qualifiers for ARB_fragment_coord_conventions. */
      if (!(yyval.type_qualifier).flags.i && state->ARB_fragment_coord_conventions_enable) {
         if (strcmp((yyvsp[(1) - (1)].identifier), "origin_upper_left") == 0) {
            (yyval.type_qualifier).flags.q.origin_upper_left = 1;
         } else if (strcmp((yyvsp[(1) - (1)].identifier), "pixel_center_integer") == 0) {
            (yyval.type_qualifier).flags.q.pixel_center_integer = 1;
         }

         if ((yyval.type_qualifier).flags.i && state->ARB_fragment_coord_conventions_warn) {
            _mesa_glsl_warning(& (yylsp[(1) - (1)]), state,
                               "GL_ARB_fragment_coord_conventions layout "
                               "identifier `%s' used\n", (yyvsp[(1) - (1)].identifier));
         }
      }

      /* Layout qualifiers for AMD/ARB_conservative_depth. */
      if (!(yyval.type_qualifier).flags.i &&
          (state->AMD_conservative_depth_enable ||
           state->ARB_conservative_depth_enable)) {
         if (strcmp((yyvsp[(1) - (1)].identifier), "depth_any") == 0) {
            (yyval.type_qualifier).flags.q.depth_any = 1;
         } else if (strcmp((yyvsp[(1) - (1)].identifier), "depth_greater") == 0) {
            (yyval.type_qualifier).flags.q.depth_greater = 1;
         } else if (strcmp((yyvsp[(1) - (1)].identifier), "depth_less") == 0) {
            (yyval.type_qualifier).flags.q.depth_less = 1;
         } else if (strcmp((yyvsp[(1) - (1)].identifier), "depth_unchanged") == 0) {
            (yyval.type_qualifier).flags.q.depth_unchanged = 1;
         }

         if ((yyval.type_qualifier).flags.i && state->AMD_conservative_depth_warn) {
            _mesa_glsl_warning(& (yylsp[(1) - (1)]), state,
                               "GL_AMD_conservative_depth "
                               "layout qualifier `%s' is used\n", (yyvsp[(1) - (1)].identifier));
         }
         if ((yyval.type_qualifier).flags.i && state->ARB_conservative_depth_warn) {
            _mesa_glsl_warning(& (yylsp[(1) - (1)]), state,
                               "GL_ARB_conservative_depth "
                               "layout qualifier `%s' is used\n", (yyvsp[(1) - (1)].identifier));
         }
      }

      /* See also interface_block_layout_qualifier. */
      if (!(yyval.type_qualifier).flags.i && state->ARB_uniform_buffer_object_enable) {
         if (strcmp((yyvsp[(1) - (1)].identifier), "std140") == 0) {
            (yyval.type_qualifier).flags.q.std140 = 1;
         } else if (strcmp((yyvsp[(1) - (1)].identifier), "shared") == 0) {
            (yyval.type_qualifier).flags.q.shared = 1;
         } else if (strcmp((yyvsp[(1) - (1)].identifier), "column_major") == 0) {
            (yyval.type_qualifier).flags.q.column_major = 1;
         /* "row_major" is a reserved word in GLSL 1.30+. Its token is parsed
          * below in the interface_block_layout_qualifier rule.
          *
          * It is not a reserved word in GLSL ES 3.00, so it's handled here as
          * an identifier.
          */
         } else if (strcmp((yyvsp[(1) - (1)].identifier), "row_major") == 0) {
            (yyval.type_qualifier).flags.q.row_major = 1;
         }

         if ((yyval.type_qualifier).flags.i && state->ARB_uniform_buffer_object_warn) {
            _mesa_glsl_warning(& (yylsp[(1) - (1)]), state,
                               "#version 140 / GL_ARB_uniform_buffer_object "
                               "layout qualifier `%s' is used\n", (yyvsp[(1) - (1)].identifier));
         }
      }

      if (!(yyval.type_qualifier).flags.i) {
         _mesa_glsl_error(& (yylsp[(1) - (1)]), state, "unrecognized layout identifier "
                          "`%s'\n", (yyvsp[(1) - (1)].identifier));
         YYERROR;
      }
   }
    break;

  case 156:
/* Line 1787 of yacc.c  */
#line 1232 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));

      if (state->ARB_explicit_attrib_location_enable) {
         if (strcmp("location", (yyvsp[(1) - (3)].identifier)) == 0) {
            (yyval.type_qualifier).flags.q.explicit_location = 1;

            if ((yyvsp[(3) - (3)].n) >= 0) {
               (yyval.type_qualifier).location = (yyvsp[(3) - (3)].n);
            } else {
               _mesa_glsl_error(& (yylsp[(3) - (3)]), state,
                                "invalid location %d specified\n", (yyvsp[(3) - (3)].n));
               YYERROR;
            }
         }

         if (strcmp("index", (yyvsp[(1) - (3)].identifier)) == 0) {
            (yyval.type_qualifier).flags.q.explicit_index = 1;

            if ((yyvsp[(3) - (3)].n) >= 0) {
               (yyval.type_qualifier).index = (yyvsp[(3) - (3)].n);
            } else {
               _mesa_glsl_error(& (yylsp[(3) - (3)]), state,
                                "invalid index %d specified\n", (yyvsp[(3) - (3)].n));
               YYERROR;
            }
         }
      }

      if (state->ARB_shading_language_420pack_enable &&
          strcmp("binding", (yyvsp[(1) - (3)].identifier)) == 0) {
         (yyval.type_qualifier).flags.q.explicit_binding = 1;
         (yyval.type_qualifier).binding = (yyvsp[(3) - (3)].n);
      }

      /* If the identifier didn't match any known layout identifiers,
       * emit an error.
       */
      if (!(yyval.type_qualifier).flags.i) {
         _mesa_glsl_error(& (yylsp[(1) - (3)]), state, "unrecognized layout identifier "
                          "`%s'\n", (yyvsp[(1) - (3)].identifier));
         YYERROR;
      } else if (state->ARB_explicit_attrib_location_warn) {
         _mesa_glsl_warning(& (yylsp[(1) - (3)]), state,
                            "GL_ARB_explicit_attrib_location layout "
                            "identifier `%s' used\n", (yyvsp[(1) - (3)].identifier));
      }
   }
    break;

  case 157:
/* Line 1787 of yacc.c  */
#line 1281 "glsl_parser.yy"
    {
      (yyval.type_qualifier) = (yyvsp[(1) - (1)].type_qualifier);
      /* Layout qualifiers for ARB_uniform_buffer_object. */
      if ((yyval.type_qualifier).flags.q.uniform && !state->ARB_uniform_buffer_object_enable) {
         _mesa_glsl_error(& (yylsp[(1) - (1)]), state,
                          "#version 140 / GL_ARB_uniform_buffer_object "
                          "layout qualifier `%s' is used\n", (yyvsp[(1) - (1)].type_qualifier));
      } else if ((yyval.type_qualifier).flags.q.uniform && state->ARB_uniform_buffer_object_warn) {
         _mesa_glsl_warning(& (yylsp[(1) - (1)]), state,
                            "#version 140 / GL_ARB_uniform_buffer_object "
                            "layout qualifier `%s' is used\n", (yyvsp[(1) - (1)].type_qualifier));
      }
   }
    break;

  case 158:
/* Line 1787 of yacc.c  */
#line 1303 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.row_major = 1;
   }
    break;

  case 159:
/* Line 1787 of yacc.c  */
#line 1308 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.packed = 1;
   }
    break;

  case 160:
/* Line 1787 of yacc.c  */
#line 1316 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.smooth = 1;
   }
    break;

  case 161:
/* Line 1787 of yacc.c  */
#line 1321 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.flat = 1;
   }
    break;

  case 162:
/* Line 1787 of yacc.c  */
#line 1326 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.noperspective = 1;
   }
    break;

  case 163:
/* Line 1787 of yacc.c  */
#line 1335 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.invariant = 1;
   }
    break;

  case 168:
/* Line 1787 of yacc.c  */
#line 1344 "glsl_parser.yy"
    {
      memset(&(yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).precision = (yyvsp[(1) - (1)].n);
   }
    break;

  case 169:
/* Line 1787 of yacc.c  */
#line 1362 "glsl_parser.yy"
    {
      if ((yyvsp[(2) - (2)].type_qualifier).flags.q.invariant)
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "Duplicate \"invariant\" qualifier.\n");

      if ((yyvsp[(2) - (2)].type_qualifier).has_layout()) {
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state,
                          "\"invariant\" cannot be used with layout(...).\n");
      }

      (yyval.type_qualifier) = (yyvsp[(2) - (2)].type_qualifier);
      (yyval.type_qualifier).flags.q.invariant = 1;
   }
    break;

  case 170:
/* Line 1787 of yacc.c  */
#line 1375 "glsl_parser.yy"
    {
      /* Section 4.3 of the GLSL 1.40 specification states:
       * "...qualified with one of these interpolation qualifiers"
       *
       * GLSL 1.30 claims to allow "one or more", but insists that:
       * "These interpolation qualifiers may only precede the qualifiers in,
       *  centroid in, out, or centroid out in a declaration."
       *
       * ...which means that e.g. smooth can't precede smooth, so there can be
       * only one after all, and the 1.40 text is a clarification, not a change.
       */
      if ((yyvsp[(2) - (2)].type_qualifier).has_interpolation())
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "Duplicate interpolation qualifier.\n");

      if ((yyvsp[(2) - (2)].type_qualifier).has_layout()) {
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "Interpolation qualifiers cannot be used "
                          "with layout(...).\n");
      }

      if (!state->ARB_shading_language_420pack_enable && (yyvsp[(2) - (2)].type_qualifier).flags.q.invariant) {
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "Interpolation qualifiers must come "
                          "after \"invariant\".\n");
      }

      (yyval.type_qualifier) = (yyvsp[(1) - (2)].type_qualifier);
      (yyval.type_qualifier).merge_qualifier(&(yylsp[(1) - (2)]), state, (yyvsp[(2) - (2)].type_qualifier));
   }
    break;

  case 171:
/* Line 1787 of yacc.c  */
#line 1403 "glsl_parser.yy"
    {
      /* The GLSL 1.50 grammar indicates that a layout(...) declaration can be
       * used standalone or immediately before a storage qualifier.  It cannot
       * be used with interpolation qualifiers or invariant.  There does not
       * appear to be any text indicating that it must come before the storage
       * qualifier, but always seems to in examples.
       */
      if (!state->ARB_shading_language_420pack_enable && (yyvsp[(2) - (2)].type_qualifier).has_layout())
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "Duplicate layout(...) qualifiers.\n");

      if ((yyvsp[(2) - (2)].type_qualifier).flags.q.invariant)
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "layout(...) cannot be used with "
                          "the \"invariant\" qualifier\n");

      if ((yyvsp[(2) - (2)].type_qualifier).has_interpolation()) {
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "layout(...) cannot be used with "
                          "interpolation qualifiers.\n");
      }

      (yyval.type_qualifier) = (yyvsp[(1) - (2)].type_qualifier);
      (yyval.type_qualifier).merge_qualifier(&(yylsp[(1) - (2)]), state, (yyvsp[(2) - (2)].type_qualifier));
   }
    break;

  case 172:
/* Line 1787 of yacc.c  */
#line 1426 "glsl_parser.yy"
    {
      if ((yyvsp[(2) - (2)].type_qualifier).has_auxiliary_storage()) {
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state,
                          "Duplicate auxiliary storage qualifier (centroid).\n");
      }

      if (!state->ARB_shading_language_420pack_enable &&
          ((yyvsp[(2) - (2)].type_qualifier).flags.q.invariant || (yyvsp[(2) - (2)].type_qualifier).has_interpolation() || (yyvsp[(2) - (2)].type_qualifier).has_layout())) {
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "Auxiliary storage qualifiers must come "
                          "just before storage qualifiers.\n");
      }
      (yyval.type_qualifier) = (yyvsp[(1) - (2)].type_qualifier);
      (yyval.type_qualifier).flags.i |= (yyvsp[(2) - (2)].type_qualifier).flags.i;
   }
    break;

  case 173:
/* Line 1787 of yacc.c  */
#line 1441 "glsl_parser.yy"
    {
      /* Section 4.3 of the GLSL 1.20 specification states:
       * "Variable declarations may have a storage qualifier specified..."
       *  1.30 clarifies this to "may have one storage qualifier".
       */
      if ((yyvsp[(2) - (2)].type_qualifier).has_storage())
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "Duplicate storage qualifier.\n");

      if (!state->ARB_shading_language_420pack_enable &&
          ((yyvsp[(2) - (2)].type_qualifier).flags.q.invariant || (yyvsp[(2) - (2)].type_qualifier).has_interpolation() || (yyvsp[(2) - (2)].type_qualifier).has_layout() ||
           (yyvsp[(2) - (2)].type_qualifier).has_auxiliary_storage())) {
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "Storage qualifiers must come after "
                          "invariant, interpolation, layout and auxiliary "
                          "storage qualifiers.\n");
      }

      (yyval.type_qualifier) = (yyvsp[(1) - (2)].type_qualifier);
      (yyval.type_qualifier).merge_qualifier(&(yylsp[(1) - (2)]), state, (yyvsp[(2) - (2)].type_qualifier));
   }
    break;

  case 174:
/* Line 1787 of yacc.c  */
#line 1461 "glsl_parser.yy"
    {
      if ((yyvsp[(2) - (2)].type_qualifier).precision != ast_precision_none)
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "Duplicate precision qualifier.\n");

      if (!state->ARB_shading_language_420pack_enable && (yyvsp[(2) - (2)].type_qualifier).flags.i != 0)
         _mesa_glsl_error(&(yylsp[(1) - (2)]), state, "Precision qualifiers must come last.\n");

      (yyval.type_qualifier) = (yyvsp[(2) - (2)].type_qualifier);
      (yyval.type_qualifier).precision = (yyvsp[(1) - (2)].n);
   }
    break;

  case 175:
/* Line 1787 of yacc.c  */
#line 1475 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.centroid = 1;
   }
    break;

  case 176:
/* Line 1787 of yacc.c  */
#line 1483 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.constant = 1;
   }
    break;

  case 177:
/* Line 1787 of yacc.c  */
#line 1488 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.attribute = 1;
   }
    break;

  case 178:
/* Line 1787 of yacc.c  */
#line 1493 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.varying = 1;
   }
    break;

  case 179:
/* Line 1787 of yacc.c  */
#line 1498 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.in = 1;
   }
    break;

  case 180:
/* Line 1787 of yacc.c  */
#line 1503 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.out = 1;
   }
    break;

  case 181:
/* Line 1787 of yacc.c  */
#line 1508 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.uniform = 1;
   }
    break;

  case 183:
/* Line 1787 of yacc.c  */
#line 1517 "glsl_parser.yy"
    {
      (yyval.type_specifier) = (yyvsp[(1) - (3)].type_specifier);
      (yyval.type_specifier)->is_array = true;
      (yyval.type_specifier)->array_size = NULL;
   }
    break;

  case 184:
/* Line 1787 of yacc.c  */
#line 1523 "glsl_parser.yy"
    {
      (yyval.type_specifier) = (yyvsp[(1) - (4)].type_specifier);
      (yyval.type_specifier)->is_array = true;
      (yyval.type_specifier)->array_size = (yyvsp[(3) - (4)].expression);
   }
    break;

  case 185:
/* Line 1787 of yacc.c  */
#line 1532 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.type_specifier) = new(ctx) ast_type_specifier((yyvsp[(1) - (1)].identifier));
      (yyval.type_specifier)->set_location(yylloc);
   }
    break;

  case 186:
/* Line 1787 of yacc.c  */
#line 1538 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.type_specifier) = new(ctx) ast_type_specifier((yyvsp[(1) - (1)].struct_specifier));
      (yyval.type_specifier)->set_location(yylloc);
   }
    break;

  case 187:
/* Line 1787 of yacc.c  */
#line 1544 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.type_specifier) = new(ctx) ast_type_specifier((yyvsp[(1) - (1)].identifier));
      (yyval.type_specifier)->set_location(yylloc);
   }
    break;

  case 188:
/* Line 1787 of yacc.c  */
#line 1552 "glsl_parser.yy"
    { (yyval.identifier) = "void"; }
    break;

  case 189:
/* Line 1787 of yacc.c  */
#line 1553 "glsl_parser.yy"
    { (yyval.identifier) = "float"; }
    break;

  case 190:
/* Line 1787 of yacc.c  */
#line 1554 "glsl_parser.yy"
    { (yyval.identifier) = "int"; }
    break;

  case 191:
/* Line 1787 of yacc.c  */
#line 1555 "glsl_parser.yy"
    { (yyval.identifier) = "uint"; }
    break;

  case 192:
/* Line 1787 of yacc.c  */
#line 1556 "glsl_parser.yy"
    { (yyval.identifier) = "bool"; }
    break;

  case 193:
/* Line 1787 of yacc.c  */
#line 1557 "glsl_parser.yy"
    { (yyval.identifier) = "vec2"; }
    break;

  case 194:
/* Line 1787 of yacc.c  */
#line 1558 "glsl_parser.yy"
    { (yyval.identifier) = "vec3"; }
    break;

  case 195:
/* Line 1787 of yacc.c  */
#line 1559 "glsl_parser.yy"
    { (yyval.identifier) = "vec4"; }
    break;

  case 196:
/* Line 1787 of yacc.c  */
#line 1560 "glsl_parser.yy"
    { (yyval.identifier) = "bvec2"; }
    break;

  case 197:
/* Line 1787 of yacc.c  */
#line 1561 "glsl_parser.yy"
    { (yyval.identifier) = "bvec3"; }
    break;

  case 198:
/* Line 1787 of yacc.c  */
#line 1562 "glsl_parser.yy"
    { (yyval.identifier) = "bvec4"; }
    break;

  case 199:
/* Line 1787 of yacc.c  */
#line 1563 "glsl_parser.yy"
    { (yyval.identifier) = "ivec2"; }
    break;

  case 200:
/* Line 1787 of yacc.c  */
#line 1564 "glsl_parser.yy"
    { (yyval.identifier) = "ivec3"; }
    break;

  case 201:
/* Line 1787 of yacc.c  */
#line 1565 "glsl_parser.yy"
    { (yyval.identifier) = "ivec4"; }
    break;

  case 202:
/* Line 1787 of yacc.c  */
#line 1566 "glsl_parser.yy"
    { (yyval.identifier) = "uvec2"; }
    break;

  case 203:
/* Line 1787 of yacc.c  */
#line 1567 "glsl_parser.yy"
    { (yyval.identifier) = "uvec3"; }
    break;

  case 204:
/* Line 1787 of yacc.c  */
#line 1568 "glsl_parser.yy"
    { (yyval.identifier) = "uvec4"; }
    break;

  case 205:
/* Line 1787 of yacc.c  */
#line 1569 "glsl_parser.yy"
    { (yyval.identifier) = "mat2"; }
    break;

  case 206:
/* Line 1787 of yacc.c  */
#line 1570 "glsl_parser.yy"
    { (yyval.identifier) = "mat2x3"; }
    break;

  case 207:
/* Line 1787 of yacc.c  */
#line 1571 "glsl_parser.yy"
    { (yyval.identifier) = "mat2x4"; }
    break;

  case 208:
/* Line 1787 of yacc.c  */
#line 1572 "glsl_parser.yy"
    { (yyval.identifier) = "mat3x2"; }
    break;

  case 209:
/* Line 1787 of yacc.c  */
#line 1573 "glsl_parser.yy"
    { (yyval.identifier) = "mat3"; }
    break;

  case 210:
/* Line 1787 of yacc.c  */
#line 1574 "glsl_parser.yy"
    { (yyval.identifier) = "mat3x4"; }
    break;

  case 211:
/* Line 1787 of yacc.c  */
#line 1575 "glsl_parser.yy"
    { (yyval.identifier) = "mat4x2"; }
    break;

  case 212:
/* Line 1787 of yacc.c  */
#line 1576 "glsl_parser.yy"
    { (yyval.identifier) = "mat4x3"; }
    break;

  case 213:
/* Line 1787 of yacc.c  */
#line 1577 "glsl_parser.yy"
    { (yyval.identifier) = "mat4"; }
    break;

  case 214:
/* Line 1787 of yacc.c  */
#line 1578 "glsl_parser.yy"
    { (yyval.identifier) = "sampler1D"; }
    break;

  case 215:
/* Line 1787 of yacc.c  */
#line 1579 "glsl_parser.yy"
    { (yyval.identifier) = "sampler2D"; }
    break;

  case 216:
/* Line 1787 of yacc.c  */
#line 1580 "glsl_parser.yy"
    { (yyval.identifier) = "sampler2DRect"; }
    break;

  case 217:
/* Line 1787 of yacc.c  */
#line 1581 "glsl_parser.yy"
    { (yyval.identifier) = "sampler3D"; }
    break;

  case 218:
/* Line 1787 of yacc.c  */
#line 1582 "glsl_parser.yy"
    { (yyval.identifier) = "samplerCube"; }
    break;

  case 219:
/* Line 1787 of yacc.c  */
#line 1583 "glsl_parser.yy"
    { (yyval.identifier) = "samplerExternalOES"; }
    break;

  case 220:
/* Line 1787 of yacc.c  */
#line 1584 "glsl_parser.yy"
    { (yyval.identifier) = "sampler1DShadow"; }
    break;

  case 221:
/* Line 1787 of yacc.c  */
#line 1585 "glsl_parser.yy"
    { (yyval.identifier) = "sampler2DShadow"; }
    break;

  case 222:
/* Line 1787 of yacc.c  */
#line 1586 "glsl_parser.yy"
    { (yyval.identifier) = "sampler2DRectShadow"; }
    break;

  case 223:
/* Line 1787 of yacc.c  */
#line 1587 "glsl_parser.yy"
    { (yyval.identifier) = "samplerCubeShadow"; }
    break;

  case 224:
/* Line 1787 of yacc.c  */
#line 1588 "glsl_parser.yy"
    { (yyval.identifier) = "sampler1DArray"; }
    break;

  case 225:
/* Line 1787 of yacc.c  */
#line 1589 "glsl_parser.yy"
    { (yyval.identifier) = "sampler2DArray"; }
    break;

  case 226:
/* Line 1787 of yacc.c  */
#line 1590 "glsl_parser.yy"
    { (yyval.identifier) = "sampler1DArrayShadow"; }
    break;

  case 227:
/* Line 1787 of yacc.c  */
#line 1591 "glsl_parser.yy"
    { (yyval.identifier) = "sampler2DArrayShadow"; }
    break;

  case 228:
/* Line 1787 of yacc.c  */
#line 1592 "glsl_parser.yy"
    { (yyval.identifier) = "samplerBuffer"; }
    break;

  case 229:
/* Line 1787 of yacc.c  */
#line 1593 "glsl_parser.yy"
    { (yyval.identifier) = "samplerCubeArray"; }
    break;

  case 230:
/* Line 1787 of yacc.c  */
#line 1594 "glsl_parser.yy"
    { (yyval.identifier) = "samplerCubeArrayShadow"; }
    break;

  case 231:
/* Line 1787 of yacc.c  */
#line 1595 "glsl_parser.yy"
    { (yyval.identifier) = "isampler1D"; }
    break;

  case 232:
/* Line 1787 of yacc.c  */
#line 1596 "glsl_parser.yy"
    { (yyval.identifier) = "isampler2D"; }
    break;

  case 233:
/* Line 1787 of yacc.c  */
#line 1597 "glsl_parser.yy"
    { (yyval.identifier) = "isampler2DRect"; }
    break;

  case 234:
/* Line 1787 of yacc.c  */
#line 1598 "glsl_parser.yy"
    { (yyval.identifier) = "isampler3D"; }
    break;

  case 235:
/* Line 1787 of yacc.c  */
#line 1599 "glsl_parser.yy"
    { (yyval.identifier) = "isamplerCube"; }
    break;

  case 236:
/* Line 1787 of yacc.c  */
#line 1600 "glsl_parser.yy"
    { (yyval.identifier) = "isampler1DArray"; }
    break;

  case 237:
/* Line 1787 of yacc.c  */
#line 1601 "glsl_parser.yy"
    { (yyval.identifier) = "isampler2DArray"; }
    break;

  case 238:
/* Line 1787 of yacc.c  */
#line 1602 "glsl_parser.yy"
    { (yyval.identifier) = "isamplerBuffer"; }
    break;

  case 239:
/* Line 1787 of yacc.c  */
#line 1603 "glsl_parser.yy"
    { (yyval.identifier) = "isamplerCubeArray"; }
    break;

  case 240:
/* Line 1787 of yacc.c  */
#line 1604 "glsl_parser.yy"
    { (yyval.identifier) = "usampler1D"; }
    break;

  case 241:
/* Line 1787 of yacc.c  */
#line 1605 "glsl_parser.yy"
    { (yyval.identifier) = "usampler2D"; }
    break;

  case 242:
/* Line 1787 of yacc.c  */
#line 1606 "glsl_parser.yy"
    { (yyval.identifier) = "usampler2DRect"; }
    break;

  case 243:
/* Line 1787 of yacc.c  */
#line 1607 "glsl_parser.yy"
    { (yyval.identifier) = "usampler3D"; }
    break;

  case 244:
/* Line 1787 of yacc.c  */
#line 1608 "glsl_parser.yy"
    { (yyval.identifier) = "usamplerCube"; }
    break;

  case 245:
/* Line 1787 of yacc.c  */
#line 1609 "glsl_parser.yy"
    { (yyval.identifier) = "usampler1DArray"; }
    break;

  case 246:
/* Line 1787 of yacc.c  */
#line 1610 "glsl_parser.yy"
    { (yyval.identifier) = "usampler2DArray"; }
    break;

  case 247:
/* Line 1787 of yacc.c  */
#line 1611 "glsl_parser.yy"
    { (yyval.identifier) = "usamplerBuffer"; }
    break;

  case 248:
/* Line 1787 of yacc.c  */
#line 1612 "glsl_parser.yy"
    { (yyval.identifier) = "usamplerCubeArray"; }
    break;

  case 249:
/* Line 1787 of yacc.c  */
#line 1613 "glsl_parser.yy"
    { (yyval.identifier) = "sampler2DMS"; }
    break;

  case 250:
/* Line 1787 of yacc.c  */
#line 1614 "glsl_parser.yy"
    { (yyval.identifier) = "isampler2DMS"; }
    break;

  case 251:
/* Line 1787 of yacc.c  */
#line 1615 "glsl_parser.yy"
    { (yyval.identifier) = "usampler2DMS"; }
    break;

  case 252:
/* Line 1787 of yacc.c  */
#line 1616 "glsl_parser.yy"
    { (yyval.identifier) = "sampler2DMSArray"; }
    break;

  case 253:
/* Line 1787 of yacc.c  */
#line 1617 "glsl_parser.yy"
    { (yyval.identifier) = "isampler2DMSArray"; }
    break;

  case 254:
/* Line 1787 of yacc.c  */
#line 1618 "glsl_parser.yy"
    { (yyval.identifier) = "usampler2DMSArray"; }
    break;

  case 255:
/* Line 1787 of yacc.c  */
#line 1623 "glsl_parser.yy"
    {
      state->check_precision_qualifiers_allowed(&(yylsp[(1) - (1)]));
      (yyval.n) = ast_precision_high;
   }
    break;

  case 256:
/* Line 1787 of yacc.c  */
#line 1628 "glsl_parser.yy"
    {
      state->check_precision_qualifiers_allowed(&(yylsp[(1) - (1)]));
      (yyval.n) = ast_precision_medium;
   }
    break;

  case 257:
/* Line 1787 of yacc.c  */
#line 1633 "glsl_parser.yy"
    {
      state->check_precision_qualifiers_allowed(&(yylsp[(1) - (1)]));
      (yyval.n) = ast_precision_low;
   }
    break;

  case 258:
/* Line 1787 of yacc.c  */
#line 1641 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.struct_specifier) = new(ctx) ast_struct_specifier((yyvsp[(2) - (5)].identifier), (yyvsp[(4) - (5)].declarator_list));
      (yyval.struct_specifier)->set_location(yylloc);
      state->symbols->add_type((yyvsp[(2) - (5)].identifier), glsl_type::void_type);
      state->symbols->add_type_ast((yyvsp[(2) - (5)].identifier), new(ctx) ast_type_specifier((yyval.struct_specifier)));
   }
    break;

  case 259:
/* Line 1787 of yacc.c  */
#line 1649 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.struct_specifier) = new(ctx) ast_struct_specifier(NULL, (yyvsp[(3) - (4)].declarator_list));
      (yyval.struct_specifier)->set_location(yylloc);
   }
    break;

  case 260:
/* Line 1787 of yacc.c  */
#line 1658 "glsl_parser.yy"
    {
      (yyval.declarator_list) = (yyvsp[(1) - (1)].declarator_list);
      (yyvsp[(1) - (1)].declarator_list)->link.self_link();
   }
    break;

  case 261:
/* Line 1787 of yacc.c  */
#line 1663 "glsl_parser.yy"
    {
      (yyval.declarator_list) = (yyvsp[(1) - (2)].declarator_list);
      (yyval.declarator_list)->link.insert_before(& (yyvsp[(2) - (2)].declarator_list)->link);
   }
    break;

  case 262:
/* Line 1787 of yacc.c  */
#line 1671 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_fully_specified_type *const type = (yyvsp[(1) - (3)].fully_specified_type);
      type->set_location(yylloc);

      if (type->qualifier.flags.i != 0)
         _mesa_glsl_error(&(yylsp[(1) - (3)]), state,
			  "only precision qualifiers may be applied to "
			  "structure members");

      (yyval.declarator_list) = new(ctx) ast_declarator_list(type);
      (yyval.declarator_list)->set_location(yylloc);

      (yyval.declarator_list)->declarations.push_degenerate_list_at_head(& (yyvsp[(2) - (3)].declaration)->link);
   }
    break;

  case 263:
/* Line 1787 of yacc.c  */
#line 1690 "glsl_parser.yy"
    {
      (yyval.declaration) = (yyvsp[(1) - (1)].declaration);
      (yyvsp[(1) - (1)].declaration)->link.self_link();
   }
    break;

  case 264:
/* Line 1787 of yacc.c  */
#line 1695 "glsl_parser.yy"
    {
      (yyval.declaration) = (yyvsp[(1) - (3)].declaration);
      (yyval.declaration)->link.insert_before(& (yyvsp[(3) - (3)].declaration)->link);
   }
    break;

  case 265:
/* Line 1787 of yacc.c  */
#line 1703 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.declaration) = new(ctx) ast_declaration((yyvsp[(1) - (1)].identifier), false, NULL, NULL);
      (yyval.declaration)->set_location(yylloc);
   }
    break;

  case 266:
/* Line 1787 of yacc.c  */
#line 1709 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.declaration) = new(ctx) ast_declaration((yyvsp[(1) - (4)].identifier), true, (yyvsp[(3) - (4)].expression), NULL);
      (yyval.declaration)->set_location(yylloc);
   }
    break;

  case 268:
/* Line 1787 of yacc.c  */
#line 1719 "glsl_parser.yy"
    {
      (yyval.expression) = (yyvsp[(2) - (3)].expression);
   }
    break;

  case 269:
/* Line 1787 of yacc.c  */
#line 1723 "glsl_parser.yy"
    {
      (yyval.expression) = (yyvsp[(2) - (4)].expression);
   }
    break;

  case 270:
/* Line 1787 of yacc.c  */
#line 1730 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.expression) = new(ctx) ast_aggregate_initializer();
      (yyval.expression)->set_location(yylloc);
      (yyval.expression)->expressions.push_tail(& (yyvsp[(1) - (1)].expression)->link);
   }
    break;

  case 271:
/* Line 1787 of yacc.c  */
#line 1737 "glsl_parser.yy"
    {
      (yyvsp[(1) - (3)].expression)->expressions.push_tail(& (yyvsp[(3) - (3)].expression)->link);
   }
    break;

  case 273:
/* Line 1787 of yacc.c  */
#line 1749 "glsl_parser.yy"
    { (yyval.node) = (ast_node *) (yyvsp[(1) - (1)].compound_statement); }
    break;

  case 281:
/* Line 1787 of yacc.c  */
#line 1764 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.compound_statement) = new(ctx) ast_compound_statement(true, NULL);
      (yyval.compound_statement)->set_location(yylloc);
   }
    break;

  case 282:
/* Line 1787 of yacc.c  */
#line 1770 "glsl_parser.yy"
    {
      state->symbols->push_scope();
   }
    break;

  case 283:
/* Line 1787 of yacc.c  */
#line 1774 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.compound_statement) = new(ctx) ast_compound_statement(true, (yyvsp[(3) - (4)].node));
      (yyval.compound_statement)->set_location(yylloc);
      state->symbols->pop_scope();
   }
    break;

  case 284:
/* Line 1787 of yacc.c  */
#line 1783 "glsl_parser.yy"
    { (yyval.node) = (ast_node *) (yyvsp[(1) - (1)].compound_statement); }
    break;

  case 286:
/* Line 1787 of yacc.c  */
#line 1789 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.compound_statement) = new(ctx) ast_compound_statement(false, NULL);
      (yyval.compound_statement)->set_location(yylloc);
   }
    break;

  case 287:
/* Line 1787 of yacc.c  */
#line 1795 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.compound_statement) = new(ctx) ast_compound_statement(false, (yyvsp[(2) - (3)].node));
      (yyval.compound_statement)->set_location(yylloc);
   }
    break;

  case 288:
/* Line 1787 of yacc.c  */
#line 1804 "glsl_parser.yy"
    {
      if ((yyvsp[(1) - (1)].node) == NULL) {
         _mesa_glsl_error(& (yylsp[(1) - (1)]), state, "<nil> statement\n");
         assert((yyvsp[(1) - (1)].node) != NULL);
      }

      (yyval.node) = (yyvsp[(1) - (1)].node);
      (yyval.node)->link.self_link();
   }
    break;

  case 289:
/* Line 1787 of yacc.c  */
#line 1814 "glsl_parser.yy"
    {
      if ((yyvsp[(2) - (2)].node) == NULL) {
         _mesa_glsl_error(& (yylsp[(2) - (2)]), state, "<nil> statement\n");
         assert((yyvsp[(2) - (2)].node) != NULL);
      }
      (yyval.node) = (yyvsp[(1) - (2)].node);
      (yyval.node)->link.insert_before(& (yyvsp[(2) - (2)].node)->link);
   }
    break;

  case 290:
/* Line 1787 of yacc.c  */
#line 1826 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.node) = new(ctx) ast_expression_statement(NULL);
      (yyval.node)->set_location(yylloc);
   }
    break;

  case 291:
/* Line 1787 of yacc.c  */
#line 1832 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.node) = new(ctx) ast_expression_statement((yyvsp[(1) - (2)].expression));
      (yyval.node)->set_location(yylloc);
   }
    break;

  case 292:
/* Line 1787 of yacc.c  */
#line 1841 "glsl_parser.yy"
    {
      (yyval.node) = new(state) ast_selection_statement((yyvsp[(3) - (5)].expression), (yyvsp[(5) - (5)].selection_rest_statement).then_statement,
                                              (yyvsp[(5) - (5)].selection_rest_statement).else_statement);
      (yyval.node)->set_location(yylloc);
   }
    break;

  case 293:
/* Line 1787 of yacc.c  */
#line 1850 "glsl_parser.yy"
    {
      (yyval.selection_rest_statement).then_statement = (yyvsp[(1) - (3)].node);
      (yyval.selection_rest_statement).else_statement = (yyvsp[(3) - (3)].node);
   }
    break;

  case 294:
/* Line 1787 of yacc.c  */
#line 1855 "glsl_parser.yy"
    {
      (yyval.selection_rest_statement).then_statement = (yyvsp[(1) - (1)].node);
      (yyval.selection_rest_statement).else_statement = NULL;
   }
    break;

  case 295:
/* Line 1787 of yacc.c  */
#line 1863 "glsl_parser.yy"
    {
      (yyval.node) = (ast_node *) (yyvsp[(1) - (1)].expression);
   }
    break;

  case 296:
/* Line 1787 of yacc.c  */
#line 1867 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_declaration *decl = new(ctx) ast_declaration((yyvsp[(2) - (4)].identifier), false, NULL, (yyvsp[(4) - (4)].expression));
      ast_declarator_list *declarator = new(ctx) ast_declarator_list((yyvsp[(1) - (4)].fully_specified_type));
      decl->set_location(yylloc);
      declarator->set_location(yylloc);

      declarator->declarations.push_tail(&decl->link);
      (yyval.node) = declarator;
   }
    break;

  case 297:
/* Line 1787 of yacc.c  */
#line 1885 "glsl_parser.yy"
    {
      (yyval.node) = new(state) ast_switch_statement((yyvsp[(3) - (5)].expression), (yyvsp[(5) - (5)].switch_body));
      (yyval.node)->set_location(yylloc);
   }
    break;

  case 298:
/* Line 1787 of yacc.c  */
#line 1893 "glsl_parser.yy"
    {
      (yyval.switch_body) = new(state) ast_switch_body(NULL);
      (yyval.switch_body)->set_location(yylloc);
   }
    break;

  case 299:
/* Line 1787 of yacc.c  */
#line 1898 "glsl_parser.yy"
    {
      (yyval.switch_body) = new(state) ast_switch_body((yyvsp[(2) - (3)].case_statement_list));
      (yyval.switch_body)->set_location(yylloc);
   }
    break;

  case 300:
/* Line 1787 of yacc.c  */
#line 1906 "glsl_parser.yy"
    {
      (yyval.case_label) = new(state) ast_case_label((yyvsp[(2) - (3)].expression));
      (yyval.case_label)->set_location(yylloc);
   }
    break;

  case 301:
/* Line 1787 of yacc.c  */
#line 1911 "glsl_parser.yy"
    {
      (yyval.case_label) = new(state) ast_case_label(NULL);
      (yyval.case_label)->set_location(yylloc);
   }
    break;

  case 302:
/* Line 1787 of yacc.c  */
#line 1919 "glsl_parser.yy"
    {
      ast_case_label_list *labels = new(state) ast_case_label_list();

      labels->labels.push_tail(& (yyvsp[(1) - (1)].case_label)->link);
      (yyval.case_label_list) = labels;
      (yyval.case_label_list)->set_location(yylloc);
   }
    break;

  case 303:
/* Line 1787 of yacc.c  */
#line 1927 "glsl_parser.yy"
    {
      (yyval.case_label_list) = (yyvsp[(1) - (2)].case_label_list);
      (yyval.case_label_list)->labels.push_tail(& (yyvsp[(2) - (2)].case_label)->link);
   }
    break;

  case 304:
/* Line 1787 of yacc.c  */
#line 1935 "glsl_parser.yy"
    {
      ast_case_statement *stmts = new(state) ast_case_statement((yyvsp[(1) - (2)].case_label_list));
      stmts->set_location(yylloc);

      stmts->stmts.push_tail(& (yyvsp[(2) - (2)].node)->link);
      (yyval.case_statement) = stmts;
   }
    break;

  case 305:
/* Line 1787 of yacc.c  */
#line 1943 "glsl_parser.yy"
    {
      (yyval.case_statement) = (yyvsp[(1) - (2)].case_statement);
      (yyval.case_statement)->stmts.push_tail(& (yyvsp[(2) - (2)].node)->link);
   }
    break;

  case 306:
/* Line 1787 of yacc.c  */
#line 1951 "glsl_parser.yy"
    {
      ast_case_statement_list *cases= new(state) ast_case_statement_list();
      cases->set_location(yylloc);

      cases->cases.push_tail(& (yyvsp[(1) - (1)].case_statement)->link);
      (yyval.case_statement_list) = cases;
   }
    break;

  case 307:
/* Line 1787 of yacc.c  */
#line 1959 "glsl_parser.yy"
    {
      (yyval.case_statement_list) = (yyvsp[(1) - (2)].case_statement_list);
      (yyval.case_statement_list)->cases.push_tail(& (yyvsp[(2) - (2)].case_statement)->link);
   }
    break;

  case 308:
/* Line 1787 of yacc.c  */
#line 1967 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.node) = new(ctx) ast_iteration_statement(ast_iteration_statement::ast_while,
                                            NULL, (yyvsp[(3) - (5)].node), NULL, (yyvsp[(5) - (5)].node));
      (yyval.node)->set_location(yylloc);
   }
    break;

  case 309:
/* Line 1787 of yacc.c  */
#line 1974 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.node) = new(ctx) ast_iteration_statement(ast_iteration_statement::ast_do_while,
                                            NULL, (yyvsp[(5) - (7)].expression), NULL, (yyvsp[(2) - (7)].node));
      (yyval.node)->set_location(yylloc);
   }
    break;

  case 310:
/* Line 1787 of yacc.c  */
#line 1981 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.node) = new(ctx) ast_iteration_statement(ast_iteration_statement::ast_for,
                                            (yyvsp[(3) - (6)].node), (yyvsp[(4) - (6)].for_rest_statement).cond, (yyvsp[(4) - (6)].for_rest_statement).rest, (yyvsp[(6) - (6)].node));
      (yyval.node)->set_location(yylloc);
   }
    break;

  case 314:
/* Line 1787 of yacc.c  */
#line 1997 "glsl_parser.yy"
    {
      (yyval.node) = NULL;
   }
    break;

  case 315:
/* Line 1787 of yacc.c  */
#line 2004 "glsl_parser.yy"
    {
      (yyval.for_rest_statement).cond = (yyvsp[(1) - (2)].node);
      (yyval.for_rest_statement).rest = NULL;
   }
    break;

  case 316:
/* Line 1787 of yacc.c  */
#line 2009 "glsl_parser.yy"
    {
      (yyval.for_rest_statement).cond = (yyvsp[(1) - (3)].node);
      (yyval.for_rest_statement).rest = (yyvsp[(3) - (3)].expression);
   }
    break;

  case 317:
/* Line 1787 of yacc.c  */
#line 2018 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.node) = new(ctx) ast_jump_statement(ast_jump_statement::ast_continue, NULL);
      (yyval.node)->set_location(yylloc);
   }
    break;

  case 318:
/* Line 1787 of yacc.c  */
#line 2024 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.node) = new(ctx) ast_jump_statement(ast_jump_statement::ast_break, NULL);
      (yyval.node)->set_location(yylloc);
   }
    break;

  case 319:
/* Line 1787 of yacc.c  */
#line 2030 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.node) = new(ctx) ast_jump_statement(ast_jump_statement::ast_return, NULL);
      (yyval.node)->set_location(yylloc);
   }
    break;

  case 320:
/* Line 1787 of yacc.c  */
#line 2036 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.node) = new(ctx) ast_jump_statement(ast_jump_statement::ast_return, (yyvsp[(2) - (3)].expression));
      (yyval.node)->set_location(yylloc);
   }
    break;

  case 321:
/* Line 1787 of yacc.c  */
#line 2042 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.node) = new(ctx) ast_jump_statement(ast_jump_statement::ast_discard, NULL);
      (yyval.node)->set_location(yylloc);
   }
    break;

  case 322:
/* Line 1787 of yacc.c  */
#line 2050 "glsl_parser.yy"
    { (yyval.node) = (yyvsp[(1) - (1)].function_definition); }
    break;

  case 323:
/* Line 1787 of yacc.c  */
#line 2051 "glsl_parser.yy"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 324:
/* Line 1787 of yacc.c  */
#line 2052 "glsl_parser.yy"
    { (yyval.node) = NULL; }
    break;

  case 325:
/* Line 1787 of yacc.c  */
#line 2053 "glsl_parser.yy"
    { (yyval.node) = NULL; }
    break;

  case 326:
/* Line 1787 of yacc.c  */
#line 2058 "glsl_parser.yy"
    {
      void *ctx = state;
      (yyval.function_definition) = new(ctx) ast_function_definition();
      (yyval.function_definition)->set_location(yylloc);
      (yyval.function_definition)->prototype = (yyvsp[(1) - (2)].function);
      (yyval.function_definition)->body = (yyvsp[(2) - (2)].compound_statement);

      state->symbols->pop_scope();
   }
    break;

  case 327:
/* Line 1787 of yacc.c  */
#line 2072 "glsl_parser.yy"
    {
      (yyval.node) = (yyvsp[(1) - (1)].interface_block);
   }
    break;

  case 328:
/* Line 1787 of yacc.c  */
#line 2076 "glsl_parser.yy"
    {
      ast_interface_block *block = (yyvsp[(2) - (2)].interface_block);
      if (!block->layout.merge_qualifier(& (yylsp[(1) - (2)]), state, (yyvsp[(1) - (2)].type_qualifier))) {
         YYERROR;
      }
      (yyval.node) = block;
   }
    break;

  case 329:
/* Line 1787 of yacc.c  */
#line 2087 "glsl_parser.yy"
    {
      ast_interface_block *const block = (yyvsp[(6) - (7)].interface_block);

      block->block_name = (yyvsp[(2) - (7)].identifier);
      block->declarations.push_degenerate_list_at_head(& (yyvsp[(4) - (7)].declarator_list)->link);

      if ((yyvsp[(1) - (7)].type_qualifier).flags.q.uniform) {
         if (!state->ARB_uniform_buffer_object_enable) {
            _mesa_glsl_error(& (yylsp[(1) - (7)]), state,
                             "#version 140 / GL_ARB_uniform_buffer_object "
                             "required for defining uniform blocks\n");
         } else if (state->ARB_uniform_buffer_object_warn) {
            _mesa_glsl_warning(& (yylsp[(1) - (7)]), state,
                               "#version 140 / GL_ARB_uniform_buffer_object "
                               "required for defining uniform blocks\n");
         }
      } else {
         if (state->es_shader || state->language_version < 150) {
            _mesa_glsl_error(& (yylsp[(1) - (7)]), state,
                             "#version 150 required for using "
                             "interface blocks.\n");
         }
      }

      /* From the GLSL 1.50.11 spec, section 4.3.7 ("Interface Blocks"):
       * "It is illegal to have an input block in a vertex shader
       *  or an output block in a fragment shader"
       */
      if ((state->target == vertex_shader) && (yyvsp[(1) - (7)].type_qualifier).flags.q.in) {
         _mesa_glsl_error(& (yylsp[(1) - (7)]), state,
                          "`in' interface block is not allowed for "
                          "a vertex shader\n");
      } else if ((state->target == fragment_shader) && (yyvsp[(1) - (7)].type_qualifier).flags.q.out) {
         _mesa_glsl_error(& (yylsp[(1) - (7)]), state,
                          "`out' interface block is not allowed for "
                          "a fragment shader\n");
      }

      /* Since block arrays require names, and both features are added in
       * the same language versions, we don't have to explicitly
       * version-check both things.
       */
      if (block->instance_name != NULL) {
         state->check_version(150, 300, & (yylsp[(1) - (7)]), "interface blocks with "
                               "an instance name are not allowed");
      }

      unsigned interface_type_mask;
      struct ast_type_qualifier temp_type_qualifier;

      /* Get a bitmask containing only the in/out/uniform flags, allowing us
       * to ignore other irrelevant flags like interpolation qualifiers.
       */
      temp_type_qualifier.flags.i = 0;
      temp_type_qualifier.flags.q.uniform = true;
      temp_type_qualifier.flags.q.in = true;
      temp_type_qualifier.flags.q.out = true;
      interface_type_mask = temp_type_qualifier.flags.i;

      /* Get the block's interface qualifier.  The interface_qualifier
       * production rule guarantees that only one bit will be set (and
       * it will be in/out/uniform).
       */
       unsigned block_interface_qualifier = (yyvsp[(1) - (7)].type_qualifier).flags.i;

      block->layout.flags.i |= block_interface_qualifier;

      foreach_list_typed (ast_declarator_list, member, link, &block->declarations) {
         ast_type_qualifier& qualifier = member->type->qualifier;
         if ((qualifier.flags.i & interface_type_mask) == 0) {
            /* GLSLangSpec.1.50.11, 4.3.7 (Interface Blocks):
             * "If no optional qualifier is used in a member declaration, the
             *  qualifier of the variable is just in, out, or uniform as declared
             *  by interface-qualifier."
             */
            qualifier.flags.i |= block_interface_qualifier;
         } else if ((qualifier.flags.i & interface_type_mask) !=
                    block_interface_qualifier) {
            /* GLSLangSpec.1.50.11, 4.3.7 (Interface Blocks):
             * "If optional qualifiers are used, they can include interpolation
             *  and storage qualifiers and they must declare an input, output,
             *  or uniform variable consistent with the interface qualifier of
             *  the block."
             */
            _mesa_glsl_error(& (yylsp[(1) - (7)]), state,
                             "uniform/in/out qualifier on "
                             "interface block member does not match "
                             "the interface block\n");
         }
      }

      (yyval.interface_block) = block;
   }
    break;

  case 330:
/* Line 1787 of yacc.c  */
#line 2184 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.in = 1;
   }
    break;

  case 331:
/* Line 1787 of yacc.c  */
#line 2189 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.out = 1;
   }
    break;

  case 332:
/* Line 1787 of yacc.c  */
#line 2194 "glsl_parser.yy"
    {
      memset(& (yyval.type_qualifier), 0, sizeof((yyval.type_qualifier)));
      (yyval.type_qualifier).flags.q.uniform = 1;
   }
    break;

  case 333:
/* Line 1787 of yacc.c  */
#line 2202 "glsl_parser.yy"
    {
      (yyval.interface_block) = new(state) ast_interface_block(*state->default_uniform_qualifier,
                                          NULL, NULL);
   }
    break;

  case 334:
/* Line 1787 of yacc.c  */
#line 2207 "glsl_parser.yy"
    {
      (yyval.interface_block) = new(state) ast_interface_block(*state->default_uniform_qualifier,
                                          (yyvsp[(1) - (1)].identifier), NULL);
   }
    break;

  case 335:
/* Line 1787 of yacc.c  */
#line 2212 "glsl_parser.yy"
    {
      (yyval.interface_block) = new(state) ast_interface_block(*state->default_uniform_qualifier,
                                          (yyvsp[(1) - (4)].identifier), (yyvsp[(3) - (4)].expression));
   }
    break;

  case 336:
/* Line 1787 of yacc.c  */
#line 2217 "glsl_parser.yy"
    {
      _mesa_glsl_error(& (yylsp[(1) - (3)]), state,
                       "instance block arrays must be explicitly sized\n");

      (yyval.interface_block) = new(state) ast_interface_block(*state->default_uniform_qualifier,
                                          (yyvsp[(1) - (3)].identifier), NULL);
   }
    break;

  case 337:
/* Line 1787 of yacc.c  */
#line 2228 "glsl_parser.yy"
    {
      (yyval.declarator_list) = (yyvsp[(1) - (1)].declarator_list);
      (yyvsp[(1) - (1)].declarator_list)->link.self_link();
   }
    break;

  case 338:
/* Line 1787 of yacc.c  */
#line 2233 "glsl_parser.yy"
    {
      (yyval.declarator_list) = (yyvsp[(1) - (2)].declarator_list);
      (yyvsp[(2) - (2)].declarator_list)->link.insert_before(& (yyval.declarator_list)->link);
   }
    break;

  case 339:
/* Line 1787 of yacc.c  */
#line 2241 "glsl_parser.yy"
    {
      void *ctx = state;
      ast_fully_specified_type *type = (yyvsp[(1) - (3)].fully_specified_type);
      type->set_location(yylloc);

      if (type->qualifier.flags.q.attribute) {
         _mesa_glsl_error(& (yylsp[(1) - (3)]), state,
                          "keyword 'attribute' cannot be used with "
                          "interface block member\n");
      } else if (type->qualifier.flags.q.varying) {
         _mesa_glsl_error(& (yylsp[(1) - (3)]), state,
                          "keyword 'varying' cannot be used with "
                          "interface block member\n");
      }

      (yyval.declarator_list) = new(ctx) ast_declarator_list(type);
      (yyval.declarator_list)->set_location(yylloc);

      (yyval.declarator_list)->declarations.push_degenerate_list_at_head(& (yyvsp[(2) - (3)].declaration)->link);
   }
    break;

  case 340:
/* Line 1787 of yacc.c  */
#line 2265 "glsl_parser.yy"
    {
      if (!state->default_uniform_qualifier->merge_qualifier(& (yylsp[(1) - (3)]), state, (yyvsp[(1) - (3)].type_qualifier))) {
         YYERROR;
      }
   }
    break;


/* Line 1787 of yacc.c  */
#line 5823 "glsl_parser.cpp"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, state, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, state, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, state);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, state);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, state, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, state);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, state);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


