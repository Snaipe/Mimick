#ifndef MMK_ALLOC_H_
# define MMK_ALLOC_H_

void *mmk_malloc(size_t size);
void *mmk_realloc(void *ptr, size_t size);
void mmk_free(void *ptr);

#endif /* !MMK_ALLOC_H_ */
