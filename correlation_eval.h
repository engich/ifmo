#ifndef W_MODEL_CORRELATION_EVAL_H
#define W_MODEL_CORRELATION_EVAL_H

#include <vector>
#include <algorithm>
#include <cmath>

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

#endif //W_MODEL_CORRELATION_EVAL_H
