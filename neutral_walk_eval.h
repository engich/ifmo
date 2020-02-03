#ifndef W_MODEL_NEUTRAL_WALK_EVAL_H
#define W_MODEL_NEUTRAL_WALK_EVAL_H


#include <stdexcept>  // runtime_error
#include <iostream>   // cout
#include <sstream>    // ostrstream, istrstream
#include <fstream>
#include <cstring>

// declaration of the namespace
using namespace std;

//-----------------------------------------------------------------------------
// representation of solutions, and neighbors
#include "paradiseo/eo/src/ga/eoBit.h"                         // bit string
#include "paradiseo/mo/src/problems/bitString/moBitNeighbor.h" // neighbor of bit string

//-----------------------------------------------------------------------------
// fitness function, and evaluation of neighbors
#include "w_model_eval.h"
#include "paradiseo/mo/src/problems/eval/moOneMaxIncrEval.h"

//-----------------------------------------------------------------------------
// neighborhood description
#include "paradiseo/mo/src/neighborhood/moRndWithoutReplNeighborhood.h" // visit one random neighbor possibly the same one several times

//-----------------------------------------------------------------------------
// the sampling class
#include "paradiseo/eo/src/utils/eoDistance.h"
#include "paradiseo/mo/src/sampling/moNeutralWalkSampling.h"

//-----------------------------------------------------------------------------
// the statistics class
#include "paradiseo/mo/src/sampling/moStatistics.h"

// Declaration of types
//-----------------------------------------------------------------------------
// Indi is the typedef of the solution type
typedef eoBit<unsigned int> Indi;                      // bit string with unsigned fitness type
// Neighbor is the typedef of the neighbor type,
// Neighbor = How to compute the neighbor from the solution + information on it (i.e. fitness)
// all classes from paradisEO-mo use this template type
typedef moBitNeighbor<unsigned int> Neighbor ;         // bit string neighbor with unsigned fitness type


void calculate_neutral_walk(int argc, char **argv)
{
    /* =========================================================
     *
     * Parameters
     *
     * ========================================================= */

    eoParser parser(argc, argv);

    // For each parameter, define Parameter, read it through the parser,
    // and assign the value to the variable

    // random seed parameter
    eoValueParam<uint32_t> seedParam(time(nullptr), "seed", "Random number seed", 'S');
    parser.processParam( seedParam );
    unsigned seed = seedParam.value();

    // length of the bit string
    eoValueParam<unsigned int> vecSizeParam(12, "vecSize", "Genotype size", 'V');
    parser.processParam( vecSizeParam, "Representation" );
    unsigned vecSize = vecSizeParam.value();

    // the number of steps of the random walk
    eoValueParam<unsigned int> stepParam(100, "nbStep", "Number of steps of the random walk", 'n');
    parser.processParam( stepParam, "Representation" );
    unsigned nbStep = stepParam.value();

    // Dummy parameter
    eoValueParam<double> dummParam(0, "dumm", "Dummy parameter of w-model transformation", 'D');
    parser.processParam( dummParam, "Representation" );
    double dumm = dummParam.value();

    // Neutrality parameter
    eoValueParam<int> neutParam(0, "neut", "Neutrality parameter of w-model transformation", 'N');
    parser.processParam( neutParam, "Representation" );
    int neut = neutParam.value();

    // Epistasis parameter
    eoValueParam<int> episParam(0, "epis", "Epistasis parameter of w-model transformation", 'E');
    parser.processParam( episParam, "Representation" );
    int epis = episParam.value();

    // Ruggedness parameter
    eoValueParam<int> ruggParam(0, "rugg", "Ruggedness parameter of w-model transformation", 'R');
    parser.processParam( ruggParam, "Representation" );
    int rugg = ruggParam.value();

    // the name of the output file
    string str_out = "out.dat"; // default value
    eoValueParam<string> outParam(str_out, "out", "Output file of the sampling", 'o');
    parser.processParam(outParam, "Persistence" );

    // the name of the "status" file where all actual parameter values will be saved
    string str_status = parser.ProgramName() + ".status"; // default value
    eoValueParam<string> statusParam(str_status, "status", "Status file");
    parser.processParam( statusParam, "Persistence" );

    if (parser.userNeedsHelp()) {
        parser.printHelp(cout);
        exit(1);
    }
    if (!statusParam.value().empty()) {
        ofstream os(statusParam.value().c_str());
        os << parser;
    }

    /* =========================================================
     *
     * Random seed
     *
     * ========================================================= */

    // reproducible random seed: if you don't change SEED above,
    // you'll aways get the same result, NOT a random run
    rng.reseed(seed);

    /* =========================================================
     *
     * Initialization of the solution
     *
     * ========================================================= */

    // a Indi random initializer: each bit is random
    eoUniformGenerator<bool> uGen;
    eoInitFixedLength<Indi> random(vecSize, uGen);

    /* =========================================================
     *
     * Eval fitness function (full evaluation)
     *
     * ========================================================= */

    WModelOneMaxEval<Indi> fullEval{dumm, neut, epis, rugg};

    /* =========================================================
     *
     * evaluation of a neighbor solution
     *
     * ========================================================= */

    moFullEvalByCopy<Neighbor> neighborEval(fullEval);

    /* =========================================================
     *
     * the neighborhood of a solution
     *
     * ========================================================= */

    // Exploration of the neighborhood in random order
    // at each step one bit is randomly generated
    moRndWithoutReplNeighborhood<Neighbor> neighborhood(vecSize);

    /* =========================================================
     *
     * The sampling of the search space
     *
     * ========================================================= */

    // Hamming distance
    eoHammingDistance<Indi> distance;

    // sampling object :
    //    - random initialization
    //    - neighborhood to compute the next step
    //    - fitness function
    //    - neighbor evaluation
    //    - number of steps of the walk
    moNeutralWalkSampling<Neighbor> sampling(random, neighborhood, fullEval, neighborEval, nbStep);

    /* =========================================================
     *
     * execute the sampling
     *
     * ========================================================= */

    // the sampling
    sampling();

    /* =========================================================
     *
     * export the sampling
     *
     * ========================================================= */

    // to export the statistics into file
    sampling.fileExport(str_out);

    // to get the values of statistics
    // so, you can compute some statistics in c++ from the data
    const std::vector<Indi> & solutions = sampling.getSolutions(0);

    std::cout << "First values:" << std::endl;
    std::cout << "Solution  " << solutions[0] << std::endl;

    std::cout << "Last values:" << std::endl;
    std::cout << "Solution  " << solutions[solutions.size() - 1] << std::endl;

    std::cout << "All solutions" << std::endl;

    for (const auto& val : solutions) { std::cout << val << std::endl; } std::cout << std::endl;

    // export only the solution into file
    sampling.fileExport(0, str_out + "_sol");

    // more basic statistics on the distribution:
    moStatistics statistics;

    vector< vector<double> > dist;
    vector<double> v;

    statistics.distances(solutions, distance, dist);

    for (unsigned i = 0; i < dist.size(); i++) {
        for (unsigned j = 0; j < dist.size(); j++) {
            std::cout << dist[i][j] << " " ;
            if (j < i)
                v.push_back(dist[i][j]);
        }
        std::cout << std::endl;
    }

    double min, max, avg, std;
    statistics.basic(v, min, max, avg, std);
    std::cout << "min=" << min << ", max=" << max << ", average=" << avg << ", std dev=" << std << std::endl;
}


#endif //W_MODEL_NEUTRAL_WALK_EVAL_H
