#ifndef LIBFOO_H_
# define LIBFOO_H_

# if defined _WIN32 || defined __CYGWIN__
#  ifdef foo_EXPORTS
#   ifdef __GNUC__
#    define FOO_API __attribute__((dllexport))
#   else
#    define FOO_API __declspec(dllexport)
#   endif
#  else
#   ifdef __GNUC__
#    define FOO_API __attribute__((dllimport))
#   else
#    define FOO_API __declspec(dllimport)
#   endif
#  endif
#  define FOO_LOCAL
# else
#  if __GNUC__ >= 4
#   define FOO_API   __attribute__((visibility("default")))
#   define FOO_LOCAL __attribute__((visibility("hidden")))
#  else
#   define FOO_API
#   define FOO_LOCAL
#  endif
# endif

FOO_API void fn_vv(void);
FOO_API void fn_vi(int i);
FOO_API int fn_iv(void);
FOO_API int fn_ii(int i);
FOO_API void fn_vli(long l, int i);
FOO_API int fn_ili(long l, int i);
FOO_API void fn_vi_va(int i, ...);
FOO_API int fn_ii_va(int i, ...);

#endif /* !LIBFOO_H_ */
