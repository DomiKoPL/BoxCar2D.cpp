#include "f1_straight_line.hpp"

F1StraightLine::F1StraightLine()
{
	srand(time(0));

	blocked_environment = new StraightLine(true);

    thread = std::thread([&]() {
        Chromosome es_car{{0.0364161,1.85795,-1.58979,-1.21015,0.155099,0.331412,-0.0295114,0.296463,0.0331009,-0.0284276,1.05205,0.582493,-2.236,-0.00489429,-0.70946,-0.597465,0.352769,0.0163485,2.81135,-2.28046,0.12093,0.212239,0.696573,-2.74378,0.0435132,0.782894,-0.02612,-0.0130346,0.227958,-0.22416,1.40178,-0.799165,}};
        Chromosome es_car2{{0.882191,-1.11781,0.280716,-0.389724,-1.32138,1.16672,0.64839,0.622571,-0.319982,1.04794,0.868475,0.310728,1.17357,-2.98021,2.19083,0.688392,-0.216209,-0.759769,-2.76066,0.187717,-0.0212164,-0.970921,1.79623,-1.15885,-1.13939,-0.0480281,-1.2926,1.70011,-0.570046,-1.13356,-0.474225,-0.607275,}};
        Chromosome es_car3{{5.3704,0.652729,2.7511,-0.102923,-1.76355,-0.0222135,0.925551,0.103014,0.199892,-0.144692,-3.06231,-2.21308,-1.16967,0.483831,1.25798,0.720035,1.25999,0.149506,-0.0531797,1.01005,2.02188,4.19546,2.45542,0.70434,-0.100496,0.176812,-0.580364,2.74529,-0.380293,-0.0999528,-1.2894,0.0706207,}};
        Chromosome f1_car{
            std::array<float, 32>{
                0.f,
                0.f,
                0.f,
                0.f,

                0.f,
                0.f,
                0.f,
                0.f,

                0.f,
                0.f,
                0.f,
                0.f,

                0.f,
                0.f,
                0.f,
                0.f,

                0.f,
                0.4f,
                1.f,
                0.4f,

                0.0f,
                3.f,
                1.f,
                0.4f,

                0.f,
                3.5f,
                0.f,
                0.f,
                
                3.14f / 2.f,
                0.2f,
                0.f,
                0.f
            }
        };

        std::vector<Chromosome> cars{
            f1_car, es_car, es_car2, es_car3
        };
        
        while(1) {
            auto res = blocked_environment->evaluate_function(cars);
            for(auto i : res) std::cout << i << "\n";
        }
    });
}

void F1StraightLine::Step(Settings& settings)
{	
	blocked_environment->Step(settings);
	Test::Step(settings);
	blocked_environment->DeleteCars();
}

Test* F1StraightLine::Create()
{
	return new F1StraightLine;
}

F1StraightLine::~F1StraightLine() {
    thread.detach();
}

static int testIndex = RegisterTest("StraightLine", "F1StraightLine", F1StraightLine::Create);