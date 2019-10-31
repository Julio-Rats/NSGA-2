#ifndef  NSGA_2
#define   NSGA_2

#include <iostream>
#include <cstdlib>
#include <cfloat>
#include <vector>
#include <fstream>
#include "./../Simulador CANbus/simulador.hpp"

#define     LENGTH_CHAR           64
#define     TIME_SIMULATION       12000

/******************************************************************

Estrutura (classe) da solução do problema

*******************************************************************/

class Indiviual
{
  public:
      double          wcrt;                  //  Pior tempo de atraso cumulado
      double          frames_burst;          //  Numero maximo de frames ocorrido em  um Burst
      double          time_mean_burst;       //  Tempo medio dos bursts
      Frame_CAN*      candb_solution;        //  Conjunto de frames
      unsigned int    transcend;
      double          distance;
      std::vector<Indiviual*> dominated;
};


/******************************************************************

Variaveis de controle do algoritimo genetico

*******************************************************************/

u_int16_t             length_frames;      // Numero de frames na solução
unsigned int          length_frontier;    // Numero de soluções na nuvem de soluções
unsigned int          ga_length_population;
unsigned int          ga_number_generation;
double                ga_prob_mution;
unsigned int          number_funct_objective;
std::vector<Indiviual>  ga_sol_population;     // Conjunto com todas as soluções (population)

Simulator_CAN* simulator;
Indiviual      first_man;

char            psa_arq_result[LENGTH_CHAR];
/******************************************************************

Escopos das funções implementadas

*******************************************************************/

void NSGA_II();
std::vector<std::vector<Indiviual>> nodominated_sort();
std::vector<Indiviual> make_new_population (std::vector<Indiviual> sort_population);
std::vector<Indiviual> crowding_distance_assigment(std::vector<std::vector<Indiviual>> frontiers);
bool is_dominated(Indiviual dmt, Indiviual slv);
std::vector<Indiviual> random_population(Indiviual person_reference);
void run_simulation(Indiviual& person);


#endif
