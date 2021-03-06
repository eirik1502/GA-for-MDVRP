#include "Validate.h"



Validate::Validate()
{
}


Validate::~Validate()
{
}

void ValidateInstances(Problem p, vector<SolutionInstance> insts) {
	const char* error = "!!!VALIDATION ERROR!!! ";
	for (int i = 0; i < insts.size(); i++) {
		SolutionInstance inst = insts[i];

		int customerCount = 0;

		for (int j = 0; j < inst.vehicleList.size(); j++) {
			Vehicle v = inst.vehicleList[j];

			//check that vehicles have valid 
			if (v.load > v.capacity) {
				cout << error << "vehicle load more than capacity. load: " << v.load << " capacity: " << v.capacity << endl;
			}

			for (int k = 0; k < v.route.size(); k++) {
				Customer cust = v.route[k];
				customerCount++;


			}
		}

		//check that there are right amount of customers
		if (p.customers.size() != customerCount) {
			cout << error << "not right amount of customers. problemCustCount: " << p.customers.size() << " custCount: " << customerCount << endl;
		}
	}
}
