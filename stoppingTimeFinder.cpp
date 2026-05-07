

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


bool writeVectorToCSV(const std::string& filename,
                      const std::vector<double>& data,
                      int precision = 6) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }
    file << std::fixed << std::setprecision(precision);
    for (size_t i = 0; i < data.size(); ++i) {
        file << data[i];
        if (i < data.size() - 1) file << ",";
    }
    file << "\n";
    file.close();
    return true;
}

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


std::vector<std::vector<double>> load_csv(const std::string& filepath) {
    std::vector<std::vector<double>> result;
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + filepath);

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::vector<double> row;
        std::istringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ',')) {
            row.push_back(std::stod(token));
        }
        result.push_back(row);
    }
    return result;
}


std::vector<std::function<double(double)>> basisSet(int type) {
    using f = std::function<double(double)>;

    switch(type) {

        // Polynomial Deg 2
        case 1:
            return {
                [](double x){ return 1.0; },
                [](double x){ return x; },
                [](double x){ return x*x; },
                [](double x){ return 0.0; }
            };

        // Polynomial Deg 3
        case 2:
            return {
                [](double x){ return 1.0; },
                [](double x){ return x; },
                [](double x){ return x*x; },
                [](double x){ return x*x*x; }
            };

        // Legendre Deg 2
        case 3:
            return {
                [](double x){ return 1.0; },
                [](double x){ return x; },
                [](double x){ return (3*x*x - 1)/2.0; },
                [](double x){ return 0.0; }
            };

        // Legendre Deg 3
        case 4:
            return {
                [](double x){ return 1.0; },
                [](double x){ return x; },
                [](double x){ return (3*x*x - 1)/2.0; },
                [](double x){ return (5*x*x*x - 3*x)/2.0; }
            };

        // Hermite Deg 2
        case 5:
            return {
                [](double x){ return 1.0; },
                [](double x){ return x; },
                [](double x){ return x*x - 1.0; },
                [](double x){ return 0.0; }
            };

        // Hermite Deg 3
        case 6:
            return {
                [](double x){ return 1.0; },
                [](double x){ return x; },
                [](double x){ return x*x - 1.0; },
                [](double x){ return x*x*x - 3*x; }
            };

        // Laguerre Deg 2
        case 7:
            return {
                [](double x){ return std::exp(-x/2.0); },
                [](double x){ return std::exp(-x/2.0)*(1 - x); },
                [](double x){ return std::exp(-x/2.0)*(1 - 2*x + x*x/2.0); },
                [](double x){ return 0.0; }
            };

        // Laguerre Deg 3
        case 8:
            return {
                [](double x){ return std::exp(-x/2.0); },
                [](double x){ return std::exp(-x/2.0)*(1 - x); },
                [](double x){ return std::exp(-x/2.0)*(1 - 2*x + x*x*0.5); },
                [](double x){ return std::exp(-x/2.0)*(1 - 3*x + 1.5*x*x - x*x*x/6.0); }
            };

        default:
            throw std::invalid_argument("Invalid basis type");
    }
}

std::vector<double> regress(const std::vector<double>& X, const std::vector<double>& Y, int regType) {
    const int n = X.size();

    auto basis = basisSet(regType);

    double B11 = 0.0;
    double B12 = 0.0;
    double B13 = 0.0;
    double B14 = 0.0;
    double B22 = 0.0;
    double B23 = 0.0;
    double B24 = 0.0;
    double B33 = 0.0;
    double B34 = 0.0;
    double B44 = 0.0;
    double S0 = 0.0;
    double S1 = 0.0;
    double S2 = 0.0;
    double S3 = 0.0;

    for (int i = 0; i < n; i++) {
        double x = X[i];
        double y = Y[i];
        double bas0 = basis[0](x);
        double bas1 = basis[1](x);
        double bas2 = basis[2](x);
        double bas3 = basis[3](x);
        B11 += bas0 * bas0;
        B12 += bas0 * bas1;
        B13 += bas0 * bas2;
        B14 += bas0 * bas3;
        B22 += bas1 * bas1;
        B23 += bas1 * bas2;
        B24 += bas1 * bas3;
        B33 += bas2 * bas2;
        B34 += bas2 * bas3;
        B44 += bas3 * bas3;
        S0 += bas0 * y;
        S1 += bas1 * y;
        S2 += bas2 * y;
        S3 += bas3 * y;
    }

    Eigen::Matrix4d A;
    A << B11,   B12,  B13,  B14,
         B12,   B22,  B23,  B24,
         B13,   B23,  B33,  B34,
         B14,   B24,  B34,  B44;

    Eigen::Vector4d b(S0, S1, S2, S3);

    Eigen::Vector4d c = A.colPivHouseholderQr().solve(b);

    return {c(0), c(1), c(2), c(3)};
}


