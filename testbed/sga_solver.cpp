// #pragma GCC optimize ("Ofast") 
#include "./test.h"
#include "./settings.h"
#include "chromosome.h"
#include "tests/environments/car_environment.hpp"

#include <iostream>
#include <chrono>
#include <random>
#include <functional>
#include <future>
#include <thread>

using f_T = float;

template <int CHR_LEN, int mu, int lambda>
class SGA_solver
{
public:
    
    SGA_solver(CarEnvironment *car_environment, int seed, bool is_plus)
        : is_plus{is_plus}
    {
        std::clog << "Running SGA solver instance\t is plus: " << is_plus << "\n";
        static_assert(mu <= lambda);
        static_assert(mu % 2 == 0 and lambda % 2 == 0);
        gen = std::mt19937(seed);
        
        initialize_population();
        evaluate_population(car_environment, 0, mu);
        for(int i = 0; i < mu ; ++i) std::cout << i << "\t" << population_values.at(i) << "\n";
        std::cout << "\tbest value " << *std::min_element(std::begin(population_values), std::begin(population_values) + mu) << "\n";
    }

    void run(int iterations, CarEnvironment *car_environment)
    {
        for(int i = 0; i < iterations; ++i)
        {
            std::cout << "Iteration " << m_iterations_done++ << "\tbest value " << *std::min_element(std::begin(population_values), std::begin(population_values) + mu) << "\n";
            get_parent_indices();
            create_children_crossover();
            mutate_children();
            evaluate_population(car_environment, mu, lambda + mu);
            choose_new_population();
        }

        int idx = std::min_element(population_values.begin(), population_values.begin() + mu) - population_values.begin();
        
        assert(idx < population.size());

        std::cout << "{";
        for(auto i : population.at(idx)) {
            std::cout << i << ",";
        }
        std::cout << "}\n";
    }


private:
    std::mt19937      gen;
    bool          is_plus;
    std::array<std::array<f_T, CHR_LEN>, mu + lambda> population;
    std::array<f_T, mu + lambda> population_values;
    std::array<f_T, mu> fitness_values;
    std::array<int, lambda> parent_indices;
    std::array<int, mu + lambda> argsort_array;
    int m_iterations_done;

    void initialize_population()
    {
        std::normal_distribution<f_T> d(0.0, 1.0);
        for (int i = 0; i < mu; ++i)
        {
            for(auto &individual : population.at(i))
            {
                individual = d(gen);
            }
        }
    }

    void evaluate_population(CarEnvironment *car_environment, int l_index, int r_index)
    {
        std::vector<Chromosome> chrs;
        chrs.reserve(r_index - l_index);

        for (int i = l_index; i < r_index; ++i)
        {   
            assert(i >= 0);
            assert(i < population.size());
            chrs.push_back(Chromosome(population.at(i)));
        }

        auto p = car_environment->evaluate_function(chrs);
        for (int i = l_index; i < r_index; ++i)
        {
            assert(i >= 0);
            assert(i < population_values.size());
            population_values.at(i) = p[i - l_index];
        }
    }

    void get_parent_indices()
    {
        auto max_value = *std::max_element(std::begin(population_values), std::begin(population_values) + mu);
        f_T sum = 0;
        for (int i = 0; i < mu; ++i)
        {
            sum += (fitness_values.at(i) = max_value - population_values.at(i));
        }

        if (sum > 0)
        {
            for (int i = 0; i < mu; ++i)
            {
                fitness_values.at(i) /= sum;
            }
        }
        else 
        {
            for (int i = 0; i < mu; ++i)
            {
                fitness_values.at(i) = 1.0 / static_cast<f_T>(mu);
            }
        }

        std::discrete_distribution<int> d(std::begin(fitness_values), std::end(fitness_values));

        for (int i = 0; i < lambda; ++i)
        {
            parent_indices.at(i) = d(gen);
        }
    }

    void create_children_crossover()
    {
        for (int i = mu; i < mu + lambda; ++i)
        {
            population.at(i) = population.at(parent_indices.at(i - mu));
        }

        for (int i = mu; i < mu + lambda; i += 2)
        {
            auto [c1, c2] = crossover(population.at(i), population.at(i + 1));
            population.at(i) = c1, population.at(i + 1) = c2;
        }
    }

    std::pair <std::array<f_T, CHR_LEN>, std::array<f_T, CHR_LEN>> crossover(std::array<f_T, CHR_LEN> a, std::array<f_T, CHR_LEN> b)
    {
        std::uniform_real_distribution<f_T> d(0.0, 1.0);

        std::array<f_T, CHR_LEN> c1, c2;

        for (int i = 0; i < CHR_LEN; ++i) 
        {
            if (d(gen) <= 0.5f)
            {
                c1.at(i) = a.at(i);
            }
            else c1.at(i) = b.at(i);

            if (d(gen) <= 0.5f)
            {
                c2.at(i) = a.at(i);
            }
            else c2.at(i) = b.at(i);
        }

        return std::make_pair(c1, c2);
    }
  
    void mutate_children()
    {
        std::uniform_real_distribution<f_T> d(0.0, 1.0);
        std::uniform_real_distribution<f_T> de(-1.0, 1.0);

        for (int i = mu; i < mu + lambda; ++i)
        {
            if (d(gen) >= 0.05f) continue;

            for (int j = 0; j < CHR_LEN; ++j)
            {
                population.at(i).at(j) += de(gen);
            }
        }

    }
    void choose_new_population()
    {
        std::iota(std::begin(argsort_array), std::end(argsort_array), 0);
        
        int left_index = is_plus ? 0 : mu;

        std::sort(std::begin(argsort_array) + left_index, std::end(argsort_array), [&](int l_i, int r_i) {
            return population_values.at(l_i) < population_values.at(r_i);
        });

        std::array<std::array<f_T, CHR_LEN>, mu + lambda> population_cpy(population);
        std::array<f_T, mu + lambda> population_values_cpy(population_values);

        for (int i = 0; i < mu; ++i)
        {
            population.at(i)        = population_cpy.at(argsort_array.at(left_index + i));
            population_values.at(i) = population_values_cpy.at(argsort_array.at(left_index + i));
        }
    }
};