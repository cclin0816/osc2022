#pragma once

#include <conf.h>
#include <config.h>

extern char __linker_start;
extern char __linker_end;

#define LINK_VAR(x) (uint64_t)(&x)