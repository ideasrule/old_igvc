#include "trainData.h"
#include <fstream>
#include <vector>
#include "svm.h"

using namespace std;

bool SVM::train(char* dataFileName, char *resultFileName)
{
	//open the data file
	ifstream dataFile(dataFileName, ios::in);

	//Load training data into an svm_problem
	vector<double> labels;
	vector<vector<struct svm_node>> svmNodesTemp;

	bool atNewLine = true;
	int index = 1;
	if (dataFile.is_open()) {
		while (dataFile.good()) {
			if (atNewLine) {
				double label;
				dataFile >> label;
				labels.push_back(label);
				if (svmNodesTemp.size()>0) {
					//teminate with the terminator node
					struct svm_node termNode;
					termNode.index = -1;
					svmNodesTemp[svmNodesTemp.size()-1].push_back(termNode);
				}
				svmNodesTemp.push_back(vector<struct svm_node>());
				atNewLine = false;
				index = 1;
				continue;
			} 
			char nextChar = dataFile.get();
			if (nextChar == '\t') {
				continue;
			} else if (nextChar == '\n') {
				atNewLine = true;
				continue;
			} else {
				dataFile.putback(nextChar);
			}

			double value;
			dataFile >> value;
			struct svm_node node;
			node.index = index++;
			node.value = value;
			svmNodesTemp[svmNodesTemp.size()-1].push_back(node);
		}
		struct svm_node termNode;
		termNode.index = -1;
		svmNodesTemp[svmNodesTemp.size()-1].push_back(termNode);
		dataFile.close();
	} else {
		return false;
	}

	if (svmNodesTemp.size() == 0)
		return false;

	if (svmNodesTemp[svmNodesTemp.size()-1].size() == 1) {  //only contains the empty node because of an extra newline in the data file
		svmNodesTemp.erase(svmNodesTemp.end() - 1);
		labels.erase(labels.end() - 1);
	}

	struct svm_node **svmNodes = new struct svm_node *[svmNodesTemp.size()];
	for (size_t i = 0; i < svmNodesTemp.size(); ++i) {
		if (svmNodesTemp[i].size() > 0)
			svmNodes[i] = &svmNodesTemp[i][0];
	}

	struct svm_problem problem;
	problem.l = svmNodesTemp.size();
	problem.y = &labels[0];
	problem.x = svmNodes;

	struct svm_parameter parameter;
	parameter.svm_type = C_SVC;
	parameter.kernel_type = RBF;
	parameter.gamma = 0.00005;

	parameter.cache_size = 1000;   //1000 MB
	parameter.eps = 0.001;
	parameter.C = 1000.0;
	parameter.nr_weight = 0;
	parameter.shrinking = 0;
	parameter.probability = 0;


	const char *svmValid = svm_check_parameter(&problem, &parameter);   //returns NULL if ok
	if (svmValid != NULL)
		return false;
	const struct svm_model *result = svm_train(&problem, &parameter);

	svm_save_model(resultFileName, result);

	delete svmNodes;
	return true;
}