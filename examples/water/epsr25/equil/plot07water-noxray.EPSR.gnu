#reset
set title '/home/tris/src/duq/examples/water/epsr25/equil/water-noxray'
set xlabel 'Q [1/A]'
set ylabel 'F(Q)'
set autoscale x
set autoscale y
set nologscale x
set nologscale y
unset grid
set label   1 'sls18498                     ' at  0.100E+02, 0.100E+00
set style line   1 lt   1 pt   6 ps    0.40
set label   2 'sls18500                     ' at  0.100E+02, 0.110E+01
set style line   2 lt   2 pt   6 ps    0.40
set label   3 'sls18502                     ' at  0.100E+02, 0.210E+01
set style line   3 lt   3 pt   6 ps    0.40
plot \
'/home/tris/src/duq/examples/water/epsr25/equil/water-noxray.EPSR.u01' u   1:(   1.00000*column(  2)+   0.000) notitle w lines ls   1, \
'/home/tris/src/duq/examples/water/epsr25/equil/water-noxray.EPSR.t01' u   1:(abs(column(  2))>=1.0e-5?   1.00000*column(  2)+   0.000:1/0) notitle w p ls   1, \
'/home/tris/src/duq/examples/water/epsr25/equil/water-noxray.EPSR.u01' u   1:(   1.00000*column(  4)+   1.000) notitle w lines ls   2, \
'/home/tris/src/duq/examples/water/epsr25/equil/water-noxray.EPSR.t01' u   1:(abs(column(  4))>=1.0e-5?   1.00000*column(  4)+   1.000:1/0) notitle w p ls   2, \
'/home/tris/src/duq/examples/water/epsr25/equil/water-noxray.EPSR.u01' u   1:(   1.00000*column(  6)+   2.000) notitle w lines ls   3, \
'/home/tris/src/duq/examples/water/epsr25/equil/water-noxray.EPSR.t01' u   1:(abs(column(  6))>=1.0e-5?   1.00000*column(  6)+   2.000:1/0) notitle w p ls   3
