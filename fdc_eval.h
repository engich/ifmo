#ifndef W_MODEL_FDC_EVAL_H
#define W_MODEL_FDC_EVAL_H


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

//-----------------------------------------------------------------------------
// the distance defined over the search space
#include "paradiseo/eo/src/utils/eoDistance.h"

//-----------------------------------------------------------------------------
// the sampling class
#include "paradiseo/mo/src/sampling/moFDCsampling.h"

// Declaration of types
//-----------------------------------------------------------------------------
// Indi is the typedef of the solution type
typedef eoBit<unsigned int> Indi;                      // bit string with unsigned fitness type
// Neighbor is the typedef of the neighbor type,
// Neighbor = How to compute the neighbor from the solution + information on it (i.e. fitness)
// all classes from paradisEO-mo use this template type
typedef moBitNeighbor<unsigned int> Neighbor ;         // bit string neighbor with unsigned fitness type


double pearson_correlation(const vector<double>& x, const vector<double>& y)  {
    unsigned objects_size = min(x.size(), y.size());
    double x_mean = 0.0, y_mean = 0.0;

    for (unsigned i = 0; i < objects_size; ++i) {
        x_mean += x[i] / objects_size;
        y_mean += y[i] / objects_size;
    }

    double numerator = 0, squares_sum_x = 0, squares_sum_y = 0;
    for (unsigned i = 0; i < objects_size; ++i) {
        numerator += (x[i] - x_mean) * (y[i] - y_mean);
        squares_sum_x += pow(x[i] - x_mean, 2);
        squares_sum_y += pow(y[i] - y_mean, 2);
    }

    double denominator = sqrt(squares_sum_x * squares_sum_y);

    return (denominator > 0 ? numerator / denominator : 0.0);
}

void calculate_fdc(int argc, char **argv)
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

    // the number of solution sampled
    eoValueParam<unsigned int> solParam(100, "nbSol", "Number of random solution", 'n');
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
     * The sampling of the search space
     *
     * ========================================================= */

    eoHammingDistance<Indi> distance; // Hamming distance
    Indi bestSolution(vecSize, true); // global optimum

    // sampling object :
    //    - random initialization
    //    - fitness function
    //    - number of solutions to sample
    moFDCsampling<Neighbor> sampling(random, fullEval, distance, bestSolution, nbSol);

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
    const std::vector<double> & fitnessValues = sampling.getValues(0);
    const std::vector<double> & distValues    = sampling.getValues(1);

    /*std::cout << "First values:" << std::endl;
    std::cout << "Fitness  " << fitnessValues[0] << std::endl;
    std::cout << "Distance " << distValues[0] << std::endl;

    std::cout << "Last values:" << std::endl;
    std::cout << "Fitness  " << fitnessValues[fitnessValues.size() - 1] << std::endl;
    std::cout << "Distance " << distValues[distValues.size() - 1] << std::endl;

    std::cout << "All Fitness" << std::endl; for (auto val : fitnessValues) { std::cout << val << std::endl;} std::cout << std::endl; std::cout << "Fitness length: " << fitnessValues.size();
    std::cout << "All Distance" << std::endl; for (auto val : distValues) { std::cout << val << std::endl;} std::cout << std::endl; std::cout << "Distance length: " << distValues.size();*/

    std::cout << "Fitness-distance correlation: " << pearson_correlation(fitnessValues, distValues);
}


#endif //W_MODEL_FDC_EVAL_H
