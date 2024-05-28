#ifndef CONTROL_H
#define CONTROL_H

#include "control.h"
#include <QString>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

class DataGenerater {
public:
    DataGenerater()  = default;
    ~DataGenerater() = default;

    enum class FunctionMode {
        SineWave,
        BandWidth,
    };
    const std::map< const std::string, const FunctionMode > functionModeMap{
        { "SineWave", FunctionMode::SineWave },
        { "BandWidth", FunctionMode::BandWidth },
    };

    enum class ControlDataVariable {
        Amplitude,
        Frequency,
        StartFrequency,
        EndFrequency,
        Phase,
        Duration,
        SampleRate,
    };
    std::map< ControlDataVariable, double > controlDataVariable{
        {
            ControlDataVariable::Amplitude,
            0.0,
        },
        {
            ControlDataVariable::Frequency,
            0.0,
        },
        {
            ControlDataVariable::StartFrequency,
            0.0,
        },
        {
            ControlDataVariable::EndFrequency,
            0.0,
        },
        {
            ControlDataVariable::Phase,
            0.0,
        },
        {
            ControlDataVariable::Duration,
            0.0,
        },
        {
            ControlDataVariable::SampleRate,
            0.0,
        },
    };

    std::vector< double > generateControlData( const FunctionMode& functionMode, std::map< ControlDataVariable, double >& controlData );

private:
    std::vector< double > generateSineWaveData( const double amplitude, const double frequency, const double phase, const double duration, const double sampleRate );
    std::vector< double > generateSweepWaveData( const double amplitude, const double startFrequency, const double endFrequency, const double phase, const double duration, const double sampleRate );
};

#endif  // CONTROL_H