#ifndef PLT_ELF_H_
# define PLT_ELF_H_

#include <link.h>

typedef struct link_map *plt_lib;
typedef struct r_debug *plt_ctx;
typedef void *plt_got;
typedef void (plt_fn)(void);

#endif /* !PLT_ELF_H_ */
