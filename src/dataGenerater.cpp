#include "dataGenerater.h"
#include <iostream>
#include <map>
#include <math.h>
#include <string>
#include <vector>

std::vector< double > DataGenerater::generateControlData( const FunctionMode& functionMode, std::map< ControlDataVariable, double >& controlData ) {
    switch ( functionMode ) {
    case FunctionMode::SineWave:
        return generateSineWaveData( controlData.at( ControlDataVariable::Amplitude ), controlData.at( ControlDataVariable::Frequency ), controlData.at( ControlDataVariable::Phase ),
                                     controlData.at( ControlDataVariable::Duration ), controlData.at( ControlDataVariable::SampleRate ) );
        break;
    case FunctionMode::BandWidth:
        return generateSweepWaveData( controlData.at( ControlDataVariable::Amplitude ), controlData.at( ControlDataVariable::StartFrequency ), controlData.at( ControlDataVariable::EndFrequency ),
                                      controlData.at( ControlDataVariable::Phase ), controlData.at( ControlDataVariable::Duration ), controlData.at( ControlDataVariable::SampleRate ) );
        break;
    default:
        return std::vector< double >();
    }
}

std::vector< double > DataGenerater::generateSineWaveData( const double amplitude, const double frequency, const double phase, const double duration, const double sampleRate ) {
    // std::cout << "Amplitude: " << amplitude << std::endl;
    // std::cout << "Frequency: " << frequency << std::endl;
    // std::cout << "Phase: " << phase << std::endl;

    int                   sampleCount = static_cast< int >( duration * sampleRate );
    std::vector< double > data( sampleCount );

    for ( int i = 0; i < sampleCount; ++i ) {
        double time = i / sampleRate;
        data[ i ]   = amplitude * std::sin( 2 * M_PI * frequency * time + phase );
    }

    return data;
}

std::vector< double > DataGenerater::generateSweepWaveData( const double amplitude, const double startFrequency, const double endFrequency, const double phase, const double duration,
                                                            const double sampleRate ) {
    int                   sampleCount = static_cast< int >( duration * sampleRate );
    std::vector< double > data( sampleCount );

    for ( int i = 0; i < sampleCount; ++i ) {
        double time      = i / sampleRate;
        double frequency = startFrequency + ( endFrequency - startFrequency ) * ( time / duration );
        data[ i ]        = amplitude * std::sin( 2 * M_PI * frequency * time + phase );
    }

    return data;
}