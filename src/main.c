
//
// main.c
//
// Copyright (c) 2014 Stephen Mathieson
// MIT licensed
//

#include <stdlib.h>
#include <string.h>
#include "commander/commander.h"
#include "parse-repo/parse-repo.h"
#include "clib-uninstall.h"
#include "log.h"

#ifndef DEFAULT_REPO_OWNER
#define DEFAULT_REPO_OWNER "clibs"
#endif

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
    char *owner = parse_repo_owner(program.argv[i], DEFAULT_REPO_OWNER);
    if (!owner) goto cleanup;
    char *name = parse_repo_name(program.argv[i]);
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
