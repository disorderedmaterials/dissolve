#reset
set title '/home/tris/src/duq/examples/water/epsr25/equil/water-noxray'
set xlabel 'r [\306]'
set ylabel 'g(r)'
set autoscale x
set autoscale y
set nologscale x
set nologscale y
unset grid
set style line   1 lt   1 pt   6 ps    0.40
plot \
'/home/tris/src/duq/examples/water/epsr25/equil/water-noxray.EPSR.g01' u   1:(   1.00000*column(  7)+   0.000) title " Block           1" w lines ls   1
