#ifndef STRDUP_H_
# define STRDUP_H_

# ifdef _MSC_VER
#  ifdef DLL_EXPORT
#   define API __declspec(dllexport)
#  else
#   define API __declspec(dllimport)
#  endif
# else
#  define API
# endif

API char *my_strdup(const char *s);

#endif /* !STRDUP_H_ */
