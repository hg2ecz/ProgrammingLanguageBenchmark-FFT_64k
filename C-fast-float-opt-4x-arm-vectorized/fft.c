#include <math.h>
#include "fft.h"
#include <arm_neon.h>

// Internal variables
static int phasevec_exist = 0;
static FLOAT_VFO_TYPE phasevec[32][2];

// Public function
struct _complexblock *fft(int log2point,const struct _complexblock xy_in) {
    int i;
    static struct _complexblock xy_out;
    if (!phasevec_exist) {
	for (i=0; i<32; i++) {
	    int point = 2<<i;
	    phasevec[i][0] = cos(-2*M_PI/point);
	    phasevec[i][1] = sin(-2*M_PI/point);
	}
	phasevec_exist = 1;
    }
    for (i=0; i < (1<<log2point); i+=2) {
	unsigned int brev = i;
	brev = ((brev & 0xaaaaaaaa) >> 1) | ((brev & 0x55555555) << 1);
	brev = ((brev & 0xcccccccc) >> 2) | ((brev & 0x33333333) << 2);
	brev = ((brev & 0xf0f0f0f0) >> 4) | ((brev & 0x0f0f0f0f) << 4);
	brev = ((brev & 0xff00ff00) >> 8) | ((brev & 0x00ff00ff) << 8);
	brev = (brev >> 16) | (brev << 16);

	brev >>= 32-log2point;
	xy_out.re[brev] = xy_in.re[i];
	xy_out.im[brev] = xy_in.im[i];

	unsigned int brev2 = brev | (1<<(log2point-1));
	xy_out.re[brev2] = xy_in.re[i+1];
	xy_out.im[brev2] = xy_in.im[i+1];
    }

    // here begins the Danielson-Lanczos section
    int n = 1<<log2point;
    int l2pt=0;
    int mmax=1;


    l2pt++;
    for (int i=0; i < n; i += 2) {
//	COMPLEX_TYPE tempXY = xy_out[i+mmax]; // w_XY = 1
//	xy_out[i+mmax]  = xy_out[i] - tempXY;
//	xy_out[i     ] += tempXY;
	FLOAT_TYPE tempX = xy_out.re[i+mmax];
	FLOAT_TYPE tempY = xy_out.im[i+mmax];
	xy_out.re[i+mmax]  = xy_out.re[i] - tempX;
	xy_out.im[i+mmax]  = xy_out.im[i] - tempY;
	xy_out.re[i     ] += tempX;
	xy_out.im[i     ] += tempY;
    }
    mmax<<=1;

