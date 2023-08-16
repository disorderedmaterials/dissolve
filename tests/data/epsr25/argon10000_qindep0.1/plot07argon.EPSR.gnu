#reset
set title '/home/tris/src/duq/tests/09/epsr_qindep/argon'
set xlabel 'Q [1/A]'
set ylabel 'F(Q)'
set autoscale x
set autoscale y
set nologscale x
set nologscale y
unset grid
set label   1 'dummy                        ' at  0.100E+02, 0.100E+00
set style line   1 lt   1 pt   6 ps    0.40
plot \
'/home/tris/src/duq/tests/09/epsr_qindep/argon.EPSR.u01' u   1:(   1.00000*column(  2)+   0.000) notitle w lines ls   1, \
'/home/tris/src/duq/tests/09/epsr_qindep/argon.EPSR.t01' u   1:(abs(column(  2))>=1.0e-5?   1.00000*column(  2)+   0.000:1/0) notitle w p ls   1
