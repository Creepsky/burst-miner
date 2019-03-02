/* $Id: shabal.c 175 2010-05-07 16:03:20Z tp $ */
/*
* Shabal implementation.
*
* ==========================(LICENSE BEGIN)============================
*
* Copyright (c) 2007-2010  Projet RNRT SAPHIR
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* ===========================(LICENSE END)=============================
*
* @author            Thomas Pornin <thomas.pornin@cryptolog.com>
* @cuda author		 Creepsky      <creepsky@gmail.com>
* 
*/

#include "shabal/sphlib/sph_shabal.h"
#include "Shabal.hpp"
#include <string>
#include <thrust/device_ptr.h>
#include <thrust/extrema.h>

using namespace Burst;

#define SPH_C32(x)    ((sph_u32)(x ## U))
#define C32   SPH_C32

#define sM    16

#define C32   SPH_C32
#define T32   SPH_T32

#define O1   13
#define O2    9
#define O3    6

/*
* We copy the state into local variables, so that the compiler knows
* that it can optimize them at will.
*/

/* BEGIN -- automatically generated code. */

#define DECL_STATE   \
	sph_u32 A00, A01, A02, A03, A04, A05, A06, A07, \
	        A08, A09, A0A, A0B; \
	sph_u32 B0, B1, B2, B3, B4, B5, B6, B7, \
	        B8, B9, BA, BB, BC, BD, BE, BF; \
	sph_u32 C0, C1, C2, C3, C4, C5, C6, C7, \
	        C8, C9, CA, CB, CC, CD, CE, CF; \
	sph_u32 M0, M1, M2, M3, M4, M5, M6, M7, \
	        M8, M9, MA, MB, MC, MD, ME, MF; \
	sph_u32 Wlow, Whigh;

#define READ_STATE(state)   do { \
		A00 = (state)->A[0]; \
		A01 = (state)->A[1]; \
		A02 = (state)->A[2]; \
		A03 = (state)->A[3]; \
		A04 = (state)->A[4]; \
		A05 = (state)->A[5]; \
		A06 = (state)->A[6]; \
		A07 = (state)->A[7]; \
		A08 = (state)->A[8]; \
		A09 = (state)->A[9]; \
		A0A = (state)->A[10]; \
		A0B = (state)->A[11]; \
		B0 = (state)->B[0]; \
		B1 = (state)->B[1]; \
		B2 = (state)->B[2]; \
		B3 = (state)->B[3]; \
		B4 = (state)->B[4]; \
		B5 = (state)->B[5]; \
		B6 = (state)->B[6]; \
		B7 = (state)->B[7]; \
		B8 = (state)->B[8]; \
		B9 = (state)->B[9]; \
		BA = (state)->B[10]; \
		BB = (state)->B[11]; \
		BC = (state)->B[12]; \
		BD = (state)->B[13]; \
		BE = (state)->B[14]; \
		BF = (state)->B[15]; \
		C0 = (state)->C[0]; \
		C1 = (state)->C[1]; \
		C2 = (state)->C[2]; \
		C3 = (state)->C[3]; \
		C4 = (state)->C[4]; \
		C5 = (state)->C[5]; \
		C6 = (state)->C[6]; \
		C7 = (state)->C[7]; \
		C8 = (state)->C[8]; \
		C9 = (state)->C[9]; \
		CA = (state)->C[10]; \
		CB = (state)->C[11]; \
		CC = (state)->C[12]; \
		CD = (state)->C[13]; \
		CE = (state)->C[14]; \
		CF = (state)->C[15]; \
		Wlow = (state)->Wlow; \
		Whigh = (state)->Whigh; \
	} while (0)

