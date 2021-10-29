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

#ifndef EXAMPLES_ANALYTICAL_APPS_LIVEGRAPH_PAGERANK_LIVEGRAPH_CONTEXT_H_
#define EXAMPLES_ANALYTICAL_APPS_LIVEGRAPH_PAGERANK_LIVEGRAPH_CONTEXT_H_

#include <grape/grape.h>

#include <iomanip>

namespace grape {
/**
 * @brief Context for the auto-parallel version of PageRank.
 *
 * @tparam FRAG_T
 */
template <typename FRAG_T>
class PageRankLiveGraphContext : public VertexDataContext<FRAG_T, double> {
  using oid_t = typename FRAG_T::oid_t;
  using vid_t = typename FRAG_T::vid_t;

 public:
  explicit PageRankParallelContext(const FRAG_T& fragment)
      : VertexDataContext<FRAG_T, double>(fragment, true),
        result(this->data()) {}

  void Init(ParallelMessageManager& messages, double delta, int max_round) {
    auto& frag = this->fragment();
    auto vertices = frag.Vertices();

    this->delta = delta;
    this->max_round = max_round;
    degree.Init(vertices, 0);
    result.SetValue(0.0);
    next_result.Init(vertices);
    step = 0;
  }

  void Output(std::ostream& os) override {
    auto& frag = this->fragment();
    auto vertices = frag.Vertices();
    for (auto v : vertices) {
      os << frag.GetId(v) << " " << std::scientific << std::setprecision(15)
         << result[v] << std::endl;
    }
  }

  typename FRAG_T::template vertex_array_t<int> degree;
  typename FRAG_T::template vertex_array_t<double>& result;
  typename FRAG_T::template vertex_array_t<double> next_result;

  vid_t dangling_vnum = 0;
  int step = 0;
  int max_round = 0;
  double delta = 0;

  double dangling_sum = 0.0;
};
}  // namespace grape

#endif  // EXAMPLES_ANALYTICAL_APPS_LIVEGRAPH_PAGERANK_LIVEGRAPH_CONTEXT_H_