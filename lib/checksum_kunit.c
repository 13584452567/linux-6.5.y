// SPDX-License-Identifier: GPL-2.0+
/*
 * Test cases csum_partial and csum_fold
 */

#include <kunit/test.h>
#include <asm/checksum.h>

#define MAX_LEN 512
#define MAX_ALIGN 64
#define TEST_BUFLEN (MAX_LEN + MAX_ALIGN)

/* Values for a little endian CPU. Byte swap each half on big endian CPU. */
static const u32 random_init_sum = 0x2847aab;
static const u8 random_buf[] = {
	0xac, 0xd7, 0x76, 0x69, 0x6e, 0xf2, 0x93, 0x2c, 0x1f, 0xe0, 0xde, 0x86,
	0x8f, 0x54, 0x33, 0x90, 0x95, 0xbf, 0xff, 0xb9, 0xea, 0x62, 0x6e, 0xb5,
	0xd3, 0x4f, 0xf5, 0x60, 0x50, 0x5c, 0xc7, 0xfa, 0x6d, 0x1a, 0xc7, 0xf0,
	0xd2, 0x2c, 0x12, 0x3d, 0x88, 0xe3, 0x14, 0x21, 0xb1, 0x5e, 0x45, 0x31,
	0xa2, 0x85, 0x36, 0x76, 0xba, 0xd8, 0xad, 0xbb, 0x9e, 0x49, 0x8f, 0xf7,
	0xce, 0xea, 0xef, 0xca, 0x2c, 0x29, 0xf7, 0x15, 0x5c, 0x1d, 0x4d, 0x09,
	0x1f, 0xe2, 0x14, 0x31, 0x8c, 0x07, 0x57, 0x23, 0x1f, 0x6f, 0x03, 0xe1,
	0x93, 0x19, 0x53, 0x03, 0x45, 0x49, 0x9a, 0x3b, 0x8e, 0x0c, 0x12, 0x5d,
	0x8a, 0xb8, 0x9b, 0x8c, 0x9a, 0x03, 0xe5, 0xa2, 0x43, 0xd2, 0x3b, 0x4e,
	0x7e, 0x30, 0x3c, 0x22, 0x2d, 0xc5, 0xfc, 0x9e, 0xdb, 0xc6, 0xf9, 0x69,
	0x12, 0x39, 0x1f, 0xa0, 0x11, 0x0c, 0x3f, 0xf5, 0x53, 0xc9, 0x30, 0xfb,
	0xb0, 0xdd, 0x21, 0x1d, 0x34, 0xe2, 0x65, 0x30, 0xf1, 0xe8, 0x1b, 0xe7,
	0x55, 0x0d, 0xeb, 0xbd, 0xcc, 0x9d, 0x24, 0xa4, 0xad, 0xa7, 0x93, 0x47,
	0x19, 0x2e, 0xc4, 0x5c, 0x3b, 0xc7, 0x6d, 0x95, 0x0c, 0x47, 0x60, 0xaf,
	0x5b, 0x47, 0xee, 0xdc, 0x31, 0x31, 0x14, 0x12, 0x7e, 0x9e, 0x45, 0xb1,
	0xc1, 0x69, 0x4b, 0x84, 0xfc, 0x88, 0xc1, 0x9e, 0x46, 0xb4, 0xc2, 0x25,
	0xc5, 0x6c, 0x4c, 0x22, 0x58, 0x5c, 0xbe, 0xff, 0xea, 0x88, 0x88, 0x7a,
	0xcb, 0x1c, 0x5d, 0x63, 0xa1, 0xf2, 0x33, 0x0c, 0xa2, 0x16, 0x0b, 0x6e,
	0x2b, 0x79, 0x58, 0xf7, 0xac, 0xd3, 0x6a, 0x3f, 0x81, 0x57, 0x48, 0x45,
	0xe3, 0x7c, 0xdc, 0xd6, 0x34, 0x7e, 0xe6, 0x73, 0xfa, 0xcb, 0x31, 0x18,
	0xa9, 0x0b, 0xee, 0x6b, 0x99, 0xb9, 0x2d, 0xde, 0x22, 0x0e, 0x71, 0x57,
	0x0e, 0x9b, 0x11, 0xd1, 0x15, 0x41, 0xd0, 0x6b, 0x50, 0x8a, 0x23, 0x64,
	0xe3, 0x9c, 0xb3, 0x55, 0x09, 0xe9, 0x32, 0x67, 0xf9, 0xe0, 0x73, 0xf1,
	0x60, 0x66, 0x0b, 0x88, 0x79, 0x8d, 0x4b, 0x52, 0x83, 0x20, 0x26, 0x78,
	0x49, 0x27, 0xe7, 0x3e, 0x29, 0xa8, 0x18, 0x82, 0x41, 0xdd, 0x1e, 0xcc,
	0x3b, 0xc4, 0x65, 0xd1, 0x21, 0x40, 0x72, 0xb2, 0x87, 0x5e, 0x16, 0x10,
	0x80, 0x3f, 0x4b, 0x58, 0x1c, 0xc2, 0x79, 0x20, 0xf0, 0xe0, 0x80, 0xd3,
	0x52, 0xa5, 0x19, 0x6e, 0x47, 0x90, 0x08, 0xf5, 0x50, 0xe2, 0xd6, 0xae,
	0xe9, 0x2e, 0xdc, 0xd5, 0xb4, 0x90, 0x1f, 0x79, 0x49, 0x82, 0x21, 0x84,
	0xa0, 0xb5, 0x2f, 0xff, 0x30, 0x71, 0xed, 0x80, 0x68, 0xb1, 0x6d, 0xef,
	0xf6, 0xcf, 0xb8, 0x41, 0x79, 0xf5, 0x01, 0xbc, 0x0c, 0x9b, 0x0e, 0x06,
	0xf3, 0xb0, 0xbb, 0x97, 0xb8, 0xb1, 0xfd, 0x51, 0x4e, 0xef, 0x0a, 0x3d,
	0x7a, 0x3d, 0xbd, 0x61, 0x00, 0xa2, 0xb3, 0xf0, 0x1d, 0x77, 0x7b, 0x6c,
	0x01, 0x61, 0xa5, 0xa3, 0xdb, 0xd5, 0xd5, 0xf4, 0xb5, 0x28, 0x9f, 0x0a,
	0xa3, 0x82, 0x5f, 0x4b, 0x40, 0x0f, 0x05, 0x0e, 0x78, 0xed, 0xbf, 0x17,
	0xf6, 0x5a, 0x8a, 0x7d, 0xf9, 0x45, 0xc1, 0xd7, 0x1b, 0x9d, 0x6c, 0x07,
	0x88, 0xf3, 0xbc, 0xf1, 0xea, 0x28, 0x1f, 0xb8, 0x7a, 0x60, 0x3c, 0xce,
	0x3e, 0x50, 0xb2, 0x0b, 0xcf, 0xe5, 0x08, 0x1f, 0x48, 0x04, 0xf9, 0x35,
	0x29, 0x15, 0xbe, 0x82, 0x96, 0xc2, 0x55, 0x04, 0x6c, 0x19, 0x45, 0x29,
	0x0b, 0xb6, 0x49, 0x12, 0xfb, 0x8d, 0x1b, 0x75, 0x8b, 0xd9, 0x6a, 0x5c,
	0xbe, 0x46, 0x2b, 0x41, 0xfe, 0x21, 0xad, 0x1f, 0x75, 0xe7, 0x90, 0x3d,
	0xe1, 0xdf, 0x4b, 0xe1, 0x81, 0xe2, 0x17, 0x02, 0x7b, 0x58, 0x8b, 0x92,
	0x1a, 0xac, 0x46, 0xdd, 0x2e, 0xce, 0x40, 0x09
};

