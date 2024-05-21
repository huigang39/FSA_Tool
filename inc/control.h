#ifndef CONTROL_H
#define CONTROL_H

#include <QString>
#include <cstdint>
#include <map>
#include <vector>

class Control {
public:
    Control()  = default;
    ~Control() = default;

    enum class FunctionMode { SineWave, BandWidth };
    const std::map< const std::string, const FunctionMode > functionModeMap = { { std::string( "SineWave" ), FunctionMode::SineWave }, { std::string( "BandWidth" ), FunctionMode::BandWidth } };

    std::vector< float > setControlData( const FunctionMode& functionMode );

private:
    std::vector< float > generateSineWaveData( float amplitude, float frequency, float phase, float time, float sampleRate );
    std::vector< float > generateSweepWaveData( float amplitude, float startFrequency, float endFrequency, float phase, float duration, float sampleRate );
};

#endif  // CONTROL_H