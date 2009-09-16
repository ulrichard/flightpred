/**
 * Copyright 2005-2007 ECMWF
 *
 * Licensed under the GNU Lesser General Public License which
 * incorporates the terms and conditions of version 3 of the GNU
 * General Public License.
 * See LICENSE and gpl-3.0.txt for details.
 */

/***************************************************************************
 *   Jean Baptiste Filippi - 01.11.2005                                                           *
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"
/*     grib level     */

/* This file is generated my ./make_class.pl */
#include "grib_accessor_class.h"


struct table_entry
{
  char                  *type;
  grib_accessor_class   **cclass;
};


static struct table_entry table[] =
{
  /* This file is generated my ./make_class.pl */
#include "grib_accessor_factory.h"
};

#define NUMBER(x) (sizeof(x)/sizeof(x[0]))

grib_section*  grib_create_root_section(const grib_context *context, grib_handle *h)
{
  grib_section*   s   = (grib_section*) grib_context_malloc_clear(context,sizeof(grib_section));

  if(h->context->grib_reader == NULL) {
    char fpath[1024];
    grib_context_full_path(h->context,"boot.def",fpath);
    if (!*fpath) {
      grib_context_log(h->context,GRIB_LOG_ERROR,
         "Unable find boot.def ");
    }
    grib_parse_file(h->context,fpath);
  }

  s->h = h;
  s->aclength = NULL;
  s->owner     = NULL;
  s->block    = (grib_block_of_accessors*)
    grib_context_malloc_clear(context, sizeof(grib_block_of_accessors));
  grib_context_log(context, GRIB_LOG_DEBUG, "Creating root section");
  return s;
}

grib_accessor* grib_accessor_factory(grib_section* p, grib_action* creator,
    const long len, grib_arguments* params)
{
  int i;

  for(i = 0; i < NUMBER(table) ; i++)
    if(strcmp(creator->op,table[i].type) == 0)
    {
      grib_accessor_class* c = *(table[i].cclass);
      grib_accessor*       a = (grib_accessor*) grib_context_malloc_clear(p->h->context,c->size);

      a->name                = creator->name;
      a->name_space          = creator->name_space;

      a->all_names[0]        = creator->name;
      a->all_name_spaces[0]  = creator->name_space;

      a->creator             = creator;
      a->next                = NULL;
      a->previous            = NULL;
      a->parent              = p;
      a->length              = 0 ;
      a->offset              = 0;
      a->flags               = creator->flags;

      if(p->block->last)
      {
        a->offset           = grib_get_next_position_offset(p->block->last);
#if 0
        printf("offset: p->block->last %s %s %ld %ld\n",
            p->block->last->cclass->name,
            p->block->last->name,(long)p->block->last->offset,(long)p->block->last->length);
#endif
      }
      else
        if(p->owner)
        {
          a->offset         = p->owner->offset;
        }
        else
          a->offset          = 0;

      a->cclass               = c;

#if 0
      if(p->h->buffer->ulength < a->offset+len && len != 0)
      {
        if(!p->h->buffer->growable)
        {
          grib_context_log(p->h->context, GRIB_LOG_ERROR,
              "Creating (%s)%s of %s at offset %d-%d over message boundary (%d)",
              p->owner?p->owner->name : "" ,a->name ,
              creator->op, a->offset,
              a->offset + len,
              p->h->buffer->ulength);
          grib_context_free(p->h->context,a);
          return NULL;
        }

      }
#endif
      grib_init_accessor(a, len,  params);
      /* grib_get_next_position_offset(a);  */


      /*
         if(a->flags & GRIB_ACCESSOR_FLAG_OVERRIDE)
         {
         printf("OVERRIDE\n");
         }
       */

      if(p->h->buffer->ulength < a->offset+a->length)
      {
        if(!p->h->buffer->growable)
        {
          /* grib_accessor_class *c = a->cclass; */
          if(!p->h->partial)
            grib_context_log(p->h->context, GRIB_LOG_ERROR,
                "Creating (%s)%s of %s at offset %d-%d over message boundary (%d)",
                p->owner?p->owner->name : "" ,a->name ,
                creator->op, a->offset,
                a->offset + a->length,
                p->h->buffer->ulength);
#if 0
          while(c)
          {
            grib_accessor_class *s = c->super ? *(c->super) : NULL;
            if(c->destroy) c->destroy(p->h->context,a);
            c = s;
          }
          a->cclass = NULL;

          for(i = 0; i < NUMBER(table) ; i++)
            if(strcmp("missing",table[i].type) == 0)
              a->cclass = *(table[i].cclass);
          assert(a->cclass);

#endif
          grib_free_accessor(p->h->context,a);
          return NULL;
        }

      }

      if(p->h->context->debug)
      {

        printf("CREATE: name=%s class=%s offset=%ld length=%ld action=",a->name,a->cclass->name,
            a->offset,a->length);
#if 0
        if(p->h->context->debug > 100)
          grib_dump_content(p->h,stdout,NULL,0,NULL);

        if(p->h->context->debug > 1)
          grib_dump_action_branch(stdout,a->creator,0);
#endif
        printf("\n");
      }


      if(p->h->buffer->growable)
      {
        size_t size = grib_get_next_position_offset(a);

        if(size > p->h->buffer->ulength)
        {
          grib_grow_buffer(p->h->context,p->h->buffer,size);
          p->h->buffer->ulength = size;
        }
      }


      if(p->owner)
        grib_context_log(p->h->context, GRIB_LOG_DEBUG, "Creating (%s)%s of %s at offset %d [len=%d]", p->owner->name ,a->name ,creator->op, a->offset,len,p->block);
      else
        grib_context_log(p->h->context, GRIB_LOG_DEBUG, "Creating root %s of %s at offset %d [len=%d]", a->name ,creator->op, a->offset,len,p->block);


      return a;
    }
  grib_context_log(p->h->context,GRIB_LOG_ERROR,"Unknown type : %s for %s", creator->op, creator->name);
  return NULL;
}