#define WRITE_STATE(state)   do { \
		(state)->A[0] = A00; \
		(state)->A[1] = A01; \
		(state)->A[2] = A02; \
		(state)->A[3] = A03; \
		(state)->A[4] = A04; \
		(state)->A[5] = A05; \
		(state)->A[6] = A06; \
		(state)->A[7] = A07; \
		(state)->A[8] = A08; \
		(state)->A[9] = A09; \
		(state)->A[10] = A0A; \
		(state)->A[11] = A0B; \
		(state)->B[0] = B0; \
		(state)->B[1] = B1; \
		(state)->B[2] = B2; \
		(state)->B[3] = B3; \
		(state)->B[4] = B4; \
		(state)->B[5] = B5; \
		(state)->B[6] = B6; \
		(state)->B[7] = B7; \
		(state)->B[8] = B8; \
		(state)->B[9] = B9; \
		(state)->B[10] = BA; \
		(state)->B[11] = BB; \
		(state)->B[12] = BC; \
		(state)->B[13] = BD; \
		(state)->B[14] = BE; \
		(state)->B[15] = BF; \
		(state)->C[0] = C0; \
		(state)->C[1] = C1; \
		(state)->C[2] = C2; \
		(state)->C[3] = C3; \
		(state)->C[4] = C4; \
		(state)->C[5] = C5; \
		(state)->C[6] = C6; \
		(state)->C[7] = C7; \
		(state)->C[8] = C8; \
		(state)->C[9] = C9; \
		(state)->C[10] = CA; \
		(state)->C[11] = CB; \
		(state)->C[12] = CC; \
		(state)->C[13] = CD; \
		(state)->C[14] = CE; \
		(state)->C[15] = CF; \
		(state)->Wlow = Wlow; \
		(state)->Whigh = Whigh; \
	} while (0)

#define DECODE_BLOCK   do { \
		M0 = cuda_sph_dec32le_aligned(buf + 0); \
		M1 = cuda_sph_dec32le_aligned(buf + 4); \
		M2 = cuda_sph_dec32le_aligned(buf + 8); \
		M3 = cuda_sph_dec32le_aligned(buf + 12); \
		M4 = cuda_sph_dec32le_aligned(buf + 16); \
		M5 = cuda_sph_dec32le_aligned(buf + 20); \
		M6 = cuda_sph_dec32le_aligned(buf + 24); \
		M7 = cuda_sph_dec32le_aligned(buf + 28); \
		M8 = cuda_sph_dec32le_aligned(buf + 32); \
		M9 = cuda_sph_dec32le_aligned(buf + 36); \
		MA = cuda_sph_dec32le_aligned(buf + 40); \
		MB = cuda_sph_dec32le_aligned(buf + 44); \
		MC = cuda_sph_dec32le_aligned(buf + 48); \
		MD = cuda_sph_dec32le_aligned(buf + 52); \
		ME = cuda_sph_dec32le_aligned(buf + 56); \
		MF = cuda_sph_dec32le_aligned(buf + 60); \
	} while (0)

#define INPUT_BLOCK_ADD   do { \
		B0 = T32(B0 + M0); \
		B1 = T32(B1 + M1); \
		B2 = T32(B2 + M2); \
		B3 = T32(B3 + M3); \
		B4 = T32(B4 + M4); \
		B5 = T32(B5 + M5); \
		B6 = T32(B6 + M6); \
		B7 = T32(B7 + M7); \
		B8 = T32(B8 + M8); \
		B9 = T32(B9 + M9); \
		BA = T32(BA + MA); \
		BB = T32(BB + MB); \
		BC = T32(BC + MC); \
		BD = T32(BD + MD); \
		BE = T32(BE + ME); \
		BF = T32(BF + MF); \
	} while (0)

#define INPUT_BLOCK_SUB   do { \
		C0 = T32(C0 - M0); \
		C1 = T32(C1 - M1); \
		C2 = T32(C2 - M2); \
		C3 = T32(C3 - M3); \
		C4 = T32(C4 - M4); \
		C5 = T32(C5 - M5); \
		C6 = T32(C6 - M6); \
		C7 = T32(C7 - M7); \
		C8 = T32(C8 - M8); \
		C9 = T32(C9 - M9); \
		CA = T32(CA - MA); \
		CB = T32(CB - MB); \
		CC = T32(CC - MC); \
		CD = T32(CD - MD); \
		CE = T32(CE - ME); \
		CF = T32(CF - MF); \
	} while (0)

#define XOR_W   do { \
		A00 ^= Wlow; \
		A01 ^= Whigh; \
	} while (0)

