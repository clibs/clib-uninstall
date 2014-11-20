
//
// main.c
//
// Copyright (c) 2014 Stephen Mathieson
// MIT licensed
//

#include <stdlib.h>
#include <string.h>
#include "commander/commander.h"
#include "substr/substr.h"
#include "strdup/strdup.h"
#include "clib-uninstall.h"
#include "log.h"

// TODO replace
static char *
parse_author(const char *slug) {
  char *name = NULL;
  char *author = NULL;
  char *copy = strdup(slug);
  if (NULL == copy) return NULL;

  name = strstr(copy, "/");
  if (!name) {
    author = strdup("clibs");
    goto cleanup;
  }

  int delta = name - copy;
  if (!delta || !(author = malloc(delta * sizeof(char *)))) goto cleanup;
  author = substr(copy, 0, delta);

cleanup:
  free(copy);
  return author;
}

// TODO replace
static char *
parse_name(const char *slug) {
  char *copy = NULL;
  char *name = NULL;
  char *t = NULL;
  char *version = NULL;

  if (!(copy = strdup(slug))) return NULL;
  version = strstr(copy, "@");
  if (version) {
    // drop version
    copy = substr(copy, 0, version - copy);
  }
  t = strstr(copy, "/");
  if (!t) {
    name = copy;
    goto cleanup;
  }
  t++;
  if (0 == strlen(t)) goto cleanup;
  name = t;

cleanup:
  free(copy);
  return strdup(name);
}


int
main(int argc, char **argv) {
  int rc = 1;
  command_t program;

  command_init(&program
    , "clib-uninstall"
    , CLIB_UNINSTALL_VERSION);
  program.usage = "[name ...]";
  command_parse(&program, argc, argv);

  if (0 == program.argc) command_help(&program);

  for (int i = 0; i < program.argc; i++) {
    char *owner = parse_author(program.argv[i]);
    if (!owner) goto cleanup;
    char *name = parse_name(program.argv[i]);
    if (!name) {
      free(owner);
      goto cleanup;
    }

    int res = clib_uninstall(owner, name, "master");
    free(owner);
    free(name);
    if (-1 == res) {
      error("error", "Failed to uninstall %s", program.argv[i]);
      goto cleanup;
    }
  }

  rc = 0;

cleanup:
  command_free(&program);
  return rc;
}
