window.BENCHMARK_DATA = {
  "lastUpdate": 1619435807709,
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
      }
    ]
  }
}