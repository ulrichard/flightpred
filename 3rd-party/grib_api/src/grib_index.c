/**
 * Copyright 2005-2007 ECMWF
 *
 * Licensed under the GNU Lesser General Public License which
 * incorporates the terms and conditions of version 3 of the GNU
 * General Public License.
 * See LICENSE and gpl-3.0.txt for details.
 */

/*
 *
 * Description: index
 *
 * Author: Enrico Fucile <enrico.fucile@ecmwf.int>
 *
 *
 */

#include "grib_api_internal.h"

#define INDEX_START_SIZE 2000
#define MAX_INDEX_KEYS 20
#define MAX_INDEX_KEY_SIZE 10000
#define STRING_VALUE_LEN 100
#define UNDEF_LONG -99999
#define UNDEF_DOUBLE -99999

static char* get_key(char** keys,int *type) {
	char* key=NULL;
	char* p=NULL;

	if (*keys == 0 || keys==NULL) return NULL;
	*type=GRIB_TYPE_STRING;
	p=*keys;
	while (*p == ' ') p++;


	while (*p != 0 && *p != ':' && *p != ',') p++;
	if ( *p == ':' ) {
		*type=grib_type_to_int(*(p+1));
		*p=0;
		p++;
		while (*p != 0 && *p != ',') {*(p++)=0;}
	} else *type=GRIB_TYPE_UNDEFINED;
	if (*p) {*p=0;p++;}
	key=strdup(*keys);
	*keys= *p==0 ? NULL : p;
	return key;
}

static int compare_long(const void* a,const void* b) {
	long* arg1 = (long*) a;
	long* arg2 = (long*) b;
    if( *arg1 == *arg2 ) return 0;

    return *arg1 < *arg2 ? -1 : 1;
}

static int compare_double(const void* a,const void* b) {
	double* arg1 = (double*) a;
	double* arg2 = (double*) b;
    if( *arg1 == *arg2 ) return 0;

    return *arg1 < *arg2 ? -1 : 1;

}


static int compare_string(const void* a,const void* b) {
  char* arg1 = (char*) a;
  char* arg2 = (char*) b;

  while (*arg1 != 0 && *arg2 != 0 && *(arg1++) == *(arg2++) ) {}
  
  if( *arg1 == *arg2 ) return 0;

  return *arg1 < *arg2 ? -1 : 1;
}


static grib_index_key* grib_index_new_key(grib_context* c,grib_index_key* keys,
		const char* key,int type,int *err) {
	grib_index_key *next=NULL,*current=NULL;

	next=(grib_index_key*)grib_context_malloc_clear(c,sizeof(grib_index_key));
	if (!next) {
		grib_context_log(c,GRIB_LOG_ERROR,
				"unable to allocate %d bytes",
				sizeof(grib_index_key));
		*err=GRIB_OUT_OF_MEMORY;
		return NULL;
	}
	next->values=grib_context_malloc_clear(c,sizeof(grib_string));
	if (!next->values) {
		grib_context_log(c,GRIB_LOG_ERROR,
				"unable to allocate %d bytes",
				sizeof(grib_string));
		*err=GRIB_OUT_OF_MEMORY;
		return NULL;
	}

	if (!keys) {
		keys=next;
		current=keys;
	} else {
		current=keys;
		while (current->next) current=current->next;
		current->next=next;
		current=current->next;
	}

	current->type=type;
	current->name=grib_context_strdup(c,key);
	return keys;
}



static grib_index* grib_index_create(grib_context* c,const char* key,int *err) {
	grib_index* index;
	grib_index_key* keys=NULL;
	char* q;
	int type;
	char* p= grib_context_strdup(c,key);
	q=p;

	*err=0;
	if (!c) c=grib_context_get_default();

	index=(grib_index*)grib_context_malloc_clear(c,sizeof(grib_index));
	if (!index) {
		grib_context_log(c,GRIB_LOG_ERROR,"unable to create index");
		*err=GRIB_OUT_OF_MEMORY;
		return NULL;
	}
	index->context=c;

	while ((key=get_key(&p,&type))!=NULL) {
		keys=grib_index_new_key(c,keys,key,type,err);
		if (*err) return NULL;
	}
	index->keys=keys;
	index->fields=(grib_field_tree*)grib_context_malloc_clear(c,
			sizeof(grib_field_tree));
	if (!index->fields) {*err=GRIB_OUT_OF_MEMORY;return NULL;}

	grib_context_free(c,q);
	return index;
}

static void grib_index_key_delete(grib_context* c,grib_index_key* keys) {
	grib_index_key* current=keys;
	if (!keys) return;
	while (current) {
		grib_context_free(c,current->name);
		keys=current->next;
		grib_context_free(c,current);
		current=keys;
	}
}

static void grib_field_tree_delete(grib_context* c,grib_field_tree* fields) {
	grib_field_tree* current=fields;
	grib_field_tree* level=fields;
	grib_field_tree* q=NULL;
	grib_field* field=NULL;
	grib_field* p=NULL;
    int err=0;

	while (level) {
		current=level;
		level=level->next_level;
		while (current) {
			field=current->field;
			while (field) {
				grib_file_close(field->file->name,&err);
				p=field->next;
				grib_context_free(c,field);
				field=p;
			}
			q=current->next;
			if (current->value) grib_context_free(c,current->value);
			grib_context_free(c,current);
			current=q;
		}
	}
}

void grib_index_delete(grib_index* index) {
	grib_index_key_delete(index->context,index->keys);
	grib_field_tree_delete(index->context,index->fields); 
	grib_context_free(index->context,index);
}

grib_index* grib_index_new_from_file(grib_context* c,
		char* filename,const char* keys,int *err) {
	double offset,dval;
	size_t svallen;
	long length,lval;
	char buf[1024]={0,};
	grib_index* index=NULL;
	grib_index_key* index_key=NULL;
	grib_handle* h=NULL;
	grib_field* field;
	grib_field_tree* field_tree;
	grib_file* file=NULL;

	if(!c) c=grib_context_get_default();

	file=grib_file_open(filename,"r",err);
	if (!file || !file->handle) return NULL;
    
    fseeko(file->handle,0,SEEK_SET);

	index=grib_index_create(c,keys,err);

	while ((h=grib_handle_new_from_file(c,file->handle,err))!=NULL) {
		grib_string* v=0;
		index_key=index->keys;
		field_tree=index->fields;
		index_key->value[0]=0;

		while (index_key) {
			if (index_key->type==GRIB_TYPE_UNDEFINED) {
				*err=grib_get_native_type(h,index_key->name,&(index_key->type));
				if (*err) {
					grib_context_log(c,GRIB_LOG_ERROR,"index: unable to get type of %s",index_key->name);
					return NULL;
				}
			}
			svallen=1024;
			switch (index_key->type) {
				case GRIB_TYPE_STRING:
					*err=grib_get_string(h,index_key->name,buf,&svallen);
                    if (*err==GRIB_NOT_FOUND) sprintf(buf,"undef");
					break;
				case GRIB_TYPE_LONG:
					*err=grib_get_long(h,index_key->name,&lval);
                    if (*err==GRIB_NOT_FOUND) sprintf(buf,"undef");
					else sprintf(buf,"%ld",lval);
					break;
				case GRIB_TYPE_DOUBLE:
					*err=grib_get_double(h,index_key->name,&dval);
                    if (*err==GRIB_NOT_FOUND) sprintf(buf,"undef");
                    else sprintf(buf,"%g",dval);
					break;
				default :
					*err=GRIB_WRONG_TYPE;
					return NULL;
			}
            if (*err && *err != GRIB_NOT_FOUND) {
				grib_context_log(c,GRIB_LOG_ERROR,"unable to create index. \"%s\": %s",index_key->name,grib_get_error_message(*err));
				return NULL;
			}

			if (!index_key->values->string) {
				index_key->values->string=grib_context_strdup(c,buf);
				index_key->values_count++;
			} else {
				v=index_key->values;
				while (v->next && strcmp(v->string,buf)) v=v->next;
				if (strcmp(v->string,buf)) {
					index_key->values_count++;
					if (v->next) v=v->next;
					v->next=grib_context_malloc_clear(c,sizeof(grib_string));
					v->next->string=grib_context_strdup(c,buf);
				}
			}

			if (!field_tree->value) {
				field_tree->value=grib_context_strdup(c,buf);
			} else {
				while (field_tree->next &&
						(field_tree->value==NULL ||
						 strcmp(field_tree->value,buf)))
					field_tree=field_tree->next;

				if (!field_tree->value || strcmp(field_tree->value,buf)){
					field_tree->next=
						(grib_field_tree*)grib_context_malloc_clear(c,
																	sizeof(grib_field_tree));
					field_tree=field_tree->next;
					field_tree->value=grib_context_strdup(c,buf);
				} 
			}

			if (index_key->next) {
				if (!field_tree->next_level) {
					field_tree->next_level=
						grib_context_malloc_clear(c,sizeof(grib_field_tree));
				}
				field_tree=field_tree->next_level;
			}
			index_key=index_key->next;
		}

		field=grib_context_malloc_clear(c,sizeof(grib_field));
		field->file=file;
		*err=grib_get_double(h,"offset",&offset);
		if (*err) return NULL;
		field->offset=(off_t)offset;

		*err=grib_get_long(h,"totalLength",&length);
		if (*err) return NULL;
		field->length=length;


		if (field_tree->field) {
			grib_field* pfield=field_tree->field;
			while (pfield->next) pfield=pfield->next;
			pfield->next=field;
		} else 
			field_tree->field=field;

		if (h) grib_handle_delete(h);

	}

	grib_file_close(file->name,err);
	index->rewind=1;
	return index;
}

