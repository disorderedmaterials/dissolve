window.BENCHMARK_DATA = {
  "lastUpdate": 1619511921873,
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
      }
    ]
  }
}