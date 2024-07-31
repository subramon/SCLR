#ifndef __SCALAR_H
#define __SCALAR_H
#include "qtypes.h"
//START_FOR_CDEF
#define SCLR_MAX_LEN_NAME 15
typedef struct _sclr_rec_type { 
  char name[SCLR_MAX_LEN_NAME+1]; 
  union { 
    bool bl;

    int8_t  i1;
    int16_t i2;
    int32_t i4;
    int64_t i8;

    uint8_t  ui1;
    uint16_t ui2;
    uint32_t ui4;
    uint64_t ui8;

    bfloat16 f2;
    float f4;
    double f8;
    char *str;
    tm_t tm1; 
  } val;
  qtype_t qtype;

} SCLR_REC_TYPE;
//STOP_FOR_CDEF
#endif
