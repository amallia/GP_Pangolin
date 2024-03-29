/*
 * pfor.c
 *
 *  Created on: July 2nd, 2014
 *      Author: qi
 */


#include "pfor.h"
#include "globals.h"
#include "profiling.h"

pf unpack[13] = {unpack2, unpack3, unpack4, unpack5, unpack6, unpack7, unpack8,
                 unpack9, unpack10, unpack12, unpack16, unpack20, unpack32};


long int  number_of_decode = 0;
int cnum[13] = {2,3,4,5,6,7,8,9,10,12,16,20,32};
int pack_encode(unsigned int **w, unsigned int *p, int num){
  int i, l, n, bb, t, s;
  unsigned int m;
  int b = cnum[num];
  int start;
  unsigned int out[CONSTS::BS];
  unsigned int ex[CONSTS::BS];

  if (b == 32)
  {
    for (i = 0; i < CONSTS::BS; i++)  (*w)[i] = p[i];
    *w += CONSTS::BS;
    return ((num<<12) + (2<<10) + CONSTS::BS);
  }

  for (m = 0, i = 0; i < CONSTS::BS; i++)  if (p[i] > m)  m = p[i];
  if (m < 256)
  {
    bb = 8;
    t = 0;
  }
  else if (m < 65536)
    {
      bb = 16;
      t = 1;
    }
    else
    {
      bb = 32;
      t = 2;
    }

  for (start = 0, n = 0, l = -1, i = 0; i < CONSTS::BS; i++)
  {
    if ((p[i] >= (1<<b)) || ((l >= 0) && (i-l == (1<<b))))
    {
      if (l < 0)
        start = i;
      else
        out[l] = i - l - 1;

      ex[n++] = p[i];
      l = i;
    }
    else
      out[i] = p[i];
  }

  if (l >= 0)
    out[l] = (1<<b)-1;
  else
    start = CONSTS::BS;

  if ((double)(n) <= CONSTS::FRAC * (double)(CONSTS::BS))
  {
    s = ((b * CONSTS::BS)>>5);
    for (i = 0; i < s; i++)  (*w)[i] = 0;
    pack(out, b, CONSTS::BS, *w);
    *w += s;

    s = ((bb * n)>>5) + ((((bb * n) & 31) > 0)? 1:0);
    for (i = 0; i < s; i++)  (*w)[i] = 0;
    pack(ex, bb, n, *w);
    *w += s;

    return ((num<<12) + (t<<10) + start);
  }
  else
    return(-1);
}

unsigned int *pack_decode(unsigned int *_p, unsigned int *_w, int flag) {
 // profiler::getInstance().start(CONSTS::DECODE);
  int pref=0;
  int base=0;
  number_of_decode ++;

  int i, s;
  unsigned int x;
  unsigned int b = cnum[(flag>>12) & 15];
  unsigned int t = (flag>>10) & 3;
  unsigned start = flag & 1023;
  void prefix();
  void prefix2();
  void prefix3();

  (unpack[(flag>>12) & 15])(_p, _w);
  _w += ((b * CONSTS::BS)>>5);

  switch (t)
  {
    case(0):
      for (s = start, i = 0; s < CONSTS::BS; i++)
      {
        x = _p[s]+1;
        _p[s] = (_w[i>>2]>>(24-((i&3)<<3)))&255;
        s += x;
      }
      _w += (i>>2);
      if ((i&3) > 0)  _w++;
      break;

    case(1):
      for (s = start, i = 0; s < CONSTS::BS; i++)
      {
        x = _p[s]+1;
        _p[s] = (_w[i>>1]>>(16-((i&1)<<4)))&65535;
        s += x;
      }
      _w += (i>>1);
      if ((i&1) > 0)  _w++;
      break;

    case(2):
      for (s = start, i=0; s < CONSTS::BS; i++)
      {
        x = _p[s]+1;
        _p[s] = _w[i];
        s += x;
      }
      _w += i;
      break;
  }

  if (pref > 0)
  {
#if (BLOCKDIFF==0)&&(ACCESS>0)
    prefix(_p, base);
#else
    _p[0] += base;
#endif
#if BLOCKDIFF==1
    _p[S-1] += base;
    for (i = 2*S-1; i < CONSTS::BS; i += S)
      _p[i] += _p[i-S];
#endif
  }
 // profiler::getInstance().end(CONSTS::DECODE);
  return(_w);
}

