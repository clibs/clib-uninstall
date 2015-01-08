
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
#include "asprintf/asprintf.h"
#include "clib-uninstall.h"


static char *
get_tarball_url(const char *owner, const char *name, const char *version) {
  char *tarball = NULL;
  int size = 0;

  size = asprintf(
      &tarball
    , "https://github.com/%s/%s/archive/%s.tar.gz"
    , owner
    , name
    , version
  );

  if (-1 == size) return NULL;
  return tarball;
}

static char *
get_tar_filepath(const char *name, const char *version) {
  char *file = NULL;
  int size = asprintf(&file, "%s-%s.tar.gz", name, version);
  if (-1 == size) return NULL;
  return file;
}

static char *
get_tmp_tarball(const char *file) {
  char *tmp = NULL;
  int size = asprintf(&tmp, "/tmp/%s", file);
  if (-1 == size) return NULL;
  return tmp;
}

static char *
get_untar_command(const char *file) {
  char *cmd = NULL;
  int size = 0;
  size = asprintf(&cmd, "cd /tmp && tar -xf %s", file);
  if (-1 == size) return NULL;
  return cmd;
}

static char *
get_uninstall_target(const char *name, const char *version) {
  int size = 0;
  char *target = NULL;
  char *dir = NULL;
  char *pkg = NULL;
  const char *val = NULL;
  JSON_Value *root = NULL;
  JSON_Object *obj = NULL;

  size = asprintf(&dir, "/tmp/%s-%s", name, version);
  if (-1 == size) return NULL;

  size = asprintf(&pkg, "%s/package.json", dir);
  if (-1 == size) goto done;

  root = json_parse_file(pkg);
  if (!root) goto done;

  obj = json_value_get_object(root);
  if (!obj) goto done;

  val = json_object_get_string(obj, "uninstall");
  if (!val) {
    logger_warn(
        "warning"
      , "No uninstall target specified.  Defaulting to '%s'."
      , CLIB_UNINSTALL_DEFAULT_TARGET
    );
    // default to "make uninstall"
    val = CLIB_UNINSTALL_DEFAULT_TARGET;
  }

  size = asprintf(&target, "cd %s && %s", dir, val);

done:
  if (root) json_value_free(root);
  free(dir);
  free(pkg);
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
