#include <iostream>
#include <cmath>
#include <vector>
#include <numeric> 

//To The Viewer: Please select the PNG files to view the reference regarding the pricing inputs! (Program Prices EUROPEAN Calls, Quotes are AMERICAN calls) This explains the delta between pricing outputs!

//Prototyping Functions!
double RationalApproximation(double t);
double NORMSINV(double p);
double RAND(); 


class Simulation
{
  private:
  double N_Simulations; 
  double InitialAssetPrice; 
  double StrikePrice;
  double TimeUntilExpiration;  
  double Drift;
  double Volatility;
  double RiskFreeRate; 
  std::vector<double>TerminalValues = std::vector<double>(0, N_Simulations); 
  std::vector<double>Payoffs = std::vector<double>(0, N_Simulations); 



  public:
  Simulation(double N_Sim, double IAP, double SP, double TTE, double DFT, double V, double RFR)
  {
    N_Simulations = N_Sim; 
    InitialAssetPrice = IAP;
    StrikePrice = SP - 1; 
    TimeUntilExpiration = TTE +1; //Adjustment For Upcoming Loop
    Drift = DFT;
    Volatility = V;
    RiskFreeRate = RFR; 
  }

  void GenerateCallPrice()
  {
    for(int i = 0; i<N_Simulations; i++)
    {
      std::vector<int>Prices(TimeUntilExpiration, 0); 
      Prices[0] = InitialAssetPrice; 
      double Days = 1;
      //Generate Asset Prices Up To TimeUntilExpiration
      for(int j = 1; j<TimeUntilExpiration; ++j)
      {
        double Roll1 = NORMSINV(RAND()); //Roll Random Number
        double Roll2 = NORMSINV(RAND());
        double RandomStochastic = NORMSINV(RAND()); 

        Prices[j] = (Prices[j-1] * exp(Drift + (Volatility*sqrt(Days/252)) * RandomStochastic)); 
        Days++;
      }
      
      TerminalValues.push_back(Prices[Prices.size() -1]); 
      Prices.clear(); 
    }

    //Populate Payoff Vector With Profit Or 0
    for(double TerminalValue: TerminalValues)
    {
      (TerminalValue - StrikePrice > 0)? Payoffs.push_back(TerminalValue - StrikePrice) : Payoffs.push_back(0);
    }
    
    //Compute Average Payoff
    float AveragePayoff = std::accumulate(Payoffs.begin(), Payoffs.end(), 0.0) / Payoffs.size();
    
    //Present Value Of Payoff
    double CallPrice = (AveragePayoff * exp(-TimeUntilExpiration/252 * RiskFreeRate));

    std::cout << "Call Price: $" << CallPrice << '\n';

  }

  void GeneratePutPrice()
  {
    for(int i = 0; i<N_Simulations; i++)
    {
      std::vector<int>Prices(TimeUntilExpiration, 0); 
      Prices[0] = InitialAssetPrice; 
      double Days = 1;

      for(int j = 1; j<TimeUntilExpiration; ++j)
      {
        double Roll1 = NORMSINV(RAND());
        double Roll2 = NORMSINV(RAND());
        double RandomStochastic = NORMSINV(RAND()); 

        Prices[j] = (Prices[j-1] * exp(Drift + (Volatility*sqrt(Days/252)) * RandomStochastic)); 
        Days++;
      }
      
      TerminalValues.push_back(Prices[Prices.size() -1]); 
      Prices.clear(); 
    }

    for(double TerminalValue: TerminalValues)
    {
      (StrikePrice - TerminalValue > 0)? Payoffs.push_back(StrikePrice - TerminalValue) : Payoffs.push_back(0);
    }
    
    float AveragePayoff = std::accumulate(Payoffs.begin(), Payoffs.end(), 0.0) / Payoffs.size();

    double PutPrice = (AveragePayoff * exp(-TimeUntilExpiration/252 * RiskFreeRate));

    std::cout << "Put Price: $" << PutPrice << '\n';

  }

  

};

int main()
{
std::cout << "Parameters For First Simulation: \n";
std::cout << "$142.51 Underlying\n"; 
std::cout << "143 Strike \n";
std::cout << "2 Days Until Expiration \n";
std::cout << "24.63% Implied Volatility \n";
std::cout << "0% Risk Free Rate \n";
 Simulation Sim1(1000000, 142.51, 143, 2, 0, .2463, .00);
 Sim1.GenerateCallPrice();

std::cout << '\n'; 

std::cout << "Parameters For Second Simulation: \n";
std::cout << "$141.50 Underlying\n"; 
std::cout << "143 Strike \n";
std::cout << "1 Day Until Expiration \n";
std::cout << "25.93% Implied Volatility \n";
std::cout << "0% Risk Free Rate \n";
 Simulation Sim2(1000000, 141.50, 143, 1, 0, .2593, .00);

 Sim2.GeneratePutPrice(); 

  return 0;
}


////////////////////////////////////////////////////////////////////////////////////////
///////Inverse CDF and Rational Approximation  Code Sourced From Mr. John D Cook!////// //////          https://www.johndcook.com/blog/cpp_phi_inverse/               ///////// 
////////////////////////////////////////////////////////////////////////////////////////

double RationalApproximation(double t)
{
    double c[] = {2.515517, 0.802853, 0.010328};
    double d[] = {1.432788, 0.189269, 0.001308};
    return t - ((c[2]*t + c[1])*t + c[0]) / 
               (((d[2]*t + d[1])*t + d[0])*t + 1.0);
}

double NORMSINV(double p)
{
  
    if (p < 0.5)
    {
        // F^-1(p) = - G^-1(p)
        return -RationalApproximation( sqrt(-2.0*log(p)) );
    }
    else
    {
        // F^-1(p) = G^-1(1-p)
        return RationalApproximation( sqrt(-2.0*log(1-p)) );
    }
}

double RAND()
{
    return rand() / (RAND_MAX + 1.);
}