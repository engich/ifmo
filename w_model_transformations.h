#ifndef W_MODEL_W_MODEL_TRANSFORMATIONS_H
#define W_MODEL_W_MODEL_TRANSFORMATIONS_H

//#include "IOHprofiler_common.h"
#define IOHprofiler_NORMAL_POLAR /* Use polar transformation method */

#define IOH_PI 3.14159265358979323846
#define DEFAULT_SEED 1000

#define RND_MULTIPLIER 16807
#define RND_MODULUS 2147483647
#define RND_MODULUS_DIV 127773
#define RND_MOD_MULTIPLIER 2836

#define IOHPROFILER_SHORT_LAG  273
#define IOHPROFILER_LONG_LAG 607
class IOHprofiler_random {
public:
    IOHprofiler_random(uint32_t seed = DEFAULT_SEED) {
        a = RND_MULTIPLIER; /// < multiplier.
        m = RND_MODULUS; /// < modulus.
        q = RND_MODULUS_DIV; /// < modulusdiv multiplier.
        r = RND_MOD_MULTIPLIER; /// < modulus mod multiplier.

        IOHprofiler_SHORT_LAG  = IOHPROFILER_SHORT_LAG;
        IOHprofiler_LONG_LAG = IOHPROFILER_LONG_LAG;
        for (int i = 0; i < IOHprofiler_LONG_LAG; ++i) {
            x[i] = ((double)seed) / (double)(((uint64_t)1UL << 32) - 1);
            seed = (uint32_t)1812433253UL * (seed ^ (seed >> 30)) + ((uint32_t)i + 1);
        }
        _seed_index = 0;
    }

    void IOHprofiler_random_generate() {
        for (int i = 0; i < IOHprofiler_SHORT_LAG; ++i) {
            double t = this->x[i] + this->x[i + (IOHprofiler_LONG_LAG - IOHprofiler_SHORT_LAG)];
            if (t >= 1.0)
                t -= 1.0;
            this->x[i] = t;
        }
        for (int i = IOHprofiler_SHORT_LAG; i < IOHprofiler_LONG_LAG; ++i) {
            double t = this->x[i] + this->x[i - IOHprofiler_SHORT_LAG];
            if (t >= 1.0)
                t -= 1.0;
            this->x[i] = t;
        }
        this->_seed_index = 0;
    }

    long _lcg_rand(const long &inseed) {

        long new_inseed =  (long)(a * (inseed - (long)floor((double)inseed / (double)q) * q) - r * (long)floor((double)inseed / (double)q));
        if (new_inseed < 0) {
            new_inseed = new_inseed + m;
        }
        return new_inseed;
    }

    void IOHprofiler_uniform_rand(const size_t &N, const long &inseed, std::vector<double> &rand_vec) {
        if (rand_vec.size() != 0) {
            std::vector<double>().swap(rand_vec);
        }
        rand_vec.reserve(N);


        long rand_seed[32];
        long seed;
        long rand_value;

        seed = inseed;
        if (seed < 0) {
            seed = -seed;
        }
        if (seed < 1) {
            seed = 1;
        }

        seed = inseed;
        for (int i = 39; i >= 0; --i) {
            seed = _lcg_rand(seed);
            if(i < 32) {
                rand_seed[i] = seed;
            }
        }

        int seed_index = 0;
        seed = rand_seed[0];
        for (int i = 0; i < N; ++i) {
            rand_value = _lcg_rand(seed);

            seed_index = (int)floor((double)seed / (double)67108865);
            seed = rand_seed[seed_index];
            rand_seed[seed_index] = rand_value;

            rand_vec.push_back((double)seed/2.147483647e9);
            if (rand_vec[i] == 0.) {
                rand_vec[i] = 1e-99;
            }
        }
    }

