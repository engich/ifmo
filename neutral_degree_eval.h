#ifndef W_MODEL_NEUTRAL_DEGREE_EVAL_H
#define W_MODEL_NEUTRAL_DEGREE_EVAL_H


#include <stdexcept>  // runtime_error
#include <iostream>   // cout
#include <sstream>    // ostrstream, istrstream
#include <fstream>
#include <cstring>

// declaration of the namespace
using namespace std;

//-----------------------------------------------------------------------------
// representation of solutions, and neighbors
#include "paradiseo/eo/src/ga/eoBit.h"                         // bit string : see also EO tutorial lesson 1: FirstBitGA.cpp
#include "paradiseo/mo/src/problems/bitString/moBitNeighbor.h" // neighbor of bit string

//-----------------------------------------------------------------------------
// fitness function, and evaluation of neighbors
#include "w_model_eval.h"
#include "paradiseo/mo/src/problems/eval/moOneMaxIncrEval.h"

//-----------------------------------------------------------------------------
// neighborhood description
#include "paradiseo/mo/src/neighborhood/moOrderNeighborhood.h" // visit all neighbor in order of their bit-flip

//-----------------------------------------------------------------------------
// the sampling class
#include "paradiseo/mo/src/sampling/moNeutralDegreeSampling.h"

// Declaration of types
//-----------------------------------------------------------------------------
// Indi is the typedef of the solution type like in paradisEO-eo
typedef eoBit<unsigned int> Indi;                      // bit string with unsigned fitness type
// Neighbor is the typedef of the neighbor type,
// Neighbor = How to compute the neighbor from the solution + information on it (i.e. fitness)
// all classes from paradisEO-mo use this template type
typedef moBitNeighbor<unsigned int> Neighbor ;         // bit string neighbor with unsigned fitness type


#include "paradiseo/problems/eval/royalRoadEval.h"
#include "paradiseo/mo/src/problems/eval/moRoyalRoadIncrEval.h"
#include "paradiseo/mo/src/eval/moFullEvalByCopy.h"

void calculate_neutral_degree(int argc, char **argv)
{
    /* =========================================================
     *
     * Parameters
     *
     * ========================================================= */
    // more information on the input parameters: see EO tutorial lesson 3
    // but don't care at first it just read the parameters of the bit string size and the random seed.

    // First define a parser from the command-line arguments
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

    // the number of solution sampled
    eoValueParam<unsigned int> solParam(100, "nbSol", "Number of random solution", 'n');
    parser.processParam( solParam, "Representation" );
    unsigned nbSol = solParam.value();

    // the name of the output file
    string str_out = "out.dat"; // default value
    eoValueParam<string> outParam(str_out, "out", "Output file of the sampling", 'o');
    parser.processParam(outParam, "Persistence" );

    // the name of the "status" file where all actual parameter values will be saved
    string str_status = parser.ProgramName() + ".status"; // default value
    eoValueParam<string> statusParam(str_status, "status", "Status file");
    parser.processParam( statusParam, "Persistence" );

    // do the following AFTER ALL PARAMETERS HAVE BEEN PROCESSED
    // i.e. in case you need parameters somewhere else, postpone these
    if (parser.userNeedsHelp()) {
        parser.printHelp(cout);
        exit(1);
    }
    if (!statusParam.value().empty()) {
        ofstream os(statusParam.value().c_str());
        os << parser;// and you can use that file as parameter file
    }

    /* =========================================================
     *
     * Random seed
     *
     * ========================================================= */

    // reproducible random seed: if you don't change SEED above,
    // you'll aways get the same result, NOT a random run
    // more information: see EO tutorial lesson 1 (FirstBitGA.cpp)
    rng.reseed(seed);

    /* =========================================================
     *
     * Initialization of the solution
     *
     * ========================================================= */

    // a Indi random initializer: each bit is random
    // more information: see EO tutorial lesson 1 (FirstBitGA.cpp)
    eoUniformGenerator<bool> uGen;
    eoInitFixedLength<Indi> random(vecSize, uGen);

    /* =========================================================
     *
     * Eval fitness function (full evaluation)
     *
     * ========================================================= */

    // the fitness function is the royal function (oneMax is a Royal Road with block of 1)
    //WModelOneMaxEval<Indi> fullEval{0.5, 1, 1, 1};
    RoyalRoadEval<Indi> fullEval(4);

    /* =========================================================
     *
     * evaluation of a neighbor solution
     *
     * ========================================================= */

    // Incremental evaluation of the neighbor: fitness is modified by +1 , 0 or -1
    //moRoyalRoadIncrEval<Neighbor> neighborEval(fullEval);
    moRoyalRoadIncrEval<Neighbor> neighborEval(fullEval);

    /* =========================================================
     *
     * the neighborhood of a solution
     *
     * ========================================================= */

    // Exploration of the neighborhood in increasing order of the neigbor's index:
    // bit-flip from bit 0 to bit (vecSize - 1)
    moOrderNeighborhood<Neighbor> neighborhood(vecSize);

    /* =========================================================
     *
     * The sampling of the search space
     *
     * ========================================================= */

    // sampling object :
    //    - random initialization
    //    - neighborhood to compute the neutral degree
    //    - fitness function
    //    - neighbor evaluation
    //    - number of solutions to sample
    moNeutralDegreeSampling<Neighbor> sampling(random, neighborhood, fullEval, neighborEval, nbSol);

    /* =========================================================
     *
     * execute the sampling
     *
     * ========================================================= */

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
    const std::vector<double> & fitnessValues = sampling.getValues(0);
    const std::vector<double> & ndValues = sampling.getValues(1);

    std::cout << "First values:" << std::endl;
    std::cout << "Fitness   " << fitnessValues[0] << std::endl;
    std::cout << "N. Degree " << ndValues[0] << std::endl;

    std::cout << "Last values:" << std::endl;
    std::cout << "Fitness  " << fitnessValues[fitnessValues.size() - 1] << std::endl;
    std::cout << "N. Degree " << ndValues[fitnessValues.size() - 1] << std::endl;

    std::cout << "All values" << std::endl; for (auto val : ndValues) { std::cout << val << std::endl;} std::cout << std::endl;

    /*const std::vector<Indi> & solutions = sampling.getSolutions(0);

    std::cout << "First values:" << std::endl;
    std::cout << "Solution  " << solutions[0] << std::endl;

    std::cout << "Last values:" << std::endl;
    std::cout << "Solution  " << solutions[solutions.size() - 1] << std::endl;

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
    std::cout << "min=" << min << ", max=" << max << ", average=" << avg << ", std dev=" << std << std::endl;*/
}


#endif //W_MODEL_NEUTRAL_DEGREE_EVAL_H
