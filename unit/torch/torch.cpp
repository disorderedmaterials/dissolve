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
    std::vector<char> types = {1, 1, 1, 2};
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
    auto dist_matrix = at::cdist(r, r);
    // std::cout << comb << std::endl;

    auto typeIdx = torch::from_blob(types.data(), {N}, opts.dtype(torch::kInt8)).repeat({N, 1});
    auto typeJdx = typeIdx.transpose(0, 1);
    std::cout << typeIdx << std::endl;
    std::cout << typeJdx << std::endl;

    EXPECT_EQ(dist_matrix.size(0), N);
    EXPECT_EQ(dist_matrix.size(0), N - 1);
    for (char j = 1; j < 3; j++)
	for (char i = j; i < 3; i++)
	{
	    auto tagged = dist_matrix.where((typeIdx == i) & (typeJdx == j), torch::zeros({N, N}, opts));
	    auto [counts, edges] = torch::masked_select(tagged, tagged != 0).histogram(3);
	    std::cout << tagged << std::endl;
	    std::cout << counts << std::endl;
	    std::cout << edges << std::endl;
	}
}
} // namespace UnitTest