    std::vector<double> IOHprofiler_gauss(const size_t N, const long inseed) {
        std::vector<double> rand_vec;
        std::vector<double> uniform_rand_vec;
        rand_vec.reserve(N);

        long seed;
        long rand_value;

        seed = inseed;
        if (seed < 0) {
            seed = -seed;
        }
        if (seed < 1) {
            seed = 1;
        }

        IOHprofiler_uniform_rand(2 * N, seed,uniform_rand_vec);

        for (int i = 0; i < N; i++) {
            rand_vec.push_back(sqrt(-2 * log(uniform_rand_vec[i])) * cos(2 * IOH_PI * uniform_rand_vec[N + i]));
            if (rand_vec[i] == 0.) {
                rand_vec[i] = 1e-99;
            }
        }
        return rand_vec;
    }

    double IOHprofiler_uniform_rand() {
        if (this->_seed_index >= IOHprofiler_LONG_LAG) {
            IOHprofiler_random_generate();
        }
        return this->x[this->_seed_index++];
    }

    double IOHprofiler_normal_rand() { /* OMS: accessor - should become const */
        double normal;
#ifdef IOHprofiler_NORMAL_POLAR
        const double u1 = this->IOHprofiler_uniform_rand();
        const double u2 = this->IOHprofiler_uniform_rand();
        normal = sqrt(-2 * log(u1)) * cos(2 * IOH_PI * u2);
#else
        int i;
    normal = 0.0;
    for (i = 0; i < 12; ++i) {
        normal += this->IOHprofiler_uniform_rand();
    }
    normal -= 6.0;
#endif
        return normal;
    }

private:
    size_t _seed_index;

    long a; /// < multiplier.
    long m; /// < modulus.
    long q; /// < modulusdiv multiplier.
    long r; /// < modulus mod multiplier.

    int IOHprofiler_SHORT_LAG;
    int IOHprofiler_LONG_LAG;
    double x[607];
};

static IOHprofiler_random random_methods;

static std::vector<int> dummy(int number_of_variables, double select_rate, long inseed) {
    std::vector<int> position;
    std::vector<int> random_index;
    std::vector<double> random_numbers;
    int temp;
    int select_num = (int)floor((double)(number_of_variables * select_rate));

    position.reserve(number_of_variables);
    for (int i = 0; i != number_of_variables; ++i) {
        position.push_back(i);
    }

    random_methods.IOHprofiler_uniform_rand((size_t)select_num, inseed,random_numbers);
    for (int i = 0; i < select_num; ++i) {
        random_index.push_back((int)floor(random_numbers[i] * 1e4 / 1e4 * number_of_variables));
    }
    for (int i = 0; i != select_num; ++i) {
        temp = position[i];
        position[i] = position[random_index[i]];
        position[random_index[i]] = temp;
    }

    /// This is a stl algorithm.
    sort(position.begin(), position.begin() + select_num);

    random_index.clear();
    for(int i = 0; i != select_num; ++i){
        random_index.push_back(position[i]);
    }
    return random_index;
}

static std::vector<int> neutrality(const std::vector<int> &variables, int mu) {
    int number_of_variables = variables.size();
    int n = (int)floor((double)number_of_variables/(double)mu);
    std::vector<int> new_variables;

    new_variables.reserve(n);
    int i = 0, temp = 0;
    while (i != number_of_variables) {
        temp += variables[i];
        if ((i+1) % mu == 0 && i != 0) {
            if (temp >= mu / 2.0) {
                new_variables.push_back(1);
            } else {
                new_variables.push_back(0);
            }
            temp = 0;
        }
        i++;
    }

    return new_variables;
}

