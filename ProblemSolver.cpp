#pragma once
#include "ProblemSolver.h"
#include "solutionInstance.h"
#include "SimpleCrossovers.h"
#include <iostream>
#include "SimpleCrossovers.h"
#include "SwapStartMutation.h"
#include "Validate.h"

vector<SolutionInstance> ProblemSolver::InitializePopulation() {
	vector<SolutionInstance> population;// = vector<SolutionInstance>(this->populationSize);
	population.reserve(populationSize);

	for (unsigned int i = 0; i < populationSize; i++) {
		//std::cout << "New Instance" << std::endl;
		//SolutionInstance *solinst = GenerateRandomSolution(this->problem->customers);
		SolutionInstance sol(this->problem);

		//sol.GenerateInitialSolution(this->problem);
		//sol.GenerateInitialSolution2(this->problem);
		sol.GenerateInitialSolution3(this->problem);
		//sol.generateRandomSolution(this->problem);

		population.push_back(sol);
	}

	//std::cout << "population should be: " << populationSize << " population is: " << population.size() << std::endl;
	return population;
}

//choose individuals to cross
vector<SolutionInstance> ProblemSolver::ChooseParents(vector<SolutionInstance> population) {
	vector<SolutionInstance> winners = Tournaments(population);
	vector<SolutionInstance> replicatedWinners = Replicate(winners, populationSize);
	return replicatedWinners;
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


vector<SolutionInstance> ProblemSolver::Crossover(vector<SolutionInstance> parents) {

	//best cost route crossover
	vector<SolutionInstance> children;
	children.reserve(parents.size());
	
	while (parents.size() > 1) {

		//find a parent1 and remove it from parents
		int par1Ind = rand() % parents.size();
		SolutionInstance par1 = parents[par1Ind];
		parents.erase(parents.begin() + par1Ind);

		//find a parent 2 and remove it
		int par2Ind = rand() % parents.size();
		SolutionInstance par2 = parents[par2Ind];
		parents.erase(parents.begin() + par2Ind);

		//modifies inplace
		BestCostRouteCrossover(par1, par2);

		children.push_back(par1);
		children.push_back(par2);
	}

	//try swap
	//SwapStartDepotCrossover(children);

	return children;
}

SolutionInstance ProblemSolver::IndividualCrossover(SolutionInstance instance) {
	int i, randomVehicleNumber, N;
	float randomScore;
	vector<SolutionInstance> mutatedChildren;
	Depot tempDepot;
	
	for (i = 0; i < instance.vehicleList.size(); i++) {
		randomScore = float(rand()) / float(RAND_MAX);
		if ((randomScore > (1.0 - this->crossoverProbability)) && instance.vehicleList[i].route.size() != 0) {
			do {
				randomVehicleNumber = rand() % instance.vehicleList.size();
			} while (instance.vehicleList[randomVehicleNumber].route.size() == 0);
			if (instance.vehicleList[i].route.size() < instance.vehicleList[randomVehicleNumber].route.size()) {
				N = rand() % instance.vehicleList[i].route.size();
			}
			else {
					N = rand() % instance.vehicleList[randomVehicleNumber].route.size();
			}
			swapRouteSectionsAtIndexN(instance.vehicleList[i].route, instance.vehicleList[randomVehicleNumber].route, N);
			tempDepot = instance.vehicleList[i].endDepot;
			instance.vehicleList[i].endDepot = instance.vehicleList[randomVehicleNumber].endDepot;
			instance.vehicleList[randomVehicleNumber].endDepot = tempDepot;
			//Recalculate fittness
			instance.vehicleList[i].RecalculateRouteDistance();
			instance.vehicleList[randomVehicleNumber].RecalculateRouteDistance();
		}
	}
	return instance;
}

void ProblemSolver::swapRouteSectionsAtIndexN(vector<Customer>& route1, vector<Customer>& route2, int N){
	int i;
	vector<Customer> swapRouteSection1, swapRouteSection2;
	/*for (i = 0; i < route1.size(); i++) {
		std::cout << route1[i].customerNumber << "   ";
	}
	std::cout << std::endl;*/
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
	/*for (i = 0; i < route1.size(); i++) {
		std::cout << route1[i].customerNumber << "   ";
	}
	std::cout << std::endl;*/
}

//return true with the probability given. Probability in the range [0, 1]
bool testProb(float probability) {
	float r = float(rand()) / float(RAND_MAX);
	return r <= probability;
}
vector<SolutionInstance> ProblemSolver::MutateChildren(vector<SolutionInstance> children) {

	vector<SolutionInstance> mutatedChildren;
	
	for (int instInd = 0; instInd < children.size(); instInd++) {
		SolutionInstance inst = children[instInd];

		//MUTATION BASED ON THE WHOLE INSTANCE

		//swap origin depot mutation
		if (testProb(this->swapStartDepotMutationProb)) {
			//perform mutation
			inst = SwapStartDepotMutation(inst);
			this->swapStartDepotMutations++;
		}

		for (int vInd = 0; vInd < inst.vehicleList.size(); vInd++) {
			//MUTATION BASED ON VEHICLE

			//Adds abillity to mutate endDepot as well, randomly choose a new depot.
			if (testProb(this->endDepotMutationProbability)) {
				inst = ChangeEndDepotMutation(inst, vInd);
				this->changeEndDepotMutations++;
			}

			for (int custInd = 0; custInd < inst.vehicleList[vInd].route.size(); custInd++) {
				//MUTATION BASED ON CUSTOMER

				if (testProb(this->idealMutationProbability)) {
					//cout << "----Ideal mutation" << endl;
					inst = IdealReroutingMutation(inst, vInd, custInd);
					this->idealCustChangeMutations++;
					//printSolutionRoute(inst);
				}
			}
		}

		mutatedChildren.push_back(inst);

		//mutatedChildren.push_back(MutateChild(children[i]));
		
		/*
		//have an initial chance for mutation at all
		float r = float(rand()) / float(RAND_MAX);
		
		if (r < mutationProbability * 8) {
			mutatedChildren.push_back(MutateChild(children[i]));
		}
		else {
			mutatedChildren.push_back(children[i]);
		}
		*/
	}

	return mutatedChildren;
	
}

SolutionInstance ProblemSolver::ChangeEndDepotMutation(SolutionInstance inst, int vehicleInd) {
	int randDepInd = rand() % problem.depots.size();
	inst.vehicleList[vehicleInd].endDepot = problem.depots[randDepInd];
	return inst;
}

SolutionInstance ProblemSolver::IdealReroutingMutation(SolutionInstance instance, int vehicleNumber, int customerNumber) {
	Customer insertCust = instance.vehicleList[vehicleNumber].route[customerNumber];
	SolutionInstance newInstance = instance;

	RemovedCust remCust = _removeCustomerFromInstance(insertCust, newInstance);
	InsertEval eval = _findBestInsertionInAll(insertCust, newInstance);
	bool inserted = _doInsert(eval);
	if (!inserted) {
		_reinsertRemovedCust(remCust);
		//throw invalid_argument("customer was not inserted");
	}
	return newInstance;
}

//not used
SolutionInstance ProblemSolver::MutateChild(SolutionInstance solutionInstance) {
	int i, j, randomCustomerNumber, randomVehicleNumber, randomDepotNumber;
	float randomScore;
	Customer tempCustomer;
	for (i = 0; i < solutionInstance.vehicleList.size(); i++) {
		for (j = 0; j < solutionInstance.vehicleList[i].route.size(); j++) {
			randomScore = float(rand()) / float(RAND_MAX);
			if (randomScore > (1 - this->mutationProbability)) {
				//cout << "mutation happening" << endl;

				//make sure that the route used is not of size 0
				vector<Customer> route;
				do  {
					randomVehicleNumber = rand() % solutionInstance.vehicleList.size();
					route = solutionInstance.vehicleList[randomVehicleNumber].route;
				} while (route.size() == 0);

				randomCustomerNumber = rand() % route.size(); //integer division by zero, happened here
				
				if (routeMutationValid(solutionInstance.vehicleList[i], solutionInstance.vehicleList[i].route[j], solutionInstance.vehicleList[randomVehicleNumber].route[randomCustomerNumber])
					&& routeMutationValid(solutionInstance.vehicleList[randomVehicleNumber], solutionInstance.vehicleList[randomVehicleNumber].route[randomCustomerNumber],  solutionInstance.vehicleList[i].route[j])) {
					//perform mutation: switch a customer between two vehicles
					tempCustomer = solutionInstance.vehicleList[i].route[j];
					solutionInstance.vehicleList[i].route[j] = solutionInstance.vehicleList[randomVehicleNumber].route[randomCustomerNumber];
					solutionInstance.vehicleList[randomVehicleNumber].route[randomCustomerNumber] = tempCustomer;

					//recalculate vehicle paths after mutation
					solutionInstance.vehicleList[i].RecalculateRouteDistance();
					solutionInstance.vehicleList[randomVehicleNumber].RecalculateRouteDistance();

					//recalculate loads after mutation
					solutionInstance.vehicleList[i].load = loadAfterMutation(solutionInstance.vehicleList[i], solutionInstance.vehicleList[i].route[j], solutionInstance.vehicleList[randomVehicleNumber].route[randomCustomerNumber]);
					solutionInstance.vehicleList[randomVehicleNumber].load = loadAfterMutation(solutionInstance.vehicleList[randomVehicleNumber], solutionInstance.vehicleList[randomVehicleNumber].route[randomCustomerNumber], solutionInstance.vehicleList[i].route[j]);
				}
			}
			randomScore = float(rand()) / float(RAND_MAX);
			if (randomScore > (1 - this->idealMutationProbability)) {
				solutionInstance = IdealReroutingMutation(solutionInstance, i, j);
			}
		}
		//Adds abillity to mutate endDepot as well, randomly choose a new depot.
		randomScore = float(rand()) / float(RAND_MAX);
		if (randomScore > (1 - this->endDepotMutationProbability)) {
			randomDepotNumber = rand() % problem.depots.size();
			solutionInstance.vehicleList[i].endDepot = problem.depots[randomDepotNumber];
		}
	}
	return solutionInstance;
}

void ProblemSolver::Evaluate(vector<SolutionInstance> &population) {
	int i;
	int solutionFitness;
	for (i = 0; i < population.size(); i++) {


		solutionFitness = this->CalculateFitness(population[i]);
		population[i].fitness = solutionFitness;
	}
}

float ProblemSolver::CalculateFitness(SolutionInstance& solutionInstance) {
	int i;
	float fitness = 0;
	for (i = 0; i < solutionInstance.vehicleList.size(); i++) {
		Vehicle* v = &solutionInstance.vehicleList[i];
		//first update routeRange by calculating route distances
		v->RecalculateRouteDistance();

		fitness += v->routeRange;
	}
	return fitness;
}

vector<SolutionInstance> ProblemSolver::Tournaments(vector<SolutionInstance> population) {
	//BUG: tournament size seems to have to be half of the population size
	int i, j, groupCount;
	int tournamentLeaderScore, randomWinnerNumber;
	float randomScore;
	vector<SolutionInstance> winners;

	if (population.size() % tournamentSize > 0) {
		throw domain_error("population size should be a mulitple of tournamant size");
		groupCount = (population.size() / tournamentSize) + 1;
	}
	else {
		groupCount = (population.size() / tournamentSize);
	}

	for (i = 0; i < groupCount; i++) {
		tournamentLeaderScore = 9999999999;
		SolutionInstance *currentWinner = nullptr;

		for (j = 0; j < tournamentSize; j++) {
			int individualId = (i*tournamentSize) + j;
			SolutionInstance *currentInstance = &population[individualId];
			
			if (currentInstance->fitness < tournamentLeaderScore) {
				tournamentLeaderScore = currentInstance->fitness;
				currentWinner = currentInstance;
			}
		}
		randomScore = float(rand()) / float(RAND_MAX);
		if (randomScore > (1 - this->randomTournamentWinnerProbability)) {
			randomWinnerNumber = rand() % tournamentSize;
			currentWinner = &population[(i*tournamentSize) + randomWinnerNumber];
		}
		winners.push_back(*currentWinner);
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

int loadAfterMutation(Vehicle vehicle, Customer currentCustomer, Customer newCustomer) {
	int updatedLoad;
	updatedLoad = vehicle.load - currentCustomer.demand + newCustomer.demand;
	return updatedLoad;
}

bool routeMutationValid(Vehicle vehicle, Customer currentCustomer, Customer newCustomer) {
	bool mutationValid;
	int updatedLoad;
	mutationValid = false;
	updatedLoad = loadAfterMutation(vehicle, currentCustomer, newCustomer);
	if (updatedLoad <= vehicle.capacity) {
		mutationValid = true;
	}
	return mutationValid;
}

SolutionInstance ProblemSolver::FindBestInstance(vector<SolutionInstance> instances) {
	if (instances.size() == 0) throw invalid_argument("no instencesin vector");

	float lowestFitness = instances[0].fitness;
	int lowestFitnessIndex = 0;

	for (int i = 1; i < instances.size(); i++) {
		if (instances[i].fitness < lowestFitness) {
			lowestFitness = instances[i].fitness;
			lowestFitnessIndex = i;
		}
	}

	return instances[lowestFitnessIndex];
}
void ProblemSolver::DrawSolutions(vector<SolutionInstance> solutions) {
	for (int i = 0; i < solutions.size(); i++) {
		DrawSolutionInstance(this->problem, solutions[i]);
	}
}



//Icludes solution for weighting the parents based off of 
/*vector<SolutionInstance> ProblemSolver::Replicate(vector<SolutionInstance> winners, int populationSize) {
	int i, j, totalFitness;
	vector<int> copyPerParent;
	vector<SolutionInstance> winnerCopys;

	//calculating total fitness
	totalFitness = 0;
	for (i = 0; i < winners.size(); i++) {
		totalFitness = totalFitness + winners[i].fitness;
	}

	for (i = 0; i < winners.size(); i++) {
		std::cout << round(populationSize*(float(winners[i].fitness) / float(totalFitness))) << std::endl;
		copyPerParent.push_back(round(populationSize*(float(winners[i].fitness) / float(totalFitness))));
		//copyPerParent.push_back(tournamentSize);
	}

	for (i = 0; i < winners.size(); i++) {
		for (j = 0; j < copyPerParent[i]; j++) {
			winnerCopys.push_back(winners[i]);
			if (winnerCopys.size() >= populationSize) {
				break;
			}
		}
	}
	return winnerCopys;
}*/