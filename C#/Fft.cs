using System.Numerics;

namespace CSharpFftDemo
{
    public sealed class Fft {
        // Internal variables
        private static Complex s_one = Complex.One;
        private static readonly Complex[] phasevec = new[] {
            new Complex(-1, -1.22464679914735E-16),
            new Complex(6.12323399573677E-17, -1),
            new Complex(0.707106781186548, -0.707106781186548),
            new Complex(0.923879532511287, -0.38268343236509),
            new Complex(0.98078528040323, -0.195090322016128),
            new Complex(0.995184726672197, -0.0980171403295606),
            new Complex(0.998795456205172, -0.049067674327418),
            new Complex(0.999698818696204, -0.0245412285229123),
            new Complex(0.999924701839145, -0.0122715382857199),
            new Complex(0.999981175282601, -0.00613588464915448),
            new Complex(0.999995293809576, -0.00306795676296598),
            new Complex(0.999998823451702, -0.00153398018628477),
            new Complex(0.999999705862882, -0.000766990318742704),
            new Complex(0.999999926465718, -0.000383495187571396),
            new Complex(0.999999981616429, -0.000191747597310703),
            new Complex(0.999999995404107, -9.58737990959773E-05),
            new Complex(0.999999998851027, -4.79368996030669E-05),
            new Complex(0.999999999712757, -2.39684498084182E-05),
            new Complex(0.999999999928189, -1.19842249050697E-05),
            new Complex(0.999999999982047, -5.99211245264243E-06),
            new Complex(0.999999999995512, -2.99605622633466E-06),
            new Complex(0.999999999998878, -1.49802811316901E-06),
            new Complex(0.999999999999719, -7.49014056584716E-07),
            new Complex(0.99999999999993, -3.74507028292384E-07),
            new Complex(0.999999999999982, -1.87253514146195E-07),
            new Complex(0.999999999999996, -9.36267570730981E-08),
            new Complex(0.999999999999999, -4.68133785365491E-08),
            new Complex(1, -2.34066892682746E-08),
            new Complex(1, -1.17033446341373E-08),
            new Complex(1, -5.85167231706864E-09),
            new Complex(1, 2.92583615853432E-09),
            new Complex(1, 0)
        };

        // Public function
        public static void Calculate(
            int Log2FftSize,
            Complex[] xy_in,
            Complex[] xy_out)
        {
            // if (xy_in is null)
            // {
            //     throw new ArgumentNullException(nameof(xy_in));
            // }

            // if (xy_out is null)
            // {
            //     throw new ArgumentNullException(nameof(xy_out));
            // }

            var n = 1 << Log2FftSize;

            for (int i = 0; i < n; i++)
            {
                long brev = i;

                brev = ((brev & 0xaaaaaaaa) >> 1) | ((brev & 0x55555555) << 1);
                brev = ((brev & 0xcccccccc) >> 2) | ((brev & 0x33333333) << 2);
                brev = ((brev & 0xf0f0f0f0) >> 4) | ((brev & 0x0f0f0f0f) << 4);
                brev = ((brev & 0xff00ff00) >> 8) | ((brev & 0x00ff00ff) << 8);
                brev = (brev >> 16) | (brev << 16);

                brev >>= 32 - Log2FftSize;
                xy_out[brev] = xy_in[i];
            }

            int l2pt = 0;
            int mmax = 1;

            while (n > mmax)
            {
                int istep = mmax << 1;
                var wphase_XY = phasevec[l2pt++];
                var w_XY = s_one;

                for (int m = 0; m < mmax; m++)
                {
                    for (int i = m; i < n; i += istep)
                    {
                        var tempXY = w_XY * xy_out[i + mmax];

                        xy_out[i + mmax] = xy_out[i] - tempXY;
                        xy_out[i] += tempXY;
                    }

                    w_XY *= wphase_XY;
                }

                mmax = istep;
            }
        }
    }
}
