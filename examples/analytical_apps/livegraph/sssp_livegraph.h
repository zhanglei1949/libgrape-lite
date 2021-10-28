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

#ifndef EXAMPLES_ANALYTICAL_APPS_SSSP_SSSP_LIVEGRAPH_H_
#define EXAMPLES_ANALYTICAL_APPS_SSSP_SSSP_LIVEGRAPH_H_

#include <grape/grape.h>

#include <charconv>
#include <string>

#include "livegraph.hpp"
#include "livegraph/sssp_livegraph_contex.h"

namespace grape {

/**
 * @brief SSSP application, determines the length of the shortest paths from a
 * given source vertex to all other vertices in graphs, which can work
 * on both directed and undirected graph.
 *
 * This version of SSSP inherits ParallelAppBase. Messages can be sent in
 * parallel with the evaluation process. This strategy improves the performance
 * by overlapping the communication time and the evaluation time.
 *
 * @tparam FRAG_T
 */
template <typename FRAG_T>
class SSSPLiveGraph
    : public ParallelAppBase<FRAG_T, SSSPLiveGraphContext<FRAG_T>>,
      public ParallelEngine {
 public:
  static constexpr MessageStrategy message_strategy =
      MessageStrategy::kAlongEdgeToOuterVertex;
  static constexpr LoadStrategy load_strategy = LoadStrategy::kBothOutIn;

  // specialize the templated worker.
  INSTALL_PARALLEL_WORKER(SSSPLiveGraph<FRAG_T>, SSSPLiveGraphContext<FRAG_T>,
                          FRAG_T)
  using vertex_t = typename fragment_t::vertex_t;

  /**
   * @brief Partial evaluation for SSSP.
   *
   * @param frag
   * @param ctx
   * @param messages
   */
  void PEval(const fragment_t& frag, context_t& ctx,
             message_manager_t& messages) {
    auto inner_vertices = frag.InnerVertices();
    messages.InitChannels(thread_num());

    vertex_t source;
    bool native_source = frag.GetInnerVertex(ctx.source_id, source);
    LOG(INFO) << "Source " << source.GetValue();

    ctx.next_modified.ParallelClear(GetThreadPool());
    LOG(INFO) << "before if";
    if (native_source) {
      ctx.partial_result[source] = 0;
      lg::EdgeIterator edgeIterator = frag.GetEdgeIterator(source);
      LOG(INFO) << "edge iterator: ,valid:" <<edgeIterator.valid();
      while (edgeIterator.valid()) {
        LOG(INFO) << edgeIterator.dst_id() << ":" << edgeIterator.edge_data();
        vertex_t v(edgeIterator.dst_id());
        std::string_view edge_data = edgeIterator.edge_data();
        int value;
        std::from_chars(edge_data.data(), edge_data.data() + edge_data.size(),
                        value);
        ctx.partial_result[v] = std::min(ctx.partial_result[v], value);
        ctx.next_modified.Insert(v);
        edgeIterator.next();
      }
    }
    messages.ForceContinue();
  }

  /**
   * @brief Incremental evaluation for SSSP.
   *
   * @param frag
   * @param ctx
   * @param messages
   */
  void IncEval(const fragment_t& frag, context_t& ctx,
               message_manager_t& messages) {
    LOG(INFO) << "No inc for SSSP livegraph.";
  }
};

}  // namespace grape

#endif  // EXAMPLES_ANALYTICAL_APPS_SSSP_SSSP_LIVEGRAPH_H_
