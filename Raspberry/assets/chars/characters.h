// Font: LiberationSansNarrow-Bold.ttf

#ifndef _CHARACTERS_H_
#define _CHARACTERS_H_

#include <cstdint>

extern const unsigned char punctuation_0[];
extern const unsigned char punctuation_1[];
extern const unsigned char punctuation_2[];
extern const unsigned char punctuation_3[];
extern const unsigned char punctuation_4[];
extern const unsigned char punctuation_5[];
extern const unsigned char punctuation_6[];
extern const unsigned char punctuation_7[];
extern const unsigned char punctuation_8[];
extern const unsigned char punctuation_9[];
extern const unsigned char punctuation_10[];
extern const unsigned char punctuation_11[];
extern const unsigned char punctuation_12[];
extern const unsigned char punctuation_13[];
extern const unsigned char punctuation_14[];
extern const unsigned char char_0[];
extern const unsigned char char_1[];
extern const unsigned char char_2[];
extern const unsigned char char_3[];
extern const unsigned char char_4[];
extern const unsigned char char_5[];
extern const unsigned char char_6[];
extern const unsigned char char_7[];
extern const unsigned char char_8[];
extern const unsigned char char_9[];
extern const unsigned char char_A[];
extern const unsigned char char_B[];
extern const unsigned char char_C[];
extern const unsigned char char_D[];
extern const unsigned char char_E[];
extern const unsigned char char_F[];
extern const unsigned char char_G[];
extern const unsigned char char_H[];
extern const unsigned char char_I[];
extern const unsigned char char_J[];
extern const unsigned char char_K[];
extern const unsigned char char_L[];
extern const unsigned char char_M[];
extern const unsigned char char_N[];
extern const unsigned char char_O[];
extern const unsigned char char_P[];
extern const unsigned char char_Q[];
extern const unsigned char char_R[];
extern const unsigned char char_S[];
extern const unsigned char char_T[];
extern const unsigned char char_U[];
extern const unsigned char char_V[];
extern const unsigned char char_W[];
extern const unsigned char char_X[];
extern const unsigned char char_Y[];
extern const unsigned char char_Z[];
extern const unsigned char char_a[];
extern const unsigned char char_b[];
extern const unsigned char char_c[];
extern const unsigned char char_d[];
extern const unsigned char char_e[];
extern const unsigned char char_f[];
extern const unsigned char char_g[];
extern const unsigned char char_h[];
extern const unsigned char char_i[];
extern const unsigned char char_j[];
extern const unsigned char char_k[];
extern const unsigned char char_l[];
extern const unsigned char char_m[];
extern const unsigned char char_n[];
extern const unsigned char char_o[];
extern const unsigned char char_p[];
extern const unsigned char char_q[];
extern const unsigned char char_r[];
extern const unsigned char char_s[];
extern const unsigned char char_t[];
extern const unsigned char char_u[];
extern const unsigned char char_v[];
extern const unsigned char char_w[];
extern const unsigned char char_x[];
extern const unsigned char char_y[];
extern const unsigned char char_z[];


struct _characters
{
    const uint16_t width;
    const uint16_t height;
    const unsigned char *characters[];
};

extern const _characters characters;

#endif