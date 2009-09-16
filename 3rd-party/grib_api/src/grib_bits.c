/**
* Copyright 2005-2007 ECMWF
*
* Licensed under the GNU Lesser General Public License which
* incorporates the terms and conditions of version 3 of the GNU
* General Public License.
* See LICENSE and gpl-3.0.txt for details.
*/

/***************************************************************************
 *   Enrico Fucile  - 19.06.2007                                           *
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"

#if OMP_PACKING
#include "omp.h"
#endif

#define mask1(i)    (1u << i)
#define test(n,i)    !!((n) & mask1(i))

long GRIB_MASK = -1;       /* Mask of sword bits */

# define VALUE(p,q,b) \
 (((b)==max_nbits ? GRIB_MASK : ~(GRIB_MASK<<(b))) & ((p)>>(max_nbits-((q)+(b)))))

# define MASKVALUE(q,b) \
 ((b)==max_nbits ? GRIB_MASK : (~(GRIB_MASK<<(b))<<(max_nbits-((q)+(b)))))


static unsigned long dmasks[] = { 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00, };

static int max_nbits = sizeof(unsigned long)*8;

unsigned long grib_decode_unsigned_byte_long(const unsigned char* p, long o, int l)
{
  long accum = 0;
  int i = 0;
  unsigned char b = p[o++];

  Assert(l <= max_nbits);

  accum  <<= 8;
  accum |= b ;

  for ( i=1; i<l; i++ )
  {
    b = p[o++];
    accum <<= 8;
    accum |= b ;
  }
  return accum;
}

long grib_decode_signed_long(const unsigned char* p, long o, int l)
{
  long accum = 0;
  int i = 0;
  unsigned char b = p[o++];
  int sign = grib_get_bit(&b, 0);

  Assert(l <= max_nbits);

  b &= 0x7f;
  accum  <<= 8;
  accum |= b ;

  for ( i=1; i<l; i++ )
  {
    b = p[o++];
    accum <<= 8;
    accum |= b ;
  }
  if (sign == 0)
    return accum;
  else
    return -accum;
}

int grib_encode_signed_long(unsigned char* p, long val , long o, int l)
{
  unsigned short accum = 0;
  int i = 0;
  int off = o;
  int sign = (val<0);

  Assert(l <= max_nbits);

  if (sign) val *= -1;

  for (  i=0; i<l; i++ )
  {
    accum    =  val >> (l*8-(8*(i+1)));
    p[o++] =  accum ;
  }

  if (sign) p[off] |= 128;

  return GRIB_SUCCESS;
}

static void grib_set_bit_on( unsigned char* p, long* bitp){

  p +=  *bitp/8;
  *p |=  (1u << (7-((*bitp)%8)));
  (*bitp)++;
}

static void grib_set_bit_off( unsigned char* p, long* bitp){

  p +=  *bitp/8;
  *p &= ~(1u << (7-((*bitp)%8)));
  (*bitp)++;
}

int grib_get_bit(const unsigned char* p, long bitp){
  p += (bitp >> 3);
  return (*p&(1<<(7-(bitp%8))));
}

void grib_set_bit( unsigned char* p, long bitp, int val){

  if(val == 0) grib_set_bit_off(p,&bitp);
  else grib_set_bit_on(p,&bitp);
}

long grib_decode_signed_longb(const unsigned char* p, long *bitp, long nbits)
{
  int sign = grib_get_bit(p, *bitp);
  long val = 0;

  Assert(nbits <= max_nbits);

  *bitp += 1;

  val = grib_decode_unsigned_long( p, bitp,  nbits-1);

  if (sign)val = -val;


  return  val;
}

#if FAST_BIG_ENDIAN

