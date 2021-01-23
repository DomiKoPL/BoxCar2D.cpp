#pragma GCC optimize ("Ofast") 
#include "./test.h"
#include "./settings.h"
#include "./tests/straight_line.hpp"

#include <iostream>
#include <chrono>
#include <random>

using f_T = float;
std::vector<float> evaluate_function(std::array<Chromosome, 20> chromosomes)
{
    // Trzeba wiedzieć jaki test
    // create_fnc
    // s_test->Step(s_settings);
    Settings s_settings;
    StraightLine* straightLine = new StraightLine();
    straightLine->testbedMode = false;
    // std::array<Chromosome, 20> chromosomes;
    // for(auto &p : chromosomes) p.Random();
    straightLine->CreateCars(chromosomes);

    auto tp_ = std::chrono::high_resolution_clock::now();
    auto tp = std::chrono::time_point(tp_);
    auto end_tp = std::chrono::time_point(tp_ + std::chrono::seconds(1));
    std::cout << "Evaluating cars\n";
    while(std::chrono::high_resolution_clock::now() < end_tp)
    {
        straightLine->Step(s_settings);
        
        // for (int j = 0; j < 20; ++j)
        // {
        //     std::cout << "Car " << j << "\tx: " << 
        //         straightLine->m_cars.at(j)->GetBody()->GetPosition().x << "\ty: " << 
        //         straightLine->m_cars.at(j)->GetBody()->GetPosition().y << "\n";
        // }

        // std::cout << "\n\n";
    }
    std::cout << "Cars evaluated\n";
    std::vector<float> dists;

    for(int i = 0; i < 20; ++i)
    {
        dists.push_back(1000.0f - straightLine->m_cars.at(i)->GetBody()->GetPosition().x);
    }
    return dists;
}


template <int CHR_LEN, int mu, int lambda>
class ES_solver
{
public:

    ES_solver(int* F, f_T K, bool is_es_plus, int max_iterations = 200)
        : is_plus{is_es_plus}, max_iterations{max_iterations}
    {
        static_assert(mu <= lambda);
        // std::cout << K << "\t" << CHR_LEN << "\t" << std::sqrt(2.0 * CHR_LEN) << "\t" << K / std::sqrt(2.0 * CHR_LEN) << "\n";
        t  = K / std::sqrt(2.0 * CHR_LEN);
        t0 = K / std::sqrt(2.0 * std::sqrt(CHR_LEN));
        gen = std::mt19937(rd());
        for(int i = 0; i < mu ; ++i) std::cout << i << "\t" << population_values.at(i) << "\n";
        initialize_sigmas();
        initialize_population();
        evaluate_population(0, mu);
        std::cout << "\tbest value " << *std::min_element(std::begin(population_values), std::begin(population_values) + mu) << "\n";
    }

    void run()
    {
        for(int i = 0; i < max_iterations; ++i)
        {
            std::cout << "Iteration " << i << "\tbest value " << *std::min_element(std::begin(population_values), std::begin(population_values) + mu) << "\n";
            get_parent_indices();
            // for(int i = 0; i < lambda; ++i)std::cout << i << " PTR\t" << parent_indices.at(i) << "\n\n";
            create_children_population();
            // for(int i = mu; i < mu + lambda; ++i)
            // {
            //     std::cout << i << "\t" << "\n";
            //     for(int j = 0; j < CHR_LEN; ++j) std::cout << sigmas.at(i).at(j) << " ";
            //     std::cout << "\n";

            // }
            evaluate_population(mu, lambda + mu);
            // std::cout << "\n\nVALUES\n";
            // for(int i = 0; i < mu + lambda; ++i) std::cout << i << "\t" << population_values.at(i) << "\n";
            choose_new_population();
        }

        int idx = std::max_element(population_values.begin(), population_values.begin() + mu) - population_values.begin();

        for (int i = 0; i < 32; ++i)
        {
            std::cout << "Gene " << i << "\t" << population.at(idx).at(i) << "\n"; 
        }

        for(int i = 0; i < mu; ++i) std::cout << i << "\t" << population_values[i] << "\n";
    }


private:
    std::random_device rd;
    std::mt19937      gen;
    bool          is_plus;
    f_T                 t;
    f_T                t0;
    int max_iterations;
    std::array<std::array<f_T, CHR_LEN>, mu + lambda> population;
    std::array<std::array<f_T, CHR_LEN>, mu + lambda> sigmas;
    std::array<f_T, mu + lambda> population_values;
    std::array<f_T, mu> fitness_values;
    std::array<int, lambda> parent_indices;
    std::array<int, mu + lambda> argsort_array;
    //          F;

    void initialize_sigmas()
    {
        std::uniform_real_distribution<f_T> d(0.1, 0.9);
        for (int i = 0; i < mu; ++i)
        {
            for(auto &sigma : sigmas.at(i))
            {
                sigma = d(gen);
            }
        }
    }

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

    void evaluate_population(int l_index, int r_index)
    {
        std::array<Chromosome, 20> chrs;
        for (int i = l_index; i < r_index; ++i)
        {
            chrs.at(i - l_index) = Chromosome(population.at(i));
            // f_T suma = 0;
            // for (int j = 0; j < CHR_LEN; ++j) suma += population.at(i).at(j) * population.at(i).at(j); 
            // population_values.at(i) = p[i - l_index];
        }
        auto p = evaluate_function(chrs);
        for (int i = l_index; i < r_index; ++i)
        {
            f_T suma = 0;
            // for (int j = 0; j < CHR_LEN; ++j) suma += population.at(i).at(j) * population.at(i).at(j); 
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

    void create_children_population()
    {
        f_T add_to_all = std::normal_distribution<f_T> (0.0, t0)(gen);
        std::normal_distribution<f_T> d(0.0, t);

        for (int i = mu; i < mu + lambda; ++i)
        {
            population.at(i) = population.at(parent_indices.at(i - mu));
            sigmas.at(i) = sigmas.at(parent_indices.at(i - mu));
        }

        for (int i = mu; i < mu + lambda; ++i)
        {
            for (int j = 0; j < CHR_LEN; ++j)
            {
                auto& sigma = sigmas.at(i).at(j);
                auto& ref = population.at(i).at(j);
                // std::cout << "Sigma przed\t" << sigmas.at(i).at(j) << "\n";
                // std::cout << "Losuje " << d(gen) << " " << add_to_all << "\t" << t << "\t" << t0 << "\n";;
                sigma *= std::exp(d(gen) + add_to_all);
                // std::cout << "Sigma po\t" << sigmas.at(i).at(j) << "\n\n";
                ref += std::normal_distribution<f_T>(0.0, sigmas.at(i).at(j))(gen);
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
        std::array<std::array<f_T, CHR_LEN>, mu + lambda> sigmas_cpy(sigmas);
        std::array<f_T, mu + lambda> population_values_cpy(population_values);

        for (int i = 0; i < mu; ++i)
        {
            population.at(i)        = population_cpy.at(argsort_array.at(left_index + i));
            sigmas.at(i)            = sigmas_cpy.at(argsort_array.at(left_index +  i));
            population_values.at(i) = population_values_cpy.at(argsort_array.at(left_index + i));
        }
    }
};

int main(int argc, char** argv)
{
    std::cout << "XD\n";
    // dlgosc chromosomu, mi, lambda, 
    ES_solver<32, 20, 20> solver(nullptr, std::stod(argv[2]), true, std::stoi(argv[1]));
    solver.run();
}