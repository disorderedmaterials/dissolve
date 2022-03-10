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
} // namespace UnitTest
