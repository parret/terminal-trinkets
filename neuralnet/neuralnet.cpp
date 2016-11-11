// neuralnet.cpp

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;

struct Connection{
	double weight;
	double deltaWeight;
};

class Neuron;

typedef vector<Neuron> Layer;



///////////// class Neuron //////////

class Neuron {
public:
	Neuron(unsigned numOutputs, unsigned myIndex);
	void setOutputVal(double val) { g_outputVal = val; }
	double getOutputVal() const {return g_outputVal; }
	void feedForward(const Layer &prevLayer);
	void calcOutputGradients(double targetVal);
	void calcHiddenGradients(const Layer &nextLayer);
	void updateInputWeights(Layer &prevLayer);

private:
	static double eta; // [0.0..1.0] overall train weight
	static double alpha; // [0.0..n] momentum (multiplier of last rain weight)
	static double transferFunction(double x);
	static double transferFunctionDerivative(double x);
	static double randomWeight() { return rand() / double(RAND_MAX); }
	double sumDOW(const Layer &nextLayer) const;
	double g_outputVal;
	vector<Connection> g_outputWeights;
	unsigned g_myIndex;
	double g_gradient;

};

double Neuron::eta = 0.2; // train rate [0.0..1.0]
double Neuron::alpha = 0.5; // momentum [0.0..n]

void Neuron::updateInputWeights(Layer &prevLayer){
	// weights to be updated are in Connection container in prev layer

	for(unsigned n = 0; n < prevLayer.size(); ++n){
		Neuron &neuron = prevLayer[n];
		double oldDeltaWeight = neuron.g_outputWeights[g_myIndex].deltaWeight;

		double newDeltaWeight = 
				// individual input, magnified by the gradient and train rate
				eta
				* neuron.getOutputVal()
				* g_gradient
				// momentum rate = fraction of prev delta weight
				+ alpha
				* oldDeltaWeight;

		neuron.g_outputWeights[g_myIndex].deltaWeight = newDeltaWeight;
		neuron.g_outputWeights[g_myIndex].weight += newDeltaWeight;
	}
}

double Neuron::sumDOW(const Layer &nextLayer) const{
	double sum = 0.0;

	// sum contributions of errors at nodes fed
	for(unsigned n = 0; n < nextLayer.size() - 1; ++n){
		sum += g_outputWeights[n].weight * nextLayer[n].g_gradient;
	}

	return sum;
}

void Neuron::calcHiddenGradients(const Layer &nextLayer){
	// derivative of weights (dow)
	double dow =  sumDOW(nextLayer);
	g_gradient = dow * Neuron::transferFunctionDerivative(g_outputVal);
}

void Neuron::calcOutputGradients(double targetVal){
	double delta = targetVal - g_outputVal;
	g_gradient = delta * Neuron::transferFunctionDerivative(g_outputVal);
}

double Neuron::transferFunction(double x){
	// tanh - output range [-1.0..1.0]

	return tanh(x);
}

double Neuron::transferFunctionDerivative(double x){
	//very quick approximation of derivative of tanh function

	return 1 - x * x;
}

void Neuron::feedForward(const Layer &prevLayer){
	double sum = 0.0;

	// sum the previous layer outputs (as our inputs)
	// include bias node

	for (unsigned n = 0; n < prevLayer.size(); ++n){
		sum += prevLayer[n].getOutputVal() *
			prevLayer[n].g_outputWeights[g_myIndex].weight;
	}

	g_outputVal = Neuron::transferFunction(sum);

}


Neuron::Neuron(unsigned numOutputs, unsigned myIndex){
	for(unsigned c = 0; c < numOutputs; ++c){
		g_outputWeights.push_back(Connection());
		g_outputWeights.back().weight = randomWeight();
	}

	g_myIndex = myIndex;
}



///////////// class Net //////////

class Net {
public:
	Net(const vector<unsigned> &topology);
	void feedForward(const vector<double> &inputVals);
	void backProp(const vector<double> &targetVals);
	void getResults(vector<double> &resultVals) const;

private:
	vector<Layer> g_layers; // m_layers[layerNum][neuronNum]
	double g_error;
	double g_recentAverageError;
	static double g_recentAverageSmoothingFactor;
};

double Net::g_recentAverageSmoothingFactor = 100.0; // Number of training samples to average over

void Net::getResults(vector<double> &resultVals) const {
	resultVals.clear();

	for(unsigned n = 0; n < g_layers.back().size() - 1; ++n){
		resultVals.push_back(g_layers.back()[n].getOutputVal());
	}
}

void Net::feedForward(const vector<double> &inputVals){
	// check that size of input values is the same as the size of input neurons
	assert(inputVals.size() == g_layers[0].size() - 1);

	// latch the input values into the input neurons
	for(unsigned i = 0; i < inputVals.size(); ++i){
		g_layers[0][i].setOutputVal(inputVals[i]);
	}

	// forward propogation (skip input layer and start with first hidden layer)
	for(unsigned layerNum = 1; layerNum < g_layers.size(); ++layerNum){
		Layer &prevLayer = g_layers[layerNum - 1];
		for(unsigned n = 0; n < g_layers[layerNum].size() - 1; ++n){
			g_layers[layerNum][n].feedForward(prevLayer);
		}
	}


}

void Net::backProp(const vector<double> &targetVals){
	// calculate overall net error (root mean square (RMS) of output neron errors)
	Layer &outputLayer = g_layers.back();
	g_error = 0.0;

	for(unsigned n = 0; n < outputLayer.size() - 1; ++n){
		double delta = targetVals[n] - outputLayer[n].getOutputVal();
		g_error += delta * delta;
	}
	g_error /= outputLayer.size() - 1; // average error squared
	g_error = sqrt(g_error);

	// recent average measurement

	g_recentAverageError = 
		(g_recentAverageError * g_recentAverageSmoothingFactor + g_error)
		/ (g_recentAverageSmoothingFactor + 1.0);

	// calculate output layer gradients
	for(unsigned n = 0; n < outputLayer.size() - 1; ++n){
		outputLayer[n].calcOutputGradients(targetVals[n]);
	}

	// calculate gradients on hidden layers
	for(unsigned layerNum = g_layers.size() - 2; layerNum > 0; --layerNum){
		Layer &hiddenLayer = g_layers[layerNum];
		Layer &nextLayer = g_layers[layerNum + 1];

		for(unsigned n = 0; n < hiddenLayer.size(); ++n){
			hiddenLayer[n].calcHiddenGradients(nextLayer);
		}
	}

	// update connection weights on outputs to first hidden layer
	for(unsigned layerNum = (g_layers.size() - 1); layerNum > 0; --layerNum){
		Layer &currentlayer = g_layers[layerNum];
		Layer &prevLayer = g_layers[layerNum - 1];

		for(unsigned n = 0; n < currentlayer.size() - 1; ++n){
			currentlayer[n].updateInputWeights(prevLayer);
		}
	}
}



Net::Net(const vector<unsigned> &topology){
	unsigned numLayers = topology.size();

	for(unsigned layerNum = 0; layerNum < numLayers; ++layerNum){
		g_layers.push_back(Layer());
		unsigned numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1];

		// populate layers with neurons
		for(unsigned neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum){
			g_layers.back().push_back(Neuron(numOutputs, neuronNum));
			cout << "Made a Neuron!" << endl;
		}
	}
}



int main() {
	// eg, {3, 2, 1}
	vector<unsigned> topology;
	topology.push_back(2);
	topology.push_back(3);
	topology.push_back(1);
	Net myNet(topology);

	string s_input = "";
	double d_input = 0;
	unsigned right = 0;
	unsigned wrong = 0;
	
	vector<double> resultVals;
	vector<double> inputVals;
	vector<double> targetVals;

	while(true){
		resultVals.clear();
		inputVals.clear();
		targetVals.clear();

		cout << "Tell me something. " << endl << "> ";
		getline(cin, s_input);
		if(s_input == "score"){
			if(right > wrong){
				cout << "I'm currently winning " << right << " to " << wrong << "!" << endl;
			} else if(right < wrong){
				cout << "I'm currently down " << right << " to " << wrong << "... I just need a little bit more time!" << endl;
			} else {
				cout << "We're tied at " << right << " each. It's getting tense!" << endl;
			}
			continue;
		} else if (s_input == "train") {
			cout << "Ok." << endl;
			cout << "Learning..." << endl;
			std::ifstream file("training");
			unsigned count = 0; 
			unsigned randomCount = (rand() % 1000000) + 2000000;

			while(getline(file, s_input)){
				inputVals.clear();
				targetVals.clear();
				resultVals.clear();
				inputVals.push_back(stod(s_input));
				getline(file, s_input);
				inputVals.push_back(stod(s_input));
				myNet.feedForward(inputVals);
				myNet.getResults(resultVals);
				getline(file, s_input);
				targetVals.push_back(stod(s_input));
				myNet.backProp(targetVals);
				count++;
				if (count % randomCount == 0){
					cout << "I'm at " << count << "." << endl;
					randomCount += (rand() % 1000000) + 2000000;
				}
				if (count % 8765432 == 0){
					cout << "This is really boring... (ᴗ˳ᴗ)" << endl;
				}
			}
			cout << "I'm all finished!" << endl;
			continue;
		}
		try{
			inputVals.push_back(stod(s_input));
			getline(cin, s_input);
			inputVals.push_back(stod(s_input));
		} catch (const std::invalid_argument& i) {
			cout << "umm..." << flush;
			sleep(1);
			cout << " no. " << flush;
			sleep(1);
			cout << endl;
			continue;
		}

		myNet.feedForward(inputVals);
		myNet.getResults(resultVals);
		string answer = "";
		if(round(resultVals[0]) == 1.0 ){
			answer = "true.";
		} else {
			answer = "false.";
		}

		cout << "I think I should say " << answer
				<< " (" << resultVals[0] << ")" << endl;
		cout << "What should I have said?" << endl << "> ";
		getline(cin, s_input);
		d_input = stod(s_input);
		if(d_input == round(resultVals[0])){
			cout << "I win!" << endl;
			right++;
		} else {
			cout << "Crap... I'll do better next time." << endl;
			wrong++;
		}
		targetVals.push_back(d_input);
		myNet.backProp(targetVals);
	}
}