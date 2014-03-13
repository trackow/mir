/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/grid/Field.h"
#include "eckit/grid/Grid.h"
#include "eckit/grid/LatLon.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

#include "mir/WeightCache.h"
#include <Eigen/Sparse>


using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

class TestWeightCache : public Tool {
public:

    TestWeightCache(int argc,char **argv): Tool(argc,argv) {}

    ~TestWeightCache() {}

    virtual void run();

    void test_constructor();
    void test_values();
};


void TestWeightCache::test_values()
{
    using namespace eckit::grid;

    mir::WeightCache wc;
    
    int n = 100, m = 200;
    // create a sparse matrix
    Eigen::SparseMatrix<double> M(n, m);

    // generate some sparse data
    std::vector<Eigen::Triplet<double> > insertions;

    int npts = 100;

    insertions.reserve(npts);

    for (unsigned int i = 0; i < npts; i++)
    {
        long x = i;
        long y = i+1;
        double w = (double)(x+y);;
        
        insertions.push_back(Eigen::Triplet<double>(x, y, w));
    }

    M.setFromTriplets(insertions.begin(), insertions.end());

    wc.add("test_matrix", M); 


    // now get the data back again
    Eigen::SparseMatrix<double> W(n, m);
    wc.get("test_matrix", W);

    // now get the triplets from W and check them against M
    std::vector<Eigen::Triplet<double> > triplets;
    for (unsigned int i = 0; i < W.outerSize(); ++i) 
    {
        for (typename Eigen::SparseMatrix<double>::InnerIterator it(W,i); it; ++it) 
        {
            triplets.push_back(Eigen::Triplet<double>(it.row(), it.col(), it.value()));
        }
    }    

    // check construction of the matrix
    assert(triplets.size() == insertions.size());
    assert(W.size() == M.size());
    assert(W.outerSize() == M.outerSize());
    assert(W.innerSize() == M.innerSize());
    
    // check the values
    for (unsigned int i = 0; i < triplets.size(); i++)
    {
        assert(triplets[i].col() == insertions[i].col());
        assert(triplets[i].row() == insertions[i].row());
        assert(triplets[i].value() == insertions[i].value());
    }
    

}

//-----------------------------------------------------------------------------

void TestWeightCache::run()
{
     test_values();
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestWeightCache mytest(argc,argv);
    mytest.start();
    return 0;
}

