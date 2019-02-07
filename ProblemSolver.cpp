#pragma once
#include "ProblemSolver.h"
#include "solutionInstance.h"
#include <iostream>

vector<SolutionInstance> ProblemSolver::InitializePopulation() {
	vector<SolutionInstance> population;// = vector<SolutionInstance>(this->populationSize);
	population.reserve(populationSize);

	for (unsigned int i = 0; i < populationSize; i++) {
		//SolutionInstance *solinst = GenerateRandomSolution(this->problem->customers);
		SolutionInstance sol(this->problem);
		sol.generateRandomSolution(this->problem);

		population.push_back(sol);
	}

	std::cout << "population should be: " << populationSize << " population is: " << population.size() << std::endl;
	return population;
}

//choose individuals to cross
vector<SolutionInstance> ProblemSolver::ChooseParents(vector<SolutionInstance> population) {
	vector<SolutionInstance> winners = Tournaments(population);
	return winners;
}
vector<SolutionInstance> ProblemSolver::ParentsCrossover(vector<SolutionInstance> parents) {
	return vector<SolutionInstance>();
}
//manipulate the population given the evaluations
vector<SolutionInstance> ProblemSolver::SelectNextGeneration(vector<SolutionInstance> population) {
	vector<SolutionInstance> currentSolutions;

	currentSolutions.reserve(populationSize);

	//put populationSize number of instances in our currentVectors
	for (unsigned int i = 0; i < populationSize; i++) {
		currentSolutions.push_back(population[i]);
	}

	//of the remaining, replace the better ones with the existing bad ones in currentVector
	for (unsigned int i = populationSize; i < population.size(); i++) {
		for (unsigned int j = 0; j < currentSolutions.size(); j++) {
			if (population[i].fitness < currentSolutions[j].fitness) {
				currentSolutions[j] = population[i];
				break;
			}
		}
	}

	return currentSolutions;
}


vector<SolutionInstance> ProblemSolver::Crossover(vector<SolutionInstance> copys) {
	int i;
	for (i = 0; i < copys.size(); i++) {
		copys[i] = IndividualCrossover(copys[i]);
	}
	return copys;
}

SolutionInstance ProblemSolver::IndividualCrossover(SolutionInstance instance) {
	int i, randomVehicleNumber, N;
	float randomScore;
	vector<SolutionInstance> mutatedChildren;
	
	for (i = 0; i < instance.vehicleList.size(); i++) {
		randomScore = float((rand() % 100)) / float(100);
		std::cout << randomScore << std::endl; //PRINT! REMOVE AFTER
		if (randomScore > (1 - this->mutationProbability)) {
			randomVehicleNumber = rand() % instance.vehicleList.size();
			if (instance.vehicleList[i].route.size() < instance.vehicleList[randomVehicleNumber].route.size()) {
				N = rand() % instance.vehicleList[i].route.size();
			}
			else {
				N = rand() % instance.vehicleList[randomVehicleNumber].route.size();
			}
			swapRouteSectionsAtIndexN(instance.vehicleList[i].route, instance.vehicleList[randomVehicleNumber].route, N);
		}
	}
	return instance;
}

void ProblemSolver::swapRouteSectionsAtIndexN(vector<Customer>& route1, vector<Customer>& route2, int N){
	int i;
	vector<Customer> swapRouteSection1, swapRouteSection2;
	for (i = N; i < route1.size(); i++) {
		swapRouteSection1.push_back(route1[i]);
	}
	for (i = N; i < route2.size(); i++) {
		swapRouteSection2.push_back(route2[i]);
	}
	route1.erase(route1.begin() + N, route1.end());
	route2.erase(route2.begin() + N, route2.end());
	for (i = 0; i < swapRouteSection2.size(); i++) {
		route1.push_back(swapRouteSection2[i]);
	}
	for (i = 0; i < swapRouteSection1.size(); i++) {
		route2.push_back(swapRouteSection1[i]);
	}
}

vector<SolutionInstance> ProblemSolver::MutateChildren(vector<SolutionInstance> children) {
	int i;
	float randomScore;
	vector<SolutionInstance> mutatedChildren;
	
	for (i = 0; i < children.size(); i++) {
		mutatedChildren.push_back(MutateChild(children[i]));
	}
	return mutatedChildren;
}

