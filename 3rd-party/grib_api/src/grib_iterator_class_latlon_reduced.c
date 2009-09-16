/**
* Copyright 2005-2007 ECMWF
* 
* Licensed under the GNU Lesser General Public License which
* incorporates the terms and conditions of version 3 of the GNU
* General Public License.
* See LICENSE and gpl-3.0.txt for details.
*/

/**************************************
 *  Enrico Fucile
 **************************************/


#include "grib_api_internal.h"
#include <math.h>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = iterator
   SUPER      = grib_iterator_class_gen
   IMPLEMENTS = destroy
   IMPLEMENTS = init;next
   MEMBERS     =   double *las
   MEMBERS     =   double *los
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "iterator.class" and rerun ./make_class.pl

*/


static void init_class              (grib_iterator_class*);

static int init               (grib_iterator* i,grib_handle*,grib_arguments*);
static int next               (grib_iterator* i, double *lat, double *lon, double *val);
static int destroy            (grib_iterator* i);


typedef struct grib_iterator_latlon_reduced{
  grib_iterator it;
/* Members defined in gen */
	long carg;
	const char* missingValue;
/* Members defined in latlon_reduced */
	double *las;
	double *los;
} grib_iterator_latlon_reduced;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_latlon_reduced = {
    &grib_iterator_class_gen,                    /* super                     */
    "latlon_reduced",                    /* name                      */
    sizeof(grib_iterator_latlon_reduced),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    &next,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_latlon_reduced = &_grib_iterator_class_latlon_reduced;


static void init_class(grib_iterator_class* c)
{
	c->previous	=	(*(c->super))->previous;
	c->reset	=	(*(c->super))->reset;
	c->has_next	=	(*(c->super))->has_next;
}
/* END_CLASS_IMP */

static int next(grib_iterator* i, double *lat, double *lon, double *val)
{

  grib_iterator_latlon_reduced* self = (grib_iterator_latlon_reduced*)i;

  if((long)i->e >= (long)(i->nv-1))
    return 0;
  i->e++;

  *lat = self->las[i->e];
  *lon = self->los[i->e];
  *val = i->data[i->e];

  return 1;
}



static int init(grib_iterator* i,grib_handle* h,grib_arguments* args)
{
  grib_iterator_latlon_reduced* self = (grib_iterator_latlon_reduced*)i;

  int ret = GRIB_SUCCESS;
  double laf;
  double lal;
  long nlats;
  double lof,tlof;
  double lol;
  long *pl;
  size_t plsize =0 ;
  long k,j,ii;
  long nlons;
  double jdirinc = 0;
  double idirinc = 0;

  const char* latofirst   = grib_arguments_get_name(h,args,self->carg++);
  const char* longoffirst = grib_arguments_get_name(h,args,self->carg++);
  const char* latoflast   = grib_arguments_get_name(h,args,self->carg++);
  const char* longoflast  = grib_arguments_get_name(h,args,self->carg++);
  const char* nlats_name    = grib_arguments_get_name(h,args,self->carg++);
  const char* jdirec      = grib_arguments_get_name(h,args,self->carg++);
  const char* plac        = grib_arguments_get_name(h,args,self->carg++);


  if(( ret = grib_get_double_internal(h,latofirst,      &laf))) return ret;
  if(( ret = grib_get_double_internal(h,longoffirst,    &lof))) return ret;

  if(( ret = grib_get_double_internal(h,latoflast,     &lal))) return ret;
  if(( ret = grib_get_double_internal(h,longoflast,    &lol))) return ret;

  if(( ret = grib_get_long_internal(h,nlats_name,&nlats))) return ret;

  if(( ret = grib_get_double_internal(h,jdirec,&jdirinc))) return ret;

  plsize = nlats;

  pl  = grib_context_malloc(h->context,plsize*sizeof(long));

  grib_get_long_array_internal(h,plac,pl, &plsize);

  self->las = grib_context_malloc(h->context,i->nv*sizeof(double));
  self->los = grib_context_malloc(h->context,i->nv*sizeof(double));

  if (laf>lal) jdirinc=-jdirinc;
  k=0;
  for (j=0;j<nlats;j++) {
     nlons=pl[j];
     tlof=lof;
     idirinc=(lol-lof)/(nlons-1);
     for (ii=0;ii<nlons;ii++) {
       self->las[k]=laf;
       self->los[k]=tlof;
       tlof+=idirinc;
       k++;
     }
     laf+=jdirinc;
  }

  i->e = -1;
  grib_context_free(h->context,pl);

  return ret;
}


static int destroy(grib_iterator* i)
{
  grib_iterator_latlon_reduced* self = (grib_iterator_latlon_reduced*)i;
  const grib_context *c = i->h->context;

  grib_context_free(c,self->las);
  grib_context_free(c,self->los);
  return 1;
}


