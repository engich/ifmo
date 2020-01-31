#ifndef W_MODEL_W_MODEL_EVAL_H
#define W_MODEL_W_MODEL_EVAL_H

#include "paradiseo/eo/src/eoEvalFunc.h"

#include "w_model_transformations.h"

/**
 * Full evaluation Function for W-Model OneMax problem
 */
template< class EOT >
class WModelOneMaxEval : public eoEvalFunc<EOT>
{
public:

    WModelOneMaxEval(double dummy_para, int neutrality_para, int epistasis_para, int ruggedness_para) :
        dummy_para(dummy_para),
        neutrality_para(neutrality_para),
        epistasis_para(epistasis_para),
        ruggedness_para(ruggedness_para) {
        prepare_problem();
    }

    /**
     * Count the number of 1 in a bitString after w-model transformation
     * @param _sol the solution to evaluate
     */
    void operator() (EOT& _sol) {
        std::vector<int> x;

        for (auto val : _sol) {
            x.push_back(val);
        }

        std::vector<int> w_model_x;
        std::vector<int> tempX;
        int n;

        /// dummy step
        if (this->dummy_para > 0){
            n = this->dummy_info.size();
            w_model_x.reserve(n);
            for (int i = 0; i != n; ++i) {
                w_model_x.push_back(x[this->dummy_info[i]]);
            }
        } else {
            w_model_x = x;
        }

        /// neutrality
        if (this->neutrality_para > 0) {
            tempX = w_model_x;
            layer_neutrality_compute(tempX,w_model_x,this->neutrality_para);
        }

        ///
        if (this->epistasis_para > 0) {
            tempX = w_model_x;
            layer_epistasis_compute(tempX,w_model_x,this->epistasis_para);
        }

        ///
        n = w_model_x.size();
        int result = 0;

        for (int i = 0; i != n; ++i) {
            result += w_model_x[i];
        }

        if (this->ruggedness_para > 0) {
            result = this->ruggedness_info[result];
        }

        _sol.fitness(result);
    }

    void prepare_problem() {
        this->temp_dimension = 12;

        if (this->dummy_para > 0) {
            this->dummy_info = dummy(this->temp_dimension, dummy_para, 10000);
            this->temp_dimension = this->dummy_info.size();
        }

        if (this->neutrality_para > 0) {
            this->temp_dimension = this->temp_dimension / this->neutrality_para;
        }
        if (this->ruggedness_para > 0) {
            this->ruggedness_info = ruggedness_raw(ruggedness_translate(this->ruggedness_para, this->temp_dimension), this->temp_dimension);
        }
    }

private:
    std::vector<int> dummy_info;
    double dummy_para = 0;
    int epistasis_para = 0;
    int neutrality_para = 0;
    int ruggedness_para = 0;
    std::vector<int> ruggedness_info;
    int temp_dimension;
};


#endif //W_MODEL_W_MODEL_EVAL_H