unsigned int *pack_decode(unsigned int *_p, unsigned int *_w, int flag,int base) {
  //profiler::getInstance().start(CONSTS::DECODE);
  number_of_decode ++;
  int pref = 1;
  int i, s;
  unsigned int x;
  unsigned int b = cnum[(flag>>12) & 15];
  unsigned int t = (flag>>10) & 3;
  unsigned start = flag & 1023;
  void prefix();
  void prefix2();
  void prefix3();

  (unpack[(flag>>12) & 15])(_p, _w);
  _w += ((b * CONSTS::BS)>>5);

  switch (t)
  {
    case(0):
      for (s = start, i = 0; s < CONSTS::BS; i++)
      {
        x = _p[s]+1;
        _p[s] = (_w[i>>2]>>(24-((i&3)<<3)))&255;
        s += x;
      }
      _w += (i>>2);
      if ((i&3) > 0)  _w++;
      break;

    case(1):
      for (s = start, i = 0; s < CONSTS::BS; i++)
      {
        x = _p[s]+1;
        _p[s] = (_w[i>>1]>>(16-((i&1)<<4)))&65535;
        s += x;
      }
      _w += (i>>1);
      if ((i&1) > 0)  _w++;
      break;

    case(2):
      for (s = start, i=0; s < CONSTS::BS; i++)
      {
        x = _p[s]+1;
        _p[s] = _w[i];
        s += x;
      }
      _w += i;
      break;
  }

  if (pref > 0)
  {
#if (BLOCKDIFF==0)&&(ACCESS>0)
    prefix(_p, base);
#else
    _p[0] += base;
#endif
#if BLOCKDIFF==1
    _p[S-1] += base;
    for (i = 2*S-1; i < CONSTS::BS; i += S)
      _p[i] += _p[i-S];
#endif
  }
  //profiler::getInstance().end(CONSTS::DECODE);
  return(_w);
}


inline void pack(unsigned int *v, unsigned int b, unsigned int n, unsigned int *w)

{
  int i, bp, wp, s;

  for (bp = 0, i = 0; i < n; i++, bp += b)
  {
    wp = bp>>5;
    s = 32 - b - (bp & 31);
    if (s >= 0)
      w[wp] |= (v[i]<<s);
    else
    {
      s = -s;
      w[wp] |= (v[i]>>s);
      w[wp+1] = (v[i]<<(32-s));
    }
  }
}


inline void unpack2(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 2)
  {
    p[0] = (w[0] >> 30);
    p[1] = (w[0] >> 28) & 3;
    p[2] = (w[0] >> 26) & 3;
    p[3] = (w[0] >> 24) & 3;
    p[4] = (w[0] >> 22) & 3;
    p[5] = (w[0] >> 20) & 3;
    p[6] = (w[0] >> 18) & 3;
    p[7] = (w[0] >> 16) & 3;
    p[8] = (w[0] >> 14) & 3;
    p[9] = (w[0] >> 12) & 3;
    p[10] = (w[0] >> 10) & 3;
    p[11] = (w[0] >> 8) & 3;
    p[12] = (w[0] >> 6) & 3;
    p[13] = (w[0] >> 4) & 3;
    p[14] = (w[0] >> 2) & 3;
    p[15] = (w[0]) & 3;
    p[16] = (w[1] >> 30);
    p[17] = (w[1] >> 28) & 3;
    p[18] = (w[1] >> 26) & 3;
    p[19] = (w[1] >> 24) & 3;
    p[20] = (w[1] >> 22) & 3;
    p[21] = (w[1] >> 20) & 3;
    p[22] = (w[1] >> 18) & 3;
    p[23] = (w[1] >> 16) & 3;
    p[24] = (w[1] >> 14) & 3;
    p[25] = (w[1] >> 12) & 3;
    p[26] = (w[1] >> 10) & 3;
    p[27] = (w[1] >> 8) & 3;
    p[28] = (w[1] >> 6) & 3;
    p[29] = (w[1] >> 4) & 3;
    p[30] = (w[1] >> 2) & 3;
    p[31] = (w[1]) & 3;
  }
}


