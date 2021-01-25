//      _____ _____ _____ __
//     |   __|     |  |  |  |        Auto-generated C++
//     |__   |  |  |  |  |  |__      SOUL Version 0.9.0
//     |_____|_____|_____|_____|     https://soul.dev
//

#include <vector>
#include <array>
#include <functional>
#include <cmath>
#include <cstddef>
#include <limits>
#include <cstring>
#include <cassert>


#ifndef SOUL_CPP_ASSERT
#define SOUL_CPP_ASSERT(x) assert (x)
#endif

//==============================================================================
// Generated from graph 'PadSynth', source file: PadSynth.soul
//
class PadSynth
{
public:
    PadSynth() = default;
    ~PadSynth() = default;

    //==============================================================================
    template <typename Type, int32_t size> struct Vector;
    template <typename Type, int32_t size> struct FixedArray;
    template <typename Type> struct DynamicArray;

    static constexpr uint32_t maxBlockSize  = 1024;
    static constexpr uint32_t latency       = 0;

    struct _RenderStats;
    struct _SparseStreamStatus;
    struct _Event_in_struct_Message_1;
    struct _Event_in_f32_1;
    struct _Stream_in_f32_1024;
    struct _Stream_out_f32_1024;
    struct NoteHandler__NoteInfo;
    struct NoteHandler__NoteHandler;
    struct Controller___State;
    struct PolyBlep___State;
    struct PolyBlep_2___State;
    struct ADSREnvelope___State;
    struct ADSREnvelope_2___State;
    struct soul__mixers__DynamicMix___for__root__Voice_voiceOscillatorMix___State;
    struct ParameterStream_3___State;
    struct ParameterStream___State;
    struct CutoffHandler___State;
    struct ParameterStream_4___State;
    struct ParameterStream_5___State;
    struct LowPassFilter__FilterTerms;
    struct LowPassFilter__FrequencySettings;
    struct LowPassFilter__PrecalcTerms;
    struct LowPassFilter___State;
    struct ParameterStream_2___State;
    struct Voice___State;
    struct soul__midi__MPEParser___State;
    struct soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic__VoiceInfo;
    struct soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State;
    struct Oscillators___State;
    struct Delay___for__root__PadSynth__instance_Delay___State;
    struct CombFilter___for__root__Reverb_comb_1116___State;
    struct CombFilter___for__root__Reverb_comb_1188___State;
    struct CombFilter___for__root__Reverb_comb_1277___State;
    struct CombFilter___for__root__Reverb_comb_1356___State;
    struct CombFilter___for__root__Reverb_comb_1422___State;
    struct CombFilter___for__root__Reverb_comb_1491___State;
    struct CombFilter___for__root__Reverb_comb_1557___State;
    struct CombFilter___for__root__Reverb_comb_1617___State;
    struct soul__mixers__FixedSum___for__root__Reverb_dryWetMixer___State;
    struct AllPassFilter___for__root__Reverb_allPass_225___State;
    struct AllPassFilter___for__root__Reverb_allPass_341___State;
    struct AllPassFilter___for__root__Reverb_allPass_441___State;
    struct AllPassFilter___for__root__Reverb_allPass_556___State;
    struct Reverb___State;
    struct soul__mixers__DynamicMix___for__root__PadSynth_dryWetMixer___State;
    struct _State;
    struct soul__midi__Message;
    struct soul__note_events__NoteOn;
    struct soul__note_events__NoteOff;
    struct soul__note_events__PitchBend;
    struct soul__note_events__Pressure;
    struct soul__note_events__Slide;
    struct soul__note_events__Control;
    struct soul__mixers__DynamicMix___for__root__PadSynth_dryWetMixer___IO;
    struct soul__mixers__DynamicMix___for__root__Voice_voiceOscillatorMix___IO;
    struct soul__mixers__FixedSum___for__root__Reverb_dryWetMixer___IO;
    struct PolyBlep___IO;
    struct ADSREnvelope___IO;
    struct LowPassFilter___IO;
    struct ParameterStream___IO;
    struct Controller___IO;
    struct CutoffHandler___IO;
    struct Voice___IO;
    struct Reverb___IO;
    struct Oscillators___IO;
    struct Delay___for__root__PadSynth__instance_Delay___IO;
    struct PolyBlep_2___IO;
    struct ADSREnvelope_2___IO;
    struct ParameterStream_2___IO;
    struct ParameterStream_3___IO;
    struct ParameterStream_4___IO;
    struct ParameterStream_5___IO;
    struct AllPassFilter___for__root__Reverb_allPass_225___IO;
    struct AllPassFilter___for__root__Reverb_allPass_341___IO;
    struct AllPassFilter___for__root__Reverb_allPass_441___IO;
    struct AllPassFilter___for__root__Reverb_allPass_556___IO;
    struct CombFilter___for__root__Reverb_comb_1116___IO;
    struct CombFilter___for__root__Reverb_comb_1188___IO;
    struct CombFilter___for__root__Reverb_comb_1277___IO;
    struct CombFilter___for__root__Reverb_comb_1356___IO;
    struct CombFilter___for__root__Reverb_comb_1422___IO;
    struct CombFilter___for__root__Reverb_comb_1491___IO;
    struct CombFilter___for__root__Reverb_comb_1557___IO;
    struct CombFilter___for__root__Reverb_comb_1617___IO;
    struct StringLiteral;

    //==============================================================================
    // The following methods provide basic initialisation and control for the processor

    void init (double newSampleRate, int sessionID)
    {
        memset (reinterpret_cast<void*> (std::addressof (state)), 0, sizeof (state));
        sampleRate = newSampleRate;
        _initialise (state, sessionID);
        initialisedState = state;
    }

    void reset() noexcept
    {
        state = initialisedState;
    }

    uint32_t getNumXRuns() noexcept
    {
        return static_cast<uint32_t> (_get_num_xruns (state));
    }

    //==============================================================================
    // These classes and functions provide a high-level rendering method that
    // presents the processor as a set of standard audio and MIDI channels.

    static constexpr uint32_t numAudioInputChannels  = 0;
    static constexpr uint32_t numAudioOutputChannels = 1;

    struct MIDIMessage
    {
        uint32_t frameIndex = 0;
        uint8_t byte0 = 0, byte1 = 0, byte2 = 0;
    };

    struct MIDIMessageArray
    {
        const MIDIMessage* messages = nullptr;
        uint32_t numMessages = 0;
    };

    template <typename FloatType = float>
    struct RenderContext
    {
        std::array<const FloatType*, 0> inputChannels;
        std::array<FloatType*, 1> outputChannels;
        MIDIMessageArray  incomingMIDI;
        uint32_t          numFrames = 0;
    };

    //==============================================================================
    template <typename FloatType>
    void render (RenderContext<FloatType> context)
    {
        uint32_t startFrame = 0, startMIDIIndex = 0;

        while (startFrame < context.numFrames)
        {
            auto framesRemaining = context.numFrames - startFrame;
            auto numFramesToDo = framesRemaining < maxBlockSize ? framesRemaining : maxBlockSize;
            auto endMIDIIndex = startMIDIIndex;

            while (endMIDIIndex < context.incomingMIDI.numMessages)
            {
                auto eventTime = context.incomingMIDI.messages[endMIDIIndex].frameIndex;

                if (eventTime > startFrame)
                {
                    auto framesUntilEvent = eventTime - startFrame;

                    if (framesUntilEvent < numFramesToDo)
                        numFramesToDo = framesUntilEvent;

                    break;
                }

                ++endMIDIIndex;
            }

            prepare (numFramesToDo);

            while (startMIDIIndex < endMIDIIndex)
            {
                auto midi = context.incomingMIDI.messages[startMIDIIndex++];
                auto packed = (static_cast<uint32_t> (midi.byte0) << 16) | (static_cast<uint32_t> (midi.byte1) << 8) | static_cast<uint32_t> (midi.byte2);
                _addInputEvent_midiIn_struct_Message (state, { static_cast<int32_t> (packed) });
            }

            advance();

            copyFromInterleaved (&context.outputChannels[0], startFrame, _getOutputFrameArrayRef_audioOut (state).elements, numFramesToDo);
            startFrame += numFramesToDo;
        }
    }

    //==============================================================================
    // The following methods provide low-level access for read/write to all the
    // endpoints directly, and to run the prepare/advance loop.

    void prepare (uint32_t numFramesToBeRendered)
    {
        SOUL_CPP_ASSERT (numFramesToBeRendered != 0);
        framesToAdvance = numFramesToBeRendered;
        _prepare (state, static_cast<int32_t> (numFramesToBeRendered));
    }

    void advance()
    {
        SOUL_CPP_ASSERT (framesToAdvance != 0); // you must call prepare() before advance()!
        auto framesRemaining = framesToAdvance;

        while (framesRemaining > 0)
        {
            auto framesThisCall = framesRemaining < maxBlockSize ? framesRemaining : maxBlockSize;

            run (state, static_cast<int32_t> (framesThisCall));

            totalFramesElapsed += framesThisCall;
            framesRemaining -= framesThisCall;
        }
    }

    void addInputEvent_midiIn (const soul__midi__Message& eventValue)
    {
        _addInputEvent_midiIn_struct_Message (state, eventValue);
    }

    void addInputEvent_cutoffParam (float eventValue)
    {
        _addInputEvent_cutoffParam_f32 (state, eventValue);
    }

    void addInputEvent_resonanceParam (float eventValue)
    {
        _addInputEvent_resonanceParam_f32 (state, eventValue);
    }

    void addInputEvent_ampAttack (float eventValue)
    {
        _addInputEvent_ampAttack_f32 (state, eventValue);
    }

    void addInputEvent_ampDecay (float eventValue)
    {
        _addInputEvent_ampDecay_f32 (state, eventValue);
    }

    void addInputEvent_ampSustain (float eventValue)
    {
        _addInputEvent_ampSustain_f32 (state, eventValue);
    }

    void addInputEvent_ampRelease (float eventValue)
    {
        _addInputEvent_ampRelease_f32 (state, eventValue);
    }

    void addInputEvent_ampSensitivity (float eventValue)
    {
        _addInputEvent_ampSensitivity_f32 (state, eventValue);
    }

    void addInputEvent_osc1waveshape (float eventValue)
    {
        _addInputEvent_osc1waveshape_f32 (state, eventValue);
    }

    void addInputEvent_osc1detuneSemitones (float eventValue)
    {
        _addInputEvent_osc1detuneSemitones_f32 (state, eventValue);
    }

    void addInputEvent_osc1detune (float eventValue)
    {
        _addInputEvent_osc1detune_f32 (state, eventValue);
    }

    void addInputEvent_osc2waveshape (float eventValue)
    {
        _addInputEvent_osc2waveshape_f32 (state, eventValue);
    }

    void addInputEvent_osc2detuneSemitones (float eventValue)
    {
        _addInputEvent_osc2detuneSemitones_f32 (state, eventValue);
    }

    void addInputEvent_osc2detune (float eventValue)
    {
        _addInputEvent_osc2detune_f32 (state, eventValue);
    }

    void addInputEvent_oscillatorMix (float eventValue)
    {
        _addInputEvent_oscillatorMix_f32 (state, eventValue);
    }

    void addInputEvent_filterAttack (float eventValue)
    {
        _addInputEvent_filterAttack_f32 (state, eventValue);
    }

    void addInputEvent_filterDecay (float eventValue)
    {
        _addInputEvent_filterDecay_f32 (state, eventValue);
    }

    void addInputEvent_filterSustain (float eventValue)
    {
        _addInputEvent_filterSustain_f32 (state, eventValue);
    }

    void addInputEvent_filterRelease (float eventValue)
    {
        _addInputEvent_filterRelease_f32 (state, eventValue);
    }

    void addInputEvent_filterSensitivity (float eventValue)
    {
        _addInputEvent_filterSensitivity_f32 (state, eventValue);
    }

    void addInputEvent_filterEnvMod (float eventValue)
    {
        _addInputEvent_filterEnvMod_f32 (state, eventValue);
    }

    void addInputEvent_filterCutoffTrack (float eventValue)
    {
        _addInputEvent_filterCutoffTrack_f32 (state, eventValue);
    }

    void setNextInputStreamFrames_dryWetMix (const float* frames, uint32_t numFramesToUse)
    {
        auto& buffer = _getInputFrameArrayRef_dryWetMix (state);

        for (uint32_t i = 0; i < numFramesToUse; ++i)
            buffer[static_cast<int> (i)] = frames[i];
    }

    void setNextInputStreamSparseFrames_dryWetMix (float targetFrameValue, uint32_t numFramesToReachValue)
    {
        _setSparseInputTarget_dryWetMix (state, targetFrameValue, (int32_t) numFramesToReachValue);
    }

    DynamicArray<const float> getOutputStreamFrames_audioOut()
    {
        return { &(_getOutputFrameArrayRef_audioOut (state).elements[0]), static_cast<int32_t> (framesToAdvance) };
    }

    //==============================================================================
    // The following methods provide a fixed interface for finding out about
    // the input/output endpoints that this processor provides.

    using EndpointID = const char*;
    enum class EndpointType     { value, stream, event };

    struct EndpointDetails
    {
        const char* name;
        EndpointID endpointID;
        EndpointType endpointType;
        const char* frameType;
        uint32_t numAudioChannels;
        const char* annotation;
    };

    std::vector<EndpointDetails> getInputEndpoints() const
    {
        return
        {
            { "midiIn",              "in:midiIn",              EndpointType::event,  "Message { int32 midiBytes }", 0, ""                                                                                                               },
            { "cutoffParam",         "in:cutoffParam",         EndpointType::event,  "float32",                     0, "{ \"name\": \"Cutoff\", \"min\": 0, \"max\": 127, \"init\": 80, \"unit\": \"semi\", \"step\": 0.1 }"            },
            { "resonanceParam",      "in:resonanceParam",      EndpointType::event,  "float32",                     0, "{ \"name\": \"Resonance\", \"min\": 0, \"max\": 100, \"init\": 20, \"unit\": \"%\", \"step\": 1 }"              },
            { "ampAttack",           "in:ampAttack",           EndpointType::event,  "float32",                     0, "{ \"name\": \"Amp Attack\", \"min\": 0, \"max\": 100, \"init\": 80, \"unit\": \"%\", \"step\": 1 }"             },
            { "ampDecay",            "in:ampDecay",            EndpointType::event,  "float32",                     0, "{ \"name\": \"Amp Decay\", \"min\": 0, \"max\": 100, \"init\": 66, \"unit\": \"%\", \"step\": 1 }"              },
            { "ampSustain",          "in:ampSustain",          EndpointType::event,  "float32",                     0, "{ \"name\": \"Amp Sustain\", \"min\": 0, \"max\": 100, \"init\": 100, \"unit\": \"%\", \"step\": 1 }"           },
            { "ampRelease",          "in:ampRelease",          EndpointType::event,  "float32",                     0, "{ \"name\": \"Amp Release\", \"min\": 0, \"max\": 100, \"init\": 88, \"unit\": \"%\", \"step\": 1 }"            },
            { "ampSensitivity",      "in:ampSensitivity",      EndpointType::event,  "float32",                     0, "{ \"name\": \"Amp Sensitivity\", \"min\": 0, \"max\": 100, \"init\": 50, \"unit\": \"%\", \"step\": 1 }"        },
            { "osc1waveshape",       "in:osc1waveshape",       EndpointType::event,  "float32",                     0, "{ \"name\": \"Osc1 Waveshape\", \"min\": 0, \"max\": 100, \"init\": 100, \"unit\": \"%\", \"step\": 1 }"        },
            { "osc1detuneSemitones", "in:osc1detuneSemitones", EndpointType::event,  "float32",                     0, "{ \"name\": \"Osc1 Detune\", \"min\": -24, \"max\": 24, \"init\": 0, \"unit\": \"semi\", \"step\": 1 }"         },
            { "osc1detune",          "in:osc1detune",          EndpointType::event,  "float32",                     0, "{ \"name\": \"Osc1 Detune\", \"min\": -100, \"max\": 100, \"init\": 0, \"unit\": \"cent\", \"step\": 1 }"       },
            { "osc2waveshape",       "in:osc2waveshape",       EndpointType::event,  "float32",                     0, "{ \"name\": \"Osc2 Waveshape\", \"min\": 0, \"max\": 100, \"init\": 1.0, \"unit\": \"%\", \"step\": 1 }"        },
            { "osc2detuneSemitones", "in:osc2detuneSemitones", EndpointType::event,  "float32",                     0, "{ \"name\": \"Osc2 Detune\", \"min\": -24, \"max\": 24, \"init\": 0, \"unit\": \"semi\", \"step\": 1 }"         },
            { "osc2detune",          "in:osc2detune",          EndpointType::event,  "float32",                     0, "{ \"name\": \"Osc2 Detune\", \"min\": -100, \"max\": 100, \"init\": 1, \"unit\": \"cent\", \"step\": 1 }"       },
            { "oscillatorMix",       "in:oscillatorMix",       EndpointType::event,  "float32",                     0, "{ \"name\": \"Osc Mix\", \"min\": 0, \"max\": 100, \"init\": 22, \"unit\": \"%\", \"step\": 1 }"                },
            { "filterAttack",        "in:filterAttack",        EndpointType::event,  "float32",                     0, "{ \"name\": \"Filter Attack\", \"min\": 0, \"max\": 100, \"init\": 80, \"unit\": \"%\", \"step\": 1 }"          },
            { "filterDecay",         "in:filterDecay",         EndpointType::event,  "float32",                     0, "{ \"name\": \"Filter Decay\", \"min\": 0, \"max\": 100, \"init\": 74, \"unit\": \"%\", \"step\": 1 }"           },
            { "filterSustain",       "in:filterSustain",       EndpointType::event,  "float32",                     0, "{ \"name\": \"Filter Sustain\", \"min\": 0, \"max\": 100, \"init\": 75, \"unit\": \"%\", \"step\": 1 }"         },
            { "filterRelease",       "in:filterRelease",       EndpointType::event,  "float32",                     0, "{ \"name\": \"Filter Release\", \"min\": 0, \"max\": 100, \"init\": 66, \"unit\": \"%\", \"step\": 1 }"         },
            { "filterSensitivity",   "in:filterSensitivity",   EndpointType::event,  "float32",                     0, "{ \"name\": \"Filter Sensitivity\", \"min\": 0, \"max\": 100, \"init\": 25, \"unit\": \"%\", \"step\": 1 }"     },
            { "filterEnvMod",        "in:filterEnvMod",        EndpointType::event,  "float32",                     0, "{ \"name\": \"Filter Env Mod\", \"min\": -24, \"max\": 24, \"init\": 12, \"unit\": \"semi\", \"step\": 1 }"     },
            { "filterCutoffTrack",   "in:filterCutoffTrack",   EndpointType::event,  "float32",                     0, "{ \"name\": \"Filter Cutoff Track\", \"min\": 0, \"max\": 1, \"init\": 1, \"unit\": \"semi\", \"step\": 0.01 }" },
            { "dryWetMix",           "in:dryWetMix",           EndpointType::stream, "float32",                     1, "{ \"name\": \"Dry Wet Mix\", \"min\": 0, \"max\": 100, \"init\": 100, \"unit\": \"%\", \"step\": 1 }"           }
        };
    }

    std::vector<EndpointDetails> getOutputEndpoints() const
    {
        return
        {
            { "audioOut", "out:audioOut", EndpointType::stream, "float32", 1, "" }
        };
    }

    //==============================================================================
    // The following methods provide help in dealing with the processor's endpoints
    // in a format suitable for traditional audio plugin channels and parameters.

    struct ParameterProperties
    {
        const char* UID;
        const char* name;
        const char* unit;
        float minValue, maxValue, step, initialValue;
        bool isAutomatable, isBoolean, isHidden;
        const char* group;
        const char* textValues;
    };

    struct Parameter
    {
        ParameterProperties properties;
        float currentValue;
        std::function<void(float)> applyValue;

        void setValue (float f)
        {
            currentValue = snapToLegalValue (f);
            applyValue (f);
        }

        float getValue() const
        {
            return currentValue;
        }

    private:
        float snapToLegalValue (float v) const
        {
            if (properties.step > 0)
                v = properties.minValue + properties.step * std::floor ((v - properties.minValue) / properties.step + 0.5f);

            return v < properties.minValue ? properties.minValue : (v > properties.maxValue ? properties.maxValue : v);
        }
    };

    struct AudioBus
    {
        const char* name;
        uint32_t numChannels;
    };

    static constexpr bool      hasMIDIInput = true;
    static constexpr uint32_t  numParameters = 22;

    static constexpr const ParameterProperties parameters[] =
    {
        {  "cutoffParam",          "Cutoff",               "semi",  0.0f,     127.0f,  0.1f,   80.0f,   true,  false,  false,  "",  ""  },
        {  "resonanceParam",       "Resonance",            "%",     0.0f,     100.0f,  1.0f,   20.0f,   true,  false,  false,  "",  ""  },
        {  "ampAttack",            "Amp Attack",           "%",     0.0f,     100.0f,  1.0f,   80.0f,   true,  false,  false,  "",  ""  },
        {  "ampDecay",             "Amp Decay",            "%",     0.0f,     100.0f,  1.0f,   66.0f,   true,  false,  false,  "",  ""  },
        {  "ampSustain",           "Amp Sustain",          "%",     0.0f,     100.0f,  1.0f,   100.0f,  true,  false,  false,  "",  ""  },
        {  "ampRelease",           "Amp Release",          "%",     0.0f,     100.0f,  1.0f,   88.0f,   true,  false,  false,  "",  ""  },
        {  "ampSensitivity",       "Amp Sensitivity",      "%",     0.0f,     100.0f,  1.0f,   50.0f,   true,  false,  false,  "",  ""  },
        {  "osc1waveshape",        "Osc1 Waveshape",       "%",     0.0f,     100.0f,  1.0f,   100.0f,  true,  false,  false,  "",  ""  },
        {  "osc1detuneSemitones",  "Osc1 Detune",          "semi",  -24.0f,   24.0f,   1.0f,   0.0f,    true,  false,  false,  "",  ""  },
        {  "osc1detune",           "Osc1 Detune",          "cent",  -100.0f,  100.0f,  1.0f,   0.0f,    true,  false,  false,  "",  ""  },
        {  "osc2waveshape",        "Osc2 Waveshape",       "%",     0.0f,     100.0f,  1.0f,   1.0f,    true,  false,  false,  "",  ""  },
        {  "osc2detuneSemitones",  "Osc2 Detune",          "semi",  -24.0f,   24.0f,   1.0f,   0.0f,    true,  false,  false,  "",  ""  },
        {  "osc2detune",           "Osc2 Detune",          "cent",  -100.0f,  100.0f,  1.0f,   1.0f,    true,  false,  false,  "",  ""  },
        {  "oscillatorMix",        "Osc Mix",              "%",     0.0f,     100.0f,  1.0f,   22.0f,   true,  false,  false,  "",  ""  },
        {  "filterAttack",         "Filter Attack",        "%",     0.0f,     100.0f,  1.0f,   80.0f,   true,  false,  false,  "",  ""  },
        {  "filterDecay",          "Filter Decay",         "%",     0.0f,     100.0f,  1.0f,   74.0f,   true,  false,  false,  "",  ""  },
        {  "filterSustain",        "Filter Sustain",       "%",     0.0f,     100.0f,  1.0f,   75.0f,   true,  false,  false,  "",  ""  },
        {  "filterRelease",        "Filter Release",       "%",     0.0f,     100.0f,  1.0f,   66.0f,   true,  false,  false,  "",  ""  },
        {  "filterSensitivity",    "Filter Sensitivity",   "%",     0.0f,     100.0f,  1.0f,   25.0f,   true,  false,  false,  "",  ""  },
        {  "filterEnvMod",         "Filter Env Mod",       "semi",  -24.0f,   24.0f,   1.0f,   12.0f,   true,  false,  false,  "",  ""  },
        {  "filterCutoffTrack",    "Filter Cutoff Track",  "semi",  0.0f,     1.0f,    0.01f,  1.0f,    true,  false,  false,  "",  ""  },
        {  "dryWetMix",            "Dry Wet Mix",          "%",     0.0f,     100.0f,  1.0f,   100.0f,  true,  false,  false,  "",  ""  }
    };

    static std::vector<ParameterProperties> getParameterProperties() { return { parameters,  parameters  + numParameters }; }

    static constexpr uint32_t numInputBuses  = 0;
    static constexpr uint32_t numOutputBuses = 1;

    static constexpr const AudioBus outputBuses[numOutputBuses] = { { "audioOut", 1 } };

    static std::vector<AudioBus> getInputBuses()  { return {}; }
    static std::vector<AudioBus> getOutputBuses() { return { outputBuses,  outputBuses  + numOutputBuses }; }

    std::vector<Parameter> createParameterList()
    {
        return
        {
            {  parameters[0],   80.0f,   [this] (float v) { addInputEvent_cutoffParam (v); }                    },
            {  parameters[1],   20.0f,   [this] (float v) { addInputEvent_resonanceParam (v); }                 },
            {  parameters[2],   80.0f,   [this] (float v) { addInputEvent_ampAttack (v); }                      },
            {  parameters[3],   66.0f,   [this] (float v) { addInputEvent_ampDecay (v); }                       },
            {  parameters[4],   100.0f,  [this] (float v) { addInputEvent_ampSustain (v); }                     },
            {  parameters[5],   88.0f,   [this] (float v) { addInputEvent_ampRelease (v); }                     },
            {  parameters[6],   50.0f,   [this] (float v) { addInputEvent_ampSensitivity (v); }                 },
            {  parameters[7],   100.0f,  [this] (float v) { addInputEvent_osc1waveshape (v); }                  },
            {  parameters[8],   0.0f,    [this] (float v) { addInputEvent_osc1detuneSemitones (v); }            },
            {  parameters[9],   0.0f,    [this] (float v) { addInputEvent_osc1detune (v); }                     },
            {  parameters[10],  1.0f,    [this] (float v) { addInputEvent_osc2waveshape (v); }                  },
            {  parameters[11],  0.0f,    [this] (float v) { addInputEvent_osc2detuneSemitones (v); }            },
            {  parameters[12],  1.0f,    [this] (float v) { addInputEvent_osc2detune (v); }                     },
            {  parameters[13],  22.0f,   [this] (float v) { addInputEvent_oscillatorMix (v); }                  },
            {  parameters[14],  80.0f,   [this] (float v) { addInputEvent_filterAttack (v); }                   },
            {  parameters[15],  74.0f,   [this] (float v) { addInputEvent_filterDecay (v); }                    },
            {  parameters[16],  75.0f,   [this] (float v) { addInputEvent_filterSustain (v); }                  },
            {  parameters[17],  66.0f,   [this] (float v) { addInputEvent_filterRelease (v); }                  },
            {  parameters[18],  25.0f,   [this] (float v) { addInputEvent_filterSensitivity (v); }              },
            {  parameters[19],  12.0f,   [this] (float v) { addInputEvent_filterEnvMod (v); }                   },
            {  parameters[20],  1.0f,    [this] (float v) { addInputEvent_filterCutoffTrack (v); }              },
            {  parameters[21],  100.0f,  [this] (float v) { setNextInputStreamSparseFrames_dryWetMix (v, 0); }  }
        };
    }

    static constexpr bool hasTimelineEndpoints = false;

    void setTimeSignature (int32_t newNumerator, int32_t newDenominator)
    {
        (void) newNumerator; (void) newDenominator;
    }

    void setTempo (float newBPM)
    {
        (void) newBPM;
    }

    void setTransportState (int32_t newState)
    {
        (void) newState;
    }

    void setPosition (int64_t currentFrame, double currentQuarterNote, double lastBarStartQuarterNote)
    {
        (void) currentFrame; (void) currentQuarterNote; (void) lastBarStartQuarterNote;
    }

    //==============================================================================
    struct ZeroInitialiser
    {
        template <typename Type>   operator Type() const   { return {}; }
        template <typename Index>  ZeroInitialiser operator[] (Index) const { return {}; }
    };

    //==============================================================================
    template <typename Type>
    struct DynamicArray
    {
        using ElementType = Type;
        ElementType* elements = nullptr;
        int32_t numElements = 0;

        constexpr ElementType& operator[] (int i) noexcept                   { return elements[i]; }
        constexpr const ElementType& operator[] (int i) const noexcept       { return elements[i]; }
        constexpr int getElementSizeBytes() const noexcept                   { return sizeof (ElementType); }
    };

    //==============================================================================
    template <typename Type, int32_t size>
    struct FixedArray
    {
        using ElementType = Type;
        ElementType elements[size];
        static constexpr int32_t numElements = size;

        static constexpr FixedArray fromRepeatedValue (ElementType value)
        {
            FixedArray a;

            for (auto& element : a.elements)
                element = value;

            return a;
        }

        static size_t elementOffset (int i) noexcept               { return sizeof (ElementType) * i; }
        ElementType& operator[] (int i) noexcept                   { return elements[i]; }
        const ElementType& operator[] (int i) const noexcept       { return elements[i]; }
        int getElementSizeBytes() const noexcept                   { return sizeof (ElementType); }
        DynamicArray<ElementType> toDynamicArray() const noexcept  { return { const_cast<ElementType*> (&elements[0]), size }; }
        operator ElementType*() const noexcept                     { return const_cast<ElementType*> (&elements[0]); }

        FixedArray& operator= (ZeroInitialiser)
        {
            for (auto& e : elements)
                e = ElementType {};

            return *this;
        }

        template <int start, int end>
        constexpr FixedArray<Type, end - start> slice() const noexcept
        {
            FixedArray<Type, end - start> newSlice;

            for (int i = 0; i < end - start; ++i)
                newSlice.elements[i] = elements[start + i];

            return newSlice;
        }
    };

    //==============================================================================
    template <typename Type, int32_t size>
    struct Vector
    {
        using ElementType = Type;
        ElementType elements[size] = {};
        static constexpr int32_t numElements = size;

        constexpr Vector() = default;
        constexpr Vector (const Vector&) = default;
        constexpr Vector& operator= (const Vector&) = default;

        explicit constexpr Vector (Type value)
        {
            for (auto& element : elements)
                element = value;
        }

        template <typename OtherType>
        constexpr Vector (const Vector<OtherType, size>& other)
        {
            for (int32_t i = 0; i < size; ++i)
                elements[i] = static_cast<Type> (other.elements[i]);
        }

        constexpr Vector (std::initializer_list<Type> i)
        {
            int n = 0;

            for (auto e : i)
                elements[n++] = e;
        }

        static constexpr Vector fromRepeatedValue (Type value)
        {
            return Vector (value);
        }

        constexpr Vector operator+ (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a + b; }); }
        constexpr Vector operator- (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a - b; }); }
        constexpr Vector operator* (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a * b; }); }
        constexpr Vector operator/ (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a / b; }); }
        constexpr Vector operator% (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a % b; }); }
        constexpr Vector operator-() const                                  { return apply<Vector> ([] (Type n) { return -n; }); }
        constexpr Vector operator~() const                                  { return apply<Vector> ([] (Type n) { return ~n; }); }
        constexpr Vector operator!() const                                  { return apply<Vector> ([] (Type n) { return ! n; }); }

        Vector& operator= (ZeroInitialiser)
        {
            for (auto& e : elements)
                e = {};

            return *this;
        }

        constexpr Vector<bool, size> operator== (const Vector& rhs) const   { return apply<Vector<bool, size>> (rhs, [] (Type a, Type b) { return a == b; }); }
        constexpr Vector<bool, size> operator!= (const Vector& rhs) const   { return apply<Vector<bool, size>> (rhs, [] (Type a, Type b) { return a != b; }); }

        template <typename ReturnType, typename Op>
        constexpr ReturnType apply (const Vector& rhs, Op&& op) const noexcept
        {
            ReturnType v;

            for (int i = 0; i < size; ++i)
                v.elements[i] = op (elements[i], rhs.elements[i]);

            return v;
        }

        template <typename ReturnType, typename Op>
        constexpr ReturnType apply (Op&& op) const noexcept
        {
            ReturnType v;

            for (int i = 0; i < size; ++i)
                v.elements[i] = op (elements[i]);

            return v;
        }

        template <int start, int end>
        constexpr Vector<Type, end - start> slice() const noexcept
        {
            Vector<Type, end - start> newSlice;

            for (int i = 0; i < end - start; ++i)
                newSlice.elements[i] = elements[start + i];

            return newSlice;
        }

        constexpr const Type& operator[] (int i) const noexcept  { return elements[i]; }
        constexpr Type& operator[] (int i) noexcept              { return elements[i]; }
    };

    //==============================================================================
    struct StringLiteral
    {
        constexpr StringLiteral (int32_t h) noexcept : handle (h) {}
        StringLiteral() = default;
        StringLiteral (const StringLiteral&) = default;
        StringLiteral& operator= (const StringLiteral&) = default;

        const char* toString() const       { return lookupStringLiteral (handle); }
        operator const char*() const       { return lookupStringLiteral (handle); }

        bool operator== (StringLiteral other) const noexcept    { return handle == other.handle; }
        bool operator!= (StringLiteral other) const noexcept    { return handle != other.handle; }

        int32_t handle = 0;
    };


    //==============================================================================
    //==============================================================================
    //
    //    All the code that follows this point should be considered internal.
    //    User code should rarely need to refer to anything beyond this point..
    //
    //==============================================================================
    //==============================================================================

    template <typename Vec>  static Vec _vec_sqrt  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::sqrt (x); }); }
    template <typename Vec>  static Vec _vec_pow   (Vec a, Vec b)  { return a.template apply<Vec> ([&] (typename Vec::ElementType x) { return std::pow (x, b); }); }
    template <typename Vec>  static Vec _vec_exp   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::exp (x); }); }
    template <typename Vec>  static Vec _vec_log   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::log (x); }); }
    template <typename Vec>  static Vec _vec_log10 (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::log10 (x); }); }
    template <typename Vec>  static Vec _vec_sin   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::sin (x); }); }
    template <typename Vec>  static Vec _vec_cos   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::cos (x); }); }
    template <typename Vec>  static Vec _vec_tan   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::tan (x); }); }
    template <typename Vec>  static Vec _vec_sinh  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::sinh (x); }); }
    template <typename Vec>  static Vec _vec_cosh  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::cosh (x); }); }
    template <typename Vec>  static Vec _vec_tanh  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::tanh (x); }); }
    template <typename Vec>  static Vec _vec_asinh (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::asinh (x); }); }
    template <typename Vec>  static Vec _vec_acosh (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::acosh (x); }); }
    template <typename Vec>  static Vec _vec_atanh (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::atanh (x); }); }
    template <typename Vec>  static Vec _vec_asin  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::asin (x); }); }
    template <typename Vec>  static Vec _vec_acos  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::acos (x); }); }
    template <typename Vec>  static Vec _vec_atan  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::atan (x); }); }
    template <typename Vec>  static Vec _vec_atan2 (Vec a, Vec b)  { return a.template apply<Vec> ([&] (typename Vec::ElementType x) { return std::atan2 (x, b); }); }

    static constexpr int32_t _intrin_clamp (int32_t n, int32_t low, int32_t high)  { return n < low ? low : (n > high ? high : n); }
    static constexpr int32_t _intrin_wrap (int32_t n, int32_t range)   { if (range == 0) return 0; auto x = n % range; return x < 0 ? x + range : x; }

    static constexpr float  _nan32 = std::numeric_limits<float>::quiet_NaN();
    static constexpr double _nan64 = std::numeric_limits<double>::quiet_NaN();

    static constexpr float  _inf32 = std::numeric_limits<float>::infinity();
    static constexpr double _inf64 = std::numeric_limits<double>::infinity();

    static constexpr float  _ninf32 = -_inf32;
    static constexpr double _ninf64 = -_inf64;

    template <typename SourceFloatType, typename DestFloatType>
    static inline void copyToInterleaved (DestFloatType* monoDest, const SourceFloatType* const* sourceChannels, uint32_t sourceStartFrame, uint32_t numFrames)
    {
        auto source = *sourceChannels + sourceStartFrame;

        for (uint32_t i = 0; i < numFrames; ++i)
            monoDest[i] = static_cast<DestFloatType> (source[i]);
    }

    template <typename SourceFloatType, typename DestFloatType, int32_t numChannels>
    static inline void copyToInterleaved (Vector<DestFloatType, numChannels>* vectorDest, const SourceFloatType* const* sourceChannels, uint32_t sourceStartFrame, uint32_t numFrames)
    {
        for (uint32_t i = 0; i < numFrames; ++i)
            for (uint32_t chan = 0; chan < static_cast<uint32_t> (numChannels); ++chan)
                vectorDest[i].elements[chan] = static_cast<DestFloatType> (sourceChannels[chan][sourceStartFrame + i]);
    }

    template <typename SourceFloatType, typename DestFloatType>
    static inline void copyFromInterleaved (DestFloatType* const* destChannels, uint32_t destStartFrame, const SourceFloatType* monoSource, uint32_t numFrames)
    {
        auto dest = *destChannels + destStartFrame;

        for (uint32_t i = 0; i < numFrames; ++i)
            dest[i] = static_cast<DestFloatType> (monoSource[i]);
    }

    template <typename SourceFloatType, typename DestFloatType, int32_t numChannels>
    static inline void copyFromInterleaved (DestFloatType* const* destChannels, uint32_t destStartFrame, const Vector<SourceFloatType, numChannels>* vectorSource, uint32_t numFrames)
    {
        for (uint32_t i = 0; i < numFrames; ++i)
            for (uint32_t chan = 0; chan < static_cast<uint32_t> (numChannels); ++chan)
                destChannels[chan][destStartFrame + i] = static_cast<DestFloatType> (vectorSource[i].elements[chan]);
    }

    //==============================================================================
    struct _RenderStats
    {
        int32_t m_underrunCount, m_underrunFrames, m_overrunCount, m_overrunFrames;
    };

    struct _SparseStreamStatus
    {
        int32_t m_activeRamps;
        FixedArray<int32_t, 23> m_rampArray;
    };

    struct _Event_in_struct_Message_1
    {
        int32_t m_numFrames;
    };

    struct _Event_in_f32_1
    {
        int32_t m_numFrames;
    };

    struct _Stream_in_f32_1024
    {
        FixedArray<float, 1024> m_buffer;
        float m_currentSparseValue, m_targetSparseValue, m_perFrameIncrement;
        int32_t m_numSparseFramesToRender, m_constantFilledFrames;
        bool m_sparseStreamActive;
    };

    struct _Stream_out_f32_1024
    {
        FixedArray<float, 1024> m_buffer;
    };

    struct NoteHandler__NoteInfo
    {
        int32_t m_channel;
        float m_bendSemitones, m_note, m_pressure, m_slide;
    };

    struct NoteHandler__NoteHandler
    {
        float m_currentVelocity, m_currentNote, m_currentBendSemitones, m_currentPressure, m_currentSlide;
        int32_t m_maxNotes;
        FixedArray<NoteHandler__NoteInfo, 16> m_noteArray;
        int32_t m_activeNotes;
    };

    struct Controller___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        NoteHandler__NoteHandler m_noteHandler;
        float m_multiplier;
        int32_t m_sampleCount;
    };

    struct PolyBlep___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_normalisedPhase, m_normalisedPhaseIncrement, m_waveshapeValue, m_detuneSemitonesValue, m_detuneValue, m_frequencyMultiplier;
    };

    struct PolyBlep_2___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_normalisedPhase, m_normalisedPhaseIncrement, m_waveshapeValue, m_detuneSemitonesValue, m_detuneValue, m_frequencyMultiplier;
    };

    struct ADSREnvelope___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        double m_attackRamp, m_decayMultiplier, m_sustainLevel, m_releaseMultiplier;
        float m_velocitySensitivity;
        double m_targetValue, m_value;
    };

    struct ADSREnvelope_2___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        double m_attackRamp, m_decayMultiplier, m_sustainLevel, m_releaseMultiplier;
        float m_velocitySensitivity;
        double m_targetValue, m_value;
    };

    struct soul__mixers__DynamicMix___for__root__Voice_voiceOscillatorMix___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
    };

    struct ParameterStream_3___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_currentValue;
    };

    struct ParameterStream___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_currentValue;
    };

    struct CutoffHandler___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
    };

    struct ParameterStream_4___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_currentValue;
    };

    struct ParameterStream_5___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_currentValue;
    };

    struct LowPassFilter__FilterTerms
    {
        float m_a, m_b, m_c;
    };

    struct LowPassFilter__FrequencySettings
    {
        FixedArray<LowPassFilter__FilterTerms, 101> m_resonance;
    };

    struct LowPassFilter__PrecalcTerms
    {
        FixedArray<LowPassFilter__FrequencySettings, 128> m_frequency;
    };

    struct LowPassFilter___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        LowPassFilter__PrecalcTerms m_precalcTerms;
        float m_factor_a1, m_factor_a2, m_factor_b1, m_factor_b2;
        int32_t m_counter_1;
        float m_in_1, m_in_2, m_out_1, m_out_2;
    };

    struct ParameterStream_2___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_currentValue;
    };

    struct Voice___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        Controller___State m_voiceController_state;
        PolyBlep___State m_osc1_state;
        PolyBlep_2___State m_osc2_state;
        ADSREnvelope___State m_amplitudeEnvelope_state;
        ADSREnvelope_2___State m_cutoffEnvelope_state;
        soul__mixers__DynamicMix___for__root__Voice_voiceOscillatorMix___State m_voiceOscillatorMix_state;
        ParameterStream_3___State m_oscillatorMixParam_state;
        ParameterStream___State m_cutoff_state;
        CutoffHandler___State m_voiceCutoffHandler_state;
        ParameterStream_4___State m_cutoffEnvMod_state;
        ParameterStream_5___State m_cutoffKeytrackMod_state;
        LowPassFilter___State m_voiceLowPassFilter_state;
        ParameterStream_2___State m_resonance_state;
    };

    struct soul__midi__MPEParser___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
    };

    struct soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic__VoiceInfo
    {
        bool m_active;
        int32_t m_channel;
        float m_note;
        int32_t m_voiceAge;
    };

    struct soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId, m_nextAllocatedVoiceAge, m_nextUnallocatedVoiceAge;
        FixedArray<soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic__VoiceInfo, 8> m_voiceInfo;
    };

    struct Oscillators___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<Voice___State, 8> m_voices_state;
        soul__midi__MPEParser___State m_midiParser_state;
        soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State m__instance_Basic_state;
    };

    struct Delay___for__root__PadSynth__instance_Delay___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 20000> m_delayBuffer;
        int32_t m_pos;
    };

    struct CombFilter___for__root__Reverb_comb_1116___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1116> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__Reverb_comb_1188___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1188> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__Reverb_comb_1277___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1277> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__Reverb_comb_1356___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1356> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__Reverb_comb_1422___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1422> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__Reverb_comb_1491___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1491> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__Reverb_comb_1557___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1557> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__Reverb_comb_1617___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1617> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct soul__mixers__FixedSum___for__root__Reverb_dryWetMixer___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
    };

    struct AllPassFilter___for__root__Reverb_allPass_225___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 225> m_buffer;
        int32_t m_bufferIndex;
    };

    struct AllPassFilter___for__root__Reverb_allPass_341___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 341> m_buffer;
        int32_t m_bufferIndex;
    };

    struct AllPassFilter___for__root__Reverb_allPass_441___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 441> m_buffer;
        int32_t m_bufferIndex;
    };

    struct AllPassFilter___for__root__Reverb_allPass_556___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 556> m_buffer;
        int32_t m_bufferIndex;
    };

    struct Reverb___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        CombFilter___for__root__Reverb_comb_1116___State m_comb_1116_state;
        CombFilter___for__root__Reverb_comb_1188___State m_comb_1188_state;
        CombFilter___for__root__Reverb_comb_1277___State m_comb_1277_state;
        CombFilter___for__root__Reverb_comb_1356___State m_comb_1356_state;
        CombFilter___for__root__Reverb_comb_1422___State m_comb_1422_state;
        CombFilter___for__root__Reverb_comb_1491___State m_comb_1491_state;
        CombFilter___for__root__Reverb_comb_1557___State m_comb_1557_state;
        CombFilter___for__root__Reverb_comb_1617___State m_comb_1617_state;
        soul__mixers__FixedSum___for__root__Reverb_dryWetMixer___State m_dryWetMixer_state;
        AllPassFilter___for__root__Reverb_allPass_225___State m_allPass_225_state;
        AllPassFilter___for__root__Reverb_allPass_341___State m_allPass_341_state;
        AllPassFilter___for__root__Reverb_allPass_441___State m_allPass_441_state;
        AllPassFilter___for__root__Reverb_allPass_556___State m_allPass_556_state;
    };

    struct soul__mixers__DynamicMix___for__root__PadSynth_dryWetMixer___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
    };

    struct _State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId, m__framesToAdvance;
        _RenderStats m__renderStats;
        _SparseStreamStatus m__sparseStreamStatus;
        _Event_in_struct_Message_1 m__in_midiIn;
        _Event_in_f32_1 m__in_cutoffParam, m__in_resonanceParam, m__in_ampAttack, m__in_ampDecay, m__in_ampSustain, m__in_ampRelease, m__in_ampSensitivity, m__in_osc1waveshape, m__in_osc1detuneSemitones, m__in_osc1detune, m__in_osc2waveshape, m__in_osc2detuneSemitones, m__in_osc2detune, m__in_oscillatorMix,
         m__in_filterAttack, m__in_filterDecay, m__in_filterSustain, m__in_filterRelease, m__in_filterSensitivity, m__in_filterEnvMod, m__in_filterCutoffTrack;
        _Stream_in_f32_1024 m__in_dryWetMix;
        _Stream_out_f32_1024 m__out_audioOut;
        Oscillators___State m_oscillators_state;
        Delay___for__root__PadSynth__instance_Delay___State m__instance_Delay_state;
        Reverb___State m_Reverb_state;
        soul__mixers__DynamicMix___for__root__PadSynth_dryWetMixer___State m_dryWetMixer_state;
    };

    struct soul__midi__Message
    {
        int32_t m_midiBytes;
    };

    struct soul__note_events__NoteOn
    {
        int32_t m_channel;
        float m_note, m_velocity;
    };

    struct soul__note_events__NoteOff
    {
        int32_t m_channel;
        float m_note, m_velocity;
    };

    struct soul__note_events__PitchBend
    {
        int32_t m_channel;
        float m_bendSemitones;
    };

    struct soul__note_events__Pressure
    {
        int32_t m_channel;
        float m_pressure;
    };

    struct soul__note_events__Slide
    {
        int32_t m_channel;
        float m_slide;
    };

    struct soul__note_events__Control
    {
        int32_t m_channel, m_control;
        float m_value;
    };

    struct soul__mixers__DynamicMix___for__root__PadSynth_dryWetMixer___IO
    {
        float m__in_in1, m__in_in2, m__in_mix, m__out_out;
    };

    struct soul__mixers__DynamicMix___for__root__Voice_voiceOscillatorMix___IO
    {
        float m__in_in1, m__in_in2, m__in_mix, m__out_out;
    };

    struct soul__mixers__FixedSum___for__root__Reverb_dryWetMixer___IO
    {
        float m__in_in1, m__in_in2, m__out_out;
    };

    struct PolyBlep___IO
    {
        float m__in_frequencyIn, m__out_audioOut;
    };

    struct ADSREnvelope___IO
    {
        float m__in_triggerLevel, m__out_envelopeLevel;
    };

    struct LowPassFilter___IO
    {
        float m__in_gainIn, m__in_cutoffIn, m__in_resonanceIn, m__in_audioIn, m__out_audioOut;
    };

    struct ParameterStream___IO
    {
        float m__out_audioOut;
    };

    struct Controller___IO
    {
        float m__out_note1_noteOut, m__out_note1_frequencyOut, m__out_note1_active, m__out_note2_frequencyOut, m__out_note1_slide;
    };

    struct CutoffHandler___IO
    {
        float m__in_cutoffIn, m__in_envelopeIn, m__in_envelopeModIn, m__in_trackModIn, m__in_noteIn, m__in_modIn, m__out_cutoffOut;
    };

    struct Voice___IO
    {
        float m__out_audioOut;
    };

    struct Reverb___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct Oscillators___IO
    {
        float m__out_audioOut;
    };

    struct Delay___for__root__PadSynth__instance_Delay___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct PolyBlep_2___IO
    {
        float m__in_frequencyIn, m__out_audioOut;
    };

    struct ADSREnvelope_2___IO
    {
        float m__in_triggerLevel, m__out_envelopeLevel;
    };

    struct ParameterStream_2___IO
    {
        float m__out_audioOut;
    };

    struct ParameterStream_3___IO
    {
        float m__out_audioOut;
    };

    struct ParameterStream_4___IO
    {
        float m__out_audioOut;
    };

    struct ParameterStream_5___IO
    {
        float m__out_audioOut;
    };

    struct AllPassFilter___for__root__Reverb_allPass_225___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct AllPassFilter___for__root__Reverb_allPass_341___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct AllPassFilter___for__root__Reverb_allPass_441___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct AllPassFilter___for__root__Reverb_allPass_556___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct CombFilter___for__root__Reverb_comb_1116___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct CombFilter___for__root__Reverb_comb_1188___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct CombFilter___for__root__Reverb_comb_1277___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct CombFilter___for__root__Reverb_comb_1356___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct CombFilter___for__root__Reverb_comb_1422___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct CombFilter___for__root__Reverb_comb_1491___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct CombFilter___for__root__Reverb_comb_1557___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct CombFilter___for__root__Reverb_comb_1617___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    //==============================================================================
    #if __clang__
     #pragma clang diagnostic push
     #pragma clang diagnostic ignored "-Wunused-label"
     #pragma clang diagnostic ignored "-Wunused-parameter"
     #pragma clang diagnostic ignored "-Wshadow"
    #elif defined(__GNUC__)
     #pragma GCC diagnostic push
     #pragma GCC diagnostic ignored "-Wunused-label"
     #pragma GCC diagnostic ignored "-Wunused-parameter"
     #pragma GCC diagnostic ignored "-Wshadow"
    #elif defined(_MSC_VER)
     #pragma warning (push)
     #pragma warning (disable: 4100 4102 4458)
    #endif

    //==============================================================================
    int32_t run (_State& _state, int32_t maxFrames) noexcept
    {
        int32_t _2 = {};
        float _3 = {};
        Oscillators___IO _4 = {};
        Delay___for__root__PadSynth__instance_Delay___IO _5 = {};
        Reverb___IO _6 = {};
        soul__mixers__DynamicMix___for__root__PadSynth_dryWetMixer___IO _7 = {};

        _2 = _internal___minInt32 (1024, maxFrames);
        _updateRampingStreams (_state, _2);
        _state.m__frameCount = 0;
        _main_loop_check: { if (! (_state.m__frameCount < _2)) goto _exit; }
        _main_loop_body: { _3 = _readFromStream_struct__Stream_in_f32_1024 (_state.m__in_dryWetMix, _state.m__frameCount);
                           _4 = ZeroInitialiser();
                           Oscillators__run (_state.m_oscillators_state, _4);
                           _5 = ZeroInitialiser();
                           _5.m__in_audioIn = _4.m__out_audioOut;
                           Delay___for__root__PadSynth__instance_Delay__run (_state.m__instance_Delay_state, _5);
                           _6 = ZeroInitialiser();
                           _6.m__in_audioIn = _5.m__out_audioOut;
                           Reverb__run (_state.m_Reverb_state, _6);
                           _7 = ZeroInitialiser();
                           _7.m__in_in1 = _4.m__out_audioOut;
                           _7.m__in_in2 = _6.m__out_audioOut;
                           _7.m__in_mix = _3;
                           soul__mixers__DynamicMix___for__root__PadSynth_dryWetMixer__run (_state.m_dryWetMixer_state, _7);
                           _writeToStream_struct__Stream_out_f32_1024 (_state.m__out_audioOut, _state.m__frameCount, _7.m__out_out);
                           _state.m__frameCount = _state.m__frameCount + 1;
                           goto _main_loop_check;
        }
        _exit: { _state.m__frameCount = 0;
                 return _2;
        }
    }

    void _initialise (_State& _state, int32_t sessionID) noexcept
    {
        _state.m__sessionID = sessionID;
        _state.m_oscillators_state.m__arrayEntry = 0;
        _state.m_oscillators_state.m__sessionID = _state.m__sessionID;
        _state.m_oscillators_state.m__processorId = 37;
        Oscillators___initialise (_state.m_oscillators_state);
        _state.m__instance_Delay_state.m__arrayEntry = 0;
        _state.m__instance_Delay_state.m__sessionID = _state.m__sessionID;
        _state.m__instance_Delay_state.m__processorId = 38;
        Delay___for__root__PadSynth__instance_Delay___initialise (_state.m__instance_Delay_state);
        _state.m_Reverb_state.m__arrayEntry = 0;
        _state.m_Reverb_state.m__sessionID = _state.m__sessionID;
        _state.m_Reverb_state.m__processorId = 39;
        Reverb___initialise (_state.m_Reverb_state);
        _state.m_dryWetMixer_state.m__arrayEntry = 0;
        _state.m_dryWetMixer_state.m__sessionID = _state.m__sessionID;
        _state.m_dryWetMixer_state.m__processorId = 40;
    }

    void _addInputEvent_midiIn_struct_Message (_State& _state, const soul__midi__Message& event) noexcept
    {
        Oscillators___midiIn_struct_Message (_state.m_oscillators_state, event);
    }

    void _addInputEvent_cutoffParam_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___cutoffParam_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_resonanceParam_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___resonanceParam_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_ampAttack_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___ampAttack_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_ampDecay_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___ampDecay_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_ampSustain_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___ampSustain_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_ampRelease_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___ampRelease_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_ampSensitivity_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___ampSensitivity_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_osc1waveshape_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___osc1waveshape_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_osc1detuneSemitones_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___osc1detuneSemitones_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_osc1detune_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___osc1detune_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_osc2waveshape_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___osc2waveshape_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_osc2detuneSemitones_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___osc2detuneSemitones_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_osc2detune_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___osc2detune_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_oscillatorMix_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___oscillatorMix_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_filterAttack_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___filterAttack_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_filterDecay_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___filterDecay_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_filterSustain_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___filterSustain_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_filterRelease_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___filterRelease_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_filterSensitivity_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___filterSensitivity_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_filterEnvMod_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___filterEnvMod_f32 (_state.m_oscillators_state, event);
    }

    void _addInputEvent_filterCutoffTrack_f32 (_State& _state, const float& event) noexcept
    {
        Oscillators___filterCutoffTrack_f32 (_state.m_oscillators_state, event);
    }

    FixedArray<float, 1024>& _getInputFrameArrayRef_dryWetMix (_State& _state) noexcept
    {
        return _state.m__in_dryWetMix.m_buffer;
    }

    void _setSparseStream_struct__Stream_in_f32_1024 (_Stream_in_f32_1024& streamState, const float& targetValue, int32_t framesToReachTarget) noexcept
    {
        float rampFrames = {}, delta = {};

        if (! (framesToReachTarget == 0)) goto _ramp;
        _no_ramp: { streamState.m_currentSparseValue = targetValue;
                    streamState.m_targetSparseValue = targetValue;
                    streamState.m_perFrameIncrement = 0;
                    streamState.m_numSparseFramesToRender = 0;
                    streamState.m_constantFilledFrames = 0;
                    streamState.m_sparseStreamActive = true;
                    return;
        }
        _ramp: { rampFrames = static_cast<float> (framesToReachTarget);
                 delta = static_cast<float> (targetValue - streamState.m_currentSparseValue);
                 streamState.m_targetSparseValue = targetValue;
                 streamState.m_perFrameIncrement = delta / rampFrames;
                 streamState.m_numSparseFramesToRender = framesToReachTarget;
                 streamState.m_constantFilledFrames = 0;
                 streamState.m_sparseStreamActive = true;
        }
    }

    void _setSparseInputTarget_dryWetMix (_State& _state, const float& targetValue, int32_t framesToReachTarget) noexcept
    {
        if (_state.m__in_dryWetMix.m_sparseStreamActive) goto _block_1;
        _block_0: { _addRampingStream (_state.m__sparseStreamStatus, 22); }
        _block_1: { _setSparseStream_struct__Stream_in_f32_1024 (_state.m__in_dryWetMix, targetValue, framesToReachTarget); }
    }

    FixedArray<float, 1024>& _getOutputFrameArrayRef_audioOut (_State& state) noexcept
    {
        return state.m__out_audioOut.m_buffer;
    }

    void _prepare (_State& state, int32_t frames) noexcept
    {
        state.m__framesToAdvance = frames;
    }

    int32_t _get_num_xruns (_State& state) noexcept
    {
        return state.m__renderStats.m_underrunCount + state.m__renderStats.m_overrunCount;
    }

    //==============================================================================
    void _renderSparseFrames_struct__Stream_in_f32_1024 (_Stream_in_f32_1024& stream, int32_t startFrame, int32_t framesToGenerate) noexcept
    {
        int32_t writePos = {};
        float currentValue = {};

        writePos = startFrame;
        currentValue = stream.m_currentSparseValue;
        _main_loop_check: { if (! (framesToGenerate > 0)) goto _exit_after_loop; }
        _main_loop_body: { stream.m_buffer[writePos] = currentValue;
                           currentValue = currentValue + stream.m_perFrameIncrement;
                           writePos = writePos + 1;
                           framesToGenerate = framesToGenerate - 1;
                           goto _main_loop_check;
        }
        _exit_after_loop: { stream.m_currentSparseValue = currentValue; }
    }

    bool _applySparseStreamData_struct__Stream_in_f32_1024 (_Stream_in_f32_1024& stream, int32_t numFrames) noexcept
    {
        int32_t rampFrames = {};

        rampFrames = 0;
        if (! (stream.m_sparseStreamActive == true)) goto _exitTrue;
        _check_stream_state: { if (! (stream.m_numSparseFramesToRender == 0)) goto _render_ramp; }
        _no_frames_to_render: { if (stream.m_constantFilledFrames == 1024) goto _rampComplete; }
        _add_fixed_value: { stream.m_currentSparseValue = stream.m_targetSparseValue;
                            stream.m_perFrameIncrement = 0;
                            _renderSparseFrames_struct__Stream_in_f32_1024 (stream, stream.m_constantFilledFrames, _internal___minInt32 (numFrames, 1024 - stream.m_constantFilledFrames));
                            stream.m_constantFilledFrames = stream.m_constantFilledFrames + _internal___minInt32 (numFrames, 1024 - stream.m_constantFilledFrames);
                            goto _exit;
        }
        _render_ramp: { rampFrames = _internal___minInt32 (numFrames, stream.m_numSparseFramesToRender);
                        _renderSparseFrames_struct__Stream_in_f32_1024 (stream, 0, rampFrames);
                        stream.m_numSparseFramesToRender = stream.m_numSparseFramesToRender - rampFrames;
                        if (rampFrames == numFrames) goto _exit;
        }
        _fill_remainder: { stream.m_currentSparseValue = stream.m_targetSparseValue;
                           stream.m_perFrameIncrement = 0;
                           _renderSparseFrames_struct__Stream_in_f32_1024 (stream, rampFrames, numFrames - rampFrames);
        }
        _exit: { return false; }
        _exitTrue: { return true; }
        _rampComplete: { stream.m_sparseStreamActive = false;
                         return true;
        }
    }

    void _addRampingStream (_SparseStreamStatus& status, int32_t streamId) noexcept
    {
        status.m_rampArray[status.m_activeRamps] = streamId;
        status.m_activeRamps = status.m_activeRamps + 1;
    }

    bool _updateRampingStream (_State& _state, int32_t streamId, int32_t framesToRender) noexcept
    {
        bool rampComplete = {};

        rampComplete = false;
        if (! (streamId == 22)) goto _exit;
        _case_22: { rampComplete = _applySparseStreamData_struct__Stream_in_f32_1024 (_state.m__in_dryWetMix, framesToRender); }
        _exit: { return rampComplete; }
    }

    void _updateRampingStreams (_State& _state, int32_t framesToRender) noexcept
    {
        bool rampComplete = {};
        int32_t readPos = {}, writePos = {};

        rampComplete = false;
        readPos = 0;
        writePos = 0;
        if (_state.m__sparseStreamStatus.m_activeRamps == 0) goto _exit;
        _loop: { rampComplete = _updateRampingStream (_state, _state.m__sparseStreamStatus.m_rampArray[readPos], framesToRender);
                 if (rampComplete) goto _rampComplete;
        }
        _rampActive: { _state.m__sparseStreamStatus.m_rampArray[writePos] = _state.m__sparseStreamStatus.m_rampArray[readPos];
                       readPos = readPos + 1;
                       writePos = writePos + 1;
                       if (readPos == _state.m__sparseStreamStatus.m_activeRamps) goto _loopExit;
                       goto _loop;
        }
        _rampComplete: { readPos = readPos + 1;
                         if (! (readPos == _state.m__sparseStreamStatus.m_activeRamps)) goto _loop;
        }
        _loopExit: { _state.m__sparseStreamStatus.m_activeRamps = writePos; }
        _exit: {}
    }

    float _readFromStream_struct__Stream_in_f32_1024 (const _Stream_in_f32_1024& stream, int32_t readPos) noexcept
    {
        float _2 = {};

        _2 = stream.m_buffer[readPos];
        return _2;
    }

    void _writeToStream_struct__Stream_out_f32_1024 (_Stream_out_f32_1024& stream, int32_t writePos, float value) noexcept
    {
        stream.m_buffer[writePos] = value;
    }

    //==============================================================================
    float soul__noteNumberToFrequency_2 (float note) noexcept
    {
        float _2 = {};

        _2 = std::pow (2.0f, (note - 69.0f) * 0.083333336f);
        return 440.0f * _2;
    }

    //==============================================================================
    float soul__intrinsics___pow_specialised (float a, float b) noexcept
    {
        return 0;
    }

    double soul__intrinsics___pow_specialised_2 (double a, double b) noexcept
    {
        return 0;
    }

    float soul__intrinsics___sin_specialised (float n) noexcept
    {
        return 0;
    }

    float soul__intrinsics___lerp_specialised (float start, float stop, float amount) noexcept
    {
        return start + ((stop - start) * amount);
    }

    double soul__intrinsics___min_specialised (double a, double b) noexcept
    {
        double _2 = {}, _3 = {}, _4 = {}, _T0 = {};

        if (! (a < b)) goto _ternary_false_0;
        _ternary_true_0: { _2 = a;
                           _T0 = _2;
                           goto _ternary_end_0;
        }
        _ternary_false_0: { _3 = b;
                            _T0 = _3;
        }
        _ternary_end_0: { _4 = _T0;
                          return _4;
        }
    }

    float soul__intrinsics___clamp_specialised (float n, float low, float high) noexcept
    {
        float _2 = {}, _3 = {}, _4 = {}, _5 = {}, _6 = {}, _T1 = {}, _T0 = {};

        if (! (n < low)) goto _ternary_false_0;
        _ternary_true_0: { _2 = low;
                           _T0 = _2;
                           goto _ternary_end_0;
        }
        _ternary_false_0: { if (! (n > high)) goto _ternary_false_1; }
        _ternary_true_1: { _3 = high;
                           _T1 = _3;
                           goto _ternary_end_1;
        }
        _ternary_false_1: { _4 = n;
                            _T1 = _4;
        }
        _ternary_end_1: { _5 = _T1;
                          _T0 = _5;
        }
        _ternary_end_0: { _6 = _T0;
                          return _6;
        }
    }

    float soul__intrinsics___tan_specialised (float n) noexcept
    {
        float _2 = {}, _3 = {};

        _2 = std::sin (n);
        _3 = std::cos (n);
        return _2 / _3;
    }

    float soul__intrinsics___cos_specialised (float n) noexcept
    {
        return 0;
    }

    //==============================================================================
    int32_t soul__midi__getByte1 (soul__midi__Message m) noexcept
    {
        return (m.m_midiBytes >> 16) & 255;
    }

    int32_t soul__midi__getByte2 (soul__midi__Message m) noexcept
    {
        return (m.m_midiBytes >> 8) & 255;
    }

    int32_t soul__midi__getByte3 (soul__midi__Message m) noexcept
    {
        return m.m_midiBytes & 255;
    }

    //==============================================================================
    static Oscillators___State& _stateUpCast (soul__midi__MPEParser___State& s)
    {
        auto offset = static_cast<int32_t> (offsetof (Oscillators___State, m_midiParser_state));
        return *reinterpret_cast<Oscillators___State*> (reinterpret_cast<char*> (&s) - offset);
    }

    //==============================================================================
    void soul__midi__MPEParser___parseMIDI_struct_Message (soul__midi__MPEParser___State& _state, soul__midi__Message message) noexcept
    {
        int32_t _2 = {}, _3 = {}, _4 = {};
        int32_t messageByte1 = {}, messageByte2 = {}, messageByte3 = {}, messageType = {}, channel = {};
        soul__note_events__NoteOff _5 = {}, _6 = {};
        float _7 = {}, _8 = {}, _9 = {}, _10 = {}, _11 = {}, _12 = {};
        soul__note_events__NoteOn _13 = {};
        soul__note_events__Slide _14 = {};
        soul__note_events__Control _15 = {};
        soul__note_events__Pressure _16 = {};
        soul__note_events__PitchBend _17 = {};

        _2 = soul__midi__getByte1 (message);
        messageByte1 = static_cast<int32_t> (_2);
        _3 = soul__midi__getByte2 (message);
        messageByte2 = static_cast<int32_t> (_3);
        _4 = soul__midi__getByte3 (message);
        messageByte3 = static_cast<int32_t> (_4);
        messageType = static_cast<int32_t> (static_cast<int32_t> (messageByte1) & 240);
        channel = static_cast<int32_t> (static_cast<int32_t> (messageByte1) & 15);
        if (! (messageType == 128)) goto _ifnot_0;
        _if_0: { _5 = ZeroInitialiser();
                 _5.m_channel = static_cast<int32_t> (channel);
                 _5.m_note = static_cast<float> (messageByte2);
                 _7 = soul__midi__MPEParser__normaliseValue (_state, static_cast<int32_t> (messageByte3));
                 _5.m_velocity = _7;
                 soul__midi__MPEParser___eventOut_struct_NoteOff (_state, _5);
                 goto _ifend_0;
        }
        _ifnot_0: { if (! (messageType == 144)) goto _ifnot_1; }
        _if_1: { if (! (messageByte3 == 0)) goto _ifnot_2; }
        _if_2: { _6 = ZeroInitialiser();
                 _6.m_channel = static_cast<int32_t> (channel);
                 _6.m_note = static_cast<float> (messageByte2);
                 soul__midi__MPEParser___eventOut_struct_NoteOff (_state, _6);
                 goto _ifend_0;
        }
        _ifnot_2: { _13 = ZeroInitialiser();
                    _13.m_channel = static_cast<int32_t> (channel);
                    _13.m_note = static_cast<float> (messageByte2);
                    _8 = soul__midi__MPEParser__normaliseValue (_state, static_cast<int32_t> (messageByte3));
                    _13.m_velocity = _8;
                    soul__midi__MPEParser___eventOut_struct_NoteOn (_state, _13);
                    goto _ifend_0;
        }
        _ifnot_1: { if (! (messageType == 176)) goto _ifnot_3; }
        _if_3: { if (! (messageByte2 == 74)) goto _ifnot_4; }
        _if_4: { _14 = ZeroInitialiser();
                 _14.m_channel = static_cast<int32_t> (channel);
                 _9 = soul__midi__MPEParser__normaliseValue (_state, static_cast<int32_t> (messageByte3));
                 _14.m_slide = _9;
                 soul__midi__MPEParser___eventOut_struct_Slide (_state, _14);
                 goto _ifend_0;
        }
        _ifnot_4: { _15 = ZeroInitialiser();
                    _15.m_channel = static_cast<int32_t> (channel);
                    _15.m_control = static_cast<int32_t> (messageByte2);
                    _10 = soul__midi__MPEParser__normaliseValue (_state, static_cast<int32_t> (messageByte3));
                    _15.m_value = _10;
                    goto _ifend_0;
        }
        _ifnot_3: { if (! (messageType == 208)) goto _ifnot_5; }
        _if_5: { _16 = ZeroInitialiser();
                 _16.m_channel = static_cast<int32_t> (channel);
                 _11 = soul__midi__MPEParser__normaliseValue (_state, static_cast<int32_t> (messageByte2));
                 _16.m_pressure = _11;
                 soul__midi__MPEParser___eventOut_struct_Pressure (_state, _16);
                 goto _ifend_0;
        }
        _ifnot_5: { if (! (messageType == 224)) goto _ifend_0; }
        _if_6: { _17 = ZeroInitialiser();
                 _17.m_channel = static_cast<int32_t> (channel);
                 _12 = soul__midi__MPEParser__translateBendSemitones (_state, static_cast<int32_t> (messageByte3), static_cast<int32_t> (messageByte2));
                 _17.m_bendSemitones = _12;
                 soul__midi__MPEParser___eventOut_struct_PitchBend (_state, _17);
        }
        _ifend_0: {}
    }

    float soul__midi__MPEParser__normaliseValue (soul__midi__MPEParser___State& _state, int32_t i) noexcept
    {
        return static_cast<float> (i) * 0.007874016f;
    }

    float soul__midi__MPEParser__translateBendSemitones (soul__midi__MPEParser___State& _state, int32_t msb, int32_t lsb) noexcept
    {
        int32_t value = {};

        value = static_cast<int32_t> ((msb * 128) + lsb);
        return static_cast<float> (value - 8192) / 170.66667f;
    }

    void soul__midi__MPEParser___eventOut_struct_NoteOff (soul__midi__MPEParser___State& _state, soul__note_events__NoteOff value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___eventIn_struct_NoteOff (_2.m__instance_Basic_state, value);
    }

    void soul__midi__MPEParser___eventOut_struct_NoteOn (soul__midi__MPEParser___State& _state, soul__note_events__NoteOn value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___eventIn_struct_NoteOn (_2.m__instance_Basic_state, value);
    }

    void soul__midi__MPEParser___eventOut_struct_Slide (soul__midi__MPEParser___State& _state, soul__note_events__Slide value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___eventIn_struct_Slide (_2.m__instance_Basic_state, value);
    }

    void soul__midi__MPEParser___eventOut_struct_Pressure (soul__midi__MPEParser___State& _state, soul__note_events__Pressure value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___eventIn_struct_Pressure (_2.m__instance_Basic_state, value);
    }

    void soul__midi__MPEParser___eventOut_struct_PitchBend (soul__midi__MPEParser___State& _state, soul__note_events__PitchBend value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___eventIn_struct_PitchBend (_2.m__instance_Basic_state, value);
    }

    //==============================================================================
    static Oscillators___State& _stateUpCast (soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State& s)
    {
        auto offset = static_cast<int32_t> (offsetof (Oscillators___State, m__instance_Basic_state));
        return *reinterpret_cast<Oscillators___State*> (reinterpret_cast<char*> (&s) - offset);
    }

    //==============================================================================
    void soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___eventIn_struct_NoteOn (soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State& _state, soul__note_events__NoteOn e) noexcept
    {
        int32_t allocatedVoice = {};
        int32_t allocatedVoiceAge = {}, i = {}, _2 = {}, _3 = {};
        int32_t age = {};
        soul__note_events__NoteOff noteOff = {};

        allocatedVoice = 0;
        allocatedVoiceAge = _state.m_voiceInfo[allocatedVoice].m_voiceAge;
        i = 1;
        _loop_0: { if (! (i < 8)) goto _break_0; }
        _body_0: { age = static_cast<int32_t> (_state.m_voiceInfo[_intrin_wrap (static_cast<int32_t> (i), 8) & 7].m_voiceAge);
                   if (! (age < static_cast<int32_t> (allocatedVoiceAge))) goto _cont_0;
        }
        _if_0: { allocatedVoiceAge = static_cast<int32_t> (age);
                 allocatedVoice = _intrin_wrap (static_cast<int32_t> (i), 8) & 7;
        }
        _cont_0: { _2 = i;
                   i = _2 + 1;
                   goto _loop_0;
        }
        _break_0: { soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___voiceEventOut_struct_NoteOn (_state, static_cast<int32_t> (allocatedVoice) & 7, e);
                    if (! _state.m_voiceInfo[allocatedVoice].m_active) goto _ifnot_1;
        }
        _if_1: { noteOff = ZeroInitialiser();
                 noteOff.m_channel = _state.m_voiceInfo[allocatedVoice].m_channel;
                 noteOff.m_note = _state.m_voiceInfo[allocatedVoice].m_note;
                 soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___voiceEventOut_struct_NoteOff (_state, static_cast<int32_t> (allocatedVoice) & 7, noteOff);
        }
        _ifnot_1: { _state.m_voiceInfo[allocatedVoice].m_active = true;
                    _state.m_voiceInfo[allocatedVoice].m_channel = e.m_channel;
                    _state.m_voiceInfo[allocatedVoice].m_note = e.m_note;
                    _3 = _state.m_nextAllocatedVoiceAge;
                    _state.m_nextAllocatedVoiceAge = _3 + 1;
                    _state.m_voiceInfo[allocatedVoice].m_voiceAge = _3;
        }
    }

    void soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___eventIn_struct_NoteOff (soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State& _state, soul__note_events__NoteOff e) noexcept
    {
        int32_t voice = {}, _2 = {};
        int32_t counter_0 = {};
        bool _3 = {}, _4 = {}, _5 = {}, _T1 = {};
        int32_t _6 = {};

        voice = 0;
        counter_0 = 8;
        _loop_0: { if (! (counter_0 > 0)) goto _break_0; }
        _body_0: { if (! (_state.m_voiceInfo[voice].m_channel == e.m_channel)) goto _ternary_false_1; }
        _ternary_true_1: { _3 = _state.m_voiceInfo[voice].m_note == e.m_note;
                           _T1 = _3;
                           goto _ternary_end_1;
        }
        _ternary_false_1: { _4 = false;
                            _T1 = _4;
        }
        _ternary_end_1: { _5 = _T1;
                          if (! _5) goto _ifnot_0;
        }
        _if_0: { _state.m_voiceInfo[voice].m_active = false;
                 _6 = _state.m_nextUnallocatedVoiceAge;
                 _state.m_nextUnallocatedVoiceAge = _6 + 1;
                 _state.m_voiceInfo[voice].m_voiceAge = _6;
                 soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___voiceEventOut_struct_NoteOff (_state, static_cast<int32_t> (voice) & 7, e);
        }
        _ifnot_0: { _2 = voice;
                    voice = (_2 + 1) & 7;
                    counter_0 = counter_0 - 1;
                    goto _loop_0;
        }
        _break_0: {}
    }

    void soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___eventIn_struct_PitchBend (soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State& _state, soul__note_events__PitchBend e) noexcept
    {
        int32_t voice = {}, _2 = {};
        int32_t counter_0 = {};

        voice = 0;
        counter_0 = 8;
        _loop_0: { if (! (counter_0 > 0)) goto _break_0; }
        _body_0: { if (! (_state.m_voiceInfo[voice].m_channel == e.m_channel)) goto _ifnot_0; }
        _if_0: { soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___voiceEventOut_struct_PitchBend (_state, static_cast<int32_t> (voice) & 7, e); }
        _ifnot_0: { _2 = voice;
                    voice = (_2 + 1) & 7;
                    counter_0 = counter_0 - 1;
                    goto _loop_0;
        }
        _break_0: {}
    }

    void soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___eventIn_struct_Pressure (soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State& _state, soul__note_events__Pressure p) noexcept
    {
        int32_t voice = {}, _2 = {};
        int32_t counter_0 = {};

        voice = 0;
        counter_0 = 8;
        _loop_0: { if (! (counter_0 > 0)) goto _break_0; }
        _body_0: { if (! (_state.m_voiceInfo[voice].m_channel == p.m_channel)) goto _ifnot_0; }
        _if_0: { soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___voiceEventOut_struct_Pressure (_state, static_cast<int32_t> (voice) & 7, p); }
        _ifnot_0: { _2 = voice;
                    voice = (_2 + 1) & 7;
                    counter_0 = counter_0 - 1;
                    goto _loop_0;
        }
        _break_0: {}
    }

    void soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___eventIn_struct_Slide (soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State& _state, soul__note_events__Slide s) noexcept
    {
        int32_t voice = {}, _2 = {};
        int32_t counter_0 = {};

        voice = 0;
        counter_0 = 8;
        _loop_0: { if (! (counter_0 > 0)) goto _break_0; }
        _body_0: { if (! (_state.m_voiceInfo[voice].m_channel == s.m_channel)) goto _ifnot_0; }
        _if_0: { soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___voiceEventOut_struct_Slide (_state, static_cast<int32_t> (voice) & 7, s); }
        _ifnot_0: { _2 = voice;
                    voice = (_2 + 1) & 7;
                    counter_0 = counter_0 - 1;
                    goto _loop_0;
        }
        _break_0: {}
    }

    void soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___initialise (soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State& _state) noexcept
    {
        _state.m_nextAllocatedVoiceAge = 1000000000;
        _state.m_nextUnallocatedVoiceAge = 1;
        _state.m_voiceInfo = ZeroInitialiser();
    }

    void soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___voiceEventOut_struct_NoteOn (soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State& _state, int32_t element, soul__note_events__NoteOn value) noexcept
    {
        if (element == 0) goto _element_0;
        _block_1: { if (element == 1) goto _element_1; }
        _block_2: { if (element == 2) goto _element_2; }
        _block_3: { if (element == 3) goto _element_3; }
        _block_4: { if (element == 4) goto _element_4; }
        _block_5: { if (element == 5) goto _element_5; }
        _block_6: { if (element == 6) goto _element_6; }
        _block_7: { if (element == 7) goto _element_7; }
        _block_8: { return; }
        _element_0: { auto& _2 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOn (_2.m_voices_state[0], value);
                      return;
        }
        _element_1: { auto& _3 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOn (_3.m_voices_state[1], value);
                      return;
        }
        _element_2: { auto& _4 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOn (_4.m_voices_state[2], value);
                      return;
        }
        _element_3: { auto& _5 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOn (_5.m_voices_state[3], value);
                      return;
        }
        _element_4: { auto& _6 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOn (_6.m_voices_state[4], value);
                      return;
        }
        _element_5: { auto& _7 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOn (_7.m_voices_state[5], value);
                      return;
        }
        _element_6: { auto& _8 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOn (_8.m_voices_state[6], value);
                      return;
        }
        _element_7: { auto& _9 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOn (_9.m_voices_state[7], value);
        }
    }

    void soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___voiceEventOut_struct_NoteOff (soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State& _state, int32_t element, soul__note_events__NoteOff value) noexcept
    {
        if (element == 0) goto _element_0;
        _block_1: { if (element == 1) goto _element_1; }
        _block_2: { if (element == 2) goto _element_2; }
        _block_3: { if (element == 3) goto _element_3; }
        _block_4: { if (element == 4) goto _element_4; }
        _block_5: { if (element == 5) goto _element_5; }
        _block_6: { if (element == 6) goto _element_6; }
        _block_7: { if (element == 7) goto _element_7; }
        _block_8: { return; }
        _element_0: { auto& _2 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOff (_2.m_voices_state[0], value);
                      return;
        }
        _element_1: { auto& _3 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOff (_3.m_voices_state[1], value);
                      return;
        }
        _element_2: { auto& _4 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOff (_4.m_voices_state[2], value);
                      return;
        }
        _element_3: { auto& _5 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOff (_5.m_voices_state[3], value);
                      return;
        }
        _element_4: { auto& _6 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOff (_6.m_voices_state[4], value);
                      return;
        }
        _element_5: { auto& _7 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOff (_7.m_voices_state[5], value);
                      return;
        }
        _element_6: { auto& _8 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOff (_8.m_voices_state[6], value);
                      return;
        }
        _element_7: { auto& _9 = _stateUpCast (_state);
                      Voice___eventIn_struct_NoteOff (_9.m_voices_state[7], value);
        }
    }

    void soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___voiceEventOut_struct_PitchBend (soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State& _state, int32_t element, soul__note_events__PitchBend value) noexcept
    {
        if (element == 0) goto _element_0;
        _block_1: { if (element == 1) goto _element_1; }
        _block_2: { if (element == 2) goto _element_2; }
        _block_3: { if (element == 3) goto _element_3; }
        _block_4: { if (element == 4) goto _element_4; }
        _block_5: { if (element == 5) goto _element_5; }
        _block_6: { if (element == 6) goto _element_6; }
        _block_7: { if (element == 7) goto _element_7; }
        _block_8: { return; }
        _element_0: { auto& _2 = _stateUpCast (_state);
                      Voice___eventIn_struct_PitchBend (_2.m_voices_state[0], value);
                      return;
        }
        _element_1: { auto& _3 = _stateUpCast (_state);
                      Voice___eventIn_struct_PitchBend (_3.m_voices_state[1], value);
                      return;
        }
        _element_2: { auto& _4 = _stateUpCast (_state);
                      Voice___eventIn_struct_PitchBend (_4.m_voices_state[2], value);
                      return;
        }
        _element_3: { auto& _5 = _stateUpCast (_state);
                      Voice___eventIn_struct_PitchBend (_5.m_voices_state[3], value);
                      return;
        }
        _element_4: { auto& _6 = _stateUpCast (_state);
                      Voice___eventIn_struct_PitchBend (_6.m_voices_state[4], value);
                      return;
        }
        _element_5: { auto& _7 = _stateUpCast (_state);
                      Voice___eventIn_struct_PitchBend (_7.m_voices_state[5], value);
                      return;
        }
        _element_6: { auto& _8 = _stateUpCast (_state);
                      Voice___eventIn_struct_PitchBend (_8.m_voices_state[6], value);
                      return;
        }
        _element_7: { auto& _9 = _stateUpCast (_state);
                      Voice___eventIn_struct_PitchBend (_9.m_voices_state[7], value);
        }
    }

    void soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___voiceEventOut_struct_Pressure (soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State& _state, int32_t element, soul__note_events__Pressure value) noexcept
    {
        if (element == 0) goto _element_0;
        _block_1: { if (element == 1) goto _element_1; }
        _block_2: { if (element == 2) goto _element_2; }
        _block_3: { if (element == 3) goto _element_3; }
        _block_4: { if (element == 4) goto _element_4; }
        _block_5: { if (element == 5) goto _element_5; }
        _block_6: { if (element == 6) goto _element_6; }
        _block_7: { if (element == 7) goto _element_7; }
        _block_8: { return; }
        _element_0: { auto& _2 = _stateUpCast (_state);
                      Voice___eventIn_struct_Pressure (_2.m_voices_state[0], value);
                      return;
        }
        _element_1: { auto& _3 = _stateUpCast (_state);
                      Voice___eventIn_struct_Pressure (_3.m_voices_state[1], value);
                      return;
        }
        _element_2: { auto& _4 = _stateUpCast (_state);
                      Voice___eventIn_struct_Pressure (_4.m_voices_state[2], value);
                      return;
        }
        _element_3: { auto& _5 = _stateUpCast (_state);
                      Voice___eventIn_struct_Pressure (_5.m_voices_state[3], value);
                      return;
        }
        _element_4: { auto& _6 = _stateUpCast (_state);
                      Voice___eventIn_struct_Pressure (_6.m_voices_state[4], value);
                      return;
        }
        _element_5: { auto& _7 = _stateUpCast (_state);
                      Voice___eventIn_struct_Pressure (_7.m_voices_state[5], value);
                      return;
        }
        _element_6: { auto& _8 = _stateUpCast (_state);
                      Voice___eventIn_struct_Pressure (_8.m_voices_state[6], value);
                      return;
        }
        _element_7: { auto& _9 = _stateUpCast (_state);
                      Voice___eventIn_struct_Pressure (_9.m_voices_state[7], value);
        }
    }

    void soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___voiceEventOut_struct_Slide (soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___State& _state, int32_t element, soul__note_events__Slide value) noexcept
    {
        if (element == 0) goto _element_0;
        _block_1: { if (element == 1) goto _element_1; }
        _block_2: { if (element == 2) goto _element_2; }
        _block_3: { if (element == 3) goto _element_3; }
        _block_4: { if (element == 4) goto _element_4; }
        _block_5: { if (element == 5) goto _element_5; }
        _block_6: { if (element == 6) goto _element_6; }
        _block_7: { if (element == 7) goto _element_7; }
        _block_8: { return; }
        _element_0: { auto& _2 = _stateUpCast (_state);
                      Voice___eventIn_struct_Slide (_2.m_voices_state[0], value);
                      return;
        }
        _element_1: { auto& _3 = _stateUpCast (_state);
                      Voice___eventIn_struct_Slide (_3.m_voices_state[1], value);
                      return;
        }
        _element_2: { auto& _4 = _stateUpCast (_state);
                      Voice___eventIn_struct_Slide (_4.m_voices_state[2], value);
                      return;
        }
        _element_3: { auto& _5 = _stateUpCast (_state);
                      Voice___eventIn_struct_Slide (_5.m_voices_state[3], value);
                      return;
        }
        _element_4: { auto& _6 = _stateUpCast (_state);
                      Voice___eventIn_struct_Slide (_6.m_voices_state[4], value);
                      return;
        }
        _element_5: { auto& _7 = _stateUpCast (_state);
                      Voice___eventIn_struct_Slide (_7.m_voices_state[5], value);
                      return;
        }
        _element_6: { auto& _8 = _stateUpCast (_state);
                      Voice___eventIn_struct_Slide (_8.m_voices_state[6], value);
                      return;
        }
        _element_7: { auto& _9 = _stateUpCast (_state);
                      Voice___eventIn_struct_Slide (_9.m_voices_state[7], value);
        }
    }

    //==============================================================================
    void soul__mixers__DynamicMix___for__root__PadSynth_dryWetMixer__run (soul__mixers__DynamicMix___for__root__PadSynth_dryWetMixer___State& _state, soul__mixers__DynamicMix___for__root__PadSynth_dryWetMixer___IO& _io) noexcept
    {
        float out_value_out = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};

        out_value_out = 0;
        _2 = _io.m__in_in1;
        _3 = _io.m__in_in2;
        _4 = _io.m__in_mix;
        _5 = soul__intrinsics___lerp_specialised (_2, _3, _4 * 0.01f);
        out_value_out = out_value_out + _5;
        _state.m__resumePoint = 1;
        _io.m__out_out = out_value_out;
    }

    //==============================================================================
    void soul__mixers__DynamicMix___for__root__Voice_voiceOscillatorMix__run (soul__mixers__DynamicMix___for__root__Voice_voiceOscillatorMix___State& _state, soul__mixers__DynamicMix___for__root__Voice_voiceOscillatorMix___IO& _io) noexcept
    {
        float out_value_out = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};

        out_value_out = 0;
        _2 = _io.m__in_in1;
        _3 = _io.m__in_in2;
        _4 = _io.m__in_mix;
        _5 = soul__intrinsics___lerp_specialised (_2, _3, _4 * 0.01f);
        out_value_out = out_value_out + _5;
        _state.m__resumePoint = 1;
        _io.m__out_out = out_value_out;
    }

    //==============================================================================
    void soul__mixers__FixedSum___for__root__Reverb_dryWetMixer__run (soul__mixers__FixedSum___for__root__Reverb_dryWetMixer___State& _state, soul__mixers__FixedSum___for__root__Reverb_dryWetMixer___IO& _io) noexcept
    {
        float out_value_out = {}, _2 = {}, _3 = {};

        out_value_out = 0;
        _2 = _io.m__in_in1;
        _3 = _io.m__in_in2;
        out_value_out = out_value_out + ((_2 * 0.8f) + (_3 * 0.01485f));
        _state.m__resumePoint = 1;
        _io.m__out_out = out_value_out;
    }

    //==============================================================================
    NoteHandler__NoteHandler NoteHandler__create() noexcept
    {
        NoteHandler__NoteHandler r = {};

        r = ZeroInitialiser();
        r.m_currentVelocity = 0;
        r.m_currentNote = 0;
        r.m_activeNotes = 0;
        r.m_maxNotes = 16;
        return r;
    }

    void NoteHandler__addNote (NoteHandler__NoteHandler& this_, int32_t channel, float note, float velocity) noexcept
    {
        int32_t _2 = {};

        if (! (static_cast<int32_t> (this_.m_activeNotes) == 0)) goto _ifnot_0;
        _if_0: { this_.m_currentVelocity = velocity; }
        _ifnot_0: { if (! (static_cast<int32_t> (this_.m_activeNotes) < this_.m_maxNotes)) goto _ifnot_1; }
        _if_1: { this_.m_noteArray[this_.m_activeNotes].m_channel = channel;
                 this_.m_noteArray[this_.m_activeNotes].m_note = note;
                 this_.m_noteArray[this_.m_activeNotes].m_bendSemitones = 0;
                 _2 = this_.m_activeNotes;
                 this_.m_activeNotes = (_2 + 1) & 15;
        }
        _ifnot_1: { this_.m_currentNote = note;
                    this_.m_currentBendSemitones = 0;
                    this_.m_currentPressure = 0;
                    this_.m_currentSlide = 0;
        }
    }

    void NoteHandler__removeNote (NoteHandler__NoteHandler& this_, int32_t channel, float note) noexcept
    {
        int32_t readPoint = {}, writePoint = {}, _2 = {}, _3 = {};

        readPoint = 0;
        writePoint = 0;
        _loop_0: { if (! (static_cast<int32_t> (readPoint) < static_cast<int32_t> (this_.m_activeNotes))) goto _break_0; }
        _body_0: { if (! (this_.m_noteArray[readPoint].m_note != note)) goto _ifnot_0; }
        _if_0: { this_.m_noteArray[writePoint] = this_.m_noteArray[readPoint];
                 _2 = writePoint;
                 writePoint = (_2 + 1) & 15;
        }
        _ifnot_0: { _3 = readPoint;
                    readPoint = (_3 + 1) & 15;
                    goto _loop_0;
        }
        _break_0: { this_.m_activeNotes = writePoint;
                    if (! (static_cast<int32_t> (this_.m_activeNotes) > 0)) goto _ifnot_1;
        }
        _if_1: { this_.m_currentNote = this_.m_noteArray[_intrin_wrap (static_cast<int32_t> (static_cast<int32_t> (this_.m_activeNotes) - 1), 16) & 15].m_note;
                 this_.m_currentBendSemitones = this_.m_noteArray[_intrin_wrap (static_cast<int32_t> (static_cast<int32_t> (this_.m_activeNotes) - 1), 16) & 15].m_bendSemitones;
        }
        _ifnot_1: {}
    }

    void NoteHandler__addPitchBend (NoteHandler__NoteHandler& this_, int32_t channel, float bendSemitones) noexcept
    {
        int32_t i = {}, _2 = {};

        i = 0;
        _loop_0: { if (! (i < static_cast<int32_t> (this_.m_activeNotes))) goto _break_0; }
        _body_0: { if (! (this_.m_noteArray[_intrin_wrap (static_cast<int32_t> (i), 16) & 15].m_channel == channel)) goto _cont_0; }
        _if_0: { this_.m_noteArray[_intrin_wrap (static_cast<int32_t> (i), 16) & 15].m_bendSemitones = bendSemitones; }
        _cont_0: { _2 = i;
                   i = _2 + 1;
                   goto _loop_0;
        }
        _break_0: { if (! (static_cast<int32_t> (this_.m_activeNotes) > 0)) goto _ifnot_1; }
        _if_1: { this_.m_currentBendSemitones = this_.m_noteArray[_intrin_wrap (static_cast<int32_t> (static_cast<int32_t> (this_.m_activeNotes) - 1), 16) & 15].m_bendSemitones; }
        _ifnot_1: {}
    }

    void NoteHandler__addPressure (NoteHandler__NoteHandler& this_, int32_t channel, float pressure) noexcept
    {
        int32_t i = {}, _2 = {};

        i = 0;
        _loop_0: { if (! (i < static_cast<int32_t> (this_.m_activeNotes))) goto _break_0; }
        _body_0: { if (! (this_.m_noteArray[_intrin_wrap (static_cast<int32_t> (i), 16) & 15].m_channel == channel)) goto _cont_0; }
        _if_0: { this_.m_noteArray[_intrin_wrap (static_cast<int32_t> (i), 16) & 15].m_pressure = pressure; }
        _cont_0: { _2 = i;
                   i = _2 + 1;
                   goto _loop_0;
        }
        _break_0: { if (! (static_cast<int32_t> (this_.m_activeNotes) > 0)) goto _ifnot_1; }
        _if_1: { this_.m_currentPressure = this_.m_noteArray[_intrin_wrap (static_cast<int32_t> (static_cast<int32_t> (this_.m_activeNotes) - 1), 16) & 15].m_pressure; }
        _ifnot_1: {}
    }

    void NoteHandler__addSlide (NoteHandler__NoteHandler& this_, int32_t channel, float slide) noexcept
    {
        int32_t i = {}, _2 = {};

        i = 0;
        _loop_0: { if (! (i < static_cast<int32_t> (this_.m_activeNotes))) goto _break_0; }
        _body_0: { if (! (this_.m_noteArray[_intrin_wrap (static_cast<int32_t> (i), 16) & 15].m_channel == channel)) goto _cont_0; }
        _if_0: { this_.m_noteArray[_intrin_wrap (static_cast<int32_t> (i), 16) & 15].m_slide = slide; }
        _cont_0: { _2 = i;
                   i = _2 + 1;
                   goto _loop_0;
        }
        _break_0: { if (! (static_cast<int32_t> (this_.m_activeNotes) > 0)) goto _ifnot_1; }
        _if_1: { this_.m_currentSlide = this_.m_noteArray[_intrin_wrap (static_cast<int32_t> (static_cast<int32_t> (this_.m_activeNotes) - 1), 16) & 15].m_slide; }
        _ifnot_1: {}
    }

    float NoteHandler__getCurrentNote (NoteHandler__NoteHandler& this_) noexcept
    {
        return this_.m_currentNote + this_.m_currentBendSemitones;
    }

    float NoteHandler__getCurrentVelocity (NoteHandler__NoteHandler& this_) noexcept
    {
        return this_.m_currentVelocity;
    }

    float NoteHandler__getCurrentSlide (NoteHandler__NoteHandler& this_) noexcept
    {
        return this_.m_currentSlide;
    }

    bool NoteHandler__isNoteActive (NoteHandler__NoteHandler& this_) noexcept
    {
        return static_cast<int32_t> (this_.m_activeNotes) != 0;
    }

    //==============================================================================
    void PolyBlep___waveshape_f32 (PolyBlep___State& _state, float f) noexcept
    {
        _state.m_waveshapeValue = f * 0.01f;
    }

    void PolyBlep___detuneSemitones_f32 (PolyBlep___State& _state, float f) noexcept
    {
        float _2 = {};

        _state.m_detuneSemitonesValue = f;
        _2 = std::pow (2.0f, (_state.m_detuneSemitonesValue + _state.m_detuneValue) / 12.0f);
        _state.m_frequencyMultiplier = _2;
    }

    void PolyBlep___detune_f32 (PolyBlep___State& _state, float f) noexcept
    {
        float _2 = {};

        _state.m_detuneValue = f * 0.01f;
        _2 = std::pow (2.0f, (_state.m_detuneSemitonesValue + _state.m_detuneValue) / 12.0f);
        _state.m_frequencyMultiplier = _2;
    }

    float PolyBlep__blep (PolyBlep___State& _state, float t2) noexcept
    {
        float t = {};

        t = t2;
        if (! (t < _state.m_normalisedPhaseIncrement)) goto _ifnot_0;
        _if_0: { t = t / _state.m_normalisedPhaseIncrement;
                 return ((t + t) - (t * t)) - 1.0f;
        }
        _ifnot_0: { if (! (t > (1.0f - _state.m_normalisedPhaseIncrement))) goto _ifnot_1; }
        _if_1: { t = (t - 1.0f) / _state.m_normalisedPhaseIncrement;
                 return ((t * t) + (t + t)) + 1.0f;
        }
        _ifnot_1: { return 0; }
    }

    void PolyBlep__run (PolyBlep___State& _state, PolyBlep___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, normalisedPhase2 = {}, _3 = {}, _4 = {}, _5 = {}, squareValue = {}, _6 = {}, _7 = {}, _8 = {}, _T1 = {};
        float sawValue = {};

        out_value_audioOut = 0;
        _2 = _io.m__in_frequencyIn;
        _state.m_normalisedPhaseIncrement = (_2 * (1.0f / static_cast<float> ((sampleRate * 1.0)))) * _state.m_frequencyMultiplier;
        _loop_1: { if (! (_state.m_normalisedPhase >= 1.0f)) goto _break_1; }
        _body_1: { _state.m_normalisedPhase = _state.m_normalisedPhase - 1.0f;
                   goto _loop_1;
        }
        _break_1: { normalisedPhase2 = _state.m_normalisedPhase + 0.5f;
                    if (! (normalisedPhase2 > 1.0f)) goto _ifnot_0;
        }
        _if_0: { normalisedPhase2 = normalisedPhase2 - 1.0f; }
        _ifnot_0: { _3 = PolyBlep__blep (_state, _state.m_normalisedPhase);
                    sawValue = static_cast<float> (((2.0f * _state.m_normalisedPhase) - 1.0f) - _3);
                    _4 = PolyBlep__blep (_state, _state.m_normalisedPhase);
                    _5 = PolyBlep__blep (_state, normalisedPhase2);
                    squareValue = (-_4) + _5;
                    if (! (_state.m_normalisedPhase <= 0.5f)) goto _ternary_false_1;
        }
        _ternary_true_1: { _6 = -1.0f;
                           _T1 = _6;
                           goto _ternary_end_1;
        }
        _ternary_false_1: { _7 = 1.0f;
                            _T1 = _7;
        }
        _ternary_end_1: { _8 = _T1;
                          squareValue = squareValue + _8;
                          out_value_audioOut = out_value_audioOut + ((squareValue * _state.m_waveshapeValue) + static_cast<float> (sawValue * static_cast<float> (1.0f - _state.m_waveshapeValue)));
                          _state.m_normalisedPhase = _state.m_normalisedPhase + _state.m_normalisedPhaseIncrement;
                          _state.m__resumePoint = 1;
                          _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void PolyBlep___initialise (PolyBlep___State& _state) noexcept
    {
        _state.m_normalisedPhase = 0;
        _state.m_normalisedPhaseIncrement = 0;
        _state.m_waveshapeValue = 0;
        _state.m_detuneSemitonesValue = 0;
        _state.m_detuneValue = 0;
        _state.m_frequencyMultiplier = 1.0f;
    }

    //==============================================================================
    void ADSREnvelope___attack_f32 (ADSREnvelope___State& _state, float f) noexcept
    {
        float _2 = {};
        double seconds = {}, secondsToLinearStep = {};

        _2 = std::pow (f * 0.01f, 4.0f);
        seconds = static_cast<double> (0.001 + (9.999 * static_cast<double> (_2)));
        secondsToLinearStep = static_cast<double> (1.0 / static_cast<double> (static_cast<double> (static_cast<float> (static_cast<float> ((sampleRate * 1.0)))) * seconds));
        _state.m_attackRamp = static_cast<double> (secondsToLinearStep);
    }

    void ADSREnvelope___decay_f32 (ADSREnvelope___State& _state, float f) noexcept
    {
        float _2 = {};
        double seconds = {}, inverse = {}, secondsToMultiplier = {};
        double _3 = {};

        _2 = std::pow (f * 0.01f, 4.0f);
        seconds = static_cast<double> (0.001 + (9.999 * static_cast<double> (_2)));
        inverse = static_cast<double> (1.0 / static_cast<double> (static_cast<double> (static_cast<float> (static_cast<float> ((sampleRate * 1.0)))) * seconds));
        _3 = std::pow (0.0001, static_cast<double> (inverse));
        secondsToMultiplier = static_cast<double> (_3);
        _state.m_decayMultiplier = static_cast<double> (secondsToMultiplier);
    }

    void ADSREnvelope___sustain_f32 (ADSREnvelope___State& _state, float f) noexcept
    {
        _state.m_sustainLevel = static_cast<double> (f) * 0.009999999776482582;
    }

    void ADSREnvelope___release_f32 (ADSREnvelope___State& _state, float f) noexcept
    {
        float _2 = {};
        double seconds = {}, inverse = {}, secondsToMultiplier = {};
        double _3 = {};

        _2 = std::pow (f * 0.01f, 4.0f);
        seconds = static_cast<double> (0.001 + (9.999 * static_cast<double> (_2)));
        inverse = static_cast<double> (1.0 / static_cast<double> (static_cast<double> (static_cast<float> (static_cast<float> ((sampleRate * 1.0)))) * seconds));
        _3 = std::pow (0.0001, static_cast<double> (inverse));
        secondsToMultiplier = static_cast<double> (_3);
        _state.m_releaseMultiplier = static_cast<double> (secondsToMultiplier);
    }

    void ADSREnvelope___sensitivity_f32 (ADSREnvelope___State& _state, float f) noexcept
    {
        _state.m_velocitySensitivity = f * 0.01f;
    }

    void ADSREnvelope__calculateTargetValue (ADSREnvelope___State& _state, float noteVelocity) noexcept
    {
        float attenuation = {};
        float _2 = {};

        attenuation = static_cast<float> ((-12.0f * _state.m_velocitySensitivity) * (0.75f - noteVelocity));
        _2 = std::pow (10.0f, static_cast<float> (attenuation / 10.0f));
        _state.m_targetValue = static_cast<double> (_2);
    }

    void ADSREnvelope__run (ADSREnvelope___State& _state, ADSREnvelope___IO& _io) noexcept
    {
        float out_value_envelopeLevel = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {}, _6 = {};
        int32_t _resumePoint = {};
        double _7 = {};
        bool _8 = {}, _9 = {}, _10 = {}, _T2 = {};

        out_value_envelopeLevel = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _resume_point_1;
        _check_resume_point_2: { if (_resumePoint == 2) goto _loop_2; }
        _check_resume_point_3: { if (_resumePoint == 3) goto _loop_3; }
        _check_resume_point_4: { if (_resumePoint == 4) goto _body_0; }
        _block_0: { _state.m_value = 0; }
        _body_0: { _2 = _io.m__in_triggerLevel;
                   if (! (_2 > 0)) goto _ifnot_0;
        }
        _if_0: { _3 = _io.m__in_triggerLevel;
                 ADSREnvelope__calculateTargetValue (_state, _3);
        }
        _loop_1: { if (! (_state.m_value < _state.m_targetValue)) goto _break_1; }
        _body_1: { _state.m_value = _state.m_value + _state.m_attackRamp;
                   out_value_envelopeLevel = out_value_envelopeLevel + static_cast<float> (_state.m_value);
                   _state.m__resumePoint = 1;
                   goto _exit;
        }
        _resume_point_1: { _4 = _io.m__in_triggerLevel;
                           if (! (_4 <= 0)) goto _loop_1;
        }
        _break_1: { _7 = soul__intrinsics___min_specialised (_state.m_value, _state.m_targetValue);
                    _state.m_value = _7;
        }
        _loop_2: { if (! (_state.m_value > (_state.m_targetValue * _state.m_sustainLevel))) goto _ternary_false_2; }
        _ternary_true_2: { _5 = _io.m__in_triggerLevel;
                           _8 = _5 > 0;
                           _T2 = _8;
                           goto _ternary_end_2;
        }
        _ternary_false_2: { _9 = false;
                            _T2 = _9;
        }
        _ternary_end_2: { _10 = _T2;
                          if (! _10) goto _loop_3;
        }
        _body_2: { _state.m_value = _state.m_value * _state.m_decayMultiplier;
                   out_value_envelopeLevel = out_value_envelopeLevel + static_cast<float> (_state.m_value);
                   _state.m__resumePoint = 2;
                   goto _exit;
        }
        _loop_3: { _6 = _io.m__in_triggerLevel;
                   if (! (_6 > 0)) goto _ifnot_0;
        }
        _body_3: { out_value_envelopeLevel = out_value_envelopeLevel + static_cast<float> (_state.m_value);
                   _state.m__resumePoint = 3;
                   goto _exit;
        }
        _ifnot_0: { _state.m_value = _state.m_value * _state.m_releaseMultiplier;
                    out_value_envelopeLevel = out_value_envelopeLevel + static_cast<float> (_state.m_value);
                    _state.m__resumePoint = 4;
        }
        _exit: { _io.m__out_envelopeLevel = out_value_envelopeLevel; }
    }

    void ADSREnvelope___initialise (ADSREnvelope___State& _state) noexcept
    {
        _state.m_attackRamp = 0.01;
        _state.m_decayMultiplier = 0.995;
        _state.m_sustainLevel = 1.0;
        _state.m_releaseMultiplier = 0.99991;
        _state.m_velocitySensitivity = 0;
        _state.m_targetValue = 1.0;
    }

    //==============================================================================
    LowPassFilter__FilterTerms LowPassFilter__calcTermEntry (LowPassFilter___State& _state, float frequency, float resonance) noexcept
    {
        LowPassFilter__FilterTerms result = {};
        float r = {}, f = {}, c = {};
        float _2 = {}, _3 = {}, _4 = {};

        result = ZeroInitialiser();
        r = static_cast<float> (1.4f - (resonance * 0.0135f));
        _2 = soul__noteNumberToFrequency_2 (frequency);
        _3 = soul__intrinsics___clamp_specialised (_2, 0, 20000.0f);
        f = static_cast<float> (_3);
        _4 = std::tan ((3.1415927f * (10.0f + static_cast<float> (f))) / static_cast<float> ((sampleRate * 1.0)));
        c = static_cast<float> (1.0f / _4);
        result.m_a = 1.0f / ((1.0f + static_cast<float> (r * c)) + static_cast<float> (c * c));
        result.m_b = (2.0f * (1.0f - static_cast<float> (c * c))) * result.m_a;
        result.m_c = ((1.0f - static_cast<float> (r * c)) + static_cast<float> (c * c)) * result.m_a;
        return result;
    }

    void LowPassFilter__initialiseTerms (LowPassFilter___State& _state) noexcept
    {
        int32_t i = {}, j = {}, _2 = {}, _3 = {};
        LowPassFilter__FilterTerms _4 = {};

        i = 0;
        _loop_0: { if (! (i < 128)) goto _break_0; }
        _body_0: { j = 0; }
        _loop_1: { if (! (j <= 100)) goto _cont_0; }
        _body_1: { _4 = LowPassFilter__calcTermEntry (_state, static_cast<float> (i), static_cast<float> (j));
                   _state.m_precalcTerms.m_frequency[_intrin_wrap (static_cast<int32_t> (i), 128) & 127].m_resonance[_intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_intrin_wrap (static_cast<int32_t> (j), 101)), 101)), 101)] = _4;
                   _2 = j;
                   j = _2 + 1;
                   goto _loop_1;
        }
        _cont_0: { _3 = i;
                   i = _3 + 1;
                   goto _loop_0;
        }
        _break_0: {}
    }

    void LowPassFilter__updateFactors (LowPassFilter___State& _state, float cutoff, float resonance) noexcept
    {
        float _2 = {}, _3 = {};
        float cutoffInterpolation = {}, terms1_a = {}, terms1_b = {}, terms1_c = {}, terms2_a = {}, terms2_b = {}, terms2_c = {};
        int32_t resonanceEntry = {};
        int32_t cutoffEntry = {}, _4 = {};

        _2 = soul__intrinsics___clamp_specialised (cutoff, 0, 126.0f);
        cutoff = _2;
        _3 = soul__intrinsics___clamp_specialised (resonance, 0, 99.0f);
        resonance = _3;
        cutoffInterpolation = static_cast<float> (cutoff - static_cast<float> (static_cast<int32_t> (cutoff)));
        resonanceEntry = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (static_cast<int32_t> (_intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_intrin_wrap (static_cast<int32_t> (static_cast<int32_t> (resonance)), 100)), 100)), 100))), 100)),
         100);
        cutoffEntry = _intrin_wrap (static_cast<int32_t> (static_cast<int32_t> (cutoff)), 128) & 127;
        terms1_a = static_cast<float> (_state.m_precalcTerms.m_frequency[cutoffEntry].m_resonance[resonanceEntry].m_a);
        terms1_b = static_cast<float> (_state.m_precalcTerms.m_frequency[cutoffEntry].m_resonance[resonanceEntry].m_b);
        terms1_c = static_cast<float> (_state.m_precalcTerms.m_frequency[cutoffEntry].m_resonance[resonanceEntry].m_c);
        _4 = cutoffEntry;
        cutoffEntry = (_4 + 1) & 127;
        terms2_a = static_cast<float> (_state.m_precalcTerms.m_frequency[cutoffEntry].m_resonance[resonanceEntry].m_a);
        terms2_b = static_cast<float> (_state.m_precalcTerms.m_frequency[cutoffEntry].m_resonance[resonanceEntry].m_b);
        terms2_c = static_cast<float> (_state.m_precalcTerms.m_frequency[cutoffEntry].m_resonance[resonanceEntry].m_c);
        _state.m_factor_a1 = static_cast<float> (terms1_a + ((terms2_a - terms1_a) * cutoffInterpolation));
        _state.m_factor_a2 = 2.0f * _state.m_factor_a1;
        _state.m_factor_b1 = static_cast<float> (terms1_b + ((terms2_b - terms1_b) * cutoffInterpolation));
        _state.m_factor_b2 = static_cast<float> (terms1_c + ((terms2_c - terms1_c) * cutoffInterpolation));
    }

    void LowPassFilter__run (LowPassFilter___State& _state, LowPassFilter___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float in = {}, f = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _resume_point_1;
        _block_0: { LowPassFilter__initialiseTerms (_state);
                    _state.m_in_1 = 0;
                    _state.m_in_2 = 0;
                    _state.m_out_1 = 0;
                    _state.m_out_2 = 0;
        }
        _body_0: { _2 = _io.m__in_cutoffIn;
                   _3 = _io.m__in_resonanceIn;
                   LowPassFilter__updateFactors (_state, _2, _3);
                   _state.m_counter_1 = 64;
        }
        _loop_1: { if (! (_state.m_counter_1 > 0)) goto _body_0; }
        _body_1: { _4 = _io.m__in_audioIn;
                   in = static_cast<float> (_4);
                   f = static_cast<float> ((((_state.m_factor_a1 * static_cast<float> (in + static_cast<float> (_state.m_in_1))) + (_state.m_factor_a2 * _state.m_in_2)) - (_state.m_factor_b1 * _state.m_out_1)) - (_state.m_factor_b2 * _state.m_out_2));
                   _state.m_out_2 = _state.m_out_1;
                   _state.m_out_1 = static_cast<float> (f);
                   _state.m_in_2 = _state.m_in_1;
                   _state.m_in_1 = static_cast<float> (in);
                   _5 = _io.m__in_gainIn;
                   out_value_audioOut = out_value_audioOut + static_cast<float> ((f * static_cast<float> (_5)) * 0.2f);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
                   return;
        }
        _resume_point_1: { _state.m_counter_1 = _state.m_counter_1 - 1;
                           goto _loop_1;
        }
    }

    void LowPassFilter___initialise (LowPassFilter___State& _state) noexcept
    {
        _state.m_precalcTerms = ZeroInitialiser();
        _state.m_factor_a1 = 0;
        _state.m_factor_a2 = 0;
        _state.m_factor_b1 = 0;
        _state.m_factor_b2 = 0;
    }

    //==============================================================================
    void ParameterStream___parameterUpdate_f32 (ParameterStream___State& _state, float f) noexcept
    {
        _state.m_currentValue = f;
    }

    void ParameterStream__run (ParameterStream___State& _state, ParameterStream___IO& _io) noexcept
    {
        float out_value_audioOut = {};

        out_value_audioOut = 0;
        out_value_audioOut = out_value_audioOut + _state.m_currentValue;
        _state.m__resumePoint = 1;
        _io.m__out_audioOut = out_value_audioOut;
    }

    void ParameterStream___initialise (ParameterStream___State& _state) noexcept
    {
        _state.m_currentValue = 0;
    }

    //==============================================================================
    void Controller___eventIn_struct_NoteOn (Controller___State& _state, soul__note_events__NoteOn e) noexcept
    {
        NoteHandler__addNote (_state.m_noteHandler, e.m_channel, e.m_note, e.m_velocity);
    }

    void Controller___eventIn_struct_NoteOff (Controller___State& _state, soul__note_events__NoteOff e) noexcept
    {
        NoteHandler__removeNote (_state.m_noteHandler, e.m_channel, e.m_note);
    }

    void Controller___eventIn_struct_PitchBend (Controller___State& _state, soul__note_events__PitchBend e) noexcept
    {
        NoteHandler__addPitchBend (_state.m_noteHandler, e.m_channel, e.m_bendSemitones);
    }

    void Controller___eventIn_struct_Pressure (Controller___State& _state, soul__note_events__Pressure p) noexcept
    {
        NoteHandler__addPressure (_state.m_noteHandler, p.m_channel, p.m_pressure);
    }

    void Controller___eventIn_struct_Slide (Controller___State& _state, soul__note_events__Slide s) noexcept
    {
        NoteHandler__addSlide (_state.m_noteHandler, s.m_channel, s.m_slide);
    }

    void Controller__run (Controller___State& _state, Controller___IO& _io) noexcept
    {
        float out_value_note1_noteOut = {}, out_value_note1_frequencyOut = {}, out_value_note1_active = {}, out_value_note2_frequencyOut = {}, out_value_note1_slide = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {}, _6 = {}, _7 = {}, _8 = {}, _T2 = {};
        int32_t _resumePoint = {}, _9 = {};
        NoteHandler__NoteHandler _10 = {};
        float noteFrequency = {};
        bool _11 = {};

        out_value_note1_noteOut = 0;
        out_value_note1_frequencyOut = 0;
        out_value_note1_active = 0;
        out_value_note2_frequencyOut = 0;
        out_value_note1_slide = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_multiplier = 1.0f;
                    _state.m_sampleCount = 0;
                    _10 = NoteHandler__create();
                    _state.m_noteHandler = _10;
        }
        _body_0: { _9 = _state.m_sampleCount;
                   _state.m_sampleCount = _9 + 1;
                   if (! (_state.m_sampleCount == 1000)) goto _ifnot_0;
        }
        _if_0: { _state.m_sampleCount = 0;
                 _state.m_multiplier = _state.m_multiplier + 0.001f;
                 if (! (_state.m_multiplier >= 1.01f)) goto _ifnot_0;
        }
        _if_1: { _state.m_multiplier = 1.0f; }
        _ifnot_0: { _2 = NoteHandler__getCurrentNote (_state.m_noteHandler);
                    _3 = soul__noteNumberToFrequency_2 (_2);
                    noteFrequency = static_cast<float> (_3);
                    _4 = NoteHandler__getCurrentNote (_state.m_noteHandler);
                    out_value_note1_noteOut = out_value_note1_noteOut + _4;
                    out_value_note1_frequencyOut = out_value_note1_frequencyOut + static_cast<float> (noteFrequency);
                    out_value_note2_frequencyOut = out_value_note2_frequencyOut + static_cast<float> (noteFrequency * static_cast<float> (_state.m_multiplier));
                    _11 = NoteHandler__isNoteActive (_state.m_noteHandler);
                    if (! _11) goto _ternary_false_2;
        }
        _ternary_true_2: { _5 = NoteHandler__getCurrentVelocity (_state.m_noteHandler);
                           _T2 = _5;
                           goto _ternary_end_2;
        }
        _ternary_false_2: { _6 = 0;
                            _T2 = _6;
        }
        _ternary_end_2: { _7 = _T2;
                          out_value_note1_active = out_value_note1_active + _7;
                          _8 = NoteHandler__getCurrentSlide (_state.m_noteHandler);
                          out_value_note1_slide = out_value_note1_slide + _8;
                          _state.m__resumePoint = 1;
                          _io.m__out_note1_noteOut = out_value_note1_noteOut;
                          _io.m__out_note1_frequencyOut = out_value_note1_frequencyOut;
                          _io.m__out_note1_active = out_value_note1_active;
                          _io.m__out_note2_frequencyOut = out_value_note2_frequencyOut;
                          _io.m__out_note1_slide = out_value_note1_slide;
        }
    }

    void Controller___initialise (Controller___State& _state) noexcept
    {
        _state.m_noteHandler = ZeroInitialiser();
    }

    //==============================================================================
    void CutoffHandler__run (CutoffHandler___State& _state, CutoffHandler___IO& _io) noexcept
    {
        float out_value_cutoffOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {}, _6 = {}, _7 = {};

        out_value_cutoffOut = 0;
        _2 = _io.m__in_cutoffIn;
        _3 = _io.m__in_envelopeIn;
        _4 = _io.m__in_envelopeModIn;
        _5 = _io.m__in_noteIn;
        _6 = _io.m__in_trackModIn;
        _7 = _io.m__in_modIn;
        out_value_cutoffOut = out_value_cutoffOut + (((_2 + (_3 * _4)) + ((_5 - 60.0f) * _6)) + (_7 * 36.0f));
        _state.m__resumePoint = 1;
        _io.m__out_cutoffOut = out_value_cutoffOut;
    }

    //==============================================================================
    void Voice___initialise (Voice___State& _state) noexcept
    {
        _state.m_voiceController_state.m__arrayEntry = 0;
        _state.m_voiceController_state.m__sessionID = _state.m__sessionID;
        _state.m_voiceController_state.m__processorId = 1;
        Controller___initialise (_state.m_voiceController_state);
        _state.m_osc1_state.m__arrayEntry = 0;
        _state.m_osc1_state.m__sessionID = _state.m__sessionID;
        _state.m_osc1_state.m__processorId = 2;
        PolyBlep___initialise (_state.m_osc1_state);
        _state.m_osc2_state.m__arrayEntry = 0;
        _state.m_osc2_state.m__sessionID = _state.m__sessionID;
        _state.m_osc2_state.m__processorId = 3;
        PolyBlep_2___initialise (_state.m_osc2_state);
        _state.m_amplitudeEnvelope_state.m__arrayEntry = 0;
        _state.m_amplitudeEnvelope_state.m__sessionID = _state.m__sessionID;
        _state.m_amplitudeEnvelope_state.m__processorId = 4;
        ADSREnvelope___initialise (_state.m_amplitudeEnvelope_state);
        _state.m_cutoffEnvelope_state.m__arrayEntry = 0;
        _state.m_cutoffEnvelope_state.m__sessionID = _state.m__sessionID;
        _state.m_cutoffEnvelope_state.m__processorId = 5;
        ADSREnvelope_2___initialise (_state.m_cutoffEnvelope_state);
        _state.m_voiceOscillatorMix_state.m__arrayEntry = 0;
        _state.m_voiceOscillatorMix_state.m__sessionID = _state.m__sessionID;
        _state.m_voiceOscillatorMix_state.m__processorId = 6;
        _state.m_oscillatorMixParam_state.m__arrayEntry = 0;
        _state.m_oscillatorMixParam_state.m__sessionID = _state.m__sessionID;
        _state.m_oscillatorMixParam_state.m__processorId = 7;
        ParameterStream_3___initialise (_state.m_oscillatorMixParam_state);
        _state.m_cutoff_state.m__arrayEntry = 0;
        _state.m_cutoff_state.m__sessionID = _state.m__sessionID;
        _state.m_cutoff_state.m__processorId = 8;
        ParameterStream___initialise (_state.m_cutoff_state);
        _state.m_voiceCutoffHandler_state.m__arrayEntry = 0;
        _state.m_voiceCutoffHandler_state.m__sessionID = _state.m__sessionID;
        _state.m_voiceCutoffHandler_state.m__processorId = 9;
        _state.m_cutoffEnvMod_state.m__arrayEntry = 0;
        _state.m_cutoffEnvMod_state.m__sessionID = _state.m__sessionID;
        _state.m_cutoffEnvMod_state.m__processorId = 10;
        ParameterStream_4___initialise (_state.m_cutoffEnvMod_state);
        _state.m_cutoffKeytrackMod_state.m__arrayEntry = 0;
        _state.m_cutoffKeytrackMod_state.m__sessionID = _state.m__sessionID;
        _state.m_cutoffKeytrackMod_state.m__processorId = 11;
        ParameterStream_5___initialise (_state.m_cutoffKeytrackMod_state);
        _state.m_voiceLowPassFilter_state.m__arrayEntry = 0;
        _state.m_voiceLowPassFilter_state.m__sessionID = _state.m__sessionID;
        _state.m_voiceLowPassFilter_state.m__processorId = 12;
        LowPassFilter___initialise (_state.m_voiceLowPassFilter_state);
        _state.m_resonance_state.m__arrayEntry = 0;
        _state.m_resonance_state.m__sessionID = _state.m__sessionID;
        _state.m_resonance_state.m__processorId = 13;
        ParameterStream_2___initialise (_state.m_resonance_state);
    }

    void Voice__run (Voice___State& _state, Voice___IO& _io) noexcept
    {
        Controller___IO _2 = {};
        PolyBlep___IO _3 = {};
        PolyBlep_2___IO _4 = {};
        ParameterStream_3___IO _5 = {};
        soul__mixers__DynamicMix___for__root__Voice_voiceOscillatorMix___IO _6 = {};
        ParameterStream___IO _7 = {};
        ADSREnvelope_2___IO _8 = {};
        ParameterStream_4___IO _9 = {};
        ParameterStream_5___IO _10 = {};
        CutoffHandler___IO _11 = {};
        ParameterStream_2___IO _12 = {};
        ADSREnvelope___IO _13 = {};
        LowPassFilter___IO _14 = {};

        _2 = ZeroInitialiser();
        Controller__run (_state.m_voiceController_state, _2);
        _3 = ZeroInitialiser();
        _3.m__in_frequencyIn = _2.m__out_note1_frequencyOut;
        PolyBlep__run (_state.m_osc1_state, _3);
        _4 = ZeroInitialiser();
        _4.m__in_frequencyIn = _2.m__out_note2_frequencyOut;
        PolyBlep_2__run (_state.m_osc2_state, _4);
        _5 = ZeroInitialiser();
        ParameterStream_3__run (_state.m_oscillatorMixParam_state, _5);
        _6 = ZeroInitialiser();
        _6.m__in_in1 = _3.m__out_audioOut;
        _6.m__in_in2 = _4.m__out_audioOut;
        _6.m__in_mix = _5.m__out_audioOut;
        soul__mixers__DynamicMix___for__root__Voice_voiceOscillatorMix__run (_state.m_voiceOscillatorMix_state, _6);
        _7 = ZeroInitialiser();
        ParameterStream__run (_state.m_cutoff_state, _7);
        _8 = ZeroInitialiser();
        _8.m__in_triggerLevel = _2.m__out_note1_active;
        ADSREnvelope_2__run (_state.m_cutoffEnvelope_state, _8);
        _9 = ZeroInitialiser();
        ParameterStream_4__run (_state.m_cutoffEnvMod_state, _9);
        _10 = ZeroInitialiser();
        ParameterStream_5__run (_state.m_cutoffKeytrackMod_state, _10);
        _11 = ZeroInitialiser();
        _11.m__in_cutoffIn = _7.m__out_audioOut;
        _11.m__in_envelopeIn = _8.m__out_envelopeLevel;
        _11.m__in_envelopeModIn = _9.m__out_audioOut;
        _11.m__in_trackModIn = _10.m__out_audioOut;
        _11.m__in_noteIn = _2.m__out_note1_noteOut;
        _11.m__in_modIn = _2.m__out_note1_slide;
        CutoffHandler__run (_state.m_voiceCutoffHandler_state, _11);
        _12 = ZeroInitialiser();
        ParameterStream_2__run (_state.m_resonance_state, _12);
        _13 = ZeroInitialiser();
        _13.m__in_triggerLevel = _2.m__out_note1_active;
        ADSREnvelope__run (_state.m_amplitudeEnvelope_state, _13);
        _14 = ZeroInitialiser();
        _14.m__in_gainIn = _13.m__out_envelopeLevel;
        _14.m__in_cutoffIn = _11.m__out_cutoffOut;
        _14.m__in_resonanceIn = _12.m__out_audioOut;
        _14.m__in_audioIn = _6.m__out_out;
        LowPassFilter__run (_state.m_voiceLowPassFilter_state, _14);
        _io.m__out_audioOut = _14.m__out_audioOut;
    }

    void Voice___eventIn_struct_NoteOn (Voice___State& _state, soul__note_events__NoteOn event) noexcept
    {
        Controller___eventIn_struct_NoteOn (_state.m_voiceController_state, event);
    }

    void Voice___eventIn_struct_NoteOff (Voice___State& _state, soul__note_events__NoteOff event) noexcept
    {
        Controller___eventIn_struct_NoteOff (_state.m_voiceController_state, event);
    }

    void Voice___eventIn_struct_PitchBend (Voice___State& _state, soul__note_events__PitchBend event) noexcept
    {
        Controller___eventIn_struct_PitchBend (_state.m_voiceController_state, event);
    }

    void Voice___eventIn_struct_Pressure (Voice___State& _state, soul__note_events__Pressure event) noexcept
    {
        Controller___eventIn_struct_Pressure (_state.m_voiceController_state, event);
    }

    void Voice___eventIn_struct_Slide (Voice___State& _state, soul__note_events__Slide event) noexcept
    {
        Controller___eventIn_struct_Slide (_state.m_voiceController_state, event);
    }

    void Voice___cutoffParam_f32 (Voice___State& _state, float event) noexcept
    {
        ParameterStream___parameterUpdate_f32 (_state.m_cutoff_state, event);
    }

    void Voice___resonanceParam_f32 (Voice___State& _state, float event) noexcept
    {
        ParameterStream_2___parameterUpdate_f32 (_state.m_resonance_state, event);
    }

    void Voice___ampAttack_f32 (Voice___State& _state, float event) noexcept
    {
        ADSREnvelope___attack_f32 (_state.m_amplitudeEnvelope_state, event);
    }

    void Voice___ampDecay_f32 (Voice___State& _state, float event) noexcept
    {
        ADSREnvelope___decay_f32 (_state.m_amplitudeEnvelope_state, event);
    }

    void Voice___ampSustain_f32 (Voice___State& _state, float event) noexcept
    {
        ADSREnvelope___sustain_f32 (_state.m_amplitudeEnvelope_state, event);
    }

    void Voice___ampRelease_f32 (Voice___State& _state, float event) noexcept
    {
        ADSREnvelope___release_f32 (_state.m_amplitudeEnvelope_state, event);
    }

    void Voice___ampSensitivity_f32 (Voice___State& _state, float event) noexcept
    {
        ADSREnvelope___sensitivity_f32 (_state.m_amplitudeEnvelope_state, event);
    }

    void Voice___osc1waveshape_f32 (Voice___State& _state, float event) noexcept
    {
        PolyBlep___waveshape_f32 (_state.m_osc1_state, event);
    }

    void Voice___osc1detuneSemitones_f32 (Voice___State& _state, float event) noexcept
    {
        PolyBlep___detuneSemitones_f32 (_state.m_osc1_state, event);
    }

    void Voice___osc1detune_f32 (Voice___State& _state, float event) noexcept
    {
        PolyBlep___detune_f32 (_state.m_osc1_state, event);
    }

    void Voice___osc2waveshape_f32 (Voice___State& _state, float event) noexcept
    {
        PolyBlep_2___waveshape_f32 (_state.m_osc2_state, event);
    }

    void Voice___osc2detuneSemitones_f32 (Voice___State& _state, float event) noexcept
    {
        PolyBlep_2___detuneSemitones_f32 (_state.m_osc2_state, event);
    }

    void Voice___osc2detune_f32 (Voice___State& _state, float event) noexcept
    {
        PolyBlep_2___detune_f32 (_state.m_osc2_state, event);
    }

    void Voice___filterAttack_f32 (Voice___State& _state, float event) noexcept
    {
        ADSREnvelope_2___attack_f32 (_state.m_cutoffEnvelope_state, event);
    }

    void Voice___filterDecay_f32 (Voice___State& _state, float event) noexcept
    {
        ADSREnvelope_2___decay_f32 (_state.m_cutoffEnvelope_state, event);
    }

    void Voice___filterSustain_f32 (Voice___State& _state, float event) noexcept
    {
        ADSREnvelope_2___sustain_f32 (_state.m_cutoffEnvelope_state, event);
    }

    void Voice___filterRelease_f32 (Voice___State& _state, float event) noexcept
    {
        ADSREnvelope_2___release_f32 (_state.m_cutoffEnvelope_state, event);
    }

    void Voice___filterSensitivity_f32 (Voice___State& _state, float event) noexcept
    {
        ADSREnvelope_2___sensitivity_f32 (_state.m_cutoffEnvelope_state, event);
    }

    void Voice___oscillatorMix_f32 (Voice___State& _state, float event) noexcept
    {
        ParameterStream_3___parameterUpdate_f32 (_state.m_oscillatorMixParam_state, event);
    }

    void Voice___filterEnvMod_f32 (Voice___State& _state, float event) noexcept
    {
        ParameterStream_4___parameterUpdate_f32 (_state.m_cutoffEnvMod_state, event);
    }

    void Voice___filterCutoffTrack_f32 (Voice___State& _state, float event) noexcept
    {
        ParameterStream_5___parameterUpdate_f32 (_state.m_cutoffKeytrackMod_state, event);
    }

    //==============================================================================
    void Reverb___initialise (Reverb___State& _state) noexcept
    {
        _state.m_comb_1116_state.m__arrayEntry = 0;
        _state.m_comb_1116_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_1116_state.m__processorId = 14;
        _state.m_comb_1188_state.m__arrayEntry = 0;
        _state.m_comb_1188_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_1188_state.m__processorId = 15;
        _state.m_comb_1277_state.m__arrayEntry = 0;
        _state.m_comb_1277_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_1277_state.m__processorId = 16;
        _state.m_comb_1356_state.m__arrayEntry = 0;
        _state.m_comb_1356_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_1356_state.m__processorId = 17;
        _state.m_comb_1422_state.m__arrayEntry = 0;
        _state.m_comb_1422_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_1422_state.m__processorId = 18;
        _state.m_comb_1491_state.m__arrayEntry = 0;
        _state.m_comb_1491_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_1491_state.m__processorId = 19;
        _state.m_comb_1557_state.m__arrayEntry = 0;
        _state.m_comb_1557_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_1557_state.m__processorId = 20;
        _state.m_comb_1617_state.m__arrayEntry = 0;
        _state.m_comb_1617_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_1617_state.m__processorId = 21;
        _state.m_dryWetMixer_state.m__arrayEntry = 0;
        _state.m_dryWetMixer_state.m__sessionID = _state.m__sessionID;
        _state.m_dryWetMixer_state.m__processorId = 22;
        _state.m_allPass_225_state.m__arrayEntry = 0;
        _state.m_allPass_225_state.m__sessionID = _state.m__sessionID;
        _state.m_allPass_225_state.m__processorId = 23;
        _state.m_allPass_341_state.m__arrayEntry = 0;
        _state.m_allPass_341_state.m__sessionID = _state.m__sessionID;
        _state.m_allPass_341_state.m__processorId = 24;
        _state.m_allPass_441_state.m__arrayEntry = 0;
        _state.m_allPass_441_state.m__sessionID = _state.m__sessionID;
        _state.m_allPass_441_state.m__processorId = 25;
        _state.m_allPass_556_state.m__arrayEntry = 0;
        _state.m_allPass_556_state.m__sessionID = _state.m__sessionID;
        _state.m_allPass_556_state.m__processorId = 26;
    }

    void Reverb__run (Reverb___State& _state, Reverb___IO& _io) noexcept
    {
        float _2 = {};
        CombFilter___for__root__Reverb_comb_1116___IO _3 = {};
        CombFilter___for__root__Reverb_comb_1188___IO _4 = {};
        CombFilter___for__root__Reverb_comb_1277___IO _5 = {};
        CombFilter___for__root__Reverb_comb_1356___IO _6 = {};
        CombFilter___for__root__Reverb_comb_1422___IO _7 = {};
        CombFilter___for__root__Reverb_comb_1491___IO _8 = {};
        CombFilter___for__root__Reverb_comb_1557___IO _9 = {};
        CombFilter___for__root__Reverb_comb_1617___IO _10 = {};
        AllPassFilter___for__root__Reverb_allPass_225___IO _11 = {};
        AllPassFilter___for__root__Reverb_allPass_341___IO _12 = {};
        AllPassFilter___for__root__Reverb_allPass_441___IO _13 = {};
        AllPassFilter___for__root__Reverb_allPass_556___IO _14 = {};
        soul__mixers__FixedSum___for__root__Reverb_dryWetMixer___IO _15 = {};

        _2 = _io.m__in_audioIn;
        _3 = ZeroInitialiser();
        _3.m__in_audioIn = _2;
        CombFilter___for__root__Reverb_comb_1116__run (_state.m_comb_1116_state, _3);
        _4 = ZeroInitialiser();
        _4.m__in_audioIn = _2;
        CombFilter___for__root__Reverb_comb_1188__run (_state.m_comb_1188_state, _4);
        _5 = ZeroInitialiser();
        _5.m__in_audioIn = _2;
        CombFilter___for__root__Reverb_comb_1277__run (_state.m_comb_1277_state, _5);
        _6 = ZeroInitialiser();
        _6.m__in_audioIn = _2;
        CombFilter___for__root__Reverb_comb_1356__run (_state.m_comb_1356_state, _6);
        _7 = ZeroInitialiser();
        _7.m__in_audioIn = _2;
        CombFilter___for__root__Reverb_comb_1422__run (_state.m_comb_1422_state, _7);
        _8 = ZeroInitialiser();
        _8.m__in_audioIn = _2;
        CombFilter___for__root__Reverb_comb_1491__run (_state.m_comb_1491_state, _8);
        _9 = ZeroInitialiser();
        _9.m__in_audioIn = _2;
        CombFilter___for__root__Reverb_comb_1557__run (_state.m_comb_1557_state, _9);
        _10 = ZeroInitialiser();
        _10.m__in_audioIn = _2;
        CombFilter___for__root__Reverb_comb_1617__run (_state.m_comb_1617_state, _10);
        _11 = ZeroInitialiser();
        _11.m__in_audioIn = ((((((_3.m__out_audioOut + _4.m__out_audioOut) + _5.m__out_audioOut) + _6.m__out_audioOut) + _7.m__out_audioOut) + _8.m__out_audioOut) + _9.m__out_audioOut) + _10.m__out_audioOut;
        AllPassFilter___for__root__Reverb_allPass_225__run (_state.m_allPass_225_state, _11);
        _12 = ZeroInitialiser();
        _12.m__in_audioIn = _11.m__out_audioOut;
        AllPassFilter___for__root__Reverb_allPass_341__run (_state.m_allPass_341_state, _12);
        _13 = ZeroInitialiser();
        _13.m__in_audioIn = _12.m__out_audioOut;
        AllPassFilter___for__root__Reverb_allPass_441__run (_state.m_allPass_441_state, _13);
        _14 = ZeroInitialiser();
        _14.m__in_audioIn = _13.m__out_audioOut;
        AllPassFilter___for__root__Reverb_allPass_556__run (_state.m_allPass_556_state, _14);
        _15 = ZeroInitialiser();
        _15.m__in_in1 = _2;
        _15.m__in_in2 = _14.m__out_audioOut;
        soul__mixers__FixedSum___for__root__Reverb_dryWetMixer__run (_state.m_dryWetMixer_state, _15);
        _io.m__out_audioOut = _15.m__out_out;
    }

    //==============================================================================
    void Oscillators___initialise (Oscillators___State& _state) noexcept
    {
        _state.m_voices_state[0].m__arrayEntry = 0;
        _state.m_voices_state[0].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[0].m__processorId = 27;
        Voice___initialise (_state.m_voices_state[0]);
        _state.m_voices_state[1].m__arrayEntry = 1;
        _state.m_voices_state[1].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[1].m__processorId = 28;
        Voice___initialise (_state.m_voices_state[1]);
        _state.m_voices_state[2].m__arrayEntry = 2;
        _state.m_voices_state[2].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[2].m__processorId = 29;
        Voice___initialise (_state.m_voices_state[2]);
        _state.m_voices_state[3].m__arrayEntry = 3;
        _state.m_voices_state[3].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[3].m__processorId = 30;
        Voice___initialise (_state.m_voices_state[3]);
        _state.m_voices_state[4].m__arrayEntry = 4;
        _state.m_voices_state[4].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[4].m__processorId = 31;
        Voice___initialise (_state.m_voices_state[4]);
        _state.m_voices_state[5].m__arrayEntry = 5;
        _state.m_voices_state[5].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[5].m__processorId = 32;
        Voice___initialise (_state.m_voices_state[5]);
        _state.m_voices_state[6].m__arrayEntry = 6;
        _state.m_voices_state[6].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[6].m__processorId = 33;
        Voice___initialise (_state.m_voices_state[6]);
        _state.m_voices_state[7].m__arrayEntry = 7;
        _state.m_voices_state[7].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[7].m__processorId = 34;
        Voice___initialise (_state.m_voices_state[7]);
        _state.m_midiParser_state.m__arrayEntry = 0;
        _state.m_midiParser_state.m__sessionID = _state.m__sessionID;
        _state.m_midiParser_state.m__processorId = 35;
        _state.m__instance_Basic_state.m__arrayEntry = 0;
        _state.m__instance_Basic_state.m__sessionID = _state.m__sessionID;
        _state.m__instance_Basic_state.m__processorId = 36;
        soul__voice_allocators__Basic___for__root__Oscillators__instance_Basic___initialise (_state.m__instance_Basic_state);
    }

    void Oscillators__run (Oscillators___State& _state, Oscillators___IO& _io) noexcept
    {
        FixedArray<Voice___IO, 8> _2 = {};

        _2 = ZeroInitialiser();
        Voice__run (_state.m_voices_state[0], _2[0]);
        Voice__run (_state.m_voices_state[1], _2[1]);
        Voice__run (_state.m_voices_state[2], _2[2]);
        Voice__run (_state.m_voices_state[3], _2[3]);
        Voice__run (_state.m_voices_state[4], _2[4]);
        Voice__run (_state.m_voices_state[5], _2[5]);
        Voice__run (_state.m_voices_state[6], _2[6]);
        Voice__run (_state.m_voices_state[7], _2[7]);
        _io.m__out_audioOut = ((((((_2[0].m__out_audioOut + _2[1].m__out_audioOut) + _2[2].m__out_audioOut) + _2[3].m__out_audioOut) + _2[4].m__out_audioOut) + _2[5].m__out_audioOut) + _2[6].m__out_audioOut) + _2[7].m__out_audioOut;
    }

    void Oscillators___midiIn_struct_Message (Oscillators___State& _state, soul__midi__Message event) noexcept
    {
        soul__midi__MPEParser___parseMIDI_struct_Message (_state.m_midiParser_state, event);
    }

    void Oscillators___cutoffParam_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___cutoffParam_f32 (_state.m_voices_state[0], event);
        Voice___cutoffParam_f32 (_state.m_voices_state[1], event);
        Voice___cutoffParam_f32 (_state.m_voices_state[2], event);
        Voice___cutoffParam_f32 (_state.m_voices_state[3], event);
        Voice___cutoffParam_f32 (_state.m_voices_state[4], event);
        Voice___cutoffParam_f32 (_state.m_voices_state[5], event);
        Voice___cutoffParam_f32 (_state.m_voices_state[6], event);
        Voice___cutoffParam_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___resonanceParam_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___resonanceParam_f32 (_state.m_voices_state[0], event);
        Voice___resonanceParam_f32 (_state.m_voices_state[1], event);
        Voice___resonanceParam_f32 (_state.m_voices_state[2], event);
        Voice___resonanceParam_f32 (_state.m_voices_state[3], event);
        Voice___resonanceParam_f32 (_state.m_voices_state[4], event);
        Voice___resonanceParam_f32 (_state.m_voices_state[5], event);
        Voice___resonanceParam_f32 (_state.m_voices_state[6], event);
        Voice___resonanceParam_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___ampAttack_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___ampAttack_f32 (_state.m_voices_state[0], event);
        Voice___ampAttack_f32 (_state.m_voices_state[1], event);
        Voice___ampAttack_f32 (_state.m_voices_state[2], event);
        Voice___ampAttack_f32 (_state.m_voices_state[3], event);
        Voice___ampAttack_f32 (_state.m_voices_state[4], event);
        Voice___ampAttack_f32 (_state.m_voices_state[5], event);
        Voice___ampAttack_f32 (_state.m_voices_state[6], event);
        Voice___ampAttack_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___ampDecay_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___ampDecay_f32 (_state.m_voices_state[0], event);
        Voice___ampDecay_f32 (_state.m_voices_state[1], event);
        Voice___ampDecay_f32 (_state.m_voices_state[2], event);
        Voice___ampDecay_f32 (_state.m_voices_state[3], event);
        Voice___ampDecay_f32 (_state.m_voices_state[4], event);
        Voice___ampDecay_f32 (_state.m_voices_state[5], event);
        Voice___ampDecay_f32 (_state.m_voices_state[6], event);
        Voice___ampDecay_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___ampSustain_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___ampSustain_f32 (_state.m_voices_state[0], event);
        Voice___ampSustain_f32 (_state.m_voices_state[1], event);
        Voice___ampSustain_f32 (_state.m_voices_state[2], event);
        Voice___ampSustain_f32 (_state.m_voices_state[3], event);
        Voice___ampSustain_f32 (_state.m_voices_state[4], event);
        Voice___ampSustain_f32 (_state.m_voices_state[5], event);
        Voice___ampSustain_f32 (_state.m_voices_state[6], event);
        Voice___ampSustain_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___ampRelease_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___ampRelease_f32 (_state.m_voices_state[0], event);
        Voice___ampRelease_f32 (_state.m_voices_state[1], event);
        Voice___ampRelease_f32 (_state.m_voices_state[2], event);
        Voice___ampRelease_f32 (_state.m_voices_state[3], event);
        Voice___ampRelease_f32 (_state.m_voices_state[4], event);
        Voice___ampRelease_f32 (_state.m_voices_state[5], event);
        Voice___ampRelease_f32 (_state.m_voices_state[6], event);
        Voice___ampRelease_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___ampSensitivity_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___ampSensitivity_f32 (_state.m_voices_state[0], event);
        Voice___ampSensitivity_f32 (_state.m_voices_state[1], event);
        Voice___ampSensitivity_f32 (_state.m_voices_state[2], event);
        Voice___ampSensitivity_f32 (_state.m_voices_state[3], event);
        Voice___ampSensitivity_f32 (_state.m_voices_state[4], event);
        Voice___ampSensitivity_f32 (_state.m_voices_state[5], event);
        Voice___ampSensitivity_f32 (_state.m_voices_state[6], event);
        Voice___ampSensitivity_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___osc1waveshape_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___osc1waveshape_f32 (_state.m_voices_state[0], event);
        Voice___osc1waveshape_f32 (_state.m_voices_state[1], event);
        Voice___osc1waveshape_f32 (_state.m_voices_state[2], event);
        Voice___osc1waveshape_f32 (_state.m_voices_state[3], event);
        Voice___osc1waveshape_f32 (_state.m_voices_state[4], event);
        Voice___osc1waveshape_f32 (_state.m_voices_state[5], event);
        Voice___osc1waveshape_f32 (_state.m_voices_state[6], event);
        Voice___osc1waveshape_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___osc1detuneSemitones_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___osc1detuneSemitones_f32 (_state.m_voices_state[0], event);
        Voice___osc1detuneSemitones_f32 (_state.m_voices_state[1], event);
        Voice___osc1detuneSemitones_f32 (_state.m_voices_state[2], event);
        Voice___osc1detuneSemitones_f32 (_state.m_voices_state[3], event);
        Voice___osc1detuneSemitones_f32 (_state.m_voices_state[4], event);
        Voice___osc1detuneSemitones_f32 (_state.m_voices_state[5], event);
        Voice___osc1detuneSemitones_f32 (_state.m_voices_state[6], event);
        Voice___osc1detuneSemitones_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___osc1detune_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___osc1detune_f32 (_state.m_voices_state[0], event);
        Voice___osc1detune_f32 (_state.m_voices_state[1], event);
        Voice___osc1detune_f32 (_state.m_voices_state[2], event);
        Voice___osc1detune_f32 (_state.m_voices_state[3], event);
        Voice___osc1detune_f32 (_state.m_voices_state[4], event);
        Voice___osc1detune_f32 (_state.m_voices_state[5], event);
        Voice___osc1detune_f32 (_state.m_voices_state[6], event);
        Voice___osc1detune_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___osc2waveshape_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___osc2waveshape_f32 (_state.m_voices_state[0], event);
        Voice___osc2waveshape_f32 (_state.m_voices_state[1], event);
        Voice___osc2waveshape_f32 (_state.m_voices_state[2], event);
        Voice___osc2waveshape_f32 (_state.m_voices_state[3], event);
        Voice___osc2waveshape_f32 (_state.m_voices_state[4], event);
        Voice___osc2waveshape_f32 (_state.m_voices_state[5], event);
        Voice___osc2waveshape_f32 (_state.m_voices_state[6], event);
        Voice___osc2waveshape_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___osc2detuneSemitones_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___osc2detuneSemitones_f32 (_state.m_voices_state[0], event);
        Voice___osc2detuneSemitones_f32 (_state.m_voices_state[1], event);
        Voice___osc2detuneSemitones_f32 (_state.m_voices_state[2], event);
        Voice___osc2detuneSemitones_f32 (_state.m_voices_state[3], event);
        Voice___osc2detuneSemitones_f32 (_state.m_voices_state[4], event);
        Voice___osc2detuneSemitones_f32 (_state.m_voices_state[5], event);
        Voice___osc2detuneSemitones_f32 (_state.m_voices_state[6], event);
        Voice___osc2detuneSemitones_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___osc2detune_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___osc2detune_f32 (_state.m_voices_state[0], event);
        Voice___osc2detune_f32 (_state.m_voices_state[1], event);
        Voice___osc2detune_f32 (_state.m_voices_state[2], event);
        Voice___osc2detune_f32 (_state.m_voices_state[3], event);
        Voice___osc2detune_f32 (_state.m_voices_state[4], event);
        Voice___osc2detune_f32 (_state.m_voices_state[5], event);
        Voice___osc2detune_f32 (_state.m_voices_state[6], event);
        Voice___osc2detune_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___filterAttack_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___filterAttack_f32 (_state.m_voices_state[0], event);
        Voice___filterAttack_f32 (_state.m_voices_state[1], event);
        Voice___filterAttack_f32 (_state.m_voices_state[2], event);
        Voice___filterAttack_f32 (_state.m_voices_state[3], event);
        Voice___filterAttack_f32 (_state.m_voices_state[4], event);
        Voice___filterAttack_f32 (_state.m_voices_state[5], event);
        Voice___filterAttack_f32 (_state.m_voices_state[6], event);
        Voice___filterAttack_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___filterDecay_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___filterDecay_f32 (_state.m_voices_state[0], event);
        Voice___filterDecay_f32 (_state.m_voices_state[1], event);
        Voice___filterDecay_f32 (_state.m_voices_state[2], event);
        Voice___filterDecay_f32 (_state.m_voices_state[3], event);
        Voice___filterDecay_f32 (_state.m_voices_state[4], event);
        Voice___filterDecay_f32 (_state.m_voices_state[5], event);
        Voice___filterDecay_f32 (_state.m_voices_state[6], event);
        Voice___filterDecay_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___filterSustain_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___filterSustain_f32 (_state.m_voices_state[0], event);
        Voice___filterSustain_f32 (_state.m_voices_state[1], event);
        Voice___filterSustain_f32 (_state.m_voices_state[2], event);
        Voice___filterSustain_f32 (_state.m_voices_state[3], event);
        Voice___filterSustain_f32 (_state.m_voices_state[4], event);
        Voice___filterSustain_f32 (_state.m_voices_state[5], event);
        Voice___filterSustain_f32 (_state.m_voices_state[6], event);
        Voice___filterSustain_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___filterRelease_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___filterRelease_f32 (_state.m_voices_state[0], event);
        Voice___filterRelease_f32 (_state.m_voices_state[1], event);
        Voice___filterRelease_f32 (_state.m_voices_state[2], event);
        Voice___filterRelease_f32 (_state.m_voices_state[3], event);
        Voice___filterRelease_f32 (_state.m_voices_state[4], event);
        Voice___filterRelease_f32 (_state.m_voices_state[5], event);
        Voice___filterRelease_f32 (_state.m_voices_state[6], event);
        Voice___filterRelease_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___filterSensitivity_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___filterSensitivity_f32 (_state.m_voices_state[0], event);
        Voice___filterSensitivity_f32 (_state.m_voices_state[1], event);
        Voice___filterSensitivity_f32 (_state.m_voices_state[2], event);
        Voice___filterSensitivity_f32 (_state.m_voices_state[3], event);
        Voice___filterSensitivity_f32 (_state.m_voices_state[4], event);
        Voice___filterSensitivity_f32 (_state.m_voices_state[5], event);
        Voice___filterSensitivity_f32 (_state.m_voices_state[6], event);
        Voice___filterSensitivity_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___oscillatorMix_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___oscillatorMix_f32 (_state.m_voices_state[0], event);
        Voice___oscillatorMix_f32 (_state.m_voices_state[1], event);
        Voice___oscillatorMix_f32 (_state.m_voices_state[2], event);
        Voice___oscillatorMix_f32 (_state.m_voices_state[3], event);
        Voice___oscillatorMix_f32 (_state.m_voices_state[4], event);
        Voice___oscillatorMix_f32 (_state.m_voices_state[5], event);
        Voice___oscillatorMix_f32 (_state.m_voices_state[6], event);
        Voice___oscillatorMix_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___filterEnvMod_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___filterEnvMod_f32 (_state.m_voices_state[0], event);
        Voice___filterEnvMod_f32 (_state.m_voices_state[1], event);
        Voice___filterEnvMod_f32 (_state.m_voices_state[2], event);
        Voice___filterEnvMod_f32 (_state.m_voices_state[3], event);
        Voice___filterEnvMod_f32 (_state.m_voices_state[4], event);
        Voice___filterEnvMod_f32 (_state.m_voices_state[5], event);
        Voice___filterEnvMod_f32 (_state.m_voices_state[6], event);
        Voice___filterEnvMod_f32 (_state.m_voices_state[7], event);
    }

    void Oscillators___filterCutoffTrack_f32 (Oscillators___State& _state, float event) noexcept
    {
        Voice___filterCutoffTrack_f32 (_state.m_voices_state[0], event);
        Voice___filterCutoffTrack_f32 (_state.m_voices_state[1], event);
        Voice___filterCutoffTrack_f32 (_state.m_voices_state[2], event);
        Voice___filterCutoffTrack_f32 (_state.m_voices_state[3], event);
        Voice___filterCutoffTrack_f32 (_state.m_voices_state[4], event);
        Voice___filterCutoffTrack_f32 (_state.m_voices_state[5], event);
        Voice___filterCutoffTrack_f32 (_state.m_voices_state[6], event);
        Voice___filterCutoffTrack_f32 (_state.m_voices_state[7], event);
    }

    //==============================================================================
    void Delay___for__root__PadSynth__instance_Delay__run (Delay___for__root__PadSynth__instance_Delay___State& _state, Delay___for__root__PadSynth__instance_Delay___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_pos = 0; }
        _body_0: { _2 = _io.m__in_audioIn;
                   _state.m_delayBuffer[_state.m_pos] = (_state.m_delayBuffer[_state.m_pos] * 0.5f) + _2;
                   out_value_audioOut = out_value_audioOut + _state.m_delayBuffer[_state.m_pos];
                   _3 = _state.m_pos;
                   _state.m_pos = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 20000)), 20000);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void Delay___for__root__PadSynth__instance_Delay___initialise (Delay___for__root__PadSynth__instance_Delay___State& _state) noexcept
    {
        _state.m_delayBuffer = ZeroInitialiser();
    }

    //==============================================================================
    void PolyBlep_2___waveshape_f32 (PolyBlep_2___State& _state, float f) noexcept
    {
        _state.m_waveshapeValue = f * 0.01f;
    }

    void PolyBlep_2___detuneSemitones_f32 (PolyBlep_2___State& _state, float f) noexcept
    {
        float _2 = {};

        _state.m_detuneSemitonesValue = f;
        _2 = std::pow (2.0f, (_state.m_detuneSemitonesValue + _state.m_detuneValue) / 12.0f);
        _state.m_frequencyMultiplier = _2;
    }

    void PolyBlep_2___detune_f32 (PolyBlep_2___State& _state, float f) noexcept
    {
        float _2 = {};

        _state.m_detuneValue = f * 0.01f;
        _2 = std::pow (2.0f, (_state.m_detuneSemitonesValue + _state.m_detuneValue) / 12.0f);
        _state.m_frequencyMultiplier = _2;
    }

    float PolyBlep_2__blep (PolyBlep_2___State& _state, float t2) noexcept
    {
        float t = {};

        t = t2;
        if (! (t < _state.m_normalisedPhaseIncrement)) goto _ifnot_0;
        _if_0: { t = t / _state.m_normalisedPhaseIncrement;
                 return ((t + t) - (t * t)) - 1.0f;
        }
        _ifnot_0: { if (! (t > (1.0f - _state.m_normalisedPhaseIncrement))) goto _ifnot_1; }
        _if_1: { t = (t - 1.0f) / _state.m_normalisedPhaseIncrement;
                 return ((t * t) + (t + t)) + 1.0f;
        }
        _ifnot_1: { return 0; }
    }

    void PolyBlep_2__run (PolyBlep_2___State& _state, PolyBlep_2___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, normalisedPhase2 = {}, _3 = {}, _4 = {}, _5 = {}, squareValue = {}, _6 = {}, _7 = {}, _8 = {}, _T1 = {};
        float sawValue = {};

        out_value_audioOut = 0;
        _2 = _io.m__in_frequencyIn;
        _state.m_normalisedPhaseIncrement = (_2 * (1.0f / static_cast<float> ((sampleRate * 1.0)))) * _state.m_frequencyMultiplier;
        _loop_1: { if (! (_state.m_normalisedPhase >= 1.0f)) goto _break_1; }
        _body_1: { _state.m_normalisedPhase = _state.m_normalisedPhase - 1.0f;
                   goto _loop_1;
        }
        _break_1: { normalisedPhase2 = _state.m_normalisedPhase + 0.5f;
                    if (! (normalisedPhase2 > 1.0f)) goto _ifnot_0;
        }
        _if_0: { normalisedPhase2 = normalisedPhase2 - 1.0f; }
        _ifnot_0: { _3 = PolyBlep_2__blep (_state, _state.m_normalisedPhase);
                    sawValue = static_cast<float> (((2.0f * _state.m_normalisedPhase) - 1.0f) - _3);
                    _4 = PolyBlep_2__blep (_state, _state.m_normalisedPhase);
                    _5 = PolyBlep_2__blep (_state, normalisedPhase2);
                    squareValue = (-_4) + _5;
                    if (! (_state.m_normalisedPhase <= 0.5f)) goto _ternary_false_1;
        }
        _ternary_true_1: { _6 = -1.0f;
                           _T1 = _6;
                           goto _ternary_end_1;
        }
        _ternary_false_1: { _7 = 1.0f;
                            _T1 = _7;
        }
        _ternary_end_1: { _8 = _T1;
                          squareValue = squareValue + _8;
                          out_value_audioOut = out_value_audioOut + ((squareValue * _state.m_waveshapeValue) + static_cast<float> (sawValue * static_cast<float> (1.0f - _state.m_waveshapeValue)));
                          _state.m_normalisedPhase = _state.m_normalisedPhase + _state.m_normalisedPhaseIncrement;
                          _state.m__resumePoint = 1;
                          _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void PolyBlep_2___initialise (PolyBlep_2___State& _state) noexcept
    {
        _state.m_normalisedPhase = 0;
        _state.m_normalisedPhaseIncrement = 0;
        _state.m_waveshapeValue = 0;
        _state.m_detuneSemitonesValue = 0;
        _state.m_detuneValue = 0;
        _state.m_frequencyMultiplier = 1.0f;
    }

    //==============================================================================
    void ADSREnvelope_2___attack_f32 (ADSREnvelope_2___State& _state, float f) noexcept
    {
        float _2 = {};
        double seconds = {}, secondsToLinearStep = {};

        _2 = std::pow (f * 0.01f, 4.0f);
        seconds = static_cast<double> (0.001 + (9.999 * static_cast<double> (_2)));
        secondsToLinearStep = static_cast<double> (1.0 / static_cast<double> (static_cast<double> (static_cast<float> (static_cast<float> ((sampleRate * 1.0)))) * seconds));
        _state.m_attackRamp = static_cast<double> (secondsToLinearStep);
    }

    void ADSREnvelope_2___decay_f32 (ADSREnvelope_2___State& _state, float f) noexcept
    {
        float _2 = {};
        double seconds = {}, inverse = {}, secondsToMultiplier = {};
        double _3 = {};

        _2 = std::pow (f * 0.01f, 4.0f);
        seconds = static_cast<double> (0.001 + (9.999 * static_cast<double> (_2)));
        inverse = static_cast<double> (1.0 / static_cast<double> (static_cast<double> (static_cast<float> (static_cast<float> ((sampleRate * 1.0)))) * seconds));
        _3 = std::pow (0.0001, static_cast<double> (inverse));
        secondsToMultiplier = static_cast<double> (_3);
        _state.m_decayMultiplier = static_cast<double> (secondsToMultiplier);
    }

    void ADSREnvelope_2___sustain_f32 (ADSREnvelope_2___State& _state, float f) noexcept
    {
        _state.m_sustainLevel = static_cast<double> (f) * 0.009999999776482582;
    }

    void ADSREnvelope_2___release_f32 (ADSREnvelope_2___State& _state, float f) noexcept
    {
        float _2 = {};
        double seconds = {}, inverse = {}, secondsToMultiplier = {};
        double _3 = {};

        _2 = std::pow (f * 0.01f, 4.0f);
        seconds = static_cast<double> (0.001 + (9.999 * static_cast<double> (_2)));
        inverse = static_cast<double> (1.0 / static_cast<double> (static_cast<double> (static_cast<float> (static_cast<float> ((sampleRate * 1.0)))) * seconds));
        _3 = std::pow (0.0001, static_cast<double> (inverse));
        secondsToMultiplier = static_cast<double> (_3);
        _state.m_releaseMultiplier = static_cast<double> (secondsToMultiplier);
    }

    void ADSREnvelope_2___sensitivity_f32 (ADSREnvelope_2___State& _state, float f) noexcept
    {
        _state.m_velocitySensitivity = f * 0.01f;
    }

    void ADSREnvelope_2__calculateTargetValue (ADSREnvelope_2___State& _state, float noteVelocity) noexcept
    {
        float attenuation = {};
        float _2 = {};

        attenuation = static_cast<float> ((-12.0f * _state.m_velocitySensitivity) * (0.75f - noteVelocity));
        _2 = std::pow (10.0f, static_cast<float> (attenuation / 10.0f));
        _state.m_targetValue = static_cast<double> (_2);
    }

    void ADSREnvelope_2__run (ADSREnvelope_2___State& _state, ADSREnvelope_2___IO& _io) noexcept
    {
        float out_value_envelopeLevel = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {}, _6 = {};
        int32_t _resumePoint = {};
        double _7 = {};
        bool _8 = {}, _9 = {}, _10 = {}, _T2 = {};

        out_value_envelopeLevel = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _resume_point_1;
        _check_resume_point_2: { if (_resumePoint == 2) goto _loop_2; }
        _check_resume_point_3: { if (_resumePoint == 3) goto _loop_3; }
        _check_resume_point_4: { if (_resumePoint == 4) goto _body_0; }
        _block_0: { _state.m_value = 0; }
        _body_0: { _2 = _io.m__in_triggerLevel;
                   if (! (_2 > 0)) goto _ifnot_0;
        }
        _if_0: { _3 = _io.m__in_triggerLevel;
                 ADSREnvelope_2__calculateTargetValue (_state, _3);
        }
        _loop_1: { if (! (_state.m_value < _state.m_targetValue)) goto _break_1; }
        _body_1: { _state.m_value = _state.m_value + _state.m_attackRamp;
                   out_value_envelopeLevel = out_value_envelopeLevel + static_cast<float> (_state.m_value);
                   _state.m__resumePoint = 1;
                   goto _exit;
        }
        _resume_point_1: { _4 = _io.m__in_triggerLevel;
                           if (! (_4 <= 0)) goto _loop_1;
        }
        _break_1: { _7 = soul__intrinsics___min_specialised (_state.m_value, _state.m_targetValue);
                    _state.m_value = _7;
        }
        _loop_2: { if (! (_state.m_value > (_state.m_targetValue * _state.m_sustainLevel))) goto _ternary_false_2; }
        _ternary_true_2: { _5 = _io.m__in_triggerLevel;
                           _8 = _5 > 0;
                           _T2 = _8;
                           goto _ternary_end_2;
        }
        _ternary_false_2: { _9 = false;
                            _T2 = _9;
        }
        _ternary_end_2: { _10 = _T2;
                          if (! _10) goto _loop_3;
        }
        _body_2: { _state.m_value = _state.m_value * _state.m_decayMultiplier;
                   out_value_envelopeLevel = out_value_envelopeLevel + static_cast<float> (_state.m_value);
                   _state.m__resumePoint = 2;
                   goto _exit;
        }
        _loop_3: { _6 = _io.m__in_triggerLevel;
                   if (! (_6 > 0)) goto _ifnot_0;
        }
        _body_3: { out_value_envelopeLevel = out_value_envelopeLevel + static_cast<float> (_state.m_value);
                   _state.m__resumePoint = 3;
                   goto _exit;
        }
        _ifnot_0: { _state.m_value = _state.m_value * _state.m_releaseMultiplier;
                    out_value_envelopeLevel = out_value_envelopeLevel + static_cast<float> (_state.m_value);
                    _state.m__resumePoint = 4;
        }
        _exit: { _io.m__out_envelopeLevel = out_value_envelopeLevel; }
    }

    void ADSREnvelope_2___initialise (ADSREnvelope_2___State& _state) noexcept
    {
        _state.m_attackRamp = 0.01;
        _state.m_decayMultiplier = 0.995;
        _state.m_sustainLevel = 1.0;
        _state.m_releaseMultiplier = 0.99991;
        _state.m_velocitySensitivity = 0;
        _state.m_targetValue = 1.0;
    }

    //==============================================================================
    void ParameterStream_2___parameterUpdate_f32 (ParameterStream_2___State& _state, float f) noexcept
    {
        _state.m_currentValue = f;
    }

    void ParameterStream_2__run (ParameterStream_2___State& _state, ParameterStream_2___IO& _io) noexcept
    {
        float out_value_audioOut = {};

        out_value_audioOut = 0;
        out_value_audioOut = out_value_audioOut + _state.m_currentValue;
        _state.m__resumePoint = 1;
        _io.m__out_audioOut = out_value_audioOut;
    }

    void ParameterStream_2___initialise (ParameterStream_2___State& _state) noexcept
    {
        _state.m_currentValue = 0;
    }

    //==============================================================================
    void ParameterStream_3___parameterUpdate_f32 (ParameterStream_3___State& _state, float f) noexcept
    {
        _state.m_currentValue = f;
    }

    void ParameterStream_3__run (ParameterStream_3___State& _state, ParameterStream_3___IO& _io) noexcept
    {
        float out_value_audioOut = {};

        out_value_audioOut = 0;
        out_value_audioOut = out_value_audioOut + _state.m_currentValue;
        _state.m__resumePoint = 1;
        _io.m__out_audioOut = out_value_audioOut;
    }

    void ParameterStream_3___initialise (ParameterStream_3___State& _state) noexcept
    {
        _state.m_currentValue = 0;
    }

    //==============================================================================
    void ParameterStream_4___parameterUpdate_f32 (ParameterStream_4___State& _state, float f) noexcept
    {
        _state.m_currentValue = f;
    }

    void ParameterStream_4__run (ParameterStream_4___State& _state, ParameterStream_4___IO& _io) noexcept
    {
        float out_value_audioOut = {};

        out_value_audioOut = 0;
        out_value_audioOut = out_value_audioOut + _state.m_currentValue;
        _state.m__resumePoint = 1;
        _io.m__out_audioOut = out_value_audioOut;
    }

    void ParameterStream_4___initialise (ParameterStream_4___State& _state) noexcept
    {
        _state.m_currentValue = 0;
    }

    //==============================================================================
    void ParameterStream_5___parameterUpdate_f32 (ParameterStream_5___State& _state, float f) noexcept
    {
        _state.m_currentValue = f;
    }

    void ParameterStream_5__run (ParameterStream_5___State& _state, ParameterStream_5___IO& _io) noexcept
    {
        float out_value_audioOut = {};

        out_value_audioOut = 0;
        out_value_audioOut = out_value_audioOut + _state.m_currentValue;
        _state.m__resumePoint = 1;
        _io.m__out_audioOut = out_value_audioOut;
    }

    void ParameterStream_5___initialise (ParameterStream_5___State& _state) noexcept
    {
        _state.m_currentValue = 0;
    }

    //==============================================================================
    void AllPassFilter___for__root__Reverb_allPass_225__run (AllPassFilter___for__root__Reverb_allPass_225___State& _state, AllPassFilter___for__root__Reverb_allPass_225___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {};
        int32_t _resumePoint = {};
        float bufferedValue = {};
        int32_t _4 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_buffer = ZeroInitialiser();
                    _state.m_bufferIndex = 0;
        }
        _body_0: { bufferedValue = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _4 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_4 + 1), 225)), 225);
                   _2 = _io.m__in_audioIn;
                   _state.m_buffer[_4] = _2 + static_cast<float> (bufferedValue * 0.5f);
                   _3 = _io.m__in_audioIn;
                   out_value_audioOut = out_value_audioOut + static_cast<float> (bufferedValue - static_cast<float> (_3));
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    //==============================================================================
    void AllPassFilter___for__root__Reverb_allPass_341__run (AllPassFilter___for__root__Reverb_allPass_341___State& _state, AllPassFilter___for__root__Reverb_allPass_341___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {};
        int32_t _resumePoint = {};
        float bufferedValue = {};
        int32_t _4 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_buffer = ZeroInitialiser();
                    _state.m_bufferIndex = 0;
        }
        _body_0: { bufferedValue = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _4 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_4 + 1), 341)), 341);
                   _2 = _io.m__in_audioIn;
                   _state.m_buffer[_4] = _2 + static_cast<float> (bufferedValue * 0.5f);
                   _3 = _io.m__in_audioIn;
                   out_value_audioOut = out_value_audioOut + static_cast<float> (bufferedValue - static_cast<float> (_3));
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    //==============================================================================
    void AllPassFilter___for__root__Reverb_allPass_441__run (AllPassFilter___for__root__Reverb_allPass_441___State& _state, AllPassFilter___for__root__Reverb_allPass_441___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {};
        int32_t _resumePoint = {};
        float bufferedValue = {};
        int32_t _4 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_buffer = ZeroInitialiser();
                    _state.m_bufferIndex = 0;
        }
        _body_0: { bufferedValue = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _4 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_4 + 1), 441)), 441);
                   _2 = _io.m__in_audioIn;
                   _state.m_buffer[_4] = _2 + static_cast<float> (bufferedValue * 0.5f);
                   _3 = _io.m__in_audioIn;
                   out_value_audioOut = out_value_audioOut + static_cast<float> (bufferedValue - static_cast<float> (_3));
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    //==============================================================================
    void AllPassFilter___for__root__Reverb_allPass_556__run (AllPassFilter___for__root__Reverb_allPass_556___State& _state, AllPassFilter___for__root__Reverb_allPass_556___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {};
        int32_t _resumePoint = {};
        float bufferedValue = {};
        int32_t _4 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_buffer = ZeroInitialiser();
                    _state.m_bufferIndex = 0;
        }
        _body_0: { bufferedValue = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _4 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_4 + 1), 556)), 556);
                   _2 = _io.m__in_audioIn;
                   _state.m_buffer[_4] = _2 + static_cast<float> (bufferedValue * 0.5f);
                   _3 = _io.m__in_audioIn;
                   out_value_audioOut = out_value_audioOut + static_cast<float> (bufferedValue - static_cast<float> (_3));
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    //==============================================================================
    void CombFilter___for__root__Reverb_comb_1116__run (CombFilter___for__root__Reverb_comb_1116___State& _state, CombFilter___for__root__Reverb_comb_1116___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_buffer = ZeroInitialiser();
                    _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_last = static_cast<float> ((out * 0.8f) + static_cast<float> (_state.m_last * 0.2f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 1116)), 1116);
                   _2 = _io.m__in_audioIn;
                   _state.m_buffer[_3] = _2 + (_state.m_last * 0.95f);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    //==============================================================================
    void CombFilter___for__root__Reverb_comb_1188__run (CombFilter___for__root__Reverb_comb_1188___State& _state, CombFilter___for__root__Reverb_comb_1188___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_buffer = ZeroInitialiser();
                    _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_last = static_cast<float> ((out * 0.8f) + static_cast<float> (_state.m_last * 0.2f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 1188)), 1188);
                   _2 = _io.m__in_audioIn;
                   _state.m_buffer[_3] = _2 + (_state.m_last * 0.95f);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    //==============================================================================
    void CombFilter___for__root__Reverb_comb_1277__run (CombFilter___for__root__Reverb_comb_1277___State& _state, CombFilter___for__root__Reverb_comb_1277___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_buffer = ZeroInitialiser();
                    _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_last = static_cast<float> ((out * 0.8f) + static_cast<float> (_state.m_last * 0.2f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 1277)), 1277);
                   _2 = _io.m__in_audioIn;
                   _state.m_buffer[_3] = _2 + (_state.m_last * 0.95f);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    //==============================================================================
    void CombFilter___for__root__Reverb_comb_1356__run (CombFilter___for__root__Reverb_comb_1356___State& _state, CombFilter___for__root__Reverb_comb_1356___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_buffer = ZeroInitialiser();
                    _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_last = static_cast<float> ((out * 0.8f) + static_cast<float> (_state.m_last * 0.2f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 1356)), 1356);
                   _2 = _io.m__in_audioIn;
                   _state.m_buffer[_3] = _2 + (_state.m_last * 0.95f);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    //==============================================================================
    void CombFilter___for__root__Reverb_comb_1422__run (CombFilter___for__root__Reverb_comb_1422___State& _state, CombFilter___for__root__Reverb_comb_1422___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_buffer = ZeroInitialiser();
                    _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_last = static_cast<float> ((out * 0.8f) + static_cast<float> (_state.m_last * 0.2f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 1422)), 1422);
                   _2 = _io.m__in_audioIn;
                   _state.m_buffer[_3] = _2 + (_state.m_last * 0.95f);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    //==============================================================================
    void CombFilter___for__root__Reverb_comb_1491__run (CombFilter___for__root__Reverb_comb_1491___State& _state, CombFilter___for__root__Reverb_comb_1491___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_buffer = ZeroInitialiser();
                    _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_last = static_cast<float> ((out * 0.8f) + static_cast<float> (_state.m_last * 0.2f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 1491)), 1491);
                   _2 = _io.m__in_audioIn;
                   _state.m_buffer[_3] = _2 + (_state.m_last * 0.95f);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    //==============================================================================
    void CombFilter___for__root__Reverb_comb_1557__run (CombFilter___for__root__Reverb_comb_1557___State& _state, CombFilter___for__root__Reverb_comb_1557___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_buffer = ZeroInitialiser();
                    _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_last = static_cast<float> ((out * 0.8f) + static_cast<float> (_state.m_last * 0.2f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 1557)), 1557);
                   _2 = _io.m__in_audioIn;
                   _state.m_buffer[_3] = _2 + (_state.m_last * 0.95f);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    //==============================================================================
    void CombFilter___for__root__Reverb_comb_1617__run (CombFilter___for__root__Reverb_comb_1617___State& _state, CombFilter___for__root__Reverb_comb_1617___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_buffer = ZeroInitialiser();
                    _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_last = static_cast<float> ((out * 0.8f) + static_cast<float> (_state.m_last * 0.2f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 1617)), 1617);
                   _2 = _io.m__in_audioIn;
                   _state.m_buffer[_3] = _2 + (_state.m_last * 0.95f);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    //==============================================================================
    int32_t _internal___minInt32 (int32_t a, int32_t b) noexcept
    {
        if (! (a < b)) goto _moreThan;
        _lessThan: { return a; }
        _moreThan: { return b; }
    }

    int32_t _internal___wrapInt32 (int32_t n, int32_t range) noexcept
    {
        int32_t _2 = {};

        if (! (n == 0)) goto _notEquals;
        _equals: { return 0; }
        _notEquals: { _2 = n % range;
                      if (! (_2 < 0)) goto _moduloPositive;
        }
        _moduloNegative: { return _2 + range; }
        _moduloPositive: { return _2; }
    }


    #if __clang__
     #pragma clang diagnostic pop
    #elif defined(__GNUC__)
     #pragma GCC diagnostic pop
    #elif defined(_MSC_VER)
     #pragma warning (pop)
    #endif

    //==============================================================================
    // The program contains no string literals, so this function should never be called
    static constexpr const char* lookupStringLiteral (int32_t)  { return {}; }

    //==============================================================================
    _State state = {}, initialisedState;

    double sampleRate = 1.0;
    uint32_t framesToAdvance = 0;
    uint64_t totalFramesElapsed = 0;
};

