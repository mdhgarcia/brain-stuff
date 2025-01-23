#include <iostream>
#include <random>
#include <cmath>
#include <vector>
#include <array>

// Class to model motor function signals for a brain-computer interface
class NeuralSignalGenerator {
public:
    // Constructor with optional sample period (default 1 ms)
    explicit NeuralSignalGenerator(int samplePeriod = 1) : m_samplePeriod(samplePeriod) {}

    // Generate time-series data for the given action and number of signals
    // Cluster-based signal generation function
    std::vector<std::array<int, 12>> generateSignals(const std::array<float, 8>& startCoords,
                                                    const std::array<float, 8>& endCoords,
                                                    int numSignals = 1024,
                                                    float clusterStrength = 0.5f) {
        // Initialize random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        // Define clusters and their corresponding neuron indices
        const int numClusters = 5; // e.g., hand movement, arm movement, etc.
        const int clusterSizes[] = {4, 3, 2, 2, 1}; // e.g., number of neurons in each cluster
        std::vector<int> clusters[numClusters];
        for (int i = 0; i < numClusters; ++i) {
            for (int j = 0; j < clusterSizes[i]; ++j) {
                clusters[i].push_back(i * clusterSizes[i] + j);
            }
        }

        // Initialize signal array
        std::vector<std::array<int, 12>> signals(numSignals);

        // Generate signals for each neuron in a cluster-based manner
        for (int i = 0; i < numSignals; ++i) {
            // Calculate cluster activation strength based on motion intent
            float clusterActivation[numClusters];
            for (int j = 0; j < numClusters; ++j) {
                clusterActivation[j] =
                    std::pow(std::sin(dist(gen)), 2) * clusterStrength +
                    std::pow(std::cos(dist(gen)), 2) * (1 - clusterStrength);
            }

            // Simulate neural activity based on cluster activation
            for (int j = 0; j < numClusters; ++j) {
                for (auto neuron : clusters[j]) {
                    signals[i][neuron] =
                        static_cast<int>(clusterActivation[j] *
                                        (dist(gen) * 100 + 50)); // scale to [0, 150]
                }
            }

            // Introduce some randomness and noise to the signal
            for (auto& neuron : signals[i]) {
                neuron = std::max(0, std::min(neuron, 200));
                if (dist(gen) < 0.1f) { // 10% chance of adding noise
                    neuron += dist(gen) * 50 - 25;
                }
            }
        }

        return signals;
    }

private:
    int m_samplePeriod;
};

int main() {
    // Define an example action
    std::array<float, 8> startCoords = {{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false}};
    std::array<float, 8> endCoords = {{10.0f, 20.0f, 30.0f, M_PI / 2.0f, M_PI / 4.0f, 0.0f, true}};

    // Generate signals
    NeuralSignalGenerator generator;
    std::vector<std::array<int, 12>> signals = generator.generateSignals(startCoords, endCoords);

    // Print generated signals
    for (const auto& signal : signals) {
        for (int value : signal) {
            std::cout << value << " ";
        }
        std::cout << "\n";
    }

    return 0;
}