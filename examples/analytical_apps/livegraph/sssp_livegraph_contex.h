/** Copyright 2020 Alibaba Group Holding Limited.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef EXAMPLES_ANALYTICAL_APPS_LIVEGRAPH_SSSP_LIVEGRAPH_CONTEXT_H_
#define EXAMPLES_ANALYTICAL_APPS_LIVEGRAPH_SSSP_LIVEGRAPH_CONTEXT_H_

#include <grape/grape.h>

#include <iomanip>
#include <iostream>
#include <limits>

namespace grape {

/**
 * @brief Context for the parallel version of SSSP.
 *
 * @tparam FRAG_T
 */
template <typename FRAG_T>
class SSSPLiveGraphContext : public VertexDataContext<FRAG_T, int> {
 public:
  using oid_t = typename FRAG_T::oid_t;
  using vid_t = typename FRAG_T::vid_t;

  explicit SSSPLiveGraphContext(const FRAG_T& fragment)
      : VertexDataContext<FRAG_T, int>(fragment, true),
        partial_result(this->data()) {}

  void Init(ParallelMessageManager& messages, oid_t source_id) {
    auto& frag = this->fragment();

    this->source_id = source_id;
    partial_result.SetValue(std::numeric_limits<int>::max());
    curr_modified.Init(frag.Vertices());
    next_modified.Init(frag.Vertices());
  }

  void Output(std::ostream& os) override {
    // If the distance is the max value for vertex_data_type
    // then the vertex is not connected to the source vertex.
    // According to specs, the output should be +inf
    auto& frag = this->fragment();
    auto inner_vertices = frag.InnerVertices();
    for (auto v : inner_vertices) {
      int d = partial_result[v];
      if (d == std::numeric_limits<int>::max()) {
        os << v.GetValue() << " infinity" << std::endl;
      } else {
        os << v.GetValue() << " " << std::scientific << std::setprecision(15)
           << d << std::endl;
      }
    }
  }

  oid_t source_id;
  typename FRAG_T::template vertex_array_t<int>& partial_result;

  DenseVertexSet<vid_t> curr_modified, next_modified;
};
}  // namespace grape

#endif  // EXAMPLES_ANALYTICAL_APPS_LIVEGRAPH_SSSP_LIVEGRAPH_CONTEXT_H_
