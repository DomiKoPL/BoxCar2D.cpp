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
#include <fstream>
#include <filesystem>

using f_T = float;

template <int CHR_LEN, int mu, int lambda>
class ES_solver
{
public:
    
    ES_solver(CarEnvironment *car_environment, f_T K, int seed, bool is_es_plus, bool save_to_file = false)
        : is_plus{is_es_plus}, m_save_to_file{save_to_file}
    {
        if(m_save_to_file) 
        {
            int i = 0;
            std::string path = std::string("results//es_");
            if(is_es_plus) path += std::string("plus_");
            while(std::filesystem::exists(path + std::to_string(i))) i++;
            std::cerr << "Opening file: " << path + std::to_string(i) << "\n";
            m_out_file.open(path + std::to_string(i), std::fstream::out);
            m_cars_file.open(path + std::to_string(i) + std::string(".cars"), std::fstream::out);

            assert(m_out_file.is_open());
            assert(m_cars_file.is_open());
            m_out_file << "seed=" << seed << "\n";
            m_cars_file << "seed=" << seed << "\n";
        }

        std::clog << "Running ES solver instance\n";
        static_assert(mu <= lambda);
        t  = K / std::sqrt(2.0 * CHR_LEN);
        t0 = K / std::sqrt(2.0 * std::sqrt(CHR_LEN));
        gen = std::mt19937(seed);
        
        initialize_sigmas();
        initialize_population();
        evaluate_population(car_environment, 0, mu);
        if(m_save_to_file) {
            for(int i = 0; i < mu ; ++i) m_out_file << population_values.at(i) << " ";
            m_out_file << std::endl;
        }
        for(int i = 0; i < mu ; ++i) std::cout << i << "\t" << population_values.at(i) << "\n";
        int idx = std::min_element(population_values.begin(), population_values.begin() + mu) - population_values.begin();
        std::cout << "\tbest value " << *std::min_element(std::begin(population_values), std::begin(population_values) + mu) << "\n";

        if(m_save_to_file) 
        {

            assert(m_out_file.is_open());
            m_cars_file << "{ ";
            for (int i = 0; i < 32; ++i)
            {
                m_cars_file << population.at(idx).at(i) << ", "[i == 31];
            }
            m_cars_file << "}" << std::endl;
        }
    }

    ~ES_solver() 
    {
        m_out_file.close();
        m_cars_file.close();
    }

    void run(int iterations, CarEnvironment *car_environment)
    {
        for(int i = 0; i < iterations; ++i)
        {
            std::cout << "Iteration " << m_iterations_done++ << "\tbest value " << *std::min_element(std::begin(population_values), std::begin(population_values) + mu) << "\n";
            get_parent_indices();
            create_children_population();
            evaluate_population(car_environment, mu, lambda + mu);
            choose_new_population();

            if(m_save_to_file) 
            {
                assert(m_out_file.is_open());
                for (int i = 0; i < mu; ++i) m_out_file << population_values.at(i) << " ";
                m_out_file << std::endl;

                int idx = std::min_element(population_values.begin(), population_values.begin() + mu) - population_values.begin();
        
                m_cars_file << "{ ";
                for (int i = 0; i < 32; ++i)
                {
                    m_cars_file << population.at(idx).at(i) << ", "[i == 31];
                }
                m_cars_file << "}" << std::endl;
            }
        }

        int idx = std::min_element(population_values.begin(), population_values.begin() + mu) - population_values.begin();
        
        assert(idx < population.size());

        // std::cout << "{";
        // for(auto i : population.at(idx)) {
        //     std::cout << i << ",";
        // }
        // std::cout << "}\n";
        // for (int i = 0; i < 32; ++i)
        // {
        //     // std::cout << "Gene " << i << "\t" << population.at(idx).at(i) << "\n"; 
        // }

        // for(int i = 0; i < mu; ++i) std::cout << i << "\t" << population_values[i] << "\n";
    }


private:
    std::mt19937      gen;
    bool          is_plus;
    f_T                 t;
    f_T                t0;
    std::array<std::array<f_T, CHR_LEN>, mu + lambda> population;
    std::array<std::array<f_T, CHR_LEN>, mu + lambda> sigmas;
    std::array<f_T, mu + lambda> population_values;
    std::array<f_T, mu> fitness_values;
    std::array<int, lambda> parent_indices;
    std::array<int, mu + lambda> argsort_array;
    int m_iterations_done;
    bool m_save_to_file;
    std::fstream m_out_file, m_cars_file;

    void initialize_sigmas()
    {
        std::uniform_real_distribution<f_T> d(0.1, 0.9);
        for (int i = 0; i < mu; ++i)
        {
            for(auto &sigma : sigmas.at(i))
            {
                sigma = d(gen);
                std::cout << sigma << " ";
            }
            std::cout << "\n";
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

// int main(int argc, char** argv)
// {
//     std::cout << "XD\n";
//     // dlgosc chromosomu, mi, lambda, 
//     TestCase *test_case = new TestCase();
//     test_case->SetBlocked(false);
//     ES_solver<32, 20, 20> solver(TestCase::evaluate_function, test_case, std::stod(argv[1]), true);
//     test_case->SetBlocked(true);
//     // test_case->blocked = true;
//     // solver.run(30, test_case);

//     auto x = std::thread(&ES_solver<32, 20, 20>::run, &solver, 30, test_case);

//     while(1) {
//         std::cout << "Siema siema\n";
//         std::cin.get();
//         Settings settings;
//         test_case->SetBlocked(false);
//         test_case->Step(settings);
//         test_case->SetBlocked(true);
//     }
//     x.join();
// }
