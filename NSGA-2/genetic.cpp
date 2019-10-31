#include "genetic.hpp"

using namespace std;

void NSGA_II()
{
    vector<Indiviual>         sort_population;
    vector<vector<Indiviual>> frontiers;
    unsigned int number_frontiers;

    ga_sol_population               = random_population(first_man);
    unsigned int generation_current = 1;

    while (generation_current <= ga_number_generation)
    {
        // std::cout << "Geração " << generation_current << '\n';
        frontiers         = nodominated_sort();
        sort_population   = crowding_distance_assigment(frontiers);
        ga_sol_population = make_new_population(sort_population);
        generation_current++;
    }
}

Indiviual* crossover(Indiviual& parent1, Indiviual& parent2)
{
    Indiviual* new_person;
    new_person = new Indiviual[2];
    if (!new_person)
    {
      std::cout << "errr" << '\n';
      exit(1);
    }
    int cross1 = rand()%(length_frames-1);
    int cross2;
    do
    {
        cross2 = (rand()%length_frames);
    }while(cross2 == cross1);

    new_person[0].candb_solution = (Frame_CAN*) malloc(sizeof(Frame_CAN)*(length_frames));
    new_person[1].candb_solution = (Frame_CAN*) malloc(sizeof(Frame_CAN)*(length_frames));

    if ((!new_person[0].candb_solution) || (!new_person[1].candb_solution))
    {
        std::cout << "aedad erro" << '\n';
        exit(2);
    }

    if (cross1 > cross2)
    {
        int temp = cross1;
        cross1 = cross2;
        cross2 = temp;
    }

    for (size_t i = 0; i < length_frames; i++)
    {
        if ((i < cross1)||(i > cross2))
        {
            new_person[0].candb_solution[i] = parent1.candb_solution[i];
            new_person[1].candb_solution[i] = parent2.candb_solution[i];
        }
        else
        {
            new_person[1].candb_solution[i] = parent1.candb_solution[i];
            new_person[0].candb_solution[i] = parent2.candb_solution[i];
        }
    }
    for (size_t i = 0; i < length_frames; i++)
    {
        double mut = ((double)rand()/RAND_MAX);
        if (mut <= ga_prob_mution)
        {
            new_person[0].candb_solution[i].delay_time += (rand()%(11))-5;
            new_person[1].candb_solution[i].delay_time += (rand()%(11))-5;

            if (new_person[0].candb_solution[i].delay_time < 0)
                new_person[0].candb_solution[i].delay_time = 0;

            if (new_person[1].candb_solution[i].delay_time < 0)
                new_person[1].candb_solution[i].delay_time = 0;

            if (new_person[0].candb_solution[i].delay_time > new_person[0].candb_solution[i].deadline_time)
                new_person[0].candb_solution[i].delay_time = new_person[0].candb_solution[i].deadline_time;

            if (new_person[1].candb_solution[i].delay_time > new_person[1].candb_solution[i].deadline_time)
                new_person[1].candb_solution[i].delay_time = new_person[1].candb_solution[i].deadline_time;
        }
    }
    parent1.dominated.clear();
    parent1.transcend = 0;
    parent2.dominated.clear();
    parent2.transcend = 0;
    return new_person;
}

vector<Indiviual> make_new_population(vector<Indiviual> sort_population)
{
      vector<Indiviual> new_generation;

      for (size_t i = 0; i < sort_population.size()-1; i+=2)
      {
          Indiviual* sons = crossover(sort_population[i],sort_population[i+1]);
          new_generation.push_back(sort_population[i]);
          new_generation.push_back(sort_population[i+1]);
          new_generation.push_back(sons[0]);
          new_generation.push_back(sons[1]);
      }
      return new_generation;
}

