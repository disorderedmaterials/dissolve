dissolve_system_test(estimatedsq benzene.txt 1)
dissolve_system_test(read-pcof pcof.txt 0)
dissolve_system_test(neutron-xray water-neutron-xray.txt 1)
dissolve_system_test(poisson water-poisson.txt 1 --restart=water-poisson_coefficients.restart)