#define SWAP(v1, v2)   do { \
		sph_u32 tmp = (v1); \
		(v1) = (v2); \
		(v2) = tmp; \
	} while (0)

#define SWAP_BC   do { \
		SWAP(B0, C0); \
		SWAP(B1, C1); \
		SWAP(B2, C2); \
		SWAP(B3, C3); \
		SWAP(B4, C4); \
		SWAP(B5, C5); \
		SWAP(B6, C6); \
		SWAP(B7, C7); \
		SWAP(B8, C8); \
		SWAP(B9, C9); \
		SWAP(BA, CA); \
		SWAP(BB, CB); \
		SWAP(BC, CC); \
		SWAP(BD, CD); \
		SWAP(BE, CE); \
		SWAP(BF, CF); \
	} while (0)

#define PERM_ELT(xa0, xa1, xb0, xb1, xb2, xb3, xc, xm)   do { \
		xa0 = T32((xa0 \
			^ (((xa1 << 15) | (xa1 >> 17)) * 5U) \
			^ xc) * 3U) \
			^ xb1 ^ (xb2 & ~xb3) ^ xm; \
		xb0 = T32(~(((xb0 << 1) | (xb0 >> 31)) ^ xa0)); \
	} while (0)

#define PERM_STEP_0   do { \
		PERM_ELT(A00, A0B, B0, BD, B9, B6, C8, M0); \
		PERM_ELT(A01, A00, B1, BE, BA, B7, C7, M1); \
		PERM_ELT(A02, A01, B2, BF, BB, B8, C6, M2); \
		PERM_ELT(A03, A02, B3, B0, BC, B9, C5, M3); \
		PERM_ELT(A04, A03, B4, B1, BD, BA, C4, M4); \
		PERM_ELT(A05, A04, B5, B2, BE, BB, C3, M5); \
		PERM_ELT(A06, A05, B6, B3, BF, BC, C2, M6); \
		PERM_ELT(A07, A06, B7, B4, B0, BD, C1, M7); \
		PERM_ELT(A08, A07, B8, B5, B1, BE, C0, M8); \
		PERM_ELT(A09, A08, B9, B6, B2, BF, CF, M9); \
		PERM_ELT(A0A, A09, BA, B7, B3, B0, CE, MA); \
		PERM_ELT(A0B, A0A, BB, B8, B4, B1, CD, MB); \
		PERM_ELT(A00, A0B, BC, B9, B5, B2, CC, MC); \
		PERM_ELT(A01, A00, BD, BA, B6, B3, CB, MD); \
		PERM_ELT(A02, A01, BE, BB, B7, B4, CA, ME); \
		PERM_ELT(A03, A02, BF, BC, B8, B5, C9, MF); \
	} while (0)

#define PERM_STEP_1   do { \
		PERM_ELT(A04, A03, B0, BD, B9, B6, C8, M0); \
		PERM_ELT(A05, A04, B1, BE, BA, B7, C7, M1); \
		PERM_ELT(A06, A05, B2, BF, BB, B8, C6, M2); \
		PERM_ELT(A07, A06, B3, B0, BC, B9, C5, M3); \
		PERM_ELT(A08, A07, B4, B1, BD, BA, C4, M4); \
		PERM_ELT(A09, A08, B5, B2, BE, BB, C3, M5); \
		PERM_ELT(A0A, A09, B6, B3, BF, BC, C2, M6); \
		PERM_ELT(A0B, A0A, B7, B4, B0, BD, C1, M7); \
		PERM_ELT(A00, A0B, B8, B5, B1, BE, C0, M8); \
		PERM_ELT(A01, A00, B9, B6, B2, BF, CF, M9); \
		PERM_ELT(A02, A01, BA, B7, B3, B0, CE, MA); \
		PERM_ELT(A03, A02, BB, B8, B4, B1, CD, MB); \
		PERM_ELT(A04, A03, BC, B9, B5, B2, CC, MC); \
		PERM_ELT(A05, A04, BD, BA, B6, B3, CB, MD); \
		PERM_ELT(A06, A05, BE, BB, B7, B4, CA, ME); \
		PERM_ELT(A07, A06, BF, BC, B8, B5, C9, MF); \
	} while (0)

