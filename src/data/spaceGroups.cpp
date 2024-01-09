// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/spaceGroups.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include <cassert>
#include <stdio.h>

namespace SpaceGroups
{
// Space group symbol data
std::vector<SpaceGroupSymbol> symbols_ = {{SpaceGroupId::NoSpaceGroup, 0, "", "None", "None"},
                                          {SpaceGroupId::SpaceGroup_1, 1, "", "P 1", "P 1"},
                                          {SpaceGroupId::SpaceGroup_2, 2, "", "P -1", "-P 1"},
                                          {SpaceGroupId::SpaceGroup_3_b, 3, "b", "P 1 2 1", "P 2y"},
                                          {SpaceGroupId::SpaceGroup_3_c, 3, "c", "P 1 1 2", "P 2"},
                                          {SpaceGroupId::SpaceGroup_3_a, 3, "a", "P 2 1 1", "P 2x"},
                                          {SpaceGroupId::SpaceGroup_4_b, 4, "b", "P 1 21 1", "P 2yb"},
                                          {SpaceGroupId::SpaceGroup_4_c, 4, "c", "P 1 1 21", "P 2c"},
                                          {SpaceGroupId::SpaceGroup_4_a, 4, "a", "P 21 1 1", "P 2xa"},
                                          {SpaceGroupId::SpaceGroup_5_b1, 5, "b1", "C 1 2 1", "C 2y"},
                                          {SpaceGroupId::SpaceGroup_5_b2, 5, "b2", "A 1 2 1", "A 2y"},
                                          {SpaceGroupId::SpaceGroup_5_b3, 5, "b3", "I 1 2 1", "I 2y"},
                                          {SpaceGroupId::SpaceGroup_5_c1, 5, "c1", "A 1 1 2", "A 2"},
                                          {SpaceGroupId::SpaceGroup_5_c2, 5, "c2", "B 1 1 2", "B 2"},
                                          {SpaceGroupId::SpaceGroup_5_c3, 5, "c3", "I 1 1 2", "I 2"},
                                          {SpaceGroupId::SpaceGroup_5_a1, 5, "a1", "B 2 1 1", "B 2x"},
                                          {SpaceGroupId::SpaceGroup_5_a2, 5, "a2", "C 2 1 1", "C 2x"},
                                          {SpaceGroupId::SpaceGroup_5_a3, 5, "a3", "I 2 1 1", "I 2x"},
                                          {SpaceGroupId::SpaceGroup_6_b, 6, "b", "P 1 m 1", "P -2y"},
                                          {SpaceGroupId::SpaceGroup_6_c, 6, "c", "P 1 1 m", "P -2"},
                                          {SpaceGroupId::SpaceGroup_6_a, 6, "a", "P m 1 1", "P -2x"},
                                          {SpaceGroupId::SpaceGroup_7_b1, 7, "b1", "P 1 c 1", "P -2yc"},
                                          {SpaceGroupId::SpaceGroup_7_b2, 7, "b2", "P 1 n 1", "P -2yac"},
                                          {SpaceGroupId::SpaceGroup_7_b3, 7, "b3", "P 1 a 1", "P -2ya"},
                                          {SpaceGroupId::SpaceGroup_7_c1, 7, "c1", "P 1 1 a", "P -2a"},
                                          {SpaceGroupId::SpaceGroup_7_c2, 7, "c2", "P 1 1 n", "P -2ab"},
                                          {SpaceGroupId::SpaceGroup_7_c3, 7, "c3", "P 1 1 b", "P -2b"},
                                          {SpaceGroupId::SpaceGroup_7_a1, 7, "a1", "P b 1 1", "P -2xb"},
                                          {SpaceGroupId::SpaceGroup_7_a2, 7, "a2", "P n 1 1", "P -2xbc"},
                                          {SpaceGroupId::SpaceGroup_7_a3, 7, "a3", "P c 1 1", "P -2xc"},
                                          {SpaceGroupId::SpaceGroup_8_b1, 8, "b1", "C 1 m 1", "C -2y"},
                                          {SpaceGroupId::SpaceGroup_8_b2, 8, "b2", "A 1 m 1", "A -2y"},
                                          {SpaceGroupId::SpaceGroup_8_b3, 8, "b3", "I 1 m 1", "I -2y"},
                                          {SpaceGroupId::SpaceGroup_8_c1, 8, "c1", "A 1 1 m", "A -2"},
                                          {SpaceGroupId::SpaceGroup_8_c2, 8, "c2", "B 1 1 m", "B -2"},
                                          {SpaceGroupId::SpaceGroup_8_c3, 8, "c3", "I 1 1 m", "I -2"},
                                          {SpaceGroupId::SpaceGroup_8_a1, 8, "a1", "B m 1 1", "B -2x"},
                                          {SpaceGroupId::SpaceGroup_8_a2, 8, "a2", "C m 1 1", "C -2x"},
                                          {SpaceGroupId::SpaceGroup_8_a3, 8, "a3", "I m 1 1", "I -2x"},
                                          {SpaceGroupId::SpaceGroup_9_b1, 9, "b1", "C 1 c 1", "C -2yc"},
                                          {SpaceGroupId::SpaceGroup_9_b2, 9, "b2", "A 1 n 1", "A -2yac"},
                                          {SpaceGroupId::SpaceGroup_9_b3, 9, "b3", "I 1 a 1", "I -2ya"},
                                          {SpaceGroupId::SpaceGroup_9_B1, 9, "-b1", "A 1 a 1", "A -2ya"},
                                          {SpaceGroupId::SpaceGroup_9_B2, 9, "-b2", "C 1 n 1", "C -2ybc"},
                                          {SpaceGroupId::SpaceGroup_9_B3, 9, "-b3", "I 1 c 1", "I -2yc"},
                                          {SpaceGroupId::SpaceGroup_9_c1, 9, "c1", "A 1 1 a", "A -2a"},
                                          {SpaceGroupId::SpaceGroup_9_c2, 9, "c2", "B 1 1 n", "B -2bc"},
                                          {SpaceGroupId::SpaceGroup_9_c3, 9, "c3", "I 1 1 b", "I -2b"},
                                          {SpaceGroupId::SpaceGroup_9_C1, 9, "-c1", "B 1 1 b", "B -2b"},
                                          {SpaceGroupId::SpaceGroup_9_C2, 9, "-c2", "A 1 1 n", "A -2ac"},
                                          {SpaceGroupId::SpaceGroup_9_C3, 9, "-c3", "I 1 1 a", "I -2a"},
                                          {SpaceGroupId::SpaceGroup_9_a1, 9, "a1", "B b 1 1", "B -2xb"},
                                          {SpaceGroupId::SpaceGroup_9_a2, 9, "a2", "C n 1 1", "C -2xbc"},
                                          {SpaceGroupId::SpaceGroup_9_a3, 9, "a3", "I c 1 1", "I -2xc"},
                                          {SpaceGroupId::SpaceGroup_9_A1, 9, "-a1", "C c 1 1", "C -2xc"},
                                          {SpaceGroupId::SpaceGroup_9_A2, 9, "-a2", "B n 1 1", "B -2xbc"},
                                          {SpaceGroupId::SpaceGroup_9_A3, 9, "-a3", "I b 1 1", "I -2xb"},
                                          {SpaceGroupId::SpaceGroup_10_b, 10, "b", "P 1 2/m 1", "-P 2y"},
                                          {SpaceGroupId::SpaceGroup_10_c, 10, "c", "P 1 1 2/m", "-P 2"},
                                          {SpaceGroupId::SpaceGroup_10_a, 10, "a", "P 2/m 1 1", "-P 2x"},
                                          {SpaceGroupId::SpaceGroup_11_b, 11, "b", "P 1 21/m 1", "-P 2yb"},
                                          {SpaceGroupId::SpaceGroup_11_c, 11, "c", "P 1 1 21/m", "-P 2c"},
                                          {SpaceGroupId::SpaceGroup_11_a, 11, "a", "P 21/m 1 1", "-P 2xa"},
                                          {SpaceGroupId::SpaceGroup_12_b1, 12, "b1", "C 1 2/m 1", "-C 2y"},
                                          {SpaceGroupId::SpaceGroup_12_b2, 12, "b2", "A 1 2/m 1", "-A 2y"},
                                          {SpaceGroupId::SpaceGroup_12_b3, 12, "b3", "I 1 2/m 1", "-I 2y"},
                                          {SpaceGroupId::SpaceGroup_12_c1, 12, "c1", "A 1 1 2/m", "-A 2"},
                                          {SpaceGroupId::SpaceGroup_12_c2, 12, "c2", "B 1 1 2/m", "-B 2"},
                                          {SpaceGroupId::SpaceGroup_12_c3, 12, "c3", "I 1 1 2/m", "-I 2"},
                                          {SpaceGroupId::SpaceGroup_12_a1, 12, "a1", "B 2/m 1 1", "-B 2x"},
                                          {SpaceGroupId::SpaceGroup_12_a2, 12, "a2", "C 2/m 1 1", "-C 2x"},
                                          {SpaceGroupId::SpaceGroup_12_a3, 12, "a3", "I 2/m 1 1", "-I 2x"},
                                          {SpaceGroupId::SpaceGroup_13_b1, 13, "b1", "P 1 2/c 1", "-P 2yc"},
                                          {SpaceGroupId::SpaceGroup_13_b2, 13, "b2", "P 1 2/n 1", "-P 2yac"},
                                          {SpaceGroupId::SpaceGroup_13_b3, 13, "b3", "P 1 2/a 1", "-P 2ya"},
                                          {SpaceGroupId::SpaceGroup_13_c1, 13, "c1", "P 1 1 2/a", "-P 2a"},
                                          {SpaceGroupId::SpaceGroup_13_c2, 13, "c2", "P 1 1 2/n", "-P 2ab"},
                                          {SpaceGroupId::SpaceGroup_13_c3, 13, "c3", "P 1 1 2/b", "-P 2b"},
                                          {SpaceGroupId::SpaceGroup_13_a1, 13, "a1", "P 2/b 1 1", "-P 2xb"},
                                          {SpaceGroupId::SpaceGroup_13_a2, 13, "a2", "P 2/n 1 1", "-P 2xbc"},
                                          {SpaceGroupId::SpaceGroup_13_a3, 13, "a3", "P 2/c 1 1", "-P 2xc"},
                                          {SpaceGroupId::SpaceGroup_14_b1, 14, "b1", "P 1 21/c 1", "-P 2ybc"},
                                          {SpaceGroupId::SpaceGroup_14_b2, 14, "b2", "P 1 21/n 1", "-P 2yn"},
                                          {SpaceGroupId::SpaceGroup_14_b3, 14, "b3", "P 1 21/a 1", "-P 2yab"},
                                          {SpaceGroupId::SpaceGroup_14_c1, 14, "c1", "P 1 1 21/a", "-P 2ac"},
                                          {SpaceGroupId::SpaceGroup_14_c2, 14, "c2", "P 1 1 21/n", "-P 2n"},
                                          {SpaceGroupId::SpaceGroup_14_c3, 14, "c3", "P 1 1 21/b", "-P 2bc"},
                                          {SpaceGroupId::SpaceGroup_14_a1, 14, "a1", "P 21/b 1 1", "-P 2xab"},
                                          {SpaceGroupId::SpaceGroup_14_a2, 14, "a2", "P 21/n 1 1", "-P 2xn"},
                                          {SpaceGroupId::SpaceGroup_14_a3, 14, "a3", "P 21/c 1 1", "-P 2xac"},
                                          {SpaceGroupId::SpaceGroup_15_b1, 15, "b1", "C 1 2/c 1", "-C 2yc"},
                                          {SpaceGroupId::SpaceGroup_15_b2, 15, "b2", "A 1 2/n 1", "-A 2yac"},
                                          {SpaceGroupId::SpaceGroup_15_b3, 15, "b3", "I 1 2/a 1", "-I 2ya"},
                                          {SpaceGroupId::SpaceGroup_15_B1, 15, "-b1", "A 1 2/a 1", "-A 2ya"},
                                          {SpaceGroupId::SpaceGroup_15_B2, 15, "-b2", "C 1 2/n 1", "-C 2ybc"},
                                          {SpaceGroupId::SpaceGroup_15_B3, 15, "-b3", "I 1 2/c 1", "-I 2yc"},
                                          {SpaceGroupId::SpaceGroup_15_c1, 15, "c1", "A 1 1 2/a", "-A 2a"},
                                          {SpaceGroupId::SpaceGroup_15_c2, 15, "c2", "B 1 1 2/n", "-B 2bc"},
                                          {SpaceGroupId::SpaceGroup_15_c3, 15, "c3", "I 1 1 2/b", "-I 2b"},
                                          {SpaceGroupId::SpaceGroup_15_C1, 15, "-c1", "B 1 1 2/b", "-B 2b"},
                                          {SpaceGroupId::SpaceGroup_15_C2, 15, "-c2", "A 1 1 2/n", "-A 2ac"},
                                          {SpaceGroupId::SpaceGroup_15_C3, 15, "-c3", "I 1 1 2/a", "-I 2a"},
                                          {SpaceGroupId::SpaceGroup_15_a1, 15, "a1", "B 2/b 1 1", "-B 2xb"},
                                          {SpaceGroupId::SpaceGroup_15_a2, 15, "a2", "C 2/n 1 1", "-C 2xbc"},
                                          {SpaceGroupId::SpaceGroup_15_a3, 15, "a3", "I 2/c 1 1", "-I 2xc"},
                                          {SpaceGroupId::SpaceGroup_15_A1, 15, "-a1", "C 2/c 1 1", "-C 2xc"},
                                          {SpaceGroupId::SpaceGroup_15_A2, 15, "-a2", "B 2/n 1 1", "-B 2xbc"},
                                          {SpaceGroupId::SpaceGroup_15_A3, 15, "-a3", "I 2/b 1 1", "-I 2xb"},
                                          {SpaceGroupId::SpaceGroup_16, 16, "", "P 2 2 2", "P 2 2"},
                                          {SpaceGroupId::SpaceGroup_17, 17, "", "P 2 2 21", "P 2c 2"},
                                          {SpaceGroupId::SpaceGroup_17_cab, 17, "cab", "P 21 2 2", "P 2a 2a"},
                                          {SpaceGroupId::SpaceGroup_17_bca, 17, "bca", "P 2 21 2", "P 2 2b"},
                                          {SpaceGroupId::SpaceGroup_18, 18, "", "P 21 21 2", "P 2 2ab"},
                                          {SpaceGroupId::SpaceGroup_18_cab, 18, "cab", "P 2 21 21", "P 2bc 2"},
                                          {SpaceGroupId::SpaceGroup_18_bca, 18, "bca", "P 21 2 21", "P 2ac 2ac"},
                                          {SpaceGroupId::SpaceGroup_19, 19, "", "P 21 21 21", "P 2ac 2ab"},
                                          {SpaceGroupId::SpaceGroup_20, 20, "", "C 2 2 21", "C 2c 2"},
                                          {SpaceGroupId::SpaceGroup_20_cab, 20, "cab", "A 21 2 2", "A 2a 2a"},
                                          {SpaceGroupId::SpaceGroup_20_bca, 20, "bca", "B 2 21 2", "B 2 2b"},
                                          {SpaceGroupId::SpaceGroup_21, 21, "", "C 2 2 2", "C 2 2"},
                                          {SpaceGroupId::SpaceGroup_21_cab, 21, "cab", "A 2 2 2", "A 2 2"},
                                          {SpaceGroupId::SpaceGroup_21_bca, 21, "bca", "B 2 2 2", "B 2 2"},
                                          {SpaceGroupId::SpaceGroup_22, 22, "", "F 2 2 2", "F 2 2"},
                                          {SpaceGroupId::SpaceGroup_23, 23, "", "I 2 2 2", "I 2 2"},
                                          {SpaceGroupId::SpaceGroup_24, 24, "", "I 21 21 21", "I 2b 2c"},
                                          {SpaceGroupId::SpaceGroup_25, 25, "", "P m m 2", "P 2 -2"},
                                          {SpaceGroupId::SpaceGroup_25_cab, 25, "cab", "P 2 m m", "P -2 2"},
                                          {SpaceGroupId::SpaceGroup_25_bca, 25, "bca", "P m 2 m", "P -2 -2"},
                                          {SpaceGroupId::SpaceGroup_26, 26, "", "P m c 21", "P 2c -2"},
                                          {SpaceGroupId::SpaceGroup_26_baC, 26, "ba-c", "P c m 21", "P 2c -2c"},
                                          {SpaceGroupId::SpaceGroup_26_cab, 26, "cab", "P 21 m a", "P -2a 2a"},
                                          {SpaceGroupId::SpaceGroup_26_Cba, 26, "-cba", "P 21 a m", "P -2 2a"},
                                          {SpaceGroupId::SpaceGroup_26_bca, 26, "bca", "P b 21 m", "P -2 -2b"},
                                          {SpaceGroupId::SpaceGroup_26_aCb, 26, "a-cb", "P m 21 b", "P -2b -2"},
                                          {SpaceGroupId::SpaceGroup_27, 27, "", "P c c 2", "P 2 -2c"},
                                          {SpaceGroupId::SpaceGroup_27_cab, 27, "cab", "P 2 a a", "P -2a 2"},
                                          {SpaceGroupId::SpaceGroup_27_bca, 27, "bca", "P b 2 b", "P -2b -2b"},
                                          {SpaceGroupId::SpaceGroup_28, 28, "", "P m a 2", "P 2 -2a"},
                                          {SpaceGroupId::SpaceGroup_28_baC, 28, "ba-c", "P b m 2", "P 2 -2b"},
                                          {SpaceGroupId::SpaceGroup_28_cab, 28, "cab", "P 2 m b", "P -2b 2"},
                                          {SpaceGroupId::SpaceGroup_28_Cba, 28, "-cba", "P 2 c m", "P -2c 2"},
                                          {SpaceGroupId::SpaceGroup_28_bca, 28, "bca", "P c 2 m", "P -2c -2c"},
                                          {SpaceGroupId::SpaceGroup_28_aCb, 28, "a-cb", "P m 2 a", "P -2a -2a"},
                                          {SpaceGroupId::SpaceGroup_29, 29, "", "P c a 21", "P 2c -2ac"},
                                          {SpaceGroupId::SpaceGroup_29_baC, 29, "ba-c", "P b c 21", "P 2c -2b"},
                                          {SpaceGroupId::SpaceGroup_29_cab, 29, "cab", "P 21 a b", "P -2b 2a"},
                                          {SpaceGroupId::SpaceGroup_29_Cba, 29, "-cba", "P 21 c a", "P -2ac 2a"},
                                          {SpaceGroupId::SpaceGroup_29_bca, 29, "bca", "P c 21 b", "P -2bc -2c"},
                                          {SpaceGroupId::SpaceGroup_29_aCb, 29, "a-cb", "P b 21 a", "P -2a -2ab"},
                                          {SpaceGroupId::SpaceGroup_30, 30, "", "P n c 2", "P 2 -2bc"},
                                          {SpaceGroupId::SpaceGroup_30_baC, 30, "ba-c", "P c n 2", "P 2 -2ac"},
                                          {SpaceGroupId::SpaceGroup_30_cab, 30, "cab", "P 2 n a", "P -2ac 2"},
                                          {SpaceGroupId::SpaceGroup_30_Cba, 30, "-cba", "P 2 a n", "P -2ab 2"},
                                          {SpaceGroupId::SpaceGroup_30_bca, 30, "bca", "P b 2 n", "P -2ab -2ab"},
                                          {SpaceGroupId::SpaceGroup_30_aCb, 30, "a-cb", "P n 2 b", "P -2bc -2bc"},
                                          {SpaceGroupId::SpaceGroup_31, 31, "", "P m n 21", "P 2ac -2"},
                                          {SpaceGroupId::SpaceGroup_31_baC, 31, "ba-c", "P n m 21", "P 2bc -2bc"},
                                          {SpaceGroupId::SpaceGroup_31_cab, 31, "cab", "P 21 m n", "P -2ab 2ab"},
                                          {SpaceGroupId::SpaceGroup_31_Cba, 31, "-cba", "P 21 n m", "P -2 2ac"},
                                          {SpaceGroupId::SpaceGroup_31_bca, 31, "bca", "P n 21 m", "P -2 -2bc"},
                                          {SpaceGroupId::SpaceGroup_31_aCb, 31, "a-cb", "P m 21 n", "P -2ab -2"},
                                          {SpaceGroupId::SpaceGroup_32, 32, "", "P b a 2", "P 2 -2ab"},
                                          {SpaceGroupId::SpaceGroup_32_cab, 32, "cab", "P 2 c b", "P -2bc 2"},
                                          {SpaceGroupId::SpaceGroup_32_bca, 32, "bca", "P c 2 a", "P -2ac -2ac"},
                                          {SpaceGroupId::SpaceGroup_33, 33, "", "P n a 21", "P 2c -2n"},
                                          {SpaceGroupId::SpaceGroup_33_baC, 33, "ba-c", "P b n 21", "P 2c -2ab"},
                                          {SpaceGroupId::SpaceGroup_33_cab, 33, "cab", "P 21 n b", "P -2bc 2a"},
                                          {SpaceGroupId::SpaceGroup_33_Cba, 33, "-cba", "P 21 c n", "P -2n 2a"},
                                          {SpaceGroupId::SpaceGroup_33_bca, 33, "bca", "P c 21 n", "P -2n -2ac"},
                                          {SpaceGroupId::SpaceGroup_33_aCb, 33, "a-cb", "P n 21 a", "P -2ac -2n"},
                                          {SpaceGroupId::SpaceGroup_34, 34, "", "P n n 2", "P 2 -2n"},
                                          {SpaceGroupId::SpaceGroup_34_cab, 34, "cab", "P 2 n n", "P -2n 2"},
                                          {SpaceGroupId::SpaceGroup_34_bca, 34, "bca", "P n 2 n", "P -2n -2n"},
                                          {SpaceGroupId::SpaceGroup_35, 35, "", "C m m 2", "C 2 -2"},
                                          {SpaceGroupId::SpaceGroup_35_cab, 35, "cab", "A 2 m m", "A -2 2"},
                                          {SpaceGroupId::SpaceGroup_35_bca, 35, "bca", "B m 2 m", "B -2 -2"},
                                          {SpaceGroupId::SpaceGroup_36, 36, "", "C m c 21", "C 2c -2"},
                                          {SpaceGroupId::SpaceGroup_36_baC, 36, "ba-c", "C c m 21", "C 2c -2c"},
                                          {SpaceGroupId::SpaceGroup_36_cab, 36, "cab", "A 21 m a", "A -2a 2a"},
                                          {SpaceGroupId::SpaceGroup_36_Cba, 36, "-cba", "A 21 a m", "A -2 2a"},
                                          {SpaceGroupId::SpaceGroup_36_bca, 36, "bca", "B b 21 m", "B -2 -2b"},
                                          {SpaceGroupId::SpaceGroup_36_aCb, 36, "a-cb", "B m 21 b", "B -2b -2"},
                                          {SpaceGroupId::SpaceGroup_37, 37, "", "C c c 2", "C 2 -2c"},
                                          {SpaceGroupId::SpaceGroup_37_cab, 37, "cab", "A 2 a a", "A -2a 2"},
                                          {SpaceGroupId::SpaceGroup_37_bca, 37, "bca", "B b 2 b", "B -2b -2b"},
                                          {SpaceGroupId::SpaceGroup_38, 38, "", "A m m 2", "A 2 -2"},
                                          {SpaceGroupId::SpaceGroup_38_baC, 38, "ba-c", "B m m 2", "B 2 -2"},
                                          {SpaceGroupId::SpaceGroup_38_cab, 38, "cab", "B 2 m m", "B -2 2"},
                                          {SpaceGroupId::SpaceGroup_38_Cba, 38, "-cba", "C 2 m m", "C -2 2"},
                                          {SpaceGroupId::SpaceGroup_38_bca, 38, "bca", "C m 2 m", "C -2 -2"},
                                          {SpaceGroupId::SpaceGroup_38_aCb, 38, "a-cb", "A m 2 m", "A -2 -2"},
                                          {SpaceGroupId::SpaceGroup_39, 39, "", "A b m 2", "A 2 -2c"},
                                          {SpaceGroupId::SpaceGroup_39_baC, 39, "ba-c", "B m a 2", "B 2 -2c"},
                                          {SpaceGroupId::SpaceGroup_39_cab, 39, "cab", "B 2 c m", "B -2c 2"},
                                          {SpaceGroupId::SpaceGroup_39_Cba, 39, "-cba", "C 2 m b", "C -2b 2"},
                                          {SpaceGroupId::SpaceGroup_39_bca, 39, "bca", "C m 2 a", "C -2b -2b"},
                                          {SpaceGroupId::SpaceGroup_39_aCb, 39, "a-cb", "A c 2 m", "A -2c -2c"},
                                          {SpaceGroupId::SpaceGroup_40, 40, "", "A m a 2", "A 2 -2a"},
                                          {SpaceGroupId::SpaceGroup_40_baC, 40, "ba-c", "B b m 2", "B 2 -2b"},
                                          {SpaceGroupId::SpaceGroup_40_cab, 40, "cab", "B 2 m b", "B -2b 2"},
                                          {SpaceGroupId::SpaceGroup_40_Cba, 40, "-cba", "C 2 c m", "C -2c 2"},
                                          {SpaceGroupId::SpaceGroup_40_bca, 40, "bca", "C c 2 m", "C -2c -2c"},
                                          {SpaceGroupId::SpaceGroup_40_aCb, 40, "a-cb", "A m 2 a", "A -2a -2a"},
                                          {SpaceGroupId::SpaceGroup_41, 41, "", "A b a 2", "A 2 -2ac"},
                                          {SpaceGroupId::SpaceGroup_41_baC, 41, "ba-c", "B b a 2", "B 2 -2bc"},
                                          {SpaceGroupId::SpaceGroup_41_cab, 41, "cab", "B 2 c b", "B -2bc 2"},
                                          {SpaceGroupId::SpaceGroup_41_Cba, 41, "-cba", "C 2 c b", "C -2bc 2"},
                                          {SpaceGroupId::SpaceGroup_41_bca, 41, "bca", "C c 2 a", "C -2bc -2bc"},
                                          {SpaceGroupId::SpaceGroup_41_aCb, 41, "a-cb", "A c 2 a", "A -2ac -2ac"},
                                          {SpaceGroupId::SpaceGroup_42, 42, "", "F m m 2", "F 2 -2"},
                                          {SpaceGroupId::SpaceGroup_42_cab, 42, "cab", "F 2 m m", "F -2 2"},
                                          {SpaceGroupId::SpaceGroup_42_bca, 42, "bca", "F m 2 m", "F -2 -2"},
                                          {SpaceGroupId::SpaceGroup_43, 43, "", "F d d 2", "F 2 -2d"},
                                          {SpaceGroupId::SpaceGroup_43_cab, 43, "cab", "F 2 d d", "F -2d 2"},
                                          {SpaceGroupId::SpaceGroup_43_bca, 43, "bca", "F d 2 d", "F -2d -2d"},
                                          {SpaceGroupId::SpaceGroup_44, 44, "", "I m m 2", "I 2 -2"},
                                          {SpaceGroupId::SpaceGroup_44_cab, 44, "cab", "I 2 m m", "I -2 2"},
                                          {SpaceGroupId::SpaceGroup_44_bca, 44, "bca", "I m 2 m", "I -2 -2"},
                                          {SpaceGroupId::SpaceGroup_45, 45, "", "I b a 2", "I 2 -2c"},
                                          {SpaceGroupId::SpaceGroup_45_cab, 45, "cab", "I 2 c b", "I -2a 2"},
                                          {SpaceGroupId::SpaceGroup_45_bca, 45, "bca", "I c 2 a", "I -2b -2b"},
                                          {SpaceGroupId::SpaceGroup_46, 46, "", "I m a 2", "I 2 -2a"},
                                          {SpaceGroupId::SpaceGroup_46_baC, 46, "ba-c", "I b m 2", "I 2 -2b"},
                                          {SpaceGroupId::SpaceGroup_46_cab, 46, "cab", "I 2 m b", "I -2b 2"},
                                          {SpaceGroupId::SpaceGroup_46_Cba, 46, "-cba", "I 2 c m", "I -2c 2"},
                                          {SpaceGroupId::SpaceGroup_46_bca, 46, "bca", "I c 2 m", "I -2c -2c"},
                                          {SpaceGroupId::SpaceGroup_46_aCb, 46, "a-cb", "I m 2 a", "I -2a -2a"},
                                          {SpaceGroupId::SpaceGroup_47, 47, "", "P m m m", "-P 2 2"},
                                          {SpaceGroupId::SpaceGroup_48_1, 48, "1", "P n n n:1", "P 2 2 -1n"},
                                          {SpaceGroupId::SpaceGroup_48_2, 48, "2", "P n n n:2", "-P 2ab 2bc"},
                                          {SpaceGroupId::SpaceGroup_49, 49, "", "P c c m", "-P 2 2c"},
                                          {SpaceGroupId::SpaceGroup_49_cab, 49, "cab", "P m a a", "-P 2a 2"},
                                          {SpaceGroupId::SpaceGroup_49_bca, 49, "bca", "P b m b", "-P 2b 2b"},
                                          {SpaceGroupId::SpaceGroup_50_1, 50, "1", "P b a n:1", "P 2 2 -1ab"},
                                          {SpaceGroupId::SpaceGroup_50_2, 50, "2", "P b a n:2", "-P 2ab 2b"},
                                          {SpaceGroupId::SpaceGroup_50_1cab, 50, "1cab", "P n c b:1", "P 2 2 -1bc"},
                                          {SpaceGroupId::SpaceGroup_50_2cab, 50, "2cab", "P n c b:2", "-P 2b 2bc"},
                                          {SpaceGroupId::SpaceGroup_50_1bca, 50, "1bca", "P c n a:1", "P 2 2 -1ac"},
                                          {SpaceGroupId::SpaceGroup_50_2bca, 50, "2bca", "P c n a:2", "-P 2a 2c"},
                                          {SpaceGroupId::SpaceGroup_51, 51, "", "P m m a", "-P 2a 2a"},
                                          {SpaceGroupId::SpaceGroup_51_baC, 51, "ba-c", "P m m b", "-P 2b 2"},
                                          {SpaceGroupId::SpaceGroup_51_cab, 51, "cab", "P b m m", "-P 2 2b"},
                                          {SpaceGroupId::SpaceGroup_51_Cba, 51, "-cba", "P c m m", "-P 2c 2c"},
                                          {SpaceGroupId::SpaceGroup_51_bca, 51, "bca", "P m c m", "-P 2c 2"},
                                          {SpaceGroupId::SpaceGroup_51_aCb, 51, "a-cb", "P m a m", "-P 2 2a"},
                                          {SpaceGroupId::SpaceGroup_52, 52, "", "P n n a", "-P 2a 2bc"},
                                          {SpaceGroupId::SpaceGroup_52_baC, 52, "ba-c", "P n n b", "-P 2b 2n"},
                                          {SpaceGroupId::SpaceGroup_52_cab, 52, "cab", "P b n n", "-P 2n 2b"},
                                          {SpaceGroupId::SpaceGroup_52_Cba, 52, "-cba", "P c n n", "-P 2ab 2c"},
                                          {SpaceGroupId::SpaceGroup_52_bca, 52, "bca", "P n c n", "-P 2ab 2n"},
                                          {SpaceGroupId::SpaceGroup_52_aCb, 52, "a-cb", "P n a n", "-P 2n 2bc"},
                                          {SpaceGroupId::SpaceGroup_53, 53, "", "P m n a", "-P 2ac 2"},
                                          {SpaceGroupId::SpaceGroup_53_baC, 53, "ba-c", "P n m b", "-P 2bc 2bc"},
                                          {SpaceGroupId::SpaceGroup_53_cab, 53, "cab", "P b m n", "-P 2ab 2ab"},
                                          {SpaceGroupId::SpaceGroup_53_Cba, 53, "-cba", "P c n m", "-P 2 2ac"},
                                          {SpaceGroupId::SpaceGroup_53_bca, 53, "bca", "P n c m", "-P 2 2bc"},
                                          {SpaceGroupId::SpaceGroup_53_aCb, 53, "a-cb", "P m a n", "-P 2ab 2"},
                                          {SpaceGroupId::SpaceGroup_54, 54, "", "P c c a", "-P 2a 2ac"},
                                          {SpaceGroupId::SpaceGroup_54_baC, 54, "ba-c", "P c c b", "-P 2b 2c"},
                                          {SpaceGroupId::SpaceGroup_54_cab, 54, "cab", "P b a a", "-P 2a 2b"},
                                          {SpaceGroupId::SpaceGroup_54_Cba, 54, "-cba", "P c a a", "-P 2ac 2c"},
                                          {SpaceGroupId::SpaceGroup_54_bca, 54, "bca", "P b c b", "-P 2bc 2b"},
                                          {SpaceGroupId::SpaceGroup_54_aCb, 54, "a-cb", "P b a b", "-P 2b 2ab"},
                                          {SpaceGroupId::SpaceGroup_55, 55, "", "P b a m", "-P 2 2ab"},
                                          {SpaceGroupId::SpaceGroup_55_cab, 55, "cab", "P m c b", "-P 2bc 2"},
                                          {SpaceGroupId::SpaceGroup_55_bca, 55, "bca", "P c m a", "-P 2ac 2ac"},
                                          {SpaceGroupId::SpaceGroup_56, 56, "", "P c c n", "-P 2ab 2ac"},
                                          {SpaceGroupId::SpaceGroup_56_cab, 56, "cab", "P n a a", "-P 2ac 2bc"},
                                          {SpaceGroupId::SpaceGroup_56_bca, 56, "bca", "P b n b", "-P 2bc 2ab"},
                                          {SpaceGroupId::SpaceGroup_57, 57, "", "P b c m", "-P 2c 2b"},
                                          {SpaceGroupId::SpaceGroup_57_baC, 57, "ba-c", "P c a m", "-P 2c 2ac"},
                                          {SpaceGroupId::SpaceGroup_57_cab, 57, "cab", "P m c a", "-P 2ac 2a"},
                                          {SpaceGroupId::SpaceGroup_57_Cba, 57, "-cba", "P m a b", "-P 2b 2a"},
                                          {SpaceGroupId::SpaceGroup_57_bca, 57, "bca", "P b m a", "-P 2a 2ab"},
                                          {SpaceGroupId::SpaceGroup_57_aCb, 57, "a-cb", "P c m b", "-P 2bc 2c"},
                                          {SpaceGroupId::SpaceGroup_58, 58, "", "P n n m", "-P 2 2n"},
                                          {SpaceGroupId::SpaceGroup_58_cab, 58, "cab", "P m n n", "-P 2n 2"},
                                          {SpaceGroupId::SpaceGroup_58_bca, 58, "bca", "P n m n", "-P 2n 2n"},
                                          {SpaceGroupId::SpaceGroup_59_1, 59, "1", "P m m n:1", "P 2 2ab -1ab"},
                                          {SpaceGroupId::SpaceGroup_59_2, 59, "2", "P m m n:2", "-P 2ab 2a"},
                                          {SpaceGroupId::SpaceGroup_59_1cab, 59, "1cab", "P n m m:1", "P 2bc 2 -1bc"},
                                          {SpaceGroupId::SpaceGroup_59_2cab, 59, "2cab", "P n m m:2", "-P 2c 2bc"},
                                          {SpaceGroupId::SpaceGroup_59_1bca, 59, "1bca", "P m n m:1", "P 2ac 2ac -1ac"},
                                          {SpaceGroupId::SpaceGroup_59_2bca, 59, "2bca", "P m n m:2", "-P 2c 2a"},
                                          {SpaceGroupId::SpaceGroup_60, 60, "", "P b c n", "-P 2n 2ab"},
                                          {SpaceGroupId::SpaceGroup_60_baC, 60, "ba-c", "P c a n", "-P 2n 2c"},
                                          {SpaceGroupId::SpaceGroup_60_cab, 60, "cab", "P n c a", "-P 2a 2n"},
                                          {SpaceGroupId::SpaceGroup_60_Cba, 60, "-cba", "P n a b", "-P 2bc 2n"},
                                          {SpaceGroupId::SpaceGroup_60_bca, 60, "bca", "P b n a", "-P 2ac 2b"},
                                          {SpaceGroupId::SpaceGroup_60_aCb, 60, "a-cb", "P c n b", "-P 2b 2ac"},
                                          {SpaceGroupId::SpaceGroup_61, 61, "", "P b c a", "-P 2ac 2ab"},
                                          {SpaceGroupId::SpaceGroup_61_baC, 61, "ba-c", "P c a b", "-P 2bc 2ac"},
                                          {SpaceGroupId::SpaceGroup_62, 62, "", "P n m a", "-P 2ac 2n"},
                                          {SpaceGroupId::SpaceGroup_62_baC, 62, "ba-c", "P m n b", "-P 2bc 2a"},
                                          {SpaceGroupId::SpaceGroup_62_cab, 62, "cab", "P b n m", "-P 2c 2ab"},
                                          {SpaceGroupId::SpaceGroup_62_Cba, 62, "-cba", "P c m n", "-P 2n 2ac"},
                                          {SpaceGroupId::SpaceGroup_62_bca, 62, "bca", "P m c n", "-P 2n 2a"},
                                          {SpaceGroupId::SpaceGroup_62_aCb, 62, "a-cb", "P n a m", "-P 2c 2n"},
                                          {SpaceGroupId::SpaceGroup_63, 63, "", "C m c m", "-C 2c 2"},
                                          {SpaceGroupId::SpaceGroup_63_baC, 63, "ba-c", "C c m m", "-C 2c 2c"},
                                          {SpaceGroupId::SpaceGroup_63_cab, 63, "cab", "A m m a", "-A 2a 2a"},
                                          {SpaceGroupId::SpaceGroup_63_Cba, 63, "-cba", "A m a m", "-A 2 2a"},
                                          {SpaceGroupId::SpaceGroup_63_bca, 63, "bca", "B b m m", "-B 2 2b"},
                                          {SpaceGroupId::SpaceGroup_63_aCb, 63, "a-cb", "B m m b", "-B 2b 2"},
                                          {SpaceGroupId::SpaceGroup_64, 64, "", "C m c a", "-C 2bc 2"},
                                          {SpaceGroupId::SpaceGroup_64_baC, 64, "ba-c", "C c m b", "-C 2bc 2bc"},
                                          {SpaceGroupId::SpaceGroup_64_cab, 64, "cab", "A b m a", "-A 2ac 2ac"},
                                          {SpaceGroupId::SpaceGroup_64_Cba, 64, "-cba", "A c a m", "-A 2 2ac"},
                                          {SpaceGroupId::SpaceGroup_64_bca, 64, "bca", "B b c m", "-B 2 2bc"},
                                          {SpaceGroupId::SpaceGroup_64_aCb, 64, "a-cb", "B m a b", "-B 2bc 2"},
                                          {SpaceGroupId::SpaceGroup_65, 65, "", "C m m m", "-C 2 2"},
                                          {SpaceGroupId::SpaceGroup_65_cab, 65, "cab", "A m m m", "-A 2 2"},
                                          {SpaceGroupId::SpaceGroup_65_bca, 65, "bca", "B m m m", "-B 2 2"},
                                          {SpaceGroupId::SpaceGroup_66, 66, "", "C c c m", "-C 2 2c"},
                                          {SpaceGroupId::SpaceGroup_66_cab, 66, "cab", "A m a a", "-A 2a 2"},
                                          {SpaceGroupId::SpaceGroup_66_bca, 66, "bca", "B b m b", "-B 2b 2b"},
                                          {SpaceGroupId::SpaceGroup_67, 67, "", "C m m a", "-C 2b 2"},
                                          {SpaceGroupId::SpaceGroup_67_baC, 67, "ba-c", "C m m b", "-C 2b 2b"},
                                          {SpaceGroupId::SpaceGroup_67_cab, 67, "cab", "A b m m", "-A 2c 2c"},
                                          {SpaceGroupId::SpaceGroup_67_Cba, 67, "-cba", "A c m m", "-A 2 2c"},
                                          {SpaceGroupId::SpaceGroup_67_bca, 67, "bca", "B m c m", "-B 2 2c"},
                                          {SpaceGroupId::SpaceGroup_67_aCb, 67, "a-cb", "B m a m", "-B 2c 2"},
                                          {SpaceGroupId::SpaceGroup_68_1, 68, "1", "C c c a:1", "C 2 2 -1bc"},
                                          {SpaceGroupId::SpaceGroup_68_2, 68, "2", "C c c a:2", "-C 2b 2bc"},
                                          {SpaceGroupId::SpaceGroup_68_1baC, 68, "1ba-c", "C c c b:1", "C 2 2 -1bc"},
                                          {SpaceGroupId::SpaceGroup_68_2baC, 68, "2ba-c", "C c c b:2", "-C 2b 2c"},
                                          {SpaceGroupId::SpaceGroup_68_1cab, 68, "1cab", "A b a a:1", "A 2 2 -1ac"},
                                          {SpaceGroupId::SpaceGroup_68_2cab, 68, "2cab", "A b a a:2", "-A 2a 2c"},
                                          {SpaceGroupId::SpaceGroup_68_1Cba, 68, "1-cba", "A c a a:1", "A 2 2 -1ac"},
                                          {SpaceGroupId::SpaceGroup_68_2Cba, 68, "2-cba", "A c a a:2", "-A 2ac 2c"},
                                          {SpaceGroupId::SpaceGroup_68_1bca, 68, "1bca", "B b c b:1", "B 2 2 -1bc"},
                                          {SpaceGroupId::SpaceGroup_68_2bca, 68, "2bca", "B b c b:2", "-B 2bc 2b"},
                                          {SpaceGroupId::SpaceGroup_68_1aCb, 68, "1a-cb", "B b a b:1", "B 2 2 -1bc"},
                                          {SpaceGroupId::SpaceGroup_68_2aCb, 68, "2a-cb", "B b a b:2", "-B 2b 2bc"},
                                          {SpaceGroupId::SpaceGroup_69, 69, "", "F m m m", "-F 2 2"},
                                          {SpaceGroupId::SpaceGroup_70_1, 70, "1", "F d d d:1", "F 2 2 -1d"},
                                          {SpaceGroupId::SpaceGroup_70_2, 70, "2", "F d d d:2", "-F 2uv 2vw"},
                                          {SpaceGroupId::SpaceGroup_71, 71, "", "I m m m", "-I 2 2"},
                                          {SpaceGroupId::SpaceGroup_72, 72, "", "I b a m", "-I 2 2c"},
                                          {SpaceGroupId::SpaceGroup_72_cab, 72, "cab", "I m c b", "-I 2a 2"},
                                          {SpaceGroupId::SpaceGroup_72_bca, 72, "bca", "I c m a", "-I 2b 2b"},
                                          {SpaceGroupId::SpaceGroup_73, 73, "", "I b c a", "-I 2b 2c"},
                                          {SpaceGroupId::SpaceGroup_73_baC, 73, "ba-c", "I c a b", "-I 2a 2b"},
                                          {SpaceGroupId::SpaceGroup_74, 74, "", "I m m a", "-I 2b 2"},
                                          {SpaceGroupId::SpaceGroup_74_baC, 74, "ba-c", "I m m b", "-I 2a 2a"},
                                          {SpaceGroupId::SpaceGroup_74_cab, 74, "cab", "I b m m", "-I 2c 2c"},
                                          {SpaceGroupId::SpaceGroup_74_Cba, 74, "-cba", "I c m m", "-I 2 2b"},
                                          {SpaceGroupId::SpaceGroup_74_bca, 74, "bca", "I m c m", "-I 2 2a"},
                                          {SpaceGroupId::SpaceGroup_74_aCb, 74, "a-cb", "I m a m", "-I 2c 2"},
                                          {SpaceGroupId::SpaceGroup_75, 75, "", "P 4", "P 4"},
                                          {SpaceGroupId::SpaceGroup_76, 76, "", "P 41", "P 4w"},
                                          {SpaceGroupId::SpaceGroup_77, 77, "", "P 42", "P 4c"},
                                          {SpaceGroupId::SpaceGroup_78, 78, "", "P 43", "P 4cw"},
                                          {SpaceGroupId::SpaceGroup_79, 79, "", "I 4", "I 4"},
                                          {SpaceGroupId::SpaceGroup_80, 80, "", "I 41", "I 4bw"},
                                          {SpaceGroupId::SpaceGroup_81, 81, "", "P -4", "P -4"},
                                          {SpaceGroupId::SpaceGroup_82, 82, "", "I -4", "I -4"},
                                          {SpaceGroupId::SpaceGroup_83, 83, "", "P 4/m", "-P 4"},
                                          {SpaceGroupId::SpaceGroup_84, 84, "", "P 42/m", "-P 4c"},
                                          {SpaceGroupId::SpaceGroup_85_1, 85, "1", "P 4/n:1", "P 4ab -1ab"},
                                          {SpaceGroupId::SpaceGroup_85_2, 85, "2", "P 4/n:2", "-P 4a"},
                                          {SpaceGroupId::SpaceGroup_86_1, 86, "1", "P 42/n:1", "P 4n -1n"},
                                          {SpaceGroupId::SpaceGroup_86_2, 86, "2", "P 42/n:2", "-P 4bc"},
                                          {SpaceGroupId::SpaceGroup_87, 87, "", "I 4/m", "-I 4"},
                                          {SpaceGroupId::SpaceGroup_88_1, 88, "1", "I 41/a:1", "I 4bw -1bw"},
                                          {SpaceGroupId::SpaceGroup_88_2, 88, "2", "I 41/a:2", "-I 4ad"},
                                          {SpaceGroupId::SpaceGroup_89, 89, "", "P 4 2 2", "P 4 2"},
                                          {SpaceGroupId::SpaceGroup_90, 90, "", "P 42 1 2", "P 4ab 2ab"},
                                          {SpaceGroupId::SpaceGroup_91, 91, "", "P 41 2 2", "P 4w 2c"},
                                          {SpaceGroupId::SpaceGroup_92, 92, "", "P 41 21 2", "P 4abw 2nw"},
                                          {SpaceGroupId::SpaceGroup_93, 93, "", "P 42 2 2", "P 4c 2"},
                                          {SpaceGroupId::SpaceGroup_94, 94, "", "P 42 21 2", "P 4n 2n"},
                                          {SpaceGroupId::SpaceGroup_95, 95, "", "P 43 2 2", "P 4cw 2c"},
                                          {SpaceGroupId::SpaceGroup_96, 96, "", "P 43 21 2", "P 4nw 2abw"},
                                          {SpaceGroupId::SpaceGroup_97, 97, "", "I 4 2 2", "I 4 2"},
                                          {SpaceGroupId::SpaceGroup_98, 98, "", "I 41 2 2", "I 4bw 2bw"},
                                          {SpaceGroupId::SpaceGroup_99, 99, "", "P 4 m m", "P 4 -2"},
                                          {SpaceGroupId::SpaceGroup_100, 100, "", "P 4 b m", "P 4 -2ab"},
                                          {SpaceGroupId::SpaceGroup_101, 101, "", "P 42 c m", "P 4c -2c"},
                                          {SpaceGroupId::SpaceGroup_102, 102, "", "P 42 n m", "P 4n -2n"},
                                          {SpaceGroupId::SpaceGroup_103, 103, "", "P 4 c c", "P 4 -2c"},
                                          {SpaceGroupId::SpaceGroup_104, 104, "", "P 4 n c", "P 4 -2n"},
                                          {SpaceGroupId::SpaceGroup_105, 105, "", "P 42 m c", "P 4c -2"},
                                          {SpaceGroupId::SpaceGroup_106, 106, "", "P 42 b c", "P 4c -2ab"},
                                          {SpaceGroupId::SpaceGroup_107, 107, "", "I 4 m m", "I 4 -2"},
                                          {SpaceGroupId::SpaceGroup_108, 108, "", "I 4 c m", "I 4 -2c"},
                                          {SpaceGroupId::SpaceGroup_109, 109, "", "I 41 m d", "I 4bw -2"},
                                          {SpaceGroupId::SpaceGroup_110, 110, "", "I 41 c d", "I 4bw -2c"},
                                          {SpaceGroupId::SpaceGroup_111, 111, "", "P -4 2 m", "P -4 2"},
                                          {SpaceGroupId::SpaceGroup_112, 112, "", "P -4 2 c", "P -4 2c"},
                                          {SpaceGroupId::SpaceGroup_113, 113, "", "P -4 21 m", "P -4 2ab"},
                                          {SpaceGroupId::SpaceGroup_114, 114, "", "P -4 21 c", "P -4 2n"},
                                          {SpaceGroupId::SpaceGroup_115, 115, "", "P -4 m 2", "P -4 -2"},
                                          {SpaceGroupId::SpaceGroup_116, 116, "", "P -4 c 2", "P -4 -2c"},
                                          {SpaceGroupId::SpaceGroup_117, 117, "", "P -4 b 2", "P -4 -2ab"},
                                          {SpaceGroupId::SpaceGroup_118, 118, "", "P -4 n 2", "P -4 -2n"},
                                          {SpaceGroupId::SpaceGroup_119, 119, "", "I -4 m 2", "I -4 -2"},
                                          {SpaceGroupId::SpaceGroup_120, 120, "", "I -4 c 2", "I -4 -2c"},
                                          {SpaceGroupId::SpaceGroup_121, 121, "", "I -4 2 m", "I -4 2"},
                                          {SpaceGroupId::SpaceGroup_122, 122, "", "I -4 2 d", "I -4 2bw"},
                                          {SpaceGroupId::SpaceGroup_123, 123, "", "P 4/m m m", "-P 4 2"},
                                          {SpaceGroupId::SpaceGroup_124, 124, "", "P 4/m c c", "-P 4 2c"},
                                          {SpaceGroupId::SpaceGroup_125_1, 125, "1", "P 4/n b m:1", "P 4 2 -1ab"},
                                          {SpaceGroupId::SpaceGroup_125_2, 125, "2", "P 4/n b m:2", "-P 4a 2b"},
                                          {SpaceGroupId::SpaceGroup_126_1, 126, "1", "P 4/n n c:1", "P 4 2 -1n"},
                                          {SpaceGroupId::SpaceGroup_126_2, 126, "2", "P 4/n n c:2", "-P 4a 2bc"},
                                          {SpaceGroupId::SpaceGroup_127, 127, "", "P 4/m b m", "-P 4 2ab"},
                                          {SpaceGroupId::SpaceGroup_128, 128, "", "P 4/m n c", "-P 4 2n"},
                                          {SpaceGroupId::SpaceGroup_129_1, 129, "1", "P 4/n m m:1", "P 4ab 2ab -1ab"},
                                          {SpaceGroupId::SpaceGroup_129_2, 129, "2", "P 4/n m m:2", "-P 4a 2a"},
                                          {SpaceGroupId::SpaceGroup_130_1, 130, "1", "P 4/n c c:1", "P 4ab 2n -1ab"},
                                          {SpaceGroupId::SpaceGroup_130_2, 130, "2", "P 4/n c c:2", "-P 4a 2ac"},
                                          {SpaceGroupId::SpaceGroup_131, 131, "", "P 42/m m c", "-P 4c 2"},
                                          {SpaceGroupId::SpaceGroup_132, 132, "", "P 42/m c m", "-P 4c 2c"},
                                          {SpaceGroupId::SpaceGroup_133_1, 133, "1", "P 42/n b c:1", "P 4n 2c -1n"},
                                          {SpaceGroupId::SpaceGroup_133_2, 133, "2", "P 42/n b c:2", "-P 4ac 2b"},
                                          {SpaceGroupId::SpaceGroup_134_1, 134, "1", "P 42/n n m:1", "P 4n 2 -1n"},
                                          {SpaceGroupId::SpaceGroup_134_2, 134, "2", "P 42/n n m:2", "-P 4ac 2bc"},
                                          {SpaceGroupId::SpaceGroup_135, 135, "", "P 42/m b c", "-P 4c 2ab"},
                                          {SpaceGroupId::SpaceGroup_136, 136, "", "P 42/m n m", "-P 4n 2n"},
                                          {SpaceGroupId::SpaceGroup_137_1, 137, "1", "P 42/n m c:1", "P 4n 2n -1n"},
                                          {SpaceGroupId::SpaceGroup_137_2, 137, "2", "P 42/n m c:2", "-P 4ac 2a"},
                                          {SpaceGroupId::SpaceGroup_138_1, 138, "1", "P 42/n c m:1", "P 4n 2ab -1n"},
                                          {SpaceGroupId::SpaceGroup_138_2, 138, "2", "P 42/n c m:2", "-P 4ac 2ac"},
                                          {SpaceGroupId::SpaceGroup_139, 139, "", "I 4/m m m", "-I 4 2"},
                                          {SpaceGroupId::SpaceGroup_140, 140, "", "I 4/m c m", "-I 4 2c"},
                                          {SpaceGroupId::SpaceGroup_141_1, 141, "1", "I 41/a m d:1", "I 4bw 2bw -1bw"},
                                          {SpaceGroupId::SpaceGroup_141_2, 141, "2", "I 41/a m d:2", "-I 4bd 2"},
                                          {SpaceGroupId::SpaceGroup_142_1, 142, "1", "I 41/a c d:1", "I 4bw 2aw -1bw"},
                                          {SpaceGroupId::SpaceGroup_142_2, 142, "2", "I 41/a c d:2", "-I 4bd 2c"},
                                          {SpaceGroupId::SpaceGroup_143, 143, "", "P 3", "P 3"},
                                          {SpaceGroupId::SpaceGroup_144, 144, "", "P 31", "P 31"},
                                          {SpaceGroupId::SpaceGroup_145, 145, "", "P 32", "P 32"},
                                          {SpaceGroupId::SpaceGroup_146_H, 146, "H", "R 3:H", "R 3"},
                                          {SpaceGroupId::SpaceGroup_146_R, 146, "R", "R 3:R", "P 3*"},
                                          {SpaceGroupId::SpaceGroup_147, 147, "", "P -3", "-P 3"},
                                          {SpaceGroupId::SpaceGroup_148_H, 148, "H", "R -3:H", "-R 3"},
                                          {SpaceGroupId::SpaceGroup_148_R, 148, "R", "R -3:R", "-P 3*"},
                                          {SpaceGroupId::SpaceGroup_149, 149, "", "P 3 1 2", "P 3 2"},
                                          {SpaceGroupId::SpaceGroup_150, 150, "", "P 3 2 1", "P 3 2\""},
                                          {SpaceGroupId::SpaceGroup_151, 151, "", "P 31 1 2", "P 31 2c (0 0 1)"},
                                          {SpaceGroupId::SpaceGroup_152, 152, "", "P 31 2 1", "P 31 2\""},
                                          {SpaceGroupId::SpaceGroup_153, 153, "", "P 32 1 2", "P 32 2c (0 0 -1)"},
                                          {SpaceGroupId::SpaceGroup_154, 154, "", "P 32 2 1", "P 32 2\""},
                                          {SpaceGroupId::SpaceGroup_155_H, 155, "H", "R 32:H", "R 3 2\""},
                                          {SpaceGroupId::SpaceGroup_155_R, 155, "R", "R 32:R", "P 3* 2"},
                                          {SpaceGroupId::SpaceGroup_156, 156, "", "P 3 m 1", "P 3 -2\""},
                                          {SpaceGroupId::SpaceGroup_157, 157, "", "P 3 1 m", "P 3 -2"},
                                          {SpaceGroupId::SpaceGroup_158, 158, "", "P 3 c 1", "P 3 -2\"c"},
                                          {SpaceGroupId::SpaceGroup_159, 159, "", "P 3 1 c", "P 3 -2c"},
                                          {SpaceGroupId::SpaceGroup_160_H, 160, "H", "R 3 m:H", "R 3 -2\""},
                                          {SpaceGroupId::SpaceGroup_160_R, 160, "R", "R 3 m:R", "P 3* -2"},
                                          {SpaceGroupId::SpaceGroup_161_H, 161, "H", "R 3 c:H", "R 3 -2\"c"},
                                          {SpaceGroupId::SpaceGroup_161_R, 161, "R", "R 3 c:R", "P 3* -2n"},
                                          {SpaceGroupId::SpaceGroup_162, 162, "", "P -3 1 m", "-P 3 2"},
                                          {SpaceGroupId::SpaceGroup_163, 163, "", "P -3 1 c", "-P 3 2c"},
                                          {SpaceGroupId::SpaceGroup_164, 164, "", "P -3 m 1", "-P 3 2\""},
                                          {SpaceGroupId::SpaceGroup_165, 165, "", "P -3 c 1", "-P 3 2\"c"},
                                          {SpaceGroupId::SpaceGroup_166_H, 166, "H", "R -3 m:H", "-R 3 2\""},
                                          {SpaceGroupId::SpaceGroup_166_R, 166, "R", "R -3 m:R", "-P 3* 2"},
                                          {SpaceGroupId::SpaceGroup_167_H, 167, "H", "R -3 c:H", "-R 3 2\"c"},
                                          {SpaceGroupId::SpaceGroup_167_R, 167, "R", "R -3 c:R", "-P 3* 2n"},
                                          {SpaceGroupId::SpaceGroup_168, 168, "", "P 6", "P 6"},
                                          {SpaceGroupId::SpaceGroup_169, 169, "", "P 61", "P 61"},
                                          {SpaceGroupId::SpaceGroup_170, 170, "", "P 65", "P 65"},
                                          {SpaceGroupId::SpaceGroup_171, 171, "", "P 62", "P 62"},
                                          {SpaceGroupId::SpaceGroup_172, 172, "", "P 64", "P 64"},
                                          {SpaceGroupId::SpaceGroup_173, 173, "", "P 63", "P 6c"},
                                          {SpaceGroupId::SpaceGroup_174, 174, "", "P -6", "P -6"},
                                          {SpaceGroupId::SpaceGroup_175, 175, "", "P 6/m", "-P 6"},
                                          {SpaceGroupId::SpaceGroup_176, 176, "", "P 63/m", "-P 6c"},
                                          {SpaceGroupId::SpaceGroup_177, 177, "", "P 6 2 2", "P 6 2"},
                                          {SpaceGroupId::SpaceGroup_178, 178, "", "P 61 2 2", "P 61 2 (0 0 -1)"},
                                          {SpaceGroupId::SpaceGroup_179, 179, "", "P 65 2 2", "P 65 2 (0 0 1)"},
                                          {SpaceGroupId::SpaceGroup_180, 180, "", "P 62 2 2", "P 62 2c (0 0 1)"},
                                          {SpaceGroupId::SpaceGroup_181, 181, "", "P 64 2 2", "P 64 2c (0 0 -1)"},
                                          {SpaceGroupId::SpaceGroup_182, 182, "", "P 63 2 2", "P 6c 2c"},
                                          {SpaceGroupId::SpaceGroup_183, 183, "", "P 6 m m", "P 6 -2"},
                                          {SpaceGroupId::SpaceGroup_184, 184, "", "P 6 c c", "P 6 -2c"},
                                          {SpaceGroupId::SpaceGroup_185, 185, "", "P 63 c m", "P 6c -2"},
                                          {SpaceGroupId::SpaceGroup_186, 186, "", "P 63 m c", "P 6c -2c"},
                                          {SpaceGroupId::SpaceGroup_187, 187, "", "P -6 m 2", "P -6 2"},
                                          {SpaceGroupId::SpaceGroup_188, 188, "", "P -6 c 2", "P -6c 2"},
                                          {SpaceGroupId::SpaceGroup_189, 189, "", "P -6 2 m", "P -6 -2"},
                                          {SpaceGroupId::SpaceGroup_190, 190, "", "P -6 2 c", "P -6c -2c"},
                                          {SpaceGroupId::SpaceGroup_191, 191, "", "P 6/m m m", "-P 6 2"},
                                          {SpaceGroupId::SpaceGroup_192, 192, "", "P 6/m c c", "-P 6 2c"},
                                          {SpaceGroupId::SpaceGroup_193, 193, "", "P 63/m c m", "-P 6c 2"},
                                          {SpaceGroupId::SpaceGroup_194, 194, "", "P 63/m m c", "-P 6c 2c"},
                                          {SpaceGroupId::SpaceGroup_195, 195, "", "P 2 3", "P 2 2 3"},
                                          {SpaceGroupId::SpaceGroup_196, 196, "", "F 2 3", "F 2 2 3"},
                                          {SpaceGroupId::SpaceGroup_197, 197, "", "I 2 3", "I 2 2 3"},
                                          {SpaceGroupId::SpaceGroup_198, 198, "", "P 21 3", "P 2ac 2ab 3"},
                                          {SpaceGroupId::SpaceGroup_199, 199, "", "I 21 3", "I 2b 2c 3"},
                                          {SpaceGroupId::SpaceGroup_200, 200, "", "P m -3", "-P 2 2 3"},
                                          {SpaceGroupId::SpaceGroup_201_1, 201, "1", "P n -3:1", "P 2 2 3 -1n"},
                                          {SpaceGroupId::SpaceGroup_201_2, 201, "2", "P n -3:2", "-P 2ab 2bc 3"},
                                          {SpaceGroupId::SpaceGroup_202, 202, "", "F m -3", "-F 2 2 3"},
                                          {SpaceGroupId::SpaceGroup_203_1, 203, "1", "F d -3:1", "F 2 2 3 -1d"},
                                          {SpaceGroupId::SpaceGroup_203_2, 203, "2", "F d -3:2", "-F 2uv 2vw 3"},
                                          {SpaceGroupId::SpaceGroup_204, 204, "", "I m -3", "-I 2 2 3"},
                                          {SpaceGroupId::SpaceGroup_205, 205, "", "P a -3", "-P 2ac 2ab 3"},
                                          {SpaceGroupId::SpaceGroup_206, 206, "", "I a -3", "-I 2b 2c 3"},
                                          {SpaceGroupId::SpaceGroup_207, 207, "", "P 4 3 2", "P 4 2 3"},
                                          {SpaceGroupId::SpaceGroup_208, 208, "", "P 42 3 2", "P 4n 2 3"},
                                          {SpaceGroupId::SpaceGroup_209, 209, "", "F 4 3 2", "F 4 2 3"},
                                          {SpaceGroupId::SpaceGroup_210, 210, "", "F 41 3 2", "F 4d 2 3"},
                                          {SpaceGroupId::SpaceGroup_211, 211, "", "I 4 3 2", "I 4 2 3"},
                                          {SpaceGroupId::SpaceGroup_212, 212, "", "P 43 3 2", "P 4acd 2ab 3"},
                                          {SpaceGroupId::SpaceGroup_213, 213, "", "P 41 3 2", "P 4bd 2ab 3"},
                                          {SpaceGroupId::SpaceGroup_214, 214, "", "I 41 3 2", "I 4bd 2c 3"},
                                          {SpaceGroupId::SpaceGroup_215, 215, "", "P -4 3 m", "P -4 2 3"},
                                          {SpaceGroupId::SpaceGroup_216, 216, "", "F -4 3 m", "F -4 2 3"},
                                          {SpaceGroupId::SpaceGroup_217, 217, "", "I -4 3 m", "I -4 2 3"},
                                          {SpaceGroupId::SpaceGroup_218, 218, "", "P -4 3 n", "P -4n 2 3"},
                                          {SpaceGroupId::SpaceGroup_219, 219, "", "F -4 3 c", "F -4c 2 3"},
                                          {SpaceGroupId::SpaceGroup_220, 220, "", "I -4 3 d", "I -4bd 2c 3"},
                                          {SpaceGroupId::SpaceGroup_221, 221, "", "P m -3 m", "-P 4 2 3"},
                                          {SpaceGroupId::SpaceGroup_222_1, 222, "1", "P n -3 n:1", "P 4 2 3 -1n"},
                                          {SpaceGroupId::SpaceGroup_222_2, 222, "2", "P n -3 n:2", "-P 4a 2bc 3"},
                                          {SpaceGroupId::SpaceGroup_223, 223, "", "P m -3 n", "-P 4n 2 3"},
                                          {SpaceGroupId::SpaceGroup_224_1, 224, "1", "P n -3 m:1", "P 4n 2 3 -1n"},
                                          {SpaceGroupId::SpaceGroup_224_2, 224, "2", "P n -3 m:2", "-P 4bc 2bc 3"},
                                          {SpaceGroupId::SpaceGroup_225, 225, "", "F m -3 m", "-F 4 2 3"},
                                          {SpaceGroupId::SpaceGroup_226, 226, "", "F m -3 c", "-F 4c 2 3"},
                                          {SpaceGroupId::SpaceGroup_227_1, 227, "1", "F d -3 m:1", "F 4d 2 3 -1d"},
                                          {SpaceGroupId::SpaceGroup_227_2, 227, "2", "F d -3 m:2", "-F 4vw 2vw 3"},
                                          {SpaceGroupId::SpaceGroup_228_1, 228, "1", "F d -3 c:1", "F 4d 2 3 -1cd"},
                                          {SpaceGroupId::SpaceGroup_228_2, 228, "2", "F d -3 c:2", "-F 4cvw 2vw 3"},
                                          {SpaceGroupId::SpaceGroup_229, 229, "", "I m -3 m", "-I 4 2 3"},
                                          {SpaceGroupId::SpaceGroup_230, 230, "", "I a -3 d", "-I 4bd 2c 3"}};

SpaceGroupSymbol::SpaceGroupSymbol(SpaceGroupId id, int sgNumber, std::string_view sgCode, std::string_view hmSymbol,
                                   std::string_view hallSymbol)
    : id_(id), internationalTableIndex_(sgNumber), code_(sgCode), hermannMauginnSymbol_(hmSymbol), hallSymbol_(hallSymbol)
{
    condensedHermannMauginnSymbol_ = hmSymbol;
    condensedHermannMauginnSymbol_.erase(std::remove_if(condensedHermannMauginnSymbol_.begin(),
                                                        condensedHermannMauginnSymbol_.end(),
                                                        [](const auto c) { return c == ' '; }),
                                         condensedHermannMauginnSymbol_.end());
}

// Return space Group ID
SpaceGroupId SpaceGroupSymbol::id() const { return id_; }

// Return space group index in International Tables (1 - 230)
int SpaceGroupSymbol::internationalTableIndex() const { return internationalTableIndex_; }

// Return space group code (if any)
const std::string &SpaceGroupSymbol::code() const { return code_; }

// Return Hermann-Mauginn symbol
const std::string &SpaceGroupSymbol::hermannMauginnSymbol() const { return hermannMauginnSymbol_; }

// Return condensed Hermann-Mauginn symbol (no spaces)
const std::string &SpaceGroupSymbol::condensedHermannMauginnSymbol() const { return condensedHermannMauginnSymbol_; }

// Return Hall symbol
const std::string &SpaceGroupSymbol::hallSymbol() const { return hallSymbol_; }

// Find space group from supplied Hall symbol
SpaceGroupId findByHallSymbol(std::string_view hallSymbol)
{
    auto it =
        std::find_if(symbols_.begin(), symbols_.end(), [hallSymbol](const auto &sg) { return sg.hallSymbol() == hallSymbol; });
    return it == symbols_.end() ? SpaceGroupId::NoSpaceGroup : it->id();
}

// Find space group from supplied Hermann-Mauginn symbol and (optional) code
SpaceGroups::SpaceGroupId findByHermannMauginnSymbol(std::string_view hmSymbol, std::string_view code)
{
    // Account for the fact that the HM symbol might not have spaces, and/or may contain brackets (e.g. Fm(-3)m) which ours
    // don't.
    std::string cleanedHMSymbol{hmSymbol};
    cleanedHMSymbol.erase(std::remove_if(cleanedHMSymbol.begin(), cleanedHMSymbol.end(),
                                         [](const auto c) { return c == ' ' || c == '(' || c == ')'; }),
                          cleanedHMSymbol.end());

    // If the provided HM symbol contains a colon already, or the supplied code is empty, do a plain search.
    auto it = hmSymbol.find(':') != std::string::npos
                  ? std::find_if(symbols_.begin(), symbols_.end(),
                                 [cleanedHMSymbol](const auto &sg) {
                                     return sg.hermannMauginnSymbol() == cleanedHMSymbol ||
                                            sg.condensedHermannMauginnSymbol() == cleanedHMSymbol;
                                 })
                  : std::find_if(symbols_.begin(), symbols_.end(),
                                 [cleanedHMSymbol, code](const auto &sg)
                                 {
                                     if (sg.hermannMauginnSymbol().find(':') == std::string::npos)
                                         return (sg.hermannMauginnSymbol() == cleanedHMSymbol ||
                                                 sg.condensedHermannMauginnSymbol() == cleanedHMSymbol) &&
                                                (code.empty() || sg.code() == code);
                                     else
                                         return (DissolveSys::beforeChar(sg.hermannMauginnSymbol(), ':') == cleanedHMSymbol ||
                                                 DissolveSys::beforeChar(sg.condensedHermannMauginnSymbol(), ':') ==
                                                     cleanedHMSymbol) &&
                                                (code.empty() || sg.code() == code);
                                 });

    return it == symbols_.end() ? SpaceGroupId::NoSpaceGroup : it->id();
}

// Find space group from supplied International Tables index and (optional) code
SpaceGroups::SpaceGroupId findByInternationalTablesIndex(int itIndex, std::string_view code)
{
    auto it = std::find_if(symbols_.begin(), symbols_.end(),
                           [itIndex, code](const auto &sg)
                           { return sg.internationalTableIndex() == itIndex && (code.empty() || sg.code() == code); });
    return it == symbols_.end() ? SpaceGroupId::NoSpaceGroup : it->id();
}

// Return index of the supplied space group id in the International Tables
int internationalTableIndex(SpaceGroupId sgid) { return symbols_[sgid].internationalTableIndex(); }

// Return full set of symmetry operators for the supplied space group id
std::vector<Matrix4> symmetryOperators(SpaceGroupId sgid)
{
    // Initialise an SGinfo object
    T_SgInfo sgInfo;
    sgInfo.MaxList = 192;
    sgInfo.ListSeitzMx = new T_RTMx[192];
    sgInfo.ListRotMxInfo = new T_RotMxInfo[192];
    InitSgInfo(&sgInfo);

    // Translate the Hall symbol and generate the whole group
    ParseHallSymbol(symbols_[sgid].hallSymbol().c_str(), &sgInfo);
    if (SgError)
    {
        Messenger::error("Error parsing Hall symbol (SGInfo error is '{}').\n", SgError);
        return {};
    }

    CompleteSgInfo(&sgInfo);
    if (SgError)
    {
        Messenger::error("Error completing space group information (SGInfo error is '{}').\n", SgError);
        return {};
    }

    bool addInverse = sgInfo.Centric == -1;
    const auto realSTBF = double(STBF);

    std::vector<Matrix4> operators;

    // Loop over translation vectors associated to lattice
    auto nTrV = sgInfo.LatticeInfo->nTrVector;
    auto TrV = sgInfo.LatticeInfo->TrVector;
    for (auto iTrV = 0; iTrV < nTrV; iTrV++, TrV += 3)
    {
        // Loop over Seitz matrices
        const auto *seitzMatrices = sgInfo.ListSeitzMx;
        for (auto seitzIndex = 0; seitzIndex < sgInfo.nList; ++seitzIndex, ++seitzMatrices)
        {
            Matrix4 op;

            // Set rotation matrix
            op.setRow(0, seitzMatrices->s.R[0], seitzMatrices->s.R[1], seitzMatrices->s.R[2]);
            op.setRow(1, seitzMatrices->s.R[3], seitzMatrices->s.R[4], seitzMatrices->s.R[5]);
            op.setRow(2, seitzMatrices->s.R[6], seitzMatrices->s.R[7], seitzMatrices->s.R[8]);

            // Set translation vector
            op.setTranslation((seitzMatrices->s.T[0] + TrV[0]) / realSTBF, (seitzMatrices->s.T[1] + TrV[1]) / realSTBF,
                              (seitzMatrices->s.T[2] + TrV[2]) / realSTBF);

            operators.push_back(op);

            // Add inverse if required
            if (addInverse)
                operators.push_back(op * -1.0);
        }
    }

    delete[] sgInfo.ListSeitzMx;
    delete[] sgInfo.ListRotMxInfo;

    return operators;
}

// Return formatted name for the supplied space group id
std::string formattedInformation(SpaceGroupId sgid)
{
    if (sgid == SpaceGroups::nSpaceGroupIds)
        return {};

    if (symbols_[sgid].code().empty())
        return fmt::format("{}  {}  ({})", symbols_[sgid].internationalTableIndex(), symbols_[sgid].hermannMauginnSymbol(),
                           symbols_[sgid].hallSymbol());
    else
        return fmt::format("{}  {} [{}]  ({})", symbols_[sgid].internationalTableIndex(), symbols_[sgid].hermannMauginnSymbol(),
                           symbols_[sgid].code(), symbols_[sgid].hallSymbol());
}

}; // namespace SpaceGroups