static std::vector<int> epistasis(const std::vector<int> &variables, int v) {
    int h, epistasis_result;
    int number_of_variables = variables.size();
    std::vector<int> new_variables;
    new_variables.reserve(number_of_variables);
    h = 0;
    while (h + v - 1 < number_of_variables) {
        int i = 0;
        while(i < v) {
            epistasis_result = -1;
            for (int j = 0; j < v; ++j) {
                if ((v - j - 1) != ((v - i - 1) - 1) % 4) {
                    if (epistasis_result == -1) {
                        epistasis_result = variables[h+j];
                    } else {
                        epistasis_result = (epistasis_result != variables[h+j]);
                    }
                }
            }
            new_variables.push_back(epistasis_result);
            ++i;
        }
        h += v;
    }
    if(number_of_variables - h > 0) {
        v = number_of_variables - h;
        int i = 0;
        while (i < v) {
            epistasis_result = -1;
            for (int j = 0; j < v; ++j) {
                if ((v - j - 1) != ((v - i - 1) - 1) % 4) {
                    if (epistasis_result == -1) {
                        epistasis_result = variables[h+j];
                    } else {
                        epistasis_result = (epistasis_result != variables[h+j]);
                    }
                }
            }
            new_variables.push_back(epistasis_result);
            ++i;
        }
        h += v;
    }
    return new_variables;
}

static double ruggedness1(double y, int number_of_variables) {
    double ruggedness_y, s;
    s = (double)number_of_variables;
    if(y == s) {
        ruggedness_y = ceil(y/2.0) + 1.0;
    } else if (y < s && number_of_variables % 2 == 0) {
        ruggedness_y = floor(y / 2.0) + 1.0;
    } else if (y < s && number_of_variables % 2 != 0) {
        ruggedness_y = ceil(y / 2.0) + 1.0;
    } else {
        ruggedness_y = y;
    }
    return ruggedness_y;
}

static double ruggedness2(double y, int number_of_variables) {
    double ruggedness_y;
    int tempy=(int)(y+0.5);
    if (tempy == number_of_variables) {
        ruggedness_y = y;
    } else if (tempy < number_of_variables && tempy % 2 == 0 && number_of_variables % 2 == 0) {
        ruggedness_y = y + 1.0;
    } else if (tempy < number_of_variables && tempy % 2 == 0 && number_of_variables % 2 != 0) {
        ruggedness_y = (y - 1.0) > 0 ? (y - 1.0) : 0;
    } else if (tempy < number_of_variables && tempy % 2 != 0 && number_of_variables % 2 == 0) {
        ruggedness_y = (y - 1.0) > 0 ? (y - 1.0) : 0;
    } else if (tempy < number_of_variables && tempy % 2 != 0 && number_of_variables % 2 != 0) {
        ruggedness_y = y + 1.0;
    } else {
        ruggedness_y = y;
    }
    return ruggedness_y;
}

static std::vector<double> ruggedness3(int number_of_variables) {
    std::vector<double> ruggedness_fitness(number_of_variables+1,0.0);

    for (int j = 1; j <= number_of_variables/5; ++j) {
        for (int k = 0; k < 5; ++k) {
            ruggedness_fitness[number_of_variables-5*j+k] = (double)(number_of_variables - 5 * j + (4-k));
        }
    }
    for (int k = 0; k < number_of_variables - number_of_variables / 5 * 5; ++k) {
        ruggedness_fitness[k] = (double)(number_of_variables - number_of_variables / 5 * 5 - 1 - k);
    }
    ruggedness_fitness[number_of_variables] = (double)number_of_variables;
    return ruggedness_fitness;
}

/// Following is the w-model soure code from Raphael's work, which refer the source code of Thomas Weise.

static void layer_neutrality_compute(const std::vector<int> &xIn, std::vector<int> &xOut, const int mu){
    int thresholdFor1 = (mu >> 1) + (mu & 1);
    int i,j,ones,flush;
    int temp;
    int dim = xIn.size();
    int temp_dim = dim / mu;
    if (xOut.size() != temp_dim) {
        xOut.resize(temp_dim);
    }
    i=0;j=0; ones=0; flush=mu;
    while ((i < dim) && (j<temp_dim)){
        if (xIn[i]==1) {
            ones+=1;
        }
        i+=1;
        if (i >= flush) {
            flush+=mu;
            if (ones >= thresholdFor1) {
                temp=1;
            } else {
                temp=0;
            }
            xOut[j]=temp;
            j+=1;
            ones=0;
        }
    }
}

