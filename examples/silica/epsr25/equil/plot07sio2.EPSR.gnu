#reset
set title '/home/tris/src/duq/examples/silica/epsr25/equil/sio2'
set xlabel 'Q [1/A]'
set ylabel 'F(Q)'
set autoscale x
set autoscale y
set nologscale x
set nologscale y
unset grid
set label   1 'NeutronSiO2sq                ' at  0.100E+02, 0.100E+00
set style line   1 lt   1 pt   6 ps    0.40
plot \
'/home/tris/src/duq/examples/silica/epsr25/equil/sio2.EPSR.u01' u   1:(   1.00000*column(  2)+   0.000) notitle w lines ls   1, \
'/home/tris/src/duq/examples/silica/epsr25/equil/sio2.EPSR.t01' u   1:(abs(column(  2))>=1.0e-5?   1.00000*column(  2)+   0.000:1/0) notitle w p ls   1