void sort_value_object(vector<Indiviual>& sort_list, unsigned int funct)
{
      double value;
      unsigned int pos_better;
      Indiviual temp;
      for (size_t i = 0; i < sort_list.size(); i++)
      {
          pos_better = i;
          switch (funct)
          {
            case 0:
                value = sort_list[i].wcrt;
            break;
            case 1:
                value = sort_list[i].frames_burst;
            break;
            case 2:
                value = sort_list[i].time_mean_burst;
            break;
          }
          for (size_t j = 0; j < sort_list.size(); j++)
          {
              switch (funct)
              {
                case 0:
                    if (value > sort_list[i].wcrt)
                    {
                        value = sort_list[i].wcrt;
                        pos_better = j;
                    }
                break;
                case 1:
                    if (value > sort_list[i].frames_burst)
                    {
                        value = sort_list[i].frames_burst;
                        pos_better = j;
                    }
                break;
                case 2:
                    if (value > sort_list[i].time_mean_burst)
                    {
                        value = sort_list[i].time_mean_burst;
                        pos_better = j;
                    }
                break;
              }
          }
          if (pos_better != i)
          {
              temp = sort_list[i];
              sort_list[i] = sort_list[pos_better];
              sort_list[pos_better] = temp;
          }
      }
}

void sort_distance(vector<Indiviual>& sort_list)
{
    double value;
    unsigned int pos_better;
    Indiviual temp;
    for (size_t i = 0; i < sort_list.size(); i++)
    {
        value = sort_list[i].distance;
        pos_better = i;
        for (size_t j = 0; j < sort_list.size(); j++)
        {
            if (value > sort_list[j].distance)
            {
                value = sort_list[j].distance;
                pos_better = j;
            }
        }
        if (pos_better != i)
        {
            temp = sort_list[i];
            sort_list[i] = sort_list[pos_better];
            sort_list[pos_better] = temp;
        }
    }
}

vector<Indiviual> crowding_distance_assigment(vector<vector<Indiviual>> frontiers)
{
    for (vector<Indiviual>& front : frontiers)
    {
        for (Indiviual& person: front)
            person.distance = 0;

        for (size_t j = 0; j < number_funct_objective; j++)
        {
            sort_value_object(front, j);
            front[0].distance = DBL_MAX;
            front[front.size()-1].distance   = DBL_MAX;

            for (int i = 1; i < front.size()-1; i++)
                if (j == 0)
                {
                  front[i].distance += (front[i+1].wcrt)-(front[i-1].wcrt);
                }
                else if (j == 1)
                {
                  front[i].distance += (front[i+1].frames_burst)-(front[i-1].frames_burst);
                }
                else if (j == 2)
                {
                  front[i].distance += (front[i+1].time_mean_burst)-(front[i-1].time_mean_burst);
                }
        }
    }
    vector<Indiviual> sort_population;
    unsigned int current_length_population = 0;
    for (vector<Indiviual>& front : frontiers)
    {
      sort_distance(front);
      for (Indiviual& e: front)
          if (current_length_population <= (ga_length_population/2))
          {
              sort_population.push_back(e);
              current_length_population++;
          }
          else
          {
              free(e.candb_solution);
          }
    }
    return sort_population;
}

vector<vector<Indiviual>> nodominated_sort()
{
    for (Indiviual& person: ga_sol_population)
        run_simulation(person);

    vector<vector<Indiviual>> frontiers(1);

    for (Indiviual& person_dmt: ga_sol_population)
    {
        for (Indiviual& person_slv: ga_sol_population)
        {
            if (is_dominated(person_dmt, person_slv))
            {
                person_dmt.dominated.push_back(&person_slv);
            }
            else if (is_dominated(person_slv, person_dmt))
            {
                person_dmt.transcend++;
            }
        }
        if (person_dmt.transcend == 0)
        {
            frontiers[0].push_back(person_dmt);
        }
    }
    unsigned int i = 0;
    vector<Indiviual> aux_dominates;
    while (frontiers[i].size() != 0)
    {
        aux_dominates.clear();
        for (Indiviual& person_frt: frontiers[i])
          for (Indiviual* person_dmtd: person_frt.dominated)
          {
              person_dmtd->transcend--;
              if (person_dmtd->transcend == 0)
              {
                  aux_dominates.push_back(*person_dmtd);
              }
          }
        frontiers.push_back(aux_dominates);
        i++;
    }
    frontiers.resize(frontiers.size()-1);
    return frontiers;
}