inline void unpack3(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 3)
  {
    p[0] = (w[0] >> 29);
    p[1] = (w[0] >> 26) & 7;
    p[2] = (w[0] >> 23) & 7;
    p[3] = (w[0] >> 20) & 7;
    p[4] = (w[0] >> 17) & 7;
    p[5] = (w[0] >> 14) & 7;
    p[6] = (w[0] >> 11) & 7;
    p[7] = (w[0] >> 8) & 7;
    p[8] = (w[0] >> 5) & 7;
    p[9] = (w[0] >> 2) & 7;
    p[10] = (w[0] << 1) & 7;
    p[10] |= (w[1] >> 31);
    p[11] = (w[1] >> 28) & 7;
    p[12] = (w[1] >> 25) & 7;
    p[13] = (w[1] >> 22) & 7;
    p[14] = (w[1] >> 19) & 7;
    p[15] = (w[1] >> 16) & 7;
    p[16] = (w[1] >> 13) & 7;
    p[17] = (w[1] >> 10) & 7;
    p[18] = (w[1] >> 7) & 7;
    p[19] = (w[1] >> 4) & 7;
    p[20] = (w[1] >> 1) & 7;
    p[21] = (w[1] << 2) & 7;
    p[21] |= (w[2] >> 30);
    p[22] = (w[2] >> 27) & 7;
    p[23] = (w[2] >> 24) & 7;
    p[24] = (w[2] >> 21) & 7;
    p[25] = (w[2] >> 18) & 7;
    p[26] = (w[2] >> 15) & 7;
    p[27] = (w[2] >> 12) & 7;
    p[28] = (w[2] >> 9) & 7;
    p[29] = (w[2] >> 6) & 7;
    p[30] = (w[2] >> 3) & 7;
    p[31] = (w[2]) & 7;
  }
}


inline void unpack4(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 4)
  {
    p[0] = (w[0] >> 28);
    p[1] = (w[0] >> 24) & 15;
    p[2] = (w[0] >> 20) & 15;
    p[3] = (w[0] >> 16) & 15;
    p[4] = (w[0] >> 12) & 15;
    p[5] = (w[0] >> 8) & 15;
    p[6] = (w[0] >> 4) & 15;
    p[7] = (w[0]) & 15;
    p[8] = (w[1] >> 28);
    p[9] = (w[1] >> 24) & 15;
    p[10] = (w[1] >> 20) & 15;
    p[11] = (w[1] >> 16) & 15;
    p[12] = (w[1] >> 12) & 15;
    p[13] = (w[1] >> 8) & 15;
    p[14] = (w[1] >> 4) & 15;
    p[15] = (w[1]) & 15;
    p[16] = (w[2] >> 28);
    p[17] = (w[2] >> 24) & 15;
    p[18] = (w[2] >> 20) & 15;
    p[19] = (w[2] >> 16) & 15;
    p[20] = (w[2] >> 12) & 15;
    p[21] = (w[2] >> 8) & 15;
    p[22] = (w[2] >> 4) & 15;
    p[23] = (w[2]) & 15;
    p[24] = (w[3] >> 28);
    p[25] = (w[3] >> 24) & 15;
    p[26] = (w[3] >> 20) & 15;
    p[27] = (w[3] >> 16) & 15;
    p[28] = (w[3] >> 12) & 15;
    p[29] = (w[3] >> 8) & 15;
    p[30] = (w[3] >> 4) & 15;
    p[31] = (w[3]) & 15;
  }
}


inline void unpack5(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 5)
  {
    p[0] = (w[0] >> 27);
    p[1] = (w[0] >> 22) & 31;
    p[2] = (w[0] >> 17) & 31;
    p[3] = (w[0] >> 12) & 31;
    p[4] = (w[0] >> 7) & 31;
    p[5] = (w[0] >> 2) & 31;
    p[6] = (w[0] << 3) & 31;
    p[6] |= (w[1] >> 29);
    p[7] = (w[1] >> 24) & 31;
    p[8] = (w[1] >> 19) & 31;
    p[9] = (w[1] >> 14) & 31;
    p[10] = (w[1] >> 9) & 31;
    p[11] = (w[1] >> 4) & 31;
    p[12] = (w[1] << 1) & 31;
    p[12] |= (w[2] >> 31);
    p[13] = (w[2] >> 26) & 31;
    p[14] = (w[2] >> 21) & 31;
    p[15] = (w[2] >> 16) & 31;
    p[16] = (w[2] >> 11) & 31;
    p[17] = (w[2] >> 6) & 31;
    p[18] = (w[2] >> 1) & 31;
    p[19] = (w[2] << 4) & 31;
    p[19] |= (w[3] >> 28);
    p[20] = (w[3] >> 23) & 31;
    p[21] = (w[3] >> 18) & 31;
    p[22] = (w[3] >> 13) & 31;
    p[23] = (w[3] >> 8) & 31;
    p[24] = (w[3] >> 3) & 31;
    p[25] = (w[3] << 2) & 31;
    p[25] |= (w[4] >> 30);
    p[26] = (w[4] >> 25) & 31;
    p[27] = (w[4] >> 20) & 31;
    p[28] = (w[4] >> 15) & 31;
    p[29] = (w[4] >> 10) & 31;
    p[30] = (w[4] >> 5) & 31;
    p[31] = (w[4]) & 31;
  }
}


