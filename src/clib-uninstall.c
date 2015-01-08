
//
// clib-uninstall.c
//
// Copyright (c) 2014 Stephen Mathieson
// MIT licensed
//

#include <string.h>
#include <stdlib.h>
#include "http-get/http-get.h"
#include "parson/parson.h"
#include "logger/logger.h"
#include "clib-uninstall.h"

static char *
get_tarball_url(const char *owner, const char *name, const char *version) {
  size_t size = 19 // https://github.com/
              + strlen(owner)
              + 1 // /
              + strlen(name)
              + 9 // /archive/
              + strlen(version)
              + 7; // .tar.gz
  char *tarball = malloc(size * sizeof(char *));
  if (tarball) {
    sprintf(tarball
    , "https://github.com/%s/%s/archive/%s.tar.gz"
    , owner
    , name
    , version);
  }
  return tarball;
}

static char *
get_tar_filepath(const char *name, const char *version) {
  size_t size = 1 // null
              + strlen(name)
              + 1 // -
              + strlen(version)
              + 7; // .tar.gz
  char *file = malloc(size * sizeof(char *));
  if (file) sprintf(file, "%s-%s.tar.gz", name, version);
  return file;
}

static char *
get_tmp_tarball(const char *file) {
  size_t size = 5 // /tmp/ + null
              + strlen(file);
  char *tmp = malloc(size * sizeof(char *));
  if (tmp) sprintf(tmp, "/tmp/%s", file);
  return tmp;
}

static char *
get_untar_command(const char *file) {
  size_t size = 19 // cd /tmp && tar -xf
              + strlen(file);
  char *command = malloc(size * sizeof(char *));
  if (command) sprintf(command, "cd /tmp && tar -xf %s", file);
  return command;
}

static char *
get_uninstall_target(const char *name, const char *version) {
  size_t size = 5 // /tmp/
              + strlen(name)
              + 1 // /
              + strlen(version);
  char *target = NULL;
  char *dir = malloc(size * sizeof(char *));
  char *package_json = malloc((size + 13) * sizeof(char));
  JSON_Value *root = NULL;
  JSON_Object *object = NULL;

  if (!dir || !package_json) goto cleanup;

  sprintf(dir, "/tmp/%s-%s", name, version);
  sprintf(package_json, "%s/package.json", dir);

  root = json_parse_file(package_json);
  if (!root) goto cleanup;

  object = json_value_get_object(root);
  if (!object) goto cleanup;

  const char *value = json_object_get_string(object, "uninstall");
  if (!value) {
    logger_warn(
        "warning"
      , "No uninstall target specified.  Defaulting to '%s'."
      , CLIB_UNINSTALL_DEFAULT_TARGET
    );
    // default to "make uninstall"
    value = CLIB_UNINSTALL_DEFAULT_TARGET;
  }

  size = 3 // 'cd '
       + strlen(dir)
       + 4 // ' && '
       + strlen(value);
  target = malloc(size * sizeof(char *));
  if (target) sprintf(target, "cd %s && %s", dir, value);

cleanup:
  if (root) json_value_free(root);
  if (dir) free(dir);
  if (package_json) free(package_json);
  return target;
}

int
clib_uninstall(const char *owner, const char *name, const char *version) {
  char *tarball = NULL;
  char *file = NULL;
  char *tarpath = NULL;
  char *command = NULL;
  char *target = NULL;
  int rc = -1;

  if (!owner || !name || !version) goto cleanup;

  tarball = get_tarball_url(owner, name, version);
  if (!tarball) goto cleanup;

  file = get_tar_filepath(name, version);
  if (!file) goto cleanup;

  tarpath = get_tmp_tarball(file);
  if (!tarpath) goto cleanup;

  logger_info("fetch", tarball);
  if (-1 == http_get_file(tarball, tarpath)) {
    logger_error("error", "failed to fetch tarball");
    goto cleanup;
  }

  command = get_untar_command(file);
  if (!command) goto cleanup;

  logger_info("untar", tarpath);
  if (0 != system(command)) {
    logger_error("error", "failed to untar");
    goto cleanup;
  }

  target = get_uninstall_target(name, version);
  if (!target) goto cleanup;

  rc = system(target);

cleanup:
  if (target) free(target);
  if (tarpath) free(tarpath);
  if (file) free(file);
  if (tarball) free(tarball);
  return rc;
}
