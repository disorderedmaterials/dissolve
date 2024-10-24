// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/peaks.h"
#include "math/data1D.h"
#include "math/interpolator.h"
#include "templates/algorithms.h"
#include <algorithm>
#include <cmath>
#include <gtest/gtest.h>
#include <random>

namespace UnitTest
{

class PeaksTest : public ::testing::Test
{
    public:
    PeaksTest()
    {
        /*
         * Peaks: test data
         */

        // Single, normalised Gaussian function with sigma = 0.22, mu = 3, and a peak at x = 2.97
        cleanGaussianY_ = {0.,    0.,    0.,    0.,    0., 0., 0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,
                           0.,    0.,    0.,    0.,    0., 0., 0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,
                           0.,    0.,    0.,    0.,    0., 0., 0.001, 0.003, 0.007, 0.015, 0.033, 0.065, 0.119, 0.203, 0.32,
                           0.468, 0.634, 0.796, 0.927, 1., 1., 0.927, 0.796, 0.634, 0.468, 0.32,  0.203, 0.119, 0.065, 0.033,
                           0.015, 0.007, 0.003, 0.001, 0., 0., 0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,
                           0.,    0.,    0.,    0.,    0., 0., 0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,
                           0.,    0.,    0.,    0.,    0., 0., 0.,    0.,    0.,    0.};

        // x
        gaussianX_ = {0.,    0.061, 0.121, 0.182, 0.242, 0.303, 0.364, 0.424, 0.485, 0.545, 0.606, 0.667, 0.727, 0.788, 0.848,
                      0.909, 0.97,  1.03,  1.091, 1.152, 1.212, 1.273, 1.333, 1.394, 1.455, 1.515, 1.576, 1.636, 1.697, 1.758,
                      1.818, 1.879, 1.939, 2.,    2.061, 2.121, 2.182, 2.242, 2.303, 2.364, 2.424, 2.485, 2.545, 2.606, 2.667,
                      2.727, 2.788, 2.848, 2.909, 2.97,  3.03,  3.091, 3.152, 3.212, 3.273, 3.333, 3.394, 3.455, 3.515, 3.576,
                      3.636, 3.697, 3.758, 3.818, 3.879, 3.939, 4.,    4.061, 4.121, 4.182, 4.242, 4.303, 4.364, 4.424, 4.485,
                      4.545, 4.606, 4.667, 4.727, 4.788, 4.848, 4.909, 4.97,  5.03,  5.091, 5.152, 5.212, 5.273, 5.333, 5.394,
                      5.455, 5.515, 5.576, 5.636, 5.697, 5.758, 5.818, 5.879, 5.939, 6.};

        /*
         *  Multiple Gaussians with local peaks at x=0.75, 1.5, and 2.25, and y=3.63, 5.44, and 3.63.
         *  The signal is a sum of Gaussians with sigma = 0.22 and mu is an equally spaced displacement of 3/4 on the last.
         */
        cleanMultiPeaksY_ = {
            1.81340652, 1.80493073, 1.77971361, 1.73846346, 1.68232345, 1.61282075, 1.53179997, 1.44134523, 1.3436958,
            1.24116055, 1.13603617, 1.03053369, 0.92671708, 0.82645663, 0.7313987,  0.64295222, 0.56229133, 0.49037237,
            0.42796277, 0.3756787,  0.33402779, 0.30345345, 0.28437674, 0.27723255, 0.28249674, 0.30070155, 0.33243734,
            0.37833928, 0.43905851, 0.51521826, 0.60735647, 0.71585718, 0.84087449, 0.98225331, 1.13945211, 1.31147359,
            1.49680892, 1.69340159, 1.89863587, 2.10935391, 2.32190421, 2.53222188, 2.73593947, 2.92852459, 3.10543833,
            3.2623071,  3.39509827, 3.50029006, 3.57502512, 3.6172387,  3.62575308, 3.60033223, 3.54169336, 3.4514744,
            3.3321599,  3.18697004, 3.01972021, 2.83465999, 2.63630164, 2.42924828, 2.21803178, 2.00696872, 1.80004156,
            1.60080988, 1.4123541,  1.23725221, 1.07758749, 0.93498371, 0.81066245, 0.70551656, 0.62019287, 0.55517713,
            0.51087456, 0.48767966, 0.48602992, 0.506439,   0.54950632, 0.61590118, 0.70632104, 0.82142541, 0.96174794,
            1.12759142, 1.31891149, 1.53519646, 1.77535158, 2.03759679, 2.31938701, 2.61736374, 2.92734548, 3.24436247,
            3.56273921, 3.87622484, 4.17816851, 4.46173321, 4.72013859, 4.94692059, 5.13619349, 5.28289941, 5.38302996,
            5.43380635, 5.43380635, 5.38302996, 5.28289941, 5.13619349, 4.94692059, 4.72013859, 4.46173321, 4.17816851,
            3.87622484, 3.56273921, 3.24436247, 2.92734548, 2.61736374, 2.31938701, 2.03759679, 1.77535158, 1.53519646,
            1.31891149, 1.12759142, 0.96174794, 0.82142541, 0.70632104, 0.61590118, 0.54950632, 0.506439,   0.48602992,
            0.48767966, 0.51087456, 0.55517713, 0.62019287, 0.70551656, 0.81066245, 0.93498371, 1.07758749, 1.23725221,
            1.4123541,  1.60080988, 1.80004156, 2.00696872, 2.21803178, 2.42924828, 2.63630164, 2.83465999, 3.01972021,
            3.18697004, 3.3321599,  3.4514744,  3.54169336, 3.60033223, 3.62575308, 3.6172387,  3.57502512, 3.50029006,
            3.39509827, 3.2623071,  3.10543833, 2.92852459, 2.73593947, 2.53222188, 2.32190421, 2.10935391, 1.89863587,
            1.69340159, 1.49680892, 1.31147359, 1.13945211, 0.98225331, 0.84087449, 0.71585718, 0.60735647, 0.51521826,
            0.43905851, 0.37833928, 0.33243734, 0.30070155, 0.28249674, 0.27723255, 0.28437674, 0.30345345, 0.33402779,
            0.3756787,  0.42796277, 0.49037237, 0.56229133, 0.64295222, 0.7313987,  0.82645663, 0.92671708, 1.03053369,
            1.13603617, 1.24116055, 1.3436958,  1.44134523, 1.53179997, 1.61282075, 1.68232345, 1.73846346, 1.77971361,
            1.80493073, 1.81340652};

        // x
        multiPeaksX_ = {
            0.,         0.01507538, 0.03015075, 0.04522613, 0.06030151, 0.07537688, 0.09045226, 0.10552764, 0.12060302,
            0.13567839, 0.15075377, 0.16582915, 0.18090452, 0.1959799,  0.21105528, 0.22613065, 0.24120603, 0.25628141,
            0.27135678, 0.28643216, 0.30150754, 0.31658291, 0.33165829, 0.34673367, 0.36180905, 0.37688442, 0.3919598,
            0.40703518, 0.42211055, 0.43718593, 0.45226131, 0.46733668, 0.48241206, 0.49748744, 0.51256281, 0.52763819,
            0.54271357, 0.55778894, 0.57286432, 0.5879397,  0.60301508, 0.61809045, 0.63316583, 0.64824121, 0.66331658,
            0.67839196, 0.69346734, 0.70854271, 0.72361809, 0.73869347, 0.75376884, 0.76884422, 0.7839196,  0.79899497,
            0.81407035, 0.82914573, 0.84422111, 0.85929648, 0.87437186, 0.88944724, 0.90452261, 0.91959799, 0.93467337,
            0.94974874, 0.96482412, 0.9798995,  0.99497487, 1.01005025, 1.02512563, 1.04020101, 1.05527638, 1.07035176,
            1.08542714, 1.10050251, 1.11557789, 1.13065327, 1.14572864, 1.16080402, 1.1758794,  1.19095477, 1.20603015,
            1.22110553, 1.2361809,  1.25125628, 1.26633166, 1.28140704, 1.29648241, 1.31155779, 1.32663317, 1.34170854,
            1.35678392, 1.3718593,  1.38693467, 1.40201005, 1.41708543, 1.4321608,  1.44723618, 1.46231156, 1.47738693,
            1.49246231, 1.50753769, 1.52261307, 1.53768844, 1.55276382, 1.5678392,  1.58291457, 1.59798995, 1.61306533,
            1.6281407,  1.64321608, 1.65829146, 1.67336683, 1.68844221, 1.70351759, 1.71859296, 1.73366834, 1.74874372,
            1.7638191,  1.77889447, 1.79396985, 1.80904523, 1.8241206,  1.83919598, 1.85427136, 1.86934673, 1.88442211,
            1.89949749, 1.91457286, 1.92964824, 1.94472362, 1.95979899, 1.97487437, 1.98994975, 2.00502513, 2.0201005,
            2.03517588, 2.05025126, 2.06532663, 2.08040201, 2.09547739, 2.11055276, 2.12562814, 2.14070352, 2.15577889,
            2.17085427, 2.18592965, 2.20100503, 2.2160804,  2.23115578, 2.24623116, 2.26130653, 2.27638191, 2.29145729,
            2.30653266, 2.32160804, 2.33668342, 2.35175879, 2.36683417, 2.38190955, 2.39698492, 2.4120603,  2.42713568,
            2.44221106, 2.45728643, 2.47236181, 2.48743719, 2.50251256, 2.51758794, 2.53266332, 2.54773869, 2.56281407,
            2.57788945, 2.59296482, 2.6080402,  2.62311558, 2.63819095, 2.65326633, 2.66834171, 2.68341709, 2.69849246,
            2.71356784, 2.72864322, 2.74371859, 2.75879397, 2.77386935, 2.78894472, 2.8040201,  2.81909548, 2.83417085,
            2.84924623, 2.86432161, 2.87939698, 2.89447236, 2.90954774, 2.92462312, 2.93969849, 2.95477387, 2.96984925,
            2.98492462, 3.};
    }