inline void unpack6(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 6)
  {
    p[0] = (w[0] >> 26);
    p[1] = (w[0] >> 20) & 63;
    p[2] = (w[0] >> 14) & 63;
    p[3] = (w[0] >> 8) & 63;
    p[4] = (w[0] >> 2) & 63;
    p[5] = (w[0] << 4) & 63;
    p[5] |= (w[1] >> 28);
    p[6] = (w[1] >> 22) & 63;
    p[7] = (w[1] >> 16) & 63;
    p[8] = (w[1] >> 10) & 63;
    p[9] = (w[1] >> 4) & 63;
    p[10] = (w[1] << 2) & 63;
    p[10] |= (w[2] >> 30);
    p[11] = (w[2] >> 24) & 63;
    p[12] = (w[2] >> 18) & 63;
    p[13] = (w[2] >> 12) & 63;
    p[14] = (w[2] >> 6) & 63;
    p[15] = (w[2]) & 63;
    p[16] = (w[3] >> 26);
    p[17] = (w[3] >> 20) & 63;
    p[18] = (w[3] >> 14) & 63;
    p[19] = (w[3] >> 8) & 63;
    p[20] = (w[3] >> 2) & 63;
    p[21] = (w[3] << 4) & 63;
    p[21] |= (w[4] >> 28);
    p[22] = (w[4] >> 22) & 63;
    p[23] = (w[4] >> 16) & 63;
    p[24] = (w[4] >> 10) & 63;
    p[25] = (w[4] >> 4) & 63;
    p[26] = (w[4] << 2) & 63;
    p[26] |= (w[5] >> 30);
    p[27] = (w[5] >> 24) & 63;
    p[28] = (w[5] >> 18) & 63;
    p[29] = (w[5] >> 12) & 63;
    p[30] = (w[5] >> 6) & 63;
    p[31] = (w[5]) & 63;
  }
}


inline void unpack7(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 7)
  {
    p[0] = (w[0] >> 25);
    p[1] = (w[0] >> 18) & 127;
    p[2] = (w[0] >> 11) & 127;
    p[3] = (w[0] >> 4) & 127;
    p[4] = (w[0] << 3) & 127;
    p[4] |= (w[1] >> 29);
    p[5] = (w[1] >> 22) & 127;
    p[6] = (w[1] >> 15) & 127;
    p[7] = (w[1] >> 8) & 127;
    p[8] = (w[1] >> 1) & 127;
    p[9] = (w[1] << 6) & 127;
    p[9] |= (w[2] >> 26);
    p[10] = (w[2] >> 19) & 127;
    p[11] = (w[2] >> 12) & 127;
    p[12] = (w[2] >> 5) & 127;
    p[13] = (w[2] << 2) & 127;
    p[13] |= (w[3] >> 30);
    p[14] = (w[3] >> 23) & 127;
    p[15] = (w[3] >> 16) & 127;
    p[16] = (w[3] >> 9) & 127;
    p[17] = (w[3] >> 2) & 127;
    p[18] = (w[3] << 5) & 127;
    p[18] |= (w[4] >> 27);
    p[19] = (w[4] >> 20) & 127;
    p[20] = (w[4] >> 13) & 127;
    p[21] = (w[4] >> 6) & 127;
    p[22] = (w[4] << 1) & 127;
    p[22] |= (w[5] >> 31);
    p[23] = (w[5] >> 24) & 127;
    p[24] = (w[5] >> 17) & 127;
    p[25] = (w[5] >> 10) & 127;
    p[26] = (w[5] >> 3) & 127;
    p[27] = (w[5] << 4) & 127;
    p[27] |= (w[6] >> 28);
    p[28] = (w[6] >> 21) & 127;
    p[29] = (w[6] >> 14) & 127;
    p[30] = (w[6] >> 7) & 127;
    p[31] = (w[6]) & 127;
  }
}


