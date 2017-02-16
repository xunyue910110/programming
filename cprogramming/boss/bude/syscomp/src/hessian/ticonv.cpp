#include <iconv.h>
size_t  ticonv(iconv_t c, char** in, size_t* inlen, char** out, size_t* outlen) {
        return iconv(c, in, inlen, out, outlen);
}
