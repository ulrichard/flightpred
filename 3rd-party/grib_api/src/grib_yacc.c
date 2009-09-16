/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* All symbols defined below should begin with grib_yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum grib_yytokentype {
     IF = 258,
     ELSE = 259,
     END = 260,
     UNSIGNED = 261,
     TEMPLATE = 262,
     TEMPLATE_NOFAIL = 263,
     TRIGGER = 264,
     ASCII = 265,
     KSEC1EXPVER = 266,
     LABEL = 267,
     LIST = 268,
     WHILE = 269,
     IBMFLOAT = 270,
     SIGNED = 271,
     BYTE = 272,
     CODETABLE = 273,
     COMPLEX_CODETABLE = 274,
     LOOKUP = 275,
     ALIAS = 276,
     UNALIAS = 277,
     META = 278,
     POS = 279,
     INTCONST = 280,
     TRANS = 281,
     FLAGBIT = 282,
     CONCEPT = 283,
     CONCEPT_NOFAIL = 284,
     NIL = 285,
     MODIFY = 286,
     READ_ONLY = 287,
     STRING_TYPE = 288,
     LONG_TYPE = 289,
     NO_COPY = 290,
     DUMP = 291,
     REQUIRED = 292,
     EDITION_SPECIFIC = 293,
     OVERRIDE = 294,
     HIDDEN = 295,
     CAN_BE_MISSING = 296,
     MISSING = 297,
     CONSTRAINT = 298,
     COPY_OK = 299,
     WHEN = 300,
     LATE_WHEN = 301,
     SET = 302,
     WRITE = 303,
     APPEND = 304,
     PRINT = 305,
     EXPORT = 306,
     REMOVE = 307,
     SKIP = 308,
     PAD = 309,
     SECTION_PADDING = 310,
     PADTO = 311,
     PADTOEVEN = 312,
     PADTOMULTIPLE = 313,
     G1_HALF_BYTE = 314,
     G1_MESSAGE_LENGTH = 315,
     G1_SECTION4_LENGTH = 316,
     SECTION_LENGTH = 317,
     FLAG = 318,
     ITERATOR = 319,
     NEAREST = 320,
     KSEC = 321,
     ASSERT = 322,
     CASE = 323,
     SWITCH = 324,
     DEFAULT = 325,
     EQ = 326,
     NE = 327,
     GE = 328,
     LE = 329,
     BIT = 330,
     BITOFF = 331,
     AND = 332,
     OR = 333,
     NOT = 334,
     IS = 335,
     IDENT = 336,
     STRING = 337,
     INTEGER = 338,
     FLOAT = 339
   };
#endif
/* Tokens.  */
#define IF 258
#define ELSE 259
#define END 260
#define UNSIGNED 261
#define TEMPLATE 262
#define TEMPLATE_NOFAIL 263
#define TRIGGER 264
#define ASCII 265
#define KSEC1EXPVER 266
#define LABEL 267
#define LIST 268
#define WHILE 269
#define IBMFLOAT 270
#define SIGNED 271
#define BYTE 272
#define CODETABLE 273
#define COMPLEX_CODETABLE 274
#define LOOKUP 275
#define ALIAS 276
#define UNALIAS 277
#define META 278
#define POS 279
#define INTCONST 280
#define TRANS 281
#define FLAGBIT 282
#define CONCEPT 283
#define CONCEPT_NOFAIL 284
#define NIL 285
#define MODIFY 286
#define READ_ONLY 287
#define STRING_TYPE 288
#define LONG_TYPE 289
#define NO_COPY 290
#define DUMP 291
#define REQUIRED 292
#define EDITION_SPECIFIC 293
#define OVERRIDE 294
#define HIDDEN 295
#define CAN_BE_MISSING 296
#define MISSING 297
#define CONSTRAINT 298
#define COPY_OK 299
#define WHEN 300
#define LATE_WHEN 301
#define SET 302
#define WRITE 303
#define APPEND 304
#define PRINT 305
#define EXPORT 306
#define REMOVE 307
#define SKIP 308
#define PAD 309
#define SECTION_PADDING 310
#define PADTO 311
#define PADTOEVEN 312
#define PADTOMULTIPLE 313
#define G1_HALF_BYTE 314
#define G1_MESSAGE_LENGTH 315
#define G1_SECTION4_LENGTH 316
#define SECTION_LENGTH 317
#define FLAG 318
#define ITERATOR 319
#define NEAREST 320
#define KSEC 321
#define ASSERT 322
#define CASE 323
#define SWITCH 324
#define DEFAULT 325
#define EQ 326
#define NE 327
#define GE 328
#define LE 329
#define BIT 330
#define BITOFF 331
#define AND 332
#define OR 333
#define NOT 334
#define IS 335
#define IDENT 336
#define STRING 337
#define INTEGER 338
#define FLOAT 339




/* Copy the first part of user declarations.  */
#line 11 "griby.y"


#include "grib_api_internal.h"
/* #include "grib_parser.h" */

extern int grib_yylex();
extern int grib_yyerror(const char*);

extern   grib_action*           grib_parser_all_actions;
extern   grib_concept_value*    grib_parser_concept;
extern   grib_context*          grib_parser_context;
extern   grib_rule*             grib_parser_rules;

static grib_concept_value* reverse(grib_concept_value* r);
static grib_concept_value *reverse_concept(grib_concept_value *r,grib_concept_value *s);

/* typedef int (*testp_proc)(long,long); */
/* typedef long (*op_proc)(long,long);   */

static long op_eq(long a, long b)     {return a == b;}
static long op_ne(long a, long b)     {return a != b;}
static long op_lt(long a, long b)     {return a <  b;}
static long op_gt(long a, long b)     {return a >  b;}
static long op_and(long a, long b)    {return a && b;}
static long op_or(long a, long b)     {return a || b;}
static long op_ge(long a, long b)     {return a >= b;}
static long op_le(long a, long b)     {return a <= b;}

static long op_bit(long a, long b)    {return a&(1<<b);}
static long op_bitoff(long a, long b) {return !op_bit(a,b);}

static long op_not(long a) {return !a;}
static long op_neg(long a) {return -a;}

static double op_neg_d(double a) {return -a;}

static long op_pow(long a, long b) {return grib_power(a,b);}
static long op_add(long a, long b) {return a+b;}
static long op_sub(long a, long b) {return a-b;}
static long op_div(long a, long b) {return a/b;}
static long op_mul(long a, long b) {return a*b;}
static long op_modulo(long a, long b) {return a%b;}

static double op_mul_d(double a, double b) {return a*b;}
static double op_div_d(double a, double b) {return a/b;}
static double op_add_d(double a, double b) {return a+b;}
static double op_sub_d(double a, double b) {return a-b;}

static double op_eq_d(double a, double b)     {return a == b;}
static double op_ne_d(double a, double b)     {return a != b;}
static double op_lt_d(double a, double b)     {return a <  b;}
static double op_gt_d(double a, double b)     {return a >  b;}
static double op_ge_d(double a, double b)     {return a >= b;}
static double op_le_d(double a, double b)     {return a <= b;}



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 68 "griby.y"
{
    char                    *str;
    long                    lval;
    double                  dval;
    grib_darray             *dvalue;
    grib_iarray             *ivalue;
    grib_action             *act;
    grib_arguments          *explist;
    grib_expression         *exp;
    grib_concept_condition  *concept_condition;
    grib_concept_value      *concept_value;
	grib_case               *case_value;
    grib_concept_value_name *concept_names;
  grib_rule               *rules;
  grib_rule_entry         *rule_entry;
}
/* Line 187 of yacc.c.  */
#line 338 "y.tab.c"
	YYSTYPE;
# define grib_yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 351 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 grib_yytype_uint8;
#else
typedef unsigned char grib_yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 grib_yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char grib_yytype_int8;
#else
typedef short int grib_yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 grib_yytype_uint16;
#else
typedef unsigned short int grib_yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 grib_yytype_int16;
#else
typedef short int grib_yytype_int16;
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
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined grib_yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined grib_yyoverflow || YYERROR_VERBOSE */


