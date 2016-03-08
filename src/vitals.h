#ifndef STRING_H_
# define STRING_H_

# include <stddef.h>
# include "plt.h"

extern void *(*mmk_malloc)(size_t);
extern void *(*mmk_realloc)(void *, size_t);
extern void (*mmk_free)(void *);

/* string functions that are notoriously replaced by sse2 variants are
 * reimplemented. */
int mmk_memcmp(const void *s1, const void *s2, size_t n);
void *mmk_memcpy(void *dst, const void *src, size_t n);
char *mmk_strchr(const char *s, int c);
int mmk_strneq(const char *src, const char *ref, size_t n);
int mmk_isspace(int c);

void mmk_init_vital_functions (plt_ctx ctx);

#endif /* !STRING_H_ */
