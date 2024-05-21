#include "control.h"
#include <cstdint>
#include <iostream>
#include <math.h>
#include <vector>

std::vector< float > Control::setControlData( const FunctionMode& functionMode ) {
    switch ( functionMode ) {
    case FunctionMode::SineWave:
        return generateSineWaveData( 10, 2, 0, 1000, 1000 );
        break;
    case FunctionMode::BandWidth:
        return generateSweepWaveData( 10, 1, 10, 0, 1000, 1000 );
        break;
    default:
        return std::vector< float >();
    }
}

std::vector< float > Control::generateSineWaveData( float amplitude, float frequency, float phase, float duration, float sampleRate ) {
    // std::cout << "Amplitude: " << amplitude << std::endl;
    // std::cout << "Frequency: " << frequency << std::endl;
    // std::cout << "Phase: " << phase << std::endl;

    int                  sampleCount = static_cast< int >( duration * sampleRate );
    std::vector< float > data( sampleCount );

    for ( int i = 0; i < sampleCount; ++i ) {
        float time = i / sampleRate;
        data[ i ]  = amplitude * std::sin( 2 * M_PI * frequency * time + phase );
    }

    return data;
}

std::vector< float > Control::generateSweepWaveData( float amplitude, float startFrequency, float endFrequency, float phase, float duration, float sampleRate ) {
    int                  sampleCount = static_cast< int >( duration * sampleRate );
    std::vector< float > data( sampleCount );

    for ( int i = 0; i < sampleCount; ++i ) {
        float time      = i / sampleRate;
        float frequency = startFrequency + ( endFrequency - startFrequency ) * ( time / duration );
        data[ i ]       = amplitude * std::sin( 2 * M_PI * frequency * time + phase );
    }

    return data;
}