unsigned long grib_decode_unsigned_long(const unsigned char* p, long *bitp, long nbits)
{
  long countOfLeftmostBits=0,leftmostBits=0;
  long startBit= *bitp ;
  long remainingBits = nbits;
  long *pp=(long*)p;
  unsigned long val=0;

  if (startBit >= max_nbits) {
    pp += startBit/max_nbits;
    startBit %= max_nbits;
  }

  countOfLeftmostBits = startBit + remainingBits;
  if (countOfLeftmostBits > max_nbits) {
    countOfLeftmostBits = max_nbits - startBit;
    remainingBits -= countOfLeftmostBits;
    leftmostBits=(VALUE(*pp,startBit,countOfLeftmostBits)) << remainingBits;
    startBit = 0;
    pp++;
  } else
    leftmostBits = 0;

  val=leftmostBits+(VALUE(*pp,startBit,remainingBits));

  *bitp += nbits;

  return val;
}

#if 1
int grib_decode_double_array(const unsigned char* p, long *bitp, long nbits,double reference_value,double s,double d,size_t size,double* val) {
  long i=0;
  long countOfLeftmostBits=0,leftmostBits=0;
  long startBit,startByte;
  long remainingBits = nbits;
  long *pp=(long*)p;
  int inited=0;
  unsigned long uval=0;
  double fact=s*d;
  double bias=reference_value*d;

  startBit=*bitp;
  remainingBits = nbits;
  if (startBit >= max_nbits) {
    pp+=startBit/max_nbits;
    startBit %= max_nbits;
  }

  if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {

    for (i=0;i<size;i++) {
      if (startBit == max_nbits) {
        startBit = 0;
        pp++;
      }

      val[i]=VALUE(*pp,startBit,remainingBits);
      val[i]= val[i] * fact + bias ;
      startBit+=remainingBits;
      remainingBits=nbits;
    }

  } else {
    for (i=0;i<size;i++) {
      countOfLeftmostBits = startBit + remainingBits;
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        remainingBits -= countOfLeftmostBits;
        leftmostBits=(VALUE(*(pp++),startBit,countOfLeftmostBits)) << remainingBits;
        startBit = 0;
      } else
        leftmostBits = 0;

      val[i]=leftmostBits+(VALUE(*pp,startBit,remainingBits));
      val[i]= val[i] * fact + bias ;
      startBit+=remainingBits;
      remainingBits=nbits;
    }

  }

  *bitp+=size*nbits;

  return GRIB_SUCCESS;
}

int grib_decode_double_array_complex(const unsigned char* p, long *bitp, long nbits,double reference_value,double s,double* d,size_t size,double* val) {
  long i=0;
  long countOfLeftmostBits=0,leftmostBits=0;
  long startBit;
  long remainingBits = nbits;
  long *pp=(long*)p;
  int inited=0;
  unsigned long uval=0;

  startBit=*bitp;
  remainingBits = nbits;
  if (startBit >= max_nbits) {
     pp+=startBit/max_nbits;
     startBit %= max_nbits;
  }

  if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {
    for (i=0;i<size;i++) {
      if (startBit == max_nbits) {
        startBit = 0;
        pp++;
      }

      val[i]=VALUE(*pp,startBit,remainingBits);
      val[i]= ((( (val[i]) * s)+reference_value)*d[i/2]);
      startBit+=remainingBits;
      remainingBits=nbits;
    }

  } else {

    for (i=0;i<size;i++) {
      countOfLeftmostBits = startBit + remainingBits;
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        remainingBits -= countOfLeftmostBits;
        leftmostBits=(VALUE(*pp,startBit,countOfLeftmostBits)) << remainingBits;
        startBit = 0;
        pp++;
      } else
        leftmostBits = 0;

      val[i]=leftmostBits+(VALUE(*pp,startBit,remainingBits));
      val[i]= ((( (val[i]) * s)+reference_value)*d[i/2]);
      startBit+=remainingBits;
      remainingBits=nbits;
    }
  }

  *bitp+=size*nbits;

  return GRIB_SUCCESS;
}

