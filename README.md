# 4096 point FFT test result

- Raspberry Pi3: ARM Cortex A53, 1.2 GHz, 32 bit Raspbian 9 beta (gcc-6.2)
- Odroid-C2: ARM Cortex A53, 1.5 GHz, 64 bit Ubuntu 17.04 alpha (gcc-6.3)
- Intel i5-3337u: Ivy Bridge, 1.8 GHz (turbo 2,7 GHz), 64 bit Ubuntu 16.10 (gcc-6.2)

<p>Architecture measurement: C-fast time in milliseconds at FFT_REPEAT=10 000 (gcc-6.3; 1 core test)</p>

<table border="1">
<tr><th>C-fast_...</th><th>double</th><th>float</th><th>integer</th></tr>
<tr><td>Raspberry Pi1<br>0.7 GHz ARM11             </td><td>5.884 ms</td><td>4.712 ms</td><td>5.344 ms</td></tr>
<tr><td>BeagleBone Black<br>1 GHz ARM Cortex A8    </td><td>4.009 ms</td><td>3.509 ms</td><td>1.759 ms</td></tr>
<tr><td>Odroid-C1<br>1.5 GHz ARM Cortex A5         </td><td>1.472 ms</td><td>0.787 ms</td><td>1.268 ms</td></tr>
<tr><td>Raspberry Pi3<br>1.2 GHz ARM Cortex A53    </td><td>0.915 ms</td><td>0.671 ms</td><td>1.053 ms</td></tr>
<tr><td>Odroid-C2@64 bit<br>1.5 GHz ARM Cortex A53 </td><td>0.738 ms</td><td>0.496 ms</td><td>0.432 ms</td></tr>
<tr><td>Intel Celeron<br>J1900                     </td><td>0.371 ms</td><td>0.342 ms</td><td>0.324 ms</td></tr>
<tr><td>Intel Core laptop<br>i5-3337u              </td><td>0.158 ms</td><td>0.107 ms</td><td>0.113 ms</td></tr>
</table>


<p>C relative speed compared with C-fast_double; floatemu if not FPU in the processor (e.g. some 32 bit microcontroller)</p>

<table border="1">
<tr><th>Language</th><th>Raspberry Pi3</th><th>Odroid-C2@64 bit</th><th>i5-3337u</th></tr>
<tr><td>C-fast_double</td><td>1 (0.915 ms)</td><td>1 (0.726 ms)</td><td>1 (0.180 ms)</td></tr>
<tr><td>C-fast_float</td><td>0.82</td><td>0.74</td><td>0.69</td></tr>
<tr><td>C-fast_integer</td><td>1.12</td><td>0.58</td><td>0.79</td></tr>
<tr><td>C-fast_longlong_64bit</td><td>-</td><td>1.46</td><td>1.38</td></tr>
<tr><td>C-fast_float-nocomplex-floatemu</td><td>4.77</td><td>4.21</td><td>5.87</td></tr>
<tr><th colspan="4">C++</th></tr>
<tr><td>C++-fast_double</td><td>1.03 (clang 6)</td><td>1.13</td><td>1.02</td></tr>
</table>

<p>Other compiled language speed compared with C-fast_double</p>

<table border="1">
<tr><th>Language</th><th>Raspberry Pi3</th><th>Odroid-C2@64 bit</th><th>i5-3337u</th></tr>
<tr><td>C# (.NET 6)</td><td>1.26</td><td></td><td></td></tr>
<tr><td>Rust (rustc-1.14)</td><td></td><td>1.34</td><td>1.35</td></tr>
<tr><td>Go (gccgo-6.3)</td><td>1.45</td><td>1.30</td><td>1.49</td></tr>
<tr><td>C# (mono-5.18)</td><td>2.78</td><td></td><td></td></tr>
<tr><td>Java (Oracle)</td><td>7.94</td><td> </td><td>1.38</td></tr>
<tr><td>C# (mono-4.x)</td><td>8.32</td><td>5.73</td><td>5.22</td></tr>
<tr><td>F# (fsharpc 4.0)</td><td>8.31</td><td> </td><td>4.93</td></tr>
</table>

<p>Script language speed compared with C-fast_double</p>

<table border="1">
<tr><th>Language</th><th>Raspberry Pi3</th><th>Odroid-C2@64 bit</th><th>i5-3337u</th></tr>
<tr><td>LUA (LUAjit)</td><td>8.01</td><td>8.49</td><td>5.4</td></tr>
<tr><td>Python (Pypy 5.4)</td><td>27.5</td><td> </td><td>4.91</td></tr>
<tr><td>JavaScript (nodejs 4.x)</td><td>25</td><td>23.7</td><td>17.3 </td></tr>
<tr><td>PHP7</td><td>129</td><td>135</td><td>88.2</td></tr>
<tr><td>Ruby</td><td>481</td><td>395</td><td>287</td></tr>
<tr><td>AWK</td><td>625</td><td>541</td><td>499</td></tr>
<tr><td>PERL</td><td>5139</td><td>5377</td><td>5710</td></tr>
<tr><td>Octave</td><td>9326</td><td> </td><td>7972</td></tr>
</table>
</pre>