inline void unpack8(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 8)
  {
    p[0] = (w[0] >> 24);
    p[1] = (w[0] >> 16) & 255;
    p[2] = (w[0] >> 8) & 255;
    p[3] = (w[0]) & 255;
    p[4] = (w[1] >> 24);
    p[5] = (w[1] >> 16) & 255;
    p[6] = (w[1] >> 8) & 255;
    p[7] = (w[1]) & 255;
    p[8] = (w[2] >> 24);
    p[9] = (w[2] >> 16) & 255;
    p[10] = (w[2] >> 8) & 255;
    p[11] = (w[2]) & 255;
    p[12] = (w[3] >> 24);
    p[13] = (w[3] >> 16) & 255;
    p[14] = (w[3] >> 8) & 255;
    p[15] = (w[3]) & 255;
    p[16] = (w[4] >> 24);
    p[17] = (w[4] >> 16) & 255;
    p[18] = (w[4] >> 8) & 255;
    p[19] = (w[4]) & 255;
    p[20] = (w[5] >> 24);
    p[21] = (w[5] >> 16) & 255;
    p[22] = (w[5] >> 8) & 255;
    p[23] = (w[5]) & 255;
    p[24] = (w[6] >> 24);
    p[25] = (w[6] >> 16) & 255;
    p[26] = (w[6] >> 8) & 255;
    p[27] = (w[6]) & 255;
    p[28] = (w[7] >> 24);
    p[29] = (w[7] >> 16) & 255;
    p[30] = (w[7] >> 8) & 255;
    p[31] = (w[7]) & 255;
  }
}


inline void unpack9(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 9)
  {
    p[0] = (w[0] >> 23);
    p[1] = (w[0] >> 14) & 511;
    p[2] = (w[0] >> 5) & 511;
    p[3] = (w[0] << 4) & 511;
    p[3] |= (w[1] >> 28);
    p[4] = (w[1] >> 19) & 511;
    p[5] = (w[1] >> 10) & 511;
    p[6] = (w[1] >> 1) & 511;
    p[7] = (w[1] << 8) & 511;
    p[7] |= (w[2] >> 24);
    p[8] = (w[2] >> 15) & 511;
    p[9] = (w[2] >> 6) & 511;
    p[10] = (w[2] << 3) & 511;
    p[10] |= (w[3] >> 29);
    p[11] = (w[3] >> 20) & 511;
    p[12] = (w[3] >> 11) & 511;
    p[13] = (w[3] >> 2) & 511;
    p[14] = (w[3] << 7) & 511;
    p[14] |= (w[4] >> 25);
    p[15] = (w[4] >> 16) & 511;
    p[16] = (w[4] >> 7) & 511;
    p[17] = (w[4] << 2) & 511;
    p[17] |= (w[5] >> 30);
    p[18] = (w[5] >> 21) & 511;
    p[19] = (w[5] >> 12) & 511;
    p[20] = (w[5] >> 3) & 511;
    p[21] = (w[5] << 6) & 511;
    p[21] |= (w[6] >> 26);
    p[22] = (w[6] >> 17) & 511;
    p[23] = (w[6] >> 8) & 511;
    p[24] = (w[6] << 1) & 511;
    p[24] |= (w[7] >> 31);
    p[25] = (w[7] >> 22) & 511;
    p[26] = (w[7] >> 13) & 511;
    p[27] = (w[7] >> 4) & 511;
    p[28] = (w[7] << 5) & 511;
    p[28] |= (w[8] >> 27);
    p[29] = (w[8] >> 18) & 511;
    p[30] = (w[8] >> 9) & 511;
    p[31] = (w[8]) & 511;
  }
}


