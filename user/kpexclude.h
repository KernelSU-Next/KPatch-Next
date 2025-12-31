#ifndef _KPU_KPEXCLUDE_H
#define _KPU_KPEXCLUDE_H


#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

long set_uid_exclude(uid_t uid, int exclude);
long get_uid_exclude(uid_t uid);

int kpexclude_set_main(int argc, char **argv);
int kpexclude_get_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif