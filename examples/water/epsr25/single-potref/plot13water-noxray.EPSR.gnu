#reset
set title '/home/tris/src/duq/examples/water/epsr25/single-potref/water-noxray'
set xlabel 'r [\305]'
set ylabel 'u(r) [kJ/mole]'
set autoscale x
set autoscale y
set nologscale x
set nologscale y
unset grid
set style line   1 lt   1 pt   6 ps    0.40
set style line   2 lt   2 pt   6 ps    0.40
set style line   3 lt   3 pt   6 ps    0.40
plot \
'/home/tris/src/duq/examples/water/epsr25/single-potref/water-noxray.EPSR.p01' u   1:(   1.00000*column(  2)+   0.000) title "O1-O1" w lines ls   1, \
'/home/tris/src/duq/examples/water/epsr25/single-potref/water-noxray.EPSR.p01' u   1:(   1.00000*column(  4)+   5.000) title "O1-H1" w lines ls   2, \
'/home/tris/src/duq/examples/water/epsr25/single-potref/water-noxray.EPSR.p01' u   1:(   1.00000*column(  6)+  10.000) title "H1-H1" w lines ls   3