static void base_epistasis(const std::vector<int> &xIn, const int start, const int nu, std::vector<int> &xOut) {
    const int end = (start + nu) - 1;
    const int flip = xIn[start];
    int result;
    int skip = start;
    for (int i = end ; i >= start; --i) {
        result = flip;
        for (int j = end; j > start; --j) {
            if (j != skip) {
                result ^= (xIn[j]);
            }
        }
        xOut[i] = result;
        if ((--skip) < start) {
            skip = end;
        }
    }
}

static void epistasis_compute(const std::vector<int>  &xIn, std::vector<int> &xOut, const int nu) {
    const int length = xIn.size();
    const int end = length - nu;
    int i;
    for (i = 0; i <= end; i += nu) {
        base_epistasis(xIn, i, nu, xOut);
    }
    if (i < length) {
        base_epistasis(xIn, i, length - i, xOut);
    }
}

static void layer_epistasis_compute(const std::vector<int> &x, std::vector<int> &epistasis_x, const int block_size) {
    epistasis_compute(x,epistasis_x,block_size);
}

static int max_gamma(int q) {
    return (int)((q * (q - 1))>>1);
}

static std::vector<int> ruggedness_raw(int gamma, int q){
    int i,j,k,start,max;
    int upper;
    int t;

    std::vector<int> r(q+1,0);
    r[0]=0;
    max = max_gamma(q);
    if (gamma <= 0){
        start = 0;
    } else {
        start = q - 1 - (int) (0.5 + sqrt(0.25 + ((max - gamma)<<1)));
    }

    k = 0;
    for (j = 1; j <= start; j++) {
        if ((j & 1) != 0){ r[j] = (q - k);}
        else{k=k+1; r[j] = k;}
    }

    for (; j <= q; j++) {
        k=k+1;
        if((start & 1) != 0){r[j] = (q - k);}
        else{r[j]=k;}
    }
    upper = ((gamma - max) +(((q - start - 1) * (q - start)) >> 1));
    j--;

    for (i = 1; i <= upper; i++) {
        j=j-1;

        if(j>0){
            t = r[j];
            r[j] = r[q];
            r[q] = t;}
    }

    std::vector<int> r2(q+1,0);
    for(i=0;i<=q;i++){r2[i]=q-r[q-i];}

    return r2;
}

static int ruggedness_translate(int gamma, int q) {
    int j, k, max, g, lastUpper;

    if (gamma <= 0) {
        return 0;
    }
    g = gamma;
    max = max_gamma(q);
    lastUpper = ((q >> 1) * ((q + 1) >> 1));
    if (g <= lastUpper) {
        j = abs(((q + 2) * 0.5)-sqrt((((q * q) * 0.25) + 1) - g));

        k = ((g - ((q + 2) * j)) + (j * j) + q);
        return ((k + 1 + ((((q + 2) * j) - (j * j) - q - 1) << 1))- (j - 1));
    }

    j = abs((((q % 2) + 1) * 0.5)
            + sqrt((((1 - (q % 2)) * 0.25) + g) - 1 - lastUpper));

    k = g - (((j - (q % 2)) * (j - 1)) + 1 + lastUpper);

    return (max - k - ((2 * j * j) - j) - ((q % 2) * ((-2 * j) + 1)));
}



static double layer_compute_ruggedness(const double y, size_t dimension,int gamma){
    double result;
    std::vector<int> r=ruggedness_raw(ruggedness_translate(gamma, dimension), dimension);
    result=r[ (int)y ];

    return result;
}

#endif //W_MODEL_W_MODEL_TRANSFORMATIONS_H
