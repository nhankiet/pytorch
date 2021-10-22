#pragma once

#include <unordered_map>
#include <vector>

#include "lazy_tensors/span.h"
#include "torch/csrc/lazy/core/ir.h"

namespace torch_lazy_tensors {
namespace ir {

class Util {
 public:
  // Tracks the emission status of the nodes during the post-order generation.
  // It helps tracking loops within the computation graphs.
  enum EmitStatus {
    kNotEmitted,
    kEmitting,
    kEmitted,
  };

  using EmissionMap = std::unordered_map<const torch::lazy::Node*, EmitStatus>;

  // Computes the post order from the given node, without using recursion. The
  // emission map can be used as saved state, for multiple separate calls to
  // this API. The returned post-order can be empty if the node has already been
  // emitted inside the emission map. An error is generated if a loop is
  // detected.
  static std::vector<const torch::lazy::Node*> ComputePostOrder(
      const torch::lazy::Node* node, EmissionMap* emap);

  static std::vector<const torch::lazy::Node*> ComputePostOrder(
      lazy_tensors::Span<const torch::lazy::Node* const> nodes,
      EmissionMap* emap);

  // Same as above, but computes the post order on the set of nodes specified as
  // argument.
  static std::vector<const torch::lazy::Node*> ComputePostOrder(
      lazy_tensors::Span<const torch::lazy::Node* const> nodes);

  // Retrieves the number of nodes within the graph whose sink are passed in the
  // nodes argument.
  static size_t GetGraphSize(
      lazy_tensors::Span<const torch::lazy::Node* const> nodes);
};

}  // namespace ir
}  // namespace torch_lazy_tensors