#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <string>
#include <cmath>
#include <vector>
#include <iterator>
#include "Eigen/Dense"
#include "Eigen/SVD"
#include <omp.h>
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <functional>
#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>

std::vector<std::vector<double>> generatePricePathMatrix(
    int P, double So, double dt, int N, double r, double v, unsigned int seed
) {
    std::vector<std::vector<double>> paths(P, std::vector<double>(N));
 
    double drift = (r - 0.5 * v * v) * dt;
    double vol = v * std::sqrt(dt);
 
    std::mt19937 gen(seed);
    std::normal_distribution<double> d(0.0, 1.0);
 
    for (int p = 0; p < P; ++p) {
        double last = So;
        for (int n = 0; n < N; ++n) {
            double z = d(gen);
            last *= std::exp(drift + vol * z);
            paths[p][n] = last;
        }
    }
 
    return paths;
}

/**
 * Write a vector of vectors of doubles to a CSV file
 * @param filename Path to output CSV file
 * @param data Vector of vectors containing the data
 * @param precision Number of decimal places (default: 6)
 * @return true if successful, false otherwise
 */
bool writeVectorToCSV(const std::string& filename, 
                      const std::vector<std::vector<double>>& data,
                      int precision = 6) {
    std::ofstream file(filename);
    
    // Check if file opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }
    
    // Set precision for floating point numbers
    file << std::fixed << std::setprecision(precision);
    
    // Write each row
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            // Add comma after each element except the last
            if (i < row.size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }
    
    file.close();
    return true;
}

/**
 * Read a CSV file back into a vector of vectors of doubles
 * @param filename Path to CSV file to read
 * @return Vector of vectors containing the data
 */
std::vector<std::vector<double>> readVectorFromCSV(const std::string& filename) {
    std::vector<std::vector<double>> data;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return data;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::vector<double> row;
        std::string value;
        
        for (char c : line) {
            if (c == ',') {
                if (!value.empty()) {
                    row.push_back(std::stod(value));
                    value.clear();
                }
            } else {
                value += c;
            }
        }
        // Add the last value in the row
        if (!value.empty()) {
            row.push_back(std::stod(value));
        }
        
        if (!row.empty()) {
            data.push_back(row);
        }
    }
    
    file.close();
    return data;
}

int main() {

    int N = 1000;
    int P = 1000;

    double T = 1;
    double So = 100;
    double K = 100;
    double r = 0.05;
    double v = 0.2;

    std::vector<std::vector<double>> GBM = generatePricePathMatrix(
            P, So, T/N, N, r, v, 42
    );

    writeVectorToCSV("GBM1.csv", 
                      GBM,
                      6);

    return 0;


}