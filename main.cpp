#include "evaluation.h"
#include "train.h"
#include "forest.h"

#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <vector>


using namespace NGCForest;

void GenerateData(std::vector<TFeatures> &x, std::vector<size_t> &y, size_t count, std::mt19937 &rng) {
    std::uniform_real_distribution<double> noise(-0.1, 0.1), other(0.0, 1.0);
    std::bernoulli_distribution answer(0.3);
    x.resize(count);
    y.resize(count);
    for (size_t i = 0; i < count; ++i) {
        x[i].resize(50);
        if (answer(rng)) {
            y[i] = 1;
            for (size_t j = 0; j < 50; ++j) {
                if (false && j % 25 == 0) {
                    x[i][j] = 0.6 + noise(rng);
                } else if (j == 49) {
                    x[i][j] = x[i][21] + x[i][22];
                } else {
                    x[i][j] = other(rng);
                }
            }
        } else {
            y[i] = 0;
            for (size_t j = 0; j < 50; ++j) {
                if (false && j % 25 == 0) {
                    x[i][j] = 0.5 + noise(rng);
                } else if (j == 49) {
                    x[i][j] = x[i][21] - x[i][22] + 0.1;
                } else {
                    x[i][j] = other(rng);
                }
            }
        }
    }
}

static void ReadPool(std::vector<TFeatures> &x, std::vector<size_t> &y, const std::string &path, double prob, std::mt19937 &rng) {
    std::bernoulli_distribution bern(prob);
    std::ifstream file(path);
    while (!file.eof()) {
            std::string line;
        std::getline(file, line);
        std::istringstream str(line);
        std::string item;
        std::getline(str, item, '\t');
        if (item == "EventId")
            continue;
        if (!bern(rng))
            continue;
        std::getline(str, item, '\t');
        y.push_back(0);
        str >> y.back();
        std::getline(str, item, '\t');
        x.push_back(TFeatures());
        TFeatures &features = x.back();
        while (!str.eof()) {
            features.push_back(0.0);
            str >> features.back();
        }
        if (features.empty()) {
            x.pop_back();
            y.pop_back();
        }
    }
}

int main() {
    std::mt19937 rng;
    std::vector<TFeatures> train_x, test_x;
    std::vector<size_t> train_y, test_y;
    ReadPool(train_x, train_y, "../train.tsv", 0.2, rng);
    std::cout << train_x.size() << std::endl;
    //GenerateData(train_x, train_y, 100000, rng);
    //TCalculatorPtr forest = TrainRandomForest(train_x, train_y, 2, 10, 100);
    //TCalculatorPtr forest = TrainFullRandomForest(train_x, train_y, 2, 10, 100);
    TCalculatorPtr forest = TrainCascadeForest(train_x, train_y, 2, 6, 1000, 10);
    train_x.clear();
    train_y.clear();
    ReadPool(test_x, test_y, "../test.tsv", 0.001, rng);
    //GenerateData(test_x, test_y, 10000, rng);
    for (size_t k = 1; k <= 10; ++k) {
        TCalculatorPtr frst = dynamic_cast<TCascadeForestCalculator*>(forest.get())->GetSlice(k);
        std::vector<std::pair<int, double>> answers(test_x.size());
        for (size_t i = 0; i < test_x.size(); ++i) {
            TFeatures res = frst->Calculate(test_x[i]);
            answers[i] = std::make_pair(test_y[i], res[1]);
            //std::cerr << test_y[i] << "\t" << res[1] << std::endl;
        }
        std::cout << "AUC: " << AUC(std::move(answers)) << std::endl;
    }
    return 0;
}