#if (! defined grib_yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union grib_yyalloc
{
  grib_yytype_int16 grib_yyss;
  YYSTYPE grib_yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union grib_yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (grib_yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T grib_yyi;				\
	  for (grib_yyi = 0; grib_yyi < (Count); grib_yyi++)	\
	    (To)[grib_yyi] = (From)[grib_yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T grib_yynewbytes;						\
	YYCOPY (&grib_yyptr->Stack, Stack, grib_yysize);				\
	Stack = &grib_yyptr->Stack;						\
	grib_yynewbytes = grib_yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	grib_yyptr += grib_yynewbytes / sizeof (*grib_yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  151
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1111

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  104
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  47
/* YYNRULES -- Number of rules.  */
#define YYNRULES  205
/* YYNRULES -- Number of states.  */
#define YYNSTATES  625

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   339

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? grib_yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const grib_yytype_uint8 grib_yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   100,     2,     2,
      89,    90,    98,   101,    85,    96,    91,    99,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    95,    86,
     103,    92,   102,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    87,     2,    88,    97,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    93,     2,    94,     2,     2,     2,     2,
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
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const grib_yytype_uint16 grib_yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    14,    16,
      20,    22,    26,    28,    31,    35,    38,    41,    43,    45,
      47,    49,    51,    53,    55,    57,    60,    62,    64,    66,
      70,    72,    80,    91,    99,   107,   115,   126,   134,   142,
     153,   162,   173,   182,   193,   202,   212,   220,   223,   226,
     231,   238,   246,   249,   255,   261,   266,   273,   281,   284,
     290,   299,   308,   312,   318,   324,   330,   336,   340,   344,
     348,   354,   363,   366,   374,   385,   390,   399,   410,   416,
     422,   428,   431,   436,   440,   445,   450,   457,   460,   462,
     465,   467,   470,   476,   478,   486,   498,   505,   513,   525,
     532,   540,   552,   557,   560,   564,   566,   569,   571,   574,
     576,   580,   582,   584,   586,   588,   590,   592,   594,   596,
     598,   600,   602,   604,   606,   615,   623,   631,   638,   648,
     661,   672,   687,   700,   709,   721,   728,   738,   751,   762,
     777,   790,   799,   811,   813,   816,   818,   821,   826,   837,
     847,   855,   861,   863,   867,   869,   871,   873,   875,   877,
     880,   885,   887,   889,   891,   893,   895,   897,   901,   904,
     908,   913,   917,   919,   923,   927,   931,   935,   939,   941,
     945,   949,   951,   955,   959,   963,   967,   971,   975,   979,
     982,   984,   988,   990,   994,   996,   998,  1000,  1002,  1007,
    1010,  1012,  1015,  1017,  1025,  1027
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const grib_yytype_int16 grib_yyrhs[] =
{
     105,     0,    -1,   106,    -1,   127,    -1,   108,    -1,   150,
      -1,     1,    -1,    -1,    84,    -1,   107,    85,    84,    -1,
      83,    -1,   107,    85,    83,    -1,   109,    -1,   109,   108,
      -1,   109,    86,   108,    -1,   109,    86,    -1,   114,    86,
      -1,   115,    -1,   123,    -1,   124,    -1,   125,    -1,   126,
      -1,   116,    -1,   130,    -1,    86,    -1,   110,    86,    -1,
     106,    -1,   112,    -1,   113,    -1,   113,    85,   112,    -1,
     144,    -1,     6,    87,    83,    88,    81,   119,   120,    -1,
       6,    87,    83,    88,    81,    87,   111,    88,   119,   120,
      -1,    10,    87,    83,    88,    81,   119,   120,    -1,    10,
      87,    83,    88,    82,   119,   120,    -1,    17,    87,    83,
      88,    81,   119,   120,    -1,    17,    87,    83,    88,    81,
      87,   111,    88,   119,   120,    -1,    11,    87,    83,    88,
      81,   119,   120,    -1,    16,    87,    83,    88,    81,   119,
     120,    -1,    16,    87,    83,    88,    81,    87,   111,    88,
     119,   120,    -1,    18,    87,    83,    88,    81,   113,   119,
     120,    -1,    18,    87,    83,    88,    81,    89,   111,    90,
     119,   120,    -1,    19,    87,    83,    88,    81,   113,   119,
     120,    -1,    19,    87,    83,    88,    81,    89,   111,    90,
     119,   120,    -1,    63,    87,    83,    88,    81,   113,   119,
     120,    -1,    20,    87,    83,    88,    81,    89,   111,    90,
     120,    -1,    27,    81,    89,   111,    90,   119,   120,    -1,
      12,    81,    -1,    12,    82,    -1,    15,    81,   119,   120,
      -1,    15,    81,    91,    81,   119,   120,    -1,    15,    81,
      87,   113,    88,   119,   120,    -1,    24,    81,    -1,    25,
      81,    92,   113,   120,    -1,    26,    81,    92,   113,   120,
      -1,    84,    81,   119,   120,    -1,    84,    81,    91,    81,
     119,   120,    -1,    84,    81,    87,   113,    88,   119,   120,
      -1,    59,    81,    -1,    62,    87,    83,    88,    81,    -1,
      60,    87,    83,    88,    81,    89,   111,    90,    -1,    61,
      87,    83,    88,    81,    89,   111,    90,    -1,    66,    81,
     113,    -1,    54,    81,    89,   111,    90,    -1,    56,    81,
      89,   111,    90,    -1,    57,    81,    89,   111,    90,    -1,
      58,    81,    89,   111,    90,    -1,    55,    81,   120,    -1,
       7,    81,    82,    -1,     8,    81,    82,    -1,    21,    81,
      92,    81,   120,    -1,    21,    87,    83,    88,    81,    92,
      81,   120,    -1,    22,    81,    -1,    21,    81,    91,    81,
      92,    81,   120,    -1,    21,    87,    83,    88,    81,    91,
      81,    92,    81,   120,    -1,    22,    81,    91,    81,    -1,
      23,    81,    81,    89,   111,    90,   119,   120,    -1,    23,
      81,    91,    81,    81,    89,   111,    90,   119,   120,    -1,
      64,    81,    89,   111,    90,    -1,    65,    81,    89,   111,
      90,    -1,    51,    81,    89,   111,    90,    -1,    52,   111,
      -1,    67,    89,   144,    90,    -1,    31,    81,   120,    -1,
      47,    81,    92,    42,    -1,    47,    81,    92,   144,    -1,
      47,    81,    92,    93,   107,    94,    -1,    48,    82,    -1,
      48,    -1,    49,    82,    -1,    49,    -1,    50,    82,    -1,
      50,    89,    82,    90,    82,    -1,    50,    -1,     3,    89,
     144,    90,    93,   108,    94,    -1,     3,    89,   144,    90,
      93,   108,    94,     4,    93,   108,    94,    -1,    45,    89,
     144,    90,   117,   110,    -1,    45,    89,   144,    90,    93,
     118,    94,    -1,    45,    89,   144,    90,    93,   118,    94,
       4,    93,   118,    94,    -1,    46,    89,   144,    90,   117,
     110,    -1,    46,    89,   144,    90,    93,   118,    94,    -1,
      46,    89,   144,    90,    93,   118,    94,     4,    93,   118,
      94,    -1,    47,    81,    92,   144,    -1,   117,   110,    -1,
     118,   117,   110,    -1,   106,    -1,    92,   111,    -1,   106,
      -1,    95,   121,    -1,   122,    -1,   121,    85,   122,    -1,
      32,    -1,    36,    -1,    35,    -1,    37,    -1,    40,    -1,
      38,    -1,    41,    -1,    43,    -1,    39,    -1,    44,    -1,
      26,    -1,    33,    -1,    34,    -1,    81,    13,    89,   144,
      90,    93,   108,    94,    -1,    14,    89,   144,    90,    93,
     108,    94,    -1,     9,    89,   111,    90,    93,   108,    94,
      -1,    28,    81,    93,   127,    94,   120,    -1,    28,    81,
      89,    81,    90,    93,   127,    94,   120,    -1,    28,    81,
      89,    81,    85,    82,    85,    81,    85,    81,    90,   120,
      -1,    28,    81,    89,    81,    85,    82,    85,    81,    90,
     120,    -1,    28,    81,    91,    81,    89,    81,    85,    82,
      85,    81,    85,    81,    90,   120,    -1,    28,    81,    91,
      81,    89,    81,    85,    82,    85,    81,    90,   120,    -1,
      28,    81,    91,    81,    93,   127,    94,   120,    -1,    28,
      81,    91,    81,    89,    81,    90,    93,   127,    94,   120,
      -1,    29,    81,    93,   127,    94,   120,    -1,    29,    81,
      89,    81,    90,    93,   127,    94,   120,    -1,    29,    81,
      89,    81,    85,    82,    85,    81,    85,    81,    90,   120,
      -1,    29,    81,    89,    81,    85,    82,    85,    81,    90,
     120,    -1,    29,    81,    91,    81,    89,    81,    85,    82,
      85,    81,    85,    81,    90,   120,    -1,    29,    81,    91,
      81,    89,    81,    85,    82,    85,    81,    90,   120,    -1,
      29,    81,    91,    81,    93,   127,    94,   120,    -1,    29,
      81,    91,    81,    89,    81,    90,    93,   127,    94,   120,
      -1,   131,    -1,   127,   131,    -1,   129,    -1,   128,   129,
      -1,    68,   112,    95,   108,    -1,    69,    89,   111,    90,
      93,   128,    70,    95,   108,    94,    -1,    69,    89,   111,
      90,    93,   128,    70,    95,    94,    -1,    69,    89,   111,
      90,    93,   128,    94,    -1,   132,    92,    93,   134,    94,
      -1,   133,    -1,   133,    85,   132,    -1,    82,    -1,    81,
      -1,    83,    -1,    84,    -1,   135,    -1,   135,   134,    -1,
      81,    92,   144,    86,    -1,    81,    -1,    82,    -1,   136,
      -1,    83,    -1,    84,    -1,    30,    -1,    89,   144,    90,
      -1,    96,   137,    -1,    81,    89,    90,    -1,    81,    89,
     111,    90,    -1,   137,    97,   138,    -1,   137,    -1,   139,
      98,   138,    -1,   139,    99,   138,    -1,   139,   100,   138,
      -1,   139,    75,   138,    -1,   139,    76,   138,    -1,   138,
      -1,   140,   101,   139,    -1,   140,    96,   139,    -1,   139,
      -1,   141,   102,   140,    -1,   141,    71,   140,    -1,   141,
     103,   140,    -1,   141,    73,   140,    -1,   141,    74,   140,
      -1,   141,    72,   140,    -1,   136,    80,   136,    -1,    79,
     141,    -1,   140,    -1,   142,    77,   141,    -1,   141,    -1,
     143,    78,   142,    -1,   142,    -1,   143,    -1,   148,    -1,
     149,    -1,    81,    92,   144,    86,    -1,    53,    86,    -1,
     146,    -1,   146,   147,    -1,   146,    -1,     3,    89,   144,
      90,    93,   147,    94,    -1,   145,    -1,   145,   150,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const grib_yytype_uint16 grib_yyrline[] =
{
       0,   239,   239,   240,   241,   242,   244,   247,   250,   251,
     252,   253,   255,   256,   257,   258,   261,   262,   263,   264,
     265,   266,   267,   268,   272,   273,   276,   277,   280,   281,
     284,   288,   291,   294,   298,   301,   304,   307,   310,   313,
     316,   319,   322,   325,   328,   331,   334,   337,   340,   343,
     346,   349,   352,   355,   358,   361,   364,   367,   370,   373,
     376,   379,   382,   385,   388,   391,   394,   397,   400,   402,
     405,   408,   411,   414,   418,   422,   426,   429,   432,   443,
     454,   457,   460,   463,   466,   467,   468,   470,   471,   472,
     473,   475,   476,   477,   481,   482,   486,   487,   488,   489,
     490,   491,   494,   497,   498,   502,   503,   506,   507,   510,
     511,   514,   515,   516,   517,   518,   519,   520,   521,   522,
     523,   524,   525,   526,   529,   532,   535,   538,   539,   540,
     541,   542,   543,   544,   545,   546,   547,   548,   549,   550,
     551,   552,   553,   557,   558,   561,   562,   565,   569,   570,
     571,   574,   577,   578,   581,   582,   583,   584,   587,   588,
     591,   594,   595,   598,   599,   600,   602,   603,   604,   605,
     606,   610,   611,   614,   615,   616,   617,   618,   619,   622,
     623,   624,   627,   629,   630,   631,   632,   633,   634,   639,
     640,   643,   644,   647,   648,   651,   657,   658,   661,   662,
     665,   666,   669,   673,   676,   677
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const grib_yytname[] =
{
  "$end", "error", "$undefined", "IF", "ELSE", "END", "UNSIGNED",
  "TEMPLATE", "TEMPLATE_NOFAIL", "TRIGGER", "ASCII", "KSEC1EXPVER",
  "LABEL", "LIST", "WHILE", "IBMFLOAT", "SIGNED", "BYTE", "CODETABLE",
  "COMPLEX_CODETABLE", "LOOKUP", "ALIAS", "UNALIAS", "META", "POS",
  "INTCONST", "TRANS", "FLAGBIT", "CONCEPT", "CONCEPT_NOFAIL", "NIL",
  "MODIFY", "READ_ONLY", "STRING_TYPE", "LONG_TYPE", "NO_COPY", "DUMP",
  "REQUIRED", "EDITION_SPECIFIC", "OVERRIDE", "HIDDEN", "CAN_BE_MISSING",
  "MISSING", "CONSTRAINT", "COPY_OK", "WHEN", "LATE_WHEN", "SET", "WRITE",
  "APPEND", "PRINT", "EXPORT", "REMOVE", "SKIP", "PAD", "SECTION_PADDING",
  "PADTO", "PADTOEVEN", "PADTOMULTIPLE", "G1_HALF_BYTE",
  "G1_MESSAGE_LENGTH", "G1_SECTION4_LENGTH", "SECTION_LENGTH", "FLAG",
  "ITERATOR", "NEAREST", "KSEC", "ASSERT", "CASE", "SWITCH", "DEFAULT",
  "EQ", "NE", "GE", "LE", "BIT", "BITOFF", "AND", "OR", "NOT", "IS",
  "IDENT", "STRING", "INTEGER", "FLOAT", "','", "';'", "'['", "']'", "'('",
  "')'", "'.'", "'='", "'{'", "'}'", "':'", "'-'", "'^'", "'*'", "'/'",
  "'%'", "'+'", "'>'", "'<'", "$accept", "all", "empty", "dvalues",
  "instructions", "instruction", "semi", "argument_list", "arguments",
  "argument", "simple", "if_block", "when_block", "set", "set_list",
  "default", "flags", "flag_list", "flag", "list_block", "while_block",
  "trigger_block", "concept_block", "concept_list", "case_list",
  "case_value", "switch_block", "concept_value", "concept_name_list",
  "concept_name", "concept_conditions", "concept_condition",
  "string_or_ident", "atom", "power", "factor", "term", "condition",
  "conjonction", "disjonction", "expression", "rule", "rule_entry",
  "rule_entries", "fact", "conditional_rule", "rules", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const grib_yytype_uint16 grib_yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,    44,    59,    91,    93,    40,
      41,    46,    61,   123,   125,    58,    45,    94,    42,    47,
      37,    43,    62,    60
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const grib_yytype_uint8 grib_yyr1[] =
{
       0,   104,   105,   105,   105,   105,   105,   106,   107,   107,
     107,   107,   108,   108,   108,   108,   109,   109,   109,   109,
     109,   109,   109,   109,   110,   110,   111,   111,   112,   112,
     113,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   115,   115,   116,   116,   116,   116,
     116,   116,   117,   118,   118,   119,   119,   120,   120,   121,
     121,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   123,   124,   125,   126,   126,   126,
     126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   127,   127,   128,   128,   129,   130,   130,
     130,   131,   132,   132,   133,   133,   133,   133,   134,   134,
     135,   136,   136,   137,   137,   137,   137,   137,   137,   137,
     137,   138,   138,   139,   139,   139,   139,   139,   139,   140,
     140,   140,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   142,   142,   143,   143,   144,   145,   145,   146,   146,
     147,   147,   148,   149,   150,   150
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const grib_yytype_uint8 grib_yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     0,     1,     3,
       1,     3,     1,     2,     3,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     3,
       1,     7,    10,     7,     7,     7,    10,     7,     7,    10,
       8,    10,     8,    10,     8,     9,     7,     2,     2,     4,
       6,     7,     2,     5,     5,     4,     6,     7,     2,     5,
       8,     8,     3,     5,     5,     5,     5,     3,     3,     3,
       5,     8,     2,     7,    10,     4,     8,    10,     5,     5,
       5,     2,     4,     3,     4,     4,     6,     2,     1,     2,
       1,     2,     5,     1,     7,    11,     6,     7,    11,     6,
       7,    11,     4,     2,     3,     1,     2,     1,     2,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     8,     7,     7,     6,     9,    12,
      10,    14,    12,     8,    11,     6,     9,    12,    10,    14,
      12,     8,    11,     1,     2,     1,     2,     4,    10,     9,
       7,     5,     1,     3,     1,     1,     1,     1,     1,     2,
       4,     1,     1,     1,     1,     1,     1,     3,     2,     3,
       4,     3,     1,     3,     3,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     3,     3,     3,     2,
       1,     3,     1,     3,     1,     1,     1,     1,     4,     2,
       1,     2,     1,     7,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const grib_yytype_uint8 grib_yydefact[] =
{
       0,     6,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      88,    90,    93,     0,     7,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   155,   154,   156,   157,     0,     2,     4,    12,     0,
      17,    22,    18,    19,    20,    21,     3,    23,   143,     0,
     152,   204,   202,   196,   197,     5,     0,     0,     0,     0,
       7,     0,     0,    47,    48,     0,     7,     0,     0,     0,
       0,     0,     0,     0,    72,     0,    52,     0,     0,     0,
       0,     0,     7,     0,     0,     0,    87,    89,    91,     0,
       0,   166,     0,   161,   162,   164,   165,     0,     0,    26,
      81,    27,    28,   163,   172,   178,   181,   190,   192,   194,
     195,    30,   199,     0,     7,     0,     0,     0,    58,     0,
       0,     0,     0,     0,     0,     0,     0,     7,     0,     0,
       7,     1,     0,     0,     0,    15,    13,    16,   155,   157,
     144,     0,     0,     0,     0,   205,     0,     0,    68,    69,
       0,     0,     0,     0,     0,     0,     7,   105,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     7,     0,     0,     0,     0,     0,     0,     0,
     107,    83,     0,     0,     0,     0,     7,   189,     0,     0,
     163,   168,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       7,    67,     7,     7,     7,     0,     0,     0,     0,     7,
       7,    62,     0,     0,     0,     0,     0,     0,     7,     0,
      14,     0,   153,     0,     0,     0,     0,     0,     0,     0,
       0,     7,   106,    49,     0,     0,     0,     0,     0,     0,
       7,     0,    75,     7,     0,     7,     7,     0,     0,     0,
       0,     0,     0,     0,   121,   111,   122,   123,   113,   112,
     114,   116,   119,   115,   117,   118,   120,   108,   109,     0,
       0,    84,     0,    85,     0,     0,   169,     0,   167,    29,
     161,   188,   171,   176,   177,   173,   174,   175,   180,   179,
     183,   187,   185,   186,   182,   184,   191,   193,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    82,     0,
       0,   198,     0,     7,    55,     0,     0,     0,   158,     0,
       0,     7,     0,     7,     7,     7,     0,     7,     7,     7,
       7,     0,     0,     0,     0,    70,     0,     0,     0,    53,
      54,     7,     0,     0,     0,     0,     7,     0,     0,     0,
       0,     7,     0,     0,     0,     0,     0,     0,    10,     8,
       0,    92,    80,   170,    63,    64,    65,    66,     0,     0,
      59,     0,    78,    79,     0,     0,     7,     7,     0,     0,
     151,   159,     0,     0,     0,   200,     0,     7,     7,     0,
       7,     7,     7,     0,     7,    50,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     0,     0,     7,     7,
       7,     0,     0,     0,     0,   127,     0,     0,     0,     0,
     135,   110,     0,     0,     0,    24,    96,     0,    99,     0,
      86,     7,     7,     7,     0,     0,   145,     0,     7,    56,
       0,     0,     0,    94,   201,   203,     0,    31,   126,    33,
      34,    37,   125,    51,     0,    38,     0,    35,     0,    30,
       7,     0,     7,     0,    73,     0,     7,     7,     0,    46,
       0,     0,     0,     0,     7,     0,     0,     0,     0,     7,
       0,   103,    97,     0,    25,   100,    11,     9,     0,     0,
       7,     0,     0,   150,   146,     0,    57,   160,     0,     7,
       7,     7,     7,    40,     7,    42,     7,     0,    71,    76,
       7,     0,     7,     0,     0,   133,     0,     7,     0,     0,
     141,   102,     0,   104,     0,    60,    61,    44,     0,     0,
     124,     0,     7,     7,     7,     7,     7,    45,     7,     7,
       0,     7,   128,     0,     0,     0,     7,   136,     0,     0,
       0,     0,   147,   149,     0,     0,    32,    39,    36,    41,
      43,    74,    77,     0,   130,     0,     7,     0,   138,     0,
       7,     0,     0,   148,    95,     7,     0,     7,   134,     7,
       0,     7,   142,    98,   101,   129,     0,   132,   137,     0,
     140,     7,     7,   131,   139
};

/* YYDEFGOTO[NTERM-NUM].  */
static const grib_yytype_int16 grib_yydefgoto[] =
{
      -1,    55,   200,   390,   414,    58,   456,   120,   121,   122,
      59,    60,    61,   453,   454,   178,   201,   297,   298,    62,
      63,    64,    65,    66,   465,   466,    67,    68,    69,    70,
     347,   348,   123,   124,   125,   126,   127,   128,   129,   130,
     131,    71,   415,   416,    73,    74,    75
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -384
static const grib_yytype_int16 grib_yypact[] =
{
     784,  -384,    18,    -3,   -43,    19,    21,    30,    46,    -5,
      60,    70,    72,   101,   109,   133,   137,   -47,   127,   146,
     164,   167,   174,   185,   193,   194,   197,   161,   172,   198,
     199,   214,   -53,   217,    40,   215,   224,   226,   229,   232,
     238,   244,   245,   246,   255,   256,   263,   264,   265,   262,
     268,    -7,  -384,  -384,   278,   365,  -384,  -384,   866,   282,
    -384,  -384,  -384,  -384,  -384,  -384,   -28,  -384,  -384,   279,
     288,     9,  -384,  -384,  -384,  -384,    40,   292,   294,   299,
      40,   300,   301,  -384,  -384,    40,    43,   302,   305,   306,
     307,   309,    74,   314,   308,   -59,  -384,   310,   315,   311,
     180,   206,   303,    40,    40,   316,  -384,  -384,  -384,   327,
     331,  -384,    40,   332,  -384,  -384,  -384,    40,   120,  -384,
    -384,  -384,   337,   343,   328,  -384,    16,   -13,   -23,   349,
     350,  -384,  -384,   338,   303,   340,   341,   342,  -384,   351,
     359,   364,   366,   362,   363,    40,    40,    40,   367,    40,
      71,  -384,   368,   419,   278,  1027,  -384,  -384,  -384,  -384,
    -384,   360,   -28,   369,   370,  -384,   371,   372,  -384,  -384,
     373,   376,   377,   381,    40,   374,    40,  -384,   303,   378,
     384,   385,   386,   387,   395,   396,   394,   403,   397,   406,
      40,    40,    40,   407,   409,   -28,   410,   411,   -28,   250,
    -384,  -384,   404,   405,   -15,   413,    40,   -23,    22,   414,
    -384,  -384,    40,   113,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,    40,    40,
      40,  -384,    40,    40,    40,   418,   420,   421,   422,    40,
      40,  -384,   424,   425,    40,   415,    40,   412,   303,    40,
    -384,   416,  -384,    40,   426,   430,   427,   125,   431,   429,
     435,   432,  -384,  -384,   436,   444,   445,   446,   447,   437,
     303,   449,  -384,    40,   450,   303,   303,   442,   -48,   -54,
      14,    47,    20,    44,  -384,  -384,  -384,  -384,  -384,  -384,
    -384,  -384,  -384,  -384,  -384,  -384,  -384,   448,  -384,   -30,
     -21,  -384,   150,  -384,   452,   453,  -384,   454,  -384,  -384,
    -384,  -384,  -384,  -384,  -384,  -384,  -384,  -384,    16,    16,
     -13,   -13,   -13,   -13,   -13,   -13,   -23,   349,   455,   458,
     459,   460,   461,   470,   471,   472,   464,   465,  -384,   463,
     467,  -384,   474,   432,  -384,   473,   443,   466,   416,   475,
     948,    69,  1027,   432,   432,   432,  1027,   432,   303,    98,
      99,    64,   181,   451,   483,  -384,   144,   476,   478,  -384,
    -384,   432,   487,   479,   490,   -28,   303,   491,   484,   495,
     -28,   303,   250,   497,   532,   494,   532,   494,  -384,  -384,
     -61,  -384,  -384,  -384,  -384,  -384,  -384,  -384,   492,   496,
    -384,    40,  -384,  -384,   515,   493,   432,   303,   498,    40,
    -384,  -384,   499,    -7,   500,   -22,   501,    40,   303,   502,
     303,   303,   303,   506,   303,  -384,    40,   303,    40,   303,
      40,   432,    40,   432,    40,   303,   503,   508,   432,    40,
     303,   505,   -28,   102,    58,  -384,   517,   -28,   129,   148,
    -384,  -384,   512,   494,   -34,  -384,   507,   -33,   507,   169,
    -384,    40,    40,   432,    40,   -49,  -384,  1027,   303,  -384,
    1027,   520,   -22,   603,  -384,  -384,   521,  -384,  -384,  -384,
    -384,  -384,  -384,  -384,   523,  -384,   524,  -384,   518,   414,
     303,   525,   303,   526,  -384,   527,   303,   303,   528,  -384,
     536,   233,   531,   529,   303,   540,   240,   541,   533,   303,
      40,   507,   620,   494,  -384,   623,  -384,  -384,   538,   539,
     303,   535,   542,  -384,  -384,   537,  -384,  -384,   543,   432,
     432,   432,   432,  -384,   432,  -384,   303,   553,  -384,  -384,
     432,   136,   303,   550,   -28,  -384,   138,   303,   554,   -28,
    -384,  -384,   545,   507,   552,  -384,  -384,  -384,  1027,   695,
    -384,  1027,   303,   303,   303,   303,   303,  -384,   303,   303,
     567,   303,  -384,   569,   247,   570,   303,  -384,   571,   312,
     532,   532,  -384,  -384,   559,   561,  -384,  -384,  -384,  -384,
    -384,  -384,  -384,   566,  -384,   153,   303,   568,  -384,   159,
     303,   -29,   -19,  -384,  -384,   303,   576,   303,  -384,   303,
     578,   303,  -384,  -384,  -384,  -384,   572,  -384,  -384,   573,
    -384,   303,   303,  -384,  -384
};

/* YYPGOTO[NTERM-NUM].  */
static const grib_yytype_int16 grib_yypgoto[] =
{
    -384,  -384,     7,  -384,     0,  -384,  -382,   171,  -210,  -144,
    -384,  -384,  -384,  -290,  -383,   -51,    78,  -384,   283,  -384,
    -384,  -384,  -384,  -175,  -384,   195,  -384,   -58,   504,  -384,
     313,  -384,   -45,   546,   121,    38,   192,  -108,   438,  -384,
     -60,  -384,    11,   253,  -384,  -384,   598
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -8
static const grib_yytype_int16 grib_yytable[] =
{
      57,   241,   309,   457,   207,   458,   148,    56,   160,   385,
     387,    72,   163,   383,   383,   111,   166,   383,   383,   464,
     280,   522,   188,   283,   459,   173,   383,   301,   383,   108,
     260,    35,   189,   460,    92,   374,   109,   372,    78,   375,
      93,   119,   373,   202,   203,   523,   275,   276,   222,   223,
     224,   225,   111,   158,    52,    53,   159,   209,   156,   164,
     512,   515,    35,   384,   112,   613,   113,   114,   115,   116,
     111,   511,   386,   210,   117,   614,    83,    84,   302,   226,
     227,   118,    72,   220,    77,   149,   242,   119,   221,   245,
     164,   215,   216,   177,   111,   158,    52,    53,   159,   248,
      79,   112,   342,   113,   114,   115,   116,    76,   376,   379,
      80,   117,   306,   380,   217,   218,   219,    81,   118,   112,
     326,   113,   114,   115,   116,   158,    52,    53,   159,   117,
     174,   553,   377,    82,   175,   176,   118,   378,   381,   158,
      52,    53,   159,   112,   303,   113,   114,   115,   116,    85,
     111,    86,   504,   430,   119,   250,   417,   177,   246,    87,
     118,   176,   247,   176,   513,   184,   185,   513,   311,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   119,   340,   426,   428,   502,    88,   345,
     176,   176,   503,   349,   310,   114,    89,   601,   602,   119,
     444,   113,   114,   115,   116,   449,   353,   354,    94,   117,
     358,   111,   231,   119,   507,   119,   118,   431,   433,   508,
      90,   570,   160,   575,    91,   160,   571,    95,   576,   158,
      52,    53,   159,   388,   389,   436,   437,   119,   606,   119,
     119,   119,   509,   607,   610,    96,   119,   119,    97,   611,
     103,   170,   516,   517,   521,    98,   263,   463,   318,   319,
     112,   104,   113,   114,   115,   116,    99,   501,   177,   193,
     432,   194,   506,   195,   100,   101,   284,   118,   102,   105,
     119,   106,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,   407,   295,   296,   196,   107,   197,   110,   198,
     418,   132,   420,   421,   422,   133,   424,   134,   427,   429,
     135,   513,   513,   136,   158,    52,    53,   159,   243,   137,
     440,   158,    52,    53,   159,   138,   344,   542,   158,    52,
      53,   159,   139,   140,   547,   312,   313,   314,   315,   316,
     317,   596,   141,   142,   143,   144,   145,   262,   365,   471,
     177,   146,   419,   369,   370,   468,   423,   147,   177,   150,
     177,   177,   177,   277,   177,   151,   177,   177,   157,   574,
     489,   161,   489,   162,   579,   167,   168,   305,   177,   307,
     490,   169,   492,   171,   172,   179,   160,   497,   180,   181,
     182,   160,   183,   158,    52,    53,   159,   186,   199,   187,
     192,   328,   190,   329,   330,   331,   600,   191,   204,   205,
     336,   337,   520,   177,   320,   321,   322,   323,   324,   325,
     206,   208,   212,   213,   119,   214,   228,   230,   229,   232,
     233,   234,   148,   119,   235,   119,   425,   119,   177,   119,
     177,   119,   236,   160,   367,   177,   119,   237,   160,   238,
     551,   239,   240,   251,   445,   261,   244,   249,   253,   450,
     255,   254,   149,   256,   257,   258,   264,   525,   119,   119,
     177,   259,   265,   266,   267,   268,   269,   270,   562,   563,
     564,   565,   271,   566,   272,   469,   273,   274,   278,   569,
     279,   281,   282,   343,   299,   300,   477,   346,   479,   480,
     481,   341,   483,   304,   308,   485,   332,   487,   333,   334,
     335,   351,   355,   494,   338,   339,   160,   359,   499,   350,
     352,   160,   356,   357,   176,   360,   361,   362,   363,   364,
     366,   368,   371,   382,   391,   409,   177,   177,   177,   177,
     434,   177,   398,   392,   393,   394,   526,   177,   395,   396,
     397,   399,   400,   401,   402,   403,   404,   405,   582,   584,
     410,   585,   406,   408,   435,   412,   438,   439,   533,   441,
     535,   443,   442,   446,   538,   539,   448,   447,   452,   383,
     455,   461,   545,   464,   495,   462,   467,   550,   476,   496,
     500,   470,   472,   514,   473,   475,   478,   484,   557,   486,
     482,   488,   505,   491,   510,   493,   527,   528,   532,   529,
     498,   530,   531,   543,   567,   534,   536,   541,   540,   537,
     572,   546,   544,   548,   552,   577,   549,   554,   555,   556,
     558,   560,   518,   519,   568,   573,   561,   559,   580,   578,
     586,   587,   588,   589,   590,   581,   591,   592,   593,   594,
     595,   597,   599,   603,   598,   604,   605,   616,   609,   619,
     524,   411,   621,   622,   211,   451,   252,   327,   474,   165,
       0,     0,     0,     0,   608,     0,     0,     0,   612,     0,
       0,     0,     0,   615,     0,   617,     0,   618,     0,   620,
       0,     0,     0,     0,     0,     0,     0,     0,   152,   623,
     624,     3,     4,     5,     6,     7,     8,     9,     0,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,     0,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      27,    28,    29,    30,    31,    32,    33,    34,     0,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,     0,    50,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   153,     0,     0,   154,
       0,     0,     0,     0,    -7,     1,     0,     2,     0,   583,
       3,     4,     5,     6,     7,     8,     9,     0,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,     0,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,     0,    50,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    51,    52,    53,    54,   152,
       0,     0,     3,     4,     5,     6,     7,     8,     9,     0,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,     0,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    27,    28,    29,    30,    31,    32,    33,    34,     0,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,     0,    50,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   153,     0,     0,
     154,   152,   155,     0,     3,     4,     5,     6,     7,     8,
       9,     0,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     0,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,     0,    50,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   413,
     152,     0,   154,     3,     4,     5,     6,     7,     8,     9,
       0,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,     0,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    27,    28,    29,    30,    31,    32,    33,    34,
       0,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,     0,    50,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   153,     0,
       0,   154
};

static const grib_yytype_int16 grib_yycheck[] =
{
       0,   145,   212,   386,   112,   387,    13,     0,    66,   299,
     300,     0,     3,    47,    47,    30,    76,    47,    47,    68,
     195,    70,    81,   198,    85,    85,    47,    42,    47,    82,
     174,    53,    91,    94,    81,    89,    89,    85,    81,    93,
      87,    34,    90,   103,   104,    94,   190,   191,    71,    72,
      73,    74,    30,    81,    82,    83,    84,   117,    58,    81,
      94,    94,    53,    93,    79,    94,    81,    82,    83,    84,
      30,   453,    93,   118,    89,    94,    81,    82,    93,   102,
     103,    96,    71,    96,    87,    92,   146,    80,   101,   149,
      81,    75,    76,    86,    30,    81,    82,    83,    84,   150,
      81,    79,   246,    81,    82,    83,    84,    89,    94,    89,
      89,    89,    90,    93,    98,    99,   100,    87,    96,    79,
     228,    81,    82,    83,    84,    81,    82,    83,    84,    89,
      87,   513,    85,    87,    91,    92,    96,    90,    94,    81,
      82,    83,    84,    79,   204,    81,    82,    83,    84,    89,
      30,    81,    94,    89,   147,   155,    87,   150,    87,    87,
      96,    92,    91,    92,   454,    91,    92,   457,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   176,   244,    87,    87,    85,    87,   249,
      92,    92,    90,   253,    81,    82,    87,   580,   581,   192,
     375,    81,    82,    83,    84,   380,    81,    82,    81,    89,
     261,    30,   134,   206,    85,   208,    96,   361,   362,    90,
      87,    85,   280,    85,    87,   283,    90,    81,    90,    81,
      82,    83,    84,    83,    84,    91,    92,   230,    85,   232,
     233,   234,    94,    90,    85,    81,   239,   240,    81,    90,
      89,    80,    83,    84,   464,    81,   178,   401,   220,   221,
      79,    89,    81,    82,    83,    84,    81,   442,   261,    89,
      89,    91,   447,    93,    81,    81,    26,    96,    81,    81,
     273,    82,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,   343,    43,    44,    89,    82,    91,    81,    93,
     351,    86,   353,   354,   355,    81,   357,    81,   359,   360,
      81,   601,   602,    81,    81,    82,    83,    84,   147,    81,
     371,    81,    82,    83,    84,    81,   248,    94,    81,    82,
      83,    84,    87,    87,    94,   214,   215,   216,   217,   218,
     219,    94,    87,    87,    81,    81,    81,   176,   270,   409,
     343,    89,   352,   275,   276,   406,   356,    89,   351,    81,
     353,   354,   355,   192,   357,     0,   359,   360,    86,   544,
     430,    92,   432,    85,   549,    83,    82,   206,   371,   208,
     431,    82,   433,    83,    83,    83,   444,   438,    83,    83,
      83,   449,    83,    81,    82,    83,    84,    83,    95,    91,
      89,   230,    92,   232,   233,   234,    94,    92,    92,    82,
     239,   240,   463,   406,   222,   223,   224,   225,   226,   227,
      89,    89,    85,    80,   417,    97,    77,    89,    78,    89,
      89,    89,    13,   426,    83,   428,   358,   430,   431,   432,
     433,   434,    83,   501,   273,   438,   439,    83,   506,    83,
     510,    89,    89,    93,   376,    81,    89,    89,    89,   381,
      88,    90,    92,    90,    88,    88,    88,   467,   461,   462,
     463,    90,    88,    88,    88,    88,    81,    81,   529,   530,
     531,   532,    88,   534,    81,   407,    89,    81,    81,   540,
      81,    81,    81,    81,    90,    90,   418,    81,   420,   421,
     422,    86,   424,    90,    90,   427,    88,   429,    88,    88,
      88,    81,    81,   435,    90,    90,   574,    81,   440,    93,
      93,   579,    93,    88,    92,    81,    81,    81,    81,    92,
      81,    81,    90,    85,    82,    92,   529,   530,   531,   532,
      89,   534,    81,    90,    90,    90,   468,   540,    90,    90,
      90,    81,    81,    81,    90,    90,    93,    90,   558,   559,
      94,   561,    88,    90,    81,    90,    90,    89,   490,    82,
     492,    81,    93,    82,   496,   497,    81,    93,    81,    47,
      86,    89,   504,    68,    81,    89,    93,   509,   417,    81,
      85,    93,    93,    86,    94,    94,    94,   426,   520,   428,
      94,   430,    85,   432,    92,   434,    86,     4,    90,    88,
     439,    88,    88,    82,   536,    90,    90,    81,    90,    92,
     542,    81,    93,    82,     4,   547,    93,     4,    90,    90,
      95,    94,   461,   462,    81,    85,    93,    95,    93,    85,
     562,   563,   564,   565,   566,    93,   568,   569,    81,   571,
      81,    81,    81,    94,   576,    94,    90,    81,    90,    81,
     465,   348,    90,    90,   118,   382,   162,   229,   415,    71,
      -1,    -1,    -1,    -1,   596,    -1,    -1,    -1,   600,    -1,
      -1,    -1,    -1,   605,    -1,   607,    -1,   609,    -1,   611,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,   621,
     622,     6,     7,     8,     9,    10,    11,    12,    -1,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    -1,    31,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    -1,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    -1,    69,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    81,    -1,    -1,    84,
      -1,    -1,    -1,    -1,     0,     1,    -1,     3,    -1,    94,
       6,     7,     8,     9,    10,    11,    12,    -1,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    -1,    31,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    -1,    69,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    81,    82,    83,    84,     3,
      -1,    -1,     6,     7,     8,     9,    10,    11,    12,    -1,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    -1,    31,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    -1,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    -1,    69,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    81,    -1,    -1,
      84,     3,    86,    -1,     6,     7,     8,     9,    10,    11,
      12,    -1,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    -1,    31,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    -1,    69,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    81,
       3,    -1,    84,     6,     7,     8,     9,    10,    11,    12,
      -1,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    -1,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    46,    47,    48,    49,    50,    51,    52,
      -1,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    -1,    69,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    81,    -1,
      -1,    84
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const grib_yytype_uint8 grib_yystos[] =
{
       0,     1,     3,     6,     7,     8,     9,    10,    11,    12,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    31,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      69,    81,    82,    83,    84,   105,   106,   108,   109,   114,
     115,   116,   123,   124,   125,   126,   127,   130,   131,   132,
     133,   145,   146,   148,   149,   150,    89,    87,    81,    81,
      89,    87,    87,    81,    82,    89,    81,    87,    87,    87,
      87,    87,    81,    87,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    89,    89,    81,    82,    82,    82,    89,
      81,    30,    79,    81,    82,    83,    84,    89,    96,   106,
     111,   112,   113,   136,   137,   138,   139,   140,   141,   142,
     143,   144,    86,    81,    81,    81,    81,    81,    81,    87,
      87,    87,    87,    81,    81,    81,    89,    89,    13,    92,
      81,     0,     3,    81,    84,    86,   108,    86,    81,    84,
     131,    92,    85,     3,    81,   150,   144,    83,    82,    82,
     111,    83,    83,   144,    87,    91,    92,   106,   119,    83,
      83,    83,    83,    83,    91,    92,    83,    91,    81,    91,
      92,    92,    89,    89,    91,    93,    89,    91,    93,    95,
     106,   120,   144,   144,    92,    82,    89,   141,    89,   144,
     136,   137,    85,    80,    97,    75,    76,    98,    99,   100,
      96,   101,    71,    72,    73,    74,   102,   103,    77,    78,
      89,   120,    89,    89,    89,    83,    83,    83,    83,    89,
      89,   113,   144,   111,    89,   144,    87,    91,   119,    89,
     108,    93,   132,    89,    90,    88,    90,    88,    88,    90,
     113,    81,   111,   120,    88,    88,    88,    88,    88,    81,
      81,    88,    81,    89,    81,   113,   113,   111,    81,    81,
     127,    81,    81,   127,    26,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    43,    44,   121,   122,    90,
      90,    42,    93,   144,    90,   111,    90,   111,    90,   112,
      81,   136,   138,   138,   138,   138,   138,   138,   139,   139,
     140,   140,   140,   140,   140,   140,   141,   142,   111,   111,
     111,   111,    88,    88,    88,    88,   111,   111,    90,    90,
     144,    86,   113,    81,   120,   144,    81,   134,   135,   144,
      93,    81,    93,    81,    82,    81,    93,    88,   119,    81,
      81,    81,    81,    81,    92,   120,    81,   111,    81,   120,
     120,    90,    85,    90,    89,    93,    94,    85,    90,    89,
      93,    94,    85,    47,    93,   117,    93,   117,    83,    84,
     107,    82,    90,    90,    90,    90,    90,    90,    81,    81,
      81,    81,    90,    90,    93,    90,    88,   119,    90,    92,
      94,   134,    90,    81,   108,   146,   147,    87,   119,   108,
     119,   119,   119,   108,   119,   120,    87,   119,    87,   119,
      89,   113,    89,   113,    89,    81,    91,    92,    90,    89,
     119,    82,    93,    81,   127,   120,    82,    93,    81,   127,
     120,   122,    81,   117,   118,    86,   110,   118,   110,    85,
      94,    89,    89,   113,    68,   128,   129,    93,   119,   120,
      93,   144,    93,    94,   147,    94,   111,   120,    94,   120,
     120,   120,    94,   120,   111,   120,   111,   120,   111,   144,
     119,   111,   119,   111,   120,    81,    81,   119,   111,   120,
      85,   127,    85,    90,    94,    85,   127,    85,    90,    94,
      92,   110,    94,   117,    86,    94,    83,    84,   111,   111,
     119,   112,    70,    94,   129,   108,   120,    86,     4,    88,
      88,    88,    90,   120,    90,   120,    90,    92,   120,   120,
      90,    81,    94,    82,    93,   120,    81,    94,    82,    93,
     120,   144,     4,   110,     4,    90,    90,   120,    95,    95,
      94,    93,   119,   119,   119,   119,   119,   120,    81,   119,
      85,    90,   120,    85,   127,    85,    90,   120,    85,   127,
      93,    93,   108,    94,   108,   108,   120,   120,   120,   120,
     120,   120,   120,    81,   120,    81,    94,    81,   120,    81,
      94,   118,   118,    94,    94,    90,    85,    90,   120,    90,
      85,    90,   120,    94,    94,   120,    81,   120,   120,    81,
     120,    90,    90,   120,   120
};

#define grib_yyerrok		(grib_yyerrstatus = 0)
#define grib_yyclearin	(grib_yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto grib_yyacceptlab
#define YYABORT		goto grib_yyabortlab
#define YYERROR		goto grib_yyerrorlab


/* Like YYERROR except do call grib_yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto grib_yyerrlab

#define YYRECOVERING()  (!!grib_yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (grib_yychar == YYEMPTY && grib_yylen == 1)				\
    {								\
      grib_yychar = (Token);						\
      grib_yylval = (Value);						\
      grib_yytoken = YYTRANSLATE (grib_yychar);				\
      YYPOPSTACK (1);						\
      goto grib_yybackup;						\
    }								\
  else								\
    {								\
      grib_yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `grib_yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX grib_yylex (YYLEX_PARAM)
#else
# define YYLEX grib_yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (grib_yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (grib_yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      grib_yy_symbol_print (stderr,						  \
		  Type, Value); \
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
grib_yy_symbol_value_print (FILE *grib_yyoutput, int grib_yytype, YYSTYPE const * const grib_yyvaluep)
#else
static void
grib_yy_symbol_value_print (grib_yyoutput, grib_yytype, grib_yyvaluep)
    FILE *grib_yyoutput;
    int grib_yytype;
    YYSTYPE const * const grib_yyvaluep;
#endif
{
  if (!grib_yyvaluep)
    return;
# ifdef YYPRINT
  if (grib_yytype < YYNTOKENS)
    YYPRINT (grib_yyoutput, grib_yytoknum[grib_yytype], *grib_yyvaluep);
# else
  YYUSE (grib_yyoutput);
# endif
  switch (grib_yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
grib_yy_symbol_print (FILE *grib_yyoutput, int grib_yytype, YYSTYPE const * const grib_yyvaluep)
#else
static void
grib_yy_symbol_print (grib_yyoutput, grib_yytype, grib_yyvaluep)
    FILE *grib_yyoutput;
    int grib_yytype;
    YYSTYPE const * const grib_yyvaluep;
#endif
{
  if (grib_yytype < YYNTOKENS)
    YYFPRINTF (grib_yyoutput, "token %s (", grib_yytname[grib_yytype]);
  else
    YYFPRINTF (grib_yyoutput, "nterm %s (", grib_yytname[grib_yytype]);

  grib_yy_symbol_value_print (grib_yyoutput, grib_yytype, grib_yyvaluep);
  YYFPRINTF (grib_yyoutput, ")");
}

/*------------------------------------------------------------------.
| grib_yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
grib_yy_stack_print (grib_yytype_int16 *bottom, grib_yytype_int16 *top)
#else
static void
grib_yy_stack_print (bottom, top)
    grib_yytype_int16 *bottom;
    grib_yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (grib_yydebug)							\
    grib_yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
grib_yy_reduce_print (YYSTYPE *grib_yyvsp, int grib_yyrule)
#else
static void
grib_yy_reduce_print (grib_yyvsp, grib_yyrule)
    YYSTYPE *grib_yyvsp;
    int grib_yyrule;
#endif
{
  int grib_yynrhs = grib_yyr2[grib_yyrule];
  int grib_yyi;
  unsigned long int grib_yylno = grib_yyrline[grib_yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     grib_yyrule - 1, grib_yylno);
  /* The symbols being reduced.  */
  for (grib_yyi = 0; grib_yyi < grib_yynrhs; grib_yyi++)
    {
      fprintf (stderr, "   $%d = ", grib_yyi + 1);
      grib_yy_symbol_print (stderr, grib_yyrhs[grib_yyprhs[grib_yyrule] + grib_yyi],
		       &(grib_yyvsp[(grib_yyi + 1) - (grib_yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (grib_yydebug)				\
    grib_yy_reduce_print (grib_yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int grib_yydebug;
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

# ifndef grib_yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define grib_yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
grib_yystrlen (const char *grib_yystr)
#else
static YYSIZE_T
grib_yystrlen (grib_yystr)
    const char *grib_yystr;
#endif
{
  YYSIZE_T grib_yylen;
  for (grib_yylen = 0; grib_yystr[grib_yylen]; grib_yylen++)
    continue;
  return grib_yylen;
}
#  endif
# endif

# ifndef grib_yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define grib_yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
grib_yystpcpy (char *grib_yydest, const char *grib_yysrc)
#else
static char *
grib_yystpcpy (grib_yydest, grib_yysrc)
    char *grib_yydest;
    const char *grib_yysrc;
#endif
{
  char *grib_yyd = grib_yydest;
  const char *grib_yys = grib_yysrc;

  while ((*grib_yyd++ = *grib_yys++) != '\0')
    continue;

  return grib_yyd - 1;
}
#  endif
# endif

# ifndef grib_yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for grib_yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from grib_yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
grib_yytnamerr (char *grib_yyres, const char *grib_yystr)
{
  if (*grib_yystr == '"')
    {
      YYSIZE_T grib_yyn = 0;
      char const *grib_yyp = grib_yystr;

      for (;;)
	switch (*++grib_yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++grib_yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (grib_yyres)
	      grib_yyres[grib_yyn] = *grib_yyp;
	    grib_yyn++;
	    break;

	  case '"':
	    if (grib_yyres)
	      grib_yyres[grib_yyn] = '\0';
	    return grib_yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! grib_yyres)
    return grib_yystrlen (grib_yystr);

  return grib_yystpcpy (grib_yyres, grib_yystr) - grib_yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
grib_yysyntax_error (char *grib_yyresult, int grib_yystate, int grib_yychar)
{
  int grib_yyn = grib_yypact[grib_yystate];

  if (! (YYPACT_NINF < grib_yyn && grib_yyn <= YYLAST))
    return 0;
  else
    {
      int grib_yytype = YYTRANSLATE (grib_yychar);
      YYSIZE_T grib_yysize0 = grib_yytnamerr (0, grib_yytname[grib_yytype]);
      YYSIZE_T grib_yysize = grib_yysize0;
      YYSIZE_T grib_yysize1;
      int grib_yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *grib_yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int grib_yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *grib_yyfmt;
      char const *grib_yyf;
      static char const grib_yyunexpected[] = "syntax error, unexpected %s";
      static char const grib_yyexpecting[] = ", expecting %s";
      static char const grib_yyor[] = " or %s";
      char grib_yyformat[sizeof grib_yyunexpected
		    + sizeof grib_yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof grib_yyor - 1))];
      char const *grib_yyprefix = grib_yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int grib_yyxbegin = grib_yyn < 0 ? -grib_yyn : 0;

      /* Stay within bounds of both grib_yycheck and grib_yytname.  */
      int grib_yychecklim = YYLAST - grib_yyn + 1;
      int grib_yyxend = grib_yychecklim < YYNTOKENS ? grib_yychecklim : YYNTOKENS;
      int grib_yycount = 1;

      grib_yyarg[0] = grib_yytname[grib_yytype];
      grib_yyfmt = grib_yystpcpy (grib_yyformat, grib_yyunexpected);

      for (grib_yyx = grib_yyxbegin; grib_yyx < grib_yyxend; ++grib_yyx)
	if (grib_yycheck[grib_yyx + grib_yyn] == grib_yyx && grib_yyx != YYTERROR)
	  {
	    if (grib_yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		grib_yycount = 1;
		grib_yysize = grib_yysize0;
		grib_yyformat[sizeof grib_yyunexpected - 1] = '\0';
		break;
	      }
	    grib_yyarg[grib_yycount++] = grib_yytname[grib_yyx];
	    grib_yysize1 = grib_yysize + grib_yytnamerr (0, grib_yytname[grib_yyx]);
	    grib_yysize_overflow |= (grib_yysize1 < grib_yysize);
	    grib_yysize = grib_yysize1;
	    grib_yyfmt = grib_yystpcpy (grib_yyfmt, grib_yyprefix);
	    grib_yyprefix = grib_yyor;
	  }

      grib_yyf = YY_(grib_yyformat);
      grib_yysize1 = grib_yysize + grib_yystrlen (grib_yyf);
      grib_yysize_overflow |= (grib_yysize1 < grib_yysize);
      grib_yysize = grib_yysize1;

      if (grib_yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (grib_yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *grib_yyp = grib_yyresult;
	  int grib_yyi = 0;
	  while ((*grib_yyp = *grib_yyf) != '\0')
	    {
	      if (*grib_yyp == '%' && grib_yyf[1] == 's' && grib_yyi < grib_yycount)
		{
		  grib_yyp += grib_yytnamerr (grib_yyp, grib_yyarg[grib_yyi++]);
		  grib_yyf += 2;
		}
	      else
		{
		  grib_yyp++;
		  grib_yyf++;
		}
	    }
	}
      return grib_yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
grib_yydestruct (const char *grib_yymsg, int grib_yytype, YYSTYPE *grib_yyvaluep)
#else
static void
grib_yydestruct (grib_yymsg, grib_yytype, grib_yyvaluep)
    const char *grib_yymsg;
    int grib_yytype;
    YYSTYPE *grib_yyvaluep;
#endif
{
  YYUSE (grib_yyvaluep);

  if (!grib_yymsg)
    grib_yymsg = "Deleting";
  YY_SYMBOL_PRINT (grib_yymsg, grib_yytype, grib_yyvaluep, grib_yylocationp);

  switch (grib_yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int grib_yyparse (void *YYPARSE_PARAM);
#else
int grib_yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int grib_yyparse (void);
#else
int grib_yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int grib_yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE grib_yylval;

/* Number of syntax errors so far.  */
int grib_yynerrs;



/*----------.
| grib_yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
grib_yyparse (void *YYPARSE_PARAM)
#else
int
grib_yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
grib_yyparse (void)
#else
int
grib_yyparse ()

#endif
#endif
{
  
  int grib_yystate;
  int grib_yyn;
  int grib_yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int grib_yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int grib_yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char grib_yymsgbuf[128];
  char *grib_yymsg = grib_yymsgbuf;
  YYSIZE_T grib_yymsg_alloc = sizeof grib_yymsgbuf;
#endif

  /* Three stacks and their tools:
     `grib_yyss': related to states,
     `grib_yyvs': related to semantic values,
     `grib_yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow grib_yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  grib_yytype_int16 grib_yyssa[YYINITDEPTH];
  grib_yytype_int16 *grib_yyss = grib_yyssa;
  grib_yytype_int16 *grib_yyssp;

  /* The semantic value stack.  */
  YYSTYPE grib_yyvsa[YYINITDEPTH];
  YYSTYPE *grib_yyvs = grib_yyvsa;
  YYSTYPE *grib_yyvsp;



#define YYPOPSTACK(N)   (grib_yyvsp -= (N), grib_yyssp -= (N))

  YYSIZE_T grib_yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE grib_yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int grib_yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  grib_yystate = 0;
  grib_yyerrstatus = 0;
  grib_yynerrs = 0;
  grib_yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  grib_yyssp = grib_yyss;
  grib_yyvsp = grib_yyvs;

  goto grib_yysetstate;

/*------------------------------------------------------------.
| grib_yynewstate -- Push a new state, which is found in grib_yystate.  |
`------------------------------------------------------------*/
 grib_yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  grib_yyssp++;

 grib_yysetstate:
  *grib_yyssp = grib_yystate;

  if (grib_yyss + grib_yystacksize - 1 <= grib_yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T grib_yysize = grib_yyssp - grib_yyss + 1;

#ifdef grib_yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *grib_yyvs1 = grib_yyvs;
	grib_yytype_int16 *grib_yyss1 = grib_yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if grib_yyoverflow is a macro.  */
	grib_yyoverflow (YY_("memory exhausted"),
		    &grib_yyss1, grib_yysize * sizeof (*grib_yyssp),
		    &grib_yyvs1, grib_yysize * sizeof (*grib_yyvsp),

		    &grib_yystacksize);

	grib_yyss = grib_yyss1;
	grib_yyvs = grib_yyvs1;
      }
#else /* no grib_yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto grib_yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= grib_yystacksize)
	goto grib_yyexhaustedlab;
      grib_yystacksize *= 2;
      if (YYMAXDEPTH < grib_yystacksize)
	grib_yystacksize = YYMAXDEPTH;

      {
	grib_yytype_int16 *grib_yyss1 = grib_yyss;
	union grib_yyalloc *grib_yyptr =
	  (union grib_yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (grib_yystacksize));
	if (! grib_yyptr)
	  goto grib_yyexhaustedlab;
	YYSTACK_RELOCATE (grib_yyss);
	YYSTACK_RELOCATE (grib_yyvs);

#  undef YYSTACK_RELOCATE
	if (grib_yyss1 != grib_yyssa)
	  YYSTACK_FREE (grib_yyss1);
      }
# endif
#endif /* no grib_yyoverflow */

      grib_yyssp = grib_yyss + grib_yysize - 1;
      grib_yyvsp = grib_yyvs + grib_yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) grib_yystacksize));

      if (grib_yyss + grib_yystacksize - 1 <= grib_yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", grib_yystate));

  goto grib_yybackup;

/*-----------.
| grib_yybackup.  |
`-----------*/
grib_yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  grib_yyn = grib_yypact[grib_yystate];
  if (grib_yyn == YYPACT_NINF)
    goto grib_yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (grib_yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      grib_yychar = YYLEX;
    }

  if (grib_yychar <= YYEOF)
    {
      grib_yychar = grib_yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      grib_yytoken = YYTRANSLATE (grib_yychar);
      YY_SYMBOL_PRINT ("Next token is", grib_yytoken, &grib_yylval, &grib_yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  grib_yyn += grib_yytoken;
  if (grib_yyn < 0 || YYLAST < grib_yyn || grib_yycheck[grib_yyn] != grib_yytoken)
    goto grib_yydefault;
  grib_yyn = grib_yytable[grib_yyn];
  if (grib_yyn <= 0)
    {
      if (grib_yyn == 0 || grib_yyn == YYTABLE_NINF)
	goto grib_yyerrlab;
      grib_yyn = -grib_yyn;
      goto grib_yyreduce;
    }

  if (grib_yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (grib_yyerrstatus)
    grib_yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", grib_yytoken, &grib_yylval, &grib_yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (grib_yychar != YYEOF)
    grib_yychar = YYEMPTY;

  grib_yystate = grib_yyn;
  *++grib_yyvsp = grib_yylval;

  goto grib_yynewstate;


/*-----------------------------------------------------------.
| grib_yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
grib_yydefault:
  grib_yyn = grib_yydefact[grib_yystate];
  if (grib_yyn == 0)
    goto grib_yyerrlab;
  goto grib_yyreduce;


/*-----------------------------.
| grib_yyreduce -- Do a reduction.  |
`-----------------------------*/
grib_yyreduce:
  /* grib_yyn is the number of a rule to reduce with.  */
  grib_yylen = grib_yyr2[grib_yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  grib_yyval = grib_yyvsp[1-grib_yylen];


  YY_REDUCE_PRINT (grib_yyn);
  switch (grib_yyn)
    {
        case 2:
#line 239 "griby.y"
    { grib_parser_all_actions = 0;grib_parser_concept=0; grib_parser_rules=0; }
    break;

  case 3:
#line 240 "griby.y"
    { grib_parser_concept     = reverse((grib_yyvsp[(1) - (1)].concept_value)); }
    break;

  case 4:
#line 241 "griby.y"
    { grib_parser_all_actions = (grib_yyvsp[(1) - (1)].act); }
    break;

  case 5:
#line 242 "griby.y"
    { grib_parser_rules       = (grib_yyvsp[(1) - (1)].rules); }
    break;

  case 6:
#line 244 "griby.y"
    { grib_parser_all_actions = 0; grib_parser_concept=0; grib_parser_rules=0; }
    break;

  case 8:
#line 250 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,0,(grib_yyvsp[(1) - (1)].dval));}
    break;

  case 9:
#line 251 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,(grib_yyvsp[(1) - (3)].dvalue),(grib_yyvsp[(3) - (3)].dval));}
    break;

  case 10:
#line 252 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,0,(grib_yyvsp[(1) - (1)].lval));}
    break;

  case 11:
#line 253 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,(grib_yyvsp[(1) - (3)].dvalue),(grib_yyvsp[(3) - (3)].lval));}
    break;

  case 13:
#line 256 "griby.y"
    { (grib_yyvsp[(1) - (2)].act)->next = (grib_yyvsp[(2) - (2)].act); (grib_yyval.act) = (grib_yyvsp[(1) - (2)].act); }
    break;

  case 14:
#line 257 "griby.y"
    { (grib_yyvsp[(1) - (3)].act)->next = (grib_yyvsp[(3) - (3)].act); (grib_yyval.act) = (grib_yyvsp[(1) - (3)].act); }
    break;

  case 15:
#line 258 "griby.y"
    {  (grib_yyval.act) = (grib_yyvsp[(1) - (2)].act);}
    break;

  case 26:
#line 276 "griby.y"
    { (grib_yyval.explist) = 0; }
    break;

  case 29:
#line 281 "griby.y"
    { (grib_yyvsp[(1) - (3)].explist)->next = (grib_yyvsp[(3) - (3)].explist); (grib_yyval.explist) = (grib_yyvsp[(1) - (3)].explist); }
    break;

  case 30:
#line 284 "griby.y"
    { (grib_yyval.explist) = grib_arguments_new(grib_parser_context,(grib_yyvsp[(1) - (1)].exp)); }
    break;

  case 31:
#line 289 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"unsigned",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL);        free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 32:
#line 292 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"unsigned",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL);        free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 33:
#line 295 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"ascii",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL);  free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 34:
#line 299 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"ascii",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL);  free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 35:
#line 302 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"bytes",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL);      free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 36:
#line 305 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"bytes",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL);      free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 37:
#line 308 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"ksec1expver",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL);  free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 38:
#line 311 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"signed",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL);      free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 39:
#line 314 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"signed",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL);      free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 40:
#line 317 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"codetable",(grib_yyvsp[(3) - (8)].lval), (grib_yyvsp[(6) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL);    free((grib_yyvsp[(5) - (8)].str)); }
    break;

  case 41:
#line 320 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"codetable",(grib_yyvsp[(3) - (10)].lval), (grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL);    free((grib_yyvsp[(5) - (10)].str)); }
    break;

  case 42:
#line 323 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"complex_codetable",(grib_yyvsp[(3) - (8)].lval), (grib_yyvsp[(6) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL);    free((grib_yyvsp[(5) - (8)].str)); }
    break;

  case 43:
#line 326 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"complex_codetable",(grib_yyvsp[(3) - (10)].lval), (grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL);    free((grib_yyvsp[(5) - (10)].str)); }
    break;

  case 44:
#line 329 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"codeflag",(grib_yyvsp[(3) - (8)].lval), (grib_yyvsp[(6) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL);  free((grib_yyvsp[(5) - (8)].str)); }
    break;

  case 45:
#line 332 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (9)].str),"lookup",(grib_yyvsp[(3) - (9)].lval),(grib_yyvsp[(7) - (9)].explist),NULL,(grib_yyvsp[(9) - (9)].lval),NULL); free((grib_yyvsp[(5) - (9)].str)); }
    break;

  case 46:
#line 335 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (7)].str),"bit",0,(grib_yyvsp[(4) - (7)].explist),(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL); free((grib_yyvsp[(2) - (7)].str)); }
    break;

  case 47:
#line 338 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"label",0,NULL,NULL,0,NULL);   free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 48:
#line 341 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"label",0,NULL,NULL,0,NULL);   free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 49:
#line 344 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (4)].str),"ibmfloat",4,NULL,(grib_yyvsp[(3) - (4)].explist),(grib_yyvsp[(4) - (4)].lval),NULL);free((grib_yyvsp[(2) - (4)].str));  }
    break;

  case 50:
#line 347 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(4) - (6)].str),"ibmfloat",4,NULL,(grib_yyvsp[(5) - (6)].explist),(grib_yyvsp[(6) - (6)].lval),(grib_yyvsp[(2) - (6)].str));free((grib_yyvsp[(4) - (6)].str)); free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 51:
#line 350 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (7)].str),"ibmfloat",4,(grib_yyvsp[(4) - (7)].explist),(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL);free((grib_yyvsp[(2) - (7)].str));  }
    break;

  case 52:
#line 353 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"position",0,NULL,NULL,0,NULL);     free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 53:
#line 356 "griby.y"
    { (grib_yyval.act) = grib_action_create_variable(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"constant",0,(grib_yyvsp[(4) - (5)].explist),NULL,(grib_yyvsp[(5) - (5)].lval),NULL);free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 54:
#line 359 "griby.y"
    { (grib_yyval.act) = grib_action_create_variable(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"transient",0,(grib_yyvsp[(4) - (5)].explist),(grib_yyvsp[(4) - (5)].explist),(grib_yyvsp[(5) - (5)].lval),NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 55:
#line 362 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (4)].str),"ieeefloat",4,NULL,(grib_yyvsp[(3) - (4)].explist),(grib_yyvsp[(4) - (4)].lval),NULL);   free((grib_yyvsp[(2) - (4)].str));  }
    break;

  case 56:
#line 365 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(4) - (6)].str),"ieeefloat",4,NULL,(grib_yyvsp[(5) - (6)].explist),(grib_yyvsp[(6) - (6)].lval),(grib_yyvsp[(2) - (6)].str));  free((grib_yyvsp[(4) - (6)].str));free((grib_yyvsp[(2) - (6)].str));}
    break;

  case 57:
#line 368 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (7)].str),"ieeefloat",4,(grib_yyvsp[(4) - (7)].explist),(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL);free((grib_yyvsp[(2) - (7)].str));  }
    break;

  case 58:
#line 371 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"g1_half_byte_codeflag",0,NULL,NULL,0,NULL);free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 59:
#line 374 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (5)].str),"section_length",(grib_yyvsp[(3) - (5)].lval),NULL,NULL,0,NULL);free((grib_yyvsp[(5) - (5)].str));  }
    break;

  case 60:
#line 377 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"g1_message_length",(grib_yyvsp[(3) - (8)].lval),(grib_yyvsp[(7) - (8)].explist),NULL,0,NULL);free((grib_yyvsp[(5) - (8)].str));  }
    break;

  case 61:
#line 380 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"g1_section4_length",(grib_yyvsp[(3) - (8)].lval),(grib_yyvsp[(7) - (8)].explist),NULL,0,NULL);free((grib_yyvsp[(5) - (8)].str));  }
    break;

  case 62:
#line 383 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (3)].str),"ksec",0,(grib_yyvsp[(3) - (3)].explist),NULL,0,NULL);free((grib_yyvsp[(2) - (3)].str)); }
    break;

  case 63:
#line 386 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"pad",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 64:
#line 389 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"padto",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 65:
#line 392 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"padtoeven",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 66:
#line 395 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"padtomultiple",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 67:
#line 398 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (3)].str),"section_padding",0,0,0,(grib_yyvsp[(3) - (3)].lval),NULL);   free((grib_yyvsp[(2) - (3)].str));  }
    break;

  case 68:
#line 401 "griby.y"
    { (grib_yyval.act) = grib_action_create_template(grib_parser_context,0,(grib_yyvsp[(2) - (3)].str),(grib_yyvsp[(3) - (3)].str)); free((grib_yyvsp[(2) - (3)].str)); free((grib_yyvsp[(3) - (3)].str));}
    break;

  case 69:
#line 403 "griby.y"
    { (grib_yyval.act) = grib_action_create_template(grib_parser_context,1,(grib_yyvsp[(2) - (3)].str),(grib_yyvsp[(3) - (3)].str)); free((grib_yyvsp[(2) - (3)].str)); free((grib_yyvsp[(3) - (3)].str));}
    break;

  case 70:
#line 406 "griby.y"
    { (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(2) - (5)].str),(grib_yyvsp[(4) - (5)].str),NULL,(grib_yyvsp[(5) - (5)].lval),0);  free((grib_yyvsp[(2) - (5)].str)); free((grib_yyvsp[(4) - (5)].str)); }
    break;

  case 71:
#line 409 "griby.y"
    { (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(5) - (8)].str),(grib_yyvsp[(7) - (8)].str),NULL,(grib_yyvsp[(8) - (8)].lval),(grib_yyvsp[(3) - (8)].lval));  free((grib_yyvsp[(5) - (8)].str)); free((grib_yyvsp[(7) - (8)].str)); }
    break;

  case 72:
#line 412 "griby.y"
    { (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(2) - (2)].str),NULL,NULL,0,0);  free((grib_yyvsp[(2) - (2)].str)); }
    break;

  case 73:
#line 415 "griby.y"
    {
         (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(4) - (7)].str),(grib_yyvsp[(6) - (7)].str),(grib_yyvsp[(2) - (7)].str),(grib_yyvsp[(7) - (7)].lval),0);  free((grib_yyvsp[(2) - (7)].str)); free((grib_yyvsp[(4) - (7)].str)); free((grib_yyvsp[(6) - (7)].str));
    }
    break;

  case 74:
#line 419 "griby.y"
    {
		(grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(7) - (10)].str),(grib_yyvsp[(9) - (10)].str),(grib_yyvsp[(5) - (10)].str),(grib_yyvsp[(10) - (10)].lval),(grib_yyvsp[(3) - (10)].lval));  free((grib_yyvsp[(5) - (10)].str)); free((grib_yyvsp[(7) - (10)].str)); free((grib_yyvsp[(9) - (10)].str));
	}
    break;

  case 75:
#line 423 "griby.y"
    {
         (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(4) - (4)].str),NULL,(grib_yyvsp[(2) - (4)].str),0,0);  free((grib_yyvsp[(2) - (4)].str)); free((grib_yyvsp[(4) - (4)].str)); 
    }
    break;

  case 76:
#line 427 "griby.y"
    { (grib_yyval.act) = grib_action_create_meta(grib_parser_context,(grib_yyvsp[(2) - (8)].str),(grib_yyvsp[(3) - (8)].str),(grib_yyvsp[(5) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL); free((grib_yyvsp[(2) - (8)].str));free((grib_yyvsp[(3) - (8)].str));}
    break;

  case 77:
#line 430 "griby.y"
    { (grib_yyval.act) = grib_action_create_meta(grib_parser_context,(grib_yyvsp[(4) - (10)].str),(grib_yyvsp[(5) - (10)].str),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),(grib_yyvsp[(2) - (10)].str)); free((grib_yyvsp[(4) - (10)].str));free((grib_yyvsp[(5) - (10)].str));free((grib_yyvsp[(2) - (10)].str));}
    break;

  case 78:
#line 433 "griby.y"
    {
      grib_arguments* a = grib_arguments_new(
        grib_parser_context,
        new_accessor_expression(grib_parser_context,(grib_yyvsp[(2) - (5)].str))
        );
      a->next=(grib_yyvsp[(4) - (5)].explist);
      (grib_yyval.act) = grib_action_create_meta(grib_parser_context,
      "ITERATOR","iterator",a,NULL,
      GRIB_ACCESSOR_FLAG_HIDDEN|GRIB_ACCESSOR_FLAG_READ_ONLY,NULL); free((grib_yyvsp[(2) - (5)].str));
    }
    break;

  case 79:
#line 444 "griby.y"
    {
      grib_arguments* a = grib_arguments_new(
        grib_parser_context,
        new_accessor_expression(grib_parser_context,(grib_yyvsp[(2) - (5)].str))
        );
      a->next=(grib_yyvsp[(4) - (5)].explist);
      (grib_yyval.act) = grib_action_create_meta(grib_parser_context,
      "NEAREST","nearest",a,NULL,
      GRIB_ACCESSOR_FLAG_HIDDEN|GRIB_ACCESSOR_FLAG_READ_ONLY,NULL); free((grib_yyvsp[(2) - (5)].str));
    }
    break;

  case 80:
#line 455 "griby.y"
    { (grib_yyval.act) = grib_action_create_put(grib_parser_context,(grib_yyvsp[(2) - (5)].str),(grib_yyvsp[(4) - (5)].explist));free((grib_yyvsp[(2) - (5)].str));}
    break;

  case 81:
#line 458 "griby.y"
    { (grib_yyval.act) = grib_action_create_remove(grib_parser_context,(grib_yyvsp[(2) - (2)].explist));}
    break;

  case 82:
#line 461 "griby.y"
    { (grib_yyval.act) = grib_action_create_assert(grib_parser_context,(grib_yyvsp[(3) - (4)].exp));}
    break;

  case 83:
#line 464 "griby.y"
    { (grib_yyval.act) = grib_action_create_modify(grib_parser_context,(grib_yyvsp[(2) - (3)].str),(grib_yyvsp[(3) - (3)].lval)); free((grib_yyvsp[(2) - (3)].str));}
    break;

  case 84:
#line 466 "griby.y"
    { (grib_yyval.act) = grib_action_create_set_missing(grib_parser_context,(grib_yyvsp[(2) - (4)].str)); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 85:
#line 467 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp)); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 86:
#line 468 "griby.y"
    { (grib_yyval.act) = grib_action_create_set_darray(grib_parser_context,(grib_yyvsp[(2) - (6)].str),(grib_yyvsp[(5) - (6)].dvalue)); free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 87:
#line 470 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(2) - (2)].str),0); free((grib_yyvsp[(2) - (2)].str));}
    break;

  case 88:
#line 471 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",0); }
    break;

  case 89:
#line 472 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(2) - (2)].str),1); free((grib_yyvsp[(2) - (2)].str));}
    break;

  case 90:
#line 473 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",1); }
    break;

  case 91:
#line 475 "griby.y"
    { (grib_yyval.act) = grib_action_create_print(grib_parser_context,(grib_yyvsp[(2) - (2)].str),0); free((grib_yyvsp[(2) - (2)].str)); }
    break;

  case 92:
#line 476 "griby.y"
    { (grib_yyval.act) = grib_action_create_print(grib_parser_context,(grib_yyvsp[(5) - (5)].str),(grib_yyvsp[(3) - (5)].str)); free((grib_yyvsp[(5) - (5)].str)); free((grib_yyvsp[(3) - (5)].str));}
    break;

  case 93:
#line 477 "griby.y"
    { (grib_yyval.act) = grib_action_create_print(grib_parser_context,"",0);  }
    break;

  case 94:
#line 481 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act),0); }
    break;

  case 95:
#line 482 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (11)].exp),(grib_yyvsp[(6) - (11)].act),(grib_yyvsp[(10) - (11)].act)); }
    break;

  case 96:
#line 486 "griby.y"
    { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[(3) - (6)].exp),(grib_yyvsp[(5) - (6)].act),NULL); }
    break;

  case 97:
#line 487 "griby.y"
    { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act),NULL); }
    break;

  case 98:
#line 488 "griby.y"
    { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[(3) - (11)].exp),(grib_yyvsp[(6) - (11)].act),(grib_yyvsp[(10) - (11)].act)); }
    break;

  case 99:
#line 489 "griby.y"
    { (grib_yyval.act) = grib_action_create_late_when(grib_parser_context,(grib_yyvsp[(3) - (6)].exp),(grib_yyvsp[(5) - (6)].act),NULL); }
    break;

  case 100:
#line 490 "griby.y"
    { (grib_yyval.act) = grib_action_create_late_when(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act),NULL); }
    break;

  case 101:
#line 491 "griby.y"
    { (grib_yyval.act) = grib_action_create_late_when(grib_parser_context,(grib_yyvsp[(3) - (11)].exp),(grib_yyvsp[(6) - (11)].act),(grib_yyvsp[(10) - (11)].act)); }
    break;

  case 102:
#line 494 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp)); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 104:
#line 498 "griby.y"
    { (grib_yyvsp[(1) - (3)].act)->next = (grib_yyvsp[(2) - (3)].act); (grib_yyval.act) = (grib_yyvsp[(1) - (3)].act); }
    break;

  case 105:
#line 502 "griby.y"
    { (grib_yyval.explist) = NULL ;}
    break;

  case 106:
#line 503 "griby.y"
    { (grib_yyval.explist) = (grib_yyvsp[(2) - (2)].explist) ;}
    break;

  case 107:
#line 506 "griby.y"
    { (grib_yyval.lval) = 0 ; }
    break;

  case 108:
#line 507 "griby.y"
    { (grib_yyval.lval) = (grib_yyvsp[(2) - (2)].lval); }
    break;

  case 110:
#line 511 "griby.y"
    { (grib_yyval.lval) = (grib_yyvsp[(1) - (3)].lval) | (grib_yyvsp[(3) - (3)].lval); }
    break;

  case 111:
#line 514 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_READ_ONLY; }
    break;

  case 112:
#line 515 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_DUMP; }
    break;

  case 113:
#line 516 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_NO_COPY; }
    break;

  case 114:
#line 517 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_REQUIRED; }
    break;

  case 115:
#line 518 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_HIDDEN; }
    break;

  case 116:
#line 519 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC; }
    break;

  case 117:
#line 520 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_CAN_BE_MISSING; }
    break;

  case 118:
#line 521 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_CONSTRAINT; }
    break;

  case 119:
#line 522 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_OVERRIDE; }
    break;

  case 120:
#line 523 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_COPY_OK; }
    break;

  case 121:
#line 524 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_TRANSIENT; }
    break;

  case 122:
#line 525 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_STRING_TYPE; }
    break;

  case 123:
#line 526 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_LONG_TYPE; }
    break;

  case 124:
#line 529 "griby.y"
    { (grib_yyval.act) = grib_action_create_list(grib_parser_context,(grib_yyvsp[(1) - (8)].str),(grib_yyvsp[(4) - (8)].exp),(grib_yyvsp[(7) - (8)].act)); free((grib_yyvsp[(1) - (8)].str)); }
    break;

  case 125:
#line 532 "griby.y"
    { (grib_yyval.act) = grib_action_create_while(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act));  }
    break;

  case 126:
#line 535 "griby.y"
    { (grib_yyval.act) = grib_action_create_trigger(grib_parser_context,(grib_yyvsp[(3) - (7)].explist),(grib_yyvsp[(6) - (7)].act));  }
    break;

  case 127:
#line 538 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (6)].str),(grib_yyvsp[(4) - (6)].concept_value),0,0,0,0,0,(grib_yyvsp[(6) - (6)].lval),0);  free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 128:
#line 539 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (9)].str),(grib_yyvsp[(7) - (9)].concept_value),0,0,(grib_yyvsp[(4) - (9)].str),0,0,(grib_yyvsp[(9) - (9)].lval),0);  free((grib_yyvsp[(2) - (9)].str));free((grib_yyvsp[(4) - (9)].str)); }
    break;

  case 129:
#line 540 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (12)].str),0,(grib_yyvsp[(6) - (12)].str),0,(grib_yyvsp[(4) - (12)].str),(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(10) - (12)].str),(grib_yyvsp[(12) - (12)].lval),0);  free((grib_yyvsp[(2) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); }
    break;

  case 130:
#line 541 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (10)].str),0,(grib_yyvsp[(6) - (10)].str),0,(grib_yyvsp[(4) - (10)].str),(grib_yyvsp[(8) - (10)].str),0,(grib_yyvsp[(10) - (10)].lval),0);  free((grib_yyvsp[(2) - (10)].str));free((grib_yyvsp[(6) - (10)].str));free((grib_yyvsp[(4) - (10)].str));free((grib_yyvsp[(8) - (10)].str)); }
    break;

  case 131:
#line 542 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (14)].str),0,(grib_yyvsp[(8) - (14)].str),(grib_yyvsp[(2) - (14)].str),(grib_yyvsp[(6) - (14)].str),(grib_yyvsp[(10) - (14)].str),(grib_yyvsp[(12) - (14)].str),(grib_yyvsp[(14) - (14)].lval),0);  free((grib_yyvsp[(4) - (14)].str));free((grib_yyvsp[(8) - (14)].str));free((grib_yyvsp[(6) - (14)].str));free((grib_yyvsp[(10) - (14)].str)); free((grib_yyvsp[(2) - (14)].str));}
    break;

  case 132:
#line 543 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (12)].str),0,(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(2) - (12)].str),(grib_yyvsp[(6) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,(grib_yyvsp[(12) - (12)].lval),0);  free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); free((grib_yyvsp[(2) - (12)].str));}
    break;

  case 133:
#line 544 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (8)].str),(grib_yyvsp[(6) - (8)].concept_value),0,(grib_yyvsp[(2) - (8)].str),0,0,0,(grib_yyvsp[(8) - (8)].lval),0);  free((grib_yyvsp[(2) - (8)].str));free((grib_yyvsp[(4) - (8)].str)); }
    break;

  case 134:
#line 545 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (11)].str),(grib_yyvsp[(9) - (11)].concept_value),0,(grib_yyvsp[(2) - (11)].str),(grib_yyvsp[(6) - (11)].str),0,0,(grib_yyvsp[(11) - (11)].lval),0);  free((grib_yyvsp[(2) - (11)].str));free((grib_yyvsp[(4) - (11)].str));free((grib_yyvsp[(6) - (11)].str)); }
    break;

  case 135:
#line 546 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (6)].str),(grib_yyvsp[(4) - (6)].concept_value),0,0,0,0,0,(grib_yyvsp[(6) - (6)].lval),1);  free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 136:
#line 547 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (9)].str),(grib_yyvsp[(7) - (9)].concept_value),0,0,(grib_yyvsp[(4) - (9)].str),0,0,(grib_yyvsp[(9) - (9)].lval),1);  free((grib_yyvsp[(2) - (9)].str));free((grib_yyvsp[(4) - (9)].str)); }
    break;

  case 137:
#line 548 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (12)].str),0,(grib_yyvsp[(6) - (12)].str),0,(grib_yyvsp[(4) - (12)].str),(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(10) - (12)].str),(grib_yyvsp[(12) - (12)].lval),1);  free((grib_yyvsp[(2) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); }
    break;

  case 138:
#line 549 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (10)].str),0,(grib_yyvsp[(6) - (10)].str),0,(grib_yyvsp[(4) - (10)].str),(grib_yyvsp[(8) - (10)].str),0,(grib_yyvsp[(10) - (10)].lval),1);  free((grib_yyvsp[(2) - (10)].str));free((grib_yyvsp[(6) - (10)].str));free((grib_yyvsp[(4) - (10)].str));free((grib_yyvsp[(8) - (10)].str)); }
    break;

  case 139:
#line 550 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (14)].str),0,(grib_yyvsp[(8) - (14)].str),(grib_yyvsp[(2) - (14)].str),(grib_yyvsp[(6) - (14)].str),(grib_yyvsp[(10) - (14)].str),(grib_yyvsp[(12) - (14)].str),(grib_yyvsp[(14) - (14)].lval),1);  free((grib_yyvsp[(4) - (14)].str));free((grib_yyvsp[(8) - (14)].str));free((grib_yyvsp[(6) - (14)].str));free((grib_yyvsp[(10) - (14)].str));free((grib_yyvsp[(12) - (14)].str)); free((grib_yyvsp[(2) - (14)].str));}
    break;

  case 140:
#line 551 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (12)].str),0,(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(2) - (12)].str),(grib_yyvsp[(6) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,(grib_yyvsp[(12) - (12)].lval),1);  free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); free((grib_yyvsp[(2) - (12)].str));}
    break;

  case 141:
#line 552 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (8)].str),(grib_yyvsp[(6) - (8)].concept_value),0,(grib_yyvsp[(2) - (8)].str),0,0,0,(grib_yyvsp[(8) - (8)].lval),1);  free((grib_yyvsp[(2) - (8)].str));free((grib_yyvsp[(4) - (8)].str)); }
    break;

  case 142:
#line 553 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (11)].str),(grib_yyvsp[(9) - (11)].concept_value),0,(grib_yyvsp[(2) - (11)].str),(grib_yyvsp[(6) - (11)].str),0,0,(grib_yyvsp[(11) - (11)].lval),1);  free((grib_yyvsp[(2) - (11)].str));free((grib_yyvsp[(4) - (11)].str));free((grib_yyvsp[(6) - (11)].str)); }
    break;

  case 144:
#line 558 "griby.y"
    { (grib_yyval.concept_value) = (grib_yyvsp[(2) - (2)].concept_value); (grib_yyvsp[(2) - (2)].concept_value)->next = (grib_yyvsp[(1) - (2)].concept_value);   }
    break;

  case 146:
#line 562 "griby.y"
    { (grib_yyval.case_value) = (grib_yyvsp[(2) - (2)].case_value); (grib_yyvsp[(2) - (2)].case_value)->next = (grib_yyvsp[(1) - (2)].case_value);   }
    break;

  case 147:
#line 565 "griby.y"
    { (grib_yyval.case_value) = grib_case_new(grib_parser_context,(grib_yyvsp[(2) - (4)].explist),(grib_yyvsp[(4) - (4)].act));  }
    break;

  case 148:
#line 569 "griby.y"
    { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[(3) - (10)].explist),(grib_yyvsp[(6) - (10)].case_value),(grib_yyvsp[(9) - (10)].act)); }
    break;

  case 149:
#line 570 "griby.y"
    { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[(3) - (9)].explist),(grib_yyvsp[(6) - (9)].case_value),grib_action_create_noop(grib_parser_context,"continue")); }
    break;

  case 150:
#line 571 "griby.y"
    { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[(3) - (7)].explist),(grib_yyvsp[(6) - (7)].case_value),0); }
    break;

  case 151:
#line 574 "griby.y"
    { (grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,(grib_yyvsp[(1) - (5)].concept_names),(grib_yyvsp[(4) - (5)].concept_condition));  }
    break;

  case 153:
#line 578 "griby.y"
    { (grib_yyvsp[(1) - (3)].concept_names)->next = (grib_yyvsp[(3) - (3)].concept_names); (grib_yyval.concept_names) = (grib_yyvsp[(1) - (3)].concept_names); }
    break;

  case 154:
#line 581 "griby.y"
    { (grib_yyval.concept_names) = grib_concept_value_name_new(grib_parser_context,(grib_yyvsp[(1) - (1)].str)); free((grib_yyvsp[(1) - (1)].str)); }
    break;

  case 155:
#line 582 "griby.y"
    { (grib_yyval.concept_names) = grib_concept_value_name_new(grib_parser_context,(grib_yyvsp[(1) - (1)].str)); free((grib_yyvsp[(1) - (1)].str)); }
    break;

  case 156:
#line 583 "griby.y"
    { char buf[80]; sprintf(buf,"%ld",(long)(grib_yyvsp[(1) - (1)].lval)); (grib_yyval.concept_names) = grib_concept_value_name_new(grib_parser_context,buf); }
    break;

  case 157:
#line 584 "griby.y"
    { char buf[80]; sprintf(buf,"%g",(grib_yyvsp[(1) - (1)].dval)); (grib_yyval.concept_names) = grib_concept_value_name_new(grib_parser_context,buf); }
    break;

  case 159:
#line 588 "griby.y"
    { (grib_yyvsp[(1) - (2)].concept_condition)->next = (grib_yyvsp[(2) - (2)].concept_condition); (grib_yyval.concept_condition) = (grib_yyvsp[(1) - (2)].concept_condition); }
    break;

  case 160:
#line 591 "griby.y"
    { (grib_yyval.concept_condition) = grib_concept_condition_new(grib_parser_context,(grib_yyvsp[(1) - (4)].str),(grib_yyvsp[(3) - (4)].exp)); free((grib_yyvsp[(1) - (4)].str)); }
    break;

  case 161:
#line 594 "griby.y"
    { (grib_yyval.exp) = new_accessor_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].str)); free((grib_yyvsp[(1) - (1)].str)); }
    break;

  case 162:
#line 595 "griby.y"
    { (grib_yyval.exp) = new_string_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].str));  free((grib_yyvsp[(1) - (1)].str)); }
    break;

  case 164:
