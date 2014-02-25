
//
// log.h
//
// Copyright (c) 2014 Stephen Mathieson
// MIT licensed
//

#ifndef CLIB_UNINSTALL_LOG
#define CLIB_UNINSTALL_LOG 1

// TODO clibs/clib-log.h ??

#include <stdio.h>
#include "console-colors/console-colors.h"

#define log(type, ...) ({                           \
  cc_fprintf(CC_FG_CYAN, stdout, "  %10s", type);   \
  printf(" : ");                                    \
  cc_fprintf(CC_FG_DARK_GRAY, stdout, __VA_ARGS__); \
  printf("\n");                                     \
});

#define warn(type, ...) ({                               \
  cc_fprintf(CC_FG_DARK_YELLOW, stderr, "  %10s", type); \
  fprintf(stderr, " : ");                                \
  cc_fprintf(CC_FG_DARK_GRAY, stderr, __VA_ARGS__);      \
  fprintf(stderr, "\n");                                 \
});

#define error(type, ...) ({                           \
  cc_fprintf(CC_FG_DARK_RED, stderr, "  %10s", type); \
  fprintf(stderr, " : ");                             \
  cc_fprintf(CC_FG_DARK_GRAY, stderr, __VA_ARGS__);   \
  fprintf(stderr, "\n");                              \
});

#endif
