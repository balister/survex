/* readval.c
 * Routines to read a prefix or number from the current input file
 * Copyright (C) 1991-2003,2005,2006 Olly Betts
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <limits.h>
#include <stddef.h> /* for offsetof */

#include "cavern.h"
#include "debug.h"
#include "filename.h"
#include "message.h"
#include "readval.h"
#include "datain.h"
#include "netbits.h"
#include "osalloc.h"
#include "str.h"

#ifdef HAVE_SETJMP_H
# define LONGJMP(JB) longjmp((JB), 1)
#else
# define LONGJMP(JB) exit(1)
#endif

int root_depr_count = 0;

/* if prefix is omitted: if f_optional return NULL, otherwise use longjmp */
static prefix *
read_prefix_(bool f_optional, bool fSurvey, bool fSuspectTypo, bool fAllowRoot)
{
   prefix *back_ptr, *ptr;
   char *name;
   size_t name_len = 32;
   size_t i;
   bool fNew;
   bool fImplicitPrefix = fTrue;
   int depth = -1;

   skipblanks();
#ifndef NO_DEPRECATED
   if (isRoot(ch)) {
      if (!fAllowRoot) {
	 compile_error_skip(/*ROOT is deprecated*/25);
	 LONGJMP(file.jbSkipLine);
      }
      if (root_depr_count < 5) {
	 compile_warning(/*ROOT is deprecated*/25);
	 if (++root_depr_count == 5)
	    compile_warning(/*Further uses of this deprecated feature will not be reported*/95);
      }
      nextch();
      ptr = root;
      if (!isNames(ch)) {
	 if (!isSep(ch)) return ptr;
	 /* Allow optional SEPARATOR after ROOT */
	 nextch();
      }
      fImplicitPrefix = fFalse;
   } else {
      ptr = pcs->Prefix;
   }
#else
   ptr = pcs->Prefix;
#endif

   i = 0;
   name = NULL;
   do {
      fNew = fFalse;
      if (isSep(ch)) fImplicitPrefix = fFalse;
      if (name == NULL) {
	 /* Need a new name buffer */
	 name = osmalloc(name_len);
      }
      /* i==0 iff this is the first pass */
      if (i) {
	 i = 0;
	 nextch();
      }
      while (isNames(ch)) {
	 if (i < pcs->Truncate) {
	    /* truncate name */
	    name[i++] = (pcs->Case == LOWER ? tolower(ch) :
			 (pcs->Case == OFF ? ch : toupper(ch)));
	    if (i >= name_len) {
	       name_len = name_len + name_len;
	       name = osrealloc(name, name_len);
	    }
	 }
	 nextch();
      }
      if (i == 0) {
	 osfree(name);
	 if (!f_optional) {
	    if (isEol(ch)) {
	       if (fSurvey) {
		  compile_error_skip(/*Expecting survey name*/89);
	       } else {
		  compile_error_skip(/*Expecting station name*/28);
	       }
	    } else {
	       compile_error_skip(/*Character `%c' not allowed in station name (use *SET NAMES to set allowed characters)*/7, ch);
	    }
	    LONGJMP(file.jbSkipLine);
	 }
	 return (prefix *)NULL;
      }

      name[i++] = '\0';

      back_ptr = ptr;
      ptr = ptr->down;
      if (ptr == NULL) {
	 /* Special case first time around at each level */
	 name = osrealloc(name, i);
	 ptr = osnew(prefix);
	 ptr->ident = name;
	 name = NULL;
	 ptr->right = ptr->down = NULL;
	 ptr->pos = NULL;
	 ptr->shape = 0;
	 ptr->stn = NULL;
	 ptr->up = back_ptr;
	 ptr->filename = file.filename;
	 ptr->line = file.line;
	 ptr->min_export = ptr->max_export = 0;
	 ptr->sflags = BIT(SFLAGS_SURVEY);
	 if (fSuspectTypo && !fImplicitPrefix)
	    ptr->sflags |= BIT(SFLAGS_SUSPECTTYPO);
	 back_ptr->down = ptr;
	 fNew = fTrue;
      } else {
	 /* Use caching to speed up adding an increasing sequence to a
	  * large survey */
	 static prefix *cached_survey = NULL, *cached_station = NULL;
	 prefix *ptrPrev = NULL;
	 int cmp = 1; /* result of strcmp ( -ve for <, 0 for =, +ve for > ) */
	 if (cached_survey == back_ptr) {
	    cmp = strcmp(cached_station->ident, name);
	    if (cmp <= 0) ptr = cached_station;
	 }
	 while (ptr && (cmp = strcmp(ptr->ident, name))<0) {
	    ptrPrev = ptr;
	    ptr = ptr->right;
	 }
	 if (cmp) {
	    /* ie we got to one that was higher, or the end */
	    prefix *newptr;
	    name = osrealloc(name, i);
	    newptr = osnew(prefix);
	    newptr->ident = name;
	    name = NULL;
	    if (ptrPrev == NULL)
	       back_ptr->down = newptr;
	    else
	       ptrPrev->right = newptr;
	    newptr->right = ptr;
	    newptr->down = NULL;
	    newptr->pos = NULL;
	    newptr->shape = 0;
	    newptr->stn = NULL;
	    newptr->up = back_ptr;
	    newptr->filename = file.filename;
	    newptr->line = file.line;
	    newptr->min_export = newptr->max_export = 0;
	    newptr->sflags = BIT(SFLAGS_SURVEY);
	    if (fSuspectTypo && !fImplicitPrefix)
	       newptr->sflags |= BIT(SFLAGS_SUSPECTTYPO);
	    ptr = newptr;
	    fNew = fTrue;
	 }
	 cached_survey = back_ptr;
	 cached_station = ptr;
      }
      depth++;
      f_optional = fFalse; /* disallow after first level */
   } while (isSep(ch));
   if (name) osfree(name);

   /* don't warn about a station that is refered to twice */
   if (!fNew) ptr->sflags &= ~BIT(SFLAGS_SUSPECTTYPO);

   if (fNew) {
      /* fNew means SFLAGS_SURVEY is currently set */
      SVX_ASSERT(TSTBIT(ptr->sflags, SFLAGS_SURVEY));
      if (!fSurvey) {
	 ptr->sflags &= ~BIT(SFLAGS_SURVEY);
	 if (TSTBIT(pcs->infer, INFER_EXPORTS)) ptr->min_export = USHRT_MAX;
      }
   } else {
      /* check that the same name isn't being used for a survey and station */
      if (fSurvey ^ TSTBIT(ptr->sflags, SFLAGS_SURVEY)) {
	 compile_error(/*`%s' can't be both a station and a survey*/27,
		       sprint_prefix(ptr));
      }
      if (!fSurvey && TSTBIT(pcs->infer, INFER_EXPORTS)) ptr->min_export = USHRT_MAX;
   }

   /* check the export level */
#if 0
   printf("R min %d max %d depth %d pfx %s\n",
	  ptr->min_export, ptr->max_export, depth, sprint_prefix(ptr));
#endif
   if (ptr->min_export == 0 || ptr->min_export == USHRT_MAX) {
      if (depth > ptr->max_export) ptr->max_export = depth;
   } else if (ptr->max_export < depth) {
      const char *filename_store = file.filename;
      unsigned int line_store = file.line;
      prefix *survey = ptr;
      char *s;
      int level;
      for (level = ptr->max_export + 1; level; level--) {
	 survey = survey->up;
	 SVX_ASSERT(survey);
      }
      s = osstrdup(sprint_prefix(survey));
      if (survey->filename) {
	 file.filename = survey->filename;
	 file.line = survey->line;
      }
      compile_error(/*Station `%s' not exported from survey `%s'*/26,
		    sprint_prefix(ptr), s);
      if (survey->filename) {
	 file.filename = filename_store;
	 file.line = line_store;
      }
      osfree(s);
#if 0
      printf(" *** pfx %s warning not exported enough depth %d "
	     "ptr->max_export %d\n", sprint_prefix(ptr),
	     depth, ptr->max_export);
#endif
   }
   return ptr;
}

