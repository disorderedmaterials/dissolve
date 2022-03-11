#include <algorithm>
#include <gtest/gtest.h>
#include <torch/torch.h>
#include <vector>

namespace UnitTest
{

TEST(TorchTest, Torch)
{

    torch::set_num_threads(2);
    auto options = torch::TensorOptions().dtype(torch::kFloat64); //.device(torch::kCUDA, 1);

    torch::Tensor tensor = torch::linspace(0.0, 7.0, 400000000, options);
    auto x = tensor.sin();
    auto y = tensor.cos();
    auto result = x * x + y * y;
    // std::cout << tensor << std::endl;
    // std::cout << result << std::endl;

    // Handle individual output
    auto it = result.data_ptr<double>();
    EXPECT_TRUE(std::all_of(it, it + result.size(0), [](auto x) { return std::abs(x - 1.0) < 1e-8; }));
}

TEST(TorchTest, Pairs)
{
    std::vector<double> rs = {1, 0, 0, 1, 1, 0, 0, 1, 0, 3, 1, 0};
    int N = rs.size() / 3;
    // std::vector<double> idx(cfg->nAtoms());
    // auto it = rs.begin();
    // auto jt = idx.begin();
    // for (auto &at : cfg->atoms())
    // {
    //	*it++ = at.r().x;
    //	*it++ = at.r().y;
    //	*it++ = at.r().z;
    // *jt++ = at.localTypeIndex();
    // }

    auto opts = torch::TensorOptions().dtype(torch::kFloat64);
    auto r = torch::from_blob(rs.data(), {N, 3}, opts);
    // std::cout << r << std::endl;
    auto comb = at::cdist(r, r);
    // std::cout << comb << std::endl;
    EXPECT_EQ(comb.size(0), N);
    EXPECT_EQ(comb.size(0), N);
}
} // namespace UnitTest