#define PERM_STEP_2   do { \
		PERM_ELT(A08, A07, B0, BD, B9, B6, C8, M0); \
		PERM_ELT(A09, A08, B1, BE, BA, B7, C7, M1); \
		PERM_ELT(A0A, A09, B2, BF, BB, B8, C6, M2); \
		PERM_ELT(A0B, A0A, B3, B0, BC, B9, C5, M3); \
		PERM_ELT(A00, A0B, B4, B1, BD, BA, C4, M4); \
		PERM_ELT(A01, A00, B5, B2, BE, BB, C3, M5); \
		PERM_ELT(A02, A01, B6, B3, BF, BC, C2, M6); \
		PERM_ELT(A03, A02, B7, B4, B0, BD, C1, M7); \
		PERM_ELT(A04, A03, B8, B5, B1, BE, C0, M8); \
		PERM_ELT(A05, A04, B9, B6, B2, BF, CF, M9); \
		PERM_ELT(A06, A05, BA, B7, B3, B0, CE, MA); \
		PERM_ELT(A07, A06, BB, B8, B4, B1, CD, MB); \
		PERM_ELT(A08, A07, BC, B9, B5, B2, CC, MC); \
		PERM_ELT(A09, A08, BD, BA, B6, B3, CB, MD); \
		PERM_ELT(A0A, A09, BE, BB, B7, B4, CA, ME); \
		PERM_ELT(A0B, A0A, BF, BC, B8, B5, C9, MF); \
	} while (0)

#define APPLY_P   do { \
		B0 = T32(B0 << 17) | (B0 >> 15); \
		B1 = T32(B1 << 17) | (B1 >> 15); \
		B2 = T32(B2 << 17) | (B2 >> 15); \
		B3 = T32(B3 << 17) | (B3 >> 15); \
		B4 = T32(B4 << 17) | (B4 >> 15); \
		B5 = T32(B5 << 17) | (B5 >> 15); \
		B6 = T32(B6 << 17) | (B6 >> 15); \
		B7 = T32(B7 << 17) | (B7 >> 15); \
		B8 = T32(B8 << 17) | (B8 >> 15); \
		B9 = T32(B9 << 17) | (B9 >> 15); \
		BA = T32(BA << 17) | (BA >> 15); \
		BB = T32(BB << 17) | (BB >> 15); \
		BC = T32(BC << 17) | (BC >> 15); \
		BD = T32(BD << 17) | (BD >> 15); \
		BE = T32(BE << 17) | (BE >> 15); \
		BF = T32(BF << 17) | (BF >> 15); \
		PERM_STEP_0; \
		PERM_STEP_1; \
		PERM_STEP_2; \
		A0B = T32(A0B + C6); \
		A0A = T32(A0A + C5); \
		A09 = T32(A09 + C4); \
		A08 = T32(A08 + C3); \
		A07 = T32(A07 + C2); \
		A06 = T32(A06 + C1); \
		A05 = T32(A05 + C0); \
		A04 = T32(A04 + CF); \
		A03 = T32(A03 + CE); \
		A02 = T32(A02 + CD); \
		A01 = T32(A01 + CC); \
		A00 = T32(A00 + CB); \
		A0B = T32(A0B + CA); \
		A0A = T32(A0A + C9); \
		A09 = T32(A09 + C8); \
		A08 = T32(A08 + C7); \
		A07 = T32(A07 + C6); \
		A06 = T32(A06 + C5); \
		A05 = T32(A05 + C4); \
		A04 = T32(A04 + C3); \
		A03 = T32(A03 + C2); \
		A02 = T32(A02 + C1); \
		A01 = T32(A01 + C0); \
		A00 = T32(A00 + CF); \
		A0B = T32(A0B + CE); \
		A0A = T32(A0A + CD); \
		A09 = T32(A09 + CC); \
		A08 = T32(A08 + CB); \
		A07 = T32(A07 + CA); \
		A06 = T32(A06 + C9); \
		A05 = T32(A05 + C8); \
		A04 = T32(A04 + C7); \
		A03 = T32(A03 + C6); \
		A02 = T32(A02 + C5); \
		A01 = T32(A01 + C4); \
		A00 = T32(A00 + C3); \
	} while (0)

