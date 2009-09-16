/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
/* Line 1489 of yacc.c.  */
#line 234 "y.tab.h"
	YYSTYPE;
# define grib_yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE grib_yylval;