    protected:
    std::vector<double> cleanGaussianY_;
    std::vector<double> noisyGaussianY_;
    std::vector<double> gaussianX_;

    std::vector<double> cleanMultiPeaksY_;
    std::vector<double> noisyMultiPeaksY_;
    std::vector<double> multiPeaksX_;

    std::vector<double> addNoise(std::vector<double> values, double amplitude = 1.0)
    {
        const double mean = 0.0;
        const double stddev = 0.1;
        std::mt19937 generator(std::random_device{}());
        std::normal_distribution<double> dist(mean, stddev);

        std::vector<double> randVec;
        for (int i = 0; i < values.size(); i++)
            randVec.push_back(dist(generator));

        // Normalise random noise vector
        for (int i = 0; i < values.size(); i++)
            randVec[i] *= 1 / (*std::max_element(randVec.begin(), randVec.end()));

        for (int i = 0; i < values.size(); i++)
            values[i] += amplitude * randVec[i];

        return values;
    }
};

TEST_F(PeaksTest, BasicPeaks)
{
    Peaks analyser(cleanGaussianY_, gaussianX_);

    auto peaks = analyser.find();

    EXPECT_TRUE(peaks.size() == 1);
    ASSERT_NEAR(peaks[0].valueAt, 2.97, 10e-5);

    // Set threshold just high enough to skip peak
    analyser.setThreshold(1.01);
    EXPECT_TRUE(analyser.find().size() == 0);

    // Find multiple peaks
    Peaks analyserMulti(cleanMultiPeaksY_, multiPeaksX_);

    auto multiPeaks = analyserMulti.find();

    EXPECT_TRUE(multiPeaks.size() == 3);
    ASSERT_NEAR(multiPeaks[0].valueAt, 0.75, 10e-3);
    ASSERT_NEAR(multiPeaks[1].valueAt, 1.5, 10e-3);
    ASSERT_NEAR(multiPeaks[2].valueAt, 2.25, 10e-3);

    // Skip two lowest peaks
    analyserMulti.setThreshold(3.64);
    EXPECT_TRUE(analyserMulti.find().size() == 1);
}

TEST_F(PeaksTest, DynamicPeaks)
{
    // Multiply peaks with a cosine over the domain [0, 3]
    auto signal = cleanMultiPeaksY_, domain = multiPeaksX_;
    for (int i = 0; i < 200; i++)
        signal[i] *= cos(domain[i]);

    Peaks analyser(signal, domain);

    auto peaks = analyser.find();

    // Find all 4 peaks
    EXPECT_TRUE(peaks.size() == 4);
    ASSERT_NEAR(peaks[0].valueAt, 0.72, 10e-2);
    ASSERT_NEAR(peaks[1].valueAt, 1.38, 10e-2);
    ASSERT_NEAR(peaks[2].valueAt, 1.87, 10e-2);
    ASSERT_NEAR(peaks[3].valueAt, 2.65, 10e-2);

    // Reject negative value peaks
    analyser.setThreshold(0);
    EXPECT_TRUE(analyser.find().size() == 2);

    // Isolate largest peak
    analyser.setIsolation(*std::max_element(multiPeaksX_.begin(), multiPeaksX_.end()));
    auto majorPeak = analyser.find();
    ASSERT_NEAR(peaks[0].peak, 2.68, 10e-3);

    // Generate a new signal: xcos(30x)
    std::vector<double> y, x;
    for (int i = 0; i < 1000; i++)
    {
        auto xI = i * (3.0 / 1000);
        x.push_back(xI);
        y.push_back(xI * cos(30 * xI));
    }

    Peaks signalAnalyser(y, x);

    // Isolate peaks by a delta of 0.6
    signalAnalyser.setThreshold(0);
    signalAnalyser.setIsolation(0.6);
    auto dynamicPeaks = signalAnalyser.find();

    EXPECT_TRUE(dynamicPeaks.size() == 5);
    ASSERT_NEAR(dynamicPeaks[4].peak, 2.93, 10e-3);
    ASSERT_NEAR(dynamicPeaks[0].peak, 0.42, 10e-3);
}

TEST_F(PeaksTest, NoisyPeaks)
{
    auto x = multiPeaksX_;

    // Lambda to check whether expected peaks are found in noisy data
    auto containsPeakNearIndex = [](const auto &peaks, const int index, const int tolerance)
    {
        return std::any_of(peaks.begin(), peaks.end(),
                           [&](const auto &peak) { return std::abs(peak.index - index) <= tolerance; });
    };

    // Added noise of amplitude 0.5
    auto y = PeaksTest::addNoise(cleanMultiPeaksY_, 0.5);

    Peaks analyserNoise(y, x);
    analyserNoise.setIsolation(3.0 / 8);

    auto peaksNoise = analyserNoise.find();
    analyserNoise.sortPeaks(peaksNoise);

    // Largest peak should be close to 5.44
    EXPECT_NEAR(peaksNoise[0].peak, 5.44, 10e-1);
    ASSERT_TRUE(containsPeakNearIndex(peaksNoise, 50, 5));
    ASSERT_TRUE(containsPeakNearIndex(peaksNoise, 100, 5));
    ASSERT_TRUE(containsPeakNearIndex(peaksNoise, 150, 5));

    // Added noise of amplitude 1.0
    auto y1 = PeaksTest::addNoise(cleanMultiPeaksY_);

    Peaks analyserNoiseLevel1(y1, x);
    analyserNoiseLevel1.setIsolation(3.0 / 8);

    auto peaksNoiseLevel1 = analyserNoiseLevel1.find();
    analyserNoiseLevel1.sortPeaks(peaksNoiseLevel1);

    // Largest peak should be close to 5.44
    EXPECT_NEAR(peaksNoiseLevel1[0].peak, 5.44, 10e-1);
    ASSERT_TRUE(containsPeakNearIndex(peaksNoiseLevel1, 50, 5));
    ASSERT_TRUE(containsPeakNearIndex(peaksNoiseLevel1, 100, 5));
    ASSERT_TRUE(containsPeakNearIndex(peaksNoiseLevel1, 150, 5));

    // Added noise of amplitude 2.0
    auto y2 = PeaksTest::addNoise(cleanMultiPeaksY_, 2.0);

    Peaks analyserNoiseLevel2(y2, x);
    analyserNoiseLevel2.setIsolation(3.0 / 8);

    auto peaksNoiseLevel2 = analyserNoiseLevel2.find();
    analyserNoiseLevel2.sortPeaks(peaksNoiseLevel2);

    // Largest peak should be close to 5.44
    EXPECT_NEAR(peaksNoiseLevel2[0].peak, 5.44, 10e-0);
    ASSERT_TRUE(containsPeakNearIndex(peaksNoiseLevel2, 50, 10));
    ASSERT_TRUE(containsPeakNearIndex(peaksNoiseLevel2, 100, 10));
    ASSERT_TRUE(containsPeakNearIndex(peaksNoiseLevel2, 150, 10));
}

TEST_F(PeaksTest, Prominences)
{
    // Multiply peaks with a cosine over the domain [0, 3]
    auto y = cleanMultiPeaksY_, x = multiPeaksX_;
    for (int i = 0; i < 200; i++)
        y[i] *= cos(x[i]);

    Peaks analyser(y, x);

    auto proms = analyser.prominences();

    // Find prominences for all 4 peaks
    EXPECT_TRUE(proms.size() == 4);
    ASSERT_NEAR(proms[0].prominence, 2.42, 10e-2);
    ASSERT_NEAR(proms[1].prominence, 0.55, 10e-2);
    ASSERT_NEAR(proms[2].prominence, 0.16, 10e-2);
    ASSERT_NEAR(proms[3].prominence, 1.55, 10e-2);
    ASSERT_NEAR(proms[0].valueAt, 0.72, 10e-2);
    ASSERT_NEAR(proms[1].valueAt, 1.38, 10e-2);
    ASSERT_NEAR(proms[2].valueAt, 1.87, 10e-2);
    ASSERT_NEAR(proms[3].valueAt, 2.65, 10e-2);
}

} // namespace UnitTest