#define INCR_W   do { \
		if ((Wlow = T32(Wlow + 1)) == 0) \
			Whigh = T32(Whigh + 1); \
	} while (0)

__constant__
sph_u32 A_init_256[] = {
C32(0x52F84552), C32(0xE54B7999), C32(0x2D8EE3EC), C32(0xB9645191),
C32(0xE0078B86), C32(0xBB7C44C9), C32(0xD2B5C1CA), C32(0xB0D2EB8C),
C32(0x14CE5A45), C32(0x22AF50DC), C32(0xEFFDBC6B), C32(0xEB21B74A)
};

__constant__
sph_u32 B_init_256[] = {
C32(0xB555C6EE), C32(0x3E710596), C32(0xA72A652F), C32(0x9301515F),
C32(0xDA28C1FA), C32(0x696FD868), C32(0x9CB6BF72), C32(0x0AFE4002),
C32(0xA6E03615), C32(0x5138C1D4), C32(0xBE216306), C32(0xB38B8890),
C32(0x3EA8B96B), C32(0x3299ACE4), C32(0x30924DD4), C32(0x55CB34A5)
};

__constant__
sph_u32 C_init_256[] = {
C32(0xB405F031), C32(0xC4233EBA), C32(0xB3733979), C32(0xC0DD9D55),
C32(0xC51C28AE), C32(0xA327B8E1), C32(0x56C56167), C32(0xED614433),
C32(0x88B59D60), C32(0x60E2CEBA), C32(0x758B4B8B), C32(0x83E82A7F),
C32(0xBC968828), C32(0xE6E00BF7), C32(0xBA839E55), C32(0x9B491C60)
};

/*
* Part of this code was automatically generated (the part between
* the "BEGIN" and "END" markers).
*/

__device__
sph_u32 cuda_sph_dec32le_aligned(const void *src)
{
#if SPH_LITTLE_ENDIAN
	return *(const sph_u32 *)src;
#elif SPH_BIG_ENDIAN
#if SPH_SPARCV9_GCC && !SPH_NO_ASM
	sph_u32 tmp;

	__asm__ __volatile__ ("lda [%1]0x88,%0" : "=r" (tmp) : "r" (src));
	return tmp;
	/*
	* Not worth it generally.
	*
	#elif (SPH_PPC32_GCC || SPH_PPC64_GCC) && !SPH_NO_ASM
	sph_u32 tmp;

	__asm__ __volatile__ ("lwbrx %0,0,%1" : "=r" (tmp) : "r" (src));
	return tmp;
	*/
#else
	return sph_bswap32(*(const sph_u32 *)src);
#endif
#else
	return (sph_u32)(((const unsigned char *)src)[0])
		| ((sph_u32)(((const unsigned char *)src)[1]) << 8)
		| ((sph_u32)(((const unsigned char *)src)[2]) << 16)
		| ((sph_u32)(((const unsigned char *)src)[3]) << 24);
#endif
}

__device__
void cuda_shabal_init(void *cc, unsigned size)
{
	/*
	* We have precomputed initial states for all the supported
	* output bit lengths.
	*/
	sph_u32 *A_init, *B_init, *C_init;
	sph_shabal_context *sc;

	A_init = A_init_256;
	B_init = B_init_256;
	C_init = C_init_256;

	sc = (sph_shabal_context *)cc;
	
	memcpy(sc->A, A_init, sizeof sc->A);
	memcpy(sc->B, B_init, sizeof sc->B);
	memcpy(sc->C, C_init, sizeof sc->C);

	sc->Wlow = 1;
	sc->Whigh = 0;
	sc->ptr = 0;
}

