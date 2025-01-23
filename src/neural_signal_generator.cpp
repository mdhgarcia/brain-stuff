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
    std::vector<std::array<int, 12>> generateSignals(const std::array<float, 8>& startCoords,
                                                      const std::array<float, 8>& endCoords,
                                                      int numSignals = 1024,
                                                      const std::string& noiseType = "gaussian",
                                                      float noiseAmplitude = 1.0f) {
        // Initialize random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distribution(-noiseAmplitude, noiseAmplitude);

        int numSamples = static_cast<int>(endCoords[7] - startCoords[7]) / m_samplePeriod + 1;

        // Initialize data structure to hold time-series signals
        std::vector<std::array<int, 12>> signals(numSignals);

        for (int i = 0; i < numSignals; ++i) {
            // Initialize signal with first sample from start position
            for (int j = 0; j < 6; ++j) {
                signals[i][j] = static_cast<int>(startCoords[j + 3] * 1024);
            }

            for (int t = m_samplePeriod; t <= endCoords[7]; t += m_samplePeriod) {
                // Generate new sample based on the current position and previous signal values
                float x = startCoords[0] + (endCoords[0] - startCoords[0]) * static_cast<float>(t) / numSamples;
                float y = startCoords[1] + (endCoords[1] - startCoords[1]) * static_cast<float>(t) / numSamples;
                float z = startCoords[2] + (endCoords[2] - startCoords[2]) * static_cast<float>(t) / numSamples;

                // Add noise to the signal
                if (noiseType == "gaussian") {
                    std::normal_distribution<float> gaussianDistribution(0.0f, noiseAmplitude);
                    signals[i][0] = static_cast<int>((x + gaussianDistribution(gen)) * 1024);
                    signals[i][1] = static_cast<int>((y + gaussianDistribution(gen)) * 1024);
                    signals[i][2] = static_cast<int>((z + gaussianDistribution(gen)) * 1024);
                } else if (noiseType == "uniform") {
                    std::uniform_real_distribution<float> uniformDistribution(-noiseAmplitude, noiseAmplitude);
                    signals[i][0] = static_cast<int>((x + uniformDistribution(gen)) * 1024);
                    signals[i][1] = static_cast<int>((y + uniformDistribution(gen)) * 1024);
                    signals[i][2] = static_cast<int>((z + uniformDistribution(gen)) * 1024);
                }

                // Update signal values
                for (int j = 0; j < 6; ++j) {
                    signals[i][j + 3] = static_cast<int>(x * 1024);
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