/* Values for a little endian CPU. Byte swap on big endian CPU. */
static const u16 expected_results[] = {
	0x82d0, 0x8224, 0xab23, 0xaaad, 0x41ad, 0x413f, 0x4f3e, 0x4eab, 0x22ab,
	0x228c, 0x428b, 0x41ad, 0xbbac, 0xbb1d, 0x671d, 0x66ea, 0xd6e9, 0xd654,
	0x1754, 0x1655, 0x5d54, 0x5c6a, 0xfa69, 0xf9fb, 0x44fb, 0x4428, 0xf527,
	0xf432, 0x9432, 0x93e2, 0x37e2, 0x371b, 0x3d1a, 0x3cad, 0x22ad, 0x21e6,
	0x31e5, 0x3113, 0x0513, 0x0501, 0xc800, 0xc778, 0xe477, 0xe463, 0xc363,
	0xc2b2, 0x64b2, 0x646d, 0x336d, 0x32cb, 0xadca, 0xad94, 0x3794, 0x36da,
	0x5ed9, 0x5e2c, 0xa32b, 0xa28d, 0x598d, 0x58fe, 0x61fd, 0x612f, 0x772e,
	0x763f, 0xac3e, 0xac12, 0x8312, 0x821b, 0x6d1b, 0x6cbf, 0x4fbf, 0x4f72,
	0x4672, 0x4653, 0x6452, 0x643e, 0x333e, 0x32b2, 0x2bb2, 0x2b5b, 0x085b,
	0x083c, 0x993b, 0x9938, 0xb837, 0xb7a4, 0x9ea4, 0x9e51, 0x9b51, 0x9b0c,
	0x520c, 0x5172, 0x1672, 0x15e4, 0x09e4, 0x09d2, 0xacd1, 0xac47, 0xf446,
	0xf3ab, 0x67ab, 0x6711, 0x6411, 0x632c, 0xc12b, 0xc0e8, 0xeee7, 0xeeac,
	0xa0ac, 0xa02e, 0x702e, 0x6ff2, 0x4df2, 0x4dc5, 0x88c4, 0x87c8, 0xe9c7,
	0xe8ec, 0x22ec, 0x21f3, 0xb8f2, 0xb8e0, 0x7fe0, 0x7fc1, 0xdfc0, 0xdfaf,
	0xd3af, 0xd370, 0xde6f, 0xde1c, 0x151c, 0x14ec, 0x19eb, 0x193b, 0x3c3a,
	0x3c19, 0x1f19, 0x1ee5, 0x3ce4, 0x3c7f, 0x0c7f, 0x0b8e, 0x238d, 0x2372,
	0x3c71, 0x3c1c, 0x2f1c, 0x2e31, 0x7130, 0x7064, 0xd363, 0xd33f, 0x2f3f,
	0x2e92, 0x8791, 0x86fe, 0x3ffe, 0x3fe5, 0x11e5, 0x1121, 0xb520, 0xb4e5,
	0xede4, 0xed77, 0x5877, 0x586b, 0x116b, 0x110b, 0x620a, 0x61af, 0x1aaf,
	0x19c1, 0x3dc0, 0x3d8f, 0x0c8f, 0x0c7b, 0xfa7a, 0xf9fc, 0x5bfc, 0x5bb7,
	0xaab6, 0xa9f5, 0x40f5, 0x40aa, 0xbca9, 0xbbad, 0x33ad, 0x32ec, 0x94eb,
	0x94a5, 0xe0a4, 0xdfe2, 0xbae2, 0xba1d, 0x4e1d, 0x4dd1, 0x2bd1, 0x2b79,
	0xcf78, 0xceba, 0xcfb9, 0xcecf, 0x46cf, 0x4647, 0xcc46, 0xcb7b, 0xaf7b,
	0xaf1e, 0x4c1e, 0x4b7d, 0x597c, 0x5949, 0x4d49, 0x4ca7, 0x36a7, 0x369c,
	0xc89b, 0xc870, 0x4f70, 0x4f18, 0x5817, 0x576b, 0x846a, 0x8400, 0x4500,
	0x447f, 0xed7e, 0xed36, 0xa836, 0xa753, 0x2b53, 0x2a77, 0x5476, 0x5442,
	0xd641, 0xd55b, 0x625b, 0x6161, 0x9660, 0x962f, 0x7e2f, 0x7d86, 0x7286,
	0x7198, 0x0698, 0x05ff, 0x4cfe, 0x4cd1, 0x6ed0, 0x6eae, 0x60ae, 0x603d,
	0x093d, 0x092f, 0x6e2e, 0x6e1d, 0x9d1c, 0x9d07, 0x5c07, 0x5b37, 0xf036,
	0xefe6, 0x65e6, 0x65c3, 0x01c3, 0x00e0, 0x64df, 0x642c, 0x0f2c, 0x0f23,
	0x2622, 0x25f0, 0xbeef, 0xbdf6, 0xddf5, 0xdd82, 0xec81, 0xec21, 0x8621,
	0x8616, 0xfe15, 0xfd9c, 0x709c, 0x7051, 0x1e51, 0x1dce, 0xfdcd, 0xfda7,
	0x85a7, 0x855e, 0x5e5e, 0x5d77, 0x1f77, 0x1f4e, 0x774d, 0x7735, 0xf534,
	0xf4f3, 0x17f3, 0x17d5, 0x4bd4, 0x4b99, 0x8798, 0x8733, 0xb632, 0xb611,
	0x7611, 0x759f, 0xc39e, 0xc317, 0x6517, 0x6501, 0x5501, 0x5481, 0x1581,
	0x1536, 0xbd35, 0xbd19, 0xfb18, 0xfa9f, 0xda9f, 0xd9af, 0xf9ae, 0xf92e,
	0x262e, 0x25dc, 0x80db, 0x80c2, 0x12c2, 0x127b, 0x827a, 0x8272, 0x8d71,
	0x8d21, 0xab20, 0xaa4a, 0xfc49, 0xfb60, 0xcd60, 0xcc84, 0xf783, 0xf6cf,
	0x66cf, 0x66b0, 0xedaf, 0xed66, 0x6b66, 0x6b45, 0xe744, 0xe6a4, 0x31a4,
	0x3175, 0x3274, 0x3244, 0xc143, 0xc056, 0x4056, 0x3fee, 0x8eed, 0x8e80,
	0x9f7f, 0x9e89, 0xcf88, 0xced0, 0x8dd0, 0x8d57, 0x9856, 0x9855, 0xdc54,
	0xdc48, 0x4148, 0x413a, 0x3b3a, 0x3a47, 0x8a46, 0x898b, 0xf28a, 0xf1d2,
	0x40d2, 0x3fd5, 0xeed4, 0xee86, 0xff85, 0xff7b, 0xc27b, 0xc201, 0x8501,
	0x8444, 0x2344, 0x2344, 0x8143, 0x8090, 0x908f, 0x9072, 0x1972, 0x18f7,
	0xacf6, 0xacf5, 0x4bf5, 0x4b50, 0xa84f, 0xa774, 0xd273, 0xd19e, 0xdd9d,
	0xdce8, 0xb4e8, 0xb449, 0xaa49, 0xa9a6, 0x27a6, 0x2747, 0xdc46, 0xdc06,
	0xcd06, 0xcd01, 0xbf01, 0xbe89, 0xd188, 0xd0c9, 0xb9c9, 0xb8d3, 0x5ed3,
	0x5e49, 0xe148, 0xe04f, 0x9b4f, 0x9a8e, 0xc38d, 0xc372, 0x2672, 0x2606,
	0x1f06, 0x1e7e, 0x2b7d, 0x2ac1, 0x39c0, 0x38d6, 0x10d6, 0x10b7, 0x58b6,
	0x583c, 0xf83b, 0xf7ff, 0x29ff, 0x29c1, 0xd9c0, 0xd90e, 0xce0e, 0xcd3f,
	0xe83e, 0xe836, 0xc936, 0xc8ee, 0xc4ee, 0xc3f5, 0x8ef5, 0x8ecc, 0x79cc,
	0x790e, 0xf70d, 0xf677, 0x3477, 0x3422, 0x3022, 0x2fb6, 0x16b6, 0x1671,
	0xed70, 0xed65, 0x3765, 0x371c, 0x251c, 0x2421, 0x9720, 0x9705, 0x2205,
	0x217a, 0x4879, 0x480f, 0xec0e, 0xeb50, 0xa550, 0xa525, 0x6425, 0x6327,
	0x4227, 0x417a, 0x227a, 0x2205, 0x3b04, 0x3a74, 0xfd73, 0xfc92, 0x1d92,
	0x1d47, 0x3c46, 0x3bc5, 0x59c4, 0x59ad, 0x57ad, 0x5732, 0xff31, 0xfea6,
	0x6ca6, 0x6c8c, 0xc08b, 0xc045, 0xe344, 0xe316, 0x1516, 0x14d6,
};