int grib_index_get_size(grib_index* index,const char* key,size_t* size) {
	grib_index_key* k=index->keys;
	while (k && strcmp(k->name,key)) k=k->next;
	if (!k) return GRIB_NOT_FOUND;
	*size=k->values_count;
	return 0;
}

int grib_index_get_string(grib_index* index,const char* key,
		char** values,size_t *size) {
	grib_index_key* k=index->keys;
	grib_string* kv;
	int i=0;
	while (k && strcmp(k->name,key)) k=k->next;
	if (!k) return GRIB_NOT_FOUND;
	if (k->values_count>*size) return GRIB_ARRAY_TOO_SMALL;
	kv=k->values;
	while (kv) {
		values[i++]=grib_context_strdup(index->context,kv->string);
		kv=kv->next;
	}
	*size=k->values_count;
    qsort(values,*size,sizeof(char*),&compare_string);
    
	return GRIB_SUCCESS;
}

int grib_index_get_long(grib_index* index,const char* key,
		long* values,size_t *size) {
	grib_index_key* k=index->keys;
	grib_string* kv;
	int i=0;
	while (k && strcmp(k->name,key)) k=k->next;
	if (!k) return GRIB_NOT_FOUND;
	if (k->type != GRIB_TYPE_LONG) {
		grib_context_log(index->context,GRIB_LOG_ERROR,
				"unable to get index %s as long");
		return GRIB_WRONG_TYPE;
	}
	if (k->values_count > *size) return GRIB_ARRAY_TOO_SMALL;
	kv=k->values;
	while (kv) {
		if (strcmp(kv->string,"undef") )
                  values[i++]=atol(kv->string);
        else
          values[i++]=UNDEF_LONG;
		kv=kv->next;
	}
	*size=k->values_count;
	qsort(values,*size,sizeof(long),&compare_long);

	return GRIB_SUCCESS;
}

int grib_index_get_double(grib_index* index,const char* key,
		double* values,size_t *size) {
	grib_index_key* k=index->keys;
	grib_string* kv;
	int i=0;
	while (k && strcmp(k->name,key)) k=k->next;
	if (!k) return GRIB_NOT_FOUND;
	if (k->type != GRIB_TYPE_DOUBLE) {
		grib_context_log(index->context,GRIB_LOG_ERROR,
				"unable to get index %s as double");
		return GRIB_WRONG_TYPE;
	}
	if (k->values_count>*size) return GRIB_ARRAY_TOO_SMALL;
	kv=k->values;
	while (kv) {
      if (strcmp(kv->string,"undef") )
		values[i++]=atof(kv->string);
      else
        values[i++]=UNDEF_DOUBLE;
      
		kv=kv->next;
	}
	*size=k->values_count;
	qsort(values,*size,sizeof(double),&compare_double);

	return GRIB_SUCCESS;
}

int grib_index_select_long(grib_index* index,const char* skey,long value) {
	grib_index_key* key=NULL;
	int err=GRIB_NOT_FOUND;

	if (!index) {
		grib_context* c=grib_context_get_default();
		grib_context_log(c,GRIB_LOG_ERROR,"null index pointer");
		return GRIB_INTERNAL_ERROR;
	}
    index->orderby=0;
	key=index->keys;

	while (key) {
		if (!strcmp(key->name,skey)) {
			err=0;
			break;
		}
		key=key->next;
	}

	if (err) {
		grib_context_log(index->context,GRIB_LOG_ERROR,
				"key \"%s\" not found in index",skey);
		return err;
	}

	sprintf(key->value,"%ld",value);
	grib_index_rewind(index);
	return 0;
}

