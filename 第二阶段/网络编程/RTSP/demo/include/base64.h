#ifndef BASE64_H
#define BASE64_H
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
void	base64_bits_to_64(unsigned char *out, const unsigned char *in, int inlen);
int		base64_64_to_bits(char *out, const char *in);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif//BASE64_H
