#pragma once

#include "ProblemStructures.h"
#include <vector>

class Vehicle {
public:
	int id;
	int load;
	int capacity;
	Depot endDepot;
	Depot originDepot;
	float routeRange;
	float maxRouteRange;
	std::vector<Customer> route;

	Vehicle(int id, int capacity, Depot originDepot);
	
	bool vehicleAvailable(Customer customer);
	float calculateDistance2Customer(Customer customer);
	void addCustomer2VehicleRoute(Customer customer);
	void RecalculateRouteDistance();

	~Vehicle();
};

class SolutionInstance {
public:
	std::vector<Vehicle> vehicleList;
	float fitness;

	SolutionInstance(Problem problem); //Generate structure, without solution
	void generateRandomSolution(Problem problem);
	void GenerateInitialSolution(Problem problem);
	void GenerateInitialSolution2(Problem problem);
	void GenerateInitialSolution3(Problem problem);
};


float distanceBetweenCoordinates(int x1, int y1, int x2, int y2);
float distanceBetween(Location &loc1, Location &loc2);

void testRand();
