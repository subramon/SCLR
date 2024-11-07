#define LUA_LIB

#include "q_incs.h"

#include "lauxlib.h"
#include "lua.h"
#include "luaconf.h"
#include "lualib.h"

#include "txt_to_BL.h"

#include "txt_to_I1.h"
#include "txt_to_I2.h"
#include "txt_to_I4.h"
#include "txt_to_I8.h"

#include "txt_to_UI1.h"
#include "txt_to_UI2.h"
#include "txt_to_UI4.h"
#include "txt_to_UI8.h"

#include "txt_to_F4.h"
#include "txt_to_F8.h"

#include "cmem_struct.h"
#include "aux_cmem.h"
#include "sclr_struct.h"
#include "aux_scalar.h"

extern int luaopen_libsclr (lua_State *L);

static int l_sclr_to_data( lua_State *L) 
{
  int status = 0;
  SCLR_REC_TYPE *ptr_sclr = NULL;
  if ( lua_gettop(L) != 1 ) { go_BYE(-1); }
  ptr_sclr = (SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  if ( ptr_sclr == NULL ) { go_BYE(-1); }
  void *ptr = (void *)(&(ptr_sclr->val));
  lua_pushlightuserdata(L, ptr);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
static int l_sclr_to_cmem( lua_State *L) 
{
  int status = 0;
  SCLR_REC_TYPE *ptr_sclr = NULL;
  CMEM_REC_TYPE *ptr_cmem = NULL;
  char *cmem_name = NULL;

  int num_args = lua_gettop(L); if ( num_args != 1 )  { go_BYE(-1); }

  ptr_sclr = (SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  if ( ptr_sclr == NULL ) { go_BYE(-1); }

  ptr_cmem = (CMEM_REC_TYPE *)lua_newuserdata(L, sizeof(CMEM_REC_TYPE));
  if ( ptr_cmem == NULL ) { WHEREAMI; goto BYE; }
  memset(ptr_cmem, '\0', sizeof(CMEM_REC_TYPE));

  if ( *ptr_sclr->name == '\0' ) { 
    cmem_name = strdup("Scalar");
  }
  else {
    size_t len = strlen(ptr_sclr->name) + 4 + strlen("Scalar_");
    cmem_name = malloc(len);
    sprintf(cmem_name, "Scalar_%s", ptr_sclr->name);
  }

  size_t size = sizeof(SCLR_REC_TYPE);
  status = cmem_malloc(ptr_cmem, size, ptr_sclr->qtype, "Scalar");
  cBYE(status);
  memcpy(ptr_cmem->data, &(ptr_sclr->val), size);
  ptr_cmem->is_foreign = false;

  luaL_getmetatable(L, "CMEM"); /* Add the metatable to the stack. */
  lua_setmetatable(L, -2); /* Set the metatable on the userdata. */
  free_if_non_null(cmem_name);
  return 1;
BYE:
  free_if_non_null(cmem_name);
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}

#define OP_BUF_LEN 4095
#define BUF_LEN 63
static int l_sclr_reincarnate(lua_State *L) {
  int status = 0;
  char op_str_buf[OP_BUF_LEN+1]; // TODO P3 try not to hard code bound
  char  buf[BUF_LEN+1];          //  TODO P3 try not to hard code bound

  memset(op_str_buf, '\0', OP_BUF_LEN+1);
  memset(buf,        '\0', BUF_LEN+1);
  SCLR_REC_TYPE *ptr_sclr=(SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  qtype_t qtype = ptr_sclr->qtype;

  strncpy(op_str_buf, "Scalar.new(", OP_BUF_LEN);
  switch ( qtype ) { 
    case BL : 
    snprintf(buf, BUF_LEN, "%s", ptr_sclr->val.bl ? "true" : "false");
    break;
    case I1 : 
    snprintf(buf, BUF_LEN, "%" PRI1, ptr_sclr->val.i1);
    break;
    case I2 : 
    snprintf(buf, BUF_LEN, "%" PRI2, ptr_sclr->val.i2);
    break;
    case I4 : 
    snprintf(buf, BUF_LEN, "%" PRI4, ptr_sclr->val.i4);
    break;
    case I8 : 
    snprintf(buf, BUF_LEN, "%" PRI8, ptr_sclr->val.i8);
    break;
    case F4 : 
    snprintf(buf, BUF_LEN, "%" PRF4, ptr_sclr->val.f4);
    break;
    case F8 : 
    snprintf(buf, BUF_LEN, "%" PRF8, ptr_sclr->val.f8);
    break;
    default : 
    go_BYE(-1);
    break;
  }
  strncat(op_str_buf, buf, OP_BUF_LEN);

  strncat(op_str_buf, ", '", OP_BUF_LEN);

  strncat(op_str_buf, get_str_qtype(qtype), OP_BUF_LEN);

  strncat(op_str_buf, "')", OP_BUF_LEN);

  lua_pushstring(L, op_str_buf);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 2;
}

static int l_sclr_to_bool( lua_State *L) {
  int status = 0;
  if ( lua_gettop(L) < 1 ) { WHEREAMI; goto BYE; }
  SCLR_REC_TYPE *ptr_sclr=(SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  qtype_t qtype = ptr_sclr->qtype;
  if ( qtype != BL ) { go_BYE(-1); } 
  lua_pushboolean(L, ptr_sclr->val.bl); 
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
static int l_sclr_to_num( lua_State *L) {
  int status = 0;
  if ( lua_gettop(L) < 1 ) { WHEREAMI; goto BYE; }
  SCLR_REC_TYPE *ptr_sclr=(SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  qtype_t qtype = ptr_sclr->qtype;
  switch ( qtype ) { 
    case I1 : lua_pushnumber(L, ptr_sclr->val.i1); break;
    case I2 : lua_pushnumber(L, ptr_sclr->val.i2); break;
    case I4 : lua_pushnumber(L, ptr_sclr->val.i4); break;
    case I8 : lua_pushnumber(L, ptr_sclr->val.i8); break;

    case UI1 : lua_pushnumber(L, ptr_sclr->val.ui1); break;
    case UI2 : lua_pushnumber(L, ptr_sclr->val.ui2); break;
    case UI4 : lua_pushnumber(L, ptr_sclr->val.ui4); break;
    case UI8 : lua_pushnumber(L, ptr_sclr->val.ui8); break;

    case F4 : lua_pushnumber(L, ptr_sclr->val.f4); break;
    case F8 : lua_pushnumber(L, ptr_sclr->val.f8); break;

    default : go_BYE(-1); break;
  }
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}

static int l_qtype(lua_State *L) {
  if ( lua_gettop(L) != 1 ) { WHEREAMI; goto BYE; }
  SCLR_REC_TYPE *ptr_sclr=(SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  const char *cptr = get_str_qtype(ptr_sclr->qtype);
  lua_pushstring(L, cptr);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}

static int l_sclr_to_str( lua_State *L) {
  int status = 0;
#define BUFLEN 127
  char buf[BUFLEN+1];
  int nw = 0;

  if ( lua_gettop(L) < 1 ) { WHEREAMI; goto BYE; }
  SCLR_REC_TYPE *ptr_sclr=(SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  // TODO Allow user to provide format
  memset(buf, '\0', BUFLEN+1);
  qtype_t qtype = ptr_sclr->qtype;
  switch ( qtype ) { 
    case BL : 
      nw = snprintf(buf, BUFLEN, "%s", ptr_sclr->val.bl ? "true" : "false");
      break;
    case I1 : nw = snprintf(buf, BUFLEN, "%d", ptr_sclr->val.i1); break;
    case I2 : nw = snprintf(buf, BUFLEN, "%d", ptr_sclr->val.i2); break;
    case I4 : nw = snprintf(buf, BUFLEN, "%d", ptr_sclr->val.i4); break;
    case I8 : nw = snprintf(buf, BUFLEN, "%" PRId64, ptr_sclr->val.i8); break;
    //------------------------------------------
    case UI1 : nw = snprintf(buf, BUFLEN, "%u", ptr_sclr->val.i1); break;
    case UI2 : nw = snprintf(buf, BUFLEN, "%u", ptr_sclr->val.i2); break;
    case UI4 : nw = snprintf(buf, BUFLEN, "%u", ptr_sclr->val.i4); break;
    case UI8 : nw = snprintf(buf, BUFLEN, "%" PRIu64, ptr_sclr->val.i8); break;
    //------------------------------------------
    case F2 : 
               {
                 float ftmp = F2_to_F4(ptr_sclr->val.f2);
                 nw = snprintf(buf, BUFLEN, "%f", ftmp); 
               }
               break;
    case F4 : nw = snprintf(buf, BUFLEN, "%f", ptr_sclr->val.f4); break;
    case F8 : nw = snprintf(buf, BUFLEN, "%lf", ptr_sclr->val.f8); break;
    //------------------------------------------
    case SC : {
                if ( ptr_sclr->val.str == NULL ) { go_BYE(-1); }
                lua_pushstring(L, ptr_sclr->val.str);
                return 1;
              } 
    default : go_BYE(-1); break;
  }
  if ( ( nw <= 0 ) || ( nw >= BUFLEN ) )  { go_BYE(-1); }
  lua_pushstring(L, buf);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}

#define mcr_chk_int(x) { if ( ceil(x) != floor(x) ) { go_BYE(-1); } }
#define mcr_chk_range_set(x, y, lb, ub) { \
      if ( ( x < lb ) || ( x > ub ) ) { go_BYE(-1); } \
       y = x;  \
}

static int l_sclr_set_name( lua_State *L) {
  int status = 0;

  if ( lua_gettop(L) != 2 ) { WHEREAMI; goto BYE; }
  SCLR_REC_TYPE *ptr_sclr=(SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  if ( !lua_isstring(L, 2) ) { go_BYE(1); } 
  const char * const name = luaL_checkstring(L, 2); 
  strncpy(ptr_sclr->name, name, SCLR_MAX_LEN_NAME); // 
  lua_pushboolean(L, true); 
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
static int l_sclr_name( lua_State *L) {
  int status = 0;

  if ( lua_gettop(L) != 1 ) { WHEREAMI; goto BYE; }
  SCLR_REC_TYPE *ptr_sclr=(SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  lua_pushstring(L, ptr_sclr->name); 
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
static int l_sclr_shift( lua_State *L) {
  int status = 0;

  if ( lua_gettop(L) != 3 ) { WHEREAMI; goto BYE; }
  SCLR_REC_TYPE *ptr_sclr=(SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  const char *direction = luaL_checkstring(L, 2);
  if ( direction == NULL ) { go_BYE(-1); }
  if ( !( ( strcmp(direction, "left") == 0 ) ||
         ( strcmp(direction, "right") == 0 ) ) ) { 
    go_BYE(-1); 
  }
  int itmp = luaL_checknumber(L, 3);
  if ( itmp < 0 ) { go_BYE(-1); } 
  if ( itmp > 63 ) { go_BYE(-1); } 
  uint32_t shift_by = (uint32_t)itmp;

  qtype_t qtype = ptr_sclr->qtype;
  uint64_t val = 0;

  switch ( qtype ) { 
    case I1 : case UI1 : val = ptr_sclr->val.ui1; break;
    case I2 : case UI2 : val = ptr_sclr->val.ui2; break;
    case I4 : case UI4 : val = ptr_sclr->val.ui4; break;
    case I8 : case UI8 : val = ptr_sclr->val.ui8; break;
    default : go_BYE(-1); break;
  }
  if ( strcmp(direction, "left") == 0 ) {
    val = val << shift_by;
  }
  else if ( strcmp(direction, "right") == 0 ) {
    val = val >> shift_by;
  }
  else {
    go_BYE(-1);
  }


  ptr_sclr->val.ui8 = val;
  ptr_sclr->qtype = UI8;

  // Push the scalar back 
  lua_pop(L, 2); // pop off direction and shift by
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
static int l_sclr_bitwise( lua_State *L) {
  int status = 0;

  int  nargs = lua_gettop(L);
  if ( nargs != 3 ) { WHEREAMI; goto BYE; }
  SCLR_REC_TYPE *ptr_s1 =(SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  SCLR_REC_TYPE *ptr_s2 =(SCLR_REC_TYPE *)luaL_checkudata(L, 2, "Scalar");
  const char *bop = luaL_checkstring(L, 3);
  if ( bop == NULL ) { go_BYE(-1); } 

  //---------------------------------------------
  qtype_t qtype1 = ptr_s1->qtype; uint64_t val1; 
  switch ( qtype1 ) { 
    case I1 : val1 = ptr_s1->val.i1; break; 
    case I2 : val1 = ptr_s1->val.i2; break; 
    case I4 : val1 = ptr_s1->val.i4; break; 
    case I8 : val1 = ptr_s1->val.i8; break; 
    case UI1 : val1 = ptr_s1->val.ui1; break; 
    case UI2 : val1 = ptr_s1->val.ui2; break; 
    case UI4 : val1 = ptr_s1->val.ui4; break; 
    case UI8 : val1 = ptr_s1->val.ui8; break; 
      break;
    default : 
      go_BYE(-1); 
      break;
  }
  //---------------------------------------------
  qtype_t qtype2 = ptr_s2->qtype; uint64_t val2; 
  switch ( qtype2 ) { 
    case I1 : val2 = ptr_s2->val.i1; break; 
    case I2 : val2 = ptr_s2->val.i2; break; 
    case I4 : val2 = ptr_s2->val.i4; break; 
    case I8 : val2 = ptr_s2->val.i8; break; 
    case UI1 : val2 = ptr_s2->val.ui1; break; 
    case UI2 : val2 = ptr_s2->val.ui2; break; 
    case UI4 : val2 = ptr_s2->val.ui4; break; 
    case UI8 : val2 = ptr_s2->val.ui8; break; 
      break;
    default : 
      go_BYE(-1); 
      break;
  }
  uint64_t val = 0;
  if ( strcmp(bop, "or") == 0 ) { 
    val = val1 | val2; 
  }
  else if ( strcmp(bop, "and") == 0 ) { 
    val = val1 & val2; 
  }
  else if ( strcmp(bop, "xor") == 0 ) { 
    val = val1 ^ val2; 
  }
  else {
    go_BYE(-1); 
  }
  ptr_s1->val.ui8 = val;
  ptr_s1->qtype = UI8;
  //---------------------------------------------
  if ( nargs > 1 ) { lua_pop(L, nargs-1); } // pop off second scalar and bop 
  // Push the scalar back 
  return 1;
BYE:
  if ( nargs > 1 ) { lua_pop(L, nargs-1); } // pop off second scalar and bop 
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
static int l_sclr_abs( lua_State *L) {
  int status = 0;

  if ( lua_gettop(L) != 1 ) { WHEREAMI; goto BYE; }
  SCLR_REC_TYPE *ptr_sclr=(SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  qtype_t qtype = ptr_sclr->qtype;
  switch ( qtype ) { 
    case I1 : ptr_sclr->val.i1 = abs(ptr_sclr->val.i1); break;
    case I2 : ptr_sclr->val.i2 = abs(ptr_sclr->val.i2); break;
    case I4 : ptr_sclr->val.i4 = abs(ptr_sclr->val.i4); break;
    case I8 : ptr_sclr->val.i8 = llabs(ptr_sclr->val.i8); break;
    case UI1 : /* nothing to do */ break;
    case UI2 : /* nothing to do */ break;
    case UI4 : /* nothing to do */ break;
    case UI8 : /* nothing to do */ break;
    case F4 : ptr_sclr->val.f4 = fabsf(ptr_sclr->val.f4); break;
    case F8 : ptr_sclr->val.f8 = fabs(ptr_sclr->val.f8); break;
    default : go_BYE(-1); break;
  }
  // Push the scalar back 
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
static int l_sclr_conv( lua_State *L) {
  int status = 0;

  if ( lua_gettop(L) != 2 ) { WHEREAMI; goto BYE; }
  SCLR_REC_TYPE *ptr_sclr=(SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  const char *to_qtype   = luaL_checkstring(L, 2);
  switch (  ptr_sclr->qtype ) {
    case I1 : 
    if ( strcmp(to_qtype, "I1") == 0 ) { 
      // Nothing to do 
    }
    else if ( strcmp(to_qtype, "I2") == 0 ) { 
      ptr_sclr->val.i2 = ptr_sclr->val.i1;
    }
    else if ( strcmp(to_qtype, "I4") == 0 ) { 
      ptr_sclr->val.i4 = ptr_sclr->val.i1;
    }
    else if ( strcmp(to_qtype, "I8") == 0 ) { 
      ptr_sclr->val.i8 = ptr_sclr->val.i1;
    }
    else if ( strcmp(to_qtype, "F4") == 0 ) { 
      ptr_sclr->val.f4 = ptr_sclr->val.i1;
    }
    else if ( strcmp(to_qtype, "F8") == 0 ) { 
      ptr_sclr->val.f8 = ptr_sclr->val.i1;
    }
    else {
      go_BYE(-1);
    }
    break;
    case I2 : 
    if ( strcmp(to_qtype, "I1") == 0 ) { 
      mcr_chk_range_set(ptr_sclr->val.i2, ptr_sclr->val.i2, SCHAR_MIN, SCHAR_MAX);
    }
    else if ( strcmp(to_qtype, "I2") == 0 ) { 
      // Nothing to do 
    }
    else if ( strcmp(to_qtype, "I4") == 0 ) { 
      ptr_sclr->val.i4 = ptr_sclr->val.i2;
    }
    else if ( strcmp(to_qtype, "I8") == 0 ) { 
      ptr_sclr->val.i8 = ptr_sclr->val.i2;
    }
    else if ( strcmp(to_qtype, "F4") == 0 ) { 
      ptr_sclr->val.f4 = ptr_sclr->val.i2;
    }
    else if ( strcmp(to_qtype, "F8") == 0 ) { 
      ptr_sclr->val.f8 = ptr_sclr->val.i2;
    }
    else {
      go_BYE(-1);
    }
    break;
    case I4 : 
    if ( strcmp(to_qtype, "I1") == 0 ) { 
      mcr_chk_range_set(ptr_sclr->val.i4, ptr_sclr->val.i1, SCHAR_MIN, SCHAR_MAX);
    }
    else if ( strcmp(to_qtype, "I2") == 0 ) { 
      mcr_chk_range_set(ptr_sclr->val.i4, ptr_sclr->val.i2, SHRT_MIN, SHRT_MAX);
    }
    else if ( strcmp(to_qtype, "I4") == 0 ) { 
      // nothing to do 
    }
    else if ( strcmp(to_qtype, "I8") == 0 ) { 
      ptr_sclr->val.i8 = ptr_sclr->val.i4;
    }
    else if ( strcmp(to_qtype, "F4") == 0 ) { 
      if ( ptr_sclr->val.i4 > 16777217 ) { go_BYE(-1); }
      if ( ptr_sclr->val.i4 < -16777217 ) { go_BYE(-1); }
      ptr_sclr->val.f4 = ptr_sclr->val.i4;
    }
    else if ( strcmp(to_qtype, "F8") == 0 ) { 
      ptr_sclr->val.f8 = ptr_sclr->val.i4;
    }
    else {
      go_BYE(-1);
    }
    break;
    case I8 : 
    if ( strcmp(to_qtype, "I1") == 0 ) { 
      mcr_chk_range_set(ptr_sclr->val.i8, ptr_sclr->val.i1, SCHAR_MIN, SCHAR_MAX);
    }
    else if ( strcmp(to_qtype, "I2") == 0 ) { 
      mcr_chk_range_set(ptr_sclr->val.i8, ptr_sclr->val.i1, SHRT_MIN, SHRT_MAX);
    }
    else if ( strcmp(to_qtype, "I4") == 0 ) { 
      mcr_chk_range_set(ptr_sclr->val.i8, ptr_sclr->val.i1, INT_MIN, INT_MAX);
    }
    else if ( strcmp(to_qtype, "I8") == 0 ) { 
      // nothing to do 
    }
    else if ( strcmp(to_qtype, "F4") == 0 ) { 
      if ( ptr_sclr->val.i8 > 16777217 ) { go_BYE(-1); }
      if ( ptr_sclr->val.i8 < -16777217 ) { go_BYE(-1); }
      ptr_sclr->val.f4 = ptr_sclr->val.i8;
    }
    else if ( strcmp(to_qtype, "F8") == 0 ) { 
      if ( ptr_sclr->val.i8 > 9007199254740993LL ) { go_BYE(-1); }
      if ( ptr_sclr->val.i8 < -9007199254740993LL ) { go_BYE(-1); }
      ptr_sclr->val.f8 = ptr_sclr->val.i8;
    }
    else {
      go_BYE(-1);
    }
    break;
    case F4 : 
    {
    float val = ptr_sclr->val.f4;
    if ( strcmp(to_qtype, "I1") == 0 ) { 
      mcr_chk_int(val);
      if ( ( val < SCHAR_MIN ) || ( val > SCHAR_MAX ) ) { go_BYE(-1); }
      ptr_sclr->val.i1 = val;
    }
    else if ( strcmp(to_qtype, "I2") == 0 ) { 
      mcr_chk_int(val);
      if ( ( val < SHRT_MIN ) || ( val > SHRT_MAX ) ) { go_BYE(-1); }
      ptr_sclr->val.i2 = val;
    }
    else if ( strcmp(to_qtype, "I4") == 0 ) { 
      mcr_chk_int(val);
      ptr_sclr->val.i4 = val;
    }
    else if ( strcmp(to_qtype, "I8") == 0 ) { 
      mcr_chk_int(val);
      ptr_sclr->val.i8 = val;
    }
    else if ( strcmp(to_qtype, "F4") == 0 ) { 
      // Nothing to do 
    }
    else if ( strcmp(to_qtype, "F8") == 0 ) { 
      ptr_sclr->val.f8 = ptr_sclr->val.f4;
    }
    else {
      go_BYE(-1);
    }
    }
    break;
    case F8 : 
    {
    double val = ptr_sclr->val.f8;
    if ( strcmp(to_qtype, "I1") == 0 ) { 
      mcr_chk_int(val);
      if ( ( val < SCHAR_MIN ) || ( val > SCHAR_MAX ) ) { go_BYE(-1); }
      ptr_sclr->val.i1 = val;
    }
    else if ( strcmp(to_qtype, "I2") == 0 ) { 
      mcr_chk_int(val);
      if ( ( val < SHRT_MIN ) || ( val > SHRT_MAX ) ) { go_BYE(-1); }
      ptr_sclr->val.i2 = val;
    }
    else if ( strcmp(to_qtype, "I4") == 0 ) { 
      mcr_chk_int(val);
      if ( ( val < INT_MIN ) || ( val > INT_MAX ) ) { go_BYE(-1); }
      ptr_sclr->val.i4 = val;
    }
    else if ( strcmp(to_qtype, "I8") == 0 ) { 
      mcr_chk_int(val);
      ptr_sclr->val.i8 = val;
    }
    else if ( strcmp(to_qtype, "F4") == 0 ) { 
      if ( ptr_sclr->val.f8 >    FLT_MAX ) { go_BYE(-1); }
      if ( ptr_sclr->val.f8 < -1*FLT_MAX ) { go_BYE(-1); }
      ptr_sclr->val.f4 = (double)ptr_sclr->val.f8;
      // TODO P3 Consider case where double value is close
      // to 0 but coercion to float makes it 0
    }
    else if ( strcmp(to_qtype, "F8") == 0 ) { 
      // Nothing to do 
    }
    else {
      go_BYE(-1);
    }
    }
    break;
    default : 
    go_BYE(-1);
    break;
  }
  ptr_sclr->qtype = get_c_qtype(to_qtype);

  // Push the scalar back 
  lua_pop(L, 1);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}

typedef enum {
  DATA_AS_UNDEF,
  DATA_AS_CMEM,
  DATA_AS_NUM,
  DATA_AS_STR,
  DATA_AS_BOOL,
} data_as_t;

static int l_sclr_new( lua_State *L) {
  int status = 0;
  const char *str_val = NULL;
  // lua_Number  num_val;
  bool bool_val;
  CMEM_REC_TYPE *ptr_cmem = NULL;
  data_as_t data_as = DATA_AS_UNDEF;

  // TESTING GC problems lua_gc(L, LUA_GCCOLLECT, 0);  

  int x_lua_type = lua_type(L, 1);  // debugging 
  const char *str_lua_type = lua_typename(L, x_lua_type); // debugging 
  if ( ( lua_gettop(L) != 1) && ( lua_gettop(L) != 2 ) ) { go_BYE(-1); }
  if ( lua_isstring(L, 1) ) { 
    str_val = luaL_checkstring(L, 1);
    data_as = DATA_AS_STR;
  }
  else if (  lua_isuserdata(L, 1) ) { 
    ptr_cmem = luaL_checkudata(L, 1, "CMEM");
    data_as = DATA_AS_CMEM;
  }
  else if ( lua_isnumber(L, 1) ) {
    // No matter how I invoke it, Lua sends value as string
    // num_val = luaL_checknumber(L, 1);
    data_as = DATA_AS_NUM;
    go_BYE(-1); 
  }
  else if ( lua_isboolean(L, 1) ) {
    // However, if I invoke as true, then it comes here
    bool_val = lua_toboolean(L, 1);
    data_as = DATA_AS_BOOL;
  }
  else {
    go_BYE(-1);
  }
  const char *str_qtype;
  qtype_t qtype;
  if ( lua_gettop(L) == 2 ) {
    str_qtype   = luaL_checkstring(L, 2);
    qtype = get_c_qtype(str_qtype);
    if ( qtype == Q0 ) { go_BYE(-1); }
  }
  else {
    qtype = F8; // default  -- use same behavior as Lua
  }
  SCLR_REC_TYPE *ptr_sclr = NULL;
  ptr_sclr = (SCLR_REC_TYPE *)lua_newuserdata(L, sizeof(SCLR_REC_TYPE));
  return_if_malloc_failed(ptr_sclr);
  memset(ptr_sclr, '\0', sizeof(SCLR_REC_TYPE));
  ptr_sclr->qtype = qtype;

  switch ( data_as ) {
    case DATA_AS_CMEM : 
      switch ( qtype )  {
        case BL : 
          ptr_sclr->val.bl = ((bool *)ptr_cmem->data)[0];
          break;
        case I1 : 
          ptr_sclr->val.i1 = ((int8_t *)ptr_cmem->data)[0];
          break;
        case I2 : 
          ptr_sclr->val.i2 = ((int16_t *)ptr_cmem->data)[0];
          break;
        case I4 : 
          ptr_sclr->val.i4 = ((int32_t *)ptr_cmem->data)[0];
          break;
        case I8 : 
          ptr_sclr->val.i8 = ((int64_t *)ptr_cmem->data)[0];
          break;
        case UI1 : 
          ptr_sclr->val.ui1 = ((uint8_t *)ptr_cmem->data)[0];
          break;
        case UI2 : 
          ptr_sclr->val.ui2 = ((uint16_t *)ptr_cmem->data)[0];
          break;
        case UI4 : 
          ptr_sclr->val.ui4 = ((uint32_t *)ptr_cmem->data)[0];
          break;
        case UI8 : 
          ptr_sclr->val.ui8 = ((uint64_t *)ptr_cmem->data)[0];
          break;
        case F4 : 
          ptr_sclr->val.f4 = ((float *)ptr_cmem->data)[0];
          break;
        case F8 : 
          ptr_sclr->val.f8 = ((double *)ptr_cmem->data)[0];
          break;
        default : 
          go_BYE(-1);
          break;
      }
      break;
    case DATA_AS_BOOL : 
      switch ( qtype ) { 
        case BL : ptr_sclr->val.bl = bool_val; break;
        default : go_BYE(-1); break; 
      }
      break;
    case DATA_AS_NUM : 
      go_BYE(-1); // TODO P2
      break;
    case DATA_AS_STR : 
      switch ( qtype )  {
        case BL :  
          status = txt_to_BL(str_val, &(ptr_sclr->val.bl)); 
          break;
          //------------------------------------------------------
        case I1 : 
          status = txt_to_I1(str_val, &(ptr_sclr->val.i1)); 
          break;
        case I2 : 
          status = txt_to_I2(str_val, &(ptr_sclr->val.i2)); 
          break;
        case I4 : 
          status = txt_to_I4(str_val, &(ptr_sclr->val.i4)); 
          break;
        case I8 : 
          status = txt_to_I8(str_val, &(ptr_sclr->val.i8)); 
          break;
          //------------------------------------------------------
        case UI1 : 
          status = txt_to_UI1(str_val, &(ptr_sclr->val.ui1)); 
          break;
        case UI2 : 
          status = txt_to_UI2(str_val, &(ptr_sclr->val.ui2)); 
          break;
        case UI4 : 
          status = txt_to_UI4(str_val, &(ptr_sclr->val.ui4)); 
          break;
        case UI8 : 
          status = txt_to_UI8(str_val, &(ptr_sclr->val.ui8)); 
          break;
          //------------------------------------------------------
        case F2 : 
          {
            float ftmp;  
            status = txt_to_F4(str_val, &ftmp); cBYE(status);
            ptr_sclr->val.f2 = F4_to_F2(ftmp); 
          }
          break;
        case F4 : 
          {
            if ( strcmp(str_val, "FLT_MIN") == 0 ) {
              ptr_sclr->val.f4 = FLT_MIN;
            }
            else if ( strcmp(str_val, "FLT_MAX") == 0 ) {
              ptr_sclr->val.f4 = FLT_MAX;
            }
            else { 
            status = txt_to_F4(str_val, &(ptr_sclr->val.f4)); 
            }
          }
          break;
        case F8 : 
          status = txt_to_F8(str_val, &(ptr_sclr->val.f8)); 
          break;
          //------------------------------------------------------
        case SC : 
          {
            int len = strlen(str_val)+1;
            ptr_sclr->val.str = malloc(len);
            memset(ptr_sclr->val.str, 0, len);
            memcpy(ptr_sclr->val.str, str_val, len);
          }
          break;
        default : 
          go_BYE(-1);
          break;
      }
      cBYE(status);
      break;
    default :
      go_BYE(-1);
      break;
  }
  luaL_getmetatable(L, "Scalar"); /* Add the metatable to the stack. */
  lua_setmetatable(L, -2); /* Set the metatable on the userdata. */
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  return 2;
}

static int l_sclr_free( lua_State *L) 
{
  int status = 0;
  SCLR_REC_TYPE *ptr_sclr = NULL;
  if ( lua_gettop(L) != 1 ) { go_BYE(-1); }
  ptr_sclr = (SCLR_REC_TYPE *)luaL_checkudata(L, 1, "Scalar");
  if ( ptr_sclr == NULL ) { go_BYE(-1); }
  if ( ptr_sclr->qtype == SC ) {
    free_if_non_null(ptr_sclr->val.str);
  }
  /* uncomment for debugging 
  if ( ptr_sclr->name[0] != '\0' ) { 
    printf("Destructor for %s \n", ptr_sclr->name); 
  }
  */
  lua_pushboolean(L, true);
  return 1;
BYE:
  lua_pushnil(L);
  lua_pushstring(L, __func__);
  lua_pushnumber(L, status);
  return 3;
}
//----------------------------------------

#include "outer_eval_cmp.c"
#include "_eval_arith.c"
#include "_outer_eval_arith.c"
//-----------------------
static const struct luaL_Reg sclr_methods[] = {
    { "__gc", l_sclr_free },
    { "abs", l_sclr_abs },
    { "bitwise", l_sclr_bitwise },
    { "conv", l_sclr_conv },
    { "name", l_sclr_name },
    { "to_str", l_sclr_to_str },
    { "to_bool", l_sclr_to_bool },
    { "to_num", l_sclr_to_num },
    { "to_cmem", l_sclr_to_cmem },
    { "to_data", l_sclr_to_data },
    { "qtype", l_qtype },
    { "reincarnate", l_sclr_reincarnate },
    { "set_name", l_sclr_set_name },
    { "shift", l_sclr_shift },
    { NULL,          NULL               },
};
 
static const struct luaL_Reg sclr_functions[] = {
    { "abs", l_sclr_abs },
    { "bitwise", l_sclr_bitwise },
    { "conv", l_sclr_conv },
    { "add", l_sclr_add },
    { "conv", l_sclr_conv },
    { "div", l_sclr_div },
    { "eq", l_sclr_eq },
    { "geq", l_sclr_geq },
    { "gt", l_sclr_gt },
    { "leq", l_sclr_leq },
    { "lt", l_sclr_lt },
    { "mul", l_sclr_mul },
    { "name", l_sclr_name },
    { "neq", l_sclr_neq },
    { "new", l_sclr_new },
    { "qtype", l_qtype },
    { "reincarnate", l_sclr_reincarnate },
    { "set_name", l_sclr_set_name },
    { "shift", l_sclr_shift },
    { "sub", l_sclr_sub },
    { "to_cmem", l_sclr_to_cmem },
    { "to_data", l_sclr_to_data },
    { "to_num", l_sclr_to_num },
    { "to_bool", l_sclr_to_bool },
    { "to_str", l_sclr_to_str },
    { NULL,  NULL         }
};
 
/*
** Open test library
*/

int luaopen_libsclr (lua_State *L) {
  int status = 0, nstack;
  nstack = lua_gettop(L); 
  if ( nstack != 1 ) { WHEREAMI; exit(1); }   
  /* Create the metatable and put it on the stack. */
  luaL_newmetatable(L, "Scalar");
  nstack = lua_gettop(L); if ( nstack != 2 ) { WHEREAMI; exit(1); }   
  /* Duplicate the metatable on the stack (We know have 2). */
  lua_pushvalue(L, -1);
  nstack = lua_gettop(L); if ( nstack != 3 ) { WHEREAMI; exit(1); }   
  /* Pop the first metatable off the stack and assign it to __index
   * of the second one. We set the metatable for the table to itself.
   * This is equivalent to the following in lua:
   * metatable = {}
   * metatable.__index = metatable
   */
  lua_setfield(L, -2, "__index"); nstack = lua_gettop(L); 
  lua_pushcfunction(L, l_sclr_to_str); lua_setfield(L, -2, "__tostring");

  lua_pushcfunction(L, l_sclr_eq); lua_setfield(L, -2, "__eq");
  lua_pushcfunction(L, l_sclr_lt); lua_setfield(L, -2, "__lt");
  lua_pushcfunction(L, l_sclr_gt); lua_setfield(L, -2, "__gt");
  lua_pushcfunction(L, l_sclr_leq); lua_setfield(L, -2, "__le");
  lua_pushcfunction(L, l_sclr_geq); lua_setfield(L, -2, "__ge");
  /* negations of above happen automatically. No need to do them here */

  lua_pushcfunction(L, l_sclr_add); lua_setfield(L, -2, "__add");
  lua_pushcfunction(L, l_sclr_sub); lua_setfield(L, -2, "__sub");
  lua_pushcfunction(L, l_sclr_mul); lua_setfield(L, -2, "__mul");
  lua_pushcfunction(L, l_sclr_div); lua_setfield(L, -2, "__div");

  // Following do not work currently
  // Will not work in 5.1 as per Indrajeet
  // TODO P2 Investigate why this does not work.
  lua_pushcfunction(L, l_sclr_to_num); lua_setfield(L, -2, "__tonumber");
  /* See error below 
> x = Q.Scalar.new(123)
> =x
123
> y = tonumber(x)
> =y
nil
*/
  // Above do not work currently

  /* Register the object.func functions into the table that is at the 
   * top of the stack. */

  /* Set the methods to the metatable that should be accessed via
   * object:func */
  nstack = lua_gettop(L); if ( nstack != 2 ) { WHEREAMI; exit(1); }   
  luaL_register(L, NULL, sclr_methods);

  /* Register Scalar in types table. Reason is as follows: */
  /* Suppose we did not do this. When we do x = Scalar.new(123),
   * type(x) == "userdata". But doing this allows us to get
   * type(x) == "Scalar" */
  status = luaL_dostring(L, "return require 'RSUTILS/lua/register_type'");
  if (status != 0 ) {
    fprintf(stderr, "Running require failed:  %s\n", lua_tostring(L, -1));
    exit(1);
  } 
  // that top of stack is a function (register_type) 
  if ( !lua_isfunction(L, -1)  ) { WHEREAMI; exit(1); }   
  nstack = lua_gettop(L); if ( nstack != 3 ) { WHEREAMI; exit(1); }   
  //-- put metatable on stack 
  luaL_getmetatable(L, "Scalar");
  nstack = lua_gettop(L); if ( nstack != 4 ) { WHEREAMI; exit(1); }   
  //--- put string "Scalar" on stack 
  lua_pushstring(L, "Scalar");
  nstack = lua_gettop(L); if ( nstack != 5 ) { WHEREAMI; exit(1); }   
  status =  lua_pcall(L, 2, 1, 0);
  if ( status != 0 ) {
     fprintf(stderr, "%d\n", status);
     fprintf(stderr, "Type registration failed: %s\n", lua_tostring(L, -1));
     exit(1);
  }
  nstack = lua_gettop(L); if ( nstack != 3 ) { WHEREAMI; exit(1); }   
    // extract return value
  luaL_checktype(L, -1, LUA_TBOOLEAN);
  if ( ! lua_isboolean(L,  -1) ) { WHEREAMI; return 1; }
  bool rslt = lua_toboolean(L, -1);
  if ( !rslt ) { WHEREAMI; exit(1); }
  lua_pop(L, 1); 
  nstack = lua_gettop(L); if ( nstack != 2 ) { WHEREAMI; exit(1); }   

  /* Register the object.func functions into the table that is at the
   op of the stack. */
  luaL_register(L, NULL, sclr_functions);
  
  // Registering with Q
  // This allows us to do 
  // local Q = require 'Q'
  // local x = Q.Scalar.new(123)
  // in addition to 
  // local Scalar = require 'libsclr'
  // local x = Scalar.new(123)
  status = luaL_dostring(L, "return require('Q/q_export').export");
  if (status != 0 ) {
    fprintf(stderr, "Q registration require failed:  %s\n", lua_tostring(L, -1));
    WHEREAMI; exit(1);
  }
  // check that top of stack is a function (q_export)
  nstack = lua_gettop(L); if ( nstack != 3 ) { WHEREAMI; exit(1); }   
  if ( !lua_isfunction(L, -1)  ) { WHEREAMI; exit(1); }   
  // ------
  lua_pushstring(L, "Scalar");
  if ( !lua_isstring(L, -1)  ) { WHEREAMI; exit(1); }   
  nstack = lua_gettop(L); if ( nstack != 4 ) { WHEREAMI; exit(1); }   
  lua_newtable(L); // instead of lua_createtable(L, 0, 0);
  // lua_newtable() Creates a new empty table and pushes it onto the stack. 
  nstack = lua_gettop(L); if ( nstack != 5 ) { WHEREAMI; exit(1); }   
  luaL_register(L, NULL, sclr_functions);
  status = lua_pcall(L, 2, 1, 0);
  if (status != 0 ){
     fprintf(stderr, "%d\n", status);
     fprintf(stderr, "q_export registration failed: %s\n", lua_tostring(L, -1));
     WHEREAMI; exit(1);
  }
  nstack = lua_gettop(L); if ( nstack != 3 ) { WHEREAMI; exit(1); }   
  lua_pop(L, 1);
  nstack = lua_gettop(L); if ( nstack != 2 ) { WHEREAMI; exit(1); }   
  return 1; // You are returning 1 thing, a table. Hence the 1. 
}