inline void unpack10(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 10)
  {
    p[0] = (w[0] >> 22);
    p[1] = (w[0] >> 12) & 1023;
    p[2] = (w[0] >> 2) & 1023;
    p[3] = (w[0] << 8) & 1023;
    p[3] |= (w[1] >> 24);
    p[4] = (w[1] >> 14) & 1023;
    p[5] = (w[1] >> 4) & 1023;
    p[6] = (w[1] << 6) & 1023;
    p[6] |= (w[2] >> 26);
    p[7] = (w[2] >> 16) & 1023;
    p[8] = (w[2] >> 6) & 1023;
    p[9] = (w[2] << 4) & 1023;
    p[9] |= (w[3] >> 28);
    p[10] = (w[3] >> 18) & 1023;
    p[11] = (w[3] >> 8) & 1023;
    p[12] = (w[3] << 2) & 1023;
    p[12] |= (w[4] >> 30);
    p[13] = (w[4] >> 20) & 1023;
    p[14] = (w[4] >> 10) & 1023;
    p[15] = (w[4]) & 1023;
    p[16] = (w[5] >> 22);
    p[17] = (w[5] >> 12) & 1023;
    p[18] = (w[5] >> 2) & 1023;
    p[19] = (w[5] << 8) & 1023;
    p[19] |= (w[6] >> 24);
    p[20] = (w[6] >> 14) & 1023;
    p[21] = (w[6] >> 4) & 1023;
    p[22] = (w[6] << 6) & 1023;
    p[22] |= (w[7] >> 26);
    p[23] = (w[7] >> 16) & 1023;
    p[24] = (w[7] >> 6) & 1023;
    p[25] = (w[7] << 4) & 1023;
    p[25] |= (w[8] >> 28);
    p[26] = (w[8] >> 18) & 1023;
    p[27] = (w[8] >> 8) & 1023;
    p[28] = (w[8] << 2) & 1023;
    p[28] |= (w[9] >> 30);
    p[29] = (w[9] >> 20) & 1023;
    p[30] = (w[9] >> 10) & 1023;
    p[31] = (w[9]) & 1023;
  }
}


inline void unpack12(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 12)
  {
    p[0] = (w[0] >> 20);
    p[1] = (w[0] >> 8) & 4095;
    p[2] = (w[0] << 4) & 4095;
    p[2] |= (w[1] >> 28);
    p[3] = (w[1] >> 16) & 4095;
    p[4] = (w[1] >> 4) & 4095;
    p[5] = (w[1] << 8) & 4095;
    p[5] |= (w[2] >> 24);
    p[6] = (w[2] >> 12) & 4095;
    p[7] = (w[2]) & 4095;
    p[8] = (w[3] >> 20);
    p[9] = (w[3] >> 8) & 4095;
    p[10] = (w[3] << 4) & 4095;
    p[10] |= (w[4] >> 28);
    p[11] = (w[4] >> 16) & 4095;
    p[12] = (w[4] >> 4) & 4095;
    p[13] = (w[4] << 8) & 4095;
    p[13] |= (w[5] >> 24);
    p[14] = (w[5] >> 12) & 4095;
    p[15] = (w[5]) & 4095;
    p[16] = (w[6] >> 20);
    p[17] = (w[6] >> 8) & 4095;
    p[18] = (w[6] << 4) & 4095;
    p[18] |= (w[7] >> 28);
    p[19] = (w[7] >> 16) & 4095;
    p[20] = (w[7] >> 4) & 4095;
    p[21] = (w[7] << 8) & 4095;
    p[21] |= (w[8] >> 24);
    p[22] = (w[8] >> 12) & 4095;
    p[23] = (w[8]) & 4095;
    p[24] = (w[9] >> 20);
    p[25] = (w[9] >> 8) & 4095;
    p[26] = (w[9] << 4) & 4095;
    p[26] |= (w[10] >> 28);
    p[27] = (w[10] >> 16) & 4095;
    p[28] = (w[10] >> 4) & 4095;
    p[29] = (w[10] << 8) & 4095;
    p[29] |= (w[11] >> 24);
    p[30] = (w[11] >> 12) & 4095;
    p[31] = (w[11]) & 4095;
  }
}