__device__
void cuda_shabal_core(void *cc, const unsigned char *data, size_t len)
{
	sph_shabal_context *sc;
	unsigned char *buf;
	size_t ptr;
	DECL_STATE

	sc = (sph_shabal_context *)cc;
	buf = sc->buf;
	ptr = sc->ptr;

	/*
	* We do not want to copy the state to local variables if the
	* amount of data is less than what is needed to complete the
	* current block. Note that it is anyway suboptimal to call
	* this method many times for small chunks of data.
	*/
	if (len < (sizeof sc->buf) - ptr) {
		memcpy(buf + ptr, data, len);
		ptr += len;
		sc->ptr = ptr;
		return;
	}

	READ_STATE(sc);
	while (len > 0) {
		size_t clen;

		clen = (sizeof sc->buf) - ptr;
		if (clen > len)
			clen = len;
		memcpy(buf + ptr, data, clen);
		ptr += clen;
		data += clen;
		len -= clen;
		if (ptr == sizeof sc->buf) {
			DECODE_BLOCK;
			INPUT_BLOCK_ADD;
			XOR_W;
			APPLY_P;
			INPUT_BLOCK_SUB;
			SWAP_BC;
			INCR_W;
			ptr = 0;
		}
	}
	WRITE_STATE(sc);
	sc->ptr = ptr;
}

__device__
void cuda_sph_enc32le_aligned(void *dst, sph_u32 val)
{
#if SPH_LITTLE_ENDIAN
	*(sph_u32 *)dst = val;
#elif SPH_BIG_ENDIAN
	*(sph_u32 *)dst = sph_bswap32(val);
#else
	((unsigned char *)dst)[0] = val;
	((unsigned char *)dst)[1] = (val >> 8);
	((unsigned char *)dst)[2] = (val >> 16);
	((unsigned char *)dst)[3] = (val >> 24);
#endif
}

__device__
void cuda_shabal_close(void *cc, unsigned ub, unsigned n, void *dst)
{
	const auto size_word = 8;
	sph_shabal_context *sc;
	unsigned char *buf;
	size_t ptr;
	int i;
	unsigned z;
	union {
		unsigned char tmp_out[64];
		sph_u32 dummy;
	} u;
	size_t out_len;
	DECL_STATE

	sc = (sph_shabal_context *)cc;
	buf = sc->buf;
	ptr = sc->ptr;
	z = 0x80 >> n;
	buf[ptr] = ((ub & -z) | z) & 0xFF;
	memset(buf + ptr + 1, 0, (sizeof sc->buf) - (ptr + 1));
	READ_STATE(sc);
	DECODE_BLOCK;
	INPUT_BLOCK_ADD;
	XOR_W;
	APPLY_P;
	for (i = 0; i < 3; i ++) {
		SWAP_BC;
		XOR_W;
		APPLY_P;
	}

	/*
	* We just use our local variables; no need to go through
	* the state structure. In order to share some code, we
	* emit the relevant words into a temporary buffer, which
	* we finally copy into the destination array.
	*/
	cuda_sph_enc32le_aligned(u.tmp_out + 32, B8);
	cuda_sph_enc32le_aligned(u.tmp_out + 36, B9);
	cuda_sph_enc32le_aligned(u.tmp_out + 40, BA);
	cuda_sph_enc32le_aligned(u.tmp_out + 44, BB);
	cuda_sph_enc32le_aligned(u.tmp_out + 48, BC);
	cuda_sph_enc32le_aligned(u.tmp_out + 52, BD);
	cuda_sph_enc32le_aligned(u.tmp_out + 56, BE);
	cuda_sph_enc32le_aligned(u.tmp_out + 60, BF);

	out_len = size_word << 2;
	memcpy(dst, u.tmp_out + (sizeof u.tmp_out) - out_len, out_len);
	cuda_shabal_init(sc, size_word << 5);
}

