/**
* Copyright 2005-2007 ECMWF
*
* Licensed under the GNU Lesser General Public License which
* incorporates the terms and conditions of version 3 of the GNU
* General Public License.
* See LICENSE and gpl-3.0.txt for details.
*/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_long
   IMPLEMENTS = unpack_long;unpack_double
   IMPLEMENTS = init;dump
   MEMBERS=const char* fname
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_double(grib_accessor*, double* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_ksec {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in ksec */
	const char* fname;
} grib_accessor_ksec;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_ksec = {
    &grib_accessor_class_long,                      /* super                     */
    "ksec",                      /* name                      */
    sizeof(grib_accessor_ksec),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    &dump,                       /* describes himself         */
    0,                /* get length of section     */
    0,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    0,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,               /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    &unpack_double,              /* grib_unpack procedures double  */
    0,                /* grib_pack procedures string    */
    0,              /* grib_unpack procedures string  */
    0,                 /* grib_pack procedures bytes     */
    0,               /* grib_unpack procedures bytes   */
    0,            /* pack_expression */
    0,              /* notify_change   */
    0,                /* update_size   */
    0,            /* preferred_size   */
    0,                    /* resize   */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor    */
    0,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,             		/* clear          */
};


grib_accessor_class* grib_accessor_class_ksec = &_grib_accessor_class_ksec;


static void init_class(grib_accessor_class* c)
{
	c->next_offset	=	(*(c->super))->next_offset;
	c->value_count	=	(*(c->super))->value_count;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->get_native_type	=	(*(c->super))->get_native_type;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_long	=	(*(c->super))->pack_long;
	c->pack_double	=	(*(c->super))->pack_double;
	c->pack_string	=	(*(c->super))->pack_string;
	c->unpack_string	=	(*(c->super))->unpack_string;
	c->pack_bytes	=	(*(c->super))->pack_bytes;
	c->unpack_bytes	=	(*(c->super))->unpack_bytes;
	c->pack_expression	=	(*(c->super))->pack_expression;
	c->notify_change	=	(*(c->super))->notify_change;
	c->update_size	=	(*(c->super))->update_size;
	c->preferred_size	=	(*(c->super))->preferred_size;
	c->resize	=	(*(c->super))->resize;
	c->nearest_smaller_value	=	(*(c->super))->nearest_smaller_value;
	c->next	=	(*(c->super))->next;
	c->compare	=	(*(c->super))->compare;
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->clear	=	(*(c->super))->clear;
}
/* END_CLASS_IMP */


static void init(grib_accessor* a, const long len , grib_arguments* arg )
{
  grib_accessor_ksec *self = (grib_accessor_ksec*)a;
  a->length = 0;
  self->fname = grib_arguments_get_name(a->parent->h,arg,0);
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
  grib_accessor_ksec *self = (grib_accessor_ksec*)a;
  grib_dump_label(dumper,a,self->fname);
}

static int  unpack_double   (grib_accessor* a, double* val, size_t *len)
{
  grib_accessor_ksec *self = (grib_accessor_ksec*)a;

  char aname[1024];

  size_t ulen = *len;
  size_t llen = 0;
  char fname[1024];
  char filename[2048]={0,};
  FILE* f = NULL;

  grib_recompose_name(a->parent->h, NULL, self->fname, fname,1);

  grib_context_full_path(a->parent->h->context,fname,filename);

  f = fopen(filename,"r");

  if(f){
    while(fgets(aname,100, f))
    {
      if(aname[0] != '#')
      { sscanf(aname,"%s",aname);
        llen = ulen;
        grib_get_double_array_internal(a->parent->h,aname,val,&llen);
        val+= llen;
        ulen -= llen;
      }
    }
    *len -= ulen;

    return GRIB_SUCCESS;
  }
  else{
    *len = 0;
    grib_context_log(a->parent->h->context, GRIB_LOG_ERROR, "file %s  not found\n", filename  );
    return GRIB_SUCCESS;
  }
}

static int    unpack_long   (grib_accessor* a, long* val, size_t *len)
{
  grib_accessor_ksec *self = (grib_accessor_ksec*)a;

  char aname[1024];

  size_t ulen = *len;
  size_t llen = 0;
  char fname[1024];
  char filename[2048]={0,};
  FILE* f = NULL;

  grib_recompose_name(a->parent->h,NULL, self->fname, fname,1);

  grib_context_full_path(a->parent->h->context,fname,filename);

  f = fopen(filename,"r");

  if(f){
    while(fgets(aname,100, f))
    {
      if(aname[0] != '#')
      { sscanf(aname,"%s",aname);
        llen = ulen;
        /*  grib_context_log(a->parent->h->context,GRIB_LOG_WARNING," unpacking %s : already %d - left %d \n", aname,*len - ulen,llen); */
        grib_get_long_array_internal(a->parent->h,aname,val,&llen);

        val+= llen;
        /* grib_context_log(a->parent->h->context,GRIB_LOG_WARNING," unpacked %s :  %d values %d left \n", aname, *len - ulen, llen ); */
        ulen -= llen;
      }
    }
    *len -= ulen;

    return GRIB_SUCCESS;
  }
  else{
    *len = 0;
    grib_context_log(a->parent->h->context, GRIB_LOG_ERROR, "file %s  not found\n", filename  );
    return GRIB_SUCCESS;
  }
}

