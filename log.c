#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "log.h"

static bool debug = false;
static bool trace = false;

bool
xcc_check_debug() {
  if(debug)
    return true;
  static int debug = -1;
  if(debug == -1) {
    const char* quapi_debug = getenv("XCC_DEBUG");
    debug = quapi_debug != NULL;
  }
  return debug;
}

bool
xcc_check_trace() {
  if(trace)
    return true;
  static int trace = -1;
  if(trace == -1) {
    const char* quapi_trace = getenv("XCC_TRACE");
    trace = quapi_trace != NULL;
  }
  return trace;
}

void
dbg(const char* format, ...) {
  if(xcc_check_debug()) {
    fprintf(stderr, "[QuAPI] [DEBUG] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputc('\n', stderr);
  }
}

void
trc(const char* format, ...) {
  if(xcc_check_trace()) {
    fprintf(stderr, "[XCC] [TRACE] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputc('\n', stderr);
  }
}

void
err(const char* format, ...) {
  flockfile(stderr);
  fprintf(stderr, "[XCC] [ERROR] ");
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputc('\n', stderr);
  funlockfile(stderr);
}
