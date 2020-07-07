#ifndef MMK_ALLOC_H_
# define MMK_ALLOC_H_

# ifdef __cplusplus
extern "C" {
# endif

void *mmk_malloc(size_t size);
void *mmk_realloc(void *ptr, size_t size);
void mmk_free(void *ptr);

# ifdef __cplusplus
}
# endif

#endif /* !MMK_ALLOC_H_ */
