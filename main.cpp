#include "adaptive_walk_eval.h"
#include "autocorrelation_eval.h"
#include "density_of_states_eval.h"
#include "fdc_eval.h"
#include "fitness_cloud_eval.h"
#include "neutral_degree_eval.h"
#include "neutral_walk_eval.h"

int main(int argc, char **argv) {

    std::cout << "Adaptive walk" << std::endl << std::endl;

    calculate_adaptive_walk(argc, argv);

    std::cout << std::endl << std::endl;

    std::cout << "Autocorrelation" << std::endl << std::endl;

    calculate_autocorrelation(argc, argv);

    std::cout << std::endl << std::endl;

    std::cout << "Density of states" << std::endl << std::endl;

    calculate_density_of_states(argc, argv);

    std::cout << std::endl << std::endl;

    std::cout << "FDC" << std::endl << std::endl;

    calculate_fdc(argc, argv);

    std::cout << std::endl << std::endl;

    std::cout << "Fitness cloud" << std::endl << std::endl;

    calculate_fitness_cloud(argc, argv);

    std::cout << std::endl << std::endl;

    std::cout << "Neutral degree" << std::endl << std::endl;

    calculate_neutral_degree(argc, argv);

    std::cout << std::endl << std::endl;

    std::cout << "Neutral walk" << std::endl << std::endl;

    calculate_neutral_walk(argc, argv);

    return 0;
}