void grib_push_accessor(grib_accessor* a, grib_block_of_accessors* l)
{
  int id;
  if (!l->first)
    l->first = l->last = a;
  else{
    l->last->next = a;
    a->previous = l->last;
  }
  l->last = a;


  if (a->parent->h->use_trie) {
    if (*(a->all_names[0]) != '_') {
      id=grib_itrie_get_id(a->parent->h->context->keys,a->all_names[0]);


      a->same=a->parent->h->accessors[id];
      a->parent->h->accessors[id]=a;

      if(a->same == a) {
        fprintf(stderr,"---> %s\n",a->name);
        Assert(a->same != a);
      }
    }
  }

}

void grib_section_post_init(grib_section* s)
{
  grib_accessor* a = s ? s->block->first : NULL;

  while(a)  {
    grib_post_init(a);
    grib_section_post_init(grib_get_sub_section(a));
    a = a->next;
  }

}

void grib_section_adjust_sizes(grib_section* s,int update,int depth)
{
  grib_accessor* a = s ? s->block->first : NULL;
  size_t length = update ? 0 : (s?s->padding:0);
  size_t offset = (s && s->owner) ? s->owner->offset:0;
  int force_update = update > 1;
  /* int i ; */

  while(a)  {
    long l;
    grib_section_adjust_sizes(grib_get_sub_section(a),update,depth+1);

    l = a->length;
    /* printf("LENGTH %s %ld\n",a->name,l); */

    if(offset != a->offset)    {
      grib_context_log(a->parent->h->context,GRIB_LOG_DEBUG,"Offset mismatch %s A->offset %ld offset %ld\n",a->name,(long)a->offset, (long)offset);
      a->offset = offset;
    }
    length += l;
    /* { int i=0;
       for(i=0;i<depth;i++) printf("   "); printf("%s A->offset %ld offset %ld\n",a->name,(long)a->offset, (long)offset);
       } */
    offset += l;
    a = a->next;
  }

  if(s)
  {
    /* for(i=0;i<depth;i++) printf("   "); printf("SECTION %s %ld\n",s->owner?s->owner->name:"ROOT",(long)length); */
    if(s->aclength)
    {
      size_t  len = 1;
      long plen = 0;
      int lret=grib_unpack_long(s->aclength, &plen, &len);
      Assert( lret == GRIB_SUCCESS);
      /* This happens when there is some padding */
      if((plen != length) || force_update)
      {
        /* for(i=0;i<depth;i++) printf("   "); */
        /* printf("Section length mismatch stored=%ld computed=%ld %s %s\n",(long)plen,(long)length, s->owner->name,update?"updating":"using stored"); */

        if(update)
        {
          plen = length;
          lret=grib_pack_long(s->aclength, &plen, &len);
          Assert(lret == GRIB_SUCCESS);
          s->padding = 0;
        }
        else {
          if(!s->h->partial) {
            if(length >= plen)
            {
		  grib_context_log(s->h->context,GRIB_LOG_ERROR,"Invalid size %ld found for %s, assuming %ld",
	  		(long)plen,s->owner->name,(long)length);
			  /* grib_pack_long(s->aclength, length, &len); */
			  plen = length;
            }

            /* Assert(length < plen); */
            s->padding = plen - length;
			/* if(s->padding<0) s->padding = 0; */

#if 0
            if(length < plen)
            {
              /* Add some padding */
              grib_accessor* ga = NULL;
              grib_action* b = grib_action_create_gen(
                  s->h->context,
                  "padding","bytes",plen-length,NULL,0,GRIB_READ_ONLY);


              for(i=0;i<depth;i++) printf("   ");
              printf("ADDING padding %ld\n",(long)(plen - length));
              ga = grib_accessor_factory(s, b , plen-length, NULL);

              grib_free_action(s->h->context,b);



              grib_push_accessor(ga,s->block);
            }
#endif
          }
          length = plen;
        }
      }
    }

    if(s->owner) s->owner->length = length;
    s->length = length;
  }

}

