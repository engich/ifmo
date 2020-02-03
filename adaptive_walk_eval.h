#ifndef W_MODEL_ADAPTIVE_WALK_EVAL_H
#define W_MODEL_ADAPTIVE_WALK_EVAL_H


#include <stdexcept>  // runtime_error
#include <iostream>   // cout
#include <sstream>    // ostrstream, istrstream
#include <fstream>
#include <cstring>
#include <numeric>

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
#include "paradiseo/mo/src/eval/moFullEvalByModif.h"
#include "paradiseo/mo/src/eval/moFullEvalByCopy.h"

//-----------------------------------------------------------------------------
// neighborhood description
#include "paradiseo/mo/src/neighborhood/moOrderNeighborhood.h"
#include "paradiseo/mo/src/neighborhood/moRndWithoutReplNeighborhood.h"
#include "paradiseo/mo/src/neighborhood/moRndWithReplNeighborhood.h"

//-----------------------------------------------------------------------------
// the sampling class
#include "paradiseo/mo/src/sampling/moHillClimberSampling.h"

// Declaration of types
//-----------------------------------------------------------------------------
// Indi is the typedef of the solution type
typedef eoBit<unsigned int> Indi;                      // bit string with unsigned fitness type
// Neighbor is the typedef of the neighbor type,
// Neighbor = How to compute the neighbor from the solution + information on it (i.e. fitness)
// all classes from paradisEO-mo use this template type
typedef moBitNeighbor<unsigned int> Neighbor ;         // bit string neighbor with unsigned fitness type


void calculate_adaptive_walk(int argc, char **argv)
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

    // the number of adaptive walks
    eoValueParam<unsigned int> solParam(100, "nbSol", "Number of adaptive walks", 'n');
    parser.processParam( solParam, "Representation" );
    unsigned nbSol = solParam.value();

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
    str_out = outParam.value();

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

    moRndWithoutReplNeighborhood<Neighbor> neighborhood(vecSize);

    /* =========================================================
     *
     * The sampling of the search space
     *
     * ========================================================= */

    // sampling object :
    //    - random initialization
    //    - local search to sample the search space
    //    - one statistic to compute
    moHillClimberSampling<Neighbor> sampling(random, neighborhood, fullEval, neighborEval, nbSol);

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
    const std::vector<double> & lengthValues = sampling.getValues(0);

    /*std::cout << "First adaptive walk length:" << std::endl;
    std::cout << "Length: " << lengthValues[0] << std::endl;

    std::cout << "Last adaptive walk length:" << std::endl;
    std::cout << "Length: " << lengthValues[lengthValues.size() - 1] << std::endl;

    std::cout << "All values" << std::endl; for (auto val : lengthValues) { std::cout << val << std::endl;} std::cout << std::endl;*/

    double mean_adaptive_walk_length = accumulate(lengthValues.begin(), lengthValues.end(), 0.0) / lengthValues.size();

    std::cout << "Mean adaptive walk length:: " << mean_adaptive_walk_length << std::endl;
}


#endif //W_MODEL_ADAPTIVE_WALK_EVAL_H