__global__
void cuda_calculate_shabal(Burst::ScoopData* buffer, Poco::UInt64* deadlines, Poco::UInt64 len,
	const GensigData* gensig, Poco::UInt64 nonceStart, Poco::UInt64 baseTarget)
{
	int tid = threadIdx.x + blockIdx.x * blockDim.x;

	sph_shabal256_context context;
	cuda_shabal_init(&context, 256);

	//printf("blockIdx.x = %i\n", blockIdx.x);
	//printf("blockIdx.x = %i, threadIdx.x = %i, index = %i\n", blockIdx.x, threadIdx.x, i);
	
	if (tid >= len)
	{
		//printf("blockIdx.x = %i, threadIdx.x = %i, index = %i\n", blockIdx.x, threadIdx.x, i);
		return;
	}

	Poco::UInt8 target[Settings::hashSize];
	auto test = buffer[tid];

	cuda_shabal_core(&context, (const unsigned char *)gensig, Settings::hashSize);
	cuda_shabal_core(&context, (const unsigned char *)&test, Settings::scoopSize);
	cuda_shabal_close(&context, 0, 0, &target[0]);

	Poco::UInt64 targetResult = 0;
	memcpy(&targetResult, &target[0], sizeof(Poco::UInt64));

	//auto buffer_deadline = (Poco::UInt64*)(buffer + tid);
	//*buffer_deadline = targetResult / baseTarget;
	deadlines[tid] = targetResult / baseTarget;
}

void cuda_calc_occupancy(int bufferSize, int& gridSize, int& blockSize)
{
	int minGridSize;

	cudaOccupancyMaxPotentialBlockSize(&minGridSize, &blockSize, (void*)cuda_calculate_shabal, 0, 0);

	gridSize = (bufferSize + blockSize - 1) / blockSize;
}

bool cuda_alloc_memory(Poco::UInt64 size, void** mem)
{
	if (size <= 0)
		return false;

	return cudaMalloc((void**)&*mem, size) == cudaSuccess;
}

bool cuda_copy_memory(Poco::UInt64 size, const void* from, void* to, MemoryCopyDirection copyDirection)
{
	if (size <= 0)
		return false;

	return cudaMemcpy(to, from, size, copyDirection == MemoryCopyDirection::ToDevice ? cudaMemcpyHostToDevice : cudaMemcpyDeviceToHost) == cudaSuccess;
}

bool cuda_free_memory(void* mem)
{
	if (mem == nullptr)
		return false;

	return cudaFree(mem) == cudaSuccess;
}

bool cuda_calculate_shabal_host_preallocated(ScoopData* buffer, Poco::UInt64* deadlines, Poco::UInt64 bufferSize, const GensigData* gensig,
	Poco::UInt64 nonceStart, Poco::UInt64 baseTarget, std::string& errorString)
{
	auto blockSize = 0;
	auto minGridSize = 0;
	
	cudaOccupancyMaxPotentialBlockSize(&minGridSize, &blockSize, (void*)cuda_calculate_shabal, 0, 0);

	int gridSize = (bufferSize + blockSize - 1) / blockSize;

	cuda_calculate_shabal<<<gridSize, blockSize>>>(buffer, deadlines, bufferSize, gensig, nonceStart, baseTarget);
	
	return !cuda_get_error(errorString);
}

bool cuda_reduce_best_deadline(Poco::UInt64* deadlines, size_t size, Poco::UInt64& minDeadline, Poco::UInt64& index, std::string& errorString)
{
	thrust::device_ptr<Poco::UInt64> deadlinesPtr(deadlines);
	auto pos = thrust::min_element(deadlinesPtr, deadlinesPtr + size);
	index = thrust::distance(deadlinesPtr, pos);

	if (!cuda_copy_memory(sizeof(Poco::UInt64), &deadlines[index], &minDeadline, MemoryCopyDirection::ToHost))
		return false;

	return !cuda_get_error(errorString);
}

bool cuda_get_error(std::string& errorString)
{
	const auto err = cudaPeekAtLastError();

	if (err != cudaSuccess)
	{
		errorString = cudaGetErrorString(err);
		return true;
	}

	return false;
}

bool cuda_get_devices(std::vector<std::string>& devices)
{
	auto count = 0;

	if (cudaGetDeviceCount(&count) != cudaSuccess)
		return false;

	devices.clear();
	
	for (auto i = 0; i < count; ++i)
	{
		cudaDeviceProp prop;
		
		if (cudaGetDeviceProperties(&prop, i) != cudaSuccess)
			continue;

		devices.emplace_back(prop.name);
	}

	return true;
}

bool cuda_set_device(unsigned index)
{
	return cudaSetDevice(static_cast<int>(index)) == cudaSuccess;
}