std::vector<double> getStoppingTimes(
    double So, double T, int N, int P, double r, double v, double K, int regType, std::vector<std::vector<double>> srcMatrix
) {
    
    double dt = T / N;
    std::vector<std::vector<double>> S = srcMatrix;
    std::vector<std::vector<double>> C(P, std::vector<double>(N, 0.0));
    std::vector<int> itm_indices;
    std::vector<double> X;
    std::vector<double> Y;
    std::vector<double> output;
    

    double c_coeff;
    double b_coeff;
    double a_coeff;


    for (int p = 0; p<P; p++) {
        C[p][N-1] = fmax(K-S[p][N-1],0);
    }
    
    for (int n= N-2; n>=0; n--) {
        //get X and Y
        X.clear();
        Y.clear();
        itm_indices.clear();

        for (int p = 0; p<P; p++) {
            if (K-S[p][n] > 0) {
                double pv = 0.0;
                for (int future = n+1; future < N; future++) {
                    if (C[p][future] > 0) {
                        pv = C[p][future] * exp(-r * dt * (future - n));
                        break;
                    }
                }
                X.push_back(S[p][n]);
                Y.push_back(pv);
                itm_indices.push_back(p);
            }
        }
        //std::cout << X_filtered.size();

        // if it is optimal to exercise nowhere in this step, skip to next step
        if (X.size() == 0) {
            continue;
        }

        
        //here is the part where we determine E() function

        //if there are less that 3 datapoints, assume E() is mean of Y_filtered
        bool useReg = X.size() >2;
        if (useReg) {
            try {
                std::vector<double> solution  = regress(X,Y,regType);
                c_coeff = solution[0];
                b_coeff = solution[1];
                a_coeff = solution[2];
            } catch (const std::runtime_error&) {
                useReg = false;
            }
        }
        
        for (int i = 0; i < itm_indices.size(); i++ ){
            int p = itm_indices[i];
            double intrinsic = fmax(K-S[p][n],0);
            double expectedContinuance;

            if (useReg) {
                expectedContinuance = c_coeff + (b_coeff * S[p][n]) + (a_coeff * S[p][n] * S[p][n]);
            } else {
                expectedContinuance = 0;
            }

            if (intrinsic > expectedContinuance) {
                C[p][n] = intrinsic;
            }
            
        }
    }
    for (int p=0; p<P; p++) {
        bool exercised = false;
        for (int n=0; n<N; n++) {
            if (C[p][n] > 0) {
                output.push_back((n * dt) + dt);
                exercised = true;
                break;
            }
        }
        if (!exercised) output.push_back(-1.0);
    }

    return output;
}

int main() {
    std::vector<int> Ns = {2, 4, 5, 8, 10, 20, 25, 40, 50, 100, 125, 200, 250, 500, 1000};

    std::ofstream out("stops.csv");
    out << std::fixed << std::setprecision(6);

    for (int N : Ns) {
        std::string inFile = "TestSet1/Matrix" + std::to_string(N) + ".csv";

        std::vector<std::vector<double>> srcMatrix = load_csv(inFile);
        std::vector<double> stops = getStoppingTimes(
            100, 1, N, 1000, 0.05, 0.2, 95, 1, srcMatrix
        );

        for (size_t i = 0; i < stops.size(); ++i) {
            out << stops[i];
            if (i < stops.size() - 1) out << ",";
        }
        out << "\n";
    }

    return 0;
}