bool is_dominated(Indiviual dmt, Indiviual slv)
{
    return(
          ((dmt.wcrt <= slv.wcrt)&&(dmt.frames_burst <= slv.frames_burst)&&(dmt.time_mean_burst <= slv.time_mean_burst))
           &&
          ((dmt.wcrt < slv.wcrt)||(dmt.frames_burst < slv.frames_burst)||(dmt.time_mean_burst < slv.time_mean_burst))
       );
}

vector<Indiviual> random_population(Indiviual person_reference)
{
    Indiviual          new_person;
    vector<Indiviual>  new_population;
    for (unsigned int i=0; i < ga_length_population; i++)
    {
          new_person.candb_solution = new Frame_CAN[length_frames];
          if (!new_person.candb_solution)
          {
              std::cerr << "ERRO alocação de memoria para vetor de frames" << '\n';
              exit(3);
          }
          for (unsigned int j=0; j < length_frames; j++)
          {
              new_person.candb_solution[j] = person_reference.candb_solution[j];
              new_person.candb_solution[j].delay_time = (double)(rand()%((int) new_person.candb_solution[j].deadline_time));
          }
          new_population.push_back(new_person);
    }
    return new_population;
}

void run_simulation(Indiviual& person)
{
    simulator = new Simulator_CAN(person.candb_solution, length_frames);
    simulator->run_simulation(TIME_SIMULATION);
    // double delay_sum = 0;
    // for (size_t i = 0; i < length_frames; i++)
    // {
    //     delay_sum += solucao.candb_solution[i].delay_time;
    // }
    // delay_sum *= 100;
    person.transcend       =  0;
    person.wcrt            =  simulator->wcrt;
    person.frames_burst    =  simulator->frames_burst;
    person.time_mean_burst =  simulator->time_mean_burst;
    delete(simulator);
}

void write_arq(vector<vector<Indiviual>> frontiers, const char *path1, const char *path2)
{
    FILE* archive_front = fopen(path1,"w");
    if (!archive_front)
    {
        printf("\n\nERRO ao abrir o aquivo fronteiras.txt\n\n");
        exit(1);
    }
    fprintf(archive_front,"WCRT\tBRUST_SIZE\tTIME_BURST\tN_FRONT\n");
    for (size_t i = 0; i < frontiers.size(); i++)
      for (Indiviual& person: frontiers[i])
      {
          fprintf(archive_front, "%f\t%f\t%f\t%d\n", person.wcrt, person.frames_burst,
                    person.time_mean_burst, i);
      }
      fclose(archive_front);

      for (size_t i = 0; i < frontiers[0].size(); i++)
      {
          char path_results[64]="";
          sprintf(path_results, "%s-%d.txt", path2, i);

          FILE* out = fopen(path_results, "w+");
          if (!out)
          {
              std::cout << "\n[ERRO:] Ao criar arquivo de resultados" << "\n\n";
              exit(10);
          }
          fprintf(out, "WCRT\t%lf\n",            frontiers[0][i].wcrt);
          fprintf(out, "TIME BURST\t%lf\n",      frontiers[0][i].time_mean_burst);
          fprintf(out, "SIZE BURST\t%lf\n\n",    frontiers[0][i].frames_burst);
          fprintf(out, "ID_MSG\tCYCLE\tDEADLINE\tSTART_DLY\tPAYLOAD\n");
          for (size_t j = 0; j < length_frames; j++)
              fprintf(out,"%u\t%lf\t%lf\t%lf\t%u\n", frontiers[0][i].candb_solution[j].id, frontiers[0][i].candb_solution[j].cycle_time,
                      frontiers[0][i].candb_solution[j].deadline_time, frontiers[0][i].candb_solution[j].delay_time,
                      frontiers[0][i].candb_solution[j].payload_frame);

          fclose(out);
      }
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    FILE* arq  = fopen(argv[1], "r");
    if (!arq)
    {
        std::cerr << "\nERRO AO ABRIR O ARQUIVO\n" << '\n';
        exit(3);
    }

    Frame_CAN* frames = get_CANDB(arq, length_frames);
    fclose(arq);
    first_man.candb_solution = frames;

    ga_length_population   = 120;
    number_funct_objective = 3;
    ga_number_generation   = 310;
    ga_prob_mution         = 0.19;

    NSGA_II();

    write_arq(nodominated_sort(), argv[2], argv[3]);


    return 0;
}
