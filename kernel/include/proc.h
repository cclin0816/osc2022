#pragma once

#include <bsl/string_view.h>
#include <conf.h>
#include <config.h>

namespace proc {

void thread_create(void (*func)(void *), void *arg);
void execve(bsl::sv_t path, const char *const argv[], const char *const envp[]);
void idle(void *);
void thread_exit();

}  // namespace proc
