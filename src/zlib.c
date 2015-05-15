#include "mruby.h"
#include "mruby/value.h"
#include "mruby/string.h"

#include <stdlib.h>
#include <zlib.h>

#define WINDOW_BITS_DEFLATE 15
#define WINDOW_BITS_GZIP    (15 + 16)
#define WINDOW_BITS_AUTO    (15 + 16 + 16)

static void
raise(mrb_state *mrb, z_streamp strm, int err)
{
  char msg[256];

  snprintf(msg, sizeof(msg), "zlib error (%d): %s", err, strm->msg);
  mrb_raise(mrb, E_RUNTIME_ERROR, msg);
}

static mrb_value
mrb_iizlib_compress(mrb_state *mrb, mrb_value self, int windowbits)
{
  mrb_value data, arg;
  z_stream strm;
  int ret;

  strm.zalloc = Z_NULL;
  strm.zfree  = Z_NULL;
  strm.opaque = Z_NULL;

  mrb_get_args(mrb, "S", &arg);

  ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION,
      Z_DEFLATED, windowbits, 8, Z_DEFAULT_STRATEGY);
  if (ret != Z_OK){
    raise(mrb, &strm, ret);
  }

  data = mrb_str_buf_new(mrb, deflateBound(&strm, RSTRING_LEN(arg)));

  strm.next_in = (Bytef *) RSTRING_PTR(arg);
  strm.avail_in = RSTRING_LEN(arg);
  strm.next_out = (Bytef *) RSTRING_PTR(data);
  strm.avail_out = RSTRING_CAPA(data);

  while (1) {
    ret = deflate(&strm, Z_FINISH);
    if (ret == Z_OK) {
      data = mrb_str_resize(mrb, data, RSTRING_CAPA(data) * 2);
      strm.next_out = (Bytef *) RSTRING_PTR(data) + strm.total_out;
      strm.avail_out = RSTRING_CAPA(data) - strm.total_out;
    } else if (ret == Z_STREAM_END) {
      data = mrb_str_resize(mrb, data, strm.total_out);
      ret = deflateEnd(&strm);
      if (ret != Z_OK) {
        raise(mrb, &strm, ret);
      }
      break;
    } else {
      raise(mrb, &strm, ret);
    }
  }

  return data;
}

static mrb_value
mrb_iizlib_deflate(mrb_state *mrb, mrb_value self)
{
  return mrb_iizlib_compress(mrb, self, WINDOW_BITS_DEFLATE);
}

static mrb_value
mrb_iizlib_gzip(mrb_state *mrb, mrb_value self)
{
  return mrb_iizlib_compress(mrb, self, WINDOW_BITS_GZIP);
}

static mrb_value
mrb_iizlib_inflate(mrb_state *mrb, mrb_value self)
{
  mrb_value data, arg;
  z_stream strm;
  int ret;

  strm.zalloc = Z_NULL;
  strm.zfree  = Z_NULL;
  strm.opaque = Z_NULL;

  mrb_get_args(mrb, "S", &arg);

  strm.next_in = (Bytef *) RSTRING_PTR(arg);
  strm.avail_in = RSTRING_LEN(arg);

  ret = inflateInit2(&strm, WINDOW_BITS_AUTO);
  if (ret != Z_OK) {
    raise(mrb, &strm, ret);
  }

  data = mrb_str_buf_new(mrb, RSTRING_LEN(arg) * 2);
  strm.next_out = (Bytef *) RSTRING_PTR(data);
  strm.avail_out = RSTRING_CAPA(data);

  while (1) {
    ret = inflate(&strm, Z_NO_FLUSH);
    if (ret == Z_OK) {
      data = mrb_str_resize(mrb, data, RSTRING_CAPA(data) * 2);
      strm.next_out = (Bytef *) RSTRING_PTR(data) + strm.total_out;
      strm.avail_out = RSTRING_CAPA(data) - strm.total_out;
    } else if (ret == Z_STREAM_END) {
      data = mrb_str_resize(mrb, data, strm.total_out);
      ret = inflateEnd(&strm);
      if (ret != Z_OK) {
        raise(mrb, &strm, ret);
      }
      break;
    } else {
      raise(mrb, &strm, ret);
    }
  }

  return data;
}

void
mrb_mruby_iizlib_gem_init(mrb_state *mrb)
{
  struct RClass *zlib;

  zlib = mrb_define_module(mrb, "Zlib");
  mrb_define_module_function(mrb, zlib, "deflate", mrb_iizlib_deflate, ARGS_REQ(1));
  mrb_define_module_function(mrb, zlib, "gzip", mrb_iizlib_gzip, ARGS_REQ(1));
  mrb_define_module_function(mrb, zlib, "inflate", mrb_iizlib_inflate, ARGS_REQ(1));
}

void
mrb_mruby_iizlib_gem_final(mrb_state *mrb)
{
}