/* Values for a little endian CPU. Byte swap each half on big endian CPU. */
static const u32 init_sums_no_overflow[] = {
	0xffffffff, 0xfffffffb, 0xfffffbfb, 0xfffffbf7, 0xfffff7f7, 0xfffff7f3,
	0xfffff3f3, 0xfffff3ef, 0xffffefef, 0xffffefeb, 0xffffebeb, 0xffffebe7,
	0xffffe7e7, 0xffffe7e3, 0xffffe3e3, 0xffffe3df, 0xffffdfdf, 0xffffdfdb,
	0xffffdbdb, 0xffffdbd7, 0xffffd7d7, 0xffffd7d3, 0xffffd3d3, 0xffffd3cf,
	0xffffcfcf, 0xffffcfcb, 0xffffcbcb, 0xffffcbc7, 0xffffc7c7, 0xffffc7c3,
	0xffffc3c3, 0xffffc3bf, 0xffffbfbf, 0xffffbfbb, 0xffffbbbb, 0xffffbbb7,
	0xffffb7b7, 0xffffb7b3, 0xffffb3b3, 0xffffb3af, 0xffffafaf, 0xffffafab,
	0xffffabab, 0xffffaba7, 0xffffa7a7, 0xffffa7a3, 0xffffa3a3, 0xffffa39f,
	0xffff9f9f, 0xffff9f9b, 0xffff9b9b, 0xffff9b97, 0xffff9797, 0xffff9793,
	0xffff9393, 0xffff938f, 0xffff8f8f, 0xffff8f8b, 0xffff8b8b, 0xffff8b87,
	0xffff8787, 0xffff8783, 0xffff8383, 0xffff837f, 0xffff7f7f, 0xffff7f7b,
	0xffff7b7b, 0xffff7b77, 0xffff7777, 0xffff7773, 0xffff7373, 0xffff736f,
	0xffff6f6f, 0xffff6f6b, 0xffff6b6b, 0xffff6b67, 0xffff6767, 0xffff6763,
	0xffff6363, 0xffff635f, 0xffff5f5f, 0xffff5f5b, 0xffff5b5b, 0xffff5b57,
	0xffff5757, 0xffff5753, 0xffff5353, 0xffff534f, 0xffff4f4f, 0xffff4f4b,
	0xffff4b4b, 0xffff4b47, 0xffff4747, 0xffff4743, 0xffff4343, 0xffff433f,
	0xffff3f3f, 0xffff3f3b, 0xffff3b3b, 0xffff3b37, 0xffff3737, 0xffff3733,
	0xffff3333, 0xffff332f, 0xffff2f2f, 0xffff2f2b, 0xffff2b2b, 0xffff2b27,
	0xffff2727, 0xffff2723, 0xffff2323, 0xffff231f, 0xffff1f1f, 0xffff1f1b,
	0xffff1b1b, 0xffff1b17, 0xffff1717, 0xffff1713, 0xffff1313, 0xffff130f,
	0xffff0f0f, 0xffff0f0b, 0xffff0b0b, 0xffff0b07, 0xffff0707, 0xffff0703,
	0xffff0303, 0xffff02ff, 0xfffffefe, 0xfffffefa, 0xfffffafa, 0xfffffaf6,
	0xfffff6f6, 0xfffff6f2, 0xfffff2f2, 0xfffff2ee, 0xffffeeee, 0xffffeeea,
	0xffffeaea, 0xffffeae6, 0xffffe6e6, 0xffffe6e2, 0xffffe2e2, 0xffffe2de,
	0xffffdede, 0xffffdeda, 0xffffdada, 0xffffdad6, 0xffffd6d6, 0xffffd6d2,
	0xffffd2d2, 0xffffd2ce, 0xffffcece, 0xffffceca, 0xffffcaca, 0xffffcac6,
	0xffffc6c6, 0xffffc6c2, 0xffffc2c2, 0xffffc2be, 0xffffbebe, 0xffffbeba,
	0xffffbaba, 0xffffbab6, 0xffffb6b6, 0xffffb6b2, 0xffffb2b2, 0xffffb2ae,
	0xffffaeae, 0xffffaeaa, 0xffffaaaa, 0xffffaaa6, 0xffffa6a6, 0xffffa6a2,
	0xffffa2a2, 0xffffa29e, 0xffff9e9e, 0xffff9e9a, 0xffff9a9a, 0xffff9a96,
	0xffff9696, 0xffff9692, 0xffff9292, 0xffff928e, 0xffff8e8e, 0xffff8e8a,
	0xffff8a8a, 0xffff8a86, 0xffff8686, 0xffff8682, 0xffff8282, 0xffff827e,
	0xffff7e7e, 0xffff7e7a, 0xffff7a7a, 0xffff7a76, 0xffff7676, 0xffff7672,
	0xffff7272, 0xffff726e, 0xffff6e6e, 0xffff6e6a, 0xffff6a6a, 0xffff6a66,
	0xffff6666, 0xffff6662, 0xffff6262, 0xffff625e, 0xffff5e5e, 0xffff5e5a,
	0xffff5a5a, 0xffff5a56, 0xffff5656, 0xffff5652, 0xffff5252, 0xffff524e,
	0xffff4e4e, 0xffff4e4a, 0xffff4a4a, 0xffff4a46, 0xffff4646, 0xffff4642,
	0xffff4242, 0xffff423e, 0xffff3e3e, 0xffff3e3a, 0xffff3a3a, 0xffff3a36,
	0xffff3636, 0xffff3632, 0xffff3232, 0xffff322e, 0xffff2e2e, 0xffff2e2a,
	0xffff2a2a, 0xffff2a26, 0xffff2626, 0xffff2622, 0xffff2222, 0xffff221e,
	0xffff1e1e, 0xffff1e1a, 0xffff1a1a, 0xffff1a16, 0xffff1616, 0xffff1612,
	0xffff1212, 0xffff120e, 0xffff0e0e, 0xffff0e0a, 0xffff0a0a, 0xffff0a06,
	0xffff0606, 0xffff0602, 0xffff0202, 0xffff01fe, 0xfffffdfd, 0xfffffdf9,
	0xfffff9f9, 0xfffff9f5, 0xfffff5f5, 0xfffff5f1, 0xfffff1f1, 0xfffff1ed,
	0xffffeded, 0xffffede9, 0xffffe9e9, 0xffffe9e5, 0xffffe5e5, 0xffffe5e1,
	0xffffe1e1, 0xffffe1dd, 0xffffdddd, 0xffffddd9, 0xffffd9d9, 0xffffd9d5,
	0xffffd5d5, 0xffffd5d1, 0xffffd1d1, 0xffffd1cd, 0xffffcdcd, 0xffffcdc9,
	0xffffc9c9, 0xffffc9c5, 0xffffc5c5, 0xffffc5c1, 0xffffc1c1, 0xffffc1bd,
	0xffffbdbd, 0xffffbdb9, 0xffffb9b9, 0xffffb9b5, 0xffffb5b5, 0xffffb5b1,
	0xffffb1b1, 0xffffb1ad, 0xffffadad, 0xffffada9, 0xffffa9a9, 0xffffa9a5,
	0xffffa5a5, 0xffffa5a1, 0xffffa1a1, 0xffffa19d, 0xffff9d9d, 0xffff9d99,
	0xffff9999, 0xffff9995, 0xffff9595, 0xffff9591, 0xffff9191, 0xffff918d,
	0xffff8d8d, 0xffff8d89, 0xffff8989, 0xffff8985, 0xffff8585, 0xffff8581,
	0xffff8181, 0xffff817d, 0xffff7d7d, 0xffff7d79, 0xffff7979, 0xffff7975,
	0xffff7575, 0xffff7571, 0xffff7171, 0xffff716d, 0xffff6d6d, 0xffff6d69,
	0xffff6969, 0xffff6965, 0xffff6565, 0xffff6561, 0xffff6161, 0xffff615d,
	0xffff5d5d, 0xffff5d59, 0xffff5959, 0xffff5955, 0xffff5555, 0xffff5551,
	0xffff5151, 0xffff514d, 0xffff4d4d, 0xffff4d49, 0xffff4949, 0xffff4945,
	0xffff4545, 0xffff4541, 0xffff4141, 0xffff413d, 0xffff3d3d, 0xffff3d39,
	0xffff3939, 0xffff3935, 0xffff3535, 0xffff3531, 0xffff3131, 0xffff312d,
	0xffff2d2d, 0xffff2d29, 0xffff2929, 0xffff2925, 0xffff2525, 0xffff2521,
	0xffff2121, 0xffff211d, 0xffff1d1d, 0xffff1d19, 0xffff1919, 0xffff1915,
	0xffff1515, 0xffff1511, 0xffff1111, 0xffff110d, 0xffff0d0d, 0xffff0d09,
	0xffff0909, 0xffff0905, 0xffff0505, 0xffff0501, 0xffff0101, 0xffff00fd,
	0xfffffcfc, 0xfffffcf8, 0xfffff8f8, 0xfffff8f4, 0xfffff4f4, 0xfffff4f0,
	0xfffff0f0, 0xfffff0ec, 0xffffecec, 0xffffece8, 0xffffe8e8, 0xffffe8e4,
	0xffffe4e4, 0xffffe4e0, 0xffffe0e0, 0xffffe0dc, 0xffffdcdc, 0xffffdcd8,
	0xffffd8d8, 0xffffd8d4, 0xffffd4d4, 0xffffd4d0, 0xffffd0d0, 0xffffd0cc,
	0xffffcccc, 0xffffccc8, 0xffffc8c8, 0xffffc8c4, 0xffffc4c4, 0xffffc4c0,
	0xffffc0c0, 0xffffc0bc, 0xffffbcbc, 0xffffbcb8, 0xffffb8b8, 0xffffb8b4,
	0xffffb4b4, 0xffffb4b0, 0xffffb0b0, 0xffffb0ac, 0xffffacac, 0xffffaca8,
	0xffffa8a8, 0xffffa8a4, 0xffffa4a4, 0xffffa4a0, 0xffffa0a0, 0xffffa09c,
	0xffff9c9c, 0xffff9c98, 0xffff9898, 0xffff9894, 0xffff9494, 0xffff9490,
	0xffff9090, 0xffff908c, 0xffff8c8c, 0xffff8c88, 0xffff8888, 0xffff8884,
	0xffff8484, 0xffff8480, 0xffff8080, 0xffff807c, 0xffff7c7c, 0xffff7c78,
	0xffff7878, 0xffff7874, 0xffff7474, 0xffff7470, 0xffff7070, 0xffff706c,
	0xffff6c6c, 0xffff6c68, 0xffff6868, 0xffff6864, 0xffff6464, 0xffff6460,
	0xffff6060, 0xffff605c, 0xffff5c5c, 0xffff5c58, 0xffff5858, 0xffff5854,
	0xffff5454, 0xffff5450, 0xffff5050, 0xffff504c, 0xffff4c4c, 0xffff4c48,
	0xffff4848, 0xffff4844, 0xffff4444, 0xffff4440, 0xffff4040, 0xffff403c,
	0xffff3c3c, 0xffff3c38, 0xffff3838, 0xffff3834, 0xffff3434, 0xffff3430,
	0xffff3030, 0xffff302c, 0xffff2c2c, 0xffff2c28, 0xffff2828, 0xffff2824,
	0xffff2424, 0xffff2420, 0xffff2020, 0xffff201c, 0xffff1c1c, 0xffff1c18,
	0xffff1818, 0xffff1814, 0xffff1414, 0xffff1410, 0xffff1010, 0xffff100c,
	0xffff0c0c, 0xffff0c08, 0xffff0808, 0xffff0804, 0xffff0404, 0xffff0400,
	0xffff0000, 0xfffffffb,
};

