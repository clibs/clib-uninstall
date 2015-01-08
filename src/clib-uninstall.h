
//
// clib-uninstall.h
//
// Copyright (c) 2014 Stephen Mathieson
// MIT licensed
//

#ifndef CLIB_UNINSTALL_H
#define CLIB_UNINSTALL_H 1

#define CLIB_UNINSTALL_VERSION "1.0.1"

/**
 * Default uninstallation target.
 */

#define CLIB_UNINSTALL_DEFAULT_TARGET "make uninstall"

/**
 * Uninstall the package represented by the
 * given `owner`, `name` and `version`.
 */

int
clib_uninstall(const char *, const char *, const char *);

#endif
