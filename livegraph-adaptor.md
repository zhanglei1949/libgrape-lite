# Livegraph + libgrape-lite
Running PIE algorithm on data store in livegraph.

# branches
[zhanglei/libgrape-lite](https://github.com/zhanglei1949/libgrape-lite/tree/livegraph-api)

[zhanglei/livegraph](https://github.com/zhanglei1949/LiveGraph)

# How to run
## 1. install livegraph

## 2. install libgrape-lite

## 3. run app
```bash
cd libgrape-lite/build
# run sssp
mpirun  -n 1 ./run_app --vfile ../dataset/p2p-31.v --efile ../dataset/p2p-31.e --application livegraph_sssp --sssp_source 0 --out_prefix ./output_sssp --directed --app_concurrency 1

# run pagerank
mpirun  -n 1 ./run_app --vfile ../dataset/p2p-31.v --efile ../dataset/p2p-31.e --application livegraph_pagerank --pr_d 0.85 --pr_mr 10 --out_prefix ./output_pagerank  --app_concurrency 1
```
For the options, see [flags.h](https://github.com/zhanglei1949/libgrape-lite/blob/master/examples/analytical_apps/flags.h)
# Core files

##  libgrapte-lite
- **grape/fragment/livegraph_wrapper.h**
    The wrapper of livegraph Transaction

- **examples/analytical_apps/livegraph/*.h**
    Impl for apps

- **grape/fragment/loader.h**
    Where we load data to livegraph

## livegraph
- **bind/livegraph.hpp**
    The interface exposed, lg::Transaction

- **core/transaction.hpp**
    Underlying impl. livegraph::Transaction

- **src/transaction.cpp**
    Underlying impl.