static u8 tmp_buf[TEST_BUFLEN];

#define full_csum(buff, len, sum) csum_fold(csum_partial(buff, len, sum))

#define CHECK_EQ(lhs, rhs) KUNIT_ASSERT_EQ(test, (__force u64)lhs, (__force u64)rhs)

static __sum16 to_sum16(u16 x)
{
	return (__force __sum16)le16_to_cpu((__force __le16)x);
}

/* This function swaps the bytes inside each half of a __wsum */
static __wsum to_wsum(u32 x)
{
	u16 hi = le16_to_cpu((__force __le16)(x >> 16));
	u16 lo = le16_to_cpu((__force __le16)x);

	return (__force __wsum)((hi << 16) | lo);
}

static void assert_setup_correct(struct kunit *test)
{
	CHECK_EQ(sizeof(random_buf) / sizeof(random_buf[0]), MAX_LEN);
	CHECK_EQ(sizeof(expected_results) / sizeof(expected_results[0]),
		 MAX_LEN);
	CHECK_EQ(sizeof(init_sums_no_overflow) /
			 sizeof(init_sums_no_overflow[0]),
		 MAX_LEN);
}

/*
 * Test with randomized input (pre determined random with known results).
 */
static void test_csum_fixed_random_inputs(struct kunit *test)
{
	int len, align;
	__wsum sum;
	__sum16 result, expec;

	assert_setup_correct(test);
	for (align = 0; align < TEST_BUFLEN; ++align) {
		memcpy(&tmp_buf[align], random_buf,
		       min(MAX_LEN, TEST_BUFLEN - align));
		for (len = 0; len < MAX_LEN && (align + len) < TEST_BUFLEN;
		     ++len) {
			/*
			 * Test the precomputed random input.
			 */
			sum = to_wsum(random_init_sum);
			result = full_csum(&tmp_buf[align], len, sum);
			expec = to_sum16(expected_results[len]);
			CHECK_EQ(result, expec);
		}
	}
}

