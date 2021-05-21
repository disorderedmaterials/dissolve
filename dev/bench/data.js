window.BENCHMARK_DATA = {
  "lastUpdate": 1621617624901,
  "repoUrl": "https://github.com/disorderedmaterials/dissolve",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "857ed1117a0bbccf3bec7335a5c3788c1487b50f",
          "message": "Merge pull request #658 from disorderedmaterials/bugfix-26-04-21-hexane\n\nBugfix 26-04-21 Hexane Benchmarks",
          "timestamp": "2021-04-26T11:56:49+01:00",
          "tree_id": "576c5a6efadbd20f6b2dc480f7f0c6b69980ff38",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/857ed1117a0bbccf3bec7335a5c3788c1487b50f"
        },
        "date": 1619435806651,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 19.28569960000459,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 19.2856136 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 27.522040400026526,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 27.5186378 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 425.93785019998904,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 425.83516599999996 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 648.377309999978,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 648.3444008000001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1694.1250154000045,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1694.0535080000002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 2450.489466399995,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2450.1410276 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 9.199777045710034,
            "unit": "ns/iter",
            "extra": "iterations: 75628058\ncpu: 9.199070006531139 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.830047089055835,
            "unit": "ns/iter",
            "extra": "iterations: 78148735\ncpu: 8.829897003451173 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 18.72411875627642,
            "unit": "ns/iter",
            "extra": "iterations: 39040193\ncpu: 18.722826856926627 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 17.848223309367658,
            "unit": "ns/iter",
            "extra": "iterations: 37359149\ncpu: 17.84739641687249 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 12.997130419627963,
            "unit": "ns/iter",
            "extra": "iterations: 53548596\ncpu: 12.996389709265163 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 12.974859408016693,
            "unit": "ns/iter",
            "extra": "iterations: 54261451\ncpu: 12.97422433469387 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.107007592251097,
            "unit": "ns/iter",
            "extra": "iterations: 53286976\ncpu: 13.106707256197087 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.231779124937868,
            "unit": "ns/iter",
            "extra": "iterations: 52869744\ncpu: 13.231463783898802 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 12.997192583790696,
            "unit": "ns/iter",
            "extra": "iterations: 53553869\ncpu: 12.997203731442728 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.292925544182623,
            "unit": "ns/iter",
            "extra": "iterations: 52386517\ncpu: 13.29261132210792 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.475743957269207,
            "unit": "ns/iter",
            "extra": "iterations: 52254278\ncpu: 13.474502566086654 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.51807116813417,
            "unit": "ns/iter",
            "extra": "iterations: 51845929\ncpu: 13.517737969359278 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 13.596732374020515,
            "unit": "ns/iter",
            "extra": "iterations: 51603458\ncpu: 13.596046044046146 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 13.883323872074007,
            "unit": "ns/iter",
            "extra": "iterations: 50033851\ncpu: 13.882981803659346 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 15.480198440510765,
            "unit": "ns/iter",
            "extra": "iterations: 45409075\ncpu: 15.479746284195398 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 16.902103459845602,
            "unit": "ns/iter",
            "extra": "iterations: 41446572\ncpu: 16.90177385478345 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 16.971834852852826,
            "unit": "ns/iter",
            "extra": "iterations: 40921107\ncpu: 16.971048412742103 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 17.40545241978138,
            "unit": "ns/iter",
            "extra": "iterations: 39451988\ncpu: 17.405454853124258 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 19.124724334009244,
            "unit": "ns/iter",
            "extra": "iterations: 36368741\ncpu: 19.12450527226112 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 26.018785632837723,
            "unit": "ns/iter",
            "extra": "iterations: 26888474\ncpu: 26.018597039013773 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 25.970036328913686,
            "unit": "ns/iter",
            "extra": "iterations: 27053661\ncpu: 25.969341487645437 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 25.935640612514593,
            "unit": "ns/iter",
            "extra": "iterations: 26853534\ncpu: 25.93566146638286 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 25.94409738802543,
            "unit": "ns/iter",
            "extra": "iterations: 26986807\ncpu: 25.943774637733156 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 26.24551635423347,
            "unit": "ns/iter",
            "extra": "iterations: 26722004\ncpu: 26.245322281966697 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 26.201559739912984,
            "unit": "ns/iter",
            "extra": "iterations: 26692399\ncpu: 26.201100395659424 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 26.35545225059861,
            "unit": "ns/iter",
            "extra": "iterations: 26572458\ncpu: 26.355474002442666 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 26.304348597902464,
            "unit": "ns/iter",
            "extra": "iterations: 26532529\ncpu: 26.303695701227632 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 26.106937620683407,
            "unit": "ns/iter",
            "extra": "iterations: 26735016\ncpu: 26.106721836261343 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 26.403183550226146,
            "unit": "ns/iter",
            "extra": "iterations: 26517565\ncpu: 26.402975801134275 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 27.451831586753777,
            "unit": "ns/iter",
            "extra": "iterations: 25720922\ncpu: 27.451834152757048 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 28.923052130970255,
            "unit": "ns/iter",
            "extra": "iterations: 24232276\ncpu: 28.922843483624746 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 30.118428790504712,
            "unit": "ns/iter",
            "extra": "iterations: 23206097\ncpu: 30.118421464841873 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 30.26228410063532,
            "unit": "ns/iter",
            "extra": "iterations: 22680822\ncpu: 30.25966210572071 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 36.85910637708931,
            "unit": "ns/iter",
            "extra": "iterations: 20127774\ncpu: 36.85732426248428 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 39.09449099361635,
            "unit": "ns/iter",
            "extra": "iterations: 17934323\ncpu: 39.093988660737125 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 38.99880285853067,
            "unit": "ns/iter",
            "extra": "iterations: 17985343\ncpu: 38.9982416237485 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 38.853818872151486,
            "unit": "ns/iter",
            "extra": "iterations: 18004465\ncpu: 38.85241660887977 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 38.88116941553056,
            "unit": "ns/iter",
            "extra": "iterations: 17943596\ncpu: 38.87964168386285 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 39.01722017741554,
            "unit": "ns/iter",
            "extra": "iterations: 17926180\ncpu: 39.01586740733314 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 38.97681818118647,
            "unit": "ns/iter",
            "extra": "iterations: 17982325\ncpu: 38.97434464119652 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 39.15147183572345,
            "unit": "ns/iter",
            "extra": "iterations: 17992599\ncpu: 39.150164131374524 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 39.280681246351385,
            "unit": "ns/iter",
            "extra": "iterations: 17885160\ncpu: 39.27977239230751 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 39.13495384264283,
            "unit": "ns/iter",
            "extra": "iterations: 17844390\ncpu: 39.134327091035445 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 39.269986628600826,
            "unit": "ns/iter",
            "extra": "iterations: 17813397\ncpu: 39.26980322731278 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 39.59086468387737,
            "unit": "ns/iter",
            "extra": "iterations: 17558648\ncpu: 39.587930118538054 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 40.425795054329136,
            "unit": "ns/iter",
            "extra": "iterations: 17275769\ncpu: 40.424111482388724 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 41.609431911622636,
            "unit": "ns/iter",
            "extra": "iterations: 16813898\ncpu: 41.606692392210356 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 43.396580599279986,
            "unit": "ns/iter",
            "extra": "iterations: 16038132\ncpu: 43.395819787491305 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 49.44330868491107,
            "unit": "ns/iter",
            "extra": "iterations: 12127078\ncpu: 49.44056507264139 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 74.7793596138803,
            "unit": "ns/iter",
            "extra": "iterations: 9339709\ncpu: 74.77600254997171 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 274.1952065389574,
            "unit": "ns/iter",
            "extra": "iterations: 2559904\ncpu: 274.1820771403928 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1062.8486829869069,
            "unit": "ns/iter",
            "extra": "iterations: 661307\ncpu: 1062.82498899906 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4407.584094343859,
            "unit": "ns/iter",
            "extra": "iterations: 160094\ncpu: 4407.318275513152 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 23896.94308525205,
            "unit": "ns/iter",
            "extra": "iterations: 29184\ncpu: 23895.407929001878 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 131391.69714070365,
            "unit": "ns/iter",
            "extra": "iterations: 5316\ncpu: 131384.22930775216 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1020634.9399707493,
            "unit": "ns/iter",
            "extra": "iterations: 683\ncpu: 1020580.956076137 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 5886483.8899078,
            "unit": "ns/iter",
            "extra": "iterations: 109\ncpu: 5886037.055045828 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 398.4968214300339,
            "unit": "ns/iter",
            "extra": "iterations: 1759439\ncpu: 398.4969464698684 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1561.013735129738,
            "unit": "ns/iter",
            "extra": "iterations: 448121\ncpu: 1560.9060878646787 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 6507.876047214836,
            "unit": "ns/iter",
            "extra": "iterations: 107428\ncpu: 6507.524174330769 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 25942.237790678362,
            "unit": "ns/iter",
            "extra": "iterations: 27049\ncpu: 25941.0039557836 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 104930.92932487598,
            "unit": "ns/iter",
            "extra": "iterations: 6636\ncpu: 104928.1764617239 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 419897.79688436765,
            "unit": "ns/iter",
            "extra": "iterations: 1669\ncpu: 419867.95266626874 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2226221.961782763,
            "unit": "ns/iter",
            "extra": "iterations: 314\ncpu: 2226083.8821655763 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 9011724.666667456,
            "unit": "ns/iter",
            "extra": "iterations: 75\ncpu: 9011658.120000068 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 886.8224002853257,
            "unit": "ns/iter",
            "extra": "iterations: 790981\ncpu: 886.8135454580968 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3614.5042117186927,
            "unit": "ns/iter",
            "extra": "iterations: 194220\ncpu: 3614.5055401091777 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 12177.885947757624,
            "unit": "ns/iter",
            "extra": "iterations: 57272\ncpu: 12177.14378754028 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 56428.88474249139,
            "unit": "ns/iter",
            "extra": "iterations: 12407\ncpu: 56423.28540340148 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 233216.4186737008,
            "unit": "ns/iter",
            "extra": "iterations: 3031\ncpu: 233208.96271857363 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 896922.793548466,
            "unit": "ns/iter",
            "extra": "iterations: 775\ncpu: 896886.3961290333 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4235531.113772322,
            "unit": "ns/iter",
            "extra": "iterations: 167\ncpu: 4235410.664670622 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 17465956.820510305,
            "unit": "ns/iter",
            "extra": "iterations: 39\ncpu: 17465959.923076943 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 3987.915937295456,
            "unit": "ns/iter",
            "extra": "iterations: 173894\ncpu: 3987.667147802709 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 19.811316352355156,
            "unit": "ns/iter",
            "extra": "iterations: 35079802\ncpu: 19.81077108131882 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 75.84826034787173,
            "unit": "ns/iter",
            "extra": "iterations: 9291599\ncpu: 75.84390523095053 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.6152100895653111,
            "unit": "ms/iter",
            "extra": "iterations: 1150\ncpu: 0.6151926365217387 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3915378809787812,
            "unit": "ms/iter",
            "extra": "iterations: 1798\ncpu: 0.3915004566184675 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 378.60367520002,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 378.590770400001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 382.1498471999803,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 382.1346214000016 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 1571.5643155169832,
            "unit": "ns/iter",
            "extra": "iterations: 429492\ncpu: 1571.5470206662812 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 19.913433396147774,
            "unit": "ns/iter",
            "extra": "iterations: 35107615\ncpu: 19.912265358953942 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 75.53430299362661,
            "unit": "ns/iter",
            "extra": "iterations: 9343616\ncpu: 75.50702361912258 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 174.29608505145237,
            "unit": "ns/iter",
            "extra": "iterations: 4071982\ncpu: 174.28423234680244 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 4.041151679775488,
            "unit": "ms/iter",
            "extra": "iterations: 178\ncpu: 4.040876516853906 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 24.665266857141432,
            "unit": "ms/iter",
            "extra": "iterations: 28\ncpu: 24.6624741785705 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2962.480787000004,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2962.3599452000008 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2991.877467200038,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2991.7489983999985 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "adam.washington@stfc.ac.uk",
            "name": "Adam Washington",
            "username": "rprospero"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "1be326dc6e5547d767a5bc5f067e098913623eb8",
          "message": "Data Manager Simulation Table Testing (#656)\n\n* Add pugixml to conan\r\n\r\n* Integrate the xml stub in with the rest of the forcefields\r\n\r\n* Link against pugixml as header only\r\n\r\n* Make an xml forcefield system test\r\n\r\n* Start reading XML bond terms\r\n\r\n* Load in most of the force data\r\n\r\n* More xml changes\r\n\r\n* Current status of the xml gui\r\n\r\n* Update forcefield linking in xml\r\n\r\n* Remove actual XML forcefield code for now\r\n\r\n* Start on a decent GUI for the XML forcefield\r\n\r\n* Start filling in slots\r\n\r\n* Use TableView instead of TableWidget\r\n\r\n* Prevent wizard from trying to go past bounds\r\n\r\n* Start with xml model\r\n\r\n* Start filling in bond table\r\n\r\n* Make XmlFFModel a QT Object\r\n\r\n* Move XmlFFModel implementation into a separate file\r\n\r\n* Get the bonds from the XML file\r\n\r\n* Get the bonds from the XML file\r\n\r\n* More accurate name for xmlBondModel\r\n\r\n* Fix xml bond loading\r\n\r\n* Start reading xml angles\r\n\r\n* Start adding atoms to xml import\r\n\r\n* Move to reset model\r\n\r\n* Proper slot for loading xml file\r\n\r\n* Get proper AtomType from atom table\r\n\r\n* Enable assigning atom types on XML Load\r\n\r\n* Move Qt models into their own folder\r\n\r\n* Unbreak the build in cmake\r\n\r\n* First XML unit test builds and passes\r\n\r\n* Better XmlBondModel Test\r\n\r\n* Add test for xml angle loading\r\n\r\n* Add test of XmlAtomModel\r\n\r\n* Highlight xml atom types that have not been linked\r\n\r\n* Separate out GUI unit tests\r\n\r\nThese tests should only build if we're building the GUI.\r\n\r\n* Fix xml table headers\r\n\r\n* Guess best match for Xml Atom Types\r\n\r\n* Stop responding for roles we don't understand\r\n\r\n* More code comments\r\n\r\n* Add Torsion to XML model\r\n\r\n* Add Improper torsions to Xml loader\r\n\r\n* Force a valid xml file in the wizard\r\n\r\n* Stop hard coding data paths in xml tests\r\n\r\n* Refactor GUI\r\n\r\n* Start writing a *real* test for the XML forcefield\r\n\r\n* Start making the actual Forcefield\r\n\r\n* Add xml torsion to real XML ff\r\n\r\n* Add impropers\r\n\r\n* Register new forcefield in unit test\r\n\r\n* Use correct names when adding terms to XML forcefield\r\n\r\n* Allow the wizard to finish\r\n\r\n* Fix improper after merge\r\n\r\n* Fix forcefield models for XML\r\n\r\n* Fix formatting\r\n\r\n* Fix formatting of unit test\r\n\r\n* Fix cmake formatting\r\n\r\n* Fix improper formatting\r\n\r\n* Tidy up xmlAtomModel\r\n\r\n* More improper formatting\r\n\r\n* fix typo in xmlAtomModel\r\n\r\n* Fix whitespace\r\n\r\n* Try fixing gui build on Azure\r\n\r\n* Fix linking to MOC headers in models\r\n\r\n* Fix Qt linking for GUI unit tests\r\n\r\n* GUI tests on Azure need access to Qt\r\n\r\n* Load the Qt ppa in Azure\r\n\r\n* Remove vestigial command line xml code\r\n\r\n* Move models into GUI directory\r\n\r\n* Reorder import headers for formatting\r\n\r\n* Explicit names in forcefield headers\r\n\r\n* Fix header comment on xml forcefield\r\n\r\n* Switch to passing references to the XML forcefield\r\n\r\n* Update src/gui/importforcefieldwizard_funcs.cpp\r\n\r\nCo-authored-by: Tristan Youngs <tristan.youngs@stfc.ac.uk>\r\n\r\n* At least display a tree in the GUI\r\n\r\n* Kind of have the tree model doing the right thing\r\n\r\n* Actually display bond info in the tree.\r\n\r\n* Add atoms and Angles to tree\r\n\r\n* All the data is visible on the tree\r\n\r\n* Clean up indices\r\n\r\n* Don't show non-existent children in tree model\r\n\r\n* Do not require pre-existing atom types\r\n\r\n* Add unit test for XML tree\r\n\r\n* Full test of tree model\r\n\r\n* Don't display tree header\r\n\r\n* Blank headers\r\n\r\n* Actually load the forcefield after the wizard\r\n\r\n* Give the dialog a name\r\n\r\n* Give the XML Forcefield a name\r\n\r\n* Remove unimplemented clear function\r\n\r\n* Clear the Tree when the model is removed\r\n\r\n* Allow clearing xml model\r\n\r\n* Reset import forcefield wizard on restart\r\n\r\n* Add tooltips to import wizard\r\n\r\n* Fix formatting\r\n\r\n* Pass model roles onto cilhdren of XmlTreeModel\r\n\r\n* Start DataManagerModel\r\n\r\n* Fix DataManagerModel Headers\r\n\r\n* Register Model Updates with the DataManager\r\n\r\n* Actually use the new model in the application\r\n\r\n* Move ReferencePoint from List to std::vector\r\n\r\n* Specify which data manager model\r\n\r\n* Set default force field name based on xml file\r\n\r\n* Change Xml tree headers\r\n\r\n* Better tests on the Xml Forcefield\r\n\r\n* Expand trees before resizing columns\r\n\r\n* Fix units on bonds\r\n\r\n* Only test for Double equality on xml import\r\n\r\nSince we're doing unit conversions now, there may be slight changes at\r\nthe low bits\r\n\r\n* Include short range potentials\r\n\r\n* Fix cmake formatting\r\n\r\n* Add cassert where needed\r\n\r\n* Fix gui test linking\r\n\r\n* Compile and link against pugixml\r\n\r\nLinking against pugixml is somehow simpler than using the library header-only\r\n\r\n* Fix cmake formatting\r\n\r\n* Fix the cmake formatting again\r\n\r\n* Stupid tab characters\r\n\r\n* Try and point more directly at the pugixml libraries\r\n\r\n* Publicly link to pugixml\r\n\r\n* Link components to pugixml\r\n\r\n* cmake whitespace\r\n\r\n* Add gcc-8 to linux build\r\n\r\nWe need this for the <filesystem> library\r\n\r\n* Remove GCC-8 dependency\r\n\r\nIt's a sink hole of time.\r\n\r\n* Propagate filename changes\r\n\r\n* Display imported angles in degrees\r\n\r\n* Offer a default header for imported forcefields\r\n\r\n* Add icons to import wizard buttons\r\n\r\nHonestly, the code probably needs a refactor to use the WizardWidget.\r\nHowever, that might get subsumed into a refactor to QML, (which the\r\ntable models already have us setup for), so it's worth just leaving\r\nthis as a note for later.\r\n\r\n* Reset AddForcefieldTermsWizard every time\r\n\r\n* Fix up forcefield registration\r\n\r\n* Give all angles in degrees\r\n\r\nThe units have been standardized since the Babylonians.  It's worth\r\ngetting to know them.\r\n\r\n* Fix unit test to ensure that we check for angles in degrees\r\n\r\n* Move logic into Reference Point Model\r\n\r\n* Actually start testing logic on data manager reference points\r\n\r\n* Get the gui model tests building properly again\r\n\r\n* Better unit test for data manager reference points\r\n\r\n* Fix formatting\r\n\r\n* Remove extra if\r\n\r\n* Remove unneeded casts\r\n\r\n* Better conditions in model\r\n\r\n* Fix more formatting\r\n\r\n* Update src/classes/referencepoint.h\r\n\r\nCo-authored-by: Tristan Youngs <tristan.youngs@stfc.ac.uk>\r\n\r\n* Fix string_view bug\r\n\r\n* Eliminate ReferencePoint class\r\n\r\n* Fix formatting\r\n\r\n* Remove extra braces\r\n\r\n* First compile on new model\r\n\r\n* Add table filter\r\n\r\n* Fix Smiulation table headers\r\n\r\n* Add Simulation Table unit tests\r\n\r\n* Update unit tests for Data Manager Dialog\r\n\r\n* Remove commented out functions\r\n\r\n* Fix formatting\r\n\r\n* Update src/gui/models/dataManagerSimulationModel.h\r\n\r\nCo-authored-by: Tristan Youngs <tristan.youngs@stfc.ac.uk>\r\n\r\n* Handle empty filter\r\n\r\n* Add SPDX to model code\r\n\r\nCo-authored-by: Tristan Youngs <tristan.youngs@stfc.ac.uk>",
          "timestamp": "2021-04-27T09:05:35+01:00",
          "tree_id": "0ad8f6e6faaa656ba7a3cd7e15869fd82447b4fb",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/1be326dc6e5547d767a5bc5f067e098913623eb8"
        },
        "date": 1619511919377,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 17.772195199995622,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 17.5403188 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 26.61277320003137,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 26.597918200000002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 403.837974399994,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 403.7632564 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 625.3110598000148,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 625.2454906 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1625.7856974000333,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1625.6508576 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 2409.2757725999945,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2409.1344268000003 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.9631061247417,
            "unit": "ns/iter",
            "extra": "iterations: 78644517\ncpu: 8.962756780615742 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.71792203951845,
            "unit": "ns/iter",
            "extra": "iterations: 79548558\ncpu: 8.717634982145103 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 18.391568638944978,
            "unit": "ns/iter",
            "extra": "iterations: 40054316\ncpu: 18.390501962385255 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 17.424121362334827,
            "unit": "ns/iter",
            "extra": "iterations: 37939018\ncpu: 17.423166619652687 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 12.651746516290586,
            "unit": "ns/iter",
            "extra": "iterations: 54956573\ncpu: 12.650606416087857 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 12.632047724954209,
            "unit": "ns/iter",
            "extra": "iterations: 55263667\ncpu: 12.631416188867924 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 12.808863470954405,
            "unit": "ns/iter",
            "extra": "iterations: 55626515\ncpu: 12.808326964218423 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 12.947823993236565,
            "unit": "ns/iter",
            "extra": "iterations: 54024947\ncpu: 12.94740709324526 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 12.743581621932927,
            "unit": "ns/iter",
            "extra": "iterations: 56689205\ncpu: 12.742804207608918 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.095209399761696,
            "unit": "ns/iter",
            "extra": "iterations: 55245144\ncpu: 13.094328073432106 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 12.867040250184905,
            "unit": "ns/iter",
            "extra": "iterations: 55138228\ncpu: 12.866775968933847 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 12.76560408670188,
            "unit": "ns/iter",
            "extra": "iterations: 53453577\ncpu: 12.765235879350112 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 12.963666225987318,
            "unit": "ns/iter",
            "extra": "iterations: 53782522\ncpu: 12.963071869333367 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 13.605959374363517,
            "unit": "ns/iter",
            "extra": "iterations: 51616962\ncpu: 13.60543704606246 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 14.392558724124898,
            "unit": "ns/iter",
            "extra": "iterations: 47577083\ncpu: 14.392068887451577 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 16.56432573239977,
            "unit": "ns/iter",
            "extra": "iterations: 42387236\ncpu: 16.563718780813986 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 16.996672836390996,
            "unit": "ns/iter",
            "extra": "iterations: 40993175\ncpu: 16.995250672825367 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 17.32465392633569,
            "unit": "ns/iter",
            "extra": "iterations: 40609418\ncpu: 17.323462650954532 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 22.03177000848874,
            "unit": "ns/iter",
            "extra": "iterations: 31774181\ncpu: 22.030860811172477 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 25.024665127418203,
            "unit": "ns/iter",
            "extra": "iterations: 27330246\ncpu: 25.023996051846616 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 25.10614005167363,
            "unit": "ns/iter",
            "extra": "iterations: 28126734\ncpu: 25.105599213900874 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 25.606379575475525,
            "unit": "ns/iter",
            "extra": "iterations: 27742567\ncpu: 25.529931530849158 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 25.26595096972841,
            "unit": "ns/iter",
            "extra": "iterations: 27793608\ncpu: 25.26486759113821 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 25.236478741036763,
            "unit": "ns/iter",
            "extra": "iterations: 28618378\ncpu: 25.235582184287257 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 24.803530527311455,
            "unit": "ns/iter",
            "extra": "iterations: 27558093\ncpu: 24.802269409570698 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 25.05230407438929,
            "unit": "ns/iter",
            "extra": "iterations: 27251529\ncpu: 25.051520118375887 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 25.131253014429657,
            "unit": "ns/iter",
            "extra": "iterations: 27826599\ncpu: 25.130147237900037 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 25.52934928909337,
            "unit": "ns/iter",
            "extra": "iterations: 27659733\ncpu: 25.5282862636454 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 25.679554680869813,
            "unit": "ns/iter",
            "extra": "iterations: 27523273\ncpu: 25.67751695083661 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 27.168585456685705,
            "unit": "ns/iter",
            "extra": "iterations: 25417928\ncpu: 27.167025298049456 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 28.62754440737353,
            "unit": "ns/iter",
            "extra": "iterations: 24582689\ncpu: 28.625780320452197 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 29.95288623255953,
            "unit": "ns/iter",
            "extra": "iterations: 23397768\ncpu: 29.951775912984495 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 30.85565851866729,
            "unit": "ns/iter",
            "extra": "iterations: 22542702\ncpu: 30.854662098624914 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 47.32618320597751,
            "unit": "ns/iter",
            "extra": "iterations: 18722184\ncpu: 47.3224956554212 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 38.21784580855865,
            "unit": "ns/iter",
            "extra": "iterations: 18235733\ncpu: 38.21657785842775 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 38.731998249974374,
            "unit": "ns/iter",
            "extra": "iterations: 18228325\ncpu: 38.73106371539901 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 37.90257198880239,
            "unit": "ns/iter",
            "extra": "iterations: 18508712\ncpu: 37.901157519766386 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 38.22103375968565,
            "unit": "ns/iter",
            "extra": "iterations: 18384911\ncpu: 38.219693367022415 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 38.3502791916016,
            "unit": "ns/iter",
            "extra": "iterations: 18240878\ncpu: 38.34849775323321 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 37.91549065462186,
            "unit": "ns/iter",
            "extra": "iterations: 18327831\ncpu: 37.914596604475705 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 37.682199098483764,
            "unit": "ns/iter",
            "extra": "iterations: 18144508\ncpu: 37.68109154571747 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 37.60917246764394,
            "unit": "ns/iter",
            "extra": "iterations: 18522889\ncpu: 37.6077763571332 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 36.9005788382747,
            "unit": "ns/iter",
            "extra": "iterations: 18167078\ncpu: 36.898744255955535 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 37.00116710099794,
            "unit": "ns/iter",
            "extra": "iterations: 18759302\ncpu: 36.99943062913501 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 38.49750021573646,
            "unit": "ns/iter",
            "extra": "iterations: 18135765\ncpu: 38.496390529982996 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 40.00159180023261,
            "unit": "ns/iter",
            "extra": "iterations: 17264101\ncpu: 39.99914863797491 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 40.877137707033555,
            "unit": "ns/iter",
            "extra": "iterations: 16952744\ncpu: 40.87548534915648 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 42.514111132609536,
            "unit": "ns/iter",
            "extra": "iterations: 16880679\ncpu: 42.51003463782417 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 61.24661665188675,
            "unit": "ns/iter",
            "extra": "iterations: 10884115\ncpu: 61.2428703665852 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 70.9663313246871,
            "unit": "ns/iter",
            "extra": "iterations: 9991691\ncpu: 70.96409816916933 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 257.85658884595904,
            "unit": "ns/iter",
            "extra": "iterations: 2688445\ncpu: 257.8500367312697 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1035.8551246129273,
            "unit": "ns/iter",
            "extra": "iterations: 712005\ncpu: 1035.8001980323218 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4196.318873124241,
            "unit": "ns/iter",
            "extra": "iterations: 164774\ncpu: 4196.13259980339 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 22570.13500601406,
            "unit": "ns/iter",
            "extra": "iterations: 30769\ncpu: 22569.43690077685 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 138979.78710194567,
            "unit": "ns/iter",
            "extra": "iterations: 4993\ncpu: 138977.27879030674 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1054808.7954544607,
            "unit": "ns/iter",
            "extra": "iterations: 660\ncpu: 1054773.26363638 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 6009089.401786127,
            "unit": "ns/iter",
            "extra": "iterations: 112\ncpu: 6008875.892857114 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 378.5775315692318,
            "unit": "ns/iter",
            "extra": "iterations: 1825824\ncpu: 378.5565711700572 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1513.3613197433806,
            "unit": "ns/iter",
            "extra": "iterations: 464666\ncpu: 1513.322603762707 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 6277.3719142196205,
            "unit": "ns/iter",
            "extra": "iterations: 111074\ncpu: 6277.240326268977 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 25196.260579469217,
            "unit": "ns/iter",
            "extra": "iterations: 27577\ncpu: 25195.833194328337 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 102685.22555620951,
            "unit": "ns/iter",
            "extra": "iterations: 6832\ncpu: 102681.24502341828 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 423034.1171779663,
            "unit": "ns/iter",
            "extra": "iterations: 1630\ncpu: 423010.85398772534 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2332123.7516772375,
            "unit": "ns/iter",
            "extra": "iterations: 298\ncpu: 2332029.8389261584 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 9758527.492957795,
            "unit": "ns/iter",
            "extra": "iterations: 71\ncpu: 9758113.661971765 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 849.4380573077757,
            "unit": "ns/iter",
            "extra": "iterations: 798706\ncpu: 849.415043833405 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3478.7389265346774,
            "unit": "ns/iter",
            "extra": "iterations: 199373\ncpu: 3478.5697060283946 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 11606.35249042073,
            "unit": "ns/iter",
            "extra": "iterations: 60813\ncpu: 11605.857168697485 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 53787.34604615282,
            "unit": "ns/iter",
            "extra": "iterations: 13215\ncpu: 53785.121074536175 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 227870.89892754005,
            "unit": "ns/iter",
            "extra": "iterations: 3077\ncpu: 227859.49821254294 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 885400.0300750629,
            "unit": "ns/iter",
            "extra": "iterations: 798\ncpu: 885362.2543859688 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4253410.779140449,
            "unit": "ns/iter",
            "extra": "iterations: 163\ncpu: 4253119.263803644 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 17798032.487176802,
            "unit": "ns/iter",
            "extra": "iterations: 39\ncpu: 17797286.666666858 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 3903.9959993570114,
            "unit": "ns/iter",
            "extra": "iterations: 174222\ncpu: 3903.770086441395 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 19.588714549563026,
            "unit": "ns/iter",
            "extra": "iterations: 35855618\ncpu: 19.58774438081092 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 74.23478099360686,
            "unit": "ns/iter",
            "extra": "iterations: 9179755\ncpu: 74.23133068366212 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.5978885116277193,
            "unit": "ms/iter",
            "extra": "iterations: 1161\ncpu: 0.5978649715762279 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3767678879826628,
            "unit": "ms/iter",
            "extra": "iterations: 1839\ncpu: 0.376747651440999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 375.0897148000149,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 374.64840620000075 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 373.7012971999775,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 373.67983700000025 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 1490.4231602303068,
            "unit": "ns/iter",
            "extra": "iterations: 456144\ncpu: 1490.3944456136903 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 19.106697052668228,
            "unit": "ns/iter",
            "extra": "iterations: 37155703\ncpu: 19.105326765046062 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 72.7552890288805,
            "unit": "ns/iter",
            "extra": "iterations: 9635559\ncpu: 72.75113784265055 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 167.71334770256576,
            "unit": "ns/iter",
            "extra": "iterations: 4143110\ncpu: 167.7014078795859 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.889490408839506,
            "unit": "ms/iter",
            "extra": "iterations: 181\ncpu: 3.8892308508287288 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 23.759045833329157,
            "unit": "ms/iter",
            "extra": "iterations: 30\ncpu: 23.757469566667073 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2897.7129433999835,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2897.535012199998 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2933.770315799984,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2933.598891600002 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "56431339+StephenSmith25@users.noreply.github.com",
            "name": "StephenSmith25",
            "username": "StephenSmith25"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "7eb152bfbd030916eb3bcfe7c3e566766f7e04af",
          "message": "Merge pull request #611 from StephenSmith25/parallel_investigations\n\nIncorporate TBB in calculateGRCells method",
          "timestamp": "2021-04-27T10:07:47+01:00",
          "tree_id": "6fe183f18efc6495f33f08687d5683673037ee13",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/7eb152bfbd030916eb3bcfe7c3e566766f7e04af"
        },
        "date": 1619515575826,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 16.418885399980354,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 16.4188078 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 23.000207399991268,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 22.9964886 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 383.063175999996,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 383.03572619999994 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 576.1741340000299,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 576.1406279999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1568.9015272000233,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1568.8275720000001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 2160.6929590000163,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2160.5508544000004 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.006617087792964,
            "unit": "ns/iter",
            "extra": "iterations: 86607586\ncpu: 8.00624335609585 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.080722486827268,
            "unit": "ns/iter",
            "extra": "iterations: 84349718\ncpu: 8.080435775730743 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 16.82550361463474,
            "unit": "ns/iter",
            "extra": "iterations: 45100705\ncpu: 16.825031515582758 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 16.13373759330852,
            "unit": "ns/iter",
            "extra": "iterations: 43751879\ncpu: 16.132432963621962 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 11.296460455328473,
            "unit": "ns/iter",
            "extra": "iterations: 61395298\ncpu: 11.296136456573592 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 11.507581039727494,
            "unit": "ns/iter",
            "extra": "iterations: 58620785\ncpu: 11.507151192192314 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 11.550334564998114,
            "unit": "ns/iter",
            "extra": "iterations: 60067252\ncpu: 11.549592197092693 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 11.326996694148624,
            "unit": "ns/iter",
            "extra": "iterations: 63334362\ncpu: 11.326830765264502 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 11.640994704472774,
            "unit": "ns/iter",
            "extra": "iterations: 60704060\ncpu: 11.640660410522889 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 12.192825548043556,
            "unit": "ns/iter",
            "extra": "iterations: 61421695\ncpu: 12.158131194523362 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 11.743981685882819,
            "unit": "ns/iter",
            "extra": "iterations: 58151418\ncpu: 11.743405999145205 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 12.003089059106943,
            "unit": "ns/iter",
            "extra": "iterations: 60333258\ncpu: 12.002627008805069 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 11.827483595088568,
            "unit": "ns/iter",
            "extra": "iterations: 57893787\ncpu: 11.826580527544404 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 11.677834161133847,
            "unit": "ns/iter",
            "extra": "iterations: 61722708\ncpu: 11.677634817966869 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 13.296691067942904,
            "unit": "ns/iter",
            "extra": "iterations: 53530806\ncpu: 13.29611013142596 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 15.891605197031538,
            "unit": "ns/iter",
            "extra": "iterations: 44132900\ncpu: 15.89072358716517 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 16.40517648491386,
            "unit": "ns/iter",
            "extra": "iterations: 43690845\ncpu: 16.40413782338151 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 17.073094966153857,
            "unit": "ns/iter",
            "extra": "iterations: 41480011\ncpu: 17.07158337060235 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 16.817189645186172,
            "unit": "ns/iter",
            "extra": "iterations: 41031609\ncpu: 16.81655810767755 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 23.1888682344043,
            "unit": "ns/iter",
            "extra": "iterations: 31445955\ncpu: 23.18827012886077 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 23.56254881032102,
            "unit": "ns/iter",
            "extra": "iterations: 29697871\ncpu: 23.561444387713863 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 26.153190300530248,
            "unit": "ns/iter",
            "extra": "iterations: 27611709\ncpu: 26.152343268574803 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 24.613305070786474,
            "unit": "ns/iter",
            "extra": "iterations: 27667707\ncpu: 24.61276270563347 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 24.35851522129492,
            "unit": "ns/iter",
            "extra": "iterations: 29690943\ncpu: 24.357646572559233 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 24.00251027094542,
            "unit": "ns/iter",
            "extra": "iterations: 29621902\ncpu: 24.00160928221289 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 24.7907803454382,
            "unit": "ns/iter",
            "extra": "iterations: 28477965\ncpu: 24.78920431990142 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 23.94898939353759,
            "unit": "ns/iter",
            "extra": "iterations: 30425790\ncpu: 23.948151354492214 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 24.07749988142492,
            "unit": "ns/iter",
            "extra": "iterations: 30086317\ncpu: 24.07131647253477 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 23.73639025263385,
            "unit": "ns/iter",
            "extra": "iterations: 28773587\ncpu: 23.735427147126305 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 23.73646720600463,
            "unit": "ns/iter",
            "extra": "iterations: 30065059\ncpu: 23.735073495115888 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 28.124426857301355,
            "unit": "ns/iter",
            "extra": "iterations: 24621268\ncpu: 28.122607576506887 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 28.55075285270745,
            "unit": "ns/iter",
            "extra": "iterations: 24911181\ncpu: 28.5492360639185 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 29.214754065505282,
            "unit": "ns/iter",
            "extra": "iterations: 24399729\ncpu: 29.21370159480072 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 31.008595441884314,
            "unit": "ns/iter",
            "extra": "iterations: 21558403\ncpu: 31.006740480730453 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 34.93239433352896,
            "unit": "ns/iter",
            "extra": "iterations: 19576007\ncpu: 34.930075883197105 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 32.991346549264094,
            "unit": "ns/iter",
            "extra": "iterations: 19607207\ncpu: 32.989665330712036 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 34.15754320237888,
            "unit": "ns/iter",
            "extra": "iterations: 20832127\ncpu: 34.15557182423086 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 34.45954441632755,
            "unit": "ns/iter",
            "extra": "iterations: 20127543\ncpu: 34.45854111453142 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 34.70597580141062,
            "unit": "ns/iter",
            "extra": "iterations: 19543956\ncpu: 34.70450869823943 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 37.259154634279945,
            "unit": "ns/iter",
            "extra": "iterations: 19279088\ncpu: 37.256531481157104 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 34.26883012545531,
            "unit": "ns/iter",
            "extra": "iterations: 19245756\ncpu: 34.26787422640087 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 34.41462929042407,
            "unit": "ns/iter",
            "extra": "iterations: 19593869\ncpu: 34.41372140438454 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 33.56028736708669,
            "unit": "ns/iter",
            "extra": "iterations: 20639664\ncpu: 33.55940053093918 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 34.23243569824082,
            "unit": "ns/iter",
            "extra": "iterations: 21703994\ncpu: 34.23131175764226 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 36.84143311566381,
            "unit": "ns/iter",
            "extra": "iterations: 19452889\ncpu: 36.83833419293108 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 37.336422798318004,
            "unit": "ns/iter",
            "extra": "iterations: 18416781\ncpu: 37.33437716395734 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 38.78420160756477,
            "unit": "ns/iter",
            "extra": "iterations: 18170469\ncpu: 38.781825114145754 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 41.34518931947033,
            "unit": "ns/iter",
            "extra": "iterations: 18089343\ncpu: 41.34199539474707 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 43.647446184030905,
            "unit": "ns/iter",
            "extra": "iterations: 12743107\ncpu: 43.643704867266315 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 64.37920486280757,
            "unit": "ns/iter",
            "extra": "iterations: 11153723\ncpu: 64.37599400666465 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 241.93080122935424,
            "unit": "ns/iter",
            "extra": "iterations: 2881135\ncpu: 241.91882435220748 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 921.745778694065,
            "unit": "ns/iter",
            "extra": "iterations: 750834\ncpu: 921.7116832748558 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 3936.066572832422,
            "unit": "ns/iter",
            "extra": "iterations: 179037\ncpu: 3935.9336226590526 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 21578.583416700585,
            "unit": "ns/iter",
            "extra": "iterations: 33986\ncpu: 21577.721120461316 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 92688.36302896412,
            "unit": "ns/iter",
            "extra": "iterations: 7184\ncpu: 92679.5644487755 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1045841.3868953886,
            "unit": "ns/iter",
            "extra": "iterations: 641\ncpu: 1045776.1575662943 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 5658198.278261689,
            "unit": "ns/iter",
            "extra": "iterations: 115\ncpu: 5657691.40000006 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 365.6488509099639,
            "unit": "ns/iter",
            "extra": "iterations: 1887711\ncpu: 365.6308984796886 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1376.2880168988868,
            "unit": "ns/iter",
            "extra": "iterations: 489506\ncpu: 1376.249384072912 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 6152.986153338177,
            "unit": "ns/iter",
            "extra": "iterations: 115118\ncpu: 6152.987873312545 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 24849.009748912373,
            "unit": "ns/iter",
            "extra": "iterations: 28516\ncpu: 24848.61684668293 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 99676.24527204312,
            "unit": "ns/iter",
            "extra": "iterations: 6874\ncpu: 99675.3460867045 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 403928.1476354146,
            "unit": "ns/iter",
            "extra": "iterations: 1734\ncpu: 403925.72491349356 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2139851.9432171066,
            "unit": "ns/iter",
            "extra": "iterations: 317\ncpu: 2139714.7570978063 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 8337871.243242802,
            "unit": "ns/iter",
            "extra": "iterations: 74\ncpu: 8337284.662162205 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 807.5961840282788,
            "unit": "ns/iter",
            "extra": "iterations: 856977\ncpu: 807.3480595161808 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3275.904883805135,
            "unit": "ns/iter",
            "extra": "iterations: 219931\ncpu: 3275.768922980373 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 11422.82937436588,
            "unit": "ns/iter",
            "extra": "iterations: 63232\ncpu: 11422.835890055578 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 50560.45753140215,
            "unit": "ns/iter",
            "extra": "iterations: 13457\ncpu: 50560.14527755026 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 222620.9001196312,
            "unit": "ns/iter",
            "extra": "iterations: 3344\ncpu: 222614.46979664793 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 794900.3356643643,
            "unit": "ns/iter",
            "extra": "iterations: 858\ncpu: 794865.0058275056 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 3966587.320225668,
            "unit": "ns/iter",
            "extra": "iterations: 178\ncpu: 3966590.7584270025 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 15660340.75555965,
            "unit": "ns/iter",
            "extra": "iterations: 45\ncpu: 15659429.555555625 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 3839.3833257398614,
            "unit": "ns/iter",
            "extra": "iterations: 188782\ncpu: 3839.1083842739476 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 17.90966387399107,
            "unit": "ns/iter",
            "extra": "iterations: 39051575\ncpu: 17.901328870858528 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 71.2977480542718,
            "unit": "ns/iter",
            "extra": "iterations: 9490282\ncpu: 71.29267918487567 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.5337799805353906,
            "unit": "ms/iter",
            "extra": "iterations: 1233\ncpu: 0.5337476090835328 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.34975813948608697,
            "unit": "ms/iter",
            "extra": "iterations: 1907\ncpu: 0.34974057629784616 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 330.5797576000259,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 330.57413639999993 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 336.18321339999966,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 336.1691122000025 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 1330.2787746879976,
            "unit": "ns/iter",
            "extra": "iterations: 523785\ncpu: 1330.2200101186681 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 17.83720729604298,
            "unit": "ns/iter",
            "extra": "iterations: 39016896\ncpu: 17.837030475207335 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 68.37236493507726,
            "unit": "ns/iter",
            "extra": "iterations: 10432760\ncpu: 68.36896142535717 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 164.3057362371391,
            "unit": "ns/iter",
            "extra": "iterations: 4219055\ncpu: 164.29480772352824 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.5758982941166946,
            "unit": "ms/iter",
            "extra": "iterations: 187\ncpu: 3.575677411764685 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 21.892688636364365,
            "unit": "ms/iter",
            "extra": "iterations: 33\ncpu: 21.891273787878507 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2612.054492800007,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2611.9469813999985 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2602.4748514000294,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2602.3597769999983 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "74f2f974bb9a308d241b1307444076c60860a786",
          "message": "Merge pull request #580 from disorderedmaterials/move-to-ubuntu-20.04\n\nMove to ubuntu-latest",
          "timestamp": "2021-04-28T13:34:18+01:00",
          "tree_id": "ca7b798bfa1f6573f4cbadee5d1b7bc1129b21ae",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/74f2f974bb9a308d241b1307444076c60860a786"
        },
        "date": 1619614653178,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 16.95976419996441,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 16.9574632 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 28.009691999977804,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 27.989492400000007 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 405.18054880003547,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 405.1273648 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 665.8775576000153,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 665.8251502 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1604.853308599968,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1604.7137128 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 2510.422053399998,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2510.1768153999997 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.996026731247078,
            "unit": "ns/iter",
            "extra": "iterations: 81371289\ncpu: 8.995275864439106 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.656749948898963,
            "unit": "ns/iter",
            "extra": "iterations: 77097317\ncpu: 8.656550434822522 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 20.12674068652858,
            "unit": "ns/iter",
            "extra": "iterations: 39530021\ncpu: 20.125036083335235 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 17.885670237580864,
            "unit": "ns/iter",
            "extra": "iterations: 37526484\ncpu: 17.884441638603658 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 13.52130432637462,
            "unit": "ns/iter",
            "extra": "iterations: 51728648\ncpu: 13.521053362925587 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 13.735296037445597,
            "unit": "ns/iter",
            "extra": "iterations: 51690488\ncpu: 13.734611288637787 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.436353119343504,
            "unit": "ns/iter",
            "extra": "iterations: 52444864\ncpu: 13.436403095639696 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.436094878534394,
            "unit": "ns/iter",
            "extra": "iterations: 51791608\ncpu: 13.43485917641325 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.428387663325875,
            "unit": "ns/iter",
            "extra": "iterations: 53086363\ncpu: 13.427901870015054 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.493429679247381,
            "unit": "ns/iter",
            "extra": "iterations: 48954992\ncpu: 13.491411764503985 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.60250573477663,
            "unit": "ns/iter",
            "extra": "iterations: 51660695\ncpu: 13.602161198179742 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.688087104160934,
            "unit": "ns/iter",
            "extra": "iterations: 44741741\ncpu: 13.68715374754859 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 14.969661062035298,
            "unit": "ns/iter",
            "extra": "iterations: 48987608\ncpu: 14.967589334021024 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 16.660781737826834,
            "unit": "ns/iter",
            "extra": "iterations: 41707896\ncpu: 16.66081307481922 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 17.45033426904383,
            "unit": "ns/iter",
            "extra": "iterations: 39216165\ncpu: 17.448319742636713 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 18.11997922689672,
            "unit": "ns/iter",
            "extra": "iterations: 36924667\ncpu: 18.118530168464357 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 17.903631208560014,
            "unit": "ns/iter",
            "extra": "iterations: 38209455\ncpu: 17.9016112896664 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 20.712351983393948,
            "unit": "ns/iter",
            "extra": "iterations: 31867117\ncpu: 20.711934625275262 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 34.73894245264882,
            "unit": "ns/iter",
            "extra": "iterations: 19621892\ncpu: 34.734892639303034 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 25.818144771576392,
            "unit": "ns/iter",
            "extra": "iterations: 27208973\ncpu: 25.81694413824449 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 26.067692978912238,
            "unit": "ns/iter",
            "extra": "iterations: 26702400\ncpu: 26.064615427826713 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 26.617871068821806,
            "unit": "ns/iter",
            "extra": "iterations: 26037178\ncpu: 26.61632097764202 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 25.914394213477845,
            "unit": "ns/iter",
            "extra": "iterations: 27137266\ncpu: 25.91142519662828 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 26.547467150697887,
            "unit": "ns/iter",
            "extra": "iterations: 26431458\ncpu: 26.546823637197658 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 26.477367654652316,
            "unit": "ns/iter",
            "extra": "iterations: 27120013\ncpu: 26.4737499941464 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 26.964410111460822,
            "unit": "ns/iter",
            "extra": "iterations: 26460465\ncpu: 26.963903431024377 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 25.790320678186735,
            "unit": "ns/iter",
            "extra": "iterations: 27477235\ncpu: 25.787296320026435 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 27.229267176826223,
            "unit": "ns/iter",
            "extra": "iterations: 25739729\ncpu: 27.228618296641702 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 31.30857936484469,
            "unit": "ns/iter",
            "extra": "iterations: 23719658\ncpu: 31.30562864776556 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 32.14266506315601,
            "unit": "ns/iter",
            "extra": "iterations: 21708251\ncpu: 32.139616268487124 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 32.248654238418204,
            "unit": "ns/iter",
            "extra": "iterations: 21805311\ncpu: 32.24724559076464 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 35.110175704375756,
            "unit": "ns/iter",
            "extra": "iterations: 20692484\ncpu: 35.10731055778543 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 39.50415220530442,
            "unit": "ns/iter",
            "extra": "iterations: 16028350\ncpu: 39.503384315915405 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 58.97916781492886,
            "unit": "ns/iter",
            "extra": "iterations: 11902016\ncpu: 58.97406044488638 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 38.68332412366423,
            "unit": "ns/iter",
            "extra": "iterations: 18475149\ncpu: 38.68347069893737 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 38.25030180552017,
            "unit": "ns/iter",
            "extra": "iterations: 18439855\ncpu: 38.2463104509232 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 37.302636906276504,
            "unit": "ns/iter",
            "extra": "iterations: 18953878\ncpu: 37.30141900248586 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 37.864463196776,
            "unit": "ns/iter",
            "extra": "iterations: 18470474\ncpu: 37.862986678090174 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 37.87110743663981,
            "unit": "ns/iter",
            "extra": "iterations: 18515723\ncpu: 37.86769406736117 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 37.42828761326423,
            "unit": "ns/iter",
            "extra": "iterations: 18483348\ncpu: 37.427151076742625 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 37.37122550242316,
            "unit": "ns/iter",
            "extra": "iterations: 18273591\ncpu: 37.36742597555169 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 37.897099403205516,
            "unit": "ns/iter",
            "extra": "iterations: 17736350\ncpu: 37.897223385871115 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 39.805711977939566,
            "unit": "ns/iter",
            "extra": "iterations: 18107668\ncpu: 39.801746530806284 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 42.55944056576595,
            "unit": "ns/iter",
            "extra": "iterations: 16598412\ncpu: 42.556789589269215 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 44.29631425989543,
            "unit": "ns/iter",
            "extra": "iterations: 13065517\ncpu: 44.27938473464193 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 43.27122337684864,
            "unit": "ns/iter",
            "extra": "iterations: 16170365\ncpu: 43.27137185833428 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 46.57705780416539,
            "unit": "ns/iter",
            "extra": "iterations: 15133425\ncpu: 46.574223680363076 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 55.867612627341835,
            "unit": "ns/iter",
            "extra": "iterations: 12768748\ncpu: 55.86395588666999 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 76.30281498155439,
            "unit": "ns/iter",
            "extra": "iterations: 9041871\ncpu: 76.29898889289711 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 78.5000160948006,
            "unit": "ns/iter",
            "extra": "iterations: 9133384\ncpu: 78.49403933963542 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 287.2729369333109,
            "unit": "ns/iter",
            "extra": "iterations: 2509250\ncpu: 287.2432631264346 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1120.1048399843612,
            "unit": "ns/iter",
            "extra": "iterations: 620002\ncpu: 1120.1071141706127 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4695.739166311764,
            "unit": "ns/iter",
            "extra": "iterations: 147849\ncpu: 4695.216545258998 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 29224.56084785151,
            "unit": "ns/iter",
            "extra": "iterations: 24438\ncpu: 29223.813282592662 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 263607.25579625164,
            "unit": "ns/iter",
            "extra": "iterations: 2631\ncpu: 263584.31204865035 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1426511.7473905112,
            "unit": "ns/iter",
            "extra": "iterations: 479\ncpu: 1426415.972860118 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 9151051.544302944,
            "unit": "ns/iter",
            "extra": "iterations: 79\ncpu: 9150616.392405037 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 446.1379243480619,
            "unit": "ns/iter",
            "extra": "iterations: 1572913\ncpu: 446.08783511866767 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1759.5494350704075,
            "unit": "ns/iter",
            "extra": "iterations: 406068\ncpu: 1759.4806879635726 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7414.35934233462,
            "unit": "ns/iter",
            "extra": "iterations: 88008\ncpu: 7412.183631033571 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 30034.296321037407,
            "unit": "ns/iter",
            "extra": "iterations: 23947\ncpu: 30033.636697707905 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 117244.8239848509,
            "unit": "ns/iter",
            "extra": "iterations: 5812\ncpu: 117241.63024776359 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 576558.6215539841,
            "unit": "ns/iter",
            "extra": "iterations: 1197\ncpu: 576484.5288220521 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2455319.121427887,
            "unit": "ns/iter",
            "extra": "iterations: 280\ncpu: 2455260.5571428845 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 10186525.475412473,
            "unit": "ns/iter",
            "extra": "iterations: 61\ncpu: 10186238.459016249 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 968.4199514582261,
            "unit": "ns/iter",
            "extra": "iterations: 720619\ncpu: 968.4006819137413 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3849.611164500001,
            "unit": "ns/iter",
            "extra": "iterations: 176900\ncpu: 3849.5314245336267 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 13007.839787024428,
            "unit": "ns/iter",
            "extra": "iterations: 53716\ncpu: 13007.033379253791 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 62510.381637546445,
            "unit": "ns/iter",
            "extra": "iterations: 11676\ncpu: 62500.49734498147 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 251021.35401326325,
            "unit": "ns/iter",
            "extra": "iterations: 2853\ncpu: 251003.79390115815 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 1041450.8002851235,
            "unit": "ns/iter",
            "extra": "iterations: 701\ncpu: 1041354.0670470686 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4530183.327043124,
            "unit": "ns/iter",
            "extra": "iterations: 159\ncpu: 4529898.427672993 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 18603593.789473385,
            "unit": "ns/iter",
            "extra": "iterations: 38\ncpu: 18603633.473684203 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 4264.887943533394,
            "unit": "ns/iter",
            "extra": "iterations: 167603\ncpu: 4264.432778649549 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 18.118198601465917,
            "unit": "ns/iter",
            "extra": "iterations: 38894758\ncpu: 18.11777183444634 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 73.95180907090422,
            "unit": "ns/iter",
            "extra": "iterations: 9445014\ncpu: 73.94665471115239 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.7416496311827692,
            "unit": "ms/iter",
            "extra": "iterations: 930\ncpu: 0.7416184440860275 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.41224403296060136,
            "unit": "ms/iter",
            "extra": "iterations: 1699\ncpu: 0.41221542966450975 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 407.9086834000009,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 407.8817063999992 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 398.8900570000169,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 398.857516999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 1629.9094524797974,
            "unit": "ns/iter",
            "extra": "iterations: 431199\ncpu: 1629.8090695943238 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 17.68337397913691,
            "unit": "ns/iter",
            "extra": "iterations: 39628117\ncpu: 17.681915595434344 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 76.09659127987084,
            "unit": "ns/iter",
            "extra": "iterations: 9316928\ncpu: 76.08937795805673 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 176.78450311994916,
            "unit": "ns/iter",
            "extra": "iterations: 4043330\ncpu: 176.77684334447383 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 4.6110639333331465,
            "unit": "ms/iter",
            "extra": "iterations: 150\ncpu: 4.610549960000109 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 27.836521230763257,
            "unit": "ms/iter",
            "extra": "iterations: 26\ncpu: 27.833239576923372 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 3112.6144267999734,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3112.3993840000026 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 3122.10038420003,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3121.8693782 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "56431339+StephenSmith25@users.noreply.github.com",
            "name": "StephenSmith25",
            "username": "StephenSmith25"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "865ab5ccbfb0b38202f5427b4993b2ccfebc0052",
          "message": "Merge pull request #659 from StephenSmith25/parallel_energy_routines\n\nParallel energy routines",
          "timestamp": "2021-04-29T11:53:12+01:00",
          "tree_id": "e7aeaa8312ccc0753a34f78e3d7257d2c6d69924",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/865ab5ccbfb0b38202f5427b4993b2ccfebc0052"
        },
        "date": 1619694833690,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 17.882386800010863,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 17.880483200000004 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 28.124831399964023,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 28.124727400000005 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 427.99094500001047,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 427.9547586 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 665.5028443999981,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 665.4643664 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1704.3044107999776,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1704.2209454000003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 2509.105229800025,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2508.9056418 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 9.063298972798135,
            "unit": "ns/iter",
            "extra": "iterations: 77336879\ncpu: 9.063065966238403 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 9.078115299803743,
            "unit": "ns/iter",
            "extra": "iterations: 77277755\ncpu: 9.077891535539568 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 18.94944146079144,
            "unit": "ns/iter",
            "extra": "iterations: 38988310\ncpu: 18.948569148034487 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 18.15647543662605,
            "unit": "ns/iter",
            "extra": "iterations: 37460084\ncpu: 18.155275546098682 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 12.881664067530718,
            "unit": "ns/iter",
            "extra": "iterations: 53485690\ncpu: 12.880866358833472 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 12.921002323406496,
            "unit": "ns/iter",
            "extra": "iterations: 53991436\ncpu: 12.92050094759476 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 12.82955511548648,
            "unit": "ns/iter",
            "extra": "iterations: 55498785\ncpu: 12.829119051885488 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.09513437676114,
            "unit": "ns/iter",
            "extra": "iterations: 53451467\ncpu: 13.094561913520554 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.16993169798223,
            "unit": "ns/iter",
            "extra": "iterations: 53706172\ncpu: 13.169609779673026 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.220007098202426,
            "unit": "ns/iter",
            "extra": "iterations: 52999340\ncpu: 13.218914518558138 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.454386193968013,
            "unit": "ns/iter",
            "extra": "iterations: 52343604\ncpu: 13.453925507307353 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.373484309953295,
            "unit": "ns/iter",
            "extra": "iterations: 52081971\ncpu: 13.372856050321055 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 13.487669444993827,
            "unit": "ns/iter",
            "extra": "iterations: 52346265\ncpu: 13.487450919373181 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 13.891202403756397,
            "unit": "ns/iter",
            "extra": "iterations: 50723253\ncpu: 13.891038396137528 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 14.753223552756566,
            "unit": "ns/iter",
            "extra": "iterations: 47858376\ncpu: 14.752326343041744 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 17.160233177717092,
            "unit": "ns/iter",
            "extra": "iterations: 40900306\ncpu: 17.158550280773024 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 17.708158753571087,
            "unit": "ns/iter",
            "extra": "iterations: 39778380\ncpu: 17.706783458753254 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 19.1265892520674,
            "unit": "ns/iter",
            "extra": "iterations: 35992953\ncpu: 19.125375014381177 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 25.028277815881278,
            "unit": "ns/iter",
            "extra": "iterations: 27424077\ncpu: 25.02711551604824 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 26.30857198444657,
            "unit": "ns/iter",
            "extra": "iterations: 26881815\ncpu: 26.307843759805568 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 25.803021293457803,
            "unit": "ns/iter",
            "extra": "iterations: 26705582\ncpu: 25.801242302077547 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 26.132458471707896,
            "unit": "ns/iter",
            "extra": "iterations: 27016158\ncpu: 26.132204438543656 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 26.33786226074283,
            "unit": "ns/iter",
            "extra": "iterations: 26848962\ncpu: 26.337217990028833 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 26.397142525039158,
            "unit": "ns/iter",
            "extra": "iterations: 26944488\ncpu: 26.396620080515103 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 26.122343133500163,
            "unit": "ns/iter",
            "extra": "iterations: 26751203\ncpu: 26.12079314713429 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 26.411657844967397,
            "unit": "ns/iter",
            "extra": "iterations: 26439535\ncpu: 26.410456273152846 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 25.891037746979638,
            "unit": "ns/iter",
            "extra": "iterations: 27372626\ncpu: 25.89067165130585 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 25.958551275637642,
            "unit": "ns/iter",
            "extra": "iterations: 27274229\ncpu: 25.957825352276576 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 26.56624507950974,
            "unit": "ns/iter",
            "extra": "iterations: 26356116\ncpu: 26.565330453091203 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 26.696565493828484,
            "unit": "ns/iter",
            "extra": "iterations: 26056730\ncpu: 26.696262731355617 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 29.727067808601532,
            "unit": "ns/iter",
            "extra": "iterations: 23500676\ncpu: 29.725137736463182 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 30.98542624668645,
            "unit": "ns/iter",
            "extra": "iterations: 22481031\ncpu: 30.983819736737306 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 33.22390047437297,
            "unit": "ns/iter",
            "extra": "iterations: 18675713\ncpu: 33.221084892448516 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 54.88161030126385,
            "unit": "ns/iter",
            "extra": "iterations: 12034096\ncpu: 54.87814689196423 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 38.7295017322862,
            "unit": "ns/iter",
            "extra": "iterations: 17889841\ncpu: 38.72631059158102 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 39.350575881970116,
            "unit": "ns/iter",
            "extra": "iterations: 17929542\ncpu: 39.34745232198332 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 38.672387113096335,
            "unit": "ns/iter",
            "extra": "iterations: 17971616\ncpu: 38.62693544086342 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 38.62905419514489,
            "unit": "ns/iter",
            "extra": "iterations: 18226614\ncpu: 38.626867612382256 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 39.17222020588038,
            "unit": "ns/iter",
            "extra": "iterations: 17892674\ncpu: 39.17139154270631 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 39.21802754125045,
            "unit": "ns/iter",
            "extra": "iterations: 17887786\ncpu: 39.217091427636596 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 38.16976458048381,
            "unit": "ns/iter",
            "extra": "iterations: 17990225\ncpu: 38.16875909000647 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 38.31744901908037,
            "unit": "ns/iter",
            "extra": "iterations: 18124526\ncpu: 38.31561829534216 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 39.12957895994876,
            "unit": "ns/iter",
            "extra": "iterations: 17896231\ncpu: 39.126906106654346 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 39.31225749747311,
            "unit": "ns/iter",
            "extra": "iterations: 17831258\ncpu: 39.309646801140154 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 39.47077816010727,
            "unit": "ns/iter",
            "extra": "iterations: 17744793\ncpu: 39.46828559792136 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 41.438753568757924,
            "unit": "ns/iter",
            "extra": "iterations: 16854231\ncpu: 41.43653157477182 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 42.71671670396052,
            "unit": "ns/iter",
            "extra": "iterations: 16324157\ncpu: 42.71453956244133 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 48.299012262805405,
            "unit": "ns/iter",
            "extra": "iterations: 14908014\ncpu: 48.29486053608424 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 57.6518135676862,
            "unit": "ns/iter",
            "extra": "iterations: 9924416\ncpu: 57.64942592088101 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 111.20770865701769,
            "unit": "ns/iter",
            "extra": "iterations: 6226558\ncpu: 111.1989396067629 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 417.27895906334777,
            "unit": "ns/iter",
            "extra": "iterations: 1688556\ncpu: 417.26480199649495 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1636.6153501002843,
            "unit": "ns/iter",
            "extra": "iterations: 423450\ncpu: 1636.5551989609337 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 6574.409211008653,
            "unit": "ns/iter",
            "extra": "iterations: 106351\ncpu: 6574.24963564049 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 26228.377048562455,
            "unit": "ns/iter",
            "extra": "iterations: 26604\ncpu: 26227.754397835 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 130794.54287327426,
            "unit": "ns/iter",
            "extra": "iterations: 5318\ncpu: 130790.65306505979 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1033994.0479042808,
            "unit": "ns/iter",
            "extra": "iterations: 668\ncpu: 1033951.5553892145 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 7575443.630435569,
            "unit": "ns/iter",
            "extra": "iterations: 92\ncpu: 7574964.184782612 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 443.0942114967139,
            "unit": "ns/iter",
            "extra": "iterations: 1573481\ncpu: 443.0558144648646 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1737.554508019706,
            "unit": "ns/iter",
            "extra": "iterations: 399244\ncpu: 1737.5027727404897 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7267.181232316678,
            "unit": "ns/iter",
            "extra": "iterations: 95430\ncpu: 7267.128334905202 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 29251.39681485568,
            "unit": "ns/iter",
            "extra": "iterations: 24112\ncpu: 29250.222710683836 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 117931.80155513916,
            "unit": "ns/iter",
            "extra": "iterations: 5916\ncpu: 117929.78093306096 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 468612.8544509184,
            "unit": "ns/iter",
            "extra": "iterations: 1539\ncpu: 468576.71020142996 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2346610.0709460867,
            "unit": "ns/iter",
            "extra": "iterations: 296\ncpu: 2346394.9763513147 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 12216790.236363357,
            "unit": "ns/iter",
            "extra": "iterations: 55\ncpu: 12215803.381818349 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 878.1436719743463,
            "unit": "ns/iter",
            "extra": "iterations: 808230\ncpu: 878.0925293542562 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3616.1812462556527,
            "unit": "ns/iter",
            "extra": "iterations: 193604\ncpu: 3616.05945641623 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 12173.069637153487,
            "unit": "ns/iter",
            "extra": "iterations: 57297\ncpu: 12171.990732499182 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 56632.52206001529,
            "unit": "ns/iter",
            "extra": "iterations: 12330\ncpu: 56629.98824006464 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 234368.3152246717,
            "unit": "ns/iter",
            "extra": "iterations: 2982\ncpu: 234358.06807511637 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 896476.759330706,
            "unit": "ns/iter",
            "extra": "iterations: 777\ncpu: 896455.5019304958 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4395721.587499679,
            "unit": "ns/iter",
            "extra": "iterations: 160\ncpu: 4395398.837499975 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 21266491.151512433,
            "unit": "ns/iter",
            "extra": "iterations: 33\ncpu: 21265422.878787953 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1884.2062602295987,
            "unit": "ns/iter",
            "extra": "iterations: 398356\ncpu: 1884.1092213999711 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 147.32416507375643,
            "unit": "ns/iter",
            "extra": "iterations: 4743503\ncpu: 147.3204621141788 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 20.03506193562812,
            "unit": "ns/iter",
            "extra": "iterations: 34745914\ncpu: 20.03455528612663 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 73.76435294324399,
            "unit": "ns/iter",
            "extra": "iterations: 9571556\ncpu: 73.76251823632472 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3624544374347954,
            "unit": "ms/iter",
            "extra": "iterations: 1918\ncpu: 0.36243044369134264 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3838930626029179,
            "unit": "ms/iter",
            "extra": "iterations: 1821\ncpu: 0.38386685941790294 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 261.2715277999996,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 261.2561291999981 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 259.4496161999814,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 259.42815520000124 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 819.9949230098497,
            "unit": "ns/iter",
            "extra": "iterations: 849913\ncpu: 819.9671131045067 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 52666.81179479107,
            "unit": "ns/iter",
            "extra": "iterations: 13294\ncpu: 52665.44975176717 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 19.886515071070825,
            "unit": "ns/iter",
            "extra": "iterations: 34844900\ncpu: 19.88538919612328 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 73.93707630114305,
            "unit": "ns/iter",
            "extra": "iterations: 9617680\ncpu: 73.9314763019773 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 170.73850199429265,
            "unit": "ns/iter",
            "extra": "iterations: 4248128\ncpu: 170.73376673207795 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 2.9833247253225816,
            "unit": "ms/iter",
            "extra": "iterations: 233\ncpu: 2.9830677510730226 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 25.212550071427945,
            "unit": "ms/iter",
            "extra": "iterations: 28\ncpu: 25.210531142857715 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2491.448255999967,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2491.3032402 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2499.6239287999742,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2499.492961799996 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "56431339+StephenSmith25@users.noreply.github.com",
            "name": "StephenSmith25",
            "username": "StephenSmith25"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "1c9e01af57ca5439228b1ab819e6cfa07d571e57",
          "message": "Merge pull request #664 from StephenSmith25/parallel_force_calculations\n\nParallel force calculations",
          "timestamp": "2021-05-06T13:11:19+01:00",
          "tree_id": "39828bb140589e202bbddca157a187380e9ed9fe",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/1c9e01af57ca5439228b1ab819e6cfa07d571e57"
        },
        "date": 1620304370167,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 17.89506939999228,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 17.8949324 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 28.252592399985588,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 28.252471000000003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 431.5731180000057,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 431.54445159999995 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 674.197767600026,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 674.1561326 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1704.828616199984,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1704.7443163999997 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 2538.3500941999955,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2538.2260342 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 9.192433292735164,
            "unit": "ns/iter",
            "extra": "iterations: 77261189\ncpu: 9.191985034038257 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 9.019445184126738,
            "unit": "ns/iter",
            "extra": "iterations: 77668434\ncpu: 9.018982859883636 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 18.025233963968862,
            "unit": "ns/iter",
            "extra": "iterations: 38805120\ncpu: 18.023950834322957 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 19.189757588992688,
            "unit": "ns/iter",
            "extra": "iterations: 37333495\ncpu: 19.188750263000973 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 13.176296996875669,
            "unit": "ns/iter",
            "extra": "iterations: 53249126\ncpu: 13.175556627915297 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 13.175679295590536,
            "unit": "ns/iter",
            "extra": "iterations: 53114580\ncpu: 13.17502981290636 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.180240646359243,
            "unit": "ns/iter",
            "extra": "iterations: 53124926\ncpu: 13.18025566755614 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.180070778580873,
            "unit": "ns/iter",
            "extra": "iterations: 53089791\ncpu: 13.17956228156934 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.495620154158736,
            "unit": "ns/iter",
            "extra": "iterations: 51817463\ncpu: 13.495126285900964 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.294881646985868,
            "unit": "ns/iter",
            "extra": "iterations: 52658228\ncpu: 13.294082132805517 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.585716858004107,
            "unit": "ns/iter",
            "extra": "iterations: 51543869\ncpu: 13.584816595742932 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.819677598903278,
            "unit": "ns/iter",
            "extra": "iterations: 50657644\ncpu: 13.819132745296967 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 13.76469622295832,
            "unit": "ns/iter",
            "extra": "iterations: 50880063\ncpu: 13.763712556723846 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 14.104294564495582,
            "unit": "ns/iter",
            "extra": "iterations: 49597532\ncpu: 14.104008380900906 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 15.150378974385143,
            "unit": "ns/iter",
            "extra": "iterations: 46147974\ncpu: 15.149759488899814 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 17.55993733681772,
            "unit": "ns/iter",
            "extra": "iterations: 39794978\ncpu: 17.558716403863702 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 17.75264741325279,
            "unit": "ns/iter",
            "extra": "iterations: 39104964\ncpu: 17.75108663953769 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 20.01720602186261,
            "unit": "ns/iter",
            "extra": "iterations: 36704940\ncpu: 20.015743521171864 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 29.48945682983751,
            "unit": "ns/iter",
            "extra": "iterations: 24269503\ncpu: 29.488134347044518 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 26.321411505205393,
            "unit": "ns/iter",
            "extra": "iterations: 26581326\ncpu: 26.321125778300207 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 26.352002911330416,
            "unit": "ns/iter",
            "extra": "iterations: 26569249\ncpu: 26.351336652383242 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 26.3305921555049,
            "unit": "ns/iter",
            "extra": "iterations: 26583220\ncpu: 26.330336392656694 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 26.85602411225972,
            "unit": "ns/iter",
            "extra": "iterations: 26062753\ncpu: 26.854616049194878 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 26.32535587840748,
            "unit": "ns/iter",
            "extra": "iterations: 26587803\ncpu: 26.324912065882287 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 26.357702521794607,
            "unit": "ns/iter",
            "extra": "iterations: 26545871\ncpu: 26.35717633827124 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 26.449833264781702,
            "unit": "ns/iter",
            "extra": "iterations: 26469512\ncpu: 26.449432728491296 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 26.570626922422893,
            "unit": "ns/iter",
            "extra": "iterations: 26340883\ncpu: 26.569093260844888 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 26.566353214287798,
            "unit": "ns/iter",
            "extra": "iterations: 26311280\ncpu: 26.566011877795276 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 26.595829509141474,
            "unit": "ns/iter",
            "extra": "iterations: 26299686\ncpu: 26.594784325561793 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 27.802541602802805,
            "unit": "ns/iter",
            "extra": "iterations: 25156960\ncpu: 27.802208017184835 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 30.409915873355715,
            "unit": "ns/iter",
            "extra": "iterations: 22838069\ncpu: 30.40879585747802 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 32.65182477581696,
            "unit": "ns/iter",
            "extra": "iterations: 21704310\ncpu: 32.65043284029741 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 39.84933046586649,
            "unit": "ns/iter",
            "extra": "iterations: 18785450\ncpu: 39.8453054890885 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 57.49395794146014,
            "unit": "ns/iter",
            "extra": "iterations: 12680612\ncpu: 57.48752946624382 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 39.24022800252498,
            "unit": "ns/iter",
            "extra": "iterations: 17832434\ncpu: 39.23742440319673 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 39.22342280428161,
            "unit": "ns/iter",
            "extra": "iterations: 17935282\ncpu: 39.22200375773261 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 39.22348449457739,
            "unit": "ns/iter",
            "extra": "iterations: 17841985\ncpu: 39.22313711170524 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 39.458123723773504,
            "unit": "ns/iter",
            "extra": "iterations: 17741716\ncpu: 39.45637073663025 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 39.2586199809378,
            "unit": "ns/iter",
            "extra": "iterations: 17832087\ncpu: 39.25718986229672 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 39.23667751442875,
            "unit": "ns/iter",
            "extra": "iterations: 17843930\ncpu: 39.233774454394094 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 39.316452407080504,
            "unit": "ns/iter",
            "extra": "iterations: 17807060\ncpu: 39.31366688268553 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 39.35580601756693,
            "unit": "ns/iter",
            "extra": "iterations: 17789879\ncpu: 39.35381038847981 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 39.33660961158186,
            "unit": "ns/iter",
            "extra": "iterations: 17790817\ncpu: 39.335687900111324 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 39.409199756871814,
            "unit": "ns/iter",
            "extra": "iterations: 17745382\ncpu: 39.40865916552319 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 40.08322105559776,
            "unit": "ns/iter",
            "extra": "iterations: 17452999\ncpu: 40.079703322047735 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 41.08049786756034,
            "unit": "ns/iter",
            "extra": "iterations: 17087757\ncpu: 41.07801246237328 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 42.82616404680732,
            "unit": "ns/iter",
            "extra": "iterations: 16413279\ncpu: 42.822487572409386 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 45.630482106942374,
            "unit": "ns/iter",
            "extra": "iterations: 13715712\ncpu: 45.627979356813164 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 63.367750673462446,
            "unit": "ns/iter",
            "extra": "iterations: 9205631\ncpu: 63.362889627011896 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 112.36357068946221,
            "unit": "ns/iter",
            "extra": "iterations: 6208336\ncpu: 112.36184349558424 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 420.60830024698396,
            "unit": "ns/iter",
            "extra": "iterations: 1664336\ncpu: 420.5815027734851 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1654.7459625360489,
            "unit": "ns/iter",
            "extra": "iterations: 422914\ncpu: 1654.717928467723 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 6591.535695444911,
            "unit": "ns/iter",
            "extra": "iterations: 106148\ncpu: 6591.334127821467 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 26340.25636876534,
            "unit": "ns/iter",
            "extra": "iterations: 26575\ncpu: 26339.85008466635 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 138813.42687279335,
            "unit": "ns/iter",
            "extra": "iterations: 5046\ncpu: 138803.29885057485 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1073519.5490798366,
            "unit": "ns/iter",
            "extra": "iterations: 652\ncpu: 1073426.452453979 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 6776141.733333437,
            "unit": "ns/iter",
            "extra": "iterations: 90\ncpu: 6775806.488888886 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 447.6147272451286,
            "unit": "ns/iter",
            "extra": "iterations: 1564977\ncpu: 447.60283825257596 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1759.743673858751,
            "unit": "ns/iter",
            "extra": "iterations: 398110\ncpu: 1759.6276054356636 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7327.779809212717,
            "unit": "ns/iter",
            "extra": "iterations: 95499\ncpu: 7327.728520717546 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 29291.38363674558,
            "unit": "ns/iter",
            "extra": "iterations: 23895\ncpu: 29290.312157355504 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 118467.42443727679,
            "unit": "ns/iter",
            "extra": "iterations: 5909\ncpu: 118462.33068200991 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 479136.80821908475,
            "unit": "ns/iter",
            "extra": "iterations: 1460\ncpu: 479115.8534246554 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2445365.9929820374,
            "unit": "ns/iter",
            "extra": "iterations: 285\ncpu: 2445205.2491228003 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 10854821.578124784,
            "unit": "ns/iter",
            "extra": "iterations: 64\ncpu: 10854042.124999853 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 892.1892229168144,
            "unit": "ns/iter",
            "extra": "iterations: 782902\ncpu: 892.1312578074891 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3644.1234427202185,
            "unit": "ns/iter",
            "extra": "iterations: 192162\ncpu: 3643.913187831116 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 12281.549919314675,
            "unit": "ns/iter",
            "extra": "iterations: 57012\ncpu: 12280.935873149498 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 56951.29331382331,
            "unit": "ns/iter",
            "extra": "iterations: 12294\ncpu: 56947.81991215226 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 235580.1340518318,
            "unit": "ns/iter",
            "extra": "iterations: 2969\ncpu: 235568.51566183692 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 910975.3263979178,
            "unit": "ns/iter",
            "extra": "iterations: 769\ncpu: 910932.0182054618 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4549540.435065168,
            "unit": "ns/iter",
            "extra": "iterations: 154\ncpu: 4549132.616883104 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 21015329.11111311,
            "unit": "ns/iter",
            "extra": "iterations: 36\ncpu: 21012550.749999784 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1795.4197015615305,
            "unit": "ns/iter",
            "extra": "iterations: 389360\ncpu: 1795.3394596260591 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 147.23574770737488,
            "unit": "ns/iter",
            "extra": "iterations: 4757603\ncpu: 147.23383350817528 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 19.866562623654787,
            "unit": "ns/iter",
            "extra": "iterations: 35116885\ncpu: 19.865869993878828 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 74.29092832146385,
            "unit": "ns/iter",
            "extra": "iterations: 9262718\ncpu: 74.28991317667202 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.36266523826719127,
            "unit": "ms/iter",
            "extra": "iterations: 1939\ncpu: 0.3626466260959296 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.38957153567452535,
            "unit": "ms/iter",
            "extra": "iterations: 1794\ncpu: 0.38954423188405785 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 260.38735860001907,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 260.37194460000137 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 258.9921583999967,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 258.9772016000012 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 827.8443549761116,
            "unit": "ns/iter",
            "extra": "iterations: 845488\ncpu: 827.7720854701579 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 53653.624256735915,
            "unit": "ns/iter",
            "extra": "iterations: 13118\ncpu: 53650.04162219892 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 19.86817164960133,
            "unit": "ns/iter",
            "extra": "iterations: 35161329\ncpu: 19.86628685167143 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 74.63503357654568,
            "unit": "ns/iter",
            "extra": "iterations: 9374698\ncpu: 74.62986060991005 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 172.3135355856996,
            "unit": "ns/iter",
            "extra": "iterations: 4064065\ncpu: 172.30251115570567 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.144063394849519,
            "unit": "ms/iter",
            "extra": "iterations: 233\ncpu: 3.0696244248927895 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 25.53135007407811,
            "unit": "ms/iter",
            "extra": "iterations: 27\ncpu: 25.52879014814801 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2527.81857519999,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2526.4382701999975 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2543.3653241999764,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2543.1972975999997 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.817618117409151,
            "unit": "ms/iter",
            "extra": "iterations: 247\ncpu: 2.817492615384673 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 41.05227912477846,
            "unit": "ns/iter",
            "extra": "iterations: 17125631\ncpu: 40.92473713815233 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 126.6180299463173,
            "unit": "ns/iter",
            "extra": "iterations: 5521281\ncpu: 126.61448964470058 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4322189480841443,
            "unit": "ms/iter",
            "extra": "iterations: 1618\ncpu: 0.43219587453647024 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 215.96441159999813,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 215.9506754000006 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 215.2680614000019,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 215.24976399999787 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.351978895145647,
            "unit": "ms/iter",
            "extra": "iterations: 515\ncpu: 1.3518783514563133 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 126.74554221001122,
            "unit": "ns/iter",
            "extra": "iterations: 5525608\ncpu: 126.73618993601936 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 40.79720147902863,
            "unit": "ns/iter",
            "extra": "iterations: 17071732\ncpu: 40.79641608713043 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 599.4273299544758,
            "unit": "ns/iter",
            "extra": "iterations: 1167276\ncpu: 599.3938365904917 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1970.4512939999859,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1970.3393664000032 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 46.22940626666908,
            "unit": "ms/iter",
            "extra": "iterations: 15\ncpu: 46.22565526666639 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2016.308935799998,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2016.1870069999964 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "aa3296826611a015753f6006391c8417ca0c5862",
          "message": "Merge pull request #668 from disorderedmaterials/bugfix-08-05-21-data-export\n\nBugfix 08/05/21 Data Export",
          "timestamp": "2021-05-10T15:52:46+01:00",
          "tree_id": "c10b20c4b37efa260d244947b5872f86c889bdde",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/aa3296826611a015753f6006391c8417ca0c5862"
        },
        "date": 1620659639105,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 14.661581000018487,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 14.661401800000002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 22.634512400009044,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 22.6343722 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 356.5113311999994,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 356.4717936 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 567.7222230000098,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 567.6558589999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1445.052005799971,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1444.9520903999996 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 2113.5848389999865,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2113.423337 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.00223461933993,
            "unit": "ns/iter",
            "extra": "iterations: 90063214\ncpu: 8.001291881499997 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 7.496784975517719,
            "unit": "ns/iter",
            "extra": "iterations: 90158878\ncpu: 7.496441160237146 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 17.853553463893164,
            "unit": "ns/iter",
            "extra": "iterations: 39195164\ncpu: 17.852354795606917 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 17.63827704824008,
            "unit": "ns/iter",
            "extra": "iterations: 43319286\ncpu: 17.63691275982711 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 11.992518054717582,
            "unit": "ns/iter",
            "extra": "iterations: 60159622\ncpu: 11.99244152498169 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 12.378808279097248,
            "unit": "ns/iter",
            "extra": "iterations: 54124250\ncpu: 12.377947389571213 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 11.848137500445379,
            "unit": "ns/iter",
            "extra": "iterations: 54522295\ncpu: 11.847418638558825 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 11.792234031985759,
            "unit": "ns/iter",
            "extra": "iterations: 57066627\ncpu: 11.791121262520011 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 11.703491775338858,
            "unit": "ns/iter",
            "extra": "iterations: 59689980\ncpu: 11.702764584608717 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 11.99883129021215,
            "unit": "ns/iter",
            "extra": "iterations: 60017466\ncpu: 11.998223133912402 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 11.922755908488996,
            "unit": "ns/iter",
            "extra": "iterations: 59439187\ncpu: 11.922190608024392 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 12.414769551309657,
            "unit": "ns/iter",
            "extra": "iterations: 56477193\ncpu: 12.413848719429064 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 13.211554685906188,
            "unit": "ns/iter",
            "extra": "iterations: 52335806\ncpu: 13.210268816725542 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 14.402209569759638,
            "unit": "ns/iter",
            "extra": "iterations: 48870600\ncpu: 14.401558892258256 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 15.572575747495074,
            "unit": "ns/iter",
            "extra": "iterations: 46496786\ncpu: 15.571240171309983 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 15.825931389376972,
            "unit": "ns/iter",
            "extra": "iterations: 44204874\ncpu: 15.824623886497152 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 15.669048481070696,
            "unit": "ns/iter",
            "extra": "iterations: 43728988\ncpu: 15.6679300925052 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 19.59087895995897,
            "unit": "ns/iter",
            "extra": "iterations: 35437735\ncpu: 19.589059120172287 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 22.470122900704894,
            "unit": "ns/iter",
            "extra": "iterations: 29576315\ncpu: 22.468121163843477 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 22.572139443433922,
            "unit": "ns/iter",
            "extra": "iterations: 30323321\ncpu: 22.571153304745323 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 22.4350013236265,
            "unit": "ns/iter",
            "extra": "iterations: 31866972\ncpu: 22.433084260406197 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 22.711397009654238,
            "unit": "ns/iter",
            "extra": "iterations: 31254444\ncpu: 22.71018140652252 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 22.419549840646624,
            "unit": "ns/iter",
            "extra": "iterations: 31630621\ncpu: 22.41829773117636 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 22.889579842314944,
            "unit": "ns/iter",
            "extra": "iterations: 29574111\ncpu: 22.888227240372505 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 22.24169736616212,
            "unit": "ns/iter",
            "extra": "iterations: 31229066\ncpu: 22.241313268863095 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 23.020842540569333,
            "unit": "ns/iter",
            "extra": "iterations: 30191377\ncpu: 23.01978607997903 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 22.823441693522813,
            "unit": "ns/iter",
            "extra": "iterations: 28281728\ncpu: 22.82327225550015 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 23.967135859854206,
            "unit": "ns/iter",
            "extra": "iterations: 28665378\ncpu: 23.966382442261803 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 25.688417800555808,
            "unit": "ns/iter",
            "extra": "iterations: 27483027\ncpu: 25.686958936510138 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 28.160481128539764,
            "unit": "ns/iter",
            "extra": "iterations: 25085853\ncpu: 28.159603821325327 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 27.51804702935424,
            "unit": "ns/iter",
            "extra": "iterations: 25671538\ncpu: 27.517548111063803 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 30.81744526446522,
            "unit": "ns/iter",
            "extra": "iterations: 23755226\ncpu: 30.816148665560995 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 32.40458120308221,
            "unit": "ns/iter",
            "extra": "iterations: 21634579\ncpu: 32.4027183057274 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 36.13412597171642,
            "unit": "ns/iter",
            "extra": "iterations: 18227648\ncpu: 36.130987333088946 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 33.719307476004545,
            "unit": "ns/iter",
            "extra": "iterations: 21293125\ncpu: 33.71529712054949 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 34.07680983523715,
            "unit": "ns/iter",
            "extra": "iterations: 20959256\ncpu: 34.07592759972003 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 33.904388197580786,
            "unit": "ns/iter",
            "extra": "iterations: 21676508\ncpu: 33.89751868705041 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 32.98532827342564,
            "unit": "ns/iter",
            "extra": "iterations: 20678616\ncpu: 32.98472881357223 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 33.54698286704337,
            "unit": "ns/iter",
            "extra": "iterations: 20496876\ncpu: 33.54634384283737 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 32.39630008301551,
            "unit": "ns/iter",
            "extra": "iterations: 21543132\ncpu: 32.39310166228371 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 33.855327495078214,
            "unit": "ns/iter",
            "extra": "iterations: 20758236\ncpu: 33.8520999568557 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 33.938815862702484,
            "unit": "ns/iter",
            "extra": "iterations: 20527265\ncpu: 33.9370035413873 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 33.68572572427065,
            "unit": "ns/iter",
            "extra": "iterations: 20522071\ncpu: 33.68416126228208 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 38.173431044100965,
            "unit": "ns/iter",
            "extra": "iterations: 18214438\ncpu: 38.17007656234022 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 40.267916095174904,
            "unit": "ns/iter",
            "extra": "iterations: 17428128\ncpu: 40.26431840528177 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 38.90337932420335,
            "unit": "ns/iter",
            "extra": "iterations: 18214559\ncpu: 38.89684065367755 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 41.864268646309874,
            "unit": "ns/iter",
            "extra": "iterations: 16746897\ncpu: 41.862435470881515 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 46.73100597777462,
            "unit": "ns/iter",
            "extra": "iterations: 13735045\ncpu: 46.72839965213107 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 48.66024532550351,
            "unit": "ns/iter",
            "extra": "iterations: 12676791\ncpu: 48.65880963092263 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 66.5082156539276,
            "unit": "ns/iter",
            "extra": "iterations: 10629586\ncpu: 66.49991815297362 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 242.94163900587787,
            "unit": "ns/iter",
            "extra": "iterations: 2930142\ncpu: 242.93140059424027 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 935.2188143007007,
            "unit": "ns/iter",
            "extra": "iterations: 746007\ncpu: 935.1425428983829 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 3946.9008549254518,
            "unit": "ns/iter",
            "extra": "iterations: 175571\ncpu: 3946.69068354116 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 24677.907319827045,
            "unit": "ns/iter",
            "extra": "iterations: 28334\ncpu: 24675.969718359524 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 219118.71748026295,
            "unit": "ns/iter",
            "extra": "iterations: 3175\ncpu: 219105.04787401322 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1121473.2127660061,
            "unit": "ns/iter",
            "extra": "iterations: 611\ncpu: 1121372.0654664491 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 18732071.552628774,
            "unit": "ns/iter",
            "extra": "iterations: 38\ncpu: 18729663.68421053 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 414.39781615405195,
            "unit": "ns/iter",
            "extra": "iterations: 1679972\ncpu: 414.38388675524976 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1673.8550087690799,
            "unit": "ns/iter",
            "extra": "iterations: 429357\ncpu: 1673.8023695898682 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 6803.805111046573,
            "unit": "ns/iter",
            "extra": "iterations: 103110\ncpu: 6803.293996702656 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 28605.96380696956,
            "unit": "ns/iter",
            "extra": "iterations: 25364\ncpu: 28604.004336855352 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 113275.90230796825,
            "unit": "ns/iter",
            "extra": "iterations: 6326\ncpu: 113267.52418589946 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 491752.47091027116,
            "unit": "ns/iter",
            "extra": "iterations: 1461\ncpu: 491714.5530458624 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2077764.9161853152,
            "unit": "ns/iter",
            "extra": "iterations: 346\ncpu: 2077669.8323699457 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 18445633.638887357,
            "unit": "ns/iter",
            "extra": "iterations: 36\ncpu: 18444272.500000224 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 828.555445470339,
            "unit": "ns/iter",
            "extra": "iterations: 882146\ncpu: 828.4822149621534 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3342.915338870695,
            "unit": "ns/iter",
            "extra": "iterations: 216085\ncpu: 3342.645084110395 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 11107.079460481847,
            "unit": "ns/iter",
            "extra": "iterations: 63390\ncpu: 11106.704006941047 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 51844.200225990884,
            "unit": "ns/iter",
            "extra": "iterations: 13275\ncpu: 51839.71804143057 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 210909.63068526704,
            "unit": "ns/iter",
            "extra": "iterations: 3298\ncpu: 210886.73802304498 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 878029.6134564604,
            "unit": "ns/iter",
            "extra": "iterations: 758\ncpu: 877986.6042216494 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 3838170.64324265,
            "unit": "ns/iter",
            "extra": "iterations: 185\ncpu: 3837834.4216216127 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 21671995.62500599,
            "unit": "ns/iter",
            "extra": "iterations: 32\ncpu: 21670270.625000045 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1507.7441353407587,
            "unit": "ns/iter",
            "extra": "iterations: 454971\ncpu: 1507.595334647712 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 120.5855879432292,
            "unit": "ns/iter",
            "extra": "iterations: 5705015\ncpu: 120.5819422034825 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 16.384569512012195,
            "unit": "ns/iter",
            "extra": "iterations: 44395524\ncpu: 16.382846365322795 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 70.96822713337544,
            "unit": "ns/iter",
            "extra": "iterations: 8321314\ncpu: 70.9667371042614 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.4018477841561333,
            "unit": "ms/iter",
            "extra": "iterations: 1742\ncpu: 0.4018249942594698 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3768146201385066,
            "unit": "ms/iter",
            "extra": "iterations: 1877\ncpu: 0.37676800372935665 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 234.68072040000152,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 234.66830060000066 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 230.42117900004087,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 230.39797899999994 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 751.7108862378301,
            "unit": "ns/iter",
            "extra": "iterations: 926151\ncpu: 751.6932865159106 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 42454.56677233242,
            "unit": "ns/iter",
            "extra": "iterations: 16721\ncpu: 42450.272591352346 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 15.525525903474254,
            "unit": "ns/iter",
            "extra": "iterations: 44511353\ncpu: 15.524427262410972 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 78.24506030087402,
            "unit": "ns/iter",
            "extra": "iterations: 9108905\ncpu: 78.24001018783298 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 174.91585186549167,
            "unit": "ns/iter",
            "extra": "iterations: 4065818\ncpu: 174.9065302480331 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 2.9904927478988044,
            "unit": "ms/iter",
            "extra": "iterations: 238\ncpu: 2.9902335042016697 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 23.84566249999504,
            "unit": "ms/iter",
            "extra": "iterations: 30\ncpu: 23.843712499999963 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2171.154301600018,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2171.001481399998 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2209.2559063999943,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2209.1029059999983 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.5291861745451816,
            "unit": "ms/iter",
            "extra": "iterations: 275\ncpu: 2.528982538181853 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 39.18962861942626,
            "unit": "ns/iter",
            "extra": "iterations: 17162826\ncpu: 39.187268868191765 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 127.6433840338417,
            "unit": "ns/iter",
            "extra": "iterations: 5277725\ncpu: 127.63601381276844 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4343614862671827,
            "unit": "ms/iter",
            "extra": "iterations: 1602\ncpu: 0.4343282134831352 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 193.5222271999919,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 193.50900740000156 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 198.60575180000524,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 198.5915919999968 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.238268134125489,
            "unit": "ms/iter",
            "extra": "iterations: 589\ncpu: 1.2382336655348092 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 135.6367209805075,
            "unit": "ns/iter",
            "extra": "iterations: 5190820\ncpu: 135.63425508879178 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 41.6977989295733,
            "unit": "ns/iter",
            "extra": "iterations: 16726089\ncpu: 41.69489920805734 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 538.2257471335305,
            "unit": "ns/iter",
            "extra": "iterations: 1350294\ncpu: 538.2124892801005 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1677.0689455999673,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1676.9608259999982 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 44.5457335625008,
            "unit": "ms/iter",
            "extra": "iterations: 16\ncpu: 44.53990237499994 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1729.1893508000157,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1729.054930799998 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "13dd87083b71b41fa2b17bcab35290aec4a35b10",
          "message": "Merge pull request #660 from disorderedmaterials/uff4mof\n\nUFF4MOF Testing",
          "timestamp": "2021-05-10T15:53:02+01:00",
          "tree_id": "d8d5c8b0f45f2e669d2aebdd1607b6aa46543218",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/13dd87083b71b41fa2b17bcab35290aec4a35b10"
        },
        "date": 1620659640035,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 18.470398999988902,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 18.468098200000004 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 27.973561999988306,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 27.9735224 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 438.9853617999961,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 438.9473287999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 656.7315004000193,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 656.6973206 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1728.9028658000007,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1728.8047491999998 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 2487.3959835999813,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2487.2200304000003 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 9.14121597802592,
            "unit": "ns/iter",
            "extra": "iterations: 77700039\ncpu: 9.141048951082213 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.988529732163899,
            "unit": "ns/iter",
            "extra": "iterations: 78582995\ncpu: 8.987891807890538 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 18.82646584529997,
            "unit": "ns/iter",
            "extra": "iterations: 39329082\ncpu: 18.825156254600603 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 17.573803176541087,
            "unit": "ns/iter",
            "extra": "iterations: 39679620\ncpu: 17.57259555409049 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 12.973335311651715,
            "unit": "ns/iter",
            "extra": "iterations: 53412400\ncpu: 12.972528158255422 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 12.957936121892498,
            "unit": "ns/iter",
            "extra": "iterations: 53419492\ncpu: 12.957484938269339 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 12.995082158151465,
            "unit": "ns/iter",
            "extra": "iterations: 54218905\ncpu: 12.99441163557243 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 12.923617021809157,
            "unit": "ns/iter",
            "extra": "iterations: 54491604\ncpu: 12.923257278313926 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.780539569835708,
            "unit": "ns/iter",
            "extra": "iterations: 50708098\ncpu: 13.780142473495989 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.136362379713564,
            "unit": "ns/iter",
            "extra": "iterations: 53895070\ncpu: 13.135570099454348 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.689333953592532,
            "unit": "ns/iter",
            "extra": "iterations: 51105778\ncpu: 13.687565582114868 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.58991985907803,
            "unit": "ns/iter",
            "extra": "iterations: 51091250\ncpu: 13.589106999730923 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 14.300523710678448,
            "unit": "ns/iter",
            "extra": "iterations: 49444285\ncpu: 14.298997184406666 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 14.77875381335531,
            "unit": "ns/iter",
            "extra": "iterations: 47466052\ncpu: 14.776632128579003 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 15.719201139781381,
            "unit": "ns/iter",
            "extra": "iterations: 44530544\ncpu: 15.717367162637823 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 17.33423304821907,
            "unit": "ns/iter",
            "extra": "iterations: 40385016\ncpu: 17.33172848068214 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 17.696053816987895,
            "unit": "ns/iter",
            "extra": "iterations: 39383551\ncpu: 17.693953219200655 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 18.690872507298185,
            "unit": "ns/iter",
            "extra": "iterations: 36186597\ncpu: 18.688201822348788 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 25.90624530693438,
            "unit": "ns/iter",
            "extra": "iterations: 27354172\ncpu: 25.90391041629759 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 25.978593761367478,
            "unit": "ns/iter",
            "extra": "iterations: 26680493\ncpu: 25.97625756015825 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 26.29298509655282,
            "unit": "ns/iter",
            "extra": "iterations: 26998718\ncpu: 26.28853688534398 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 26.224570774230152,
            "unit": "ns/iter",
            "extra": "iterations: 26758237\ncpu: 26.22181752856127 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 26.223647305411774,
            "unit": "ns/iter",
            "extra": "iterations: 26684368\ncpu: 26.221399997181823 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 26.009238402481092,
            "unit": "ns/iter",
            "extra": "iterations: 26980747\ncpu: 26.00895779497878 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 26.1231205827458,
            "unit": "ns/iter",
            "extra": "iterations: 27199189\ncpu: 26.12279038908105 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 25.98662592166029,
            "unit": "ns/iter",
            "extra": "iterations: 26929108\ncpu: 25.986484884683225 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 26.155896246449235,
            "unit": "ns/iter",
            "extra": "iterations: 26737629\ncpu: 26.155907728392936 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 26.36480664304691,
            "unit": "ns/iter",
            "extra": "iterations: 26682206\ncpu: 26.364330932757237 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 26.249256057309562,
            "unit": "ns/iter",
            "extra": "iterations: 26889168\ncpu: 26.249046939645 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 27.334890775765334,
            "unit": "ns/iter",
            "extra": "iterations: 25337052\ncpu: 27.334460299485386 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 29.710570757923634,
            "unit": "ns/iter",
            "extra": "iterations: 23534443\ncpu: 29.710520193743235 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 31.290766489680074,
            "unit": "ns/iter",
            "extra": "iterations: 22562091\ncpu: 31.28845455857801 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 32.817830682275336,
            "unit": "ns/iter",
            "extra": "iterations: 22181518\ncpu: 32.815829647006126 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 41.56818003884648,
            "unit": "ns/iter",
            "extra": "iterations: 16719637\ncpu: 41.56727164590933 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 38.18264508632966,
            "unit": "ns/iter",
            "extra": "iterations: 18260765\ncpu: 38.182140397732304 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 38.44566077959923,
            "unit": "ns/iter",
            "extra": "iterations: 18329652\ncpu: 38.443811971989355 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 38.414413213774196,
            "unit": "ns/iter",
            "extra": "iterations: 18113462\ncpu: 38.41319423089872 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 38.57389854019101,
            "unit": "ns/iter",
            "extra": "iterations: 18109830\ncpu: 38.572696706705365 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 38.130157153990815,
            "unit": "ns/iter",
            "extra": "iterations: 18366889\ncpu: 38.12927605758412 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 38.15929375061342,
            "unit": "ns/iter",
            "extra": "iterations: 18290253\ncpu: 38.15776523156949 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 38.18974545804618,
            "unit": "ns/iter",
            "extra": "iterations: 18332813\ncpu: 38.18886839679199 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 38.63108676613518,
            "unit": "ns/iter",
            "extra": "iterations: 18143830\ncpu: 38.62920678820321 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 38.44174016670658,
            "unit": "ns/iter",
            "extra": "iterations: 18249194\ncpu: 38.440514304357286 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 38.271040285242826,
            "unit": "ns/iter",
            "extra": "iterations: 18227866\ncpu: 38.26973991360324 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 39.52492336893532,
            "unit": "ns/iter",
            "extra": "iterations: 18100362\ncpu: 39.52271275016488 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 41.33314449334201,
            "unit": "ns/iter",
            "extra": "iterations: 16818471\ncpu: 41.331523894175724 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 42.952743447324806,
            "unit": "ns/iter",
            "extra": "iterations: 16375401\ncpu: 42.94973851327313 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 44.30519829211354,
            "unit": "ns/iter",
            "extra": "iterations: 16035863\ncpu: 44.30397721656717 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 56.85176544339021,
            "unit": "ns/iter",
            "extra": "iterations: 11389915\ncpu: 56.849228637790475 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 72.92514168868927,
            "unit": "ns/iter",
            "extra": "iterations: 9746720\ncpu: 72.91965204704712 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 270.4260155311685,
            "unit": "ns/iter",
            "extra": "iterations: 2585002\ncpu: 270.4129787907318 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1040.5958678319869,
            "unit": "ns/iter",
            "extra": "iterations: 663429\ncpu: 1040.5500528315713 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4350.081899054014,
            "unit": "ns/iter",
            "extra": "iterations: 161870\ncpu: 4349.893117934203 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 23360.248622542338,
            "unit": "ns/iter",
            "extra": "iterations: 29402\ncpu: 23359.25753350094 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 132738.63760323726,
            "unit": "ns/iter",
            "extra": "iterations: 5207\ncpu: 132728.65219896496 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1061486.144376818,
            "unit": "ns/iter",
            "extra": "iterations: 658\ncpu: 1061422.3389057703 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 6090015.424240736,
            "unit": "ns/iter",
            "extra": "iterations: 99\ncpu: 6089517.585858555 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 387.86329213939496,
            "unit": "ns/iter",
            "extra": "iterations: 1804717\ncpu: 387.8563663998346 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1521.4291174368934,
            "unit": "ns/iter",
            "extra": "iterations: 461270\ncpu: 1521.3661998395635 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 6338.931944087639,
            "unit": "ns/iter",
            "extra": "iterations: 109028\ncpu: 6338.510153355179 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 25695.989139128364,
            "unit": "ns/iter",
            "extra": "iterations: 27530\ncpu: 25694.85256084306 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 104040.22691566053,
            "unit": "ns/iter",
            "extra": "iterations: 6747\ncpu: 104037.71883800266 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 420126.5816326278,
            "unit": "ns/iter",
            "extra": "iterations: 1666\ncpu: 420107.67767106986 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2293861.249180453,
            "unit": "ns/iter",
            "extra": "iterations: 305\ncpu: 2293835.360655724 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 9427905.736111421,
            "unit": "ns/iter",
            "extra": "iterations: 72\ncpu: 9427129.694444543 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 877.8116919961303,
            "unit": "ns/iter",
            "extra": "iterations: 821314\ncpu: 877.8035732983957 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3599.7253668654857,
            "unit": "ns/iter",
            "extra": "iterations: 196189\ncpu: 3599.6503983404086 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 12218.686687266229,
            "unit": "ns/iter",
            "extra": "iterations: 57291\ncpu: 12218.697160112515 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 56191.2850182555,
            "unit": "ns/iter",
            "extra": "iterations: 12315\ncpu: 56188.784896467216 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 231622.7451891789,
            "unit": "ns/iter",
            "extra": "iterations: 3014\ncpu: 231608.46118115168 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 908653.1901039882,
            "unit": "ns/iter",
            "extra": "iterations: 768\ncpu: 908614.3763020934 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4323636.938270732,
            "unit": "ns/iter",
            "extra": "iterations: 162\ncpu: 4323589.376543204 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 18140722.105266303,
            "unit": "ns/iter",
            "extra": "iterations: 38\ncpu: 18139694.5526317 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1786.9815598594887,
            "unit": "ns/iter",
            "extra": "iterations: 407806\ncpu: 1786.8704261339954 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 142.02692847208988,
            "unit": "ns/iter",
            "extra": "iterations: 4836925\ncpu: 142.0195826894175 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 19.73532325336836,
            "unit": "ns/iter",
            "extra": "iterations: 35438610\ncpu: 19.734721085279762 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 75.3536557871408,
            "unit": "ns/iter",
            "extra": "iterations: 9310047\ncpu: 75.3486508714724 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.35948056087407043,
            "unit": "ms/iter",
            "extra": "iterations: 1922\ncpu: 0.35945992559833695 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.38238662750548397,
            "unit": "ms/iter",
            "extra": "iterations: 1796\ncpu: 0.3823531336302847 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 255.24901879998652,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 255.23956360000003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 254.51567159998376,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 254.5027916000009 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 808.9559124322348,
            "unit": "ns/iter",
            "extra": "iterations: 730909\ncpu: 808.9188462585532 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 52197.741067422285,
            "unit": "ns/iter",
            "extra": "iterations: 13378\ncpu: 52196.617880101956 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 19.754909749204216,
            "unit": "ns/iter",
            "extra": "iterations: 36030099\ncpu: 19.754080969913662 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 76.13960777822771,
            "unit": "ns/iter",
            "extra": "iterations: 9159461\ncpu: 76.13543864644444 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 170.55460593447683,
            "unit": "ns/iter",
            "extra": "iterations: 4141418\ncpu: 170.54806759424036 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.1048913026312985,
            "unit": "ms/iter",
            "extra": "iterations: 228\ncpu: 3.1047171973684446 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 24.555288678576353,
            "unit": "ms/iter",
            "extra": "iterations: 28\ncpu: 24.553964071427817 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2503.7928486000055,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2503.679041800001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2515.3687770000033,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2515.2565505999974 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.7627207751939125,
            "unit": "ms/iter",
            "extra": "iterations: 258\ncpu: 2.7625929922480683 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 41.87591124424165,
            "unit": "ns/iter",
            "extra": "iterations: 16798460\ncpu: 41.871470480032265 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 121.80575523023597,
            "unit": "ns/iter",
            "extra": "iterations: 5739614\ncpu: 121.79827458780458 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4259441441551166,
            "unit": "ms/iter",
            "extra": "iterations: 1651\ncpu: 0.42593554996971533 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 212.26520459999847,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 212.25035200000093 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 208.59794779998992,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 208.58454300000062 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.3245859035916874,
            "unit": "ms/iter",
            "extra": "iterations: 529\ncpu: 1.324529361058582 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 123.05012275903637,
            "unit": "ns/iter",
            "extra": "iterations: 5788169\ncpu: 123.04848096176715 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 41.32653443735422,
            "unit": "ns/iter",
            "extra": "iterations: 17083705\ncpu: 41.32583634521914 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 583.3844861662888,
            "unit": "ns/iter",
            "extra": "iterations: 1186425\ncpu: 583.3690599911573 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1922.2548599999755,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1922.1779479999955 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 44.98717218750414,
            "unit": "ms/iter",
            "extra": "iterations: 16\ncpu: 44.98296224999976 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1975.5238757999905,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1975.4272741999953 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "491023b5ffd7340144837aa50fee4ff8fa4bb378",
          "message": "Merge pull request #669 from disorderedmaterials/bugfix-07-05-21-system-tests\n\nBugfix 07/05/21 System Tests",
          "timestamp": "2021-05-10T15:52:32+01:00",
          "tree_id": "71bf61451d659a8d87825b4604a8a0bd5ab6dcff",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/491023b5ffd7340144837aa50fee4ff8fa4bb378"
        },
        "date": 1620659649595,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 16.345345999980054,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 16.345265599999998 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 23.35790000001907,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 23.3543002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 393.01915740002187,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 392.9779542 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 575.4178122000212,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 575.3698585999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1515.7634449999932,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1515.6870198000001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 2203.597864600033,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2202.5313738 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 7.939078919982337,
            "unit": "ns/iter",
            "extra": "iterations: 74327310\ncpu: 7.938083296704824 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.269542525707209,
            "unit": "ns/iter",
            "extra": "iterations: 90724289\ncpu: 8.265773953874675 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 17.431647204588746,
            "unit": "ns/iter",
            "extra": "iterations: 35884231\ncpu: 17.431304379909957 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 18.084810440333506,
            "unit": "ns/iter",
            "extra": "iterations: 41389633\ncpu: 18.083259327281304 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 12.83183771658533,
            "unit": "ns/iter",
            "extra": "iterations: 52877517\ncpu: 12.831671275336168 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 12.123253515747361,
            "unit": "ns/iter",
            "extra": "iterations: 58106951\ncpu: 12.121687455258122 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.446045894462443,
            "unit": "ns/iter",
            "extra": "iterations: 49450324\ncpu: 13.445388689465462 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 12.254612985659453,
            "unit": "ns/iter",
            "extra": "iterations: 56953949\ncpu: 12.253408486213976 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 12.327203680869992,
            "unit": "ns/iter",
            "extra": "iterations: 56616206\ncpu: 12.32644416335492 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 12.545146304333784,
            "unit": "ns/iter",
            "extra": "iterations: 57775903\ncpu: 12.54467913725218 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 12.437440422696474,
            "unit": "ns/iter",
            "extra": "iterations: 54690499\ncpu: 12.436535329472905 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 12.661454933003927,
            "unit": "ns/iter",
            "extra": "iterations: 56555525\ncpu: 12.660993421951291 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 13.284659758655827,
            "unit": "ns/iter",
            "extra": "iterations: 52671818\ncpu: 13.283308011126614 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 15.18311295291211,
            "unit": "ns/iter",
            "extra": "iterations: 46403850\ncpu: 15.182242292396028 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 16.16653559137108,
            "unit": "ns/iter",
            "extra": "iterations: 46248228\ncpu: 16.164418796759108 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 15.17330485652754,
            "unit": "ns/iter",
            "extra": "iterations: 42505237\ncpu: 15.171528793028568 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 16.639832984039366,
            "unit": "ns/iter",
            "extra": "iterations: 45902918\ncpu: 16.638168536475245 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 16.034296045089665,
            "unit": "ns/iter",
            "extra": "iterations: 40325466\ncpu: 16.03305863842967 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 17.818828266523298,
            "unit": "ns/iter",
            "extra": "iterations: 37671997\ncpu: 17.81694891300832 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 23.46271383814913,
            "unit": "ns/iter",
            "extra": "iterations: 29828868\ncpu: 23.461158298062124 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 23.447939775784405,
            "unit": "ns/iter",
            "extra": "iterations: 31370639\ncpu: 23.442488850800782 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 23.501439878286057,
            "unit": "ns/iter",
            "extra": "iterations: 29588612\ncpu: 23.500213088738434 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 23.22828401048202,
            "unit": "ns/iter",
            "extra": "iterations: 30755794\ncpu: 23.227071133328355 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 23.12246025392056,
            "unit": "ns/iter",
            "extra": "iterations: 27251740\ncpu: 23.120739372972235 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 23.110804883495966,
            "unit": "ns/iter",
            "extra": "iterations: 29848531\ncpu: 23.110412803899678 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 24.162364300219828,
            "unit": "ns/iter",
            "extra": "iterations: 29601926\ncpu: 24.160475977137455 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 23.04069204683523,
            "unit": "ns/iter",
            "extra": "iterations: 29492397\ncpu: 23.04012925772015 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 26.40748423692187,
            "unit": "ns/iter",
            "extra": "iterations: 26133379\ncpu: 26.40539116659942 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 27.163861328427693,
            "unit": "ns/iter",
            "extra": "iterations: 26598991\ncpu: 27.074575911545068 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 30.076385070530023,
            "unit": "ns/iter",
            "extra": "iterations: 24186873\ncpu: 30.074561726106484 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 28.393726703389678,
            "unit": "ns/iter",
            "extra": "iterations: 24832207\ncpu: 28.3922890945617 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 31.470306181300575,
            "unit": "ns/iter",
            "extra": "iterations: 23857760\ncpu: 31.46707867796498 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 28.351895455338052,
            "unit": "ns/iter",
            "extra": "iterations: 25257493\ncpu: 28.350133641529617 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 33.0188686074105,
            "unit": "ns/iter",
            "extra": "iterations: 20971182\ncpu: 33.01740946218469 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 35.9344615299267,
            "unit": "ns/iter",
            "extra": "iterations: 19439697\ncpu: 35.93029994243234 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 36.66314564126757,
            "unit": "ns/iter",
            "extra": "iterations: 19348341\ncpu: 36.66042711362183 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 34.41993388517285,
            "unit": "ns/iter",
            "extra": "iterations: 19903857\ncpu: 34.41660287249846 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 34.782552086362045,
            "unit": "ns/iter",
            "extra": "iterations: 19747106\ncpu: 34.78053153712754 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 34.4884521454952,
            "unit": "ns/iter",
            "extra": "iterations: 20027270\ncpu: 34.48597552237524 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 33.43293900104616,
            "unit": "ns/iter",
            "extra": "iterations: 21059819\ncpu: 33.417107905818156 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 36.92638133004299,
            "unit": "ns/iter",
            "extra": "iterations: 20402148\ncpu: 36.92421989096506 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 35.38782033970318,
            "unit": "ns/iter",
            "extra": "iterations: 19959046\ncpu: 35.38451361853663 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 34.972895068111185,
            "unit": "ns/iter",
            "extra": "iterations: 20287046\ncpu: 34.97090848021932 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 42.109801092996484,
            "unit": "ns/iter",
            "extra": "iterations: 17310702\ncpu: 42.10487338988286 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 41.61334435603874,
            "unit": "ns/iter",
            "extra": "iterations: 16793227\ncpu: 41.612903821284945 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 38.9163027223198,
            "unit": "ns/iter",
            "extra": "iterations: 17335068\ncpu: 38.91240322795374 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 41.34807387624864,
            "unit": "ns/iter",
            "extra": "iterations: 15694786\ncpu: 41.34547428681121 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 46.30530706474587,
            "unit": "ns/iter",
            "extra": "iterations: 13931882\ncpu: 46.30199078631283 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 55.13952161185282,
            "unit": "ns/iter",
            "extra": "iterations: 10583665\ncpu: 55.13771666053271 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 79.94073098824191,
            "unit": "ns/iter",
            "extra": "iterations: 9383352\ncpu: 79.93218276368607 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 261.80935902106654,
            "unit": "ns/iter",
            "extra": "iterations: 2823244\ncpu: 261.80053548329647 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 954.0964049410513,
            "unit": "ns/iter",
            "extra": "iterations: 681936\ncpu: 954.0311510170995 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4339.641392099686,
            "unit": "ns/iter",
            "extra": "iterations: 169327\ncpu: 4339.457139144973 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 25300.534576374266,
            "unit": "ns/iter",
            "extra": "iterations: 27996\ncpu: 25297.523467638573 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 232359.31222149156,
            "unit": "ns/iter",
            "extra": "iterations: 3142\ncpu: 232351.86441756535 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1210759.6000000972,
            "unit": "ns/iter",
            "extra": "iterations: 590\ncpu: 1210633.1593220264 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 5285479.137614232,
            "unit": "ns/iter",
            "extra": "iterations: 109\ncpu: 5285324.07339445 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 422.9816576322012,
            "unit": "ns/iter",
            "extra": "iterations: 1618984\ncpu: 422.9627568895117 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1784.2980159044632,
            "unit": "ns/iter",
            "extra": "iterations: 420897\ncpu: 1784.2212726628945 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7068.399823043176,
            "unit": "ns/iter",
            "extra": "iterations: 91548\ncpu: 7067.906125748327 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 30386.93746409051,
            "unit": "ns/iter",
            "extra": "iterations: 22627\ncpu: 30386.486631015836 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 127449.03899809545,
            "unit": "ns/iter",
            "extra": "iterations: 6308\ncpu: 127436.95909955692 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 564695.8024883351,
            "unit": "ns/iter",
            "extra": "iterations: 1286\ncpu: 564679.7822706118 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2032456.8683472357,
            "unit": "ns/iter",
            "extra": "iterations: 357\ncpu: 2032278.4425770321 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 8727383.35526061,
            "unit": "ns/iter",
            "extra": "iterations: 76\ncpu: 8727135.960526392 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 809.6143811658485,
            "unit": "ns/iter",
            "extra": "iterations: 873142\ncpu: 809.5439458873764 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3313.998949101151,
            "unit": "ns/iter",
            "extra": "iterations: 210296\ncpu: 3312.435443375068 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 11594.925822116433,
            "unit": "ns/iter",
            "extra": "iterations: 61366\ncpu: 11593.996952709893 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 50480.23666840395,
            "unit": "ns/iter",
            "extra": "iterations: 13483\ncpu: 50476.261217829895 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 216878.36416023577,
            "unit": "ns/iter",
            "extra": "iterations: 3471\ncpu: 216850.36070296998 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 924555.0369356505,
            "unit": "ns/iter",
            "extra": "iterations: 731\ncpu: 924480.6196990375 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4035252.7967040786,
            "unit": "ns/iter",
            "extra": "iterations: 182\ncpu: 4034852.7967033084 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 16102483.348837523,
            "unit": "ns/iter",
            "extra": "iterations: 43\ncpu: 16102306.62790696 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1646.8118774068994,
            "unit": "ns/iter",
            "extra": "iterations: 439894\ncpu: 1646.7234447389408 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 134.90288222458176,
            "unit": "ns/iter",
            "extra": "iterations: 4995100\ncpu: 134.90134091409584 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 17.10968098833112,
            "unit": "ns/iter",
            "extra": "iterations: 40796031\ncpu: 17.10790387917879 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 74.14640725811952,
            "unit": "ns/iter",
            "extra": "iterations: 8909087\ncpu: 74.14436428783341 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.44406101044864016,
            "unit": "ms/iter",
            "extra": "iterations: 1627\ncpu: 0.444013561155502 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3999483096065254,
            "unit": "ms/iter",
            "extra": "iterations: 1728\ncpu: 0.39990832060184983 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 246.7998980000175,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 246.78421780000122 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 256.5829976000259,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 256.56069800000125 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 824.9697169064962,
            "unit": "ns/iter",
            "extra": "iterations: 895252\ncpu: 824.9003509626283 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 46863.99309955704,
            "unit": "ns/iter",
            "extra": "iterations: 14202\ncpu: 46859.9796507534 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 17.867270044206876,
            "unit": "ns/iter",
            "extra": "iterations: 42279333\ncpu: 17.86693657158669 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 87.59525631141635,
            "unit": "ns/iter",
            "extra": "iterations: 8179542\ncpu: 87.58608709387391 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 186.00358837025576,
            "unit": "ns/iter",
            "extra": "iterations: 3977293\ncpu: 185.98188994373888 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.0798076767239464,
            "unit": "ms/iter",
            "extra": "iterations: 232\ncpu: 3.0796408706896434 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 24.529174750000493,
            "unit": "ms/iter",
            "extra": "iterations: 28\ncpu: 24.52697475000016 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2267.922597200004,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2267.7387372 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2277.0471331999943,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2276.876593200001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.7251642621728154,
            "unit": "ms/iter",
            "extra": "iterations: 267\ncpu: 2.724284865168547 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 40.78917348867486,
            "unit": "ns/iter",
            "extra": "iterations: 18421502\ncpu: 40.78748524414558 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 132.96944559149486,
            "unit": "ns/iter",
            "extra": "iterations: 5163052\ncpu: 132.96165930538984 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4466108599303832,
            "unit": "ms/iter",
            "extra": "iterations: 1435\ncpu: 0.4465588745644593 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 201.60198059998038,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 201.59140060000027 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 206.4239784000165,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 206.40609840000366 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.327461354545366,
            "unit": "ms/iter",
            "extra": "iterations: 550\ncpu: 1.3274038981818526 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 141.35726193636634,
            "unit": "ns/iter",
            "extra": "iterations: 5033265\ncpu: 141.34704987716984 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 41.15528304271104,
            "unit": "ns/iter",
            "extra": "iterations: 16789586\ncpu: 41.15263848673808 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 544.7282651492505,
            "unit": "ns/iter",
            "extra": "iterations: 1343545\ncpu: 544.6601624806015 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1770.6496841999979,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1770.500664399998 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 50.292683923072005,
            "unit": "ms/iter",
            "extra": "iterations: 13\ncpu: 50.29150707692382 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1823.7329414000214,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1823.5907216000044 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "f69ad551851aff1a0d8d20f89582839569e1bc28",
          "message": "Merge pull request #665 from disorderedmaterials/general-functions\n\nGeneralised Functions",
          "timestamp": "2021-05-10T15:52:04+01:00",
          "tree_id": "e8a7f49dd9969ed6bce46be00ad4c55f177be5e0",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/f69ad551851aff1a0d8d20f89582839569e1bc28"
        },
        "date": 1620659689592,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 17.312529000037102,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 17.244713400000002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 24.733026000012615,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 24.732949 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 393.5153544000059,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 393.4767154 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 628.2965136000257,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 628.237356 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1605.9494839999843,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1605.6677233999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 2315.919143200017,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2315.7819418000004 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.460928677629811,
            "unit": "ns/iter",
            "extra": "iterations: 75311925\ncpu: 8.460273230301839 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.589159811401002,
            "unit": "ns/iter",
            "extra": "iterations: 83743543\ncpu: 8.58918407595915 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 19.966032727659947,
            "unit": "ns/iter",
            "extra": "iterations: 34663219\ncpu: 19.965850892267166 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 18.21862763812007,
            "unit": "ns/iter",
            "extra": "iterations: 33533473\ncpu: 18.217767244090684 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 13.441483745306787,
            "unit": "ns/iter",
            "extra": "iterations: 49555675\ncpu: 13.440939085180483 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 14.124470642749918,
            "unit": "ns/iter",
            "extra": "iterations: 49972868\ncpu: 14.123538596984337 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.455095466692105,
            "unit": "ns/iter",
            "extra": "iterations: 51792410\ncpu: 13.454841973949451 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.297117898157971,
            "unit": "ns/iter",
            "extra": "iterations: 52606989\ncpu: 13.296405635380458 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.732732863775645,
            "unit": "ns/iter",
            "extra": "iterations: 55962204\ncpu: 13.732385790237966 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 14.502408751506566,
            "unit": "ns/iter",
            "extra": "iterations: 49392185\ncpu: 14.501363241978474 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.61544706799464,
            "unit": "ns/iter",
            "extra": "iterations: 52599885\ncpu: 13.614527084992652 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.727372823925924,
            "unit": "ns/iter",
            "extra": "iterations: 49977149\ncpu: 13.727115326246416 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 14.793191225649606,
            "unit": "ns/iter",
            "extra": "iterations: 46934174\ncpu: 14.792607344064397 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 16.56849089696756,
            "unit": "ns/iter",
            "extra": "iterations: 40757701\ncpu: 16.56755257123067 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 17.3646037535734,
            "unit": "ns/iter",
            "extra": "iterations: 39691525\ncpu: 17.36223750536161 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 17.502553705486093,
            "unit": "ns/iter",
            "extra": "iterations: 41096360\ncpu: 17.502115004832582 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 17.240430100069485,
            "unit": "ns/iter",
            "extra": "iterations: 41417152\ncpu: 17.23926360267348 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 18.7129361821822,
            "unit": "ns/iter",
            "extra": "iterations: 36398753\ncpu: 18.71258531851364 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 23.447610369963716,
            "unit": "ns/iter",
            "extra": "iterations: 29423132\ncpu: 23.445729672830257 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 25.932963850195122,
            "unit": "ns/iter",
            "extra": "iterations: 27428947\ncpu: 25.93219393365716 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 25.314866933451896,
            "unit": "ns/iter",
            "extra": "iterations: 27870529\ncpu: 25.313635740462637 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 25.61619008368899,
            "unit": "ns/iter",
            "extra": "iterations: 27238908\ncpu: 25.61525326198814 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 25.121494371595986,
            "unit": "ns/iter",
            "extra": "iterations: 27905054\ncpu: 25.11897937914754 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 26.35622701466555,
            "unit": "ns/iter",
            "extra": "iterations: 27263578\ncpu: 26.355656766694196 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 24.526818337898636,
            "unit": "ns/iter",
            "extra": "iterations: 26420392\ncpu: 24.524792289228735 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 26.44229866028833,
            "unit": "ns/iter",
            "extra": "iterations: 27230035\ncpu: 26.441467739574858 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 25.012351421536167,
            "unit": "ns/iter",
            "extra": "iterations: 27116312\ncpu: 25.01028465817904 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 27.186648637344938,
            "unit": "ns/iter",
            "extra": "iterations: 25928140\ncpu: 27.186094876069053 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 29.109083330235343,
            "unit": "ns/iter",
            "extra": "iterations: 24314045\ncpu: 29.107537515867822 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 30.93939720987248,
            "unit": "ns/iter",
            "extra": "iterations: 21978064\ncpu: 30.938268675530455 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 29.27129856709871,
            "unit": "ns/iter",
            "extra": "iterations: 23347134\ncpu: 29.26930427520568 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 33.48858731391203,
            "unit": "ns/iter",
            "extra": "iterations: 20675676\ncpu: 33.48782772568129 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 31.601740881850763,
            "unit": "ns/iter",
            "extra": "iterations: 21769312\ncpu: 31.599316413858205 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 44.847770365833064,
            "unit": "ns/iter",
            "extra": "iterations: 16427695\ncpu: 44.843661268363995 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 36.52002332249824,
            "unit": "ns/iter",
            "extra": "iterations: 18900210\ncpu: 36.51937391171851 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 38.024221942349264,
            "unit": "ns/iter",
            "extra": "iterations: 18642931\ncpu: 38.02279990201119 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 36.45205929545794,
            "unit": "ns/iter",
            "extra": "iterations: 18653637\ncpu: 36.45109610527963 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 37.75117246755201,
            "unit": "ns/iter",
            "extra": "iterations: 18841886\ncpu: 37.74950947054936 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 37.55407718978283,
            "unit": "ns/iter",
            "extra": "iterations: 19296232\ncpu: 37.55170926634794 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 35.27632607248796,
            "unit": "ns/iter",
            "extra": "iterations: 19894840\ncpu: 35.275682538788985 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 38.02030446433043,
            "unit": "ns/iter",
            "extra": "iterations: 18318139\ncpu: 38.01997036926085 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 37.83105787265275,
            "unit": "ns/iter",
            "extra": "iterations: 18955136\ncpu: 37.83022981212047 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 39.267575066398884,
            "unit": "ns/iter",
            "extra": "iterations: 17806405\ncpu: 39.26490692534511 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 43.659051109047226,
            "unit": "ns/iter",
            "extra": "iterations: 13314280\ncpu: 43.65716839363444 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 45.363446056359166,
            "unit": "ns/iter",
            "extra": "iterations: 15404275\ncpu: 45.359396011821616 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 41.689639868025274,
            "unit": "ns/iter",
            "extra": "iterations: 16596555\ncpu: 41.652142146367524 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 48.04329320420954,
            "unit": "ns/iter",
            "extra": "iterations: 13518773\ncpu: 48.042709497378155 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 52.32055558489051,
            "unit": "ns/iter",
            "extra": "iterations: 12634829\ncpu: 52.317109079988406 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 64.28503787442736,
            "unit": "ns/iter",
            "extra": "iterations: 11512649\ncpu: 64.28125038815915 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 79.85049054111012,
            "unit": "ns/iter",
            "extra": "iterations: 8169142\ncpu: 79.84768253508082 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 263.3110609132522,
            "unit": "ns/iter",
            "extra": "iterations: 2555223\ncpu: 263.3033285157469 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1016.1337357731998,
            "unit": "ns/iter",
            "extra": "iterations: 665110\ncpu: 1016.0072033197612 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4408.73471399827,
            "unit": "ns/iter",
            "extra": "iterations: 158233\ncpu: 4408.649485252778 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 28087.92958207606,
            "unit": "ns/iter",
            "extra": "iterations: 24454\ncpu: 28085.237220904615 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 242756.0678905593,
            "unit": "ns/iter",
            "extra": "iterations: 2887\ncpu: 242744.06165569712 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1239330.085814121,
            "unit": "ns/iter",
            "extra": "iterations: 571\ncpu: 1239223.6304728533 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 6833224.398303821,
            "unit": "ns/iter",
            "extra": "iterations: 118\ncpu: 6832681.872881388 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 456.42455263381936,
            "unit": "ns/iter",
            "extra": "iterations: 1541187\ncpu: 456.4034870525173 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1832.8802541947966,
            "unit": "ns/iter",
            "extra": "iterations: 391354\ncpu: 1832.7993887886514 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7752.184077853316,
            "unit": "ns/iter",
            "extra": "iterations: 88267\ncpu: 7752.1058039811005 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 30558.724977413436,
            "unit": "ns/iter",
            "extra": "iterations: 23249\ncpu: 30556.30267968515 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 125221.52981233862,
            "unit": "ns/iter",
            "extra": "iterations: 6021\ncpu: 125221.79405414242 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 549039.3518091043,
            "unit": "ns/iter",
            "extra": "iterations: 1299\ncpu: 549020.835257886 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2216961.6205792115,
            "unit": "ns/iter",
            "extra": "iterations: 311\ncpu: 2216718.6430868176 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 8704562.465752993,
            "unit": "ns/iter",
            "extra": "iterations: 73\ncpu: 8702501.5616439 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 846.8745317577204,
            "unit": "ns/iter",
            "extra": "iterations: 798465\ncpu: 846.8574878047335 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3467.0935808583445,
            "unit": "ns/iter",
            "extra": "iterations: 198983\ncpu: 3466.9288984486434 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 11979.407827371038,
            "unit": "ns/iter",
            "extra": "iterations: 58206\ncpu: 11978.913754595784 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 54460.83539767761,
            "unit": "ns/iter",
            "extra": "iterations: 12837\ncpu: 54452.810236037076 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 218022.3418803013,
            "unit": "ns/iter",
            "extra": "iterations: 3159\ncpu: 218003.82051282085 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 989513.3054830597,
            "unit": "ns/iter",
            "extra": "iterations: 766\ncpu: 989489.5979112259 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4182128.2209290327,
            "unit": "ns/iter",
            "extra": "iterations: 172\ncpu: 4182093.511627925 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 18930793.949999727,
            "unit": "ns/iter",
            "extra": "iterations: 40\ncpu: 18928521.04999996 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1692.6913352151118,
            "unit": "ns/iter",
            "extra": "iterations: 405942\ncpu: 1692.6588379620616 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 138.27323588324768,
            "unit": "ns/iter",
            "extra": "iterations: 5201158\ncpu: 138.26293510022174 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 17.86195006757958,
            "unit": "ns/iter",
            "extra": "iterations: 39884105\ncpu: 17.84705067344531 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 82.01187284866772,
            "unit": "ns/iter",
            "extra": "iterations: 8759987\ncpu: 82.00287100882785 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.43781234793187296,
            "unit": "ms/iter",
            "extra": "iterations: 1644\ncpu: 0.43778064902676633 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.41537371665551176,
            "unit": "ms/iter",
            "extra": "iterations: 1507\ncpu: 0.4153511658925018 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 251.18019219999042,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 251.16033420000008 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 255.94912599999589,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 255.9402288000001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 845.3272441736277,
            "unit": "ns/iter",
            "extra": "iterations: 841167\ncpu: 845.25705953752 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 47817.8125128941,
            "unit": "ns/iter",
            "extra": "iterations: 14545\ncpu: 47812.81780680662 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 17.4783833878625,
            "unit": "ns/iter",
            "extra": "iterations: 39885436\ncpu: 17.477778480345737 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 85.02928200170581,
            "unit": "ns/iter",
            "extra": "iterations: 7754183\ncpu: 85.01275995162791 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 193.54785146423302,
            "unit": "ns/iter",
            "extra": "iterations: 3488911\ncpu: 193.53639602729714 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.2334664497344545,
            "unit": "ms/iter",
            "extra": "iterations: 189\ncpu: 3.2332189153439432 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 28.968152839997856,
            "unit": "ms/iter",
            "extra": "iterations: 25\ncpu: 28.965735799999948 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2429.5680494000408,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2429.4317445999977 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2441.1988140000176,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2441.092504400001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.8104329126207834,
            "unit": "ms/iter",
            "extra": "iterations: 206\ncpu: 2.8102807087379054 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 41.321289745396456,
            "unit": "ns/iter",
            "extra": "iterations: 15930524\ncpu: 41.31880878494606 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 146.8082649726004,
            "unit": "ns/iter",
            "extra": "iterations: 4863295\ncpu: 146.78998518494134 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4939856638713554,
            "unit": "ms/iter",
            "extra": "iterations: 1431\ncpu: 0.4939478329839317 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 221.90335459999915,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 221.8988252000031 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 214.80843779995666,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 214.79076499999792 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.3099779364793176,
            "unit": "ms/iter",
            "extra": "iterations: 551\ncpu: 1.3099099764065283 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 157.20180864414206,
            "unit": "ns/iter",
            "extra": "iterations: 4727077\ncpu: 157.19615758321694 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 44.26285664888885,
            "unit": "ns/iter",
            "extra": "iterations: 15501240\ncpu: 44.260687983669655 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 609.9944879724871,
            "unit": "ns/iter",
            "extra": "iterations: 1218245\ncpu: 609.9751289765392 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1869.8797369999738,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1869.7852853999962 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 50.17461557143373,
            "unit": "ms/iter",
            "extra": "iterations: 14\ncpu: 50.169484928570036 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1929.3467561999933,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1929.2520330000002 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "d78a4f378d656e98a89fd4a26d36188941b617f4",
          "message": "Merge pull request #680 from disorderedmaterials/bragg-2021\n\nBragg Part 4 - Module",
          "timestamp": "2021-05-11T14:31:39+01:00",
          "tree_id": "d667915a93781c3b1629cab4b8876f02e5881980",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/d78a4f378d656e98a89fd4a26d36188941b617f4"
        },
        "date": 1620741142686,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 14.959301399994729,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 14.958039600000001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 22.25512360000721,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 22.198761600000005 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 374.4162338000024,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 374.3697334 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 526.4472091999778,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 526.4004895999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1320.6782186000055,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1320.5419176 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 2050.9665459999724,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2050.7069702 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 7.27798465889268,
            "unit": "ns/iter",
            "extra": "iterations: 95838724\ncpu: 7.277718712114742 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 7.406341222688448,
            "unit": "ns/iter",
            "extra": "iterations: 94733150\ncpu: 7.405902221133804 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 16.114619350591195,
            "unit": "ns/iter",
            "extra": "iterations: 40971389\ncpu: 16.112103009248695 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 14.022272535483499,
            "unit": "ns/iter",
            "extra": "iterations: 46597389\ncpu: 14.021357913422987 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 10.540429337717539,
            "unit": "ns/iter",
            "extra": "iterations: 71740354\ncpu: 10.539400641931586 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 10.976173666753409,
            "unit": "ns/iter",
            "extra": "iterations: 65523007\ncpu: 10.975873375286302 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 11.981160395429905,
            "unit": "ns/iter",
            "extra": "iterations: 61327508\ncpu: 11.980402040793821 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 11.856084579871002,
            "unit": "ns/iter",
            "extra": "iterations: 60675430\ncpu: 11.854225425349261 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 12.053781593314012,
            "unit": "ns/iter",
            "extra": "iterations: 59075509\ncpu: 12.051983910963862 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 11.39897568017009,
            "unit": "ns/iter",
            "extra": "iterations: 62323503\ncpu: 11.393669239034908 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 11.609795761913542,
            "unit": "ns/iter",
            "extra": "iterations: 61136786\ncpu: 11.607542012430944 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 11.736817563815832,
            "unit": "ns/iter",
            "extra": "iterations: 59383959\ncpu: 11.735867947100003 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 12.55486509637651,
            "unit": "ns/iter",
            "extra": "iterations: 55885193\ncpu: 12.553952206982686 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 15.40506118925565,
            "unit": "ns/iter",
            "extra": "iterations: 46197245\ncpu: 15.404866329150153 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 15.551643924551785,
            "unit": "ns/iter",
            "extra": "iterations: 42214559\ncpu: 15.54747244901922 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 14.406871452908444,
            "unit": "ns/iter",
            "extra": "iterations: 46151375\ncpu: 14.405389525230863 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 14.410318858760984,
            "unit": "ns/iter",
            "extra": "iterations: 51348973\ncpu: 14.40873166051436 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 14.721573082603962,
            "unit": "ns/iter",
            "extra": "iterations: 49584351\ncpu: 14.720105179958908 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 15.455607208358245,
            "unit": "ns/iter",
            "extra": "iterations: 44747490\ncpu: 15.453989173470916 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 21.407348837819505,
            "unit": "ns/iter",
            "extra": "iterations: 32775196\ncpu: 21.406735965819884 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 19.803944495354198,
            "unit": "ns/iter",
            "extra": "iterations: 33678478\ncpu: 19.801660900471703 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 22.169819563509193,
            "unit": "ns/iter",
            "extra": "iterations: 33257352\ncpu: 22.168207288421588 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 23.774989930546848,
            "unit": "ns/iter",
            "extra": "iterations: 32663156\ncpu: 23.698817713756856 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 23.06236713687842,
            "unit": "ns/iter",
            "extra": "iterations: 33053754\ncpu: 23.06146823746555 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 21.041827056626072,
            "unit": "ns/iter",
            "extra": "iterations: 29116536\ncpu: 21.032183223993535 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 22.07029527707215,
            "unit": "ns/iter",
            "extra": "iterations: 31120398\ncpu: 22.06775970538678 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 23.309122054999893,
            "unit": "ns/iter",
            "extra": "iterations: 34171127\ncpu: 23.307369230169037 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 23.6463513175935,
            "unit": "ns/iter",
            "extra": "iterations: 27664370\ncpu: 23.64406375420807 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 26.195615066672516,
            "unit": "ns/iter",
            "extra": "iterations: 27676544\ncpu: 26.192757339933607 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 28.875674537023468,
            "unit": "ns/iter",
            "extra": "iterations: 23399012\ncpu: 28.87277685057838 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 26.613603890392714,
            "unit": "ns/iter",
            "extra": "iterations: 26250285\ncpu: 26.611462580310988 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 28.061094166617472,
            "unit": "ns/iter",
            "extra": "iterations: 26838618\ncpu: 28.058932319093127 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 29.06212696465267,
            "unit": "ns/iter",
            "extra": "iterations: 21461438\ncpu: 29.05771901211836 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 47.183001734115074,
            "unit": "ns/iter",
            "extra": "iterations: 19309052\ncpu: 47.179703954393894 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 33.89488290637219,
            "unit": "ns/iter",
            "extra": "iterations: 20843746\ncpu: 33.89120261780175 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 32.73219013271606,
            "unit": "ns/iter",
            "extra": "iterations: 21724446\ncpu: 32.72807007368561 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 33.14584190525578,
            "unit": "ns/iter",
            "extra": "iterations: 20410622\ncpu: 33.14213814748014 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 33.842771490601706,
            "unit": "ns/iter",
            "extra": "iterations: 21137795\ncpu: 33.838685160869424 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 33.825953050991906,
            "unit": "ns/iter",
            "extra": "iterations: 20139767\ncpu: 33.82351061956157 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 29.512905020129406,
            "unit": "ns/iter",
            "extra": "iterations: 21634604\ncpu: 29.509869096748695 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 31.49005490263758,
            "unit": "ns/iter",
            "extra": "iterations: 22981776\ncpu: 31.48693930355946 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 30.49556038645708,
            "unit": "ns/iter",
            "extra": "iterations: 22929248\ncpu: 30.4921671220968 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 35.94478447929057,
            "unit": "ns/iter",
            "extra": "iterations: 21512991\ncpu: 35.94258622615524 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 39.15438596739476,
            "unit": "ns/iter",
            "extra": "iterations: 16988202\ncpu: 39.150683927587615 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 37.0924061362284,
            "unit": "ns/iter",
            "extra": "iterations: 17465312\ncpu: 37.09139830997583 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 34.99586490494593,
            "unit": "ns/iter",
            "extra": "iterations: 19770525\ncpu: 34.99488865369047 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 38.104704206887526,
            "unit": "ns/iter",
            "extra": "iterations: 18965790\ncpu: 38.09692651874755 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 39.12687259186373,
            "unit": "ns/iter",
            "extra": "iterations: 15996812\ncpu: 39.12388981004484 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 40.82329490675748,
            "unit": "ns/iter",
            "extra": "iterations: 18254471\ncpu: 40.819678258548436 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 75.63561444195004,
            "unit": "ns/iter",
            "extra": "iterations: 9978233\ncpu: 75.62529107107433 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 245.0459728105242,
            "unit": "ns/iter",
            "extra": "iterations: 2815425\ncpu: 245.01787190211218 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 990.086474303782,
            "unit": "ns/iter",
            "extra": "iterations: 741862\ncpu: 989.9757919936557 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4097.401282036036,
            "unit": "ns/iter",
            "extra": "iterations: 170354\ncpu: 4096.554316306096 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 23010.031034485848,
            "unit": "ns/iter",
            "extra": "iterations: 27550\ncpu: 23008.35459165184 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 237624.42836624905,
            "unit": "ns/iter",
            "extra": "iterations: 2785\ncpu: 237600.4833034123 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1250530.9805310871,
            "unit": "ns/iter",
            "extra": "iterations: 565\ncpu: 1250406.0070796437 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 5588510.54098363,
            "unit": "ns/iter",
            "extra": "iterations: 122\ncpu: 5587968.024590175 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 357.0969913731038,
            "unit": "ns/iter",
            "extra": "iterations: 1857093\ncpu: 357.0826646807688 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1403.2247238207417,
            "unit": "ns/iter",
            "extra": "iterations: 509452\ncpu: 1403.090251878491 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 6216.475055533505,
            "unit": "ns/iter",
            "extra": "iterations: 121550\ncpu: 6216.00445084331 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 24165.358398408487,
            "unit": "ns/iter",
            "extra": "iterations: 29071\ncpu: 24163.135839840477 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 102230.86639380842,
            "unit": "ns/iter",
            "extra": "iterations: 6841\ncpu: 102227.15275544488 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 552600.0449999628,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 552582.9199999919 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2197346.780645427,
            "unit": "ns/iter",
            "extra": "iterations: 310\ncpu: 2197049.064516149 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 8348672.4875001535,
            "unit": "ns/iter",
            "extra": "iterations: 80\ncpu: 8347113.724999921 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 791.1619214846442,
            "unit": "ns/iter",
            "extra": "iterations: 899825\ncpu: 791.063124496426 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3262.870521246065,
            "unit": "ns/iter",
            "extra": "iterations: 220932\ncpu: 3262.5917341082713 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 10988.230384103823,
            "unit": "ns/iter",
            "extra": "iterations: 63316\ncpu: 10987.407495735528 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 48361.10490720513,
            "unit": "ns/iter",
            "extra": "iterations: 12125\ncpu: 48359.70424742347 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 193438.1902742354,
            "unit": "ns/iter",
            "extra": "iterations: 3537\ncpu: 193405.08368673877 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 915067.9241830565,
            "unit": "ns/iter",
            "extra": "iterations: 765\ncpu: 915008.1856209262 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 3870027.5329669598,
            "unit": "ns/iter",
            "extra": "iterations: 182\ncpu: 3869594.582417585 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 16535024.536584921,
            "unit": "ns/iter",
            "extra": "iterations: 41\ncpu: 16533643.682926945 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1434.9818073232111,
            "unit": "ns/iter",
            "extra": "iterations: 411924\ncpu: 1434.9155159689524 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 107.8297723112889,
            "unit": "ns/iter",
            "extra": "iterations: 6627030\ncpu: 107.82367455707893 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 13.820019719125122,
            "unit": "ns/iter",
            "extra": "iterations: 51365366\ncpu: 13.818764281753753 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 64.86102919326292,
            "unit": "ns/iter",
            "extra": "iterations: 10964238\ncpu: 64.85721625159886 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.4369672595282111,
            "unit": "ms/iter",
            "extra": "iterations: 1653\ncpu: 0.43690669509982033 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3313962104029886,
            "unit": "ms/iter",
            "extra": "iterations: 2134\ncpu: 0.33136363964386295 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 221.30633679998937,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 221.28361459999724 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 228.19968640001207,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 227.67190760000062 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 729.9435219816064,
            "unit": "ns/iter",
            "extra": "iterations: 875916\ncpu: 729.8274777490047 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 39044.92038923597,
            "unit": "ns/iter",
            "extra": "iterations: 17573\ncpu: 39040.59073578854 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 13.938810653391783,
            "unit": "ns/iter",
            "extra": "iterations: 50845632\ncpu: 13.93757172690873 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 66.6358797087553,
            "unit": "ns/iter",
            "extra": "iterations: 9933492\ncpu: 66.63453929393683 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 146.38022664034864,
            "unit": "ns/iter",
            "extra": "iterations: 4657246\ncpu: 146.37522003347328 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.2804277042254517,
            "unit": "ms/iter",
            "extra": "iterations: 213\ncpu: 3.280297140845075 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 21.77963803030087,
            "unit": "ms/iter",
            "extra": "iterations: 33\ncpu: 21.777484060606007 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2239.6834147999925,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2239.4472116000006 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2165.490044800026,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2165.318743800003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.47955827340819,
            "unit": "ms/iter",
            "extra": "iterations: 267\ncpu: 2.4793054119850972 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 33.70586771194494,
            "unit": "ns/iter",
            "extra": "iterations: 19658736\ncpu: 33.703390797861886 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 111.72191220815492,
            "unit": "ns/iter",
            "extra": "iterations: 5812819\ncpu: 111.70818582171525 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.40313509273752907,
            "unit": "ms/iter",
            "extra": "iterations: 1790\ncpu: 0.40307772458101604 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 176.5820423999685,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 176.57308080000007 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 178.3632492000379,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 178.35170880000533 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.2230526927272545,
            "unit": "ms/iter",
            "extra": "iterations: 550\ncpu: 1.2229027018181362 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 105.96311210219422,
            "unit": "ns/iter",
            "extra": "iterations: 7101245\ncpu: 105.95188914056422 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 33.45604910431832,
            "unit": "ns/iter",
            "extra": "iterations: 20349335\ncpu: 33.451665963531426 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 467.96902105708796,
            "unit": "ns/iter",
            "extra": "iterations: 1450340\ncpu: 467.9040645641692 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1608.8376691999656,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1608.6955693999982 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 35.78098347368627,
            "unit": "ms/iter",
            "extra": "iterations: 19\ncpu: 35.78006236842151 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1639.735216200006,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1639.5862600000044 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "16b629f39bea02b3942f37ff23f5541a8ff783a8",
          "message": "Merge pull request #676 from disorderedmaterials/bragg-part3-more-data\n\nBragg Part 3 - More Data",
          "timestamp": "2021-05-11T14:33:55+01:00",
          "tree_id": "122045fbfa14ccef67d0bd9843b7a5727e05fe32",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/16b629f39bea02b3942f37ff23f5541a8ff783a8"
        },
        "date": 1620741285036,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 17.8747802000089,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 17.874676400000006 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 26.780179800016413,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 26.7781548 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 430.7803887999853,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 430.74839440000005 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 639.5232452000073,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 639.4839920000001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1687.7891212000122,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1687.661724 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 2424.2725015999895,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2423.6271468000004 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.852706826676673,
            "unit": "ns/iter",
            "extra": "iterations: 77392229\ncpu: 8.852052213149204 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.902301747628467,
            "unit": "ns/iter",
            "extra": "iterations: 79797671\ncpu: 8.901967414562739 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 17.809614653914217,
            "unit": "ns/iter",
            "extra": "iterations: 36940196\ncpu: 17.809425889348326 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 18.663260268215215,
            "unit": "ns/iter",
            "extra": "iterations: 38961776\ncpu: 18.65668354029871 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 13.080070579385806,
            "unit": "ns/iter",
            "extra": "iterations: 53866152\ncpu: 13.079105075112805 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 15.353893255934162,
            "unit": "ns/iter",
            "extra": "iterations: 45703674\ncpu: 15.35296943523625 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 12.951302287246337,
            "unit": "ns/iter",
            "extra": "iterations: 55176575\ncpu: 12.950706345944777 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.142910759883272,
            "unit": "ns/iter",
            "extra": "iterations: 52446030\ncpu: 13.142376572640536 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.030697663074784,
            "unit": "ns/iter",
            "extra": "iterations: 54353486\ncpu: 13.029862334864704 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.133281381734806,
            "unit": "ns/iter",
            "extra": "iterations: 52234520\ncpu: 13.133020519763535 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.642129181949356,
            "unit": "ns/iter",
            "extra": "iterations: 51092618\ncpu: 13.641046559798468 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.458370691254986,
            "unit": "ns/iter",
            "extra": "iterations: 51710359\ncpu: 13.457620377379303 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 13.437898401151523,
            "unit": "ns/iter",
            "extra": "iterations: 51836572\ncpu: 13.436968420674216 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 13.908698360989675,
            "unit": "ns/iter",
            "extra": "iterations: 50618927\ncpu: 13.907434011787803 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 15.225801741206892,
            "unit": "ns/iter",
            "extra": "iterations: 46027103\ncpu: 15.22464348885917 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 17.035352571475205,
            "unit": "ns/iter",
            "extra": "iterations: 40927829\ncpu: 17.033544119821396 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 17.8761279327797,
            "unit": "ns/iter",
            "extra": "iterations: 38683378\ncpu: 17.875696377911844 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 18.33926554002976,
            "unit": "ns/iter",
            "extra": "iterations: 37383222\ncpu: 18.337951581594442 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 27.60123938514802,
            "unit": "ns/iter",
            "extra": "iterations: 26710341\ncpu: 27.598404677798765 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 26.248968355690167,
            "unit": "ns/iter",
            "extra": "iterations: 26771824\ncpu: 26.246750240103182 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 26.248452022303674,
            "unit": "ns/iter",
            "extra": "iterations: 26691922\ncpu: 26.24656171256596 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 26.885869950254857,
            "unit": "ns/iter",
            "extra": "iterations: 25844403\ncpu: 26.884106086722383 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 26.154672829541376,
            "unit": "ns/iter",
            "extra": "iterations: 26848497\ncpu: 26.15325833695656 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 26.197503414830102,
            "unit": "ns/iter",
            "extra": "iterations: 26715852\ncpu: 26.195852784331926 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 26.26871893103427,
            "unit": "ns/iter",
            "extra": "iterations: 26762439\ncpu: 26.267250940768132 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 26.32832121450677,
            "unit": "ns/iter",
            "extra": "iterations: 26666367\ncpu: 26.32589771227545 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 26.353274780414793,
            "unit": "ns/iter",
            "extra": "iterations: 26547154\ncpu: 26.351745652283633 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 26.298730487811827,
            "unit": "ns/iter",
            "extra": "iterations: 26441101\ncpu: 26.296586401602703 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 26.49459836955101,
            "unit": "ns/iter",
            "extra": "iterations: 26564109\ncpu: 26.492742557260105 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 27.638928185488204,
            "unit": "ns/iter",
            "extra": "iterations: 25506876\ncpu: 27.63507381303775 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 30.479190415978664,
            "unit": "ns/iter",
            "extra": "iterations: 23005938\ncpu: 30.476807726770595 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 31.28929147456949,
            "unit": "ns/iter",
            "extra": "iterations: 22218045\ncpu: 31.286573458645748 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 35.491778859552944,
            "unit": "ns/iter",
            "extra": "iterations: 20038035\ncpu: 35.488236097002456 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 49.99401911774437,
            "unit": "ns/iter",
            "extra": "iterations: 13030519\ncpu: 49.989476474421146 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 38.75713035541695,
            "unit": "ns/iter",
            "extra": "iterations: 17966671\ncpu: 38.75528226681495 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 39.20913078148211,
            "unit": "ns/iter",
            "extra": "iterations: 17906441\ncpu: 39.20888645599669 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 39.05398210308283,
            "unit": "ns/iter",
            "extra": "iterations: 17918735\ncpu: 39.0522800856197 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 38.74479872020314,
            "unit": "ns/iter",
            "extra": "iterations: 17908429\ncpu: 38.742753649692155 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 38.78403486524498,
            "unit": "ns/iter",
            "extra": "iterations: 17951856\ncpu: 38.78193201861743 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 38.77559799648701,
            "unit": "ns/iter",
            "extra": "iterations: 17961435\ncpu: 38.772847325394714 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 38.88490745576926,
            "unit": "ns/iter",
            "extra": "iterations: 18063038\ncpu: 38.882846229964414 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 38.64052854648873,
            "unit": "ns/iter",
            "extra": "iterations: 18126769\ncpu: 38.63702510910819 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 38.75638741209708,
            "unit": "ns/iter",
            "extra": "iterations: 18136140\ncpu: 38.75589193731436 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 38.58225408436624,
            "unit": "ns/iter",
            "extra": "iterations: 18034223\ncpu: 38.579770029460285 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 39.46866449078009,
            "unit": "ns/iter",
            "extra": "iterations: 17751012\ncpu: 39.46619094167708 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 41.772715573949824,
            "unit": "ns/iter",
            "extra": "iterations: 16660640\ncpu: 41.769228312958575 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 43.08476524415757,
            "unit": "ns/iter",
            "extra": "iterations: 16217083\ncpu: 43.08102949217177 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 49.545728674146545,
            "unit": "ns/iter",
            "extra": "iterations: 12670890\ncpu: 49.53946257918789 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 60.046765416813194,
            "unit": "ns/iter",
            "extra": "iterations: 10181049\ncpu: 60.04138375132059 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 110.89237739450559,
            "unit": "ns/iter",
            "extra": "iterations: 6327797\ncpu: 110.88811524769166 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 414.8639824385961,
            "unit": "ns/iter",
            "extra": "iterations: 1676857\ncpu: 414.84114507080716 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1637.4259320078688,
            "unit": "ns/iter",
            "extra": "iterations: 423253\ncpu: 1637.238542904594 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 6558.966002453987,
            "unit": "ns/iter",
            "extra": "iterations: 107596\ncpu: 6558.919811145307 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 26292.42525540822,
            "unit": "ns/iter",
            "extra": "iterations: 26624\ncpu: 26291.245868389084 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 138202.0996829154,
            "unit": "ns/iter",
            "extra": "iterations: 5046\ncpu: 138198.86563614634 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1108307.433811928,
            "unit": "ns/iter",
            "extra": "iterations: 627\ncpu: 1108230.8181818034 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 7177158.619999772,
            "unit": "ns/iter",
            "extra": "iterations: 100\ncpu: 7176492.920000044 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 442.86158042182467,
            "unit": "ns/iter",
            "extra": "iterations: 1580983\ncpu: 442.8181251790829 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1742.6925545331012,
            "unit": "ns/iter",
            "extra": "iterations: 402003\ncpu: 1742.5767494272206 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7249.478307484886,
            "unit": "ns/iter",
            "extra": "iterations: 96070\ncpu: 7249.1108046216395 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 29246.141885973047,
            "unit": "ns/iter",
            "extra": "iterations: 23977\ncpu: 29245.15498185748 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 118464.00253892133,
            "unit": "ns/iter",
            "extra": "iterations: 5908\ncpu: 118459.61882193573 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 477526.9794801888,
            "unit": "ns/iter",
            "extra": "iterations: 1462\ncpu: 477436.77633379365 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2438752.36619683,
            "unit": "ns/iter",
            "extra": "iterations: 284\ncpu: 2438517.475352127 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 10201954.507462023,
            "unit": "ns/iter",
            "extra": "iterations: 67\ncpu: 10200665.761193952 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 883.6612902414015,
            "unit": "ns/iter",
            "extra": "iterations: 795448\ncpu: 883.6086343796168 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3630.18699652373,
            "unit": "ns/iter",
            "extra": "iterations: 193902\ncpu: 3629.9477055419147 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 12279.944082793429,
            "unit": "ns/iter",
            "extra": "iterations: 57299\ncpu: 12279.183249271382 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 56675.71195345033,
            "unit": "ns/iter",
            "extra": "iterations: 12373\ncpu: 56672.2676796247 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 231491.53200006366,
            "unit": "ns/iter",
            "extra": "iterations: 3000\ncpu: 231470.8720000027 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 893525.9885639647,
            "unit": "ns/iter",
            "extra": "iterations: 787\ncpu: 893509.2299873065 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4517813.352564214,
            "unit": "ns/iter",
            "extra": "iterations: 156\ncpu: 4517415.211538469 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 18881284.805552747,
            "unit": "ns/iter",
            "extra": "iterations: 36\ncpu: 18879996.02777768 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1753.7839480352554,
            "unit": "ns/iter",
            "extra": "iterations: 394419\ncpu: 1753.74973061645 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 144.65267085404707,
            "unit": "ns/iter",
            "extra": "iterations: 4783264\ncpu: 144.6462739668986 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 19.41903127000736,
            "unit": "ns/iter",
            "extra": "iterations: 35908912\ncpu: 19.41863253890809 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 76.01535686052951,
            "unit": "ns/iter",
            "extra": "iterations: 9101665\ncpu: 76.01177531803262 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3621889756097373,
            "unit": "ms/iter",
            "extra": "iterations: 1927\ncpu: 0.3621756777374157 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3860718753461838,
            "unit": "ms/iter",
            "extra": "iterations: 1805\ncpu: 0.3860344459833788 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 255.6962117999774,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 255.68247559999972 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 255.18792500001837,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 255.17268520000016 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 803.7820977100913,
            "unit": "ns/iter",
            "extra": "iterations: 860996\ncpu: 803.7435377167762 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 53000.38997444613,
            "unit": "ns/iter",
            "extra": "iterations: 13306\ncpu: 52995.63144446083 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 19.348121231318824,
            "unit": "ns/iter",
            "extra": "iterations: 34286685\ncpu: 19.34776269563536 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 75.83397645752959,
            "unit": "ns/iter",
            "extra": "iterations: 9230980\ncpu: 75.83086736186148 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 171.82401403245225,
            "unit": "ns/iter",
            "extra": "iterations: 4058602\ncpu: 171.8118379185758 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.1230598883920737,
            "unit": "ms/iter",
            "extra": "iterations: 224\ncpu: 3.1227023258928455 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 24.84748092857509,
            "unit": "ms/iter",
            "extra": "iterations: 28\ncpu: 24.845579142857485 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2503.346906200022,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2503.1530241999976 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2535.5766545999813,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2534.9573002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.7987249087302537,
            "unit": "ms/iter",
            "extra": "iterations: 252\ncpu: 2.7983167182540116 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 42.04770281639067,
            "unit": "ns/iter",
            "extra": "iterations: 16583633\ncpu: 42.04546464577447 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 122.79021695704178,
            "unit": "ns/iter",
            "extra": "iterations: 5849591\ncpu: 122.78149327021447 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4353307882715741,
            "unit": "ms/iter",
            "extra": "iterations: 1620\ncpu: 0.43532022222222727 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 212.59429659999114,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 212.58022859999528 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 213.89857340000162,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 213.88562000000206 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.3186543532708903,
            "unit": "ms/iter",
            "extra": "iterations: 535\ncpu: 1.3185464654205805 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 121.81933581139056,
            "unit": "ns/iter",
            "extra": "iterations: 5731324\ncpu: 121.81571082004973 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 41.68401312618543,
            "unit": "ns/iter",
            "extra": "iterations: 16759174\ncpu: 41.68137087185756 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 596.1267127262324,
            "unit": "ns/iter",
            "extra": "iterations: 1162693\ncpu: 596.0557447236606 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1935.8437551999941,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1935.6967372000042 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 46.336275266655015,
            "unit": "ms/iter",
            "extra": "iterations: 15\ncpu: 46.33127926666513 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1985.0709659999666,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1984.9186327999973 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "56431339+StephenSmith25@users.noreply.github.com",
            "name": "StephenSmith25",
            "username": "StephenSmith25"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "dc0ce5d046b7bfeb6492fdde1236894a899cb434",
          "message": "Merge pull request #681 from StephenSmith25/apply_clang_tidy\n\nApply clang tidy",
          "timestamp": "2021-05-11T16:47:00+01:00",
          "tree_id": "850f2ac3c1574de5767b11ac6a0b3315fb101ecd",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/dc0ce5d046b7bfeb6492fdde1236894a899cb434"
        },
        "date": 1620749399078,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 16.82426900001701,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 16.824148 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 20.40112559998306,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 20.395492400000002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 410.4670113999873,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 410.4021960000001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 488.39195359996666,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 488.34600679999994 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1650.8204665999983,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1650.6205192000002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 1826.5535464000095,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1825.7413413999998 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.902462699281626,
            "unit": "ns/iter",
            "extra": "iterations: 78755860\ncpu: 8.901611892753127 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.94324806809943,
            "unit": "ns/iter",
            "extra": "iterations: 79072480\ncpu: 8.942834852277322 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 18.495819079084093,
            "unit": "ns/iter",
            "extra": "iterations: 33205005\ncpu: 18.49426088024986 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 20.6155389878985,
            "unit": "ns/iter",
            "extra": "iterations: 35352547\ncpu: 20.608924839276714 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 13.448971868933636,
            "unit": "ns/iter",
            "extra": "iterations: 52182452\ncpu: 13.448085958091786 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 13.612737110603565,
            "unit": "ns/iter",
            "extra": "iterations: 50784422\ncpu: 13.611262682087803 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.794002148584378,
            "unit": "ns/iter",
            "extra": "iterations: 51659966\ncpu: 13.793777196833611 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 14.139353977684323,
            "unit": "ns/iter",
            "extra": "iterations: 50885982\ncpu: 14.137736950816702 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.939059915444663,
            "unit": "ns/iter",
            "extra": "iterations: 52191050\ncpu: 13.938391524983595 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.897265996761252,
            "unit": "ns/iter",
            "extra": "iterations: 50195844\ncpu: 13.896548367629798 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 14.008276359894671,
            "unit": "ns/iter",
            "extra": "iterations: 51426594\ncpu: 14.005888237513929 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 14.358473342189006,
            "unit": "ns/iter",
            "extra": "iterations: 51305564\ncpu: 14.35727031477526 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 15.601777876268331,
            "unit": "ns/iter",
            "extra": "iterations: 45290216\ncpu: 15.59993964259295 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 16.50333249891829,
            "unit": "ns/iter",
            "extra": "iterations: 42001514\ncpu: 16.502617334222627 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 17.44215126432228,
            "unit": "ns/iter",
            "extra": "iterations: 40462587\ncpu: 17.440959373161153 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 17.681939787643884,
            "unit": "ns/iter",
            "extra": "iterations: 39340595\ncpu: 17.680798777954447 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 17.67232150273741,
            "unit": "ns/iter",
            "extra": "iterations: 38922319\ncpu: 17.670050517801783 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 18.93476845458189,
            "unit": "ns/iter",
            "extra": "iterations: 35204087\ncpu: 18.933422417686913 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 34.51820142489753,
            "unit": "ns/iter",
            "extra": "iterations: 18220112\ncpu: 34.51446763883776 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 26.930679357039697,
            "unit": "ns/iter",
            "extra": "iterations: 26788182\ncpu: 26.929404653141454 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 24.571548534909585,
            "unit": "ns/iter",
            "extra": "iterations: 27266631\ncpu: 24.569536368464558 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 26.943652997323397,
            "unit": "ns/iter",
            "extra": "iterations: 27205369\ncpu: 26.94301955617642 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 25.605855814811374,
            "unit": "ns/iter",
            "extra": "iterations: 27053417\ncpu: 25.604047429572425 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 27.036904680562625,
            "unit": "ns/iter",
            "extra": "iterations: 26461088\ncpu: 27.036171150634495 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 26.465470698611902,
            "unit": "ns/iter",
            "extra": "iterations: 27307474\ncpu: 26.462907737273678 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 26.256393877338322,
            "unit": "ns/iter",
            "extra": "iterations: 25118053\ncpu: 26.256203416721977 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 26.096200021278616,
            "unit": "ns/iter",
            "extra": "iterations: 27124994\ncpu: 26.093254582839634 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 28.249824244902086,
            "unit": "ns/iter",
            "extra": "iterations: 24320774\ncpu: 28.249084177995528 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 29.150804648127327,
            "unit": "ns/iter",
            "extra": "iterations: 24110477\ncpu: 29.14790711108687 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 32.994762757369315,
            "unit": "ns/iter",
            "extra": "iterations: 21675910\ncpu: 32.99236959371008 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 31.774115663796874,
            "unit": "ns/iter",
            "extra": "iterations: 22759896\ncpu: 31.770421798061033 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 34.170656263742956,
            "unit": "ns/iter",
            "extra": "iterations: 19753415\ncpu: 34.16711419265959 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 35.353004580102876,
            "unit": "ns/iter",
            "extra": "iterations: 20029471\ncpu: 35.35128691117193 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 46.54979436904727,
            "unit": "ns/iter",
            "extra": "iterations: 13310009\ncpu: 46.545542906845135 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 36.68541960615588,
            "unit": "ns/iter",
            "extra": "iterations: 19077187\ncpu: 36.68352755571337 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 38.51596739706076,
            "unit": "ns/iter",
            "extra": "iterations: 18611205\ncpu: 38.51239288374947 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 37.73301980160554,
            "unit": "ns/iter",
            "extra": "iterations: 18462991\ncpu: 37.73146414901051 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 39.10531850141575,
            "unit": "ns/iter",
            "extra": "iterations: 18748928\ncpu: 39.09953641082853 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 38.77569584770565,
            "unit": "ns/iter",
            "extra": "iterations: 18679195\ncpu: 38.77094141369587 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 37.459833588802184,
            "unit": "ns/iter",
            "extra": "iterations: 18376648\ncpu: 37.456399175736436 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 38.73272126516764,
            "unit": "ns/iter",
            "extra": "iterations: 18297853\ncpu: 38.730889192300225 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 39.18084196058435,
            "unit": "ns/iter",
            "extra": "iterations: 17341263\ncpu: 39.17878432499388 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 40.241226634818695,
            "unit": "ns/iter",
            "extra": "iterations: 17252274\ncpu: 40.2384592894823 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 44.70293241698219,
            "unit": "ns/iter",
            "extra": "iterations: 15853066\ncpu: 44.70206053516721 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 46.364475146057615,
            "unit": "ns/iter",
            "extra": "iterations: 15259131\ncpu: 46.35683493378494 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 43.16716351564891,
            "unit": "ns/iter",
            "extra": "iterations: 16026667\ncpu: 43.16195382358676 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 48.912378418924405,
            "unit": "ns/iter",
            "extra": "iterations: 13291132\ncpu: 48.90525569981527 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 54.88783818076136,
            "unit": "ns/iter",
            "extra": "iterations: 12881683\ncpu: 54.88709534305361 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 67.966222171342,
            "unit": "ns/iter",
            "extra": "iterations: 10813987\ncpu: 67.9557110619789 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 77.89809952946267,
            "unit": "ns/iter",
            "extra": "iterations: 9005454\ncpu: 77.89578770820418 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 280.8246080973264,
            "unit": "ns/iter",
            "extra": "iterations: 2486523\ncpu: 280.79989728628766 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1093.9508499444823,
            "unit": "ns/iter",
            "extra": "iterations: 632512\ncpu: 1093.9383632247395 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4713.421930830736,
            "unit": "ns/iter",
            "extra": "iterations: 148330\ncpu: 4713.0174138744505 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 29944.476504453884,
            "unit": "ns/iter",
            "extra": "iterations: 22749\ncpu: 29943.164842410624 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 240558.99130433679,
            "unit": "ns/iter",
            "extra": "iterations: 2990\ncpu: 240528.798996657 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1238447.764397828,
            "unit": "ns/iter",
            "extra": "iterations: 573\ncpu: 1238367.7417103124 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 6134825.990826241,
            "unit": "ns/iter",
            "extra": "iterations: 109\ncpu: 6130660.926605497 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 472.6262925603539,
            "unit": "ns/iter",
            "extra": "iterations: 1484070\ncpu: 472.5703336095991 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1928.6140026874618,
            "unit": "ns/iter",
            "extra": "iterations: 381741\ncpu: 1928.4746228463964 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 8147.477570312627,
            "unit": "ns/iter",
            "extra": "iterations: 89279\ncpu: 8146.883802462016 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 31449.18641411592,
            "unit": "ns/iter",
            "extra": "iterations: 22332\ncpu: 31444.688608274806 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 124585.01332132924,
            "unit": "ns/iter",
            "extra": "iterations: 5555\ncpu: 124573.4738073811 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 536930.0558375417,
            "unit": "ns/iter",
            "extra": "iterations: 1379\ncpu: 536860.2799129847 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2212459.4379085787,
            "unit": "ns/iter",
            "extra": "iterations: 306\ncpu: 2212306.3888889276 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 9056895.594200624,
            "unit": "ns/iter",
            "extra": "iterations: 69\ncpu: 9056179.217391236 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 940.5720154004758,
            "unit": "ns/iter",
            "extra": "iterations: 760747\ncpu: 940.4630632785882 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3803.6765012369387,
            "unit": "ns/iter",
            "extra": "iterations: 190010\ncpu: 3803.2551655176444 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 12787.465370010616,
            "unit": "ns/iter",
            "extra": "iterations: 56093\ncpu: 12786.642789652768 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 61704.819187249144,
            "unit": "ns/iter",
            "extra": "iterations: 11935\ncpu: 61697.109677419656 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 241073.00503233323,
            "unit": "ns/iter",
            "extra": "iterations: 2782\ncpu: 241063.85406182517 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 975043.8486112973,
            "unit": "ns/iter",
            "extra": "iterations: 720\ncpu: 974928.2444444458 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4031142.6988637326,
            "unit": "ns/iter",
            "extra": "iterations: 176\ncpu: 4030924.414772728 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 17067623.474997617,
            "unit": "ns/iter",
            "extra": "iterations: 40\ncpu: 17066807.02500023 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1732.7380489387465,
            "unit": "ns/iter",
            "extra": "iterations: 422201\ncpu: 1732.5234994706304 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 137.11067197855172,
            "unit": "ns/iter",
            "extra": "iterations: 5107038\ncpu: 137.1009254287909 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 18.595089486808476,
            "unit": "ns/iter",
            "extra": "iterations: 37231913\ncpu: 18.592341548499128 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 76.99872402525048,
            "unit": "ns/iter",
            "extra": "iterations: 8154550\ncpu: 76.99478794047556 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.4493283198197971,
            "unit": "ms/iter",
            "extra": "iterations: 1554\ncpu: 0.449285930501928 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.39766058199536014,
            "unit": "ms/iter",
            "extra": "iterations: 1744\ncpu: 0.39763010665138293 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 235.18586259997392,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 235.16618319999907 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 239.5028037999964,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 239.4857938000001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 758.5206521111668,
            "unit": "ns/iter",
            "extra": "iterations: 1004982\ncpu: 758.4374217647771 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 44167.61857151689,
            "unit": "ns/iter",
            "extra": "iterations: 16003\ncpu: 44164.521527214405 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 17.25284078802748,
            "unit": "ns/iter",
            "extra": "iterations: 41886529\ncpu: 17.252645331390543 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 80.87857726509178,
            "unit": "ns/iter",
            "extra": "iterations: 8549618\ncpu: 80.8711754139177 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 189.43574325945042,
            "unit": "ns/iter",
            "extra": "iterations: 3805663\ncpu: 189.36720592443473 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.186228977168966,
            "unit": "ms/iter",
            "extra": "iterations: 219\ncpu: 3.186003278538843 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 24.53236889654959,
            "unit": "ms/iter",
            "extra": "iterations: 29\ncpu: 24.530782275861945 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2336.5590427999905,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2332.4538727999993 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2346.3949021999724,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2345.1312605999988 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 3.0034268577405423,
            "unit": "ms/iter",
            "extra": "iterations: 239\ncpu: 3.0026136569037605 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 43.399420230715016,
            "unit": "ns/iter",
            "extra": "iterations: 15516172\ncpu: 43.39622221254114 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 145.99019827412684,
            "unit": "ns/iter",
            "extra": "iterations: 4965146\ncpu: 145.98284703007621 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4643816651525063,
            "unit": "ms/iter",
            "extra": "iterations: 1541\ncpu: 0.46436086632057566 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 221.03349160001926,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 221.00519840000175 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 207.7319376000105,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 207.72093420000033 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.3694763606868938,
            "unit": "ms/iter",
            "extra": "iterations: 524\ncpu: 1.3693987786259623 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 147.1889600175002,
            "unit": "ns/iter",
            "extra": "iterations: 4279572\ncpu: 147.18526081580126 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 46.651600328454236,
            "unit": "ns/iter",
            "extra": "iterations: 14805898\ncpu: 46.647526006190155 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 583.2529761755858,
            "unit": "ns/iter",
            "extra": "iterations: 1195326\ncpu: 583.1865440892246 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1778.5639285999878,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1778.434533799998 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 47.93090373332234,
            "unit": "ms/iter",
            "extra": "iterations: 15\ncpu: 47.928172733332985 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1840.0567449999926,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1839.960862800001 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "56431339+StephenSmith25@users.noreply.github.com",
            "name": "StephenSmith25",
            "username": "StephenSmith25"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "90aa0cc5b794f28f437a7153f95ea7a4ad91dfb4",
          "message": "Merge pull request #679 from StephenSmith25/clean_up_GR_cells_impl\n\nSmall bug fix in parallel computations + Clean up parallel GR implementation",
          "timestamp": "2021-05-11T18:20:23+01:00",
          "tree_id": "4ded5c97dd6b0d7834bdd78efc0ed79f5d0435ab",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/90aa0cc5b794f28f437a7153f95ea7a4ad91dfb4"
        },
        "date": 1620755039186,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 18.077446600000258,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 18.068982 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 16.429629599997497,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 16.415860400000003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 412.10602179999114,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 412.0362177999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 773.3111680000093,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 773.212474 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1638.9488778000214,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1638.244746 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 3989.258792400005,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3988.916242400001 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.891037546592191,
            "unit": "ns/iter",
            "extra": "iterations: 76937181\ncpu: 8.890095076397403 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.690725860272302,
            "unit": "ns/iter",
            "extra": "iterations: 80365481\ncpu: 8.68971506560133 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 20.2063474673181,
            "unit": "ns/iter",
            "extra": "iterations: 37002916\ncpu: 20.205114645559206 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 17.9443619775255,
            "unit": "ns/iter",
            "extra": "iterations: 40094865\ncpu: 17.942129247722793 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 13.292620575962813,
            "unit": "ns/iter",
            "extra": "iterations: 53719599\ncpu: 13.292015806000283 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 15.470315658396755,
            "unit": "ns/iter",
            "extra": "iterations: 45597710\ncpu: 15.469061275226368 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.389890597259077,
            "unit": "ns/iter",
            "extra": "iterations: 52426658\ncpu: 13.389178593073868 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.230427847313159,
            "unit": "ns/iter",
            "extra": "iterations: 53098873\ncpu: 13.229496283282682 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.256036098529693,
            "unit": "ns/iter",
            "extra": "iterations: 51062909\ncpu: 13.255033257114247 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.560862611947178,
            "unit": "ns/iter",
            "extra": "iterations: 52934115\ncpu: 13.555326446848156 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.756247648921565,
            "unit": "ns/iter",
            "extra": "iterations: 50819797\ncpu: 13.754255610269437 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.724009148629566,
            "unit": "ns/iter",
            "extra": "iterations: 50950149\ncpu: 13.722316592243834 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 14.91205256963581,
            "unit": "ns/iter",
            "extra": "iterations: 45528789\ncpu: 14.910364846295392 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 17.19473341873248,
            "unit": "ns/iter",
            "extra": "iterations: 41248960\ncpu: 17.194227151423874 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 18.395455793711676,
            "unit": "ns/iter",
            "extra": "iterations: 37408601\ncpu: 18.39352580974629 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 17.92406181583701,
            "unit": "ns/iter",
            "extra": "iterations: 38613341\ncpu: 17.92307516202745 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 18.366353019474335,
            "unit": "ns/iter",
            "extra": "iterations: 36384406\ncpu: 18.36475274599775 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 26.484271952459824,
            "unit": "ns/iter",
            "extra": "iterations: 28096558\ncpu: 26.481389108231777 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 35.218228831797504,
            "unit": "ns/iter",
            "extra": "iterations: 21110327\ncpu: 35.21381198879583 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 26.434123341570633,
            "unit": "ns/iter",
            "extra": "iterations: 25528215\ncpu: 26.43208434275556 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 26.012021715105906,
            "unit": "ns/iter",
            "extra": "iterations: 26638961\ncpu: 26.009329305298493 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 26.334233815383584,
            "unit": "ns/iter",
            "extra": "iterations: 25141349\ncpu: 26.33319305976775 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 27.009422790909287,
            "unit": "ns/iter",
            "extra": "iterations: 25655244\ncpu: 27.005301801066405 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 26.48598397689846,
            "unit": "ns/iter",
            "extra": "iterations: 26873065\ncpu: 26.484326406384856 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 26.053055851103732,
            "unit": "ns/iter",
            "extra": "iterations: 26477419\ncpu: 26.04527669407659 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 27.13116935034083,
            "unit": "ns/iter",
            "extra": "iterations: 26538471\ncpu: 27.12980744067734 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 26.988257359869802,
            "unit": "ns/iter",
            "extra": "iterations: 26756930\ncpu: 26.985294800263066 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 27.61070210324234,
            "unit": "ns/iter",
            "extra": "iterations: 25257596\ncpu: 27.60724646953719 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 29.57473774679424,
            "unit": "ns/iter",
            "extra": "iterations: 23542896\ncpu: 29.571431271667056 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 31.93504728005945,
            "unit": "ns/iter",
            "extra": "iterations: 21735487\ncpu: 31.9328814670684 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 31.350447349451024,
            "unit": "ns/iter",
            "extra": "iterations: 21337793\ncpu: 31.346096571467886 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 34.83593429301333,
            "unit": "ns/iter",
            "extra": "iterations: 20114819\ncpu: 34.833754705921116 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 45.818919091073184,
            "unit": "ns/iter",
            "extra": "iterations: 12842784\ncpu: 45.811062383358745 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 61.813092361642184,
            "unit": "ns/iter",
            "extra": "iterations: 11536238\ncpu: 61.809448019362634 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 38.61614290783397,
            "unit": "ns/iter",
            "extra": "iterations: 18419928\ncpu: 38.56491523745396 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 38.23333788027825,
            "unit": "ns/iter",
            "extra": "iterations: 17726196\ncpu: 38.22986099217285 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 37.11666395851405,
            "unit": "ns/iter",
            "extra": "iterations: 18641267\ncpu: 37.11172861801774 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 38.515637938924534,
            "unit": "ns/iter",
            "extra": "iterations: 18293875\ncpu: 38.51257724238303 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 38.782036306563406,
            "unit": "ns/iter",
            "extra": "iterations: 18210097\ncpu: 38.780330110268345 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 37.699824671779524,
            "unit": "ns/iter",
            "extra": "iterations: 18210987\ncpu: 37.69626802764753 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 38.48323643365356,
            "unit": "ns/iter",
            "extra": "iterations: 18037212\ncpu: 38.480953431162035 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 37.98044787314582,
            "unit": "ns/iter",
            "extra": "iterations: 18352786\ncpu: 37.976451095762755 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 39.25848543885283,
            "unit": "ns/iter",
            "extra": "iterations: 18136039\ncpu: 39.25526433859129 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 42.548091810712506,
            "unit": "ns/iter",
            "extra": "iterations: 16652960\ncpu: 42.54631476926633 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 43.62871315344023,
            "unit": "ns/iter",
            "extra": "iterations: 16141334\ncpu: 43.62688226388199 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 43.66778660155231,
            "unit": "ns/iter",
            "extra": "iterations: 15405360\ncpu: 43.664293531602 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 52.81301633222129,
            "unit": "ns/iter",
            "extra": "iterations: 13350658\ncpu: 52.80827199678122 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 60.007096624068865,
            "unit": "ns/iter",
            "extra": "iterations: 10950418\ncpu: 60.00255031360463 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 75.58256708217908,
            "unit": "ns/iter",
            "extra": "iterations: 9317006\ncpu: 75.57823113991749 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 77.86721805353464,
            "unit": "ns/iter",
            "extra": "iterations: 9149414\ncpu: 77.8627020265983 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 287.1807265552452,
            "unit": "ns/iter",
            "extra": "iterations: 2449972\ncpu: 287.1467131052945 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1123.8598921141206,
            "unit": "ns/iter",
            "extra": "iterations: 640121\ncpu: 1123.7153319450547 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4856.923610043792,
            "unit": "ns/iter",
            "extra": "iterations: 136641\ncpu: 4856.688439048306 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 27958.5558239034,
            "unit": "ns/iter",
            "extra": "iterations: 25258\ncpu: 27955.71886135114 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 274241.2321857801,
            "unit": "ns/iter",
            "extra": "iterations: 2498\ncpu: 274224.22978382854 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1450543.6316832658,
            "unit": "ns/iter",
            "extra": "iterations: 505\ncpu: 1450180.8059405915 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 9782443.309859445,
            "unit": "ns/iter",
            "extra": "iterations: 71\ncpu: 9781692.309859304 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 451.7911667781852,
            "unit": "ns/iter",
            "extra": "iterations: 1567016\ncpu: 451.771295889769 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1824.6353607870542,
            "unit": "ns/iter",
            "extra": "iterations: 383453\ncpu: 1824.49258448883 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7352.189097261975,
            "unit": "ns/iter",
            "extra": "iterations: 97315\ncpu: 7351.48678004419 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 29689.77192394315,
            "unit": "ns/iter",
            "extra": "iterations: 23878\ncpu: 29687.72037021527 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 118603.62079307581,
            "unit": "ns/iter",
            "extra": "iterations: 6002\ncpu: 118593.76707763852 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 592231.9087102714,
            "unit": "ns/iter",
            "extra": "iterations: 1194\ncpu: 592185.7989949774 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2368644.4206901044,
            "unit": "ns/iter",
            "extra": "iterations: 290\ncpu: 2368214.2103448035 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 11186696.296878295,
            "unit": "ns/iter",
            "extra": "iterations: 64\ncpu: 11185888.48437496 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 975.0039181506255,
            "unit": "ns/iter",
            "extra": "iterations: 713602\ncpu: 974.892787576271 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3938.099089206858,
            "unit": "ns/iter",
            "extra": "iterations: 176659\ncpu: 3937.8669866805694 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 13187.546899090388,
            "unit": "ns/iter",
            "extra": "iterations: 53178\ncpu: 13186.821373500268 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 62055.935944890974,
            "unit": "ns/iter",
            "extra": "iterations: 11615\ncpu: 62052.841756350244 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 252531.25713211932,
            "unit": "ns/iter",
            "extra": "iterations: 2664\ncpu: 252485.23686186344 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 1060298.4901367242,
            "unit": "ns/iter",
            "extra": "iterations: 659\ncpu: 1060207.8770864937 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4442504.389937667,
            "unit": "ns/iter",
            "extra": "iterations: 159\ncpu: 4442208.018867938 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 19707521.39999864,
            "unit": "ns/iter",
            "extra": "iterations: 35\ncpu: 19705893.51428568 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1814.6979447695999,
            "unit": "ns/iter",
            "extra": "iterations: 389932\ncpu: 1814.5886795646554 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 166.17456156690824,
            "unit": "ns/iter",
            "extra": "iterations: 4347870\ncpu: 166.15726045166676 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 18.172837498670944,
            "unit": "ns/iter",
            "extra": "iterations: 38183965\ncpu: 18.17118824616574 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 77.58062756440002,
            "unit": "ns/iter",
            "extra": "iterations: 8861529\ncpu: 77.57084234560472 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.48649792592590935,
            "unit": "ms/iter",
            "extra": "iterations: 1458\ncpu: 0.48644570164609263 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.42156947857564264,
            "unit": "ms/iter",
            "extra": "iterations: 1657\ncpu: 0.4215424248642101 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 261.32227879998027,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 261.2958931999998 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 262.33568459997514,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 262.31946180000136 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 788.3741683892661,
            "unit": "ns/iter",
            "extra": "iterations: 893146\ncpu: 788.261982923287 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 48943.26428425919,
            "unit": "ns/iter",
            "extra": "iterations: 14719\ncpu: 48940.43725796565 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 18.064021128700524,
            "unit": "ns/iter",
            "extra": "iterations: 39607924\ncpu: 18.063348788489527 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 75.97345762337343,
            "unit": "ns/iter",
            "extra": "iterations: 9476544\ncpu: 75.96385201187175 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 191.79926229649624,
            "unit": "ns/iter",
            "extra": "iterations: 3820234\ncpu: 191.77287045767252 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.628625326530023,
            "unit": "ms/iter",
            "extra": "iterations: 196\ncpu: 3.6282564948980474 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 26.353993346149835,
            "unit": "ms/iter",
            "extra": "iterations: 26\ncpu: 26.35113096153838 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2568.1169110000155,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2567.896960400003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2588.179912800024,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2587.9637167999986 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.8297451480002564,
            "unit": "ms/iter",
            "extra": "iterations: 250\ncpu: 2.829339848000018 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 38.596660983101444,
            "unit": "ns/iter",
            "extra": "iterations: 17738874\ncpu: 38.594417886952286 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 125.78274232992747,
            "unit": "ns/iter",
            "extra": "iterations: 5669595\ncpu: 125.7391048214201 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4703615439999946,
            "unit": "ms/iter",
            "extra": "iterations: 1500\ncpu: 0.470306407999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 214.19579919997886,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 214.183491 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 218.38334079998276,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 218.3631691999949 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.3844290059996638,
            "unit": "ms/iter",
            "extra": "iterations: 500\ncpu: 1.3843139940000242 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 124.56298567926916,
            "unit": "ns/iter",
            "extra": "iterations: 5642870\ncpu: 124.55439891402985 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 38.72193432840062,
            "unit": "ns/iter",
            "extra": "iterations: 16850541\ncpu: 38.716941135598454 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 590.7157506469774,
            "unit": "ns/iter",
            "extra": "iterations: 1183672\ncpu: 590.6806809656744 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1974.6343574000093,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1974.4530230000012 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 48.17132873333018,
            "unit": "ms/iter",
            "extra": "iterations: 15\ncpu: 48.16488073333289 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2020.9105716000067,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2020.7368922 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "ed5f4ad7acb46b72d3bd4f1e081617cb7bf274b4",
          "message": "Merge pull request #657 from disorderedmaterials/remove-array-1\n\nRemove Array 1",
          "timestamp": "2021-05-12T13:48:14+01:00",
          "tree_id": "5a53ffa2597f0410f89dcfda7462b9bf038f4765",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/ed5f4ad7acb46b72d3bd4f1e081617cb7bf274b4"
        },
        "date": 1620825037302,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 16.46843160001481,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 16.4599984 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 15.579145400033667,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 15.579063000000005 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 398.4183777999988,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 398.383821 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 756.0632327999883,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 756.0176596 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1576.6410184000051,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1576.5486778 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 3795.4572148000357,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3795.2422346 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.625295987912562,
            "unit": "ns/iter",
            "extra": "iterations: 80882526\ncpu: 8.624667112894068 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.454079074086522,
            "unit": "ns/iter",
            "extra": "iterations: 82352390\ncpu: 8.453759909093076 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 17.254777754006422,
            "unit": "ns/iter",
            "extra": "iterations: 36217750\ncpu: 17.25435014599186 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 17.377592988613078,
            "unit": "ns/iter",
            "extra": "iterations: 38398115\ncpu: 17.37711132434483 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 13.264108690959322,
            "unit": "ns/iter",
            "extra": "iterations: 54126159\ncpu: 13.26371677325192 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 13.060727031815928,
            "unit": "ns/iter",
            "extra": "iterations: 53705490\ncpu: 13.059300957872264 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.025545219391374,
            "unit": "ns/iter",
            "extra": "iterations: 53716587\ncpu: 13.02449195068927 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.604188229788686,
            "unit": "ns/iter",
            "extra": "iterations: 51976279\ncpu: 13.603478790776906 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.029241663357967,
            "unit": "ns/iter",
            "extra": "iterations: 54041830\ncpu: 13.028829371618155 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.210981259597308,
            "unit": "ns/iter",
            "extra": "iterations: 52794490\ncpu: 13.210199189347136 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.292378307208821,
            "unit": "ns/iter",
            "extra": "iterations: 53522808\ncpu: 13.29143112222365 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.246585852961388,
            "unit": "ns/iter",
            "extra": "iterations: 53656740\ncpu: 13.246202080111475 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 14.91444557884352,
            "unit": "ns/iter",
            "extra": "iterations: 46710970\ncpu: 14.913315587323458 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 15.958337412836299,
            "unit": "ns/iter",
            "extra": "iterations: 42672698\ncpu: 15.95730668353802 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 17.152534801663297,
            "unit": "ns/iter",
            "extra": "iterations: 40832603\ncpu: 17.150137697564926 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 16.396519168924304,
            "unit": "ns/iter",
            "extra": "iterations: 41410283\ncpu: 16.395561194305348 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 17.511660999037737,
            "unit": "ns/iter",
            "extra": "iterations: 41474148\ncpu: 17.510284454788437 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 17.209760489155855,
            "unit": "ns/iter",
            "extra": "iterations: 40393160\ncpu: 17.208455862329263 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 17.71467288925042,
            "unit": "ns/iter",
            "extra": "iterations: 37469955\ncpu: 17.713712786684727 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 25.319892490105023,
            "unit": "ns/iter",
            "extra": "iterations: 27690475\ncpu: 25.31881782454079 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 25.02995471548271,
            "unit": "ns/iter",
            "extra": "iterations: 27764677\ncpu: 25.02891393262037 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 25.714308092660076,
            "unit": "ns/iter",
            "extra": "iterations: 27590414\ncpu: 25.713366243797648 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 24.988678876120012,
            "unit": "ns/iter",
            "extra": "iterations: 28049512\ncpu: 24.987274858828403 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 25.40106401794588,
            "unit": "ns/iter",
            "extra": "iterations: 27596527\ncpu: 25.40015249020301 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 25.07408111012516,
            "unit": "ns/iter",
            "extra": "iterations: 27814432\ncpu: 25.073172085628098 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 25.510295011008456,
            "unit": "ns/iter",
            "extra": "iterations: 27569276\ncpu: 25.509047390290434 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 25.32424206966982,
            "unit": "ns/iter",
            "extra": "iterations: 27655708\ncpu: 25.32221445930805 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 26.69003349724383,
            "unit": "ns/iter",
            "extra": "iterations: 25472546\ncpu: 26.688181542590865 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 29.034832115568257,
            "unit": "ns/iter",
            "extra": "iterations: 24460903\ncpu: 29.033815718086828 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 29.949672460232588,
            "unit": "ns/iter",
            "extra": "iterations: 21900852\ncpu: 29.94857779962136 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 30.891981889956682,
            "unit": "ns/iter",
            "extra": "iterations: 23274375\ncpu: 30.890658846908252 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 30.721598751099872,
            "unit": "ns/iter",
            "extra": "iterations: 22151966\ncpu: 30.71974767386366 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 30.3321088338597,
            "unit": "ns/iter",
            "extra": "iterations: 22802242\ncpu: 30.329512729493604 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 32.64144209562656,
            "unit": "ns/iter",
            "extra": "iterations: 21574242\ncpu: 32.63941657834391 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 36.90683384609147,
            "unit": "ns/iter",
            "extra": "iterations: 18999228\ncpu: 36.90523872864704 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 36.98282987018597,
            "unit": "ns/iter",
            "extra": "iterations: 18897411\ncpu: 36.980278409566196 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 36.65586533337858,
            "unit": "ns/iter",
            "extra": "iterations: 19302185\ncpu: 36.65392809156078 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 36.818289931142964,
            "unit": "ns/iter",
            "extra": "iterations: 18807769\ncpu: 36.8162535386303 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 37.34620135640344,
            "unit": "ns/iter",
            "extra": "iterations: 18963519\ncpu: 37.34468802968492 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 36.37801293838122,
            "unit": "ns/iter",
            "extra": "iterations: 19030965\ncpu: 36.37564842350335 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 36.976581472959964,
            "unit": "ns/iter",
            "extra": "iterations: 19027499\ncpu: 36.97530311261607 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 37.47474462476965,
            "unit": "ns/iter",
            "extra": "iterations: 18780893\ncpu: 37.473173932677376 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 37.93484143388848,
            "unit": "ns/iter",
            "extra": "iterations: 18431176\ncpu: 37.93306238299715 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 41.534899229171835,
            "unit": "ns/iter",
            "extra": "iterations: 17053199\ncpu: 41.53299935103135 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 42.46247322839788,
            "unit": "ns/iter",
            "extra": "iterations: 16464739\ncpu: 42.45767588541834 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 42.34429391567708,
            "unit": "ns/iter",
            "extra": "iterations: 16164500\ncpu: 42.34173918153971 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 43.08350856421268,
            "unit": "ns/iter",
            "extra": "iterations: 16605710\ncpu: 43.08178650596752 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 43.46415072793,
            "unit": "ns/iter",
            "extra": "iterations: 16007703\ncpu: 43.46160870176034 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 45.01951307054361,
            "unit": "ns/iter",
            "extra": "iterations: 12821816\ncpu: 45.01440443381758 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 95.93947936281866,
            "unit": "ns/iter",
            "extra": "iterations: 7080593\ncpu: 95.9363268020062 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 328.1680932217316,
            "unit": "ns/iter",
            "extra": "iterations: 2117105\ncpu: 328.15841443858204 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1190.1804564539832,
            "unit": "ns/iter",
            "extra": "iterations: 580212\ncpu: 1190.148416440882 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4516.560853843032,
            "unit": "ns/iter",
            "extra": "iterations: 154033\ncpu: 4516.400511578685 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 28889.83795693591,
            "unit": "ns/iter",
            "extra": "iterations: 23827\ncpu: 28889.335291895637 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 273167.0933125496,
            "unit": "ns/iter",
            "extra": "iterations: 2572\ncpu: 273149.3573094842 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1347452.9621511777,
            "unit": "ns/iter",
            "extra": "iterations: 502\ncpu: 1347355.9621513754 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 5918798.758622071,
            "unit": "ns/iter",
            "extra": "iterations: 116\ncpu: 5918451.275862018 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 432.5487025153461,
            "unit": "ns/iter",
            "extra": "iterations: 1582639\ncpu: 432.53442636002234 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1714.797927939663,
            "unit": "ns/iter",
            "extra": "iterations: 411185\ncpu: 1714.7301457981148 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7156.202226575564,
            "unit": "ns/iter",
            "extra": "iterations: 96471\ncpu: 7155.805433757347 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 28908.196352809224,
            "unit": "ns/iter",
            "extra": "iterations: 24512\ncpu: 28906.638177219505 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 115503.23575212923,
            "unit": "ns/iter",
            "extra": "iterations: 6036\ncpu: 115500.985752152 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 567709.4592933247,
            "unit": "ns/iter",
            "extra": "iterations: 1302\ncpu: 567700.1497695826 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2478907.41463351,
            "unit": "ns/iter",
            "extra": "iterations: 287\ncpu: 2478859.6933798115 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 9100159.067567827,
            "unit": "ns/iter",
            "extra": "iterations: 74\ncpu: 9099532.418918986 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 906.4466004811698,
            "unit": "ns/iter",
            "extra": "iterations: 771065\ncpu: 906.3930628416416 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3622.35833315859,
            "unit": "ns/iter",
            "extra": "iterations: 190228\ncpu: 3622.157542527922 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 12486.202445507735,
            "unit": "ns/iter",
            "extra": "iterations: 57166\ncpu: 12485.536455235619 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 57104.43543204716,
            "unit": "ns/iter",
            "extra": "iterations: 12452\ncpu: 57100.16358817812 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 232275.94794430563,
            "unit": "ns/iter",
            "extra": "iterations: 3016\ncpu: 232258.47314323584 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 1018762.8151382526,
            "unit": "ns/iter",
            "extra": "iterations: 687\ncpu: 1018707.6695778692 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4341886.374999149,
            "unit": "ns/iter",
            "extra": "iterations: 160\ncpu: 4341569.637499986 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 17600114.43589531,
            "unit": "ns/iter",
            "extra": "iterations: 39\ncpu: 17599081.051282056 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1705.8014221832334,
            "unit": "ns/iter",
            "extra": "iterations: 401636\ncpu: 1705.6816819209412 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 150.4006879566617,
            "unit": "ns/iter",
            "extra": "iterations: 4635757\ncpu: 150.3907879554514 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 17.307047112370707,
            "unit": "ns/iter",
            "extra": "iterations: 40913907\ncpu: 17.30607712433827 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 75.55465127858145,
            "unit": "ns/iter",
            "extra": "iterations: 9458754\ncpu: 75.55127081220235 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.475200084393799,
            "unit": "ms/iter",
            "extra": "iterations: 1493\ncpu: 0.47517402210314436 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.4094268864425956,
            "unit": "ms/iter",
            "extra": "iterations: 1726\ncpu: 0.4093981066048639 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 254.17252900001586,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 254.15312820000227 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 257.76645539999663,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 257.75045759999955 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 813.5343602042303,
            "unit": "ns/iter",
            "extra": "iterations: 887291\ncpu: 813.3984814452086 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 47908.500033829536,
            "unit": "ns/iter",
            "extra": "iterations: 14779\ncpu: 47904.379322011046 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 16.886880854248883,
            "unit": "ns/iter",
            "extra": "iterations: 40268285\ncpu: 16.88551749844785 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 77.30243872175588,
            "unit": "ns/iter",
            "extra": "iterations: 9235289\ncpu: 77.29767590380752 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 173.74154021959606,
            "unit": "ns/iter",
            "extra": "iterations: 4060330\ncpu: 173.7344513869558 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.5050252722770225,
            "unit": "ms/iter",
            "extra": "iterations: 202\ncpu: 3.504682202970339 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 25.760849821429993,
            "unit": "ms/iter",
            "extra": "iterations: 28\ncpu: 25.758778107143396 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2445.169447000035,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2444.4834045999983 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2457.616219600004,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2457.405900800001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.7492543100774225,
            "unit": "ms/iter",
            "extra": "iterations: 258\ncpu: 2.7491736937984714 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 35.166441514093556,
            "unit": "ns/iter",
            "extra": "iterations: 19909462\ncpu: 35.16381452195858 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 119.7886780234138,
            "unit": "ns/iter",
            "extra": "iterations: 5417146\ncpu: 119.77990975322035 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4435239589387302,
            "unit": "ms/iter",
            "extra": "iterations: 1583\ncpu: 0.443512019583067 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 207.98054779997983,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 207.9652278000026 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 208.35638220000874,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 208.3450426000013 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.3265465536396106,
            "unit": "ms/iter",
            "extra": "iterations: 522\ncpu: 1.3264841015326083 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 122.49099248657349,
            "unit": "ns/iter",
            "extra": "iterations: 5773791\ncpu: 122.44961585897195 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 35.46011185535309,
            "unit": "ns/iter",
            "extra": "iterations: 19560085\ncpu: 35.45724382077122 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 570.3541075501522,
            "unit": "ns/iter",
            "extra": "iterations: 1236613\ncpu: 570.3262904401096 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1899.3286502000046,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1899.0691701999992 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 46.93475580000571,
            "unit": "ms/iter",
            "extra": "iterations: 15\ncpu: 46.92919573333446 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1949.4846624000274,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1948.8122024000008 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "distinct": true,
          "id": "7a1720cc04ed5184daf123926308eeed8dd1aebb",
          "message": "Generalise curl opts and try to fix tagging.",
          "timestamp": "2021-05-12T16:43:36+01:00",
          "tree_id": "fc9cbc265a7de6bfbc150cf46ed1b6a6b1ad014d",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/7a1720cc04ed5184daf123926308eeed8dd1aebb"
        },
        "date": 1620835552922,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 15.549573199996303,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 15.5384498 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 14.731957799995143,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 14.728155200000003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 370.0256101999912,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 369.7374988 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 703.1132718000208,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 702.9999237999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1470.1472319999993,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1469.9793479999998 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 3493.986108399986,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3492.9756448000007 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.252129679450912,
            "unit": "ns/iter",
            "extra": "iterations: 87124027\ncpu: 8.251466188540654 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 7.624810134490526,
            "unit": "ns/iter",
            "extra": "iterations: 91420633\ncpu: 7.624261089944615 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 16.43759589076497,
            "unit": "ns/iter",
            "extra": "iterations: 43706633\ncpu: 16.43567153754447 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 17.972348664833998,
            "unit": "ns/iter",
            "extra": "iterations: 37182436\ncpu: 17.94832388066242 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 12.179412080831467,
            "unit": "ns/iter",
            "extra": "iterations: 58548967\ncpu: 12.177958357489073 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 12.274255570280014,
            "unit": "ns/iter",
            "extra": "iterations: 60140835\ncpu: 12.272991720184821 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 12.277103587777324,
            "unit": "ns/iter",
            "extra": "iterations: 55778704\ncpu: 12.276927947268204 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 12.892917272081364,
            "unit": "ns/iter",
            "extra": "iterations: 60106855\ncpu: 12.891529659969718 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 12.731523682766063,
            "unit": "ns/iter",
            "extra": "iterations: 54197056\ncpu: 12.731110560691741 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 12.67790621632485,
            "unit": "ns/iter",
            "extra": "iterations: 56040116\ncpu: 12.676180595343405 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 12.506363882436553,
            "unit": "ns/iter",
            "extra": "iterations: 55722981\ncpu: 12.503670290001114 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 12.114014491032174,
            "unit": "ns/iter",
            "extra": "iterations: 60153608\ncpu: 12.112935619755321 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 12.659585837199726,
            "unit": "ns/iter",
            "extra": "iterations: 56206786\ncpu: 12.658384879007349 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 15.208042530701118,
            "unit": "ns/iter",
            "extra": "iterations: 45069371\ncpu: 15.20726133497635 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 16.021894827922548,
            "unit": "ns/iter",
            "extra": "iterations: 40572824\ncpu: 16.02054155757068 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 16.49405664924223,
            "unit": "ns/iter",
            "extra": "iterations: 40139142\ncpu: 16.493864990935748 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 17.041240800322477,
            "unit": "ns/iter",
            "extra": "iterations: 43575585\ncpu: 17.039136594494376 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 17.456587901626584,
            "unit": "ns/iter",
            "extra": "iterations: 39958930\ncpu: 17.453266816704094 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 20.390155301625473,
            "unit": "ns/iter",
            "extra": "iterations: 31616733\ncpu: 20.388161515612474 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 24.210819891295724,
            "unit": "ns/iter",
            "extra": "iterations: 28833432\ncpu: 24.210569799668576 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 23.107224623820628,
            "unit": "ns/iter",
            "extra": "iterations: 29840571\ncpu: 23.105482364931987 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 24.04880303896812,
            "unit": "ns/iter",
            "extra": "iterations: 29954651\ncpu: 24.04724074401673 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 23.502942707044085,
            "unit": "ns/iter",
            "extra": "iterations: 29577528\ncpu: 23.500376400624276 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 23.257787952445746,
            "unit": "ns/iter",
            "extra": "iterations: 28647196\ncpu: 23.256115537450977 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 23.437914386271835,
            "unit": "ns/iter",
            "extra": "iterations: 29902354\ncpu: 23.43417571740332 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 24.276108026794954,
            "unit": "ns/iter",
            "extra": "iterations: 27612667\ncpu: 24.274152909604744 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 24.124298438976314,
            "unit": "ns/iter",
            "extra": "iterations: 27419033\ncpu: 24.12199467428328 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 25.489710370590487,
            "unit": "ns/iter",
            "extra": "iterations: 27961940\ncpu: 25.484865105926065 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 27.398142223582667,
            "unit": "ns/iter",
            "extra": "iterations: 24901059\ncpu: 27.394827183856165 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 30.584426762447784,
            "unit": "ns/iter",
            "extra": "iterations: 23427216\ncpu: 30.58192531284978 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 30.29689458595567,
            "unit": "ns/iter",
            "extra": "iterations: 22110868\ncpu: 30.292784073425018 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 28.889152980550495,
            "unit": "ns/iter",
            "extra": "iterations: 22939426\ncpu: 28.885556116356387 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 31.693638030408046,
            "unit": "ns/iter",
            "extra": "iterations: 22344291\ncpu: 31.689775791051453 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 42.92405477222449,
            "unit": "ns/iter",
            "extra": "iterations: 17357483\ncpu: 42.920614123602974 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 33.624460920231066,
            "unit": "ns/iter",
            "extra": "iterations: 19748701\ncpu: 33.619766940619954 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 34.57001088091431,
            "unit": "ns/iter",
            "extra": "iterations: 20442201\ncpu: 34.5666742049933 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 34.49761251896704,
            "unit": "ns/iter",
            "extra": "iterations: 20100055\ncpu: 34.49430456782344 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 34.619130177153494,
            "unit": "ns/iter",
            "extra": "iterations: 22270604\ncpu: 34.61727091012027 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 35.023877326950995,
            "unit": "ns/iter",
            "extra": "iterations: 19161567\ncpu: 35.02320008588018 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 33.764107709955255,
            "unit": "ns/iter",
            "extra": "iterations: 19900519\ncpu: 33.76214791182112 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 34.679821830462025,
            "unit": "ns/iter",
            "extra": "iterations: 20732163\ncpu: 34.67687317526872 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 34.041471116247,
            "unit": "ns/iter",
            "extra": "iterations: 19695901\ncpu: 34.0351453838035 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 34.23258086388239,
            "unit": "ns/iter",
            "extra": "iterations: 21161612\ncpu: 34.231044922286856 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 37.95157763005488,
            "unit": "ns/iter",
            "extra": "iterations: 18449097\ncpu: 37.94856929854031 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 38.77875301742594,
            "unit": "ns/iter",
            "extra": "iterations: 17815678\ncpu: 38.774307887692984 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 41.853129064968,
            "unit": "ns/iter",
            "extra": "iterations: 16624535\ncpu: 41.84799815453493 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 42.70676342337409,
            "unit": "ns/iter",
            "extra": "iterations: 12905358\ncpu: 42.7035616524545 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 47.232269803110356,
            "unit": "ns/iter",
            "extra": "iterations: 15803406\ncpu: 47.226605517822826 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 55.93860891564836,
            "unit": "ns/iter",
            "extra": "iterations: 12498020\ncpu: 55.9326931785999 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 78.71290113696578,
            "unit": "ns/iter",
            "extra": "iterations: 8983247\ncpu: 78.70481564182701 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 269.54062829753684,
            "unit": "ns/iter",
            "extra": "iterations: 2630728\ncpu: 269.51281128266027 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 956.0367109342758,
            "unit": "ns/iter",
            "extra": "iterations: 616029\ncpu: 955.9249158724668 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4178.863274133132,
            "unit": "ns/iter",
            "extra": "iterations: 159743\ncpu: 4178.1716945343815 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 25428.687412797917,
            "unit": "ns/iter",
            "extra": "iterations: 27234\ncpu: 25383.624697069765 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 264987.3421367894,
            "unit": "ns/iter",
            "extra": "iterations: 2499\ncpu: 264959.8631452592 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1450277.9529413388,
            "unit": "ns/iter",
            "extra": "iterations: 510\ncpu: 1450117.1019607757 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 6332688.035714438,
            "unit": "ns/iter",
            "extra": "iterations: 112\ncpu: 6331918.151785767 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 428.57139574428834,
            "unit": "ns/iter",
            "extra": "iterations: 1745076\ncpu: 428.5678497670023 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1666.4735006890182,
            "unit": "ns/iter",
            "extra": "iterations: 438785\ncpu: 1666.2382579167752 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 6850.716539934634,
            "unit": "ns/iter",
            "extra": "iterations: 108495\ncpu: 6849.743896032007 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 27250.593175241233,
            "unit": "ns/iter",
            "extra": "iterations: 25554\ncpu: 27245.919308131608 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 109584.5422402905,
            "unit": "ns/iter",
            "extra": "iterations: 6392\ncpu: 109566.64048811085 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 532548.9853958389,
            "unit": "ns/iter",
            "extra": "iterations: 1301\ncpu: 532486.3366641045 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2348752.1980830105,
            "unit": "ns/iter",
            "extra": "iterations: 313\ncpu: 2348459.1533546313 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 9351016.970587058,
            "unit": "ns/iter",
            "extra": "iterations: 68\ncpu: 9350169.94117637 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 828.917225033775,
            "unit": "ns/iter",
            "extra": "iterations: 851441\ncpu: 828.8300351991554 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3462.1495048463607,
            "unit": "ns/iter",
            "extra": "iterations: 207612\ncpu: 3461.7465512590697 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 11870.853044692085,
            "unit": "ns/iter",
            "extra": "iterations: 60236\ncpu: 11869.22478252209 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 52811.112605293034,
            "unit": "ns/iter",
            "extra": "iterations: 13534\ncpu: 52807.95950938332 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 211680.8935773872,
            "unit": "ns/iter",
            "extra": "iterations: 3223\ncpu: 211645.03785293072 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 911198.8614379881,
            "unit": "ns/iter",
            "extra": "iterations: 765\ncpu: 910926.2849673195 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4244484.230303263,
            "unit": "ns/iter",
            "extra": "iterations: 165\ncpu: 4243927.212121176 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 16972097.564103723,
            "unit": "ns/iter",
            "extra": "iterations: 39\ncpu: 16970489.92307679 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1556.954130593224,
            "unit": "ns/iter",
            "extra": "iterations: 430374\ncpu: 1556.784959128581 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 140.33921511697648,
            "unit": "ns/iter",
            "extra": "iterations: 5025233\ncpu: 140.32176418486372 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 16.471303609523986,
            "unit": "ns/iter",
            "extra": "iterations: 43175674\ncpu: 16.468700546515905 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 69.13240096895964,
            "unit": "ns/iter",
            "extra": "iterations: 10624673\ncpu: 69.12507782592472 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.46099457874271416,
            "unit": "ms/iter",
            "extra": "iterations: 1543\ncpu: 0.4609906882696033 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.37935633984802,
            "unit": "ms/iter",
            "extra": "iterations: 1842\ncpu: 0.3793010792616687 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 235.6092465999609,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 235.58598920000122 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 233.29823200001556,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 233.27326879999927 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 703.9945494201953,
            "unit": "ns/iter",
            "extra": "iterations: 978428\ncpu: 703.937626478395 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 44523.16406880922,
            "unit": "ns/iter",
            "extra": "iterations: 16103\ncpu: 44516.87368813193 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 15.463932490797644,
            "unit": "ns/iter",
            "extra": "iterations: 46277662\ncpu: 15.463183987125412 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 69.51740458889374,
            "unit": "ns/iter",
            "extra": "iterations: 9492554\ncpu: 69.50735450122241 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 154.98147628487212,
            "unit": "ns/iter",
            "extra": "iterations: 4265613\ncpu: 154.9694069762086 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.336432599033964,
            "unit": "ms/iter",
            "extra": "iterations: 207\ncpu: 3.336222864734342 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 23.185226533329722,
            "unit": "ms/iter",
            "extra": "iterations: 30\ncpu: 23.183246366666783 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2328.141869199999,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2327.9153549999987 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2359.0180472000156,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2358.0935064000014 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.5391637014923045,
            "unit": "ms/iter",
            "extra": "iterations: 268\ncpu: 2.539037597014917 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 33.369953081156424,
            "unit": "ns/iter",
            "extra": "iterations: 20643734\ncpu: 33.36858041282708 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 110.34170547029522,
            "unit": "ns/iter",
            "extra": "iterations: 6506264\ncpu: 110.33323962876605 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.40539093949961996,
            "unit": "ms/iter",
            "extra": "iterations: 1719\ncpu: 0.40536930191972825 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 193.03702679999333,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 193.01172799999904 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 194.83759679997092,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 194.82995899999764 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.2491067113043517,
            "unit": "ms/iter",
            "extra": "iterations: 575\ncpu: 1.2489989060869626 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 112.20540620838317,
            "unit": "ns/iter",
            "extra": "iterations: 6368123\ncpu: 112.19452231058007 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 33.58333298223877,
            "unit": "ns/iter",
            "extra": "iterations: 20649561\ncpu: 33.58149313682775 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 567.4895962365285,
            "unit": "ns/iter",
            "extra": "iterations: 1213359\ncpu: 567.4652860365323 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1746.4538918000017,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1746.274571800001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 43.38440743750027,
            "unit": "ms/iter",
            "extra": "iterations: 16\ncpu: 43.381001812500486 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1797.641075599995,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1797.4499557999993 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "trisyoungs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "trisyoungs",
            "username": "trisyoungs"
          },
          "distinct": true,
          "id": "9ffa29b8c9fdc9fe3014b88a0d61a1814109d431",
          "message": "Try different quoting.",
          "timestamp": "2021-05-12T20:35:55+01:00",
          "tree_id": "e05c68ce4d202da7853f803021cf3aeec464fef2",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/9ffa29b8c9fdc9fe3014b88a0d61a1814109d431"
        },
        "date": 1620849511119,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 16.625921000013477,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 16.6067626 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 13.981424800022069,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 13.9813826 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 381.7107389999819,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 381.64415840000004 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 737.7895015999911,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 737.6957654 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1521.6589396000018,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1521.4726591999997 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 3730.203748800022,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3729.7248498 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.102509067901284,
            "unit": "ns/iter",
            "extra": "iterations: 87567658\ncpu: 8.10168787430625 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 7.648498693130181,
            "unit": "ns/iter",
            "extra": "iterations: 93629026\ncpu: 7.648129138927561 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 16.783042396837118,
            "unit": "ns/iter",
            "extra": "iterations: 37112509\ncpu: 16.78152220859008 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 15.898694035218547,
            "unit": "ns/iter",
            "extra": "iterations: 36040099\ncpu: 15.8979779994501 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 12.153192502628904,
            "unit": "ns/iter",
            "extra": "iterations: 57295082\ncpu: 12.152143529526636 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 12.418258423841266,
            "unit": "ns/iter",
            "extra": "iterations: 58228312\ncpu: 12.415402510723666 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 12.228979607978088,
            "unit": "ns/iter",
            "extra": "iterations: 55739636\ncpu: 12.228224113268366 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 12.197752983081928,
            "unit": "ns/iter",
            "extra": "iterations: 57970458\ncpu: 12.196681420043271 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 12.091804001418224,
            "unit": "ns/iter",
            "extra": "iterations: 59614798\ncpu: 12.091366106784319 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 12.747893956420072,
            "unit": "ns/iter",
            "extra": "iterations: 58520394\ncpu: 12.746445093995854 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 11.875713516475118,
            "unit": "ns/iter",
            "extra": "iterations: 60605118\ncpu: 11.874372078608959 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.006419717528853,
            "unit": "ns/iter",
            "extra": "iterations: 57178684\ncpu: 13.00482125471804 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 13.49521704410287,
            "unit": "ns/iter",
            "extra": "iterations: 48720813\ncpu: 13.493570745627808 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 14.679560637927473,
            "unit": "ns/iter",
            "extra": "iterations: 47319970\ncpu: 14.678816935006566 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 15.403553915334086,
            "unit": "ns/iter",
            "extra": "iterations: 41085447\ncpu: 15.402933135910583 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 14.644401207600335,
            "unit": "ns/iter",
            "extra": "iterations: 46941962\ncpu: 14.643751554312948 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 15.793694246468993,
            "unit": "ns/iter",
            "extra": "iterations: 47012494\ncpu: 15.791988508416477 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 15.163090379813337,
            "unit": "ns/iter",
            "extra": "iterations: 47490085\ncpu: 15.161022558708753 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 17.05941274401148,
            "unit": "ns/iter",
            "extra": "iterations: 30863917\ncpu: 17.058958621486582 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 24.946937692816324,
            "unit": "ns/iter",
            "extra": "iterations: 30064995\ncpu: 24.945694120354755 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 25.3543164744958,
            "unit": "ns/iter",
            "extra": "iterations: 29666896\ncpu: 25.273563132455738 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 25.84016374642704,
            "unit": "ns/iter",
            "extra": "iterations: 28165012\ncpu: 25.838708075110986 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 24.51186190895738,
            "unit": "ns/iter",
            "extra": "iterations: 27987738\ncpu: 24.506930713729115 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 23.31266850736444,
            "unit": "ns/iter",
            "extra": "iterations: 29631494\ncpu: 23.311453820046886 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 24.1024755287235,
            "unit": "ns/iter",
            "extra": "iterations: 29680003\ncpu: 24.09888065038271 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 25.073446573184246,
            "unit": "ns/iter",
            "extra": "iterations: 28259603\ncpu: 25.070024585978906 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 25.426902907480454,
            "unit": "ns/iter",
            "extra": "iterations: 26647893\ncpu: 25.426178234804397 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 26.966973944577543,
            "unit": "ns/iter",
            "extra": "iterations: 26606750\ncpu: 26.962712469580087 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 28.245091562373183,
            "unit": "ns/iter",
            "extra": "iterations: 24479378\ncpu: 28.243012424580225 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 30.155780628660338,
            "unit": "ns/iter",
            "extra": "iterations: 23004773\ncpu: 30.15434566557097 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 30.815959925208283,
            "unit": "ns/iter",
            "extra": "iterations: 23585499\ncpu: 30.812559827544675 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 32.75296925315055,
            "unit": "ns/iter",
            "extra": "iterations: 20997536\ncpu: 32.7504309077023 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 38.656952770359055,
            "unit": "ns/iter",
            "extra": "iterations: 18899488\ncpu: 38.653100020487344 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 41.801185166741156,
            "unit": "ns/iter",
            "extra": "iterations: 17544873\ncpu: 41.79661921747765 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 36.10002239134817,
            "unit": "ns/iter",
            "extra": "iterations: 18895699\ncpu: 36.099280741083106 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 36.84396858425441,
            "unit": "ns/iter",
            "extra": "iterations: 19151797\ncpu: 36.83888754668813 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 37.19175859534628,
            "unit": "ns/iter",
            "extra": "iterations: 19152075\ncpu: 37.18779025249196 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 37.3484040564425,
            "unit": "ns/iter",
            "extra": "iterations: 17397639\ncpu: 37.34231777081963 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 34.677485352236694,
            "unit": "ns/iter",
            "extra": "iterations: 18625368\ncpu: 34.6726311125771 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 34.07200070919298,
            "unit": "ns/iter",
            "extra": "iterations: 19639251\ncpu: 34.06015158113677 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 35.15390290265745,
            "unit": "ns/iter",
            "extra": "iterations: 19180340\ncpu: 35.14951872594531 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 33.10429507855488,
            "unit": "ns/iter",
            "extra": "iterations: 20827349\ncpu: 33.100347864723446 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 35.489173893705775,
            "unit": "ns/iter",
            "extra": "iterations: 19515927\ncpu: 35.484116281025244 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 37.92483732244801,
            "unit": "ns/iter",
            "extra": "iterations: 17691746\ncpu: 37.919982290046725 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 43.76906126237549,
            "unit": "ns/iter",
            "extra": "iterations: 18024685\ncpu: 43.76432853056814 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 43.51539905755056,
            "unit": "ns/iter",
            "extra": "iterations: 14894418\ncpu: 43.51028969376326 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 48.10918139285556,
            "unit": "ns/iter",
            "extra": "iterations: 13561047\ncpu: 48.105678492229 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 56.703208289865515,
            "unit": "ns/iter",
            "extra": "iterations: 10754421\ncpu: 56.70079198126909 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 74.52388614273335,
            "unit": "ns/iter",
            "extra": "iterations: 8234942\ncpu: 74.51855702201692 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 86.05754806302066,
            "unit": "ns/iter",
            "extra": "iterations: 7940302\ncpu: 86.04791404659464 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 295.5830442528777,
            "unit": "ns/iter",
            "extra": "iterations: 2368141\ncpu: 295.54537757676013 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1048.5644012642965,
            "unit": "ns/iter",
            "extra": "iterations: 642270\ncpu: 1048.3380027091525 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4381.482648970188,
            "unit": "ns/iter",
            "extra": "iterations: 159126\ncpu: 4381.03452609881 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 27037.811526419388,
            "unit": "ns/iter",
            "extra": "iterations: 25871\ncpu: 27034.56866762025 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 271844.9031167966,
            "unit": "ns/iter",
            "extra": "iterations: 2663\ncpu: 271504.0000000008 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1360304.0237152304,
            "unit": "ns/iter",
            "extra": "iterations: 506\ncpu: 1360065.4328063175 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 7913490.162161759,
            "unit": "ns/iter",
            "extra": "iterations: 74\ncpu: 7913017.067567532 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 443.80644367927283,
            "unit": "ns/iter",
            "extra": "iterations: 1573542\ncpu: 443.7614407495965 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1751.0401876076824,
            "unit": "ns/iter",
            "extra": "iterations: 398705\ncpu: 1750.8430794697817 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7058.594881234281,
            "unit": "ns/iter",
            "extra": "iterations: 99946\ncpu: 7058.183819262493 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 28644.86926124073,
            "unit": "ns/iter",
            "extra": "iterations: 24744\ncpu: 28642.9857743293 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 118598.46094011873,
            "unit": "ns/iter",
            "extra": "iterations: 5914\ncpu: 118587.45164017577 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 550113.0893417939,
            "unit": "ns/iter",
            "extra": "iterations: 1276\ncpu: 550010.7476489 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2365109.838283448,
            "unit": "ns/iter",
            "extra": "iterations: 303\ncpu: 2364749.7623762335 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 9542518.294872537,
            "unit": "ns/iter",
            "extra": "iterations: 78\ncpu: 9542093.897435823 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 858.5103962538473,
            "unit": "ns/iter",
            "extra": "iterations: 809330\ncpu: 858.4586336846653 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3486.285141575282,
            "unit": "ns/iter",
            "extra": "iterations: 194314\ncpu: 3486.100342744208 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 11702.564976051637,
            "unit": "ns/iter",
            "extra": "iterations: 61792\ncpu: 11701.221857198376 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 53335.19523659648,
            "unit": "ns/iter",
            "extra": "iterations: 12764\ncpu: 53328.41005954185 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 212779.9079456915,
            "unit": "ns/iter",
            "extra": "iterations: 3096\ncpu: 212752.67377261003 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 957004.8644988445,
            "unit": "ns/iter",
            "extra": "iterations: 738\ncpu: 956920.0216802034 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4267940.03529347,
            "unit": "ns/iter",
            "extra": "iterations: 170\ncpu: 4267339.38823523 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 17519905.702704057,
            "unit": "ns/iter",
            "extra": "iterations: 37\ncpu: 17518654.32432421 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1710.7116561362832,
            "unit": "ns/iter",
            "extra": "iterations: 401857\ncpu: 1710.6190933590537 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 141.3765387106895,
            "unit": "ns/iter",
            "extra": "iterations: 5161464\ncpu: 141.36925201841842 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 16.71069870495135,
            "unit": "ns/iter",
            "extra": "iterations: 40990335\ncpu: 16.710161846688955 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 72.05241915843843,
            "unit": "ns/iter",
            "extra": "iterations: 9505723\ncpu: 72.05088524039746 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.4525532300067185,
            "unit": "ms/iter",
            "extra": "iterations: 1513\ncpu: 0.45250643225379206 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.41060584384380405,
            "unit": "ms/iter",
            "extra": "iterations: 1665\ncpu: 0.4105338330330394 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 243.44186580001406,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 243.4138044000008 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 248.59981760000665,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 248.56157399999856 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 726.4085399177786,
            "unit": "ns/iter",
            "extra": "iterations: 986239\ncpu: 726.3526670512937 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 44913.292262056755,
            "unit": "ns/iter",
            "extra": "iterations: 14836\ncpu: 44907.78612833673 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 15.30690218290118,
            "unit": "ns/iter",
            "extra": "iterations: 45294598\ncpu: 15.305221894231186 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 69.34667411430401,
            "unit": "ns/iter",
            "extra": "iterations: 10320935\ncpu: 69.3442229797981 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 164.61758320996825,
            "unit": "ns/iter",
            "extra": "iterations: 4384899\ncpu: 164.60068293477363 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.2984668256883674,
            "unit": "ms/iter",
            "extra": "iterations: 218\ncpu: 3.2979484862386035 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 24.95429190322108,
            "unit": "ms/iter",
            "extra": "iterations: 31\ncpu: 24.950695354837922 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2308.2626704000177,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2308.0118885999977 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2492.686187199979,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2492.387987999996 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.88488040890664,
            "unit": "ms/iter",
            "extra": "iterations: 247\ncpu: 2.884433028340061 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 35.32381945183392,
            "unit": "ns/iter",
            "extra": "iterations: 19276850\ncpu: 35.32180247291465 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 123.84294104399827,
            "unit": "ns/iter",
            "extra": "iterations: 5555347\ncpu: 123.83473291586965 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4685356821404865,
            "unit": "ms/iter",
            "extra": "iterations: 1551\ncpu: 0.4684743558994246 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 206.72385540001414,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 206.69231819999823 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 213.50211839999247,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 213.4718389999989 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.2320013617020016,
            "unit": "ms/iter",
            "extra": "iterations: 564\ncpu: 1.2309714078014522 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 127.94105246579976,
            "unit": "ns/iter",
            "extra": "iterations: 5675776\ncpu: 127.93157552376857 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 36.28421309891079,
            "unit": "ns/iter",
            "extra": "iterations: 20241699\ncpu: 36.276486919402906 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 553.6608317725437,
            "unit": "ns/iter",
            "extra": "iterations: 1174600\ncpu: 553.5296466882396 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1862.020838599983,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1861.803160799997 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 46.2135669374959,
            "unit": "ms/iter",
            "extra": "iterations: 16\ncpu: 46.20943593750049 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1854.2510411999956,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1854.041241800002 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "b0f5a922b01f823fc035bba8f8899216a30bb496",
          "message": "Merge pull request #670 from disorderedmaterials/remove-array-2\n\nRemove Array 2",
          "timestamp": "2021-05-12T23:06:42+01:00",
          "tree_id": "fb84199b852c72bce76fd4c04d9b596829004d07",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/b0f5a922b01f823fc035bba8f8899216a30bb496"
        },
        "date": 1620858402252,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 14.559505399984118,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 14.559385599999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 14.232685999968453,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 14.1811258 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 362.4138799999855,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 362.39127940000003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 676.2135039999976,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 676.1850817999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1417.7490397999918,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1417.6839768 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 3438.9766849999883,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3438.854797 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 7.6347593435580485,
            "unit": "ns/iter",
            "extra": "iterations: 92053758\ncpu: 7.6342704987666385 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 7.567309855895813,
            "unit": "ns/iter",
            "extra": "iterations: 94447788\ncpu: 7.567180631059316 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 16.17951966268212,
            "unit": "ns/iter",
            "extra": "iterations: 40614292\ncpu: 16.179086243827545 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 15.797346102588367,
            "unit": "ns/iter",
            "extra": "iterations: 39604696\ncpu: 15.796876511815645 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 11.654996802482101,
            "unit": "ns/iter",
            "extra": "iterations: 58548582\ncpu: 11.654588457838319 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 11.65981127114388,
            "unit": "ns/iter",
            "extra": "iterations: 60261585\ncpu: 11.659099242079304 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 11.67152485771214,
            "unit": "ns/iter",
            "extra": "iterations: 57511530\ncpu: 11.671020697936564 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 11.84712084146342,
            "unit": "ns/iter",
            "extra": "iterations: 60178798\ncpu: 11.846738663673554 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 12.182976889298656,
            "unit": "ns/iter",
            "extra": "iterations: 58134839\ncpu: 12.182885721933465 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 11.802768087531417,
            "unit": "ns/iter",
            "extra": "iterations: 59330566\ncpu: 11.802690555825828 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 12.011397604611018,
            "unit": "ns/iter",
            "extra": "iterations: 58363667\ncpu: 12.010645424318525 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 12.163792795117022,
            "unit": "ns/iter",
            "extra": "iterations: 58546574\ncpu: 12.16358098084442 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 13.139851876801597,
            "unit": "ns/iter",
            "extra": "iterations: 54351784\ncpu: 13.139130575732413 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 14.710120489410214,
            "unit": "ns/iter",
            "extra": "iterations: 48722871\ncpu: 14.709804292936722 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 14.974667014035102,
            "unit": "ns/iter",
            "extra": "iterations: 46823971\ncpu: 14.974064801124987 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 15.217935180219921,
            "unit": "ns/iter",
            "extra": "iterations: 46723177\ncpu: 15.217772520049369 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 15.001588890402754,
            "unit": "ns/iter",
            "extra": "iterations: 45930166\ncpu: 15.001234025585756 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 15.006759167330394,
            "unit": "ns/iter",
            "extra": "iterations: 46481613\ncpu: 15.006122334868252 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 14.945829791220024,
            "unit": "ns/iter",
            "extra": "iterations: 42026421\ncpu: 14.94522065059972 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 22.504664880449216,
            "unit": "ns/iter",
            "extra": "iterations: 29589933\ncpu: 22.504485664093945 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 22.428813381234043,
            "unit": "ns/iter",
            "extra": "iterations: 31759459\ncpu: 22.42867483983282 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 22.445473425119566,
            "unit": "ns/iter",
            "extra": "iterations: 31308330\ncpu: 22.445339243581454 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 22.487471922394388,
            "unit": "ns/iter",
            "extra": "iterations: 31023834\ncpu: 22.486910966581434 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 22.728293547587718,
            "unit": "ns/iter",
            "extra": "iterations: 31006080\ncpu: 22.72745822754758 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 22.192664490374597,
            "unit": "ns/iter",
            "extra": "iterations: 31458755\ncpu: 22.192308373297074 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 23.000765657954876,
            "unit": "ns/iter",
            "extra": "iterations: 31136619\ncpu: 23.000120051570256 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 22.202390375949342,
            "unit": "ns/iter",
            "extra": "iterations: 29851706\ncpu: 22.201743779735732 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 23.744178789519022,
            "unit": "ns/iter",
            "extra": "iterations: 28434507\ncpu: 23.742940821868224 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 25.177291644217675,
            "unit": "ns/iter",
            "extra": "iterations: 28120530\ncpu: 25.17654126718093 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 29.26519651859414,
            "unit": "ns/iter",
            "extra": "iterations: 25355133\ncpu: 29.263315124397067 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 27.116164669577195,
            "unit": "ns/iter",
            "extra": "iterations: 25462432\ncpu: 27.11540661159149 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 28.282999782853903,
            "unit": "ns/iter",
            "extra": "iterations: 24637450\ncpu: 28.281039312104266 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 26.44047033463435,
            "unit": "ns/iter",
            "extra": "iterations: 24531895\ncpu: 26.44048256361754 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 29.2361296051139,
            "unit": "ns/iter",
            "extra": "iterations: 23427253\ncpu: 29.234639887143263 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 32.87703466621754,
            "unit": "ns/iter",
            "extra": "iterations: 21083495\ncpu: 32.875706376006036 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 32.66663294737725,
            "unit": "ns/iter",
            "extra": "iterations: 21441732\ncpu: 32.665564890000326 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 31.652273095730624,
            "unit": "ns/iter",
            "extra": "iterations: 21329546\ncpu: 31.65096491036436 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 32.81377424802731,
            "unit": "ns/iter",
            "extra": "iterations: 21290498\ncpu: 32.81237437470924 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 33.355237245474406,
            "unit": "ns/iter",
            "extra": "iterations: 21056756\ncpu: 33.353869418441896 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 32.21663140366342,
            "unit": "ns/iter",
            "extra": "iterations: 21981019\ncpu: 32.215575856606065 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 33.0936883364835,
            "unit": "ns/iter",
            "extra": "iterations: 20589564\ncpu: 33.092459461501846 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 33.9323951213928,
            "unit": "ns/iter",
            "extra": "iterations: 20556194\ncpu: 33.9317821674576 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 33.1778052626201,
            "unit": "ns/iter",
            "extra": "iterations: 21169649\ncpu: 33.17604335338801 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 38.32476264573814,
            "unit": "ns/iter",
            "extra": "iterations: 18564866\ncpu: 38.324434068093886 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 39.632335038849476,
            "unit": "ns/iter",
            "extra": "iterations: 17678451\ncpu: 39.63173549537824 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 37.933678790056874,
            "unit": "ns/iter",
            "extra": "iterations: 18889583\ncpu: 37.93130176563429 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 39.83765589126049,
            "unit": "ns/iter",
            "extra": "iterations: 17578519\ncpu: 39.835493763723676 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 43.34760742063681,
            "unit": "ns/iter",
            "extra": "iterations: 16507185\ncpu: 43.345420312428196 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 37.28403473657601,
            "unit": "ns/iter",
            "extra": "iterations: 17357039\ncpu: 37.28361410030873 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 65.90603440307714,
            "unit": "ns/iter",
            "extra": "iterations: 10460637\ncpu: 65.9037495517717 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 237.13464027571996,
            "unit": "ns/iter",
            "extra": "iterations: 2977586\ncpu: 237.13010606578402 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 940.0028165434203,
            "unit": "ns/iter",
            "extra": "iterations: 700504\ncpu: 939.9766867855275 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 3905.57958755183,
            "unit": "ns/iter",
            "extra": "iterations: 178156\ncpu: 3905.504922652048 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 23832.997434614852,
            "unit": "ns/iter",
            "extra": "iterations: 30015\ncpu: 23832.02458770639 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 212050.6296980857,
            "unit": "ns/iter",
            "extra": "iterations: 3246\ncpu: 212044.83703019013 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1081461.5269648272,
            "unit": "ns/iter",
            "extra": "iterations: 649\ncpu: 1081416.9938366802 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 4605322.7032262515,
            "unit": "ns/iter",
            "extra": "iterations: 155\ncpu: 4605084.612903225 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 424.67820542037305,
            "unit": "ns/iter",
            "extra": "iterations: 1671411\ncpu: 424.65762041771984 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1627.18488993312,
            "unit": "ns/iter",
            "extra": "iterations: 426378\ncpu: 1627.1382060050034 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 6813.703086557152,
            "unit": "ns/iter",
            "extra": "iterations: 103643\ncpu: 6813.4888704494215 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 26982.228122570617,
            "unit": "ns/iter",
            "extra": "iterations: 25780\ncpu: 26980.377812257513 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 109896.4233883404,
            "unit": "ns/iter",
            "extra": "iterations: 6422\ncpu: 109895.48878853954 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 461962.6611949925,
            "unit": "ns/iter",
            "extra": "iterations: 1523\ncpu: 461952.6789231768 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2005039.5149859327,
            "unit": "ns/iter",
            "extra": "iterations: 367\ncpu: 2004978.471389621 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 7614355.284090379,
            "unit": "ns/iter",
            "extra": "iterations: 88\ncpu: 7613972.306818181 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 810.1896437515576,
            "unit": "ns/iter",
            "extra": "iterations: 863498\ncpu: 810.158602567692 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3302.7155865323657,
            "unit": "ns/iter",
            "extra": "iterations: 215359\ncpu: 3302.61247962705 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 10956.968949685228,
            "unit": "ns/iter",
            "extra": "iterations: 63381\ncpu: 10956.700762058139 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 51615.730680500004,
            "unit": "ns/iter",
            "extra": "iterations: 13872\ncpu: 51614.9881776237 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 209245.0902233235,
            "unit": "ns/iter",
            "extra": "iterations: 3314\ncpu: 209241.95141822376 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 859007.6243781279,
            "unit": "ns/iter",
            "extra": "iterations: 804\ncpu: 858987.103233817 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 3655201.128865592,
            "unit": "ns/iter",
            "extra": "iterations: 194\ncpu: 3655087.221649452 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 14542865.1249991,
            "unit": "ns/iter",
            "extra": "iterations: 48\ncpu: 14541981.770833286 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1540.7502368676073,
            "unit": "ns/iter",
            "extra": "iterations: 468616\ncpu: 1540.70393029688 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 131.4067446768187,
            "unit": "ns/iter",
            "extra": "iterations: 5304094\ncpu: 131.3988641604015 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 15.856284631987936,
            "unit": "ns/iter",
            "extra": "iterations: 43682343\ncpu: 15.855627593053034 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 68.61309238183317,
            "unit": "ns/iter",
            "extra": "iterations: 9809277\ncpu: 68.61170542946222 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3956975601588952,
            "unit": "ms/iter",
            "extra": "iterations: 1762\ncpu: 0.3956821804767277 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3655125701617471,
            "unit": "ms/iter",
            "extra": "iterations: 1917\ncpu: 0.3654949384454883 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 225.79570140001124,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 225.78570139999957 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 220.54534740000236,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 220.53450779999935 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 679.7445313094876,
            "unit": "ns/iter",
            "extra": "iterations: 958959\ncpu: 679.7196063648241 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 40851.8740796103,
            "unit": "ns/iter",
            "extra": "iterations: 17384\ncpu: 40850.366946617265 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 16.43990982544513,
            "unit": "ns/iter",
            "extra": "iterations: 44691100\ncpu: 16.439319036676228 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 75.41177032929845,
            "unit": "ns/iter",
            "extra": "iterations: 9445785\ncpu: 75.4084776437331 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 169.3466884173404,
            "unit": "ns/iter",
            "extra": "iterations: 4118182\ncpu: 169.3383597422375 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 2.969909511210005,
            "unit": "ms/iter",
            "extra": "iterations: 223\ncpu: 2.9697301390134037 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 23.041113133331237,
            "unit": "ms/iter",
            "extra": "iterations: 30\ncpu: 23.04045986666665 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2095.2859056000307,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2095.201940200002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2147.2561294000116,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2147.040503400001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.549068028368775,
            "unit": "ms/iter",
            "extra": "iterations: 282\ncpu: 2.549000989361717 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 41.36894422397106,
            "unit": "ns/iter",
            "extra": "iterations: 18027706\ncpu: 41.36597629226899 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 127.11657333822622,
            "unit": "ns/iter",
            "extra": "iterations: 5529755\ncpu: 127.11129245328246 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4381624300126813,
            "unit": "ms/iter",
            "extra": "iterations: 1586\ncpu: 0.438148495586383 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 190.34729219997644,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 190.34191220000025 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 201.75404100000378,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 201.74520119999784 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.1523771867769403,
            "unit": "ms/iter",
            "extra": "iterations: 605\ncpu: 1.152278510743804 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 138.8735158866733,
            "unit": "ns/iter",
            "extra": "iterations: 4990943\ncpu: 138.86984864383155 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 41.093884906309725,
            "unit": "ns/iter",
            "extra": "iterations: 17197120\ncpu: 41.09263463882354 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 523.0667061889391,
            "unit": "ns/iter",
            "extra": "iterations: 1312337\ncpu: 523.0498675264047 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1658.9232469999843,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1658.740565800002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 42.23356887500529,
            "unit": "ms/iter",
            "extra": "iterations: 16\ncpu: 42.23263118750076 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1704.3775195999842,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1704.3188162000035 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "56431339+StephenSmith25@users.noreply.github.com",
            "name": "StephenSmith25",
            "username": "StephenSmith25"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "f437cd43f39c2ef2132537b1313ae3da036b238f",
          "message": "Merge pull request #682 from StephenSmith25/improve_fallback_impl_for_counting_iterator\n\nImprove fallback implementation of counting iterator",
          "timestamp": "2021-05-13T17:12:25+01:00",
          "tree_id": "17599a0a0240d7a7a4bda8ef1823d6807eb84ba9",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/f437cd43f39c2ef2132537b1313ae3da036b238f"
        },
        "date": 1620923739753,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 17.87915680001788,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 17.879159 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 15.832008000006681,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 15.831928800000004 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 408.65039559998877,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 408.6078362 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 753.5941249999723,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 753.5134049999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1619.1373612000007,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1618.9557826000002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 3846.633673600036,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3846.2342340000005 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.829826835670977,
            "unit": "ns/iter",
            "extra": "iterations: 80838069\ncpu: 8.82950026923579 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.556875607372143,
            "unit": "ns/iter",
            "extra": "iterations: 81909411\ncpu: 8.55607353836295 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 18.658919921368977,
            "unit": "ns/iter",
            "extra": "iterations: 35338927\ncpu: 18.65758148231276 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 19.322829499839468,
            "unit": "ns/iter",
            "extra": "iterations: 34929311\ncpu: 19.320115475509937 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 13.248917202367936,
            "unit": "ns/iter",
            "extra": "iterations: 53222549\ncpu: 13.247536227549029 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 13.225042144275191,
            "unit": "ns/iter",
            "extra": "iterations: 51663603\ncpu: 13.22371821415558 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.210072270005833,
            "unit": "ns/iter",
            "extra": "iterations: 53247123\ncpu: 13.207882423995025 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 14.41925114325373,
            "unit": "ns/iter",
            "extra": "iterations: 49967661\ncpu: 14.367251470906448 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 14.134061480003734,
            "unit": "ns/iter",
            "extra": "iterations: 50526542\ncpu: 14.132717671436932 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.484272838140747,
            "unit": "ns/iter",
            "extra": "iterations: 52534590\ncpu: 13.48310598788335 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.868226821629193,
            "unit": "ns/iter",
            "extra": "iterations: 50780554\ncpu: 13.866044864339184 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.829967694819146,
            "unit": "ns/iter",
            "extra": "iterations: 50995227\ncpu: 13.828591056178674 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 14.525043331169037,
            "unit": "ns/iter",
            "extra": "iterations: 47291592\ncpu: 14.523119120202201 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 16.433923346342443,
            "unit": "ns/iter",
            "extra": "iterations: 38926260\ncpu: 16.431829644050104 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 17.159392487726862,
            "unit": "ns/iter",
            "extra": "iterations: 41222476\ncpu: 17.15885625113836 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 16.962227366617572,
            "unit": "ns/iter",
            "extra": "iterations: 42467333\ncpu: 16.960652038120774 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 18.137880235836466,
            "unit": "ns/iter",
            "extra": "iterations: 39464126\ncpu: 18.137373522474565 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 19.190826823547035,
            "unit": "ns/iter",
            "extra": "iterations: 39799169\ncpu: 19.188985076547688 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 29.54064051502799,
            "unit": "ns/iter",
            "extra": "iterations: 22840963\ncpu: 29.538092461338014 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 25.66136093642648,
            "unit": "ns/iter",
            "extra": "iterations: 27395475\ncpu: 25.658546603043238 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 25.000753480057877,
            "unit": "ns/iter",
            "extra": "iterations: 27834844\ncpu: 24.998199127683144 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 25.93164190312886,
            "unit": "ns/iter",
            "extra": "iterations: 26867322\ncpu: 25.928545204468097 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 25.297039690697932,
            "unit": "ns/iter",
            "extra": "iterations: 27664643\ncpu: 25.294780489305584 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 25.7233392694437,
            "unit": "ns/iter",
            "extra": "iterations: 27039591\ncpu: 25.719045565445274 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 25.587133235940666,
            "unit": "ns/iter",
            "extra": "iterations: 27522087\ncpu: 25.585022204166325 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 25.70728058356482,
            "unit": "ns/iter",
            "extra": "iterations: 26389739\ncpu: 25.70122144823048 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 25.60412960061261,
            "unit": "ns/iter",
            "extra": "iterations: 27223359\ncpu: 25.601543622886574 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 27.00066779998354,
            "unit": "ns/iter",
            "extra": "iterations: 26039234\ncpu: 26.99683508355136 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 29.60533888708623,
            "unit": "ns/iter",
            "extra": "iterations: 24687692\ncpu: 29.60126001247894 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 32.95195826766285,
            "unit": "ns/iter",
            "extra": "iterations: 21444668\ncpu: 32.94737899416296 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 30.84341193274841,
            "unit": "ns/iter",
            "extra": "iterations: 23101358\ncpu: 30.840840655341943 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 32.61599154701785,
            "unit": "ns/iter",
            "extra": "iterations: 21212882\ncpu: 32.6116875585313 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 37.871245461482154,
            "unit": "ns/iter",
            "extra": "iterations: 18044428\ncpu: 37.86524937227235 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 57.9795362637441,
            "unit": "ns/iter",
            "extra": "iterations: 11682373\ncpu: 57.97708821658032 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 37.880125802977986,
            "unit": "ns/iter",
            "extra": "iterations: 18695583\ncpu: 37.87755835161759 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 37.766825854089376,
            "unit": "ns/iter",
            "extra": "iterations: 18677774\ncpu: 37.765680214355015 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 36.820929976953366,
            "unit": "ns/iter",
            "extra": "iterations: 18997976\ncpu: 36.81639791523091 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 37.5230917025086,
            "unit": "ns/iter",
            "extra": "iterations: 18869267\ncpu: 37.51962569611237 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 37.75040982578997,
            "unit": "ns/iter",
            "extra": "iterations: 18837272\ncpu: 37.74576998198045 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 37.02436043925699,
            "unit": "ns/iter",
            "extra": "iterations: 19202281\ncpu: 37.01994945288031 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 38.436291571949305,
            "unit": "ns/iter",
            "extra": "iterations: 18187264\ncpu: 38.43234919776866 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 38.854918292764985,
            "unit": "ns/iter",
            "extra": "iterations: 18365265\ncpu: 38.84630959585903 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 38.383824639748646,
            "unit": "ns/iter",
            "extra": "iterations: 17925066\ncpu: 38.38213426940799 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 43.30608493169329,
            "unit": "ns/iter",
            "extra": "iterations: 15979029\ncpu: 43.30148521540293 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 44.42688791206046,
            "unit": "ns/iter",
            "extra": "iterations: 15643022\ncpu: 44.42135841783051 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 42.42019520152904,
            "unit": "ns/iter",
            "extra": "iterations: 16203562\ncpu: 42.41409759162787 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 48.878627052734224,
            "unit": "ns/iter",
            "extra": "iterations: 10451909\ncpu: 48.878713161395304 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 58.59030704354816,
            "unit": "ns/iter",
            "extra": "iterations: 11334581\ncpu: 58.585472193458685 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 56.423610199201164,
            "unit": "ns/iter",
            "extra": "iterations: 11605620\ncpu: 56.421593934663285 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 75.04988629376768,
            "unit": "ns/iter",
            "extra": "iterations: 9346455\ncpu: 75.04022487670507 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 271.6596362504034,
            "unit": "ns/iter",
            "extra": "iterations: 2580786\ncpu: 271.6283663194097 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1066.469062567892,
            "unit": "ns/iter",
            "extra": "iterations: 657957\ncpu: 1066.3666242626873 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4500.34732055945,
            "unit": "ns/iter",
            "extra": "iterations: 151972\ncpu: 4499.945930829281 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 28280.003430395718,
            "unit": "ns/iter",
            "extra": "iterations: 25070\ncpu: 28276.59700837625 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 244138.3248987923,
            "unit": "ns/iter",
            "extra": "iterations: 2964\ncpu: 244116.76585695066 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1201162.919382875,
            "unit": "ns/iter",
            "extra": "iterations: 583\ncpu: 1201069.950257299 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 7775629.071426508,
            "unit": "ns/iter",
            "extra": "iterations: 84\ncpu: 7774883.833333389 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 480.8593173308197,
            "unit": "ns/iter",
            "extra": "iterations: 1497768\ncpu: 480.8070395414987 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1899.6269868274626,
            "unit": "ns/iter",
            "extra": "iterations: 382016\ncpu: 1899.4699201080846 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7756.777573447682,
            "unit": "ns/iter",
            "extra": "iterations: 90812\ncpu: 7756.224772056494 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 31359.147670698778,
            "unit": "ns/iter",
            "extra": "iterations: 22496\ncpu: 31356.653805121317 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 126315.62742257965,
            "unit": "ns/iter",
            "extra": "iterations: 5521\ncpu: 126304.94149610739 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 531810.475683973,
            "unit": "ns/iter",
            "extra": "iterations: 1316\ncpu: 531767.9460486269 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2172729.5440726858,
            "unit": "ns/iter",
            "extra": "iterations: 329\ncpu: 2172546.142857136 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 9040981.253331969,
            "unit": "ns/iter",
            "extra": "iterations: 75\ncpu: 9040264.000000017 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 920.9699552618995,
            "unit": "ns/iter",
            "extra": "iterations: 757071\ncpu: 920.8689039733499 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3853.4331865221657,
            "unit": "ns/iter",
            "extra": "iterations: 187088\ncpu: 3841.276741426495 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 12627.552834064267,
            "unit": "ns/iter",
            "extra": "iterations: 56015\ncpu: 12626.72273498183 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 59132.06476093334,
            "unit": "ns/iter",
            "extra": "iterations: 11859\ncpu: 59125.70638333819 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 237514.96272447513,
            "unit": "ns/iter",
            "extra": "iterations: 2951\ncpu: 237481.68858014277 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 973326.1006992627,
            "unit": "ns/iter",
            "extra": "iterations: 715\ncpu: 973189.1748251861 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4217330.846625323,
            "unit": "ns/iter",
            "extra": "iterations: 163\ncpu: 4216925.95705525 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 17398389.375000533,
            "unit": "ns/iter",
            "extra": "iterations: 40\ncpu: 17396924.37500011 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1709.710631910444,
            "unit": "ns/iter",
            "extra": "iterations: 407716\ncpu: 1709.4175234722159 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 115.6694961051583,
            "unit": "ns/iter",
            "extra": "iterations: 6104627\ncpu: 115.6601769444726 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 19.09692220448864,
            "unit": "ns/iter",
            "extra": "iterations: 39010194\ncpu: 19.094384278119705 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 80.2652896903436,
            "unit": "ns/iter",
            "extra": "iterations: 8819227\ncpu: 80.25561786764277 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.47259778335447006,
            "unit": "ms/iter",
            "extra": "iterations: 1574\ncpu: 0.4725890717916106 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.4198659958134478,
            "unit": "ms/iter",
            "extra": "iterations: 1672\ncpu: 0.4198117482057447 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 253.2320644000265,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 253.2048427999996 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 253.2278445999964,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 253.2013059999997 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 800.9066018508735,
            "unit": "ns/iter",
            "extra": "iterations: 853700\ncpu: 800.8176959119082 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 46226.73774782253,
            "unit": "ns/iter",
            "extra": "iterations: 15283\ncpu: 46223.040240790186 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 18.218398006182575,
            "unit": "ns/iter",
            "extra": "iterations: 38695193\ncpu: 18.21667420033299 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 86.86823107906449,
            "unit": "ns/iter",
            "extra": "iterations: 8203634\ncpu: 86.86063651791291 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 197.67581537577578,
            "unit": "ns/iter",
            "extra": "iterations: 3551430\ncpu: 197.65658622019876 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.3290747804873786,
            "unit": "ms/iter",
            "extra": "iterations: 205\ncpu: 3.3286113170731824 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 27.325807840006746,
            "unit": "ms/iter",
            "extra": "iterations: 25\ncpu: 27.321556000000555 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2413.914370400016,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2413.666509199999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2445.072368199999,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2444.2175669999983 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.902220580246514,
            "unit": "ms/iter",
            "extra": "iterations: 243\ncpu: 2.9021098600823785 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 42.051189186003235,
            "unit": "ns/iter",
            "extra": "iterations: 16730604\ncpu: 42.04812306836027 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 146.0228541358851,
            "unit": "ns/iter",
            "extra": "iterations: 4843981\ncpu: 146.01112886280632 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.49869542261473626,
            "unit": "ms/iter",
            "extra": "iterations: 1415\ncpu: 0.49866022261485676 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 219.01141319999624,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 218.9908503999959 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 219.6029449999969,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 219.46266320000518 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.319501542694271,
            "unit": "ms/iter",
            "extra": "iterations: 527\ncpu: 1.319261698292225 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 161.2237917138955,
            "unit": "ns/iter",
            "extra": "iterations: 4502307\ncpu: 161.20442231060173 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 45.07627507368082,
            "unit": "ns/iter",
            "extra": "iterations: 15576845\ncpu: 45.071646857882705 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 595.9942629152031,
            "unit": "ns/iter",
            "extra": "iterations: 1178996\ncpu: 595.9309929804683 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1909.7007321999627,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1909.5035317999987 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 49.528937071435,
            "unit": "ms/iter",
            "extra": "iterations: 14\ncpu: 49.52337949999982 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1951.946796600032,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1951.683169800003 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "d5e7fffb8845832303e603091f91454f4874e5e5",
          "message": "Merge pull request #671 from disorderedmaterials/remove-array-3-isotopes\n\nRemove Array 3 (Isotopes)",
          "timestamp": "2021-05-14T11:34:04+01:00",
          "tree_id": "3ffd894326403fb54aa539dc91216b566696e399",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/d5e7fffb8845832303e603091f91454f4874e5e5"
        },
        "date": 1620989891869,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 17.239758000005168,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 17.236382799999998 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 17.29116060000706,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 17.2858552 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 415.59306359999937,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 415.5717396 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 823.4423778000291,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 823.3890955999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1673.5165455999777,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1672.8898581999995 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 4494.114321200004,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 4493.5619146 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.829727090707296,
            "unit": "ns/iter",
            "extra": "iterations: 74342613\ncpu: 8.829576127489574 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.883945283426046,
            "unit": "ns/iter",
            "extra": "iterations: 77623868\ncpu: 8.883090481912065 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 19.40823747849995,
            "unit": "ns/iter",
            "extra": "iterations: 34530688\ncpu: 19.407056760641453 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 20.177445273344805,
            "unit": "ns/iter",
            "extra": "iterations: 38036719\ncpu: 20.175354293833838 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 13.730376776396378,
            "unit": "ns/iter",
            "extra": "iterations: 52436963\ncpu: 13.729839884128992 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 13.712050835871382,
            "unit": "ns/iter",
            "extra": "iterations: 51390012\ncpu: 13.711735871943393 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.83949732944309,
            "unit": "ns/iter",
            "extra": "iterations: 51085069\ncpu: 13.838831831665052 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.95718318447446,
            "unit": "ns/iter",
            "extra": "iterations: 49512323\ncpu: 13.956386392939061 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 14.458056405625113,
            "unit": "ns/iter",
            "extra": "iterations: 49616289\ncpu: 14.334909872038136 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 14.156589950353888,
            "unit": "ns/iter",
            "extra": "iterations: 46745337\ncpu: 14.15349618722388 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.909309340790868,
            "unit": "ns/iter",
            "extra": "iterations: 50766441\ncpu: 13.907195661007727 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.958095335332153,
            "unit": "ns/iter",
            "extra": "iterations: 48050999\ncpu: 13.956875006906744 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 15.141530831120681,
            "unit": "ns/iter",
            "extra": "iterations: 46273112\ncpu: 15.139924455480816 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 18.218251027540184,
            "unit": "ns/iter",
            "extra": "iterations: 42099142\ncpu: 18.216306854899692 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 17.23981875252541,
            "unit": "ns/iter",
            "extra": "iterations: 39306038\ncpu: 17.237982291677437 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 17.281999297708,
            "unit": "ns/iter",
            "extra": "iterations: 39206087\ncpu: 17.280127624059997 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 51.19539509546226,
            "unit": "ns/iter",
            "extra": "iterations: 37734463\ncpu: 51.18340086090533 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 68.9656487000093,
            "unit": "ns/iter",
            "extra": "iterations: 10000000\ncpu: 68.96016040000035 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 29.903098231808016,
            "unit": "ns/iter",
            "extra": "iterations: 21409857\ncpu: 29.901617044896803 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 28.85202901471033,
            "unit": "ns/iter",
            "extra": "iterations: 25469973\ncpu: 28.84410800121383 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 28.531591280652645,
            "unit": "ns/iter",
            "extra": "iterations: 25393415\ncpu: 28.527427799687583 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 26.347956899220353,
            "unit": "ns/iter",
            "extra": "iterations: 26467735\ncpu: 26.346061383794225 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 26.128789407852594,
            "unit": "ns/iter",
            "extra": "iterations: 27259602\ncpu: 26.126954678208264 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 27.404918577796803,
            "unit": "ns/iter",
            "extra": "iterations: 25910498\ncpu: 27.40252564809845 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 26.087369500612596,
            "unit": "ns/iter",
            "extra": "iterations: 27143614\ncpu: 26.086112998807014 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 26.469790455527516,
            "unit": "ns/iter",
            "extra": "iterations: 25675743\ncpu: 26.46799958232962 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 26.05692605802844,
            "unit": "ns/iter",
            "extra": "iterations: 26122536\ncpu: 26.05558507030079 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 28.51305126633881,
            "unit": "ns/iter",
            "extra": "iterations: 24267875\ncpu: 28.510690243789558 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 28.65743665929336,
            "unit": "ns/iter",
            "extra": "iterations: 23365384\ncpu: 28.655956777769962 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 32.44256416952387,
            "unit": "ns/iter",
            "extra": "iterations: 21772289\ncpu: 32.43926088800279 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 34.044053222117576,
            "unit": "ns/iter",
            "extra": "iterations: 20891162\ncpu: 34.039574533958806 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 33.83632557854471,
            "unit": "ns/iter",
            "extra": "iterations: 20456434\ncpu: 33.83363649793528 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 34.786058699984046,
            "unit": "ns/iter",
            "extra": "iterations: 17552782\ncpu: 34.78189833383674 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 64.28459823310581,
            "unit": "ns/iter",
            "extra": "iterations: 10079402\ncpu: 64.27698895232162 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 38.343298446811886,
            "unit": "ns/iter",
            "extra": "iterations: 18279270\ncpu: 38.33575739074917 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 38.83505669689213,
            "unit": "ns/iter",
            "extra": "iterations: 18072507\ncpu: 38.832484281234 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 38.27573537961806,
            "unit": "ns/iter",
            "extra": "iterations: 18617357\ncpu: 38.273048048656584 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 39.41572604041292,
            "unit": "ns/iter",
            "extra": "iterations: 17944428\ncpu: 39.41065683453333 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 40.556393837347905,
            "unit": "ns/iter",
            "extra": "iterations: 18358584\ncpu: 40.55189850154011 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 37.834069628339314,
            "unit": "ns/iter",
            "extra": "iterations: 18702875\ncpu: 37.82977194682648 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 39.02212130108044,
            "unit": "ns/iter",
            "extra": "iterations: 17888776\ncpu: 39.01776068971901 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 40.31682998373717,
            "unit": "ns/iter",
            "extra": "iterations: 17915050\ncpu: 40.31121967284502 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 41.23067508671984,
            "unit": "ns/iter",
            "extra": "iterations: 17371087\ncpu: 41.226978599553874 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 45.618010271435345,
            "unit": "ns/iter",
            "extra": "iterations: 15330674\ncpu: 45.61209618050736 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 46.74834455243589,
            "unit": "ns/iter",
            "extra": "iterations: 11708314\ncpu: 46.74403402573536 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 60.46996394161592,
            "unit": "ns/iter",
            "extra": "iterations: 11778393\ncpu: 60.458331539795296 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 82.0099121359744,
            "unit": "ns/iter",
            "extra": "iterations: 12371501\ncpu: 81.99873071181817 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 84.2883747145567,
            "unit": "ns/iter",
            "extra": "iterations: 7966170\ncpu: 84.27787129825246 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 81.9754622537059,
            "unit": "ns/iter",
            "extra": "iterations: 8274028\ncpu: 81.96297559060741 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 77.23013409492798,
            "unit": "ns/iter",
            "extra": "iterations: 9321083\ncpu: 77.22060805595305 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 280.59979550837846,
            "unit": "ns/iter",
            "extra": "iterations: 2517463\ncpu: 280.56296557288175 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1082.90989469064,
            "unit": "ns/iter",
            "extra": "iterations: 648186\ncpu: 1082.8026739238624 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4863.890563665226,
            "unit": "ns/iter",
            "extra": "iterations: 149539\ncpu: 4863.227412246994 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 30262.9712269954,
            "unit": "ns/iter",
            "extra": "iterations: 23529\ncpu: 30257.79433890049 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 246745.2975061497,
            "unit": "ns/iter",
            "extra": "iterations: 2847\ncpu: 246723.96873902492 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1257538.010544896,
            "unit": "ns/iter",
            "extra": "iterations: 569\ncpu: 1257404.4850615081 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 9841493.32467413,
            "unit": "ns/iter",
            "extra": "iterations: 77\ncpu: 9839966.207792187 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 482.13813498324737,
            "unit": "ns/iter",
            "extra": "iterations: 1421081\ncpu: 482.11079312157466 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1943.2001049186142,
            "unit": "ns/iter",
            "extra": "iterations: 352655\ncpu: 1943.0859281734479 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 8019.456840434018,
            "unit": "ns/iter",
            "extra": "iterations: 89563\ncpu: 8018.0141129706435 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 32439.578039425443,
            "unit": "ns/iter",
            "extra": "iterations: 21912\ncpu: 32433.013645491093 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 132459.66585731538,
            "unit": "ns/iter",
            "extra": "iterations: 5354\ncpu: 132446.02596189632 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 815517.1192212809,
            "unit": "ns/iter",
            "extra": "iterations: 822\ncpu: 815339.8479318701 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 3000205.8770053783,
            "unit": "ns/iter",
            "extra": "iterations: 187\ncpu: 2999656.037433136 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 42263691.40000251,
            "unit": "ns/iter",
            "extra": "iterations: 70\ncpu: 42254642.52857145 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 961.2080323601103,
            "unit": "ns/iter",
            "extra": "iterations: 745360\ncpu: 961.1370277449834 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3995.816582051477,
            "unit": "ns/iter",
            "extra": "iterations: 183379\ncpu: 3995.3011468052446 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 13129.915025601074,
            "unit": "ns/iter",
            "extra": "iterations: 50780\ncpu: 13128.909984245609 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 59763.60621532898,
            "unit": "ns/iter",
            "extra": "iterations: 11359\ncpu: 59758.24984593653 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 245343.94998249656,
            "unit": "ns/iter",
            "extra": "iterations: 2859\ncpu: 245312.23644630922 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 999173.3670360387,
            "unit": "ns/iter",
            "extra": "iterations: 722\ncpu: 999092.9335180062 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4346130.664634152,
            "unit": "ns/iter",
            "extra": "iterations: 164\ncpu: 4345844.4573170105 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 19265893.735297035,
            "unit": "ns/iter",
            "extra": "iterations: 34\ncpu: 19263483.41176474 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1768.3069664449133,
            "unit": "ns/iter",
            "extra": "iterations: 402386\ncpu: 1768.1683557578926 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 118.42036701790974,
            "unit": "ns/iter",
            "extra": "iterations: 5638744\ncpu: 118.4096754525471 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 18.442382698523446,
            "unit": "ns/iter",
            "extra": "iterations: 38231190\ncpu: 18.441392538396084 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 79.81664271850713,
            "unit": "ns/iter",
            "extra": "iterations: 8494045\ncpu: 79.81200417468962 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.46039203207174084,
            "unit": "ms/iter",
            "extra": "iterations: 1559\ncpu: 0.4603759409878169 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.43166918079431715,
            "unit": "ms/iter",
            "extra": "iterations: 1687\ncpu: 0.43162187136929026 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 257.96962539998276,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 257.94246759999737 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 264.1265394000129,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 264.1034128000001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 797.8610357695134,
            "unit": "ns/iter",
            "extra": "iterations: 878960\ncpu: 797.7686925457347 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 48262.64765992339,
            "unit": "ns/iter",
            "extra": "iterations: 14679\ncpu: 48258.3272702501 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 18.44815651264558,
            "unit": "ns/iter",
            "extra": "iterations: 34919198\ncpu: 18.44608003310951 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 86.79893751806956,
            "unit": "ns/iter",
            "extra": "iterations: 8007854\ncpu: 86.79236234826239 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 199.00777328363003,
            "unit": "ns/iter",
            "extra": "iterations: 3501609\ncpu: 198.98928349795483 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.503742135921818,
            "unit": "ms/iter",
            "extra": "iterations: 206\ncpu: 3.503172786407781 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 26.970517720001226,
            "unit": "ms/iter",
            "extra": "iterations: 25\ncpu: 26.967845240000088 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2499.0076258000045,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2498.8021756000026 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2484.9904036000225,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2484.771195399998 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.9174413262717667,
            "unit": "ms/iter",
            "extra": "iterations: 236\ncpu: 2.9173327415254198 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 43.553089830471215,
            "unit": "ns/iter",
            "extra": "iterations: 16121289\ncpu: 43.550081200082666 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 150.880200846611,
            "unit": "ns/iter",
            "extra": "iterations: 4682579\ncpu: 150.86864140466028 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.500841753313612,
            "unit": "ms/iter",
            "extra": "iterations: 1358\ncpu: 0.500785193667165 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 224.14781620000213,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 224.12668860000053 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 222.00244659998134,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 221.98558660000458 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.3984287623574547,
            "unit": "ms/iter",
            "extra": "iterations: 526\ncpu: 1.3983288041824788 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 164.38476130767717,
            "unit": "ns/iter",
            "extra": "iterations: 4204534\ncpu: 164.37459180970035 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 46.364190610754534,
            "unit": "ns/iter",
            "extra": "iterations: 15320373\ncpu: 46.36046263364561 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 617.0352472634239,
            "unit": "ns/iter",
            "extra": "iterations: 1150756\ncpu: 616.9665185321621 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2012.1415838000303,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2011.9333168000026 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 50.608065769228446,
            "unit": "ms/iter",
            "extra": "iterations: 13\ncpu: 50.602781461538356 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2117.380524800001,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2117.1620958000008 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "0bfccc6088fd8d74ae3bdcad66f440f4f98533f1",
          "message": "Merge pull request #691 from disorderedmaterials/bugfix-18-05-21-fix-benchmarks\n\nBugfix 18/05/21 Benchmarks and Const",
          "timestamp": "2021-05-18T16:53:54+01:00",
          "tree_id": "c996cbc8b238b20950748988c442bf641e4525dd",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/0bfccc6088fd8d74ae3bdcad66f440f4f98533f1"
        },
        "date": 1621354668947,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 18.135839800015674,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 18.135756800000003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 17.203499800007194,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 17.135233000000003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 421.0999857999923,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 420.91605419999996 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 798.9020631999665,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 798.8422774 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1697.0291180000004,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1696.6803352 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 4105.893598200009,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 4105.4951077999995 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 9.344778850612125,
            "unit": "ns/iter",
            "extra": "iterations: 72183311\ncpu: 9.344577806911632 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.704985074757897,
            "unit": "ns/iter",
            "extra": "iterations: 78313309\ncpu: 8.704181328872249 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 20.777005518321204,
            "unit": "ns/iter",
            "extra": "iterations: 33311775\ncpu: 20.77580849414372 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 20.336521215887192,
            "unit": "ns/iter",
            "extra": "iterations: 34688229\ncpu: 20.333697145507152 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 13.733418740385336,
            "unit": "ns/iter",
            "extra": "iterations: 51206544\ncpu: 13.732995610873408 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 14.43245225466197,
            "unit": "ns/iter",
            "extra": "iterations: 50734743\ncpu: 14.432013245834394 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.983299221390421,
            "unit": "ns/iter",
            "extra": "iterations: 51087678\ncpu: 13.952011461550432 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 14.063886865737045,
            "unit": "ns/iter",
            "extra": "iterations: 49924894\ncpu: 14.062249406077854 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.655769214849098,
            "unit": "ns/iter",
            "extra": "iterations: 49045166\ncpu: 13.65407471553868 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.963029916100828,
            "unit": "ns/iter",
            "extra": "iterations: 51452791\ncpu: 13.926482161094036 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 14.338207527268265,
            "unit": "ns/iter",
            "extra": "iterations: 48690102\ncpu: 14.32365797878187 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.961509073480293,
            "unit": "ns/iter",
            "extra": "iterations: 48249579\ncpu: 13.960661439139251 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 15.253961278497037,
            "unit": "ns/iter",
            "extra": "iterations: 47433802\ncpu: 15.252867164221826 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 16.67917195425533,
            "unit": "ns/iter",
            "extra": "iterations: 42653344\ncpu: 16.675558661942112 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 17.832305625333575,
            "unit": "ns/iter",
            "extra": "iterations: 36226081\ncpu: 17.830322109642577 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 17.653283720426995,
            "unit": "ns/iter",
            "extra": "iterations: 37094647\ncpu: 17.650481159720883 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 16.800799369766317,
            "unit": "ns/iter",
            "extra": "iterations: 43601599\ncpu: 16.799440268234193 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 18.763636025017533,
            "unit": "ns/iter",
            "extra": "iterations: 33827270\ncpu: 18.760220201038933 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 24.72670308387083,
            "unit": "ns/iter",
            "extra": "iterations: 27301929\ncpu: 24.725748499309496 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 25.8653632673233,
            "unit": "ns/iter",
            "extra": "iterations: 26388638\ncpu: 25.86260200318038 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 25.39652407068092,
            "unit": "ns/iter",
            "extra": "iterations: 27611436\ncpu: 25.395003831021178 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 25.390198973191296,
            "unit": "ns/iter",
            "extra": "iterations: 26486633\ncpu: 25.388124681608513 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 25.593436046296308,
            "unit": "ns/iter",
            "extra": "iterations: 29383114\ncpu: 25.592584638918787 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 26.61627662434857,
            "unit": "ns/iter",
            "extra": "iterations: 26166605\ncpu: 26.61449060739813 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 25.833292504303692,
            "unit": "ns/iter",
            "extra": "iterations: 27680471\ncpu: 25.830722497460503 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 26.280334977518777,
            "unit": "ns/iter",
            "extra": "iterations: 26196863\ncpu: 26.278226747988857 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 25.215960125837263,
            "unit": "ns/iter",
            "extra": "iterations: 26271447\ncpu: 25.215391333412036 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 28.374597976883614,
            "unit": "ns/iter",
            "extra": "iterations: 25608913\ncpu: 28.37126913586734 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 30.278190222252114,
            "unit": "ns/iter",
            "extra": "iterations: 22681437\ncpu: 30.275992389723918 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 33.05688231955598,
            "unit": "ns/iter",
            "extra": "iterations: 21432389\ncpu: 33.05392805253722 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 32.09218974370072,
            "unit": "ns/iter",
            "extra": "iterations: 22229501\ncpu: 32.09040441348641 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 34.676602082242525,
            "unit": "ns/iter",
            "extra": "iterations: 18852793\ncpu: 34.67510591136291 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 35.36596153614304,
            "unit": "ns/iter",
            "extra": "iterations: 20110414\ncpu: 35.361831934439756 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 45.308901751698265,
            "unit": "ns/iter",
            "extra": "iterations: 12797616\ncpu: 45.30610646545394 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 38.248430132487044,
            "unit": "ns/iter",
            "extra": "iterations: 19046671\ncpu: 38.24529058122547 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 39.573908942859084,
            "unit": "ns/iter",
            "extra": "iterations: 18828574\ncpu: 39.57125760028343 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 37.9811129837619,
            "unit": "ns/iter",
            "extra": "iterations: 18129968\ncpu: 37.97615544605471 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 37.48884463911058,
            "unit": "ns/iter",
            "extra": "iterations: 18320743\ncpu: 37.48743705427229 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 36.1307858005512,
            "unit": "ns/iter",
            "extra": "iterations: 19111249\ncpu: 36.12749988239967 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 38.76671873023155,
            "unit": "ns/iter",
            "extra": "iterations: 18239244\ncpu: 38.76410173579606 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 39.19409657379168,
            "unit": "ns/iter",
            "extra": "iterations: 17738953\ncpu: 39.1927340920287 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 37.24918127825569,
            "unit": "ns/iter",
            "extra": "iterations: 17918481\ncpu: 37.24750831278598 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 40.50927852375582,
            "unit": "ns/iter",
            "extra": "iterations: 17578389\ncpu: 40.504588048426925 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 42.753873551287384,
            "unit": "ns/iter",
            "extra": "iterations: 16675331\ncpu: 42.748355519899285 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 46.196202909031065,
            "unit": "ns/iter",
            "extra": "iterations: 15261578\ncpu: 46.195247241143875 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 43.96267376896227,
            "unit": "ns/iter",
            "extra": "iterations: 15842371\ncpu: 43.96090509432008 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 48.10877411894263,
            "unit": "ns/iter",
            "extra": "iterations: 14717113\ncpu: 48.10377755474176 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 54.43800257947855,
            "unit": "ns/iter",
            "extra": "iterations: 11658977\ncpu: 54.43502504550765 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 58.20184734224164,
            "unit": "ns/iter",
            "extra": "iterations: 9491257\ncpu: 58.20043372548078 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 72.33343690539593,
            "unit": "ns/iter",
            "extra": "iterations: 10115341\ncpu: 72.32902944151749 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 279.9862305000596,
            "unit": "ns/iter",
            "extra": "iterations: 2555721\ncpu: 279.9680180270037 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1025.3836454230086,
            "unit": "ns/iter",
            "extra": "iterations: 653872\ncpu: 1025.2944888296329 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4410.2569037290195,
            "unit": "ns/iter",
            "extra": "iterations: 159986\ncpu: 4410.035109322133 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 28942.441359946566,
            "unit": "ns/iter",
            "extra": "iterations: 25648\ncpu: 28939.28271210214 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 255021.09981444984,
            "unit": "ns/iter",
            "extra": "iterations: 2695\ncpu: 255014.5168831137 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1284975.2747663665,
            "unit": "ns/iter",
            "extra": "iterations: 535\ncpu: 1284891.5794392647 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 5652476.781511998,
            "unit": "ns/iter",
            "extra": "iterations: 119\ncpu: 5651969.966386557 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 468.0409669998719,
            "unit": "ns/iter",
            "extra": "iterations: 1459028\ncpu: 467.9916780212579 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1953.4866123583693,
            "unit": "ns/iter",
            "extra": "iterations: 375757\ncpu: 1953.2956698078926 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7997.094849354965,
            "unit": "ns/iter",
            "extra": "iterations: 91872\ncpu: 7995.340049198858 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 32531.01149375609,
            "unit": "ns/iter",
            "extra": "iterations: 23230\ncpu: 32527.663021954748 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 133291.07838039123,
            "unit": "ns/iter",
            "extra": "iterations: 5384\ncpu: 133281.55033432238 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 542628.1932175466,
            "unit": "ns/iter",
            "extra": "iterations: 1268\ncpu: 542602.9550473248 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2425828.744897902,
            "unit": "ns/iter",
            "extra": "iterations: 294\ncpu: 2425714.2823129194 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 9453927.42105349,
            "unit": "ns/iter",
            "extra": "iterations: 76\ncpu: 9453131.710526314 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 905.22089516578,
            "unit": "ns/iter",
            "extra": "iterations: 753045\ncpu: 905.177856569013 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3866.214590353907,
            "unit": "ns/iter",
            "extra": "iterations: 189920\ncpu: 3865.5941501685234 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 13031.843169900954,
            "unit": "ns/iter",
            "extra": "iterations: 56456\ncpu: 12992.571967549833 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 59518.72040585892,
            "unit": "ns/iter",
            "extra": "iterations: 10644\ncpu: 59517.03955279894 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 233897.79435192782,
            "unit": "ns/iter",
            "extra": "iterations: 2762\ncpu: 233890.1227371476 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 1014162.5688455898,
            "unit": "ns/iter",
            "extra": "iterations: 719\ncpu: 1014121.2948539587 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4385072.26380369,
            "unit": "ns/iter",
            "extra": "iterations: 163\ncpu: 4384863.993865069 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 17708785.025001816,
            "unit": "ns/iter",
            "extra": "iterations: 40\ncpu: 17706936.775000058 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1742.0833829552812,
            "unit": "ns/iter",
            "extra": "iterations: 391291\ncpu: 1741.9734519833294 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 121.0219846370522,
            "unit": "ns/iter",
            "extra": "iterations: 5986999\ncpu: 121.00995289960652 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 17.812227217094556,
            "unit": "ns/iter",
            "extra": "iterations: 38576006\ncpu: 17.81189104958133 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 81.82222876447985,
            "unit": "ns/iter",
            "extra": "iterations: 9123602\ncpu: 81.81004695294692 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.472128139757501,
            "unit": "ms/iter",
            "extra": "iterations: 1567\ncpu: 0.4720657319719268 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.40984058606794393,
            "unit": "ms/iter",
            "extra": "iterations: 1737\ncpu: 0.40981128785262155 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 264.69938000000184,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 264.6825058000019 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 257.3138835999998,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 257.2922979999987 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 768.1117577658123,
            "unit": "ns/iter",
            "extra": "iterations: 871322\ncpu: 768.047505973676 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 48576.88815606624,
            "unit": "ns/iter",
            "extra": "iterations: 14404\ncpu: 48573.41967508984 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 20.15060631823858,
            "unit": "ns/iter",
            "extra": "iterations: 37364289\ncpu: 20.14555336514008 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 90.82352371147907,
            "unit": "ns/iter",
            "extra": "iterations: 7677632\ncpu: 90.81885352150242 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 204.23457107348094,
            "unit": "ns/iter",
            "extra": "iterations: 3300262\ncpu: 204.21586740689708 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.4170808461544184,
            "unit": "ms/iter",
            "extra": "iterations: 195\ncpu: 3.4168724923077685 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 27.295858000002227,
            "unit": "ms/iter",
            "extra": "iterations: 26\ncpu: 27.294443653846 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2483.1567890000315,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2482.967706599999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2536.3148920000185,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2535.2400692 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 3.0296315427352205,
            "unit": "ms/iter",
            "extra": "iterations: 234\ncpu: 3.029316397435871 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 44.417613868850474,
            "unit": "ns/iter",
            "extra": "iterations: 14760664\ncpu: 44.41192435516419 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 149.7536952021786,
            "unit": "ns/iter",
            "extra": "iterations: 4736615\ncpu: 149.73959737069595 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.5190001504818053,
            "unit": "ms/iter",
            "extra": "iterations: 1349\ncpu: 0.5189789547813103 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 233.50087959997836,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 233.46990699999992 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 237.63941760003036,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 237.6089239999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.4368469999999618,
            "unit": "ms/iter",
            "extra": "iterations: 494\ncpu: 1.4366547975708173 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 161.6207206391607,
            "unit": "ns/iter",
            "extra": "iterations: 4274733\ncpu: 161.6023545798048 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 48.8176235830189,
            "unit": "ns/iter",
            "extra": "iterations: 14456436\ncpu: 48.81352547751081 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 600.8752021302272,
            "unit": "ns/iter",
            "extra": "iterations: 1106341\ncpu: 600.8318755248212 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1972.557434600003,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1972.392586199999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 50.27441176922856,
            "unit": "ms/iter",
            "extra": "iterations: 13\ncpu: 50.267535076922236 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2012.854815799983,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2012.595615599997 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "467a2f90cc23df2649dd3da6193993efafa7bde9",
          "message": "Merge pull request #687 from disorderedmaterials/species-addition\n\nPeriodic Species",
          "timestamp": "2021-05-19T11:02:59+01:00",
          "tree_id": "376d7ac6885762bc9969e187db6ea7bdb08f7bc5",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/467a2f90cc23df2649dd3da6193993efafa7bde9"
        },
        "date": 1621419882045,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 18.649518599977455,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 18.6480682 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 17.998943600014172,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 17.9989024 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 457.01340020000316,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 456.501864 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 940.0908375999734,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 940.030285 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1795.25643019997,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1795.1493974 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 4967.558226799974,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 4967.2332004 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 9.011740456173909,
            "unit": "ns/iter",
            "extra": "iterations: 78974870\ncpu: 9.011407894688501 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 9.077358631153572,
            "unit": "ns/iter",
            "extra": "iterations: 80337254\ncpu: 9.076577847682008 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 18.024518466877282,
            "unit": "ns/iter",
            "extra": "iterations: 35976393\ncpu: 18.023445791244285 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 19.217385405554353,
            "unit": "ns/iter",
            "extra": "iterations: 35585733\ncpu: 19.21511901974873 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 12.998056278787754,
            "unit": "ns/iter",
            "extra": "iterations: 52384570\ncpu: 12.997799810135017 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 12.891548032557427,
            "unit": "ns/iter",
            "extra": "iterations: 56088775\ncpu: 12.891290387425999 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.334855117522634,
            "unit": "ns/iter",
            "extra": "iterations: 55258821\ncpu: 13.33385115111311 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.203921706513624,
            "unit": "ns/iter",
            "extra": "iterations: 53894650\ncpu: 13.20364483302157 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.569950186120359,
            "unit": "ns/iter",
            "extra": "iterations: 52775049\ncpu: 13.568787856549445 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 12.818684722650776,
            "unit": "ns/iter",
            "extra": "iterations: 51040961\ncpu: 12.817735778916795 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.708963921230842,
            "unit": "ns/iter",
            "extra": "iterations: 49782711\ncpu: 13.707353884363638 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.846500652876726,
            "unit": "ns/iter",
            "extra": "iterations: 52311558\ncpu: 13.846150730207675 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 13.994975793191783,
            "unit": "ns/iter",
            "extra": "iterations: 50540117\ncpu: 13.994695560360483 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 14.379988816293956,
            "unit": "ns/iter",
            "extra": "iterations: 49872552\ncpu: 14.3782668871647 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 15.556508991548146,
            "unit": "ns/iter",
            "extra": "iterations: 41948280\ncpu: 15.556038412063652 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 17.17772996316054,
            "unit": "ns/iter",
            "extra": "iterations: 41249128\ncpu: 17.175593239207345 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 17.525708713634405,
            "unit": "ns/iter",
            "extra": "iterations: 37201803\ncpu: 17.524195265482227 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 18.00446139478053,
            "unit": "ns/iter",
            "extra": "iterations: 39069172\ncpu: 18.00297521022467 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 21.704972996590683,
            "unit": "ns/iter",
            "extra": "iterations: 33622424\ncpu: 21.703903085631122 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 27.111521858947466,
            "unit": "ns/iter",
            "extra": "iterations: 26691153\ncpu: 27.02763724744298 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 26.390234777757044,
            "unit": "ns/iter",
            "extra": "iterations: 27248535\ncpu: 26.387173035174097 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 25.865519917324004,
            "unit": "ns/iter",
            "extra": "iterations: 26648660\ncpu: 25.86338322452244 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 26.03084709869168,
            "unit": "ns/iter",
            "extra": "iterations: 26654403\ncpu: 26.029655925889738 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 26.267475124717436,
            "unit": "ns/iter",
            "extra": "iterations: 27103612\ncpu: 26.265174508843902 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 26.270582767580326,
            "unit": "ns/iter",
            "extra": "iterations: 27558551\ncpu: 26.269800723557324 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 26.001490296818417,
            "unit": "ns/iter",
            "extra": "iterations: 25935773\ncpu: 25.999594883869463 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 26.4814160975435,
            "unit": "ns/iter",
            "extra": "iterations: 27125815\ncpu: 26.480415132227364 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 26.706060296288495,
            "unit": "ns/iter",
            "extra": "iterations: 27175206\ncpu: 26.703200557155093 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 26.55820536059923,
            "unit": "ns/iter",
            "extra": "iterations: 26273044\ncpu: 26.557542285545868 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 28.39625106370753,
            "unit": "ns/iter",
            "extra": "iterations: 23741241\ncpu: 28.393647113897647 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 30.097883744980052,
            "unit": "ns/iter",
            "extra": "iterations: 23235952\ncpu: 30.096734964850942 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 31.344572413220092,
            "unit": "ns/iter",
            "extra": "iterations: 22836300\ncpu: 31.34228237499056 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 31.778503170307296,
            "unit": "ns/iter",
            "extra": "iterations: 18773131\ncpu: 31.777468127186474 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 43.9471367441664,
            "unit": "ns/iter",
            "extra": "iterations: 17299048\ncpu: 43.94307779248895 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 39.61093163419682,
            "unit": "ns/iter",
            "extra": "iterations: 17989314\ncpu: 39.607483587200605 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 39.86615224628959,
            "unit": "ns/iter",
            "extra": "iterations: 18224654\ncpu: 39.866271041414855 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 39.60564416373332,
            "unit": "ns/iter",
            "extra": "iterations: 17628546\ncpu: 39.60504269609062 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 39.48461538899995,
            "unit": "ns/iter",
            "extra": "iterations: 17533690\ncpu: 39.48252273195216 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 39.74016909348646,
            "unit": "ns/iter",
            "extra": "iterations: 17366252\ncpu: 39.73696270214198 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 39.19872159076712,
            "unit": "ns/iter",
            "extra": "iterations: 17554942\ncpu: 39.19575530354937 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 39.33652993279008,
            "unit": "ns/iter",
            "extra": "iterations: 17565020\ncpu: 39.33378561481879 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 39.72696762788378,
            "unit": "ns/iter",
            "extra": "iterations: 18463730\ncpu: 39.72349963956304 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 38.817078529123144,
            "unit": "ns/iter",
            "extra": "iterations: 18314083\ncpu: 38.81610436078022 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 39.59657055931641,
            "unit": "ns/iter",
            "extra": "iterations: 17866237\ncpu: 39.59556469557594 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 39.938815434288294,
            "unit": "ns/iter",
            "extra": "iterations: 17274618\ncpu: 39.9362788224893 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 41.384629064716485,
            "unit": "ns/iter",
            "extra": "iterations: 16143598\ncpu: 41.38181513191849 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 42.05621399089949,
            "unit": "ns/iter",
            "extra": "iterations: 16249887\ncpu: 42.04702629624392 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 44.42301171881901,
            "unit": "ns/iter",
            "extra": "iterations: 16486652\ncpu: 44.4212448349119 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 52.67840563431552,
            "unit": "ns/iter",
            "extra": "iterations: 11588157\ncpu: 52.67635043260198 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 74.34848576539683,
            "unit": "ns/iter",
            "extra": "iterations: 10278394\ncpu: 74.2541120723719 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 273.8705531218082,
            "unit": "ns/iter",
            "extra": "iterations: 2687419\ncpu: 273.84480611323073 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1048.2201145452464,
            "unit": "ns/iter",
            "extra": "iterations: 654064\ncpu: 1048.1697265099522 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4554.6566648135,
            "unit": "ns/iter",
            "extra": "iterations: 154738\ncpu: 4554.336026056961 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 23883.31446801037,
            "unit": "ns/iter",
            "extra": "iterations: 28995\ncpu: 23881.821279531152 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 136348.1375812902,
            "unit": "ns/iter",
            "extra": "iterations: 5226\ncpu: 136333.29047072332 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1055103.7216336364,
            "unit": "ns/iter",
            "extra": "iterations: 661\ncpu: 1055050.8729198326 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 6288165.155340112,
            "unit": "ns/iter",
            "extra": "iterations: 103\ncpu: 6287662.398058213 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 398.3303345863432,
            "unit": "ns/iter",
            "extra": "iterations: 1804198\ncpu: 398.32232992166206 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1558.2603400784142,
            "unit": "ns/iter",
            "extra": "iterations: 465422\ncpu: 1558.1561464649114 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 6485.386690038798,
            "unit": "ns/iter",
            "extra": "iterations: 109527\ncpu: 6485.083961032467 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 25730.96625292221,
            "unit": "ns/iter",
            "extra": "iterations: 27795\ncpu: 25728.84403669719 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 104749.22715867516,
            "unit": "ns/iter",
            "extra": "iterations: 6775\ncpu: 104744.85623616245 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 425381.99752782943,
            "unit": "ns/iter",
            "extra": "iterations: 1618\ncpu: 425346.1940667507 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2453950.6095890505,
            "unit": "ns/iter",
            "extra": "iterations: 292\ncpu: 2453953.2534246426 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 10078067.73845914,
            "unit": "ns/iter",
            "extra": "iterations: 65\ncpu: 10077513.061538378 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 880.849860483029,
            "unit": "ns/iter",
            "extra": "iterations: 794168\ncpu: 880.7931218583421 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3593.8080976782776,
            "unit": "ns/iter",
            "extra": "iterations: 194599\ncpu: 3593.6248387710516 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 12403.939250100177,
            "unit": "ns/iter",
            "extra": "iterations: 56461\ncpu: 12403.625458280932 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 56562.88841032851,
            "unit": "ns/iter",
            "extra": "iterations: 12770\ncpu: 56559.315740015925 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 233333.81745498622,
            "unit": "ns/iter",
            "extra": "iterations: 3002\ncpu: 233321.5892738211 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 922652.1841108705,
            "unit": "ns/iter",
            "extra": "iterations: 793\ncpu: 922577.4388398491 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4549629.58333337,
            "unit": "ns/iter",
            "extra": "iterations: 156\ncpu: 4549422.570512809 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 19021870.24324416,
            "unit": "ns/iter",
            "extra": "iterations: 37\ncpu: 19020252.270270493 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1746.969377142058,
            "unit": "ns/iter",
            "extra": "iterations: 403816\ncpu: 1746.8382555421376 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 121.56039348249351,
            "unit": "ns/iter",
            "extra": "iterations: 5528378\ncpu: 121.55479183948688 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 20.00458756498609,
            "unit": "ns/iter",
            "extra": "iterations: 34743922\ncpu: 19.93253504886407 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 74.6018729018417,
            "unit": "ns/iter",
            "extra": "iterations: 9360875\ncpu: 74.59618112622975 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3567796180591328,
            "unit": "ms/iter",
            "extra": "iterations: 1927\ncpu: 0.3567428635184263 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3690735668589012,
            "unit": "ms/iter",
            "extra": "iterations: 1907\ncpu: 0.3690604299947549 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 253.92294640000728,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 253.90336320000134 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 255.77366500001517,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 255.75543959999723 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 768.8903833449774,
            "unit": "ns/iter",
            "extra": "iterations: 886982\ncpu: 768.8576949701244 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 52582.43157740161,
            "unit": "ns/iter",
            "extra": "iterations: 13687\ncpu: 52579.32023087548 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 19.645104534878428,
            "unit": "ns/iter",
            "extra": "iterations: 31885530\ncpu: 19.643185294394783 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 73.26764121444225,
            "unit": "ns/iter",
            "extra": "iterations: 9641258\ncpu: 73.26354517221604 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 167.64558349013458,
            "unit": "ns/iter",
            "extra": "iterations: 4052125\ncpu: 167.62613542277538 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 2.963453793859914,
            "unit": "ms/iter",
            "extra": "iterations: 228\ncpu: 2.963132679824467 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 24.272733413791403,
            "unit": "ms/iter",
            "extra": "iterations: 29\ncpu: 24.269770724137334 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2468.208080600016,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2468.0255527999975 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2483.070651199978,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2482.896594599998 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.744975951613117,
            "unit": "ms/iter",
            "extra": "iterations: 248\ncpu: 2.7447697943547986 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 41.18071260086163,
            "unit": "ns/iter",
            "extra": "iterations: 16812469\ncpu: 41.17687110679695 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 123.93786622800214,
            "unit": "ns/iter",
            "extra": "iterations: 5385477\ncpu: 123.92888912161557 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4322684861537989,
            "unit": "ms/iter",
            "extra": "iterations: 1625\ncpu: 0.43225785538461775 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 211.05693439999413,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 211.03897419999953 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 212.11803360001795,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 212.1085007999966 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.3139635656934145,
            "unit": "ms/iter",
            "extra": "iterations: 548\ncpu: 1.2991839762773796 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 127.33975961396972,
            "unit": "ns/iter",
            "extra": "iterations: 5467289\ncpu: 127.33650205796408 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 40.75088528527631,
            "unit": "ns/iter",
            "extra": "iterations: 16575448\ncpu: 40.74985013979628 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 588.1917480117887,
            "unit": "ns/iter",
            "extra": "iterations: 1133545\ncpu: 588.1329651668049 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1886.2386329999936,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1886.1202511999977 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 47.049272666663455,
            "unit": "ms/iter",
            "extra": "iterations: 15\ncpu: 47.0462606666653 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1926.9111161999717,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1926.6534087999958 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "7dd3f34b965fae12d0d731558cbf45721886fd22",
          "message": "Merge pull request #688 from disorderedmaterials/speciesreflist-keyword\n\nUpdating SpeciesRefList",
          "timestamp": "2021-05-20T11:20:26+01:00",
          "tree_id": "45f53b67c659259c6d7ed6d34ae089023e9bc011",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/7dd3f34b965fae12d0d731558cbf45721886fd22"
        },
        "date": 1621507435081,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 16.54459679998581,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 16.544493999999997 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 14.85573539998768,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 14.847685199999997 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 410.66979239999455,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 410.0708694 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 777.3432339999999,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 777.285571 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1591.9968791999963,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1591.8363816000003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 3971.5957446000175,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3969.6122712000006 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.76913877372539,
            "unit": "ns/iter",
            "extra": "iterations: 77124168\ncpu: 8.768783541366703 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.565621747348118,
            "unit": "ns/iter",
            "extra": "iterations: 75046432\ncpu: 8.565066517752568 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 17.555827906897335,
            "unit": "ns/iter",
            "extra": "iterations: 41081739\ncpu: 17.554413994013224 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 18.771869992130288,
            "unit": "ns/iter",
            "extra": "iterations: 37470441\ncpu: 18.769271677373546 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 13.188546719226293,
            "unit": "ns/iter",
            "extra": "iterations: 54606991\ncpu: 13.187339968979405 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 13.795289332050075,
            "unit": "ns/iter",
            "extra": "iterations: 54345414\ncpu: 13.794214245934345 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.402008463600458,
            "unit": "ns/iter",
            "extra": "iterations: 53001807\ncpu: 13.400083661298654 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.627068381451457,
            "unit": "ns/iter",
            "extra": "iterations: 51781249\ncpu: 13.625789404963998 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.191110996862793,
            "unit": "ns/iter",
            "extra": "iterations: 53684310\ncpu: 13.190557799848774 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.189909559667363,
            "unit": "ns/iter",
            "extra": "iterations: 49285976\ncpu: 13.188919886663172 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.121227955166175,
            "unit": "ns/iter",
            "extra": "iterations: 54926077\ncpu: 13.120075515314857 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 14.151598282918716,
            "unit": "ns/iter",
            "extra": "iterations: 53032507\ncpu: 14.150387648089177 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 14.279915662938867,
            "unit": "ns/iter",
            "extra": "iterations: 47299729\ncpu: 14.278198930061487 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 16.593550936399605,
            "unit": "ns/iter",
            "extra": "iterations: 40927089\ncpu: 16.590636094348046 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 17.477146977167735,
            "unit": "ns/iter",
            "extra": "iterations: 37969244\ncpu: 17.47533232423596 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 17.250606674102354,
            "unit": "ns/iter",
            "extra": "iterations: 39845116\ncpu: 17.249002964378267 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 17.355637180129275,
            "unit": "ns/iter",
            "extra": "iterations: 41212263\ncpu: 17.35543335729941 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 19.040543185192,
            "unit": "ns/iter",
            "extra": "iterations: 36846365\ncpu: 19.00863219479038 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 21.85866806366236,
            "unit": "ns/iter",
            "extra": "iterations: 30041646\ncpu: 21.854427683489696 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 25.113591264003066,
            "unit": "ns/iter",
            "extra": "iterations: 27685166\ncpu: 25.110271543974072 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 25.532355837992657,
            "unit": "ns/iter",
            "extra": "iterations: 26316920\ncpu: 25.530193692878928 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 25.49237022676579,
            "unit": "ns/iter",
            "extra": "iterations: 26532571\ncpu: 25.489809826571225 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 26.000382065723834,
            "unit": "ns/iter",
            "extra": "iterations: 27492652\ncpu: 25.99818831591796 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 26.03268138002443,
            "unit": "ns/iter",
            "extra": "iterations: 26749666\ncpu: 26.028879612926755 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 25.06983610873998,
            "unit": "ns/iter",
            "extra": "iterations: 27146902\ncpu: 25.06905535666656 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 25.97907804936456,
            "unit": "ns/iter",
            "extra": "iterations: 27515790\ncpu: 25.977801873033638 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 26.101401224432028,
            "unit": "ns/iter",
            "extra": "iterations: 25816207\ncpu: 26.099271360816072 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 26.921335569689457,
            "unit": "ns/iter",
            "extra": "iterations: 26202122\ncpu: 26.91949949702553 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 29.656191251629203,
            "unit": "ns/iter",
            "extra": "iterations: 24212374\ncpu: 29.652715095182415 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 32.38148168724512,
            "unit": "ns/iter",
            "extra": "iterations: 21294089\ncpu: 32.378072290390115 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 30.71198956624144,
            "unit": "ns/iter",
            "extra": "iterations: 21863635\ncpu: 30.70871774066835 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 32.60991820154646,
            "unit": "ns/iter",
            "extra": "iterations: 21188297\ncpu: 32.6058732799525 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 33.53110230850008,
            "unit": "ns/iter",
            "extra": "iterations: 18890093\ncpu: 33.527013763246 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 43.79106582555685,
            "unit": "ns/iter",
            "extra": "iterations: 15656108\ncpu: 43.78664921064725 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 36.9919170061571,
            "unit": "ns/iter",
            "extra": "iterations: 18926032\ncpu: 36.98742055387039 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 38.76643391454491,
            "unit": "ns/iter",
            "extra": "iterations: 18710606\ncpu: 38.731870469614236 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 35.96814736208455,
            "unit": "ns/iter",
            "extra": "iterations: 19103096\ncpu: 35.96258240025552 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 37.3568070263912,
            "unit": "ns/iter",
            "extra": "iterations: 18669481\ncpu: 37.353062733774166 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 36.55486739064818,
            "unit": "ns/iter",
            "extra": "iterations: 18839961\ncpu: 36.55177815920086 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 36.349802287464136,
            "unit": "ns/iter",
            "extra": "iterations: 18827587\ncpu: 36.34842664649509 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 38.715027334533666,
            "unit": "ns/iter",
            "extra": "iterations: 18870266\ncpu: 38.71466740320454 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 38.364763255403155,
            "unit": "ns/iter",
            "extra": "iterations: 18381454\ncpu: 38.361955860510584 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 38.01178232000942,
            "unit": "ns/iter",
            "extra": "iterations: 18527251\ncpu: 38.00825243852954 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 40.90148522360808,
            "unit": "ns/iter",
            "extra": "iterations: 17170815\ncpu: 40.89466533766737 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 43.0605445553143,
            "unit": "ns/iter",
            "extra": "iterations: 16986961\ncpu: 43.05720664220097 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 43.192343715218776,
            "unit": "ns/iter",
            "extra": "iterations: 16203368\ncpu: 43.182962887715874 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 43.12353548238759,
            "unit": "ns/iter",
            "extra": "iterations: 15107449\ncpu: 43.12107967400713 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 50.30916049061643,
            "unit": "ns/iter",
            "extra": "iterations: 10630042\ncpu: 50.307993891274066 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 60.5458140113291,
            "unit": "ns/iter",
            "extra": "iterations: 8949642\ncpu: 60.537230651238474 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 85.4398558522252,
            "unit": "ns/iter",
            "extra": "iterations: 8115561\ncpu: 85.4277078319043 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 290.067795597716,
            "unit": "ns/iter",
            "extra": "iterations: 2394005\ncpu: 290.0176933632139 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1096.3940391289436,
            "unit": "ns/iter",
            "extra": "iterations: 655374\ncpu: 1096.2061891377987 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4600.651295550017,
            "unit": "ns/iter",
            "extra": "iterations: 148547\ncpu: 4600.15737106773 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 28097.39713157435,
            "unit": "ns/iter",
            "extra": "iterations: 26077\ncpu: 28094.97764313376 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 272153.28307936905,
            "unit": "ns/iter",
            "extra": "iterations: 2494\ncpu: 272128.0180433017 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1380124.2047244038,
            "unit": "ns/iter",
            "extra": "iterations: 508\ncpu: 1379993.8425196912 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 6937377.777778356,
            "unit": "ns/iter",
            "extra": "iterations: 108\ncpu: 6936328.851851827 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 438.2274810519645,
            "unit": "ns/iter",
            "extra": "iterations: 1595412\ncpu: 438.1652325543512 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1732.2466239416024,
            "unit": "ns/iter",
            "extra": "iterations: 419424\ncpu: 1732.0586995498554 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7327.353541258544,
            "unit": "ns/iter",
            "extra": "iterations: 90674\ncpu: 7326.708427994811 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 28262.46954423879,
            "unit": "ns/iter",
            "extra": "iterations: 23214\ncpu: 28259.81601619757 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 117450.50124521996,
            "unit": "ns/iter",
            "extra": "iterations: 6023\ncpu: 117432.87713763818 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 572052.9058630036,
            "unit": "ns/iter",
            "extra": "iterations: 1211\ncpu: 571977.1090008247 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2475761.577319883,
            "unit": "ns/iter",
            "extra": "iterations: 291\ncpu: 2475479.1821306003 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 9629159.705880016,
            "unit": "ns/iter",
            "extra": "iterations: 68\ncpu: 9627524.705882246 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 897.7696699101763,
            "unit": "ns/iter",
            "extra": "iterations: 762853\ncpu: 897.6278024730849 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3730.975761307296,
            "unit": "ns/iter",
            "extra": "iterations: 193286\ncpu: 3730.4556460375434 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 12413.3821107882,
            "unit": "ns/iter",
            "extra": "iterations: 56358\ncpu: 12412.305280527973 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 56301.61583086773,
            "unit": "ns/iter",
            "extra": "iterations: 12583\ncpu: 56294.23197965489 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 234032.35663628622,
            "unit": "ns/iter",
            "extra": "iterations: 2961\ncpu: 234023.7355623133 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 1033138.0469252982,
            "unit": "ns/iter",
            "extra": "iterations: 618\ncpu: 1033072.8899676409 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4579403.245399612,
            "unit": "ns/iter",
            "extra": "iterations: 163\ncpu: 4578834.319018399 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 17654179.35897135,
            "unit": "ns/iter",
            "extra": "iterations: 39\ncpu: 17652215.153846055 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1748.1939432006095,
            "unit": "ns/iter",
            "extra": "iterations: 410811\ncpu: 1748.0042817743324 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 118.07524345473283,
            "unit": "ns/iter",
            "extra": "iterations: 6111814\ncpu: 118.06648811629371 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 17.849514555648962,
            "unit": "ns/iter",
            "extra": "iterations: 41629282\ncpu: 17.84687713326417 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 78.17229433283865,
            "unit": "ns/iter",
            "extra": "iterations: 8885655\ncpu: 78.16041912498301 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.4701283084815506,
            "unit": "ms/iter",
            "extra": "iterations: 1462\ncpu: 0.47006741655266343 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.38961396000003234,
            "unit": "ms/iter",
            "extra": "iterations: 1800\ncpu: 0.3895539077777717 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 252.30460899997524,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 252.27382919999854 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 253.72730799999772,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 253.70754820000056 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 769.1143826648854,
            "unit": "ns/iter",
            "extra": "iterations: 878210\ncpu: 769.0514136709915 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 45921.23406149385,
            "unit": "ns/iter",
            "extra": "iterations: 14603\ncpu: 45915.852153667096 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 17.73300684375561,
            "unit": "ns/iter",
            "extra": "iterations: 41880051\ncpu: 17.731242519260412 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 77.52810979589876,
            "unit": "ns/iter",
            "extra": "iterations: 8831512\ncpu: 77.51921007410837 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 170.95114365351964,
            "unit": "ns/iter",
            "extra": "iterations: 4221908\ncpu: 170.94052878461707 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.5530597967911923,
            "unit": "ms/iter",
            "extra": "iterations: 187\ncpu: 3.5526154278074578 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 25.647579461537344,
            "unit": "ms/iter",
            "extra": "iterations: 26\ncpu: 25.642356115384885 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2470.9518033999757,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2470.689348799999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2497.929825400024,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2497.662249999996 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.802009301587662,
            "unit": "ms/iter",
            "extra": "iterations: 252\ncpu: 2.8017197301586467 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 38.04432469778682,
            "unit": "ns/iter",
            "extra": "iterations: 19027022\ncpu: 38.03989930741549 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 119.70356830529556,
            "unit": "ns/iter",
            "extra": "iterations: 5980430\ncpu: 119.69091001817705 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4433584383116488,
            "unit": "ms/iter",
            "extra": "iterations: 1540\ncpu: 0.4433109629870133 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 214.40931919996729,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 214.38645939999788 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 214.02684739996403,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 214.00909939999906 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.383992836852157,
            "unit": "ms/iter",
            "extra": "iterations: 521\ncpu: 1.3837978099808417 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 122.84950554795029,
            "unit": "ns/iter",
            "extra": "iterations: 5780439\ncpu: 122.83143754306272 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 39.87200281144372,
            "unit": "ns/iter",
            "extra": "iterations: 18339270\ncpu: 39.87015655475837 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 600.6494983416554,
            "unit": "ns/iter",
            "extra": "iterations: 1295603\ncpu: 600.5632813446813 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1919.3566976000056,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1919.0127017999998 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 46.67250266667603,
            "unit": "ms/iter",
            "extra": "iterations: 15\ncpu: 46.66129593333229 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1991.923757199993,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1991.7166520000023 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "adam.washington@stfc.ac.uk",
            "name": "Adam Washington",
            "username": "rprospero"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "154b3294cefc8dc92e408cf0c6b45928a45a6816",
          "message": "Show all parameters in a single cell (#692)\n\n* Single Parameters cell for SpeciesBondModel\r\n\r\n* Move angle table over to combined parameters\r\n\r\n* Single parameters cell for torsion model\r\n\r\n* Single cell for improper parameters\r\n\r\n* Better parameter formatting\r\n\r\n* Document utility functions\r\n\r\n* Fix formatting\r\n\r\n* Better string splitting\r\n\r\n* Move generic string splitting into algorithms library\r\n\r\n* Switch from O(n²) string joiner to O(n)",
          "timestamp": "2021-05-21T09:47:48+01:00",
          "tree_id": "5f9c28b50c964c7f4fcd057c1ec667e2d2f2e851",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/154b3294cefc8dc92e408cf0c6b45928a45a6816"
        },
        "date": 1621588151921,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 17.089712200004215,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 17.051235400000003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 16.58038939999642,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 16.580370600000002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 370.98856100001285,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 370.9637886 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 708.0064116000358,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 707.9671987999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1486.7738658000235,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1486.6145728000001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 3603.6481787999946,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3603.477109000001 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 7.931019360619382,
            "unit": "ns/iter",
            "extra": "iterations: 86983682\ncpu: 7.931053918825859 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.069347355116511,
            "unit": "ns/iter",
            "extra": "iterations: 83596411\ncpu: 8.06883893615957 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 16.303563290080575,
            "unit": "ns/iter",
            "extra": "iterations: 38345124\ncpu: 16.303229557948438 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 18.149216089878824,
            "unit": "ns/iter",
            "extra": "iterations: 38780275\ncpu: 18.148009290805803 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 12.147777705277383,
            "unit": "ns/iter",
            "extra": "iterations: 57468750\ncpu: 12.14744529853175 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 12.57678133719182,
            "unit": "ns/iter",
            "extra": "iterations: 57397457\ncpu: 12.576420728883472 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 12.226324505543056,
            "unit": "ns/iter",
            "extra": "iterations: 54117233\ncpu: 12.225840315967387 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.469905665274625,
            "unit": "ns/iter",
            "extra": "iterations: 50840981\ncpu: 13.469811292586963 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 12.381381737788901,
            "unit": "ns/iter",
            "extra": "iterations: 56865898\ncpu: 12.380235180670168 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 12.571616330770913,
            "unit": "ns/iter",
            "extra": "iterations: 58702805\ncpu: 12.571342016791128 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 12.676411633109888,
            "unit": "ns/iter",
            "extra": "iterations: 56245174\ncpu: 12.675810603768467 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 12.837312707337643,
            "unit": "ns/iter",
            "extra": "iterations: 54480512\ncpu: 12.836545295315885 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 13.427864886018453,
            "unit": "ns/iter",
            "extra": "iterations: 54497779\ncpu: 13.427406647158946 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 15.583605410351234,
            "unit": "ns/iter",
            "extra": "iterations: 43427961\ncpu: 15.582879817912668 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 16.19434216722719,
            "unit": "ns/iter",
            "extra": "iterations: 43054825\ncpu: 16.19365875950035 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 16.596405946799354,
            "unit": "ns/iter",
            "extra": "iterations: 40824660\ncpu: 16.59484270046585 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 15.459473715915165,
            "unit": "ns/iter",
            "extra": "iterations: 46084388\ncpu: 15.459141000201639 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 17.405704520925944,
            "unit": "ns/iter",
            "extra": "iterations: 42515437\ncpu: 17.403935775139875 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 15.812910274350612,
            "unit": "ns/iter",
            "extra": "iterations: 34197848\ncpu: 15.812907291710285 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 23.218048875974617,
            "unit": "ns/iter",
            "extra": "iterations: 29797911\ncpu: 23.21667968603576 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 23.740273257933005,
            "unit": "ns/iter",
            "extra": "iterations: 31274115\ncpu: 23.739847154747586 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 23.451116007293948,
            "unit": "ns/iter",
            "extra": "iterations: 29521402\ncpu: 23.44961949977859 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 23.935829724968375,
            "unit": "ns/iter",
            "extra": "iterations: 31229288\ncpu: 23.917793482835794 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 24.78907432136049,
            "unit": "ns/iter",
            "extra": "iterations: 29340420\ncpu: 24.787853718522065 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 23.637923357040684,
            "unit": "ns/iter",
            "extra": "iterations: 29228019\ncpu: 23.63747549910919 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 24.162288529816465,
            "unit": "ns/iter",
            "extra": "iterations: 30649948\ncpu: 24.160360402569083 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 23.342684148295614,
            "unit": "ns/iter",
            "extra": "iterations: 30268663\ncpu: 23.34220553448303 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 25.53426841825757,
            "unit": "ns/iter",
            "extra": "iterations: 28207634\ncpu: 25.532815159186978 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 28.054523190651498,
            "unit": "ns/iter",
            "extra": "iterations: 25602812\ncpu: 28.053651059891415 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 32.32547687743593,
            "unit": "ns/iter",
            "extra": "iterations: 22719150\ncpu: 32.3231656554052 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 27.9873053010634,
            "unit": "ns/iter",
            "extra": "iterations: 24542764\ncpu: 27.986247840707744 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 30.643241183250296,
            "unit": "ns/iter",
            "extra": "iterations: 22576866\ncpu: 30.64116436710013 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 28.92190073031395,
            "unit": "ns/iter",
            "extra": "iterations: 22500825\ncpu: 28.921553365265623 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 40.31168216408551,
            "unit": "ns/iter",
            "extra": "iterations: 13175855\ncpu: 40.30757647226629 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 34.6934548444548,
            "unit": "ns/iter",
            "extra": "iterations: 19695865\ncpu: 34.692316636004854 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 33.9181561957364,
            "unit": "ns/iter",
            "extra": "iterations: 20240066\ncpu: 33.91718821470239 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 33.33856703459109,
            "unit": "ns/iter",
            "extra": "iterations: 21115904\ncpu: 33.33865227839627 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 35.09696908206153,
            "unit": "ns/iter",
            "extra": "iterations: 20343309\ncpu: 35.09499895026877 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 36.90158030139269,
            "unit": "ns/iter",
            "extra": "iterations: 19586770\ncpu: 36.900351665946125 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 33.55884368666799,
            "unit": "ns/iter",
            "extra": "iterations: 19794358\ncpu: 33.556070926876814 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 34.469720133097894,
            "unit": "ns/iter",
            "extra": "iterations: 18417254\ncpu: 34.4676960528426 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 36.47994159307074,
            "unit": "ns/iter",
            "extra": "iterations: 20040774\ncpu: 36.47897466435236 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 35.88023520548261,
            "unit": "ns/iter",
            "extra": "iterations: 19319108\ncpu: 35.87903483949671 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 40.55424372796247,
            "unit": "ns/iter",
            "extra": "iterations: 17701005\ncpu: 40.55347591845761 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 44.00518434674066,
            "unit": "ns/iter",
            "extra": "iterations: 16575666\ncpu: 44.00199859239454 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 41.28433846625844,
            "unit": "ns/iter",
            "extra": "iterations: 17654829\ncpu: 41.28399838933591 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 41.123635902611696,
            "unit": "ns/iter",
            "extra": "iterations: 13503618\ncpu: 41.12197731008098 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 45.45912228580213,
            "unit": "ns/iter",
            "extra": "iterations: 15911922\ncpu: 45.45870775384717 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 38.88410322778572,
            "unit": "ns/iter",
            "extra": "iterations: 13497218\ncpu: 38.8796131173106 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 71.59450697399757,
            "unit": "ns/iter",
            "extra": "iterations: 10251508\ncpu: 71.59336831225251 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 249.9338836760789,
            "unit": "ns/iter",
            "extra": "iterations: 2779722\ncpu: 249.9242503387037 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 997.4952581765809,
            "unit": "ns/iter",
            "extra": "iterations: 733684\ncpu: 997.4786611129533 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4325.3883084613435,
            "unit": "ns/iter",
            "extra": "iterations: 165778\ncpu: 4324.956984642116 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 25447.92993389489,
            "unit": "ns/iter",
            "extra": "iterations: 26475\ncpu: 25447.306742209545 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 236559.02083330962,
            "unit": "ns/iter",
            "extra": "iterations: 2832\ncpu: 236544.82521186295 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1165703.3923205812,
            "unit": "ns/iter",
            "extra": "iterations: 599\ncpu: 1165680.8497495973 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 5031060.593751135,
            "unit": "ns/iter",
            "extra": "iterations: 128\ncpu: 5030696.539062518 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 432.51639280306546,
            "unit": "ns/iter",
            "extra": "iterations: 1640598\ncpu: 432.5123784132438 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1705.8988139024113,
            "unit": "ns/iter",
            "extra": "iterations: 403761\ncpu: 1705.7350908086885 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7168.4417712474415,
            "unit": "ns/iter",
            "extra": "iterations: 95142\ncpu: 7168.327226671673 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 29110.975965372625,
            "unit": "ns/iter",
            "extra": "iterations: 23799\ncpu: 29108.96739358818 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 118882.13643305033,
            "unit": "ns/iter",
            "extra": "iterations: 5893\ncpu: 118879.69285593015 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 518533.13599985995,
            "unit": "ns/iter",
            "extra": "iterations: 1000\ncpu: 518509.1390000025 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2088803.6617641186,
            "unit": "ns/iter",
            "extra": "iterations: 340\ncpu: 2088654.5529411982 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 8405743.790696528,
            "unit": "ns/iter",
            "extra": "iterations: 86\ncpu: 8405028.697674336 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 847.1854307328546,
            "unit": "ns/iter",
            "extra": "iterations: 811791\ncpu: 847.1258082437439 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3428.4004449331414,
            "unit": "ns/iter",
            "extra": "iterations: 211717\ncpu: 3428.3404261348824 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 11615.492269515898,
            "unit": "ns/iter",
            "extra": "iterations: 62868\ncpu: 11614.937344913125 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 53654.98941103671,
            "unit": "ns/iter",
            "extra": "iterations: 12938\ncpu: 53654.610140669996 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 221918.22194036635,
            "unit": "ns/iter",
            "extra": "iterations: 3154\ncpu: 221902.62238427548 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 908604.8398487156,
            "unit": "ns/iter",
            "extra": "iterations: 793\ncpu: 908597.3972257314 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 3825758.3169393176,
            "unit": "ns/iter",
            "extra": "iterations: 183\ncpu: 3825529.3661202323 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 15430893.543477554,
            "unit": "ns/iter",
            "extra": "iterations: 46\ncpu: 15430269.586956553 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1539.435252841006,
            "unit": "ns/iter",
            "extra": "iterations: 417555\ncpu: 1539.340189915109 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 102.43114971697325,
            "unit": "ns/iter",
            "extra": "iterations: 6973951\ncpu: 102.42301888843265 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 16.423772614040082,
            "unit": "ns/iter",
            "extra": "iterations: 44100594\ncpu: 16.42279537096481 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 75.18927246829894,
            "unit": "ns/iter",
            "extra": "iterations: 9731521\ncpu: 75.18650897429124 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.416283878465998,
            "unit": "ms/iter",
            "extra": "iterations: 1695\ncpu: 0.4162764536873164 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3533198882536994,
            "unit": "ms/iter",
            "extra": "iterations: 1924\ncpu: 0.3532886517671489 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 230.41205100003026,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 230.39687339999946 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 233.64602479996393,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 233.63436599999832 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 709.1845410021444,
            "unit": "ns/iter",
            "extra": "iterations: 958665\ncpu: 709.1563778796622 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 44487.20864626871,
            "unit": "ns/iter",
            "extra": "iterations: 16377\ncpu: 44485.97557550213 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 16.350335300434033,
            "unit": "ns/iter",
            "extra": "iterations: 42832632\ncpu: 16.349872989360165 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 78.09119349839301,
            "unit": "ns/iter",
            "extra": "iterations: 7871548\ncpu: 78.08509812809339 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 181.01645408359687,
            "unit": "ns/iter",
            "extra": "iterations: 3992322\ncpu: 181.00944588137168 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.0802848846149526,
            "unit": "ms/iter",
            "extra": "iterations: 234\ncpu: 3.080150824786309 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 24.04228642857171,
            "unit": "ms/iter",
            "extra": "iterations: 28\ncpu: 24.04065835714318 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2229.4600878000438,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2229.357923600003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2276.158014600014,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2276.0471254000036 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.700401922764256,
            "unit": "ms/iter",
            "extra": "iterations: 246\ncpu: 2.700314516260151 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 41.75999273942411,
            "unit": "ns/iter",
            "extra": "iterations: 16692895\ncpu: 41.758386906525416 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 131.3882623135061,
            "unit": "ns/iter",
            "extra": "iterations: 4978562\ncpu: 131.37324030513608 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.45814512437456917,
            "unit": "ms/iter",
            "extra": "iterations: 1399\ncpu: 0.45813568548963984 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 197.0489882000038,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 197.0411879999972 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 206.98517079999874,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 206.97493360000294 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.212318989932659,
            "unit": "ms/iter",
            "extra": "iterations: 596\ncpu: 1.2122525755033382 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 141.65214581318818,
            "unit": "ns/iter",
            "extra": "iterations: 4994610\ncpu: 141.64806201084642 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 43.90286040797679,
            "unit": "ns/iter",
            "extra": "iterations: 15581134\ncpu: 43.900062922249326 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 550.2851136176372,
            "unit": "ns/iter",
            "extra": "iterations: 1269301\ncpu: 550.2748103089697 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1758.6019862000285,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1758.5165124000014 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 46.146078687499426,
            "unit": "ms/iter",
            "extra": "iterations: 16\ncpu: 45.34361887500005 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1798.1558244000098,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1797.7403163999952 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "14754c048d4fd12c37191ae8767ebfacd754a659",
          "message": "Merge pull request #689 from disorderedmaterials/node-keywords\n\nNode Keywords",
          "timestamp": "2021-05-21T12:15:37+01:00",
          "tree_id": "ee1b8be9546efc3002f2f83550caf5423b88399d",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/14754c048d4fd12c37191ae8767ebfacd754a659"
        },
        "date": 1621597019442,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 19.848318999993353,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 19.8466218 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 18.31600960003925,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 18.312531200000002 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 471.0324122000202,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 471.0036156 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 981.4781556000071,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 981.1660264 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1863.7487139999848,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1863.6593585999995 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 5175.532064400022,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 5174.792278 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 9.574833354650991,
            "unit": "ns/iter",
            "extra": "iterations: 73507302\ncpu: 9.574206260488216 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 9.487739549914057,
            "unit": "ns/iter",
            "extra": "iterations: 73564836\ncpu: 9.487325792447946 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 18.79593519174912,
            "unit": "ns/iter",
            "extra": "iterations: 34602321\ncpu: 18.794915953759354 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 19.869985852009002,
            "unit": "ns/iter",
            "extra": "iterations: 37327553\ncpu: 19.869317284205724 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 13.723881833932783,
            "unit": "ns/iter",
            "extra": "iterations: 51005572\ncpu: 13.723195634390638 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 13.723597740179057,
            "unit": "ns/iter",
            "extra": "iterations: 50993599\ncpu: 13.723336981961241 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.724576970884627,
            "unit": "ns/iter",
            "extra": "iterations: 51008723\ncpu: 13.724304429263958 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.724302264683473,
            "unit": "ns/iter",
            "extra": "iterations: 51003259\ncpu: 13.723706243948026 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.724509538588313,
            "unit": "ns/iter",
            "extra": "iterations: 50994379\ncpu: 13.724064450319174 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.782397698820319,
            "unit": "ns/iter",
            "extra": "iterations: 50804596\ncpu: 13.781720515994223 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 14.030109695030811,
            "unit": "ns/iter",
            "extra": "iterations: 49894328\ncpu: 14.029570535552766 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 14.182594950977673,
            "unit": "ns/iter",
            "extra": "iterations: 49344015\ncpu: 14.18223630565932 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 14.286140135308742,
            "unit": "ns/iter",
            "extra": "iterations: 49004894\ncpu: 14.285882977320632 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 14.68610319374454,
            "unit": "ns/iter",
            "extra": "iterations: 47643508\ncpu: 14.685545006467681 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 15.807875156012104,
            "unit": "ns/iter",
            "extra": "iterations: 44283237\ncpu: 15.806967340711932 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 18.16969257592921,
            "unit": "ns/iter",
            "extra": "iterations: 38539435\ncpu: 18.166856390084522 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 18.040529057638054,
            "unit": "ns/iter",
            "extra": "iterations: 38763867\ncpu: 18.038940258462887 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 18.41686064450272,
            "unit": "ns/iter",
            "extra": "iterations: 38023244\ncpu: 18.415382469733387 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 20.44224648331441,
            "unit": "ns/iter",
            "extra": "iterations: 29003091\ncpu: 20.44056593829938 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 27.416701614321397,
            "unit": "ns/iter",
            "extra": "iterations: 25535807\ncpu: 27.41488839573352 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 27.437121562378763,
            "unit": "ns/iter",
            "extra": "iterations: 25516259\ncpu: 27.435785120381553 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 27.41753900115161,
            "unit": "ns/iter",
            "extra": "iterations: 25536745\ncpu: 27.415690527512186 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 27.434963591191252,
            "unit": "ns/iter",
            "extra": "iterations: 25506190\ncpu: 27.43375619800535 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 27.41482162652583,
            "unit": "ns/iter",
            "extra": "iterations: 25536617\ncpu: 27.41336090054572 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 27.446148543108226,
            "unit": "ns/iter",
            "extra": "iterations: 25506413\ncpu: 27.445258531648598 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 27.48353361209743,
            "unit": "ns/iter",
            "extra": "iterations: 25472496\ncpu: 27.481947548445834 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 27.608152359526255,
            "unit": "ns/iter",
            "extra": "iterations: 25356340\ncpu: 27.607044155426145 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 27.596633870344085,
            "unit": "ns/iter",
            "extra": "iterations: 25365749\ncpu: 27.5947888627297 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 27.67783447399786,
            "unit": "ns/iter",
            "extra": "iterations: 25262315\ncpu: 27.676330850913764 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 28.795797111927993,
            "unit": "ns/iter",
            "extra": "iterations: 24306762\ncpu: 28.793258764783506 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 31.322402565137207,
            "unit": "ns/iter",
            "extra": "iterations: 22330483\ncpu: 31.321695459968485 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 32.574254211689116,
            "unit": "ns/iter",
            "extra": "iterations: 21452837\ncpu: 32.57317952865635 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 32.07020483358064,
            "unit": "ns/iter",
            "extra": "iterations: 21533090\ncpu: 32.06830673163946 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 35.438155481240024,
            "unit": "ns/iter",
            "extra": "iterations: 19537649\ncpu: 35.43573410495813 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 40.883636178448775,
            "unit": "ns/iter",
            "extra": "iterations: 17122229\ncpu: 40.88117732802244 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 40.88269951882021,
            "unit": "ns/iter",
            "extra": "iterations: 17122044\ncpu: 40.88202202961263 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 40.84172214894744,
            "unit": "ns/iter",
            "extra": "iterations: 17140004\ncpu: 40.840345194785776 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 40.88914706635123,
            "unit": "ns/iter",
            "extra": "iterations: 17123020\ncpu: 40.887710228685606 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 40.90036811819955,
            "unit": "ns/iter",
            "extra": "iterations: 17118958\ncpu: 40.898050278527485 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 41.1219718203387,
            "unit": "ns/iter",
            "extra": "iterations: 17134056\ncpu: 40.98991254610067 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 40.934350359324185,
            "unit": "ns/iter",
            "extra": "iterations: 17101038\ncpu: 40.93229779385371 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 40.96740275015174,
            "unit": "ns/iter",
            "extra": "iterations: 17089049\ncpu: 40.96448848616445 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 40.94546191989569,
            "unit": "ns/iter",
            "extra": "iterations: 17095908\ncpu: 40.94489459115062 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 41.04648028093129,
            "unit": "ns/iter",
            "extra": "iterations: 17052199\ncpu: 41.045301605969144 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 41.69107322563418,
            "unit": "ns/iter",
            "extra": "iterations: 16780339\ncpu: 41.68885688185377 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 43.53130522478981,
            "unit": "ns/iter",
            "extra": "iterations: 16078035\ncpu: 43.52816441810224 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 44.74770428380058,
            "unit": "ns/iter",
            "extra": "iterations: 15643702\ncpu: 44.74287806044919 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 45.16119942880156,
            "unit": "ns/iter",
            "extra": "iterations: 13873337\ncpu: 45.157777613274185 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 48.21922834926425,
            "unit": "ns/iter",
            "extra": "iterations: 11215981\ncpu: 48.21592814752371 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 116.93935301846165,
            "unit": "ns/iter",
            "extra": "iterations: 5986415\ncpu: 116.93317202365704 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 438.3225091627442,
            "unit": "ns/iter",
            "extra": "iterations: 1597505\ncpu: 438.29940751359123 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1723.3995494830083,
            "unit": "ns/iter",
            "extra": "iterations: 406200\ncpu: 1723.2934416543453 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 6864.448097343718,
            "unit": "ns/iter",
            "extra": "iterations: 101989\ncpu: 6864.308847032523 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 27433.048359918535,
            "unit": "ns/iter",
            "extra": "iterations: 25517\ncpu: 27432.562409374535 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 138381.20122555242,
            "unit": "ns/iter",
            "extra": "iterations: 5059\ncpu: 138379.1846214667 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1102602.5181104697,
            "unit": "ns/iter",
            "extra": "iterations: 635\ncpu: 1102538.8897637837 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 5923279.203540183,
            "unit": "ns/iter",
            "extra": "iterations: 113\ncpu: 5922936.300884987 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 466.39466196466617,
            "unit": "ns/iter",
            "extra": "iterations: 1501751\ncpu: 466.3588317903539 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1833.02232008437,
            "unit": "ns/iter",
            "extra": "iterations: 381943\ncpu: 1832.9432454580756 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7635.777473576658,
            "unit": "ns/iter",
            "extra": "iterations: 91679\ncpu: 7635.247352174466 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 30508.850143841708,
            "unit": "ns/iter",
            "extra": "iterations: 22942\ncpu: 30507.76052654527 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 123371.94959462254,
            "unit": "ns/iter",
            "extra": "iterations: 5674\ncpu: 123368.74215720689 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 494289.0261114389,
            "unit": "ns/iter",
            "extra": "iterations: 1417\ncpu: 494252.11150317616 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2515770.971223587,
            "unit": "ns/iter",
            "extra": "iterations: 278\ncpu: 2515630.3669064995 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 9934707.985509532,
            "unit": "ns/iter",
            "extra": "iterations: 69\ncpu: 9934317.710144939 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 931.4569345106075,
            "unit": "ns/iter",
            "extra": "iterations: 751495\ncpu: 931.4054371619263 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3792.6117579501715,
            "unit": "ns/iter",
            "extra": "iterations: 184573\ncpu: 3792.4887713803887 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 12783.356048292551,
            "unit": "ns/iter",
            "extra": "iterations: 54751\ncpu: 12782.88122591368 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 59300.09596001661,
            "unit": "ns/iter",
            "extra": "iterations: 11807\ncpu: 59297.80037266063 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 245433.0297932457,
            "unit": "ns/iter",
            "extra": "iterations: 2853\ncpu: 245421.49631966505 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 948035.3970190828,
            "unit": "ns/iter",
            "extra": "iterations: 738\ncpu: 947972.3983739818 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4659497.440000754,
            "unit": "ns/iter",
            "extra": "iterations: 150\ncpu: 4659226.90000004 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 19134476.75000018,
            "unit": "ns/iter",
            "extra": "iterations: 36\ncpu: 19132901.27777777 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1897.857355298787,
            "unit": "ns/iter",
            "extra": "iterations: 353711\ncpu: 1897.7618988383051 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 130.09395101801778,
            "unit": "ns/iter",
            "extra": "iterations: 5390841\ncpu: 130.08383664070436 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 22.377280794361358,
            "unit": "ns/iter",
            "extra": "iterations: 33050108\ncpu: 22.37627114561893 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 77.1587592998568,
            "unit": "ns/iter",
            "extra": "iterations: 9063721\ncpu: 77.15342065361347 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3796430245365191,
            "unit": "ms/iter",
            "extra": "iterations: 1834\ncpu: 0.3796197938931298 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.38440058901095064,
            "unit": "ms/iter",
            "extra": "iterations: 1820\ncpu: 0.3843787802197801 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 267.3243419999835,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 267.3103949999984 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 267.63078799999676,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 267.61039980000305 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 786.9109290839987,
            "unit": "ns/iter",
            "extra": "iterations: 889123\ncpu: 786.8472787229645 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 55373.63078383948,
            "unit": "ns/iter",
            "extra": "iterations: 12643\ncpu: 55369.34366843506 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 20.94565799217741,
            "unit": "ns/iter",
            "extra": "iterations: 33608530\ncpu: 20.9439233135159 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 77.41983642119291,
            "unit": "ns/iter",
            "extra": "iterations: 9035400\ncpu: 77.41807767226842 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 179.3982098951288,
            "unit": "ns/iter",
            "extra": "iterations: 3901224\ncpu: 179.39367337020528 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.170227378379283,
            "unit": "ms/iter",
            "extra": "iterations: 222\ncpu: 3.170000369369351 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 25.830761296295652,
            "unit": "ms/iter",
            "extra": "iterations: 27\ncpu: 25.82858992592576 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2621.2376984,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2620.7408737999967 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2597.776855999973,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2597.623231600005 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.8950354356847323,
            "unit": "ms/iter",
            "extra": "iterations: 241\ncpu: 2.8948797053941417 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 42.73670516073461,
            "unit": "ns/iter",
            "extra": "iterations: 16379664\ncpu: 42.73373806691022 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 131.70677448164594,
            "unit": "ns/iter",
            "extra": "iterations: 5313794\ncpu: 131.69909879833338 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4528535689319598,
            "unit": "ms/iter",
            "extra": "iterations: 1545\ncpu: 0.45284315210356174 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 222.0627216000139,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 222.04585840000277 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 222.739430799993,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 222.70170779999603 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.3777645117648254,
            "unit": "ms/iter",
            "extra": "iterations: 510\ncpu: 1.3776985254901988 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 131.67621804027326,
            "unit": "ns/iter",
            "extra": "iterations: 5310231\ncpu: 131.66952680589463 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 42.760007693121175,
            "unit": "ns/iter",
            "extra": "iterations: 16399059\ncpu: 42.75780110309915 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 625.6802321269242,
            "unit": "ns/iter",
            "extra": "iterations: 1118871\ncpu: 625.6479156220853 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1954.3858102000286,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1954.2883271999985 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 48.10418919999696,
            "unit": "ms/iter",
            "extra": "iterations: 15\ncpu: 48.10163900000172 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1998.9287282000078,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1998.8347178000026 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "distinct": true,
          "id": "ddc4e915fb8dd7b38a93badcb234c3a5b55ada2e",
          "message": "Update scripts.",
          "timestamp": "2021-05-21T13:09:43+01:00",
          "tree_id": "6d458b9cee34ff580f56a2e5a5531c005094a8ad",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/ddc4e915fb8dd7b38a93badcb234c3a5b55ada2e"
        },
        "date": 1621600076610,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 15.72763499998473,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 15.6947384 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 14.687822200016853,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 14.686518600000003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 377.37898160000896,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 377.3364054 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 783.4705398000096,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 783.4245946 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1492.1915548000015,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1492.0925975999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 4171.8994031999955,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 4171.240783 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 7.6500705122431825,
            "unit": "ns/iter",
            "extra": "iterations: 91385548\ncpu: 7.649455655723589 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 7.5291830930715875,
            "unit": "ns/iter",
            "extra": "iterations: 91679624\ncpu: 7.528816337641242 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 15.732307586857354,
            "unit": "ns/iter",
            "extra": "iterations: 46597869\ncpu: 15.731438233795666 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 14.999415639886882,
            "unit": "ns/iter",
            "extra": "iterations: 43998554\ncpu: 14.99820337277447 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 10.98235446438572,
            "unit": "ns/iter",
            "extra": "iterations: 63750856\ncpu: 10.980425423620929 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 10.98252288052119,
            "unit": "ns/iter",
            "extra": "iterations: 63747347\ncpu: 10.98176091939943 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 11.481808513032819,
            "unit": "ns/iter",
            "extra": "iterations: 60960932\ncpu: 11.480151566580421 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 11.458997259167143,
            "unit": "ns/iter",
            "extra": "iterations: 61094550\ncpu: 11.45812836005823 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 11.222905616195956,
            "unit": "ns/iter",
            "extra": "iterations: 62374790\ncpu: 11.22218498851862 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 11.209665512952881,
            "unit": "ns/iter",
            "extra": "iterations: 62468428\ncpu: 11.209019090411614 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 11.226200608784199,
            "unit": "ns/iter",
            "extra": "iterations: 62371795\ncpu: 11.225623713410942 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 11.482220171918692,
            "unit": "ns/iter",
            "extra": "iterations: 60968756\ncpu: 11.481120526716962 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 11.390846685726489,
            "unit": "ns/iter",
            "extra": "iterations: 61458766\ncpu: 11.389994651047855 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 11.741335796194637,
            "unit": "ns/iter",
            "extra": "iterations: 59563263\ncpu: 11.740115681707982 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 12.80078348125622,
            "unit": "ns/iter",
            "extra": "iterations: 54637427\ncpu: 12.799870627143566 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 14.777340860421996,
            "unit": "ns/iter",
            "extra": "iterations: 47350933\ncpu: 14.777117084472264 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 14.729715453128565,
            "unit": "ns/iter",
            "extra": "iterations: 47524367\ncpu: 14.729288282787586 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 15.104390387351355,
            "unit": "ns/iter",
            "extra": "iterations: 46247400\ncpu: 15.103984267223717 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 15.471729617995187,
            "unit": "ns/iter",
            "extra": "iterations: 42609877\ncpu: 15.470277137856996 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 21.931766814069178,
            "unit": "ns/iter",
            "extra": "iterations: 31929302\ncpu: 21.931403417462597 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 21.949871606288305,
            "unit": "ns/iter",
            "extra": "iterations: 31898369\ncpu: 21.94917874954675 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 21.933822133040078,
            "unit": "ns/iter",
            "extra": "iterations: 31910412\ncpu: 21.93265088523439 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 21.947909092739717,
            "unit": "ns/iter",
            "extra": "iterations: 31893954\ncpu: 21.94705077959309 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 21.934553802796124,
            "unit": "ns/iter",
            "extra": "iterations: 31891280\ncpu: 21.932674982001423 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 22.076783759691537,
            "unit": "ns/iter",
            "extra": "iterations: 31701860\ncpu: 22.07599292281265 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 22.06493413472663,
            "unit": "ns/iter",
            "extra": "iterations: 31720204\ncpu: 22.063919923087475 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 22.0792477941686,
            "unit": "ns/iter",
            "extra": "iterations: 31697122\ncpu: 22.078431694839633 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 22.061818666986216,
            "unit": "ns/iter",
            "extra": "iterations: 31726501\ncpu: 22.06041706269471 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 22.18068703365834,
            "unit": "ns/iter",
            "extra": "iterations: 31557959\ncpu: 22.1793667011227 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 23.24673579693472,
            "unit": "ns/iter",
            "extra": "iterations: 30127951\ncpu: 23.244961033028922 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 25.556357090048536,
            "unit": "ns/iter",
            "extra": "iterations: 27360852\ncpu: 25.554721322274723 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 26.872083866836913,
            "unit": "ns/iter",
            "extra": "iterations: 25912697\ncpu: 26.86856173249734 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 28.064229481981204,
            "unit": "ns/iter",
            "extra": "iterations: 24460901\ncpu: 28.062465687588794 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 43.71669143769468,
            "unit": "ns/iter",
            "extra": "iterations: 13137658\ncpu: 43.71403936683376 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 32.70761706468939,
            "unit": "ns/iter",
            "extra": "iterations: 21399792\ncpu: 32.70632452876124 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 32.71051020456949,
            "unit": "ns/iter",
            "extra": "iterations: 21384246\ncpu: 32.70873305516574 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 32.68195669717373,
            "unit": "ns/iter",
            "extra": "iterations: 21421649\ncpu: 32.680240629467605 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 32.714009508228585,
            "unit": "ns/iter",
            "extra": "iterations: 21399138\ncpu: 32.712170228539605 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 32.74817271368136,
            "unit": "ns/iter",
            "extra": "iterations: 21376234\ncpu: 32.74607430850516 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 32.71439419597063,
            "unit": "ns/iter",
            "extra": "iterations: 21397951\ncpu: 32.71383802121985 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 32.77361799093455,
            "unit": "ns/iter",
            "extra": "iterations: 21362179\ncpu: 32.773117058891735 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 32.77680721541044,
            "unit": "ns/iter",
            "extra": "iterations: 21365162\ncpu: 32.7758405482716 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 32.74549039585001,
            "unit": "ns/iter",
            "extra": "iterations: 21369614\ncpu: 32.744601423311146 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 32.824204974171096,
            "unit": "ns/iter",
            "extra": "iterations: 21323584\ncpu: 32.8230138517047 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 33.40903743878416,
            "unit": "ns/iter",
            "extra": "iterations: 20953857\ncpu: 33.40772989908256 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 35.25689287131923,
            "unit": "ns/iter",
            "extra": "iterations: 19870384\ncpu: 35.255251987078 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 36.56863851346521,
            "unit": "ns/iter",
            "extra": "iterations: 19215269\ncpu: 36.46134201920339 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 43.70466795169181,
            "unit": "ns/iter",
            "extra": "iterations: 13534823\ncpu: 43.70274749806467 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 61.07071990007682,
            "unit": "ns/iter",
            "extra": "iterations: 10414862\ncpu: 61.06426748621241 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 93.51350147556933,
            "unit": "ns/iter",
            "extra": "iterations: 7474109\ncpu: 93.50973032370823 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 350.4083711845643,
            "unit": "ns/iter",
            "extra": "iterations: 1997734\ncpu: 350.40131318784626 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1378.099227834183,
            "unit": "ns/iter",
            "extra": "iterations: 507922\ncpu: 1378.0826898618197 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 5489.26447039241,
            "unit": "ns/iter",
            "extra": "iterations: 127519\ncpu: 5489.090527686124 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 21944.095139606357,
            "unit": "ns/iter",
            "extra": "iterations: 31911\ncpu: 21942.992291059654 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 110435.93894935092,
            "unit": "ns/iter",
            "extra": "iterations: 6339\ncpu: 110432.42277961697 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 921661.805006701,
            "unit": "ns/iter",
            "extra": "iterations: 759\ncpu: 921595.0316205482 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 5370458.203125317,
            "unit": "ns/iter",
            "extra": "iterations: 128\ncpu: 5370182.593749994 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 372.9890868924648,
            "unit": "ns/iter",
            "extra": "iterations: 1876734\ncpu: 372.96655572926596 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1466.5536859745791,
            "unit": "ns/iter",
            "extra": "iterations: 477350\ncpu: 1466.4397548968163 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 6110.295636842738,
            "unit": "ns/iter",
            "extra": "iterations: 114573\ncpu: 6109.891667321312 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 24408.343572403322,
            "unit": "ns/iter",
            "extra": "iterations: 28681\ncpu: 24407.109863672817 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 98712.12819066302,
            "unit": "ns/iter",
            "extra": "iterations: 7091\ncpu: 98705.62967141416 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 395133.3133822794,
            "unit": "ns/iter",
            "extra": "iterations: 1771\ncpu: 395117.1214003405 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2081241.991045114,
            "unit": "ns/iter",
            "extra": "iterations: 335\ncpu: 2081106.0358208755 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 8349703.743902398,
            "unit": "ns/iter",
            "extra": "iterations: 82\ncpu: 8349303.926829377 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 745.2415471843119,
            "unit": "ns/iter",
            "extra": "iterations: 939539\ncpu: 745.2198578238894 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3033.2869548261924,
            "unit": "ns/iter",
            "extra": "iterations: 230775\ncpu: 3033.200827645994 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 10223.885980925728,
            "unit": "ns/iter",
            "extra": "iterations: 68471\ncpu: 10223.521227965211 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 47410.13310303042,
            "unit": "ns/iter",
            "extra": "iterations: 14763\ncpu: 47408.041929147126 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 196271.01121388574,
            "unit": "ns/iter",
            "extra": "iterations: 3567\ncpu: 196263.70955985502 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 758150.4799567116,
            "unit": "ns/iter",
            "extra": "iterations: 923\ncpu: 758120.9371614362 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 3813494.657608969,
            "unit": "ns/iter",
            "extra": "iterations: 184\ncpu: 3813245.7934781974 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 15739104.61363229,
            "unit": "ns/iter",
            "extra": "iterations: 44\ncpu: 15738761.27272717 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1551.784407542502,
            "unit": "ns/iter",
            "extra": "iterations: 452488\ncpu: 1551.6655226216178 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 103.64499569041675,
            "unit": "ns/iter",
            "extra": "iterations: 6730369\ncpu: 103.64016891198757 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 17.020575531629582,
            "unit": "ns/iter",
            "extra": "iterations: 41326660\ncpu: 17.01731717491807 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 68.96752461858108,
            "unit": "ns/iter",
            "extra": "iterations: 11367226\ncpu: 68.96179208542182 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.3040859748154342,
            "unit": "ms/iter",
            "extra": "iterations: 2303\ncpu: 0.3040679965262704 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.30810114511874426,
            "unit": "ms/iter",
            "extra": "iterations: 2274\ncpu: 0.3080700510114338 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 214.35538300002008,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 214.3417991999996 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 215.81361540002035,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 215.80588599999828 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 652.8919931467681,
            "unit": "ns/iter",
            "extra": "iterations: 1113059\ncpu: 652.8535944635527 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 44997.14367926819,
            "unit": "ns/iter",
            "extra": "iterations: 15465\ncpu: 44995.36857419972 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 16.64156245102644,
            "unit": "ns/iter",
            "extra": "iterations: 42035596\ncpu: 16.640927417800633 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 64.09403823731434,
            "unit": "ns/iter",
            "extra": "iterations: 11333326\ncpu: 64.09105773539027 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 143.29928046267287,
            "unit": "ns/iter",
            "extra": "iterations: 4883416\ncpu: 143.29439822452136 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 2.513185010829827,
            "unit": "ms/iter",
            "extra": "iterations: 277\ncpu: 2.5130568772562865 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 20.670203676465515,
            "unit": "ms/iter",
            "extra": "iterations: 34\ncpu: 20.66938091176449 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2065.998139000021,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2065.180723000003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2078.417962599997,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2078.179107800003 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.326095993355413,
            "unit": "ms/iter",
            "extra": "iterations: 301\ncpu: 2.3260084019932985 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 34.14953678407787,
            "unit": "ns/iter",
            "extra": "iterations: 20488825\ncpu: 34.14886832212101 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 105.34068435050851,
            "unit": "ns/iter",
            "extra": "iterations: 6638265\ncpu: 105.33434971336511 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.3620222857142933,
            "unit": "ms/iter",
            "extra": "iterations: 1939\ncpu: 0.36201027797833735 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 177.51842299999225,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 177.51225260000183 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 177.87212760003968,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 177.86586279999597 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.1067283811024127,
            "unit": "ms/iter",
            "extra": "iterations: 635\ncpu: 1.1066777700787545 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 105.39594868654639,
            "unit": "ns/iter",
            "extra": "iterations: 6637403\ncpu: 105.38996050111574 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 33.98404584841087,
            "unit": "ns/iter",
            "extra": "iterations: 20585927\ncpu: 33.98176793301525 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 500.4484551895451,
            "unit": "ns/iter",
            "extra": "iterations: 1398521\ncpu: 500.4222911204041 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1578.0125708000014,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1577.907136600004 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 38.967809722218284,
            "unit": "ms/iter",
            "extra": "iterations: 18\ncpu: 38.96447099999989 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1609.379349799974,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1609.270434399997 ms\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "tristan.youngs@stfc.ac.uk",
            "name": "Tristan Youngs",
            "username": "trisyoungs"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "671600e1bac80b132eaeb61f2a24dbf6fb83b8e0",
          "message": "Merge pull request #693 from disorderedmaterials/box-regions\n\nBox Regions",
          "timestamp": "2021-05-21T17:56:49+01:00",
          "tree_id": "ecbfec82797d6bb26beca743a6ba1305e0c9b21f",
          "url": "https://github.com/disorderedmaterials/dissolve/commit/671600e1bac80b132eaeb61f2a24dbf6fb83b8e0"
        },
        "date": 1621617623745,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::SimpleMethod>/iterations:5",
            "value": 17.64036499998838,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 17.6402644 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::small, Method::CellsMethod>/iterations:5",
            "value": 15.910992799990709,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 15.910987800000001 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::SimpleMethod>/iterations:5",
            "value": 428.40189479998116,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 428.34753199999994 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::medium, Method::CellsMethod>/iterations:5",
            "value": 788.0445998000141,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 787.9184393999999 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::SimpleMethod>/iterations:5",
            "value": 1660.9509361999699,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1660.0711296000004 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateGR<ProblemType::atomic, Population::large, Method::CellsMethod>/iterations:5",
            "value": 3898.5310430000027,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3898.2141965999995 ms\nthreads: 1"
          },
          {
            "name": "BM_MIM<CubicBox>",
            "value": 8.964572971623316,
            "unit": "ns/iter",
            "extra": "iterations: 76729608\ncpu: 8.96445829620298 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<OrthorhombicBox>",
            "value": 8.673016822011666,
            "unit": "ns/iter",
            "extra": "iterations: 80803955\ncpu: 8.672083637490251 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<MonoclinicBox>",
            "value": 18.246766262381307,
            "unit": "ns/iter",
            "extra": "iterations: 36640264\ncpu: 18.24526507232583 ns\nthreads: 1"
          },
          {
            "name": "BM_MIM<TriclinicBox>",
            "value": 17.7062611661676,
            "unit": "ns/iter",
            "extra": "iterations: 40014287\ncpu: 17.70495475778433 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1024",
            "value": 15.03155283988507,
            "unit": "ns/iter",
            "extra": "iterations: 51633514\ncpu: 15.030328828675028 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2048",
            "value": 13.489953790513423,
            "unit": "ns/iter",
            "extra": "iterations: 53141685\ncpu: 13.48838240639158 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4096",
            "value": 13.485935459002418,
            "unit": "ns/iter",
            "extra": "iterations: 51786155\ncpu: 13.485337055821182 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8192",
            "value": 13.347297443145168,
            "unit": "ns/iter",
            "extra": "iterations: 52387538\ncpu: 13.347194460636747 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16384",
            "value": 13.19596502080561,
            "unit": "ns/iter",
            "extra": "iterations: 51110152\ncpu: 13.19425463653486 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/32768",
            "value": 13.371438445988858,
            "unit": "ns/iter",
            "extra": "iterations: 52605618\ncpu: 13.370182002234053 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/65536",
            "value": 13.627116495774597,
            "unit": "ns/iter",
            "extra": "iterations: 53191637\ncpu: 13.625900045151786 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/131072",
            "value": 13.454279991236934,
            "unit": "ns/iter",
            "extra": "iterations: 51648295\ncpu: 13.452700229504087 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/262144",
            "value": 14.856285303832587,
            "unit": "ns/iter",
            "extra": "iterations: 48091804\ncpu: 14.855133070075848 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/524288",
            "value": 16.36381523024553,
            "unit": "ns/iter",
            "extra": "iterations: 42832833\ncpu: 16.362927896924266 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/1048576",
            "value": 17.238767582927974,
            "unit": "ns/iter",
            "extra": "iterations: 40530435\ncpu: 17.238083677118265 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/2097152",
            "value": 17.263203379330513,
            "unit": "ns/iter",
            "extra": "iterations: 40940409\ncpu: 17.262275396418215 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/4194304",
            "value": 17.433050746715114,
            "unit": "ns/iter",
            "extra": "iterations: 37281312\ncpu: 17.43271240025027 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/8388608",
            "value": 17.640803856029624,
            "unit": "ns/iter",
            "extra": "iterations: 36559432\ncpu: 17.639055852946534 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_1d/16777216",
            "value": 20.30994574228609,
            "unit": "ns/iter",
            "extra": "iterations: 36574153\ncpu: 20.307903535045654 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1024",
            "value": 25.930348632796754,
            "unit": "ns/iter",
            "extra": "iterations: 27050553\ncpu: 25.927860328770258 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2048",
            "value": 25.699706506496973,
            "unit": "ns/iter",
            "extra": "iterations: 27110311\ncpu: 25.69762228105752 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4096",
            "value": 26.371256985469987,
            "unit": "ns/iter",
            "extra": "iterations: 26610236\ncpu: 26.368969031315675 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8192",
            "value": 25.667170693401058,
            "unit": "ns/iter",
            "extra": "iterations: 26715514\ncpu: 25.664569845071973 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16384",
            "value": 26.523436237424793,
            "unit": "ns/iter",
            "extra": "iterations: 25010926\ncpu: 26.52086951918545 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/32768",
            "value": 25.883615335631887,
            "unit": "ns/iter",
            "extra": "iterations: 26869107\ncpu: 25.880898609693205 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/65536",
            "value": 26.857614829591782,
            "unit": "ns/iter",
            "extra": "iterations: 26432233\ncpu: 26.854273719515025 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/131072",
            "value": 25.856752451066324,
            "unit": "ns/iter",
            "extra": "iterations: 27368573\ncpu: 25.853946385878675 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/262144",
            "value": 27.661395892439504,
            "unit": "ns/iter",
            "extra": "iterations: 25610469\ncpu: 27.65856494076718 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/524288",
            "value": 29.591702148164362,
            "unit": "ns/iter",
            "extra": "iterations: 23242160\ncpu: 29.58838494356803 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/1048576",
            "value": 31.396389243100273,
            "unit": "ns/iter",
            "extra": "iterations: 22275385\ncpu: 31.3932424961456 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/2097152",
            "value": 32.25896242623539,
            "unit": "ns/iter",
            "extra": "iterations: 21797390\ncpu: 32.255627439799106 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/4194304",
            "value": 32.63799988514511,
            "unit": "ns/iter",
            "extra": "iterations: 21802808\ncpu: 32.63564711481258 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/8388608",
            "value": 33.06576584581241,
            "unit": "ns/iter",
            "extra": "iterations: 19660661\ncpu: 33.06349150722828 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_2d/16777216",
            "value": 48.908047911794164,
            "unit": "ns/iter",
            "extra": "iterations: 14311464\ncpu: 48.89928570550161 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1024",
            "value": 41.06459567220989,
            "unit": "ns/iter",
            "extra": "iterations: 18085128\ncpu: 40.86358023012012 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2048",
            "value": 38.870863978069984,
            "unit": "ns/iter",
            "extra": "iterations: 17947076\ncpu: 38.869748977493344 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4096",
            "value": 38.645866922436525,
            "unit": "ns/iter",
            "extra": "iterations: 18133364\ncpu: 38.644079885012125 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8192",
            "value": 38.13009193829155,
            "unit": "ns/iter",
            "extra": "iterations: 18563756\ncpu: 38.12564246157877 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16384",
            "value": 37.48265839236531,
            "unit": "ns/iter",
            "extra": "iterations: 18492259\ncpu: 37.47946202786792 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/32768",
            "value": 37.7421797582141,
            "unit": "ns/iter",
            "extra": "iterations: 18761824\ncpu: 37.73789440728143 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/65536",
            "value": 37.58673981374762,
            "unit": "ns/iter",
            "extra": "iterations: 18396725\ncpu: 37.58225004722346 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/131072",
            "value": 38.181358819350436,
            "unit": "ns/iter",
            "extra": "iterations: 17945623\ncpu: 38.17820434542749 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/262144",
            "value": 39.45152772472105,
            "unit": "ns/iter",
            "extra": "iterations: 18241768\ncpu: 39.45051214334023 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/524288",
            "value": 41.7670140417514,
            "unit": "ns/iter",
            "extra": "iterations: 16699912\ncpu: 41.7660569708392 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/1048576",
            "value": 43.278637319521195,
            "unit": "ns/iter",
            "extra": "iterations: 16338357\ncpu: 43.274212517207 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/2097152",
            "value": 42.95120594197393,
            "unit": "ns/iter",
            "extra": "iterations: 16237141\ncpu: 42.946230743454734 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/4194304",
            "value": 46.04465770836103,
            "unit": "ns/iter",
            "extra": "iterations: 15704590\ncpu: 46.03865162987435 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/8388608",
            "value": 53.43963259550513,
            "unit": "ns/iter",
            "extra": "iterations: 12285206\ncpu: 53.427984032176866 ns\nthreads: 1"
          },
          {
            "name": "BM_HistogramBinning_3d/16777216",
            "value": 64.89662877981927,
            "unit": "ns/iter",
            "extra": "iterations: 10498454\ncpu: 64.8895343066699 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1024",
            "value": 86.60453815556708,
            "unit": "ns/iter",
            "extra": "iterations: 7978087\ncpu: 86.59794609910894 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4096",
            "value": 293.7439106699606,
            "unit": "ns/iter",
            "extra": "iterations: 2379490\ncpu: 293.6965505213311 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16384",
            "value": 1120.651114392759,
            "unit": "ns/iter",
            "extra": "iterations: 635234\ncpu: 1120.3337085231644 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/65536",
            "value": 4728.627530426018,
            "unit": "ns/iter",
            "extra": "iterations: 132043\ncpu: 4728.375317131471 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/262144",
            "value": 28366.58673084369,
            "unit": "ns/iter",
            "extra": "iterations: 23890\ncpu: 28366.64483047296 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/1048576",
            "value": 269056.87908622046,
            "unit": "ns/iter",
            "extra": "iterations: 2539\ncpu: 269045.25167388475 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/4194304",
            "value": 1383766.7633138073,
            "unit": "ns/iter",
            "extra": "iterations: 507\ncpu: 1383720.3905325562 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_1d/16777216",
            "value": 6947116.666665671,
            "unit": "ns/iter",
            "extra": "iterations: 114\ncpu: 6946323.684210486 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1024",
            "value": 448.20315623075624,
            "unit": "ns/iter",
            "extra": "iterations: 1569467\ncpu: 448.1656301151942 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4096",
            "value": 1750.0690042476872,
            "unit": "ns/iter",
            "extra": "iterations: 400642\ncpu: 1749.8696242530725 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16384",
            "value": 7390.727958819022,
            "unit": "ns/iter",
            "extra": "iterations: 82947\ncpu: 7390.088912196944 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/65536",
            "value": 30035.573003006233,
            "unit": "ns/iter",
            "extra": "iterations: 22321\ncpu: 30030.887325836644 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/262144",
            "value": 121499.71733926654,
            "unit": "ns/iter",
            "extra": "iterations: 5848\ncpu: 121493.18690150311 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/1048576",
            "value": 565304.4423394633,
            "unit": "ns/iter",
            "extra": "iterations: 1214\ncpu: 565259.7001647461 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/4194304",
            "value": 2489262.9027775116,
            "unit": "ns/iter",
            "extra": "iterations: 288\ncpu: 2486906.874999997 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_2d/16777216",
            "value": 10099092.865674272,
            "unit": "ns/iter",
            "extra": "iterations: 67\ncpu: 10097151.01492535 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1024",
            "value": 919.698019463127,
            "unit": "ns/iter",
            "extra": "iterations: 774285\ncpu: 919.633442466278 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4096",
            "value": 3692.227766096121,
            "unit": "ns/iter",
            "extra": "iterations: 187855\ncpu: 3691.778573900008 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16384",
            "value": 12577.033533624666,
            "unit": "ns/iter",
            "extra": "iterations: 55586\ncpu: 12575.207660202233 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/65536",
            "value": 57605.28716602532,
            "unit": "ns/iter",
            "extra": "iterations: 12202\ncpu: 57602.6407146367 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/262144",
            "value": 244016.79823324562,
            "unit": "ns/iter",
            "extra": "iterations: 2830\ncpu: 244016.80883392057 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/1048576",
            "value": 1000690.3422912769,
            "unit": "ns/iter",
            "extra": "iterations: 707\ncpu: 1000617.748231975 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/4194304",
            "value": 4318265.125000665,
            "unit": "ns/iter",
            "extra": "iterations: 160\ncpu: 4317773.743749953 ns\nthreads: 1"
          },
          {
            "name": "BM_Array_3d/16777216",
            "value": 17945011.111111045,
            "unit": "ns/iter",
            "extra": "iterations: 36\ncpu: 17943317.083333533 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 1763.1254484591932,
            "unit": "ns/iter",
            "extra": "iterations: 391340\ncpu: 1762.9754152399485 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 117.7299555486083,
            "unit": "ns/iter",
            "extra": "iterations: 5960889\ncpu: 117.7145026522043 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 18.08502873148663,
            "unit": "ns/iter",
            "extra": "iterations: 38182329\ncpu: 18.0821952479641 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 75.45953716948186,
            "unit": "ns/iter",
            "extra": "iterations: 9320863\ncpu: 75.45048275036382 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.4864817617530265,
            "unit": "ms/iter",
            "extra": "iterations: 1255\ncpu: 0.48643587330676824 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::smallMolecule, Population::small>",
            "value": 0.38889944573637436,
            "unit": "ms/iter",
            "extra": "iterations: 1806\ncpu: 0.38885580509412887 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 260.03204580001693,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 260.0104655999985 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 259.22468060002757,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 259.20236039999907 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_AtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 777.3922140905444,
            "unit": "ns/iter",
            "extra": "iterations: 868903\ncpu: 777.3490596763891 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_SpeciesInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 49193.60723814774,
            "unit": "ns/iter",
            "extra": "iterations: 14617\ncpu: 49192.75234316269 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeBondEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 17.549673652683865,
            "unit": "ns/iter",
            "extra": "iterations: 40282391\ncpu: 17.542678139437758 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeAngleEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 78.48191382339239,
            "unit": "ns/iter",
            "extra": "iterations: 9191799\ncpu: 78.47585951346655 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeTorsionEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 176.04498568764058,
            "unit": "ns/iter",
            "extra": "iterations: 3885458\ncpu: 176.02483336585078 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_MoleculeEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 3.6940590765304795,
            "unit": "ms/iter",
            "extra": "iterations: 196\ncpu: 3.6934830510204493 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalIntraMolecularEnergy<ProblemType::mediumMolecule, Population::small>",
            "value": 27.523994555554896,
            "unit": "ms/iter",
            "extra": "iterations: 27\ncpu: 27.518639333333407 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterMolecularEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2620.3882257999794,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2620.1448298000005 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateEnergy_TotalInterAtomicEnergy<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2594.9034673999904,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2594.6642835999965 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::smallMolecule, Population::small>",
            "value": 2.8608754526752738,
            "unit": "ms/iter",
            "extra": "iterations: 243\ncpu: 2.8605092263375096 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::smallMolecule, Population::small>",
            "value": 35.70360460391639,
            "unit": "ns/iter",
            "extra": "iterations: 20030689\ncpu: 35.7000104689351 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::smallMolecule, Population::small>",
            "value": 124.96818334163672,
            "unit": "ns/iter",
            "extra": "iterations: 5594365\ncpu: 124.9427661584453 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::smallMolecule, Population::small>",
            "value": 0.4656555807327492,
            "unit": "ms/iter",
            "extra": "iterations: 1474\ncpu: 0.4655350793758595 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 215.59510739998586,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 215.58135059999586 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::smallMolecule, Population::small>/iterations:5",
            "value": 218.5270513999967,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 218.50231020000024 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_InterAtomic<ProblemType::mediumMolecule, Population::small>",
            "value": 1.3891149206963964,
            "unit": "ms/iter",
            "extra": "iterations: 517\ncpu: 1.3889621431334682 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesAngle<ProblemType::mediumMolecule, Population::small>",
            "value": 123.69291434696463,
            "unit": "ns/iter",
            "extra": "iterations: 5611734\ncpu: 123.68046079874783 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesBond<ProblemType::mediumMolecule, Population::small>",
            "value": 36.37160179912262,
            "unit": "ns/iter",
            "extra": "iterations: 18986407\ncpu: 36.36697696409763 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_SpeciesTorsion<ProblemType::mediumMolecule, Population::small>",
            "value": 596.3889994926351,
            "unit": "ns/iter",
            "extra": "iterations: 1186418\ncpu: 596.3615243531334 ns\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalInterAtomic<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 1965.1935043999856,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 1965.0025171999973 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalIntraMolecular<ProblemType::mediumMolecule, Population::small>",
            "value": 47.78893592856548,
            "unit": "ms/iter",
            "extra": "iterations: 14\ncpu: 47.78751621428715 ms\nthreads: 1"
          },
          {
            "name": "BM_CalculateForces_TotalForces<ProblemType::mediumMolecule, Population::small>/iterations:5",
            "value": 2020.8970254000178,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2020.6986454000003 ms\nthreads: 1"
          }
        ]
      }
    ]
  }
}