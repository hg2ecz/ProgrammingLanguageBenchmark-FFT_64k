#![feature(stdsimd)]
use std::simd::{f32x4};

type FloatVfoType = f32;
type FloatType = f32;
type VectorType = f32x4;
// Internal variables
pub struct Fft {
    phasevec: (Vec<f32>, Vec<f32>) // = [[0.0; 2]; 32];
}

// Public function
impl Fft {
    pub fn new() -> Fft {
	let mut f = Fft {phasevec: (Vec::new(), Vec::new())};
	for i in 0..32 {
	    let point: i32 = 2<<i;
	    f.phasevec.0.push( (-2.0*std::f64::consts::PI/(point as f64)).cos() as FloatVfoType);
	    f.phasevec.1.push( (-2.0*std::f64::consts::PI/(point as f64)).sin() as FloatVfoType);
	}
	return f;
    }

    pub fn fft(&self, log2point: u32, xy_in: &(Vec<FloatType>, Vec<FloatType>)) -> (Vec<FloatType>, Vec<FloatType>) {
	let mut i=0;
	let mut xy_out: (Vec<FloatType>, Vec<FloatType>) = (vec![0.; 1<<log2point], vec![0.; 1<<log2point]);
	while i < 1<<log2point {
	    let mut brev: usize = i;
	    brev = ((brev & 0xaaaaaaaa) >> 1) | ((brev & 0x55555555) << 1);
	    brev = ((brev & 0xcccccccc) >> 2) | ((brev & 0x33333333) << 2);
	    brev = ((brev & 0xf0f0f0f0) >> 4) | ((brev & 0x0f0f0f0f) << 4);
	    brev = ((brev & 0xff00ff00) >> 8) | ((brev & 0x00ff00ff) << 8);
	    brev = (brev >> 16) | (brev << 16);

	    brev >>= 32-log2point;
	    xy_out.0[brev] = xy_in.0[i];
	    xy_out.1[brev] = xy_in.1[i];

	    let brev2 = brev | (1<<(log2point-1));
	    xy_out.0[brev2] = xy_in.0[i+1];
	    xy_out.1[brev2] = xy_in.1[i+1];
	    i+=2;
	}

	// here begins the Danielson-Lanczos section;
	let n = 1<<log2point;
	let mut l2pt=0;
	let mut mmax=1;


	l2pt+=1;

	i=0;
	while i < 1<<log2point {
	    let tempx: FloatType = xy_out.0[i+mmax];
	    let tempy: FloatType = xy_out.1[i+mmax];
	    xy_out.0[i+mmax]  = xy_out.0[i] - tempx;
	    xy_out.1[i+mmax]  = xy_out.1[i] - tempy;
	    xy_out.0[i     ] += tempx;
	    xy_out.1[i     ] += tempy;
	    i+=2;
	}
	mmax<<=1;

	let w_x2: FloatVfoType = self.phasevec.0[l2pt];
	let w_y2: FloatVfoType = self.phasevec.1[l2pt]; l2pt+=1;

	i=0;
	while i < n {
	    let tempx: FloatType = xy_out.0[i+mmax];
	    let tempy: FloatType = xy_out.1[i+mmax];
	    xy_out.0[i+mmax]  = xy_out.0[i] - tempx;
	    xy_out.1[i+mmax]  = xy_out.1[i] - tempy;
	    xy_out.0[i     ] += tempx;
	    xy_out.1[i     ] += tempy;

	    let tempx2: FloatType = w_x2 as FloatType * xy_out.0[i+1+mmax] - w_y2 as FloatType * xy_out.1[i+1+mmax];
	    let tempy2: FloatType = w_x2 as FloatType * xy_out.1[i+1+mmax] + w_y2 as FloatType * xy_out.0[i+1+mmax];
	    xy_out.0[i+1+mmax]  = xy_out.0[i+1] - tempx2;
	    xy_out.1[i+1+mmax]  = xy_out.1[i+1] - tempy2;
	    xy_out.0[i+1     ] += tempx2;
	    xy_out.1[i+1     ] += tempy2;
	    i+=4;
	}
	mmax<<=1;

	while n>mmax {
	    let istep = mmax<<1;
	    let wphase_x: FloatVfoType = self.phasevec.0[l2pt];
	    let wphase_y: FloatVfoType = self.phasevec.1[l2pt];

	    let wphase_xvec: VectorType = VectorType::splat(self.phasevec.0[l2pt-2]);
	    let wphase_yvec: VectorType = VectorType::splat(self.phasevec.1[l2pt-2]);
	    l2pt+=1;


	    let w_x0 = 1.;
	    let w_y0 = 0.;

	    let w_x1 = w_x0 * wphase_x; // - w_Yvec[0] * wphase_Y;
	    let w_y1 = w_x0 * wphase_y; // + w_Yvec[0] * wphase_X;

	    let w_x2 = w_x1 * wphase_x - w_y1 * wphase_y;
	    let w_y2 = w_x1 * wphase_y + w_y1 * wphase_x;

	    let w_x3 = w_x2 * wphase_x - w_y2 * wphase_y;
	    let w_y3 = w_x2 * wphase_y + w_y2 * wphase_x;

	    let mut w_xvec = VectorType::new(w_x0, w_x1, w_x2, w_x3);
	    let mut w_yvec = VectorType::new(w_y0, w_y1, w_y2, w_y3);


	    let mut m=0;
	    while  m < mmax { // optimization: tempXY and tempXY2
		let mut i=m;
		while i < n {
		    let reg1_re = VectorType::load_unaligned(&xy_out.0[i+mmax..i+mmax+4]); // 4 lanes reg
		    let reg1_im = VectorType::load_unaligned(&xy_out.1[i+mmax..i+mmax+4]); // 4 lanes reg

		    let temp_re: VectorType = w_xvec * reg1_re - w_yvec * reg1_im; // 4 lanes mul
		    let temp_im: VectorType = w_xvec * reg1_im + w_yvec * reg1_re; // 4 lanes mul

		    let reg2_re = VectorType::load_unaligned(&xy_out.0[i..i+4]); // 4 lanes reg
		    let reg2_im = VectorType::load_unaligned(&xy_out.1[i..i+4]); // 4 lanes reg

		    (reg2_re - temp_re).store_unaligned(&mut xy_out.0[i+mmax..i+mmax+4]); // 4 lanes sub&store
		    (reg2_im - temp_im).store_unaligned(&mut xy_out.1[i+mmax..i+mmax+4]); // 4 lanes sub&store 
		    (reg2_re + temp_re).store_unaligned(&mut xy_out.0[i..i+4]); // 4 lanes add&store
		    (reg2_im + temp_im).store_unaligned(&mut xy_out.1[i..i+4]); // 4 lanes add&store

		    i+=istep;
		}
		let w_xtmp: VectorType = w_xvec * wphase_xvec - w_yvec * wphase_yvec; // 4 lanes rotate
		w_yvec = w_xvec * wphase_yvec + w_yvec * wphase_xvec; // 4 lanes rotate
		w_xvec = w_xtmp;
		m+=4;
	    }
	    mmax=istep;
	}
	return xy_out;
    }
}
