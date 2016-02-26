#ifndef TRAMPOLINE_H_
# define TRAMPOLINE_H_

# include "plt.h"

plt_fn *create_trampoline (void *ctx, plt_fn *routine);
void destroy_trampoline (plt_fn *trampoline);

#endif /* !TRAMPOLINE_H_ */
