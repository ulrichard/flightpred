/**
* Copyright 2005-2007 ECMWF
*
* Licensed under the GNU Lesser General Public License which
* incorporates the terms and conditions of version 3 of the GNU
* General Public License.
* See LICENSE and gpl-3.0.txt for details.
*/

/*
 * C Implementation: grib_ls
 *
 * Author: Enrico Fucile <enrico.fucile@ecmwf.int>
 *
 *
 */
#include "grib_tools.h"

grib_option grib_options[]={
/*  {id, args, help}, on, command_line, value*/
    {"f",0,0,1,0,0},
    {"p:",0,0,0,1,0},
    {"F:",0,0,1,1,"%g"},
    {"P:",0,0,0,1,0},
    {"w:",0,0,0,1,0},
    {"B:",0,0,0,1,0},
    {"l:",0,0,0,1,0},
    {"i:",0,0,0,1,0},
    {"n:",0,0,1,1,"ls"},
    {"m",0,0,0,1,0},
    {"V",0,0,0,1,0},
    {"W:",0,0,1,1,"10"},
    {"S",0,0,1,0,0},
    {"M",0,0,0,1,0},
    {"H",0,0,1,0,0},
    {"g",0,0,0,1,0},
    {"P",0,0,1,0,0},
    {"G",0,0,0,1,0},
    {"7",0,0,0,1,0},
    {"v",0,0,1,0,0}
};

char* grib_tool_description="List content of grib files printing values of "
                  "some keys.\n\tIt does not fail when a key is not found.";
char* grib_tool_name="grib_ls";
char* grib_tool_usage="[options] grib_file grib_file ...";

int grib_options_count=sizeof(grib_options)/sizeof(grib_option);
double lat=0;
double lon=0;
int mode=0;
grib_nearest* n=NULL;
/*double *outlats,*outlons,*values,*lsm_values,*distances;*/

int main(int argc, char *argv[]) { return grib_tool(argc,argv);}

int grib_tool_before_getopt(grib_runtime_options* options) {
  return 0;
}

int grib_tool_init(grib_runtime_options* options) {
  char  *end = NULL, *end1=NULL;
  size_t size=4;
  int ret=0;
  double min=0,max=0;
  int i=0,idx=0;
  char* p=NULL;
  if (options->latlon) {

    lat = strtod(options->latlon,&end);
    if (*end != ',') {
      printf("ERROR: wrong latitude value\n");
      exit(1);
    }
    lon= strtod(++end,&end1);
    
    mode=GRIB_NEAREST_SAME_POINT | GRIB_NEAREST_SAME_GRID;

    if (end1 && *end1 == ',') {
      end1++;
      if (*end1 != '0') {
        p=end1;
        while (*p != ',' && *p !='\0') p++;
        if (*end1 == '4') {
            options->latlon_mode=4;
        } else if (*end1 == '1') {
            options->latlon_mode=1;
        } else {
          printf("ERROR %s: wrong mode given in option -l\n",grib_tool_name);
          exit(1);
        }
      }
      if (*p == ',') {
        p++;
        options->latlon_mask=strdup(p);
      }
    }


  }

  if (options->latlon && options->latlon_mask) {
    FILE* f=NULL;
    grib_handle* hh;
    f=fopen(options->latlon_mask,"r");
    if(!f) {
      perror(options->latlon_mask);
      exit(1);
    }
    hh=grib_handle_new_from_file(0,f,&ret);
    fclose(f);
    GRIB_CHECK(ret,0);
    n=grib_nearest_new(hh,&ret);
    GRIB_CHECK(ret,0);
    GRIB_CHECK(grib_nearest_find(n,hh,lat,lon,mode,
        options->lats,options->lons,options->mask_values,options->distances,options->indexes,&size),0);
    grib_nearest_delete(n);
    n=NULL;
    grib_handle_delete( hh);

    options->latlon_idx=-1;
    max=options->distances[0];
    for (i=0;i<4;i++)
      if (max<options->distances[i]) {max=options->distances[i];idx=i;}
    min=max;
    for (i=0;i<4;i++) {
      if ((min >= options->distances[i]) && (options->mask_values[i] >= 0.5)) {
        options->latlon_idx=i;
        min = options->distances[i];
      }
    }

    if (options->latlon_idx<0){
      min=0;
      options->latlon_idx=0;
      for (i=1;i<4;i++)
        if (min>options->distances[i]) {
          min = options->distances[i];
          options->latlon_idx=i;
        }
    }
  }

  return 0;
}

int grib_tool_new_file_action(grib_runtime_options* options) {
   return 0;
}

int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h) {
  size_t size=4;

  if (options->latlon) {
    int err=0;
    if (!n) n=grib_nearest_new(h,&err);
    GRIB_CHECK(grib_nearest_find(n,h,lat,lon,mode,
        options->lats,options->lons,options->values,
        options->distances,options->indexes,&size),0);

  }
  return 0;
}

int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h) {
  grib_handle_delete(h);
  return 0;
}

void grib_tool_print_key_values(grib_runtime_options* options,grib_handle* h) {
  grib_print_key_values(options,h);
}

int grib_tool_finalise_action(grib_runtime_options* options) {
  int i=0;
  if (options->latlon) {
    printf("Input Point: latitude=%.2f  longitude=%.2f\n",lat,lon);
    printf("Grid Point choosen #%d index=%d latitude=%.2f longitude=%.2f distance=%.2f (Km)\n",
        options->latlon_idx+1,(int)options->indexes[options->latlon_idx],
        options->lats[options->latlon_idx],
        options->lons[options->latlon_idx],
        options->distances[options->latlon_idx]);

    if (options->latlon_mask) {
      printf("Mask values:\n");
      for (i=0;i<4;i++) {
      printf("- %d - index=%d latitude=%.2f longitude=%.2f distance=%.2f (Km) value=%.2f\n",
        i+1,(int)options->indexes[i],options->lats[i],options->lons[i],
        options->distances[i],options->mask_values[i]);
      }
    } else {
      printf("Other grid Points\n");
      for (i=0;i<4;i++) {
        printf("- %d - index=%d latitude=%.2f longitude=%.2f distance=%.2f (Km)\n",
          i+1,(int)options->indexes[i],options->lats[i],options->lons[i],
          options->distances[i]);
      }
    }

    if (n) grib_nearest_delete(n);
  }
  return 0;
}