#else
int grib_decode_double_array(const unsigned char* p, long *bitp, long nbits,double reference_value,double s,double d,size_t size,double* val) {
  long i=0;
  long countOfLeftmostBits=0,leftmostBits=0;
  long startBit,startByte;
  long remainingBits = nbits;
  long *pp=(long*)p;
  int inited=0;
  unsigned long uval=0;
  double fact=s*d;
  double bias=reference_value*d;

  if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {

#pragma omp parallel for schedule(static) firstprivate(inited,pp) private(startBit,countOfLeftmostBits,remainingBits,leftmostBits)
    for (i=0;i<size;i++) {

      if (!inited) {
        startBit=*bitp+i*nbits;
        remainingBits = nbits;
        if (startBit >= max_nbits) {
          pp+=startBit/max_nbits;
          startBit %= max_nbits;
        }
        inited=1;
      }

      if (startBit == max_nbits) {
        startBit = 0;
        pp++;
      }

      val[i]=VALUE(*pp,startBit,remainingBits);
      val[i]= val[i] * fact + bias ;
      startBit+=remainingBits;
      remainingBits=nbits;
    }

  } else {

#pragma omp parallel for schedule(static) firstprivate(inited,pp) private(startBit,countOfLeftmostBits,remainingBits,leftmostBits)
    for (i=0;i<size;i++) {

      if (!inited) {
        startBit=*bitp+i*nbits;
        remainingBits = nbits;
        if (startBit >= max_nbits) {
          pp+=startBit/max_nbits;
          startBit %= max_nbits;
        }
        inited=1;
      }

      countOfLeftmostBits = startBit + remainingBits;
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        remainingBits -= countOfLeftmostBits;
        leftmostBits=(VALUE(*(pp++),startBit,countOfLeftmostBits)) << remainingBits;
        startBit = 0;
      } else
        leftmostBits = 0;

      val[i]=leftmostBits+(VALUE(*pp,startBit,remainingBits));
      val[i]= val[i] * fact + bias ;
      startBit+=remainingBits;
      remainingBits=nbits;
    }

  }

  *bitp+=size*nbits;

  return GRIB_SUCCESS;
}

int grib_decode_double_array_complex(const unsigned char* p, long *bitp, long nbits,double reference_value,double s,double* d,size_t size,double* val) {
  long i=0;
  long countOfLeftmostBits=0,leftmostBits=0;
  long startBit;
  long remainingBits = nbits;
  long *pp=(long*)p;
  int inited=0;
  unsigned long uval=0;

  if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {

#pragma omp parallel for schedule(static) firstprivate(inited,pp) private(startBit,countOfLeftmostBits,remainingBits,leftmostBits)
    for (i=0;i<size;i++) {

      if (!inited) {
        startBit=*bitp+i*nbits;
        remainingBits = nbits;
        if (startBit >= max_nbits) {
          pp+=startBit/max_nbits;
          startBit %= max_nbits;
        }
        inited=1;
      }

      if (startBit == max_nbits) {
        startBit = 0;
        pp++;
      }

      val[i]=VALUE(*pp,startBit,remainingBits);
      val[i]= ((( (val[i]) * s)+reference_value)*d[i/2]);
      startBit+=remainingBits;
      remainingBits=nbits;
    }

  } else {

#pragma omp parallel for schedule(static) firstprivate(inited,pp) private(startBit,countOfLeftmostBits,remainingBits,leftmostBits)
    for (i=0;i<size;i++) {

      if (!inited) {
        startBit=*bitp+i*nbits;
        remainingBits = nbits;
        if (startBit >= max_nbits) {
          pp+=startBit/max_nbits;
          startBit %= max_nbits;
        }
        inited=1;
      }

      countOfLeftmostBits = startBit + remainingBits;
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        remainingBits -= countOfLeftmostBits;
        leftmostBits=(VALUE(*pp,startBit,countOfLeftmostBits)) << remainingBits;
        startBit = 0;
        pp++;
      } else
        leftmostBits = 0;

      val[i]=leftmostBits+(VALUE(*pp,startBit,remainingBits));
      val[i]= ((( (val[i]) * s)+reference_value)*d[i/2]);
      startBit+=remainingBits;
      remainingBits=nbits;
    }
  }

  *bitp+=size*nbits;

  return GRIB_SUCCESS;
}
#endif


#else

