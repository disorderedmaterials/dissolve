#reset
set title '/home/tris/src/duq/examples/water/epsr25/ref-10.0/water-noxray'
set xlabel 'r [A]'
set ylabel 'g(r)'
set autoscale x
set autoscale y
set nologscale x
set nologscale y
unset grid
set style line   1 lt   1 pt   6 ps    0.40
set style line   2 lt   2 pt   6 ps    0.40
set style line   3 lt   3 pt   6 ps    0.40
plot \
'/home/tris/src/duq/examples/water/epsr25/ref-10.0/water-noxray.EPSR.g01' u   1:(   1.00000*column(  2)+   0.000) title "O1-O1" w lines ls   1, \
'/home/tris/src/duq/examples/water/epsr25/ref-10.0/water-noxray.EPSR.r01' u   1:(abs(column(  2))>=1.0e-5?   1.00000*column(  2)+   0.000:1/0) notitle w p ls   1, \
'/home/tris/src/duq/examples/water/epsr25/ref-10.0/water-noxray.EPSR.g01' u   1:(   1.00000*column(  4)+   1.000) title "O1-H1" w lines ls   2, \
'/home/tris/src/duq/examples/water/epsr25/ref-10.0/water-noxray.EPSR.r01' u   1:(abs(column(  4))>=1.0e-5?   1.00000*column(  4)+   1.000:1/0) notitle w p ls   2, \
'/home/tris/src/duq/examples/water/epsr25/ref-10.0/water-noxray.EPSR.g01' u   1:(   1.00000*column(  6)+   2.000) title "H1-H1" w lines ls   3, \
'/home/tris/src/duq/examples/water/epsr25/ref-10.0/water-noxray.EPSR.r01' u   1:(abs(column(  6))>=1.0e-5?   1.00000*column(  6)+   2.000:1/0) notitle w p ls   3
