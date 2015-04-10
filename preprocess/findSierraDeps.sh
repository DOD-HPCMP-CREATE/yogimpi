sierraSource="/home/sadamec/devel/kestrel/src/avtools/sierra-4.34.1/sources/sierra-4.34.1/distro/4.34.1/"

for i in acme \
FETI-DP \
filters \
components/fsiMimdCoupling \
geometry_toolkit \
MATHLIB \
math_toolkit \
components/mesh \
MPIH \
Salinas \
Sierra/sddm_classic \
selem \
TPLs_src/Trilinos/packages/stk/stk_expreval \
TPLs_src/Trilinos/packages/stk/stk_io \
TPLs_src/Trilinos/packages/stk/stk_mesh \
TPLs_src/Trilinos/packages/stk/stk_topology \
TPLs_src/Trilinos/packages/stk/stk_util \
unitTestUtils \
utility \
TPLs_src/ARPACK \
TPLs_src/hdf5 \
TPLs_src/hp3d \
TPLs_src/matio \
TPLs_src/netcdf \
TPLs_src/operating_system \
TPLs_src/ParMetis \
TPLs_src/pnetcdf \
TPLs_src/slatec \
TPLs_src/sparseKit \
TPLs_src/SparsePack \
TPLs_src/superlu \
TPLs_src/UMFPACK \
TPLs_src/xdmf \
TPLs_src/y12m \
TPLs_src/Trilinos/packages/amesos \
TPLs_src/Trilinos/packages/amesos2 \
TPLs_src/Trilinos/packages/aztecoo \
TPLs_src/Trilinos/packages/belos \
TPLs_src/Trilinos/packages/claps \
TPLs_src/Trilinos/packages/common \
TPLs_src/Trilinos/packages/epetra \
TPLs_src/Trilinos/packages/epetraext \
TPLs_src/Trilinos/packages/fei \
TPLs_src/Trilinos/packages/ifpack \
TPLs_src/Trilinos/packages/ifpack2 \
TPLs_src/Trilinos/packages/kokkos \
TPLs_src/Trilinos/packages/ml \
TPLs_src/Trilinos/packages/muelu \
TPLs_src/Trilinos/packages/pamgen \
TPLs_src/Trilinos/packages/rtop \
TPLs_src/Trilinos/packages/seacas \
TPLs_src/Trilinos/packages/shards \
TPLs_src/Trilinos/packages/teuchos \
TPLs_src/Trilinos/packages/ThreadPool \
TPLs_src/Trilinos/packages/thyra \
TPLs_src/Trilinos/packages/tpetra \
TPLs_src/Trilinos/packages/triutils \
TPLs_src/Trilinos/packages/xpetra \
TPLs_src/Trilinos/packages/zoltan/src \
TPLs_src/Trilinos/packages/zoltan2/src; do
    python yogisearch.py -i $sierraSource/$i 
done