#line 599 "griby.y"
    { (grib_yyval.exp) = new_long_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].lval));  }
    break;

  case 165:
#line 600 "griby.y"
    { (grib_yyval.exp) = new_double_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].dval));  /* TODO: change to new_float_expression*/}
    break;

  case 166:
#line 602 "griby.y"
    { (grib_yyval.exp) = NULL; }
    break;

  case 167:
#line 603 "griby.y"
    { (grib_yyval.exp) = (grib_yyvsp[(2) - (3)].exp); }
    break;

  case 168:
#line 604 "griby.y"
    { (grib_yyval.exp) = new_unop_expression(grib_parser_context,&op_neg,&op_neg_d,(grib_yyvsp[(2) - (2)].exp)); }
    break;

  case 169:
#line 605 "griby.y"
    { (grib_yyval.exp) = new_func_expression(grib_parser_context,(grib_yyvsp[(1) - (3)].str),NULL); free((grib_yyvsp[(1) - (3)].str));}
    break;

  case 170:
#line 606 "griby.y"
    { (grib_yyval.exp) = new_func_expression(grib_parser_context,(grib_yyvsp[(1) - (4)].str),(grib_yyvsp[(3) - (4)].explist)); free((grib_yyvsp[(1) - (4)].str));}
    break;

  case 171:
#line 610 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_pow,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 173:
#line 614 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_mul,&op_mul_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 174:
#line 615 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_div,&op_div_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 175:
#line 616 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_modulo,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 176:
#line 617 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_bit,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 177:
#line 618 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_bitoff,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 179:
#line 622 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_add,&op_add_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 180:
#line 623 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_sub,&op_sub_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 182:
#line 627 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_gt,&op_gt_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 183:
#line 629 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_eq,&op_eq_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 184:
#line 630 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_lt,&op_lt_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 185:
#line 631 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_ge,&op_ge_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 186:
#line 632 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_le,&op_le_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 187:
#line 633 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_ne,&op_ne_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 188:
#line 634 "griby.y"
    { (grib_yyval.exp) = new_string_compare_expression(grib_parser_context,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 189:
#line 639 "griby.y"
    { (grib_yyval.exp) = new_unop_expression(grib_parser_context,&op_not,NULL,(grib_yyvsp[(2) - (2)].exp)); }
    break;

  case 191:
#line 643 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_and,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 193:
#line 647 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&op_or,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp));}
    break;

  case 198:
#line 661 "griby.y"
    { (grib_yyval.rule_entry) = grib_new_rule_entry(grib_parser_context,(grib_yyvsp[(1) - (4)].str),(grib_yyvsp[(3) - (4)].exp)); free((grib_yyvsp[(1) - (4)].str)); }
    break;

  case 199:
#line 662 "griby.y"
    { (grib_yyval.rule_entry) = grib_new_rule_entry(grib_parser_context,"skip",0);}
    break;

  case 201:
#line 666 "griby.y"
    { (grib_yyvsp[(1) - (2)].rule_entry)->next = (grib_yyvsp[(2) - (2)].rule_entry); (grib_yyval.rule_entry) = (grib_yyvsp[(1) - (2)].rule_entry); }
    break;

  case 202:
#line 669 "griby.y"
    { (grib_yyval.rules) = grib_new_rule(grib_parser_context,NULL,(grib_yyvsp[(1) - (1)].rule_entry)); }
    break;

  case 203:
#line 673 "griby.y"
    { (grib_yyval.rules) = grib_new_rule(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].rule_entry)); }
    break;

  case 205:
#line 677 "griby.y"
    { (grib_yyvsp[(1) - (2)].rules)->next = (grib_yyvsp[(2) - (2)].rules); (grib_yyval.rules) = (grib_yyvsp[(1) - (2)].rules); }
    break;


/* Line 1267 of yacc.c.  */
#line 3053 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", grib_yyr1[grib_yyn], &grib_yyval, &grib_yyloc);

  YYPOPSTACK (grib_yylen);
  grib_yylen = 0;
  YY_STACK_PRINT (grib_yyss, grib_yyssp);

  *++grib_yyvsp = grib_yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  grib_yyn = grib_yyr1[grib_yyn];

  grib_yystate = grib_yypgoto[grib_yyn - YYNTOKENS] + *grib_yyssp;
  if (0 <= grib_yystate && grib_yystate <= YYLAST && grib_yycheck[grib_yystate] == *grib_yyssp)
    grib_yystate = grib_yytable[grib_yystate];
  else
    grib_yystate = grib_yydefgoto[grib_yyn - YYNTOKENS];

  goto grib_yynewstate;


/*------------------------------------.
| grib_yyerrlab -- here on detecting error |
`------------------------------------*/
grib_yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!grib_yyerrstatus)
    {
      ++grib_yynerrs;
#if ! YYERROR_VERBOSE
      grib_yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T grib_yysize = grib_yysyntax_error (0, grib_yystate, grib_yychar);
	if (grib_yymsg_alloc < grib_yysize && grib_yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T grib_yyalloc = 2 * grib_yysize;
	    if (! (grib_yysize <= grib_yyalloc && grib_yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      grib_yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (grib_yymsg != grib_yymsgbuf)
	      YYSTACK_FREE (grib_yymsg);
	    grib_yymsg = (char *) YYSTACK_ALLOC (grib_yyalloc);
	    if (grib_yymsg)
	      grib_yymsg_alloc = grib_yyalloc;
	    else
	      {
		grib_yymsg = grib_yymsgbuf;
		grib_yymsg_alloc = sizeof grib_yymsgbuf;
	      }
	  }

	if (0 < grib_yysize && grib_yysize <= grib_yymsg_alloc)
	  {
	    (void) grib_yysyntax_error (grib_yymsg, grib_yystate, grib_yychar);
	    grib_yyerror (grib_yymsg);
	  }
	else
	  {
	    grib_yyerror (YY_("syntax error"));
	    if (grib_yysize != 0)
	      goto grib_yyexhaustedlab;
	  }
      }
#endif
    }



  if (grib_yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (grib_yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (grib_yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  grib_yydestruct ("Error: discarding",
		      grib_yytoken, &grib_yylval);
	  grib_yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto grib_yyerrlab1;


/*---------------------------------------------------.
| grib_yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
grib_yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label grib_yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto grib_yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (grib_yylen);
  grib_yylen = 0;
  YY_STACK_PRINT (grib_yyss, grib_yyssp);
  grib_yystate = *grib_yyssp;
  goto grib_yyerrlab1;


/*-------------------------------------------------------------.
| grib_yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
grib_yyerrlab1:
  grib_yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      grib_yyn = grib_yypact[grib_yystate];
      if (grib_yyn != YYPACT_NINF)
	{
	  grib_yyn += YYTERROR;
	  if (0 <= grib_yyn && grib_yyn <= YYLAST && grib_yycheck[grib_yyn] == YYTERROR)
	    {
	      grib_yyn = grib_yytable[grib_yyn];
	      if (0 < grib_yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (grib_yyssp == grib_yyss)
	YYABORT;


      grib_yydestruct ("Error: popping",
		  grib_yystos[grib_yystate], grib_yyvsp);
      YYPOPSTACK (1);
      grib_yystate = *grib_yyssp;
      YY_STACK_PRINT (grib_yyss, grib_yyssp);
    }

  if (grib_yyn == YYFINAL)
    YYACCEPT;

  *++grib_yyvsp = grib_yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", grib_yystos[grib_yyn], grib_yyvsp, grib_yylsp);

  grib_yystate = grib_yyn;
  goto grib_yynewstate;


/*-------------------------------------.
| grib_yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
grib_yyacceptlab:
  grib_yyresult = 0;
  goto grib_yyreturn;

/*-----------------------------------.
| grib_yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
grib_yyabortlab:
  grib_yyresult = 1;
  goto grib_yyreturn;

#ifndef grib_yyoverflow
/*-------------------------------------------------.
| grib_yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
grib_yyexhaustedlab:
  grib_yyerror (YY_("memory exhausted"));
  grib_yyresult = 2;
  /* Fall through.  */
#endif

grib_yyreturn:
  if (grib_yychar != YYEOF && grib_yychar != YYEMPTY)
     grib_yydestruct ("Cleanup: discarding lookahead",
		 grib_yytoken, &grib_yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (grib_yylen);
  YY_STACK_PRINT (grib_yyss, grib_yyssp);
  while (grib_yyssp != grib_yyss)
    {
      grib_yydestruct ("Cleanup: popping",
		  grib_yystos[*grib_yyssp], grib_yyvsp);
      YYPOPSTACK (1);
    }
#ifndef grib_yyoverflow
  if (grib_yyss != grib_yyssa)
    YYSTACK_FREE (grib_yyss);
#endif
#if YYERROR_VERBOSE
  if (grib_yymsg != grib_yymsgbuf)
    YYSTACK_FREE (grib_yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (grib_yyresult);
}


#line 681 "griby.y"


static grib_concept_value *reverse_concept(grib_concept_value *r,grib_concept_value *s)
{
    grib_concept_value *v;

    if(r == NULL) return s;

    v         = r->next;
    r->next   = s;
    return reverse_concept(v,r);
}


static grib_concept_value* reverse(grib_concept_value* r)
{
    return reverse_concept(r,NULL);
}