int grib_index_select_double(grib_index* index,const char* skey,double value) {
	grib_index_key* key=NULL;
	int err=GRIB_NOT_FOUND;

	if (!index) {
		grib_context* c=grib_context_get_default();
		grib_context_log(c,GRIB_LOG_ERROR,"null index pointer");
		return GRIB_INTERNAL_ERROR;
	}
    index->orderby=0;
	key=index->keys;

	while (key ) {
		if (!strcmp(key->name,skey)) {
			err=0;
			break;
		}
		key=key->next;
	}

	if (err) {
		grib_context_log(index->context,GRIB_LOG_ERROR,
				"key \"%s\" not found in index",skey);
		return err;
	}

	sprintf(key->value,"%g",value);
	grib_index_rewind(index);
	return 0;
}

int grib_index_select_string(grib_index* index,const char* skey,char* value) {
	grib_index_key* key=NULL;
	int err=GRIB_NOT_FOUND;

	if (!index) {
		grib_context* c=grib_context_get_default();
		grib_context_log(c,GRIB_LOG_ERROR,"null index pointer");
		return GRIB_INTERNAL_ERROR;
	}
    index->orderby=0;
	key=index->keys;

	while (key ) {
		if (!strcmp(key->name,skey)) {
			err=0;
			break;
		}
		key=key->next;
	}

	if (err) {
		grib_context_log(index->context,GRIB_LOG_ERROR,
				"key \"%s\" not found in index",skey);
		return err;
	}

	sprintf(key->value,"%s",value);
	grib_index_rewind(index);
	return 0;
}

static grib_handle* grib_get_handle(grib_field* field,int *err) {
	grib_handle* h=NULL;
	grib_file_open(field->file->name,"r",err);
	if (*err!=GRIB_SUCCESS) return NULL;

	fseeko(field->file->handle,field->offset,SEEK_SET);
	h=grib_handle_new_from_file(0,field->file->handle,err);
	if (*err!=GRIB_SUCCESS) return NULL;

	grib_file_close(field->file->name,err);
	return h;
}

static int grib_index_execute(grib_index* index) {
	grib_index_key* keys=index->keys;
	grib_field_tree* fields;

	if (!index) return GRIB_INTERNAL_ERROR;

    fields=index->fields;
	index->rewind=0;

	while (keys) {
		if (keys->value[0]) {
			while (fields && strcmp(fields->value,keys->value))
				fields=fields->next;
			if (fields && !strcmp(fields->value,keys->value)) {
				if (fields->next_level) {
					keys=keys->next;
					fields=fields->next_level;
				} else {
					index->current=index->fieldset;
					while(index->current->next) index->current=index->current->next;
					index->current->field=fields->field;
					return 0;
				}
			} else return GRIB_END_OF_INDEX;
		} else {
			grib_context_log(index->context,GRIB_LOG_ERROR,
					"please select a value for index key \"%s\"",
					keys->name);
			return GRIB_NOT_FOUND;
		}
	}

	return 0;
}

grib_handle* grib_handle_new_from_index(grib_index* index,int *err) {
	grib_index_key* keys;
	grib_field_list *fieldset,*next;
	grib_handle* h=NULL;
	grib_context* c=NULL;

	if (!index) return NULL;
	c=index->context;
	if (!index->rewind) {
		if (!index->current) {*err=GRIB_END_OF_INDEX;return NULL;}

		if (index->current->field->next)
			index->current->field=index->current->field->next;
		else if(index->current->next)
			index->current=index->current->next;
		else  {*err=GRIB_END_OF_INDEX;return NULL;}

		h=grib_get_handle(index->current->field,err);
		return h;
	}

	if (!index->fieldset) {
		index->fieldset=grib_context_malloc_clear(index->context,
				sizeof(grib_field_list));
		if (!index->fieldset) {
			grib_context_log(index->context,GRIB_LOG_ERROR,
					"unable to allocat %d bytes",
					sizeof(grib_field_list));
			return NULL;
		}
		index->current=index->fieldset;
	} else {
		fieldset=index->fieldset;
		while(fieldset->next) {
			next=fieldset->next;
			grib_context_free(c,fieldset);
			fieldset=next;
		}
		fieldset->field=NULL;
		fieldset->next=NULL;
		index->fieldset=fieldset;
		index->current=fieldset;
	}

	*err=GRIB_END_OF_INDEX;
	h=NULL;
	keys=index->keys;

	if ((*err=grib_index_execute(index))==GRIB_SUCCESS) {

		if (!index->fieldset) {*err=GRIB_END_OF_INDEX;return NULL;}
		index->current=index->fieldset;
		h=grib_get_handle(index->current->field,err);
	}
	return h;

}

void grib_index_rewind(grib_index* index) {
	index->rewind=1;
}