inline void unpack16(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 16)
  {
    p[0] = (w[0] >> 16);
    p[1] = (w[0]) & 65535;
    p[2] = (w[1] >> 16);
    p[3] = (w[1]) & 65535;
    p[4] = (w[2] >> 16);
    p[5] = (w[2]) & 65535;
    p[6] = (w[3] >> 16);
    p[7] = (w[3]) & 65535;
    p[8] = (w[4] >> 16);
    p[9] = (w[4]) & 65535;
    p[10] = (w[5] >> 16);
    p[11] = (w[5]) & 65535;
    p[12] = (w[6] >> 16);
    p[13] = (w[6]) & 65535;
    p[14] = (w[7] >> 16);
    p[15] = (w[7]) & 65535;
    p[16] = (w[8] >> 16);
    p[17] = (w[8]) & 65535;
    p[18] = (w[9] >> 16);
    p[19] = (w[9]) & 65535;
    p[20] = (w[10] >> 16);
    p[21] = (w[10]) & 65535;
    p[22] = (w[11] >> 16);
    p[23] = (w[11]) & 65535;
    p[24] = (w[12] >> 16);
    p[25] = (w[12]) & 65535;
    p[26] = (w[13] >> 16);
    p[27] = (w[13]) & 65535;
    p[28] = (w[14] >> 16);
    p[29] = (w[14]) & 65535;
    p[30] = (w[15] >> 16);
    p[31] = (w[15]) & 65535;
  }
}


inline void unpack20(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 20)
  {
    p[0] = (w[0] >> 12);
    p[1] = (w[0] << 8) & ((1<<20)-1);
    p[1] |= (w[1] >> 24);
    p[2] = (w[1] >> 4) & ((1<<20)-1);
    p[3] = (w[1] << 16) & ((1<<20)-1);
    p[3] |= (w[2] >> 16);
    p[4] = (w[2] << 4) & ((1<<20)-1);
    p[4] |= (w[3] >> 28);
    p[5] = (w[3] >> 8) & ((1<<20)-1);
    p[6] = (w[3] << 12) & ((1<<20)-1);
    p[6] |= (w[4] >> 20);
    p[7] = (w[4]) & ((1<<20)-1);
    p[8] = (w[5] >> 12);
    p[9] = (w[5] << 8) & ((1<<20)-1);
    p[9] |= (w[6] >> 24);
    p[10] = (w[6] >> 4) & ((1<<20)-1);
    p[11] = (w[6] << 16) & ((1<<20)-1);
    p[11] |= (w[7] >> 16);
    p[12] = (w[7] << 4) & ((1<<20)-1);
    p[12] |= (w[8] >> 28);
    p[13] = (w[8] >> 8) & ((1<<20)-1);
    p[14] = (w[8] << 12) & ((1<<20)-1);
    p[14] |= (w[9] >> 20);
    p[15] = (w[9]) & ((1<<20)-1);
    p[16] = (w[10] >> 12);
    p[17] = (w[10] << 8) & ((1<<20)-1);
    p[17] |= (w[11] >> 24);
    p[18] = (w[11] >> 4) & ((1<<20)-1);
    p[19] = (w[11] << 16) & ((1<<20)-1);
    p[19] |= (w[12] >> 16);
    p[20] = (w[12] << 4) & ((1<<20)-1);
    p[20] |= (w[13] >> 28);
    p[21] = (w[13] >> 8) & ((1<<20)-1);
    p[22] = (w[13] << 12) & ((1<<20)-1);
    p[22] |= (w[14] >> 20);
    p[23] = (w[14]) & ((1<<20)-1);
    p[24] = (w[15] >> 12);
    p[25] = (w[15] << 8) & ((1<<20)-1);
    p[25] |= (w[16] >> 24);
    p[26] = (w[16] >> 4) & ((1<<20)-1);
    p[27] = (w[16] << 16) & ((1<<20)-1);
    p[27] |= (w[17] >> 16);
    p[28] = (w[17] << 4) & ((1<<20)-1);
    p[28] |= (w[18] >> 28);
    p[29] = (w[18] >> 8) & ((1<<20)-1);
    p[30] = (w[18] << 12) & ((1<<20)-1);
    p[30] |= (w[19] >> 20);
    p[31] = (w[19]) & ((1<<20)-1);
  }
}


inline void unpack32(unsigned int *p, unsigned int *w)
{
  int i;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32, w += 32)
  {
    p[0] = w[0];
    p[1] = w[1];
    p[2] = w[2];
    p[3] = w[3];
    p[4] = w[4];
    p[5] = w[5];
    p[6] = w[6];
    p[7] = w[7];
    p[8] = w[8];
    p[9] = w[9];
    p[10] = w[10];
    p[11] = w[11];
    p[12] = w[12];
    p[13] = w[13];
    p[14] = w[14];
    p[15] = w[15];
    p[16] = w[16];
    p[17] = w[17];
    p[18] = w[18];
    p[19] = w[19];
    p[20] = w[20];
    p[21] = w[21];
    p[22] = w[22];
    p[23] = w[23];
    p[24] = w[24];
    p[25] = w[25];
    p[26] = w[26];
    p[27] = w[27];
    p[28] = w[28];
    p[29] = w[29];
    p[30] = w[30];
    p[31] = w[31];
  }
}


