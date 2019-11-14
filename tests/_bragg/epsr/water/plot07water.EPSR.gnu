#reset
set title '/home/tris/src/dissolve/tests/bragg/epsr/water/water'
set xlabel 'Q [1/A]'
set ylabel 'F(Q)'
set autoscale x
set autoscale y
set nologscale x
set nologscale y
unset grid
set label   1 'D2O                          ' at  0.100E+02, 0.100E+00
set style line   1 lt   1 pt   6 ps    0.40
set label   2 'H2O                          ' at  0.100E+02, 0.110E+01
set style line   2 lt   2 pt   6 ps    0.40
set label   3 'HDO                          ' at  0.100E+02, 0.210E+01
set style line   3 lt   3 pt   6 ps    0.40
plot \
'/home/tris/src/dissolve/tests/bragg/epsr/water/water.EPSR.u01' u   1:(   1.00000*column(  2)+   0.000) notitle w lines ls   1, \
'/home/tris/src/dissolve/tests/bragg/epsr/water/water.EPSR.t01' u   1:(abs(column(  2))>=1.0e-5?   1.00000*column(  2)+   0.000:1/0) notitle w p ls   1, \
'/home/tris/src/dissolve/tests/bragg/epsr/water/water.EPSR.u01' u   1:(   1.00000*column(  4)+   1.000) notitle w lines ls   2, \
'/home/tris/src/dissolve/tests/bragg/epsr/water/water.EPSR.t01' u   1:(abs(column(  4))>=1.0e-5?   1.00000*column(  4)+   1.000:1/0) notitle w p ls   2, \
'/home/tris/src/dissolve/tests/bragg/epsr/water/water.EPSR.u01' u   1:(   1.00000*column(  6)+   2.000) notitle w lines ls   3, \
'/home/tris/src/dissolve/tests/bragg/epsr/water/water.EPSR.t01' u   1:(abs(column(  6))>=1.0e-5?   1.00000*column(  6)+   2.000:1/0) notitle w p ls   3