/*
 * All ones input test. If there are any missing carry operations, it fails.
 */
static void test_csum_all_carry_inputs(struct kunit *test)
{
	int len, align;
	__wsum sum;
	__sum16 result, expec;

	assert_setup_correct(test);
	memset(tmp_buf, 0xff, TEST_BUFLEN);
	for (align = 0; align < TEST_BUFLEN; ++align) {
		for (len = 0; len < MAX_LEN && (align + len) < TEST_BUFLEN;
		     ++len) {
			/*
			 * All carries from input and initial sum.
			 */
			sum = to_wsum(0xffffffff);
			result = full_csum(&tmp_buf[align], len, sum);
			expec = to_sum16((len & 1) ? 0xff00 : 0);
			CHECK_EQ(result, expec);

			/*
			 * All carries from input.
			 */
			sum = 0;
			result = full_csum(&tmp_buf[align], len, sum);
			if (len & 1)
				expec = to_sum16(0xff00);
			else if (len)
				expec = 0;
			else
				expec = to_sum16(0xffff);
			CHECK_EQ(result, expec);
		}
	}
}

/*
 * Test with input that alone doesn't cause any carries. By selecting the
 * maximum initial sum, this allows us to test that there are no carries
 * where there shouldn't be.
 */
static void test_csum_no_carry_inputs(struct kunit *test)
{
	int len, align;
	__wsum sum;
	__sum16 result, expec;

	assert_setup_correct(test);
	memset(tmp_buf, 0x4, TEST_BUFLEN);
	for (align = 0; align < TEST_BUFLEN; ++align) {
		for (len = 0; len < MAX_LEN && (align + len) < TEST_BUFLEN;
		     ++len) {
			/*
			 * Expect no carries.
			 */
			sum = to_wsum(init_sums_no_overflow[len]);
			result = full_csum(&tmp_buf[align], len, sum);
			expec = 0;
			CHECK_EQ(result, expec);

			/*
			 * Expect one carry.
			 */
			sum = to_wsum(init_sums_no_overflow[len] + 1);
			result = full_csum(&tmp_buf[align], len, sum);
			expec = to_sum16(len ? 0xfffe : 0xffff);
			CHECK_EQ(result, expec);
		}
	}
}

static struct kunit_case __refdata checksum_test_cases[] = {
	KUNIT_CASE(test_csum_fixed_random_inputs),
	KUNIT_CASE(test_csum_all_carry_inputs),
	KUNIT_CASE(test_csum_no_carry_inputs),
	{}
};

static struct kunit_suite checksum_test_suite = {
	.name = "checksum",
	.test_cases = checksum_test_cases,
};

kunit_test_suites(&checksum_test_suite);

MODULE_AUTHOR("Noah Goldstein <goldstein.w.n@gmail.com>");
MODULE_LICENSE("GPL");
