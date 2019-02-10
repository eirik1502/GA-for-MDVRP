#pragma once
#include "ProblemSolver.h"

void _checkLoad(vector<SolutionInstance> insts) {
	bool nogood = false;
	for (auto &inst : insts) {
		for (auto &v : inst.vehicleList) {
			if (v.load > v.capacity || v.load > v.originDepot.maxVehicleCapacity) {
				cout << "vehicle overload, load: " << v.load << " v capacity: " << v.capacity << " depot capacity: " << v.originDepot.maxVehicleCapacity << endl;
				nogood = true;
			}
		}
	}
	if (!nogood)
		cout << "NO OVERLOAD :)" << endl;
}

void ProblemSolver::SolveMdvrpWithGa(const Problem &problem) {
	this->problem = problem;

	//declare
	vector<SolutionInstance> population;
	vector<SolutionInstance> parents;
	vector<SolutionInstance> children;
	vector<SolutionInstance> mutatedChildren;
	vector<float> evaluations;

	//stats
	vector<SolutionInstance> generationBest;


	//start
	population = InitializePopulation();
	Evaluate(population);

	SolutionInstance bestInitInstance = FindBestInstance(population);
	DrawSolutionInstance(this->problem, bestInitInstance);

	int i = 0;
	while (running) {
		parents = ChooseParents(population);
		_checkLoad(parents);
		children = Crossover(parents);
		_checkLoad(children);
		mutatedChildren = MutateChildren(children);
		_checkLoad(mutatedChildren);



		//evaluate only the newly generated children here
		//Evaluate(children);


		//add mutated children to the population
		population.insert(population.end(), mutatedChildren.begin(), mutatedChildren.end());
		_checkLoad(population);

		Evaluate(population);

		population = SelectNextGeneration(population);
		Evaluate(population);

		cout << "iteration " << i << endl;
		generationBest.push_back(FindBestInstance(population));

		if (i++ >= iterations)
			running = false;

		if (i % 15 == 0) {
			DrawSolutionInstance(this->problem, generationBest.back());
			PlotGenerations(generationBest);
		}
	}

	DrawSolutionInstance(this->problem, generationBest.back());
	PlotGenerations(generationBest);
}

void ProblemSolver::SolveByRandom(const Problem &problem) {
	this->problem = problem;
	vector<SolutionInstance> population;

	SolutionInstance bestInstance(this->problem);
	bestInstance.fitness = 999999999;

	for (int i = 0; i < iterations; i++) {
		population = InitializePopulation();
		Evaluate(population);
		SolutionInstance inst = FindBestInstance(population);
		if (inst.fitness < bestInstance.fitness) {
			bestInstance = inst;
		}

		cout << "iteration " << i << endl;
	}
	DrawSolutionInstance(this->problem, bestInstance);
}