int grib_get_block_length(grib_section* s, size_t *l)
{
  *l = s->length;
  return GRIB_SUCCESS;
#if 0

  /* TODO: Because grib_pack_long takes a SIGNED value, we may have problems */

  if(s->aclength)
  {
    size_t  len = 1;
    long plen = 0;

    int ret = grib_unpack_long(s->aclength, &plen, &len);
    if(ret == GRIB_SUCCESS && plen != 0)
    {
      *l = plen;
      return GRIB_SUCCESS;
    }
  }

  /* empty block */
  if(s->block->first == NULL)
  {
    *l = 0;
    return GRIB_SUCCESS;
  }
  /* no accessor for block length */
  if(s->owner)
    *l = grib_get_next_position_offset(s->block->last) - s->owner->offset;
  else
    *l = grib_get_next_position_offset(s->block->last);


  if(s->aclength)
  {
    size_t  len = 1;
    long plen = *l;

    int ret = grib_pack_long(s->aclength, &plen, &len);
    if(ret != GRIB_SUCCESS)
      ;
    if(s->h->context->debug)
      printf("SECTION updating length %ld %s\n",plen,s->owner->name);
  }

  /*
     if(s->aclength)
     Assert(*l == plen);*/

  return GRIB_SUCCESS;
#endif
}


grib_accessor* find_paddings(grib_section* s)
{
  grib_accessor* a = s ? s->block->first : NULL;

  while(a)
  {
    grib_accessor* p = find_paddings(grib_get_sub_section(a));
    if(p) return p;

    if(grib_preferred_size(a,0) != a->length)
      return a;

    a = a->next;
  }

  return NULL;
}

void grib_update_paddings(grib_section* s)
{
  grib_accessor* last = NULL;
  grib_accessor* changed;

  while((changed = find_paddings(s)) != NULL)
  {
    Assert(changed != last);
    grib_resize(changed,grib_preferred_size(changed,0));
    last = changed;
  }
}