/* if prefix is omitted: if f_optional return NULL, otherwise use longjmp */
extern prefix *
read_prefix_survey(bool f_optional, bool fAllowRoot)
{
   return read_prefix_(f_optional, fTrue, fFalse, fAllowRoot);
}

/* if prefix is omitted: if f_optional return NULL, otherwise use longjmp */
extern prefix *
read_prefix_stn(bool f_optional, bool fAllowRoot)
{
   return read_prefix_(f_optional, fFalse, fFalse, fAllowRoot);
}

/* if prefix is omitted: if f_optional return NULL, otherwise use longjmp */
/* Same as read_prefix_stn but implicit checks are made */
extern prefix *
read_prefix_stn_check_implicit(bool f_optional, bool fAllowRoot)
{
   return read_prefix_(f_optional, fFalse, fTrue, fAllowRoot);
}

/* if numeric expr is omitted: if f_optional return HUGE_REAL, else longjmp */
static real
read_number(bool f_optional)
{
   bool fPositive, fDigits = fFalse;
   real n = (real)0.0;
   filepos fp;
   int ch_old;

   get_pos(&fp);
   ch_old = ch;
   fPositive = !isMinus(ch);
   if (isSign(ch)) nextch();

   while (isdigit(ch)) {
      n = n * (real)10.0 + (char)(ch - '0');
      nextch();
      fDigits = fTrue;
   }

   if (isDecimal(ch)) {
      real mult = (real)1.0;
      nextch();
      while (isdigit(ch)) {
	 mult *= (real).1;
	 n += (char)(ch - '0') * mult;
	 fDigits = fTrue;
	 nextch();
      }
   }

   /* !'fRead' => !fDigits so fDigits => 'fRead' */
   if (fDigits) return (fPositive ? n : -n);

   /* didn't read a valid number.  If it's optional, reset filepos & return */
   if (f_optional) {
      set_pos(&fp);
      return HUGE_REAL;
   }

   if (isOmit(ch_old)) {
      compile_error(/*Field may not be omitted*/8);
   } else {
      compile_error_token(/*Expecting numeric field, found `%s'*/9);
   }
   LONGJMP(file.jbSkipLine);
   return 0.0; /* for brain-fried compilers */
}

