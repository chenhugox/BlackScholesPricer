#include <iostream>
#include <random>
#include <string>
#include "Option.h"
#include "VanillaOption.h"
#include "BlackScholesPricer.h"
#include "BinaryTree.h"
#include "CRRPricer.h"
#include "DigitalOption.h"
#include "AsianOption.h"
#include "BlackScholesMCPricer.h"
#include "MT.h"
#include "AmericanOption.h"

int main()
{
    double S0(100), K(101), T(5), r(0.01), sigma(0.1);
    #pragma region Part1    
    {
        // Vanilla Option
        CallOption opt1(T, K);
        PutOption opt2(T, K);
        //Black Scholes pricer
        BlackScholesPricer BSpricerCall(&opt1, S0, r, sigma);
        BlackScholesPricer BSpricerPut(&opt2, S0, r, sigma);
        std::cout << "######################### Vanilla option ################################" << std::endl;
        std::cout << "Black scholes call price = " << BSpricerCall() << ", delta = " << BSpricerCall.delta() << std::endl;
        std::cout << "Black scholes put price = " << BSpricerPut() << ", delta = " << BSpricerPut.delta() << std::endl;
        std::cout << std::endl;
        
        int N(5);
        double U = 0.05; /*exp(sigma * sqrt(T / N)) - 1.0;*/
        double D = -0.045; /*exp(-sigma * sqrt(T / N)) - 1.0;*/
        double R = 0.01; /*exp(r * T / N) - 1.0;*/
        // CRR Pricer
        CRRPricer crr_pricer1(&opt1, N, S0, U, D, R);
        std::cout << "Calling CRR pricer with depth = " << N << std::endl;
        std::cout << std::endl;
        std::cout << "CRR pricer computed call price = " << crr_pricer1() << std::endl;
        std::cout << "CRR pricer explicit formula call price = " << crr_pricer1(true) << std::endl;
        std::cout << std::endl;


        CRRPricer crr_pricer2(&opt2, N, S0, U, D, R);
        std::cout << "Calling CRR pricer with depth = " << N << std::endl;
        std::cout << std::endl;
        std::cout << "CRR pricer computed put price = " << crr_pricer2() << std::endl;
        std::cout << "CRR pricer explicit formula put price = " << crr_pricer2(true) << std::endl;
        
    }
    #pragma endregion

    {
        //Digital Option
        DigitalCallOption opt1(T, K);
        DigitalPutOption opt2(T, K);
        std::cout << "######################### Digital option ################################" << std::endl;
        // Black Scholes Pricer
        BlackScholesPricer pricer1(&opt1, S0, r, sigma);
        std::cout << "BlackScholes digital call price = " << pricer1() << ", delta = " << pricer1.delta() << std::endl;
            
        BlackScholesPricer pricer2(&opt2, S0, r, sigma);
        std::cout << "BlackScholes digital put price = " << pricer2() << ", delta = " << pricer2.delta() << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;

        int N(5);
        double U = 0.05; /*exp(sigma * sqrt(T / N)) - 1.0;*/
        double D = -0.045; /*exp(-sigma * sqrt(T / N)) - 1.0;*/
        double R = 0.01; /*exp(r * T / N) - 1.0;*/
            
        // CRR pricer  
        CRRPricer crr_pricer1(&opt1, N, S0, U, D, R);
        std::cout << "Calling CRR pricer with depth = " << N << std::endl;
        std::cout << std::endl;
        std::cout << "CRR pricer computed digital call price = " << crr_pricer1() << std::endl;
        std::cout << "CRR pricer explicit formula digital call price = " << crr_pricer1(true) << std::endl;
        std::cout << std::endl;
            
        CRRPricer crr_pricer2(&opt2, N, S0, U, D, R);
        std::cout << "Calling CRR pricer with depth = " << N << std::endl;
        std::cout << std::endl;
        std::cout << "CRR pricer computed digital put price = " << crr_pricer2() << std::endl;
        std::cout << "CRR pricer explicit formula digital put price = " << crr_pricer2(true) << std::endl;
    }

    { //TODO TEST A changer
     std::cout << std::endl << "########################## Binary Tree Example #############################" << std::endl << std::endl;
    
     BinaryTree<bool> t1(3);
     t1.setDepth(3);
     t1.setNode(1, 1, true);
     t1.display();
     t1.setDepth(5);
     t1.display();
     t1.setDepth(3);
     t1.display();
     
     BinaryTree<double> t2;
     t2.setDepth(2);
     t2.setNode(2, 1, 3.14);
     t2.display();
     t2.setDepth(4);
     t2.display();
     t2.setDepth(3);
     t2.display();
     
     BinaryTree<int> t3;
     t3.setDepth(4);
     t3.setNode(3, 0, 188);
     t3.display();
     t3.setDepth(2);
     t3.display();
     t3.setDepth(4);
     t3.display();
     }


     {
        std::cout << std::endl;
        std::cout << "###################################### Monte Carlo ####################################" << std::endl;
        std::cout << std::endl;
        
        std::vector<Option*> opt_ptrs;
        opt_ptrs.push_back(new CallOption(T, K));
        opt_ptrs.push_back(new PutOption(T, K));
        opt_ptrs.push_back(new DigitalCallOption(T, K));
        opt_ptrs.push_back(new DigitalPutOption(T, K));


        std::vector<double> fixing_dates;
        for (int i = 1; i <= 5; i++) {
            fixing_dates.push_back(0.1 * i);
        }
        opt_ptrs.push_back(new AsianCallOption(fixing_dates, K));
        opt_ptrs.push_back(new AsianPutOption(fixing_dates, K));

        std::vector<double> ci;
        BlackScholesMCPricer* pricer;

        std::vector<std::string> options{ "Call", "Put", "Digital Call", "Digital Put", "Asian Call", "Asian Put" };
        int counter = 0;
        for (auto& opt_ptr : opt_ptrs) {
            pricer = new BlackScholesMCPricer(opt_ptr, S0, r, sigma);
            do {
                pricer->generate(10);
                ci = pricer->confidenceInterval();
            } while (ci[1] - ci[0] > 1e-2);
            std::cout << "nb samples : " << pricer->getNbPaths() << std::endl;
            std::cout << "price of " << options[counter] << " : " << (*pricer)() << std::endl << std::endl;
            counter++;
        }
    }


        #pragma region TD8
    std::cout << std::endl;
    std::cout << "********************************** TD 8 **********************************" << std::endl;
    std::cout << std::endl;
    {
        std::vector<Option*> opt_ptrs;
        opt_ptrs.push_back(new CallOption(T, K));
        opt_ptrs.push_back(new PutOption(T, K));
        opt_ptrs.push_back(new DigitalCallOption(T, K));
        opt_ptrs.push_back(new DigitalPutOption(T, K));
        opt_ptrs.push_back(new AmericanCallOption(T, K));
        opt_ptrs.push_back(new AmericanPutOption(T, K));

        CRRPricer* pricer;
        std::vector<std::string> options{ "Call", "Put", "Digital Call", "Digital Put", "American Call", "American Put" };
        for (auto& opt_ptr : opt_ptrs) {
            pricer = new CRRPricer(opt_ptr, 5, S0, r, sigma);
            pricer->compute();
            std::cout << "price : " << (*pricer)() << std::endl << std::endl;

            delete pricer;
            delete opt_ptr;
        }
    }
    #pragma endregion
}