    FLOAT_VFO_TYPE w_X2 = phasevec[l2pt][0];
    FLOAT_VFO_TYPE w_Y2 = phasevec[l2pt][1]; l2pt++;
    for (int i=0; i < n; i += 4) {
//	COMPLEX_TYPE tempXY = xy_out[i+mmax]; // w_XY = 1
//	xy_out[i+mmax]  = xy_out[i] - tempXY;
//	xy_out[i     ] += tempXY;

//	COMPLEX_TYPE tempXY2 = w_XY2 * xy_out[i+1+mmax];
//	xy_out[i+1+mmax]  = xy_out[i+1] - tempXY2;
//	xy_out[i+1     ] += tempXY2;
	FLOAT_TYPE tempX = xy_out.re[i+mmax];
	FLOAT_TYPE tempY = xy_out.im[i+mmax];
	xy_out.re[i+mmax]  = xy_out.re[i] - tempX;
	xy_out.im[i+mmax]  = xy_out.im[i] - tempY;
	xy_out.re[i     ] += tempX;
	xy_out.im[i     ] += tempY;

	FLOAT_TYPE tempX2 = (FLOAT_TYPE)w_X2 * xy_out.re[i+1+mmax] - (FLOAT_TYPE)w_Y2 * xy_out.im[i+1+mmax];
	FLOAT_TYPE tempY2 = (FLOAT_TYPE)w_X2 * xy_out.im[i+1+mmax] + (FLOAT_TYPE)w_Y2 * xy_out.re[i+1+mmax];
	xy_out.re[i+1+mmax]  = xy_out.re[i+1] - tempX2;
	xy_out.im[i+1+mmax]  = xy_out.im[i+1] - tempY2;
	xy_out.re[i+1     ] += tempX2;
	xy_out.im[i+1     ] += tempY2;
    }
    mmax<<=1;

#ifdef MOD_SPEED
    while (l2pt < log2point) {
	int istep = 2<<l2pt;
#else
    while (n>mmax) {
	int istep = mmax<<1;
#endif
//	double theta = -2*M_PI/istep;
//	COMPLEX_TYPE wphase_XY = cos(theta) + sin(theta)*I;
	FLOAT_VFO_TYPE wphase_X = phasevec[l2pt][0];
	FLOAT_VFO_TYPE wphase_Y = phasevec[l2pt][1]; l2pt++;

	float32x4_t wphase_Xvec, wphase_Yvec;
	wphase_Xvec[0] = wphase_Xvec[1] = wphase_Xvec[2] = wphase_Xvec[3]= phasevec[l2pt+1][0];
	wphase_Yvec[0] = wphase_Yvec[1] = wphase_Yvec[2] = wphase_Yvec[3]= phasevec[l2pt+1][1];

	float32x4_t w_Xvec, w_Yvec;
	w_Xvec[0] = 1.;
	w_Yvec[0] = 0.;

	w_Xvec[1] = w_Xvec[0] * wphase_X; // - w_Yvec[0] * wphase_Y;
	w_Yvec[1] = w_Xvec[0] * wphase_Y; // + w_Yvec[0] * wphase_X;

	w_Xvec[2] = w_Xvec[1] * wphase_X - w_Yvec[1] * wphase_Y;
	w_Yvec[2] = w_Xvec[1] * wphase_Y + w_Yvec[1] * wphase_X;

	w_Xvec[3] = w_Xvec[2] * wphase_X - w_Yvec[2] * wphase_Y;
	w_Yvec[3] = w_Xvec[2] * wphase_Y + w_Yvec[2] * wphase_X;

	for (int m=0; m < mmax; m+=4) { // optimization: tempXY and tempXY2
	    for (int i=m; i < n; i += istep) {
/*		FLOAT_TYPE tempX = (FLOAT_TYPE)w_X * xy_out.re[i+mmax] - (FLOAT_TYPE)w_Y * xy_out.im[i+mmax];
		FLOAT_TYPE tempY = (FLOAT_TYPE)w_X * xy_out.im[i+mmax] + (FLOAT_TYPE)w_Y * xy_out.re[i+mmax];
		xy_out.re[i+mmax]  = xy_out.re[i] - tempX;
		xy_out.im[i+mmax]  = xy_out.im[i] - tempY;
		xy_out.re[i     ] += tempX;
		xy_out.im[i     ] += tempY;

		FLOAT_TYPE tempX2 = (FLOAT_TYPE)w_X2 * xy_out.re[i+1+mmax] - (FLOAT_TYPE)w_Y2 * xy_out.im[i+1+mmax];
		FLOAT_TYPE tempY2 = (FLOAT_TYPE)w_X2 * xy_out.im[i+1+mmax] + (FLOAT_TYPE)w_Y2 * xy_out.re[i+1+mmax];
		xy_out.re[i+1+mmax]  = xy_out.re[i+1] - tempX2;
		xy_out.im[i+1+mmax]  = xy_out.im[i+1] - tempY2;
		xy_out.re[i+1     ] += tempX2;
		xy_out.im[i+1     ] += tempY2;

		FLOAT_TYPE tempX3 = (FLOAT_TYPE)w_X3 * xy_out.re[i+2+mmax] - (FLOAT_TYPE)w_Y3 * xy_out.im[i+2+mmax];
		FLOAT_TYPE tempY3 = (FLOAT_TYPE)w_X3 * xy_out.im[i+2+mmax] + (FLOAT_TYPE)w_Y3 * xy_out.re[i+2+mmax];
		xy_out.re[i+2+mmax]  = xy_out.re[i+2] - tempX3;
		xy_out.im[i+2+mmax]  = xy_out.im[i+2] - tempY3;
		xy_out.re[i+2     ] += tempX3;
		xy_out.im[i+2     ] += tempY3;

		FLOAT_TYPE tempX4 = (FLOAT_TYPE)w_X4 * xy_out.re[i+3+mmax] - (FLOAT_TYPE)w_Y4 * xy_out.im[i+3+mmax];
		FLOAT_TYPE tempY4 = (FLOAT_TYPE)w_X4 * xy_out.im[i+3+mmax] + (FLOAT_TYPE)w_Y4 * xy_out.re[i+3+mmax];
		xy_out.re[i+3+mmax]  = xy_out.re[i+3] - tempX4;
		xy_out.im[i+3+mmax]  = xy_out.im[i+3] - tempY4;
		xy_out.re[i+3     ] += tempX4;
		xy_out.im[i+3     ] += tempY4;
*/
		float32x4_t *reg1_re = (float32x4_t *)&xy_out.re[i+mmax]; // 4 lanes reg
		float32x4_t *reg1_im = (float32x4_t *)&xy_out.im[i+mmax]; // 4 lanes reg

		float32x4_t temp_re = w_Xvec * *reg1_re - w_Yvec * *reg1_im; // 4 lanes mul
		float32x4_t temp_im = w_Xvec * *reg1_im + w_Yvec * *reg1_re; // 4 lanes mul

		float32x4_t *reg2_re = (float32x4_t *)&xy_out.re[i]; // 4 lanes reg
		float32x4_t *reg2_im = (float32x4_t *)&xy_out.im[i]; // 4 lanes reg

		*reg1_re = *reg2_re - temp_re; // 4 lanes sub&store
		*reg1_im = *reg2_im - temp_im; // 4 lanes sub&store 
		*reg2_re += temp_re; // 4 lanes add&store
		*reg2_im += temp_im; // 4 lanes add&store
	    }
	    float32x4_t w_Xtmp;
	    w_Xtmp = w_Xvec * wphase_Xvec - w_Yvec * wphase_Yvec; // 4 lanes rotate
	    w_Yvec = w_Xvec * wphase_Yvec + w_Yvec * wphase_Xvec; // 4 lanes rotate
	    w_Xvec = w_Xtmp;
	}
	mmax=istep;
    }
    return &xy_out;
}