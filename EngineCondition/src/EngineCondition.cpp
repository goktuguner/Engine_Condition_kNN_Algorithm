#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include <cmath>
#include <algorithm>
#include <iostream>
using namespace std;

class Engine{
public:
	vector <double> features;
	int condition;
};

void splitLine(string line, vector <double> & features, int & condition){
	stringstream ss(line);
	string token;
	int ind=0;

	while(getline(ss,token, ',')){
		if(ind <= 5){
			features.push_back(stod(token));
		}
		else{
			condition = stoi(token);
		}
		ind++;
	}
}

void readFile(vector <Engine> & motorlar, string enginefile){
	ifstream okuyucu;
	string line;
	int ind=0;

	okuyucu.open(enginefile);

	if(okuyucu){
		while(getline(okuyucu, line)){
			if(ind > 0){
				Engine motor;
				splitLine(line, motor.features, motor.condition);
				motorlar.push_back(motor);
			}
			ind++;
		}
		okuyucu.close();
	}
	else{
		cout << "Dosya okunamadı: " << enginefile << endl;
	}
}

void splitData(string main_file, string train_file, string test_file, double split_ratio){
	ifstream okuyucu;
	ofstream test_yazici, train_yazici;
	string line;

	okuyucu.open(main_file);
	if(okuyucu){
		train_yazici.open(train_file);
		test_yazici.open(test_file);

		getline(okuyucu,line);
		test_yazici << line << endl;
		train_yazici << line << endl;


		while(getline(okuyucu,line)){
			if((rand()%100) > split_ratio * 100){
				train_yazici << line << endl;
			}
			else{
				test_yazici << line << endl;
			}
		}
		test_yazici.close();
		train_yazici.close();
		okuyucu.close();
	}
	else{
		cout << "Dosya okunamadı: " << main_file << endl;
	}
}


vector <int> get_train_condition(vector <Engine> train_engine){
	vector <int> train_condition;
	for(Engine motor: train_engine){
		train_condition.push_back(motor.condition);
	}
	return train_condition;
}

void find_distances(vector <Engine> test_engines, vector <Engine> train_engines, vector <vector <double>> & distances_vector, vector <double> ratios){
	for(Engine test: test_engines){
		vector <double> distances;
		for(Engine train: train_engines){
			double distance=0;

			for(unsigned int i=0; i<train.features.size(); i++){
				distance += pow(test.features[i]/ratios[i]-train.features[i]/ratios[i],2);
			}
			distances.push_back(sqrt(distance));
		}
		distances_vector.push_back(distances);
	}
}

//test1 in bütün traine olan uzaklıklarını veriyoruz (vec vektörü ile)
//vec vektörünü en yakın k. elemana kadar veriyor
//aynı şekilde o k. elemana kadar olanların kondisyonlarını veriyor
void sortVector(vector<double>& vec, vector<int> condition, vector<int>& klicondition, int k) {
    vector <pair<double, int>> vec_index;

    for(unsigned int i=0; i<vec.size(); i++){
    	vec_index.push_back(make_pair(vec[i], condition[i]));
    }

    sort(vec_index.begin(), vec_index.end());

    vec.clear();
    klicondition.clear();

    for(int i=0; i<k ; i++){
    	vec.push_back(vec_index[i].first);
    	klicondition.push_back(vec_index[i].second);
    }

}

//{1,0,1}

int mostFrequentNumber(vector <int> en_yakinlar){
	int maxCount = 0;
	int mostFrequentNumber = 0;

	for(unsigned int i=0; i<en_yakinlar.size(); i++){

		int count=0;

		for(unsigned int j=0; j<en_yakinlar.size(); j++){
			if(en_yakinlar[i] == en_yakinlar[j]){
				count++;
			}
		}
		if(count > maxCount){
			maxCount = count;
			mostFrequentNumber = en_yakinlar[i];
		}
	}
	return mostFrequentNumber;
}


void en_yakinibul(vector <vector<double>> & en_yakinlar, vector <vector<int>> & en_yakinlarin_conditions, vector <int> train_condition, int k){
	for(unsigned int i=0; i<en_yakinlar.size(); i++){
		vector <int> kaliteleri;
		sortVector(en_yakinlar[i], train_condition, kaliteleri, k);

		en_yakinlarin_conditions.push_back(kaliteleri);
	}

}

void predict_engine(vector <double> & predictions, vector <vector<int>> en_yakinlarin_condition){
	int prediction;
	for(unsigned int i=0; i<en_yakinlarin_condition.size(); i++){
		prediction = mostFrequentNumber(en_yakinlarin_condition[i]);
		predictions.push_back(prediction);
	}
}

void find_maxes(vector <Engine> train_engines, vector <double> & ratios){

	for(unsigned int j = 0; j <= 10; j++) {
			double enb=0;
			for(Engine engine: train_engines){
				if(engine.features[j] > enb){
					enb = engine.features[j];
				}
			}
			ratios.push_back(enb);
		}
}


void kNN(vector <Engine> train_engine, vector <Engine> test_engine, vector <double> & predictions, int k){

	vector <vector <double>> distances_vector;
	vector <int> train_condition = get_train_condition(train_engine);

	vector <double> ratios;

	find_maxes(train_engine, ratios);

	find_distances(test_engine, train_engine, distances_vector, ratios);

	vector <vector<double>> en_yakinlar = distances_vector;
	vector <vector<int>> en_yakinlarin_condition;

	en_yakinibul(en_yakinlar, en_yakinlarin_condition, train_condition, k);

	predict_engine(predictions, en_yakinlarin_condition);


}

void yuzdehesap(vector <int> original, vector <double> predictions){
	double dogru=0, hepsi=0;
	for(unsigned int i=0; i<original.size(); i++){
		if(original[i] == predictions[i]) {
			dogru++;
		}
		hepsi++;
	}

	cout << "%" << dogru/hepsi*100 << endl;
}


int main() {
	string main_file = "engine_data.csv";
	string test_file = "test_file.csv";
	string train_file = "train_file.csv";

	double split_ratio = 0.2;

	vector <Engine> test_engine;
	vector <Engine> train_engine;

	splitData(main_file, train_file, test_file, split_ratio);

	readFile(test_engine, test_file);
	readFile(train_engine, train_file);

	vector <double> predictions;

	kNN(train_engine, test_engine, predictions, 3);

	vector <int> original = get_train_condition(test_engine);

	yuzdehesap(original, predictions);


	return 0;
}
