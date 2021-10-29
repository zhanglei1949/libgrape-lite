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

#ifndef GRAPE_FRAGMENT_LIVEGRAPH_WRAPPER_H_
#define GRAPE_FRAGMENT_LIVEGRAPH_WRAPPER_H_

#include <assert.h>
#include <stddef.h>

#include <algorithm>
#include <iosfwd>
#include <limits>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "flat_hash_map/flat_hash_map.hpp"
#include "grape/config.h"
#include "grape/fragment/edgecut_fragment_base.h"
#include "grape/graph/adj_list.h"
#include "grape/graph/edge.h"
#include "grape/graph/vertex.h"
#include "grape/io/io_adaptor_base.h"
#include "grape/serialization/in_archive.h"
#include "grape/serialization/out_archive.h"
#include "grape/types.h"
#include "grape/util.h"
#include "grape/utils/vertex_array.h"
#include "grape/vertex_map/global_vertex_map.h"
#include "grape/worker/comm_spec.h"
#include "livegraph.hpp"

namespace grape {
class CommSpec;
class OutArchive;

template <typename OID_T, typename VID_T, typename VDATA_T, typename EDATA_T,
          LoadStrategy _load_strategy = LoadStrategy::kBothOutIn>
class LiveGraphWrapper {
 public:
  using internal_vertex_t = internal::Vertex<VID_T, VDATA_T>;
  using edge_t = Edge<VID_T, EDATA_T>;
  // using nbr_t = Nbr<VID_T, EDATA_T>;
  using vertex_t = Vertex<VID_T>;
  // using const_adj_list_t = ConstAdjList<VID_T, EDATA_T>;
  // using adj_list_t = LiveGraphAdjList<VID_T>;
  using vid_t = VID_T;
  using oid_t = OID_T;
  using vdata_t = VDATA_T;
  using edata_t = EDATA_T;
  // using vertex_range_t = VertexRange<vid_t>;
  template <typename DATA_T>
  using vertex_array_t = VertexArray<DATA_T, vid_t>;
  // using vertex_map_t = GlobalVertexMap<oid_t, vid_t>;

  // using IsEdgeCut = std::true_type;
  // using IsVertexCut = std::false_type;
  static constexpr LoadStrategy load_strategy = _load_strategy;
  LiveGraphWrapper() = default;

  explicit LiveGraphWrapper(std::unique_ptr<lg::Graph> graph)
      : graph_(std::move(graph)) {
    transaction_ = graph_->begin_transaction();
  }

  virtual ~LiveGraphWrapper() = default;

  void Init(fid_t fid, std::vector<internal_vertex_t>& vertices,
            std::vector<edge_t>& edges) {
    LOG(INFO) << "Init livegraph wrapper";
  }

  void PrepareToRunApp(MessageStrategy strategy, bool need_split_edges) {
    // if (strategy == MessageStrategy::kAlongEdgeToOuterVertex ||
    //     strategy == MessageStrategy::kAlongIncomingEdgeToOuterVertex ||
    //     strategy == MessageStrategy::kAlongOutgoingEdgeToOuterVertex) {
    //   initMessageDestination(strategy);
    // }

    // if (need_split_edges) {
    //   initEdgesSplitter(ieoffset_, iespliters_);
    //   initEdgesSplitter(oeoffset_, oespliters_);
    // }
  }

  inline size_t GetEdgeNum() { return 0; }

  inline VID_T GetMaxVertexID() { return transaction_.get_max_vid(); }

  size_t GetVerticesNum() { return transaction_.get_vertex_num(); }

  inline VertexRange<VID_T> Vertices() const {
    return VertexRange<VID_T>(0, transaction_.get_max_vid());
  }

  // inline bool GetInnerVertex(const OID_T& oid, vertex_t& v) const {
  //   v.SetValue((VID_T) 0);
  //   return true;
  // }

  inline int GetDegree(const vertex_t& v) const { return GetVerticesNum(); }

 public:
  // /**
  //  * @brief Returns the incoming adjacent vertices of v.
  //  *
  //  * @param v Input vertex.
  //  *
  //  * @return The incoming adjacent vertices of v.
  //  *
  //  * @attention Only inner vertex is available.
  //  */
  // inline adj_list_t GetIncomingAdjList(const vertex_t& v) {
  //   return adj_list_t(transaction_.get_edges(v.GetValue(), 0));
  // }

  // /**
  //  * @brief Returns the outgoing adjacent vertices of v.
  //  *
  //  * @param v Input vertex.
  //  *
  //  * @return The outgoing adjacent vertices of v.
  //  *
  //  * @attention Only inner vertex is available.
  //  */
  // inline adj_list_t GetOutgoingAdjList(const vertex_t& v) {
  //   // return adj_list_t(oeoffset_[v.GetValue()], oeoffset_[v.GetValue() +
  //   1]); return adj_list_t(transaction_.get_edges(v.GetValue(), 0));
  // }
  lg::EdgeIterator GetEdgeIterator(const vertex_t& v) const {
    return transaction_.get_edges(v.GetValue(), 0, false);
  }

 private:
  std::unique_ptr<lg::Graph> graph_;
  lg::Transaction transaction_;
};
}  // namespace grape

#endif  // GRAPE_FRAGMENT_LIVEGRAPH_WRAPPER_H_
