"use strict"

// Internal variables
var phasevec_exist = 0
var phasevec = []

// Public function
module.exports = {
  fft: function (log2point, xy_in) {
    var i
    if (!phasevec_exist) {
	for (i=0; i<32; i++) {
	    var point = 2<<i
	    phasevec[i] = [ Math.cos(-2*Math.PI/point), Math.sin(-2*Math.PI/point) ] // complex.h ?
	}
	phasevec_exist = 1
    }

    var xy_out = []
    for (i=0; i < (1<<log2point); i++) {
	var brev = i
	brev = ((brev & 0xaaaaaaaa) >> 1) | ((brev & 0x55555555) << 1)
	brev = ((brev & 0xcccccccc) >> 2) | ((brev & 0x33333333) << 2)
	brev = ((brev & 0xf0f0f0f0) >> 4) | ((brev & 0x0f0f0f0f) << 4)
	brev = ((brev & 0xff00ff00) >> 8) | ((brev & 0x00ff00ff) << 8)
	//brev = (brev >> 16) | (brev << 16)

	//brev >>= 32-log2point
	brev >>= 16-log2point
	xy_out[brev] = [ xy_in[i][0], xy_in[i][1] ]
    }

    // here begins the Danielson-Lanczos section
    var n = 1<<log2point
    var l2pt=0
    var mmax=1
    while (n>mmax) {
	var istep = mmax<<1

//	double theta = -2*M_PI/istep
//	double complex wphase_XY = cos(theta) + sin(theta)*I
	var wphase_XY = phasevec[l2pt++]

	var w_XY = [ 1.0, 0.0 ]
	for (var m=0; m < mmax; m++) {
	    for (var i=m; i < n; i += istep) {
		var tempXY = [  w_XY[0] * xy_out[i+mmax][0] - w_XY[1] * xy_out[i+mmax][1],
				w_XY[0] * xy_out[i+mmax][1] + w_XY[1] * xy_out[i+mmax][0] ]

		xy_out[i+mmax][0]  = xy_out[i][0] - tempXY[0]
		xy_out[i+mmax][1]  = xy_out[i][1] - tempXY[1]

		xy_out[i     ][0] += tempXY[0]
		xy_out[i     ][1] += tempXY[1]
	    }
	    var w_XY_t = w_XY[0] * wphase_XY[0] - w_XY[1] * wphase_XY[1]; // rotate ... complex.h ?
	    w_XY[1]    = w_XY[0] * wphase_XY[1] + w_XY[1] * wphase_XY[0]
	    w_XY[0] = w_XY_t
	}
	mmax=istep
    }
    return xy_out
  }
}