unsigned long grib_decode_unsigned_long(const unsigned char* p, long *bitp, long nbits)
{
  int i;
  long ret = 0;
  long o = *bitp/8;
  int l = nbits/8;

  if (nbits==0) return 0;

  if(nbits > max_nbits)
  {
    int bits = nbits;
    int mod = bits % max_nbits;

    if(mod != 0)
    {
      int e=grib_decode_unsigned_long(p,bitp,mod);
      Assert( e == 0);
      bits -= mod;
    }

    while(bits > max_nbits)
    {
      int e=grib_decode_unsigned_long(p,bitp,max_nbits);
      Assert( e == 0);
      bits -= max_nbits;
    }

    return grib_decode_unsigned_long(p,bitp,bits);
  }

  if((nbits%8 > 0)||(*bitp%8 > 0)) {
    for(i=0; i< nbits;i++){
      ret <<= 1;
      if(grib_get_bit( p, *bitp)) ret += 1;
      *bitp += 1;
    }
    return ret;
  }

  ret  <<= 8;
  ret |= p[o++] ;

  for ( i=1; i<l; i++ )
  {
    ret <<= 8;
    ret |= p[o++] ;
  }
  *bitp += nbits;

  return ret;
}

int grib_decode_double_array(const unsigned char* p, long *bitp, long bitsPerValue,
                             double reference_value,double s,double d,
                             size_t n_vals,double* val) {
  long i=0;
  long lvalue = 0;

  if(bitsPerValue%8)
  {
    int j=0;
    for(i=0;i < n_vals;i++) {
      lvalue=0;
      for(j=0; j< bitsPerValue;j++){
        lvalue <<= 1;
        if(grib_get_bit( p, *bitp)) lvalue += 1;
        *bitp += 1;
      }
      val[i] = (double) (((lvalue*s)+reference_value)*d);
    }
  }  else  {
    int bc;
    int l = bitsPerValue/8;
    size_t o = 0;

    for(i=0;i < n_vals;i++)
    {
      lvalue  = 0;
      lvalue  <<= 8;
      lvalue |= p[o++] ;

      for ( bc=1; bc<l; bc++ )
      {
        lvalue <<= 8;
        lvalue |= p[o++] ;
      }
      val[i] = (double) (((lvalue*s)+reference_value)*d);
    }
  }

  return 0;
}


int grib_decode_double_array_complex(const unsigned char* p, long *bitp, long nbits,double reference_value,double s,double* d,size_t size,double* val) {
  return GRIB_NOT_IMPLEMENTED;
}

#endif


int grib_encode_signed_longb(unsigned char* p,  long val ,long *bitp, long nb)
{
  short  sign = val < 0;

  Assert(nb <= max_nbits);

  if(sign) val = -val;

  if(sign)
    grib_set_bit_on (p, bitp);
  else
    grib_set_bit_off(p, bitp);

  return grib_encode_unsigned_longb(p, val ,bitp, nb-1);
}

#if FAST_BIG_ENDIAN
int grib_encode_unsigned_long(unsigned char* p, unsigned long val ,long *bitp, long nbits)
{
  long* destination = (long*)p;
  long countOfLeftmostBits=0,nextWord=0,startBit=0,remainingBits=0,rightmostBits=0;

  startBit = *bitp;
  remainingBits = nbits;

  if (startBit >= max_nbits) {
    nextWord = startBit / max_nbits;
    startBit %= max_nbits;
  } else
    nextWord = 0;

  countOfLeftmostBits = startBit + remainingBits;
  if (countOfLeftmostBits > max_nbits) {
    countOfLeftmostBits = max_nbits - startBit;
    startBit = max_nbits - remainingBits;
    remainingBits -= countOfLeftmostBits;
    destination[nextWord] =
      ((destination[nextWord] >> countOfLeftmostBits) << countOfLeftmostBits)
      + (VALUE(val,startBit,countOfLeftmostBits));
    startBit = 0;
    nextWord++;
  }

  rightmostBits = VALUE(val,max_nbits-remainingBits,remainingBits);
  destination[nextWord] =
    (destination[nextWord] & ~MASKVALUE(startBit,remainingBits))
    + (rightmostBits << max_nbits-(remainingBits+startBit));

  *bitp+=nbits;
  return GRIB_SUCCESS;
}