extern real
read_numeric(bool f_optional, int *p_n_readings)
{
   size_t n_readings = 0;
   real tot = (real)0.0;

   skipblanks();
   if (!isOpen(ch)) {
      real r = read_number(f_optional);
      if (p_n_readings) *p_n_readings = (r == HUGE_REAL ? 0 : 1);
      return r;
   }
   nextch();

   skipblanks();
   do {
      tot += read_number(fFalse);
      ++n_readings;
      skipblanks();
   } while (!isClose(ch));
   nextch();

   if (p_n_readings) *p_n_readings = n_readings;
   /* FIXME: special averaging for bearings ... */
   /* And for percentage gradient */
   return tot / n_readings;
}

/* read numeric expr or omit (return HUGE_REAL); else longjmp */
extern real
read_numeric_or_omit(int *p_n_readings)
{
   real v = read_numeric(fTrue, p_n_readings);
   if (v == HUGE_REAL) {
      if (!isOmit(ch)) {
	 compile_error_token(/*Expecting numeric field, found `%s'*/9);
	 LONGJMP(file.jbSkipLine);
	 return 0.0; /* for brain-fried compilers */
      }
      nextch();
   }
   return v;
}

/* Don't skip blanks, variable error code */
static unsigned int
read_uint_internal(int errmsg, filepos *fp)
{
   unsigned int n = 0;
   if (!isdigit(ch)) {
      if (fp) set_pos(fp);
      compile_error_token(errmsg);
      LONGJMP(file.jbSkipLine);
   }
   while (isdigit(ch)) {
      n = n * 10 + (char)(ch - '0');
      nextch();
   }
   return n;
}

extern unsigned int
read_uint(void)
{
   skipblanks();
   return read_uint_internal(/*Expecting numeric field, found `%s'*/9, NULL);
}

extern void
read_string(char **pstr, int *plen)
{
   s_zero(pstr);

   skipblanks();
   if (ch == '\"') {
      /* String quoted in "" */
      nextch();
      while (1) {
	 if (isEol(ch)) {
	    compile_error_skip(/*Missing &quot;*/69);
	    return;
	 }

	 if (ch == '\"') break;

	 s_catchar(pstr, plen, ch);
	 nextch();
      }
   } else {
      /* Unquoted string */
      while (1) {
	 if (isEol(ch) || isComm(ch)) {
	    if (!*pstr || !(*pstr)[0]) {
	       compile_error_skip(/*Expecting string field*/121);
	    }
	    return;
	 }

	 if (isBlank(ch)) break;

	 s_catchar(pstr, plen, ch);
	 nextch();
      }
   }

   nextch();
}

static int lastday[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

extern int
last_day(int year, int month)
{
    SVX_ASSERT(month >= 1 && month <= 12);
    return (month == 2 && is_leap_year(year)) ? 29 : lastday[month - 1];
}

extern void
read_date(int *py, int *pm, int *pd)
{
   int y = 0, m = 0, d = 0;
   filepos fp;

   skipblanks();

   get_pos(&fp);
   y = read_uint_internal(/*Expecting date, found `%s'*/198, &fp);
   /* Two digit year is 19xx. */
   if (y < 100) y += 1900;
   if (y < 1970 || y > 2037) {
      compile_error_skip(/*Invalid year (< 1970 or > 2037)*/58);
      LONGJMP(file.jbSkipLine);
      return; /* for brain-fried compilers */
   }
   if (ch == '.') {
      nextch();
      m = read_uint_internal(/*Expecting date, found `%s'*/198, &fp);
      if (m < 1 || m > 12) {
	 compile_error_skip(/*Invalid month*/86);
	 LONGJMP(file.jbSkipLine);
	 return; /* for brain-fried compilers */
      }
      if (ch == '.') {
	 nextch();
	 d = read_uint_internal(/*Expecting date, found `%s'*/198, &fp);
	 if (d < 1 || d > last_day(y, m)) {
	    compile_error_skip(/*Invalid day of the month*/87);
	    LONGJMP(file.jbSkipLine);
	    return; /* for brain-fried compilers */
	 }
      }
   }
   if (py) *py = y;
   if (pm) *pm = m;
   if (pd) *pd = d;
}