inline void prefix(unsigned int *_p, unsigned int s)

{
  int i;
  unsigned int *p;

  for (i = 0, p = _p; i < CONSTS::BS; i += 32, p += 32)
  {
    p[0] += s;
    p[1] += p[0];
    p[2] += p[1];
    p[3] += p[2];
    p[7] += p[6] + p[5] + p[4];
    p[11] += p[10] + p[9] + p[8];
    p[15] += p[14] + p[13] + p[12];
    p[19] += p[18] + p[17] + p[16];
    p[23] += p[22] + p[21] + p[20];
    p[27] += p[26] + p[25] + p[24];
    p[31] += p[30] + p[29] + p[28];

    p[7] += p[3];
    p[11] += p[7];
    p[15] += p[11];
    p[19] += p[15];
    p[23] += p[19];
    p[27] += p[23];
    s = (p[31] += p[27]);

    p[4] += p[3];
    p[8] += p[7];
    p[12] += p[11];
    p[16] += p[15];
    p[20] += p[19];
    p[24] += p[23];
    p[28] += p[27];
    p[5] += p[4];
    p[9] += p[8];
    p[13] += p[12];
    p[17] += p[16];
    p[21] += p[20];
    p[25] += p[24];
    p[29] += p[28];
    p[6] += p[5];
    p[10] += p[9];
    p[14] += p[13];
    p[18] += p[17];
    p[22] += p[21];
    p[26] += p[25];
    p[30] += p[29];
  }
}


inline void prefix2(unsigned int *p)

{
  int i;
  unsigned int s = 0;

  for (i = 0; i < CONSTS::BS; i += 16, p += 16)
  {
    p[0] += s;
    p[15] += p[14];
    p[13] += p[12];
    p[11] += p[10];
    p[9] += p[8];
    p[7] += p[6];
    p[5] += p[4];
    p[3] += p[2];
    p[1] += p[0];

    p[11] += p[9];
    p[7] += p[5];
    p[3] += p[1];
    p[2] += p[1];

    p[6] += p[5];
    p[12] += p[11];
    p[13] += p[11];

    p[4] += p[3];
    p[5] += p[3];
    p[7] += p[3];

    p[8] += p[7];
    p[9] += p[7];
    p[11] += p[7];

    p[10] += p[9];

    s = (p[15] += p[13]);
    p[14] += p[13];
  }
}


inline void prefix3(unsigned int *p)

{
  int i;
  unsigned int s = 0;

  for (i = 0; i < CONSTS::BS; i += 32, p += 32)
  {
    p[29] += p[28];
    p[27] += p[26];
    p[25] += p[24];
    p[23] += p[22];
    p[21] += p[20];
    p[19] += p[18];
    p[17] += p[16];
    p[15] += p[14];
    p[0] += s;
    p[13] += p[12];
    p[11] += p[10];
    p[15] += p[14];
    p[13] += p[12];
    p[11] += p[10];
    p[9] += p[8];
    p[7] += p[6];
    p[5] += p[4];
    p[3] += p[2];
    p[1] += p[0];

    p[23] += p[21];
    p[19] += p[17];
    p[15] += p[13];
    p[11] += p[9];
    p[7] += p[5];
    p[3] += p[1];
    p[27] += p[25];

    p[7] += p[3];
    p[15] += p[11];
    p[23] += p[19];

    p[15] += p[7];

    p[23] += p[15];

    p[11] += p[7];
    p[19] += p[15];
    p[27] += p[23];

    p[5] += p[3];
    p[9] += p[7];
    p[13] += p[11];
    p[17] += p[15];
    p[21] += p[19];
    p[25] += p[23];
    p[29] += p[27];

    p[2] += p[1];
    p[4] += p[3];
    p[6] += p[5];
    p[8] += p[7];
    p[10] += p[9];
    p[12] += p[11];
    p[14] += p[13];
    p[30] += p[29];
    p[16] += p[15];
    p[18] += p[17];
    p[20] += p[19];
    p[22] += p[21];
    p[24] += p[23];
    p[26] += p[25];
    p[28] += p[27];

    s = (p[31] += p[30]);
  }
}