int grib_encode_unsigned_longb(unsigned char* p, unsigned long val ,long *bitp, long nbits)
{
  return grib_encode_unsigned_long(p,val ,bitp, nbits);
}

int grib_encode_double_array(size_t n_vals,const double* val,long nbits,double reference_value,double d,double divisor,unsigned char* p,long *bitp)
{
  long* destination = (long*)p;
  double* v=(double*)val;
  long countOfLeftmostBits=0,startBit=0,remainingBits=0,rightmostBits=0;
  unsigned long uval=0;
  size_t i=0;

  startBit=*bitp;
  remainingBits = nbits;

  if (startBit >= max_nbits) {
    destination += startBit / max_nbits;
    startBit %= max_nbits;
  }

  if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {

    for(i=0;i< n_vals;i++){
      uval  = (unsigned long)(((((*v)*d)-reference_value)*divisor)+0.5);
      if (startBit == max_nbits) {
        startBit = 0;
        destination++;
      }

      rightmostBits = VALUE(uval,max_nbits-remainingBits,remainingBits);
      *destination = ((*destination) & ~MASKVALUE(startBit,remainingBits))
        + (rightmostBits << max_nbits-(remainingBits+startBit));
      startBit+=remainingBits;
      remainingBits=nbits;
      v++;
    }

  } else {

    for(i=0;i< n_vals;i++){
      countOfLeftmostBits = startBit + remainingBits;
      uval  = (unsigned long)(((((*v)*d)-reference_value)*divisor)+0.5);
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        startBit = max_nbits - remainingBits;
        remainingBits -= countOfLeftmostBits;
        *destination = (((*destination) >> countOfLeftmostBits) << countOfLeftmostBits)
          + (VALUE(uval,startBit,countOfLeftmostBits));
        startBit = 0;
        destination++;
      }

      rightmostBits = VALUE(uval,max_nbits-remainingBits,remainingBits);
      *destination = ((*destination) & ~MASKVALUE(startBit,remainingBits))
        + (rightmostBits << max_nbits-(remainingBits+startBit));
      startBit+=remainingBits;
      remainingBits=nbits;
      v++;
    }

  }

  *bitp+=n_vals*nbits;

  return GRIB_SUCCESS;
}

int grib_encode_double_array_complex(size_t n_vals,double* val,long nbits,double reference_value,
        double* scal,double d,double divisor,unsigned char* p,long *bitp) {
  long* destination = (long*)p;
  double* v=val;
  long countOfLeftmostBits=0,startBit=0,remainingBits=0,rightmostBits=0;
  unsigned long uval=0;
  size_t i=0;

  startBit=*bitp;
  remainingBits = nbits;

  if (startBit >= max_nbits) {
    destination += startBit / max_nbits;
    startBit %= max_nbits;
  }

  if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {

    for(i=0;i< n_vals;i++) {
      uval  = (unsigned long)(((((*v)*d*scal[i/2])-reference_value)*divisor)+0.5);
      if (startBit == max_nbits) {
        startBit = 0;
        destination++;
      }

      rightmostBits = VALUE(uval,max_nbits-remainingBits,remainingBits);
      *destination = ((*destination) & ~MASKVALUE(startBit,remainingBits))
        + (rightmostBits << max_nbits-(remainingBits+startBit));
      startBit+=remainingBits;
      remainingBits=nbits;

      v++;
    }

  } else {

    for(i=0;i< n_vals;i++) {
      countOfLeftmostBits = startBit + remainingBits;
      uval  = (unsigned long)(((((*v)*d*scal[i/2])-reference_value)*divisor)+0.5);
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        startBit = max_nbits - remainingBits;
        remainingBits -= countOfLeftmostBits;
        *destination = (((*destination) >> countOfLeftmostBits) << countOfLeftmostBits)
          + (VALUE(uval,startBit,countOfLeftmostBits));
        startBit = 0;
        destination++;
      }

      rightmostBits = VALUE(uval,max_nbits-remainingBits,remainingBits);
      *destination = ((*destination) & ~MASKVALUE(startBit,remainingBits))
        + (rightmostBits << max_nbits-(remainingBits+startBit));
      startBit+=remainingBits;
      remainingBits=nbits;

      v++;
    }

  }

  *bitp+=n_vals*nbits;

  return 0;
}
#else