SolutionInstance ProblemSolver::MutateChild(SolutionInstance solutionInstance) {
	int i, j, randomCustomerNumber, randomVehicleNumber;
	float randomScore;
	Customer tempCustomer;
	for (i = 0; i < solutionInstance.vehicleList.size(); i++) {
		for (j = 0; j < solutionInstance.vehicleList[i].route.size(); j++) {
			randomScore = float((rand() % 100)) / float(100);
			if (randomScore > (1 - this->mutationProbability)) {
				cout << "mutation happening" << endl;

				vector<Customer> route;
				do  {
					randomVehicleNumber = rand() % solutionInstance.vehicleList.size();
					route = solutionInstance.vehicleList[randomVehicleNumber].route;
				} while (route.size() == 0);

				randomCustomerNumber = rand() % route.size(); //integer division by zero, happened here
				tempCustomer = solutionInstance.vehicleList[i].route[j];
				solutionInstance.vehicleList[i].route[j] = solutionInstance.vehicleList[randomVehicleNumber].route[randomCustomerNumber];
				solutionInstance.vehicleList[randomVehicleNumber].route[randomCustomerNumber] = tempCustomer;
			}
		}
	}
	return solutionInstance;
}

vector<SolutionInstance> ProblemSolver::Evaluate(vector<SolutionInstance> population) {
	int i;
	int solutionFitness;
	for (i = 0; i < population.size(); i++) {
		solutionFitness = this->CalculateFitness(population[i]);
		population[i].fitness = solutionFitness;
	}
	return population;
}


float ProblemSolver::CalculateFitness(SolutionInstance& solutionInstance) {
	int i;
	float fitness;
	fitness = 0;
	for (i = 0; i < solutionInstance.vehicleList.size(); i++) {
		fitness += solutionInstance.vehicleList[i].routeRange;
	}
	return fitness;
}

vector<SolutionInstance> ProblemSolver::Tournaments(vector<SolutionInstance> population) {
	int i, j, numWinners;
	int tournamentLeaderScore;
	vector<SolutionInstance> winners;

	if (population.size() % tournamentSize > 0) {
		numWinners = (population.size() / tournamentSize) + 1;
	}
	else {
		numWinners = (population.size() / tournamentSize);
	}
	//vector<SolutionInstance> winners(tournamentSize);
	for (i = 0; i < numWinners; i++) {
		tournamentLeaderScore = 9999999999;
		for (j = 0; j < tournamentSize; j++) {
			if (population[(i*numWinners) + j].fitness < tournamentLeaderScore) {
				winners.push_back(population[(i*numWinners) + j]);
			}
		}
	}
	return winners;
}


vector<SolutionInstance> ProblemSolver::Replicate(vector<SolutionInstance> winners, int populationSize) {
	int i, j, copyPerParent;
	vector<SolutionInstance> winnerCopys;
	copyPerParent = tournamentSize;
	for (i = 0; i < winners.size(); i++) {
		for (j = 0; j < copyPerParent; j++) {
			winnerCopys.push_back(winners[i]);
			if (winnerCopys.size() >= populationSize) {
				break;
			}
		}
	}
	return winnerCopys;
}

/*SolutionInstance ProblemSolver::GenerateRandomSolution(Problem problem) {
	SolutionInstance solutionInstance(problem);
	std::vector<Customer> customers;
	customers = problem.customers;

	bool vehicleAvailable;
	int i, j, customerCount, vehicleCount, randomVehicleNumber, randomDepotNumber;
	customerCount = customers.size();
	vehicleCount = solutionInstance.vehicleList.size();

	for (i = 0; i < customerCount; i++) {

		randomVehicleNumber = rand() % vehicleCount;
		vehicleAvailable = solutionInstance.vehicleList[randomVehicleNumber].vehicleAvailable(customers[i]);

		if (vehicleAvailable) {
			solutionInstance.vehicleList[randomVehicleNumber].addCustomer2VehicleRoute(customers[i]);
		}
		else {
			i--;
		}
	}

	for (j = 0; j < solutionInstance.vehicleList.size(); j++) {
		randomDepotNumber = rand() % problem.depots.size();
		solutionInstance.vehicleList[j].endDepot = problem.depots[randomDepotNumber];
	}

}
*/