int grib_encode_unsigned_long(unsigned char* p, unsigned long val ,long *bitp, long nbits)
{
  long            len     = nbits;
  int              s      = *bitp%8;
  int              n      = 8-s;

  if(nbits > max_nbits)
  {
    /* TODO: Do some real code here, to support long longs */
    int bits = nbits;
    int mod = bits % max_nbits;
    long zero = 0;

    /* printf("Warning: encoding %ld bits=%ld %ld\n",val,nbits,*bitp); */

    if(mod != 0)
    {
      int e=grib_encode_unsigned_long(p,zero,bitp,mod);
      /* printf(" -> : encoding %ld bits=%ld %ld\n",zero,(long)mod,*bitp); */
      Assert( e == 0);
      bits -= mod;
    }

    while(bits > max_nbits)
    {
      int e=grib_encode_unsigned_long(p,zero,bitp,max_nbits);
      /* printf(" -> : encoding %ld bits=%ld %ld\n",zero,(long)max_nbits,*bitp); */
      Assert(e == 0);
      bits -= max_nbits;
    }

    /* printf(" -> : encoding %ld bits=%ld %ld\n",val,(long)bits,*bitp); */
    return grib_encode_unsigned_long(p,val,bitp,bits);

  }

  if(s)
    p += (*bitp >> 3); /* skip the bytes */
  else
    p += (*bitp >> 3); /* skip the bytes */

  /* head */
  if(s) {
    len   -= n;
    if (len < 0) *p  = ((val << -len) | ((*p)++ & dmasks[n]));
    else *p  = ((val >> len)| ((*p)++ & dmasks[n]));
  }

  /*  write the middle words */
  while(len >= 8)
  {
    len   -= 8;
    *p++ = (val >> len);
  }

  /*  write the end bits */
  if(len) *p =  (val << (8-len));

  *bitp += nbits;
  return GRIB_SUCCESS;
}

int grib_encode_unsigned_longb(unsigned char* p, unsigned long val ,long *bitp, long nb)
{
  long  i = 0;

  Assert(nb <= max_nbits);

  for(i=nb-1; i >= 0; i--){
    if(test(val,i))
      grib_set_bit_on (p, bitp);
    else
      grib_set_bit_off(p, bitp);
  }
  return GRIB_SUCCESS;
}

int grib_encode_double_array(size_t n_vals,const double* val,long bits_per_value,double reference_value,double d,double divisor,unsigned char* p,long *off)
{
  size_t i=0;
  unsigned long unsigned_val=0;
  unsigned char *encoded=p;
  if(bits_per_value%8){
    for(i=0;i< n_vals;i++){
      unsigned_val = (unsigned long)((((val[i]*d)-reference_value)*divisor)+0.5);
      grib_encode_unsigned_longb(encoded, unsigned_val, off , bits_per_value);
    }
  } else{
    for(i=0;i< n_vals;i++){
    int blen=0;
      blen = bits_per_value;
      unsigned_val = (unsigned long)((((val[i]*d)-reference_value)*divisor)+0.5);
      while(blen >= 8)
      {
        blen   -= 8;
        *encoded = (unsigned_val >> blen);
        encoded++;
        *off+=8;
      }
    }
  }
  return GRIB_SUCCESS;
}

int grib_encode_double_array_complex(size_t n_vals,double* val,long nbits,double reference_value,
        double* scal,double d,double divisor,unsigned char* p,long *bitp) {
  return GRIB_NOT_IMPLEMENTED;
}

#endif

