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

#define SOUL_CPP_ASSERT(x) assert (x)

#ifndef SOUL_CPP_ASSERT
 #define SOUL_CPP_ASSERT(x)
#endif

//==============================================================================
// Generated from graph 'Reverb', source file: Reverb.soul
//
class Reverb
{
public:
    Reverb() = default;
    ~Reverb() = default;

    //==============================================================================
    template <typename Type, int32_t size> struct Vector;
    template <typename Type, int32_t size> struct FixedArray;
    template <typename Type> struct DynamicArray;

    static constexpr uint32_t maxBlockSize  = 1024;
    static constexpr uint32_t latency       = 0;

    struct _RenderStats;
    struct _SparseStreamStatus;
    struct _Stream_in_f32_1024;
    struct _Event_in_f32_1;
    struct _Stream_out_vec_2_f32_1024;
    struct ReverbParameterProcessorParam___State;
    struct ParameterRamp___for__root__Reverb_dryGainParameterRamp___State;
    struct ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___State;
    struct ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___State;
    struct ParameterRamp___for__root__Reverb_dampingParameterRamp___State;
    struct ParameterRamp___for__root__Reverb_feedbackParameterRamp___State;
    struct Mixer___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8___State;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1___State;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2___State;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3___State;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4___State;
    struct ReverbChannel___for__root__Reverb_reverbChannelLeft___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7___State;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8___State;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1___State;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2___State;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3___State;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4___State;
    struct ReverbChannel___for__root__Reverb_reverbChannelRight___State;
    struct _State;
    struct Mixer___IO;
    struct ParameterRamp___for__root__Reverb_dryGainParameterRamp___IO;
    struct ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___IO;
    struct ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___IO;
    struct ParameterRamp___for__root__Reverb_dampingParameterRamp___IO;
    struct ParameterRamp___for__root__Reverb_feedbackParameterRamp___IO;
    struct ReverbChannel___for__root__Reverb_reverbChannelLeft___IO;
    struct ReverbChannel___for__root__Reverb_reverbChannelRight___IO;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1___IO;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2___IO;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3___IO;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8___IO;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1___IO;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2___IO;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3___IO;
    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7___IO;
    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8___IO;
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

    static constexpr uint32_t numAudioInputChannels  = 1;
    static constexpr uint32_t numAudioOutputChannels = 2;

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
        std::array<const FloatType*, 1> inputChannels;
        std::array<FloatType*, 2> outputChannels;
        MIDIMessageArray  incomingMIDI;
        uint32_t          numFrames = 0;
    };

    //==============================================================================
    template <typename FloatType>
    void render (RenderContext<FloatType> context)
    {
        uint32_t startFrame = 0;

        while (startFrame < context.numFrames)
        {
            auto framesRemaining = context.numFrames - startFrame;
            auto numFramesToDo = framesRemaining < maxBlockSize ? framesRemaining : maxBlockSize;
            prepare (numFramesToDo);

            copyToInterleaved (_getInputFrameArrayRef_audioIn (state).elements, &context.inputChannels[0], startFrame, numFramesToDo);

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

    void setNextInputStreamFrames_audioIn (const float* frames, uint32_t numFramesToUse)
    {
        auto& buffer = _getInputFrameArrayRef_audioIn (state);

        for (uint32_t i = 0; i < numFramesToUse; ++i)
            buffer[static_cast<int> (i)] = frames[i];
    }

    void setNextInputStreamSparseFrames_audioIn (float targetFrameValue, uint32_t numFramesToReachValue)
    {
        _setSparseInputTarget_audioIn (state, targetFrameValue, (int32_t) numFramesToReachValue);
    }

    void addInputEvent_roomSize (float eventValue)
    {
        _addInputEvent_roomSize_f32 (state, eventValue);
    }

    void addInputEvent_damping (float eventValue)
    {
        _addInputEvent_damping_f32 (state, eventValue);
    }

    void addInputEvent_wetLevel (float eventValue)
    {
        _addInputEvent_wetLevel_f32 (state, eventValue);
    }

    void addInputEvent_dryLevel (float eventValue)
    {
        _addInputEvent_dryLevel_f32 (state, eventValue);
    }

    void addInputEvent_width (float eventValue)
    {
        _addInputEvent_width_f32 (state, eventValue);
    }

    DynamicArray<const Vector<float, 2>> getOutputStreamFrames_audioOut()
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
            { "audioIn",  "in:audioIn",  EndpointType::stream, "float32", 1, ""                                                                                                       },
            { "roomSize", "in:roomSize", EndpointType::event,  "float32", 0, "{ \"name\": \"Room Size\", \"max\": 100, \"init\": 80, \"text\": \"tiny|small|medium|large|hall\" }"    },
            { "damping",  "in:damping",  EndpointType::event,  "float32", 0, "{ \"min\": 0, \"max\": 100, \"init\": 50, \"name\": \"Damping Factor\", \"unit\": \"%\", \"step\": 1 }" },
            { "wetLevel", "in:wetLevel", EndpointType::event,  "float32", 0, "{ \"min\": 0, \"max\": 100, \"init\": 33, \"name\": \"Wet Level\", \"unit\": \"%\", \"step\": 1 }"      },
            { "dryLevel", "in:dryLevel", EndpointType::event,  "float32", 0, "{ \"min\": 0, \"max\": 100, \"init\": 40, \"name\": \"Dry Level\", \"unit\": \"%\", \"step\": 1 }"      },
            { "width",    "in:width",    EndpointType::event,  "float32", 0, "{ \"min\": 0, \"max\": 100, \"init\": 100, \"name\": \"Width\", \"unit\": \"%\", \"step\": 1 }"         }
        };
    }

    std::vector<EndpointDetails> getOutputEndpoints() const
    {
        return
        {
            { "audioOut", "out:audioOut", EndpointType::stream, "float32<2>", 2, "" }
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

    static constexpr bool      hasMIDIInput = false;
    static constexpr uint32_t  numParameters = 5;

    static constexpr const ParameterProperties parameters[] =
    {
        {  "roomSize",  "Room Size",       "",   0.0f,  100.0f,  25.0f,  80.0f,   true,  false,  false,  "",  "tiny|small|medium|large|hall"  },
        {  "damping",   "Damping Factor",  "%",  0.0f,  100.0f,  1.0f,   50.0f,   true,  false,  false,  "",  ""                              },
        {  "wetLevel",  "Wet Level",       "%",  0.0f,  100.0f,  1.0f,   33.0f,   true,  false,  false,  "",  ""                              },
        {  "dryLevel",  "Dry Level",       "%",  0.0f,  100.0f,  1.0f,   40.0f,   true,  false,  false,  "",  ""                              },
        {  "width",     "Width",           "%",  0.0f,  100.0f,  1.0f,   100.0f,  true,  false,  false,  "",  ""                              }
    };

    static std::vector<ParameterProperties> getParameterProperties() { return { parameters,  parameters  + numParameters }; }

    static constexpr uint32_t numInputBuses  = 1;
    static constexpr uint32_t numOutputBuses = 1;

    static constexpr const AudioBus inputBuses[numInputBuses] = { { "audioIn", 1 } };

    static constexpr const AudioBus outputBuses[numOutputBuses] = { { "audioOut", 2 } };

    static std::vector<AudioBus> getInputBuses()  { return { inputBuses,  inputBuses  + numInputBuses }; }
    static std::vector<AudioBus> getOutputBuses() { return { outputBuses,  outputBuses  + numOutputBuses }; }

    std::vector<Parameter> createParameterList()
    {
        return
        {
            {  parameters[0],  80.0f,   [this] (float v) { addInputEvent_roomSize (v); }  },
            {  parameters[1],  50.0f,   [this] (float v) { addInputEvent_damping (v); }   },
            {  parameters[2],  33.0f,   [this] (float v) { addInputEvent_wetLevel (v); }  },
            {  parameters[3],  40.0f,   [this] (float v) { addInputEvent_dryLevel (v); }  },
            {  parameters[4],  100.0f,  [this] (float v) { addInputEvent_width (v); }     }
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
        FixedArray<int32_t, 6> m_rampArray;
    };

    struct _Stream_in_f32_1024
    {
        FixedArray<float, 1024> m_buffer;
        float m_currentSparseValue, m_targetSparseValue, m_perFrameIncrement;
        int32_t m_numSparseFramesToRender, m_constantFilledFrames;
        bool m_sparseStreamActive;
    };

    struct _Event_in_f32_1
    {
        int32_t m_numFrames;
    };

    struct _Stream_out_vec_2_f32_1024
    {
        FixedArray<Vector<float, 2>, 1024> m_buffer;
    };

    struct ReverbParameterProcessorParam___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_roomSizeScaled, m_dampingScaled, m_wetLevelScaled, m_dryLevelScaled, m_widthScaled;
    };

    struct ParameterRamp___for__root__Reverb_dryGainParameterRamp___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_targetValue, m_currentValue, m_rampIncrement;
        int32_t m_rampSamples;
    };

    struct ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_targetValue, m_currentValue, m_rampIncrement;
        int32_t m_rampSamples;
    };

    struct ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_targetValue, m_currentValue, m_rampIncrement;
        int32_t m_rampSamples;
    };

    struct ParameterRamp___for__root__Reverb_dampingParameterRamp___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_targetValue, m_currentValue, m_rampIncrement;
        int32_t m_rampSamples;
    };

    struct ParameterRamp___for__root__Reverb_feedbackParameterRamp___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_targetValue, m_currentValue, m_rampIncrement;
        int32_t m_rampSamples;
    };

    struct Mixer___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1116> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1188> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1277> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1356> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1422> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1491> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1557> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1617> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 225> m_buffer;
        int32_t m_bufferIndex;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 341> m_buffer;
        int32_t m_bufferIndex;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 441> m_buffer;
        int32_t m_bufferIndex;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 556> m_buffer;
        int32_t m_bufferIndex;
    };

    struct ReverbChannel___for__root__Reverb_reverbChannelLeft___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1___State m_comb_1_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2___State m_comb_2_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3___State m_comb_3_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4___State m_comb_4_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5___State m_comb_5_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6___State m_comb_6_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7___State m_comb_7_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8___State m_comb_8_state;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1___State m_allpass_1_state;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2___State m_allpass_2_state;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3___State m_allpass_3_state;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4___State m_allpass_4_state;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1139> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1211> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1300> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1379> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1445> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1514> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1580> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 1640> m_buffer;
        int32_t m_bufferIndex;
        float m_last;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 248> m_buffer;
        int32_t m_bufferIndex;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 364> m_buffer;
        int32_t m_bufferIndex;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 464> m_buffer;
        int32_t m_bufferIndex;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        FixedArray<float, 579> m_buffer;
        int32_t m_bufferIndex;
    };

    struct ReverbChannel___for__root__Reverb_reverbChannelRight___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1___State m_comb_1_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2___State m_comb_2_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3___State m_comb_3_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4___State m_comb_4_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5___State m_comb_5_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6___State m_comb_6_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7___State m_comb_7_state;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8___State m_comb_8_state;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1___State m_allpass_1_state;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2___State m_allpass_2_state;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3___State m_allpass_3_state;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4___State m_allpass_4_state;
    };

    struct _State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId, m__framesToAdvance;
        _RenderStats m__renderStats;
        _SparseStreamStatus m__sparseStreamStatus;
        _Stream_in_f32_1024 m__in_audioIn;
        _Event_in_f32_1 m__in_roomSize, m__in_damping, m__in_wetLevel, m__in_dryLevel, m__in_width;
        _Stream_out_vec_2_f32_1024 m__out_audioOut;
        ReverbParameterProcessorParam___State m_ReverbParameterProcessorParam_state;
        ParameterRamp___for__root__Reverb_dryGainParameterRamp___State m_dryGainParameterRamp_state;
        ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___State m_wetGain1ParameterRamp_state;
        ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___State m_wetGain2ParameterRamp_state;
        ParameterRamp___for__root__Reverb_dampingParameterRamp___State m_dampingParameterRamp_state;
        ParameterRamp___for__root__Reverb_feedbackParameterRamp___State m_feedbackParameterRamp_state;
        Mixer___State m_Mixer_state;
        ReverbChannel___for__root__Reverb_reverbChannelLeft___State m_reverbChannelLeft_state;
        ReverbChannel___for__root__Reverb_reverbChannelRight___State m_reverbChannelRight_state;
    };

    struct Mixer___IO
    {
        float m__in_audioInDry, m__in_dryIn, m__in_wet1In, m__in_wet2In, m__in_audioInLeftWet, m__in_audioInRightWet;
        Vector<float, 2> m__out_audioOut;
    };

    struct ParameterRamp___for__root__Reverb_dryGainParameterRamp___IO
    {
        float m__out_parameterOut;
    };

    struct ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___IO
    {
        float m__out_parameterOut;
    };

    struct ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___IO
    {
        float m__out_parameterOut;
    };

    struct ParameterRamp___for__root__Reverb_dampingParameterRamp___IO
    {
        float m__out_parameterOut;
    };

    struct ParameterRamp___for__root__Reverb_feedbackParameterRamp___IO
    {
        float m__out_parameterOut;
    };

    struct ReverbChannel___for__root__Reverb_reverbChannelLeft___IO
    {
        float m__in_audioIn, m__in_damping, m__in_feedback, m__out_audioOut;
    };

    struct ReverbChannel___for__root__Reverb_reverbChannelRight___IO
    {
        float m__in_audioIn, m__in_damping, m__in_feedback, m__out_audioOut;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4___IO
    {
        float m__in_audioIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
    };

    struct CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8___IO
    {
        float m__in_audioIn, m__in_dampingIn, m__in_feedbackLevelIn, m__out_audioOut;
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
        ParameterRamp___for__root__Reverb_dampingParameterRamp___IO _4 = {};
        ParameterRamp___for__root__Reverb_feedbackParameterRamp___IO _5 = {};
        ReverbChannel___for__root__Reverb_reverbChannelLeft___IO _6 = {};
        ReverbChannel___for__root__Reverb_reverbChannelRight___IO _7 = {};
        ParameterRamp___for__root__Reverb_dryGainParameterRamp___IO _8 = {};
        ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___IO _9 = {};
        ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___IO _10 = {};
        Mixer___IO _11 = {};

        _2 = _internal___minInt32 (1024, maxFrames);
        _updateRampingStreams (_state, _2);
        _state.m__frameCount = 0;
        _main_loop_check: { if (! (_state.m__frameCount < _2)) goto _exit; }
        _main_loop_body: { _3 = _readFromStream_struct__Stream_in_f32_1024 (_state.m__in_audioIn, _state.m__frameCount);
                           _4 = ZeroInitialiser();
                           ParameterRamp___for__root__Reverb_dampingParameterRamp__run (_state.m_dampingParameterRamp_state, _4);
                           _5 = ZeroInitialiser();
                           ParameterRamp___for__root__Reverb_feedbackParameterRamp__run (_state.m_feedbackParameterRamp_state, _5);
                           _6 = ZeroInitialiser();
                           _6.m__in_audioIn = _3;
                           _6.m__in_damping = _4.m__out_parameterOut;
                           _6.m__in_feedback = _5.m__out_parameterOut;
                           ReverbChannel___for__root__Reverb_reverbChannelLeft__run (_state.m_reverbChannelLeft_state, _6);
                           _7 = ZeroInitialiser();
                           _7.m__in_audioIn = _3;
                           _7.m__in_damping = _4.m__out_parameterOut;
                           _7.m__in_feedback = _5.m__out_parameterOut;
                           ReverbChannel___for__root__Reverb_reverbChannelRight__run (_state.m_reverbChannelRight_state, _7);
                           _8 = ZeroInitialiser();
                           ParameterRamp___for__root__Reverb_dryGainParameterRamp__run (_state.m_dryGainParameterRamp_state, _8);
                           _9 = ZeroInitialiser();
                           ParameterRamp___for__root__Reverb_wetGain1ParameterRamp__run (_state.m_wetGain1ParameterRamp_state, _9);
                           _10 = ZeroInitialiser();
                           ParameterRamp___for__root__Reverb_wetGain2ParameterRamp__run (_state.m_wetGain2ParameterRamp_state, _10);
                           _11 = ZeroInitialiser();
                           _11.m__in_audioInDry = _3;
                           _11.m__in_dryIn = _8.m__out_parameterOut;
                           _11.m__in_wet1In = _9.m__out_parameterOut;
                           _11.m__in_wet2In = _10.m__out_parameterOut;
                           _11.m__in_audioInLeftWet = _6.m__out_audioOut;
                           _11.m__in_audioInRightWet = _7.m__out_audioOut;
                           Mixer__run (_state.m_Mixer_state, _11);
                           _writeToStream_struct__Stream_out_vec_2_f32_1024 (_state.m__out_audioOut, _state.m__frameCount, _11.m__out_audioOut);
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
        _state.m_ReverbParameterProcessorParam_state.m__arrayEntry = 0;
        _state.m_ReverbParameterProcessorParam_state.m__sessionID = _state.m__sessionID;
        _state.m_ReverbParameterProcessorParam_state.m__processorId = 25;
        ReverbParameterProcessorParam___initialise (_state.m_ReverbParameterProcessorParam_state);
        _state.m_dryGainParameterRamp_state.m__arrayEntry = 0;
        _state.m_dryGainParameterRamp_state.m__sessionID = _state.m__sessionID;
        _state.m_dryGainParameterRamp_state.m__processorId = 26;
        ParameterRamp___for__root__Reverb_dryGainParameterRamp___initialise (_state.m_dryGainParameterRamp_state);
        _state.m_wetGain1ParameterRamp_state.m__arrayEntry = 0;
        _state.m_wetGain1ParameterRamp_state.m__sessionID = _state.m__sessionID;
        _state.m_wetGain1ParameterRamp_state.m__processorId = 27;
        ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___initialise (_state.m_wetGain1ParameterRamp_state);
        _state.m_wetGain2ParameterRamp_state.m__arrayEntry = 0;
        _state.m_wetGain2ParameterRamp_state.m__sessionID = _state.m__sessionID;
        _state.m_wetGain2ParameterRamp_state.m__processorId = 28;
        ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___initialise (_state.m_wetGain2ParameterRamp_state);
        _state.m_dampingParameterRamp_state.m__arrayEntry = 0;
        _state.m_dampingParameterRamp_state.m__sessionID = _state.m__sessionID;
        _state.m_dampingParameterRamp_state.m__processorId = 29;
        ParameterRamp___for__root__Reverb_dampingParameterRamp___initialise (_state.m_dampingParameterRamp_state);
        _state.m_feedbackParameterRamp_state.m__arrayEntry = 0;
        _state.m_feedbackParameterRamp_state.m__sessionID = _state.m__sessionID;
        _state.m_feedbackParameterRamp_state.m__processorId = 30;
        ParameterRamp___for__root__Reverb_feedbackParameterRamp___initialise (_state.m_feedbackParameterRamp_state);
        _state.m_Mixer_state.m__arrayEntry = 0;
        _state.m_Mixer_state.m__sessionID = _state.m__sessionID;
        _state.m_Mixer_state.m__processorId = 31;
        _state.m_reverbChannelLeft_state.m__arrayEntry = 0;
        _state.m_reverbChannelLeft_state.m__sessionID = _state.m__sessionID;
        _state.m_reverbChannelLeft_state.m__processorId = 32;
        ReverbChannel___for__root__Reverb_reverbChannelLeft___initialise (_state.m_reverbChannelLeft_state);
        _state.m_reverbChannelRight_state.m__arrayEntry = 0;
        _state.m_reverbChannelRight_state.m__sessionID = _state.m__sessionID;
        _state.m_reverbChannelRight_state.m__processorId = 33;
        ReverbChannel___for__root__Reverb_reverbChannelRight___initialise (_state.m_reverbChannelRight_state);
    }

    void _addInputEvent_roomSize_f32 (_State& _state, const float& event) noexcept
    {
        ReverbParameterProcessorParam___roomSize_f32 (_state.m_ReverbParameterProcessorParam_state, event);
    }

    void _addInputEvent_damping_f32 (_State& _state, const float& event) noexcept
    {
        ReverbParameterProcessorParam___damping_f32 (_state.m_ReverbParameterProcessorParam_state, event);
    }

    void _addInputEvent_wetLevel_f32 (_State& _state, const float& event) noexcept
    {
        ReverbParameterProcessorParam___wetLevel_f32 (_state.m_ReverbParameterProcessorParam_state, event);
    }

    void _addInputEvent_dryLevel_f32 (_State& _state, const float& event) noexcept
    {
        ReverbParameterProcessorParam___dryLevel_f32 (_state.m_ReverbParameterProcessorParam_state, event);
    }

    void _addInputEvent_width_f32 (_State& _state, const float& event) noexcept
    {
        ReverbParameterProcessorParam___width_f32 (_state.m_ReverbParameterProcessorParam_state, event);
    }

    FixedArray<float, 1024>& _getInputFrameArrayRef_audioIn (_State& _state) noexcept
    {
        return _state.m__in_audioIn.m_buffer;
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

    void _setSparseInputTarget_audioIn (_State& _state, const float& targetValue, int32_t framesToReachTarget) noexcept
    {
        if (_state.m__in_audioIn.m_sparseStreamActive) goto _block_1;
        _block_0: { _addRampingStream (_state.m__sparseStreamStatus, 0); }
        _block_1: { _setSparseStream_struct__Stream_in_f32_1024 (_state.m__in_audioIn, targetValue, framesToReachTarget); }
    }

    FixedArray<Vector<float, 2>, 1024>& _getOutputFrameArrayRef_audioOut (_State& state) noexcept
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
        if (! (streamId == 0)) goto _exit;
        _case_0: { rampComplete = _applySparseStreamData_struct__Stream_in_f32_1024 (_state.m__in_audioIn, framesToRender); }
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

    void _writeToStream_struct__Stream_out_vec_2_f32_1024 (_Stream_out_vec_2_f32_1024& stream, int32_t writePos, Vector<float, 2> value) noexcept
    {
        stream.m_buffer[writePos] = value;
    }

    //==============================================================================
    float soul__intrinsics___abs_specialised (float n) noexcept
    {
        float _2 = {}, _3 = {}, _4 = {}, _T0 = {};

        if (! (n < 0)) goto _ternary_false_0;
        _ternary_true_0: { _2 = -n;
                           _T0 = _2;
                           goto _ternary_end_0;
        }
        _ternary_false_0: { _3 = n;
                            _T0 = _3;
        }
        _ternary_end_0: { _4 = _T0;
                          return _4;
        }
    }

    //==============================================================================
    void Mixer__run (Mixer___State& _state, Mixer___IO& _io) noexcept
    {
        Vector<float, 2> out_value_audioOut = {}, _2 = {};
        float _3 = {}, _4 = {}, _5 = {}, _6 = {}, _7 = {}, _8 = {}, _9 = {}, _10 = {}, _11 = {}, _12 = {};
        float left = {}, right = {};

        out_value_audioOut = ZeroInitialiser();
        _3 = _io.m__in_audioInLeftWet;
        _4 = _io.m__in_wet1In;
        _5 = _io.m__in_audioInRightWet;
        _6 = _io.m__in_wet2In;
        left = static_cast<float> ((_3 * _4) + (_5 * _6));
        _7 = _io.m__in_audioInRightWet;
        _8 = _io.m__in_wet1In;
        _9 = _io.m__in_audioInLeftWet;
        _10 = _io.m__in_wet2In;
        right = static_cast<float> ((_7 * _8) + (_9 * _10));
        _2 = ZeroInitialiser();
        _2[0] = static_cast<float> (left);
        _2[1] = static_cast<float> (right);
        _11 = _io.m__in_audioInDry;
        _12 = _io.m__in_dryIn;
        out_value_audioOut = out_value_audioOut + (_2 + Vector<float, 2> (_11 * _12));
        _state.m__resumePoint = 1;
        _io.m__out_audioOut = out_value_audioOut;
    }

    //==============================================================================
    static _State& _stateUpCast (ReverbParameterProcessorParam___State& s)
    {
        auto offset = static_cast<int32_t> (offsetof (_State, m_ReverbParameterProcessorParam_state));
        return *reinterpret_cast<_State*> (reinterpret_cast<char*> (&s) - offset);
    }

    //==============================================================================
    void ReverbParameterProcessorParam___roomSize_f32 (ReverbParameterProcessorParam___State& _state, float newValue) noexcept
    {
        _state.m_roomSizeScaled = newValue / 100.0f;
        ReverbParameterProcessorParam__onUpdate (_state);
    }

    void ReverbParameterProcessorParam___damping_f32 (ReverbParameterProcessorParam___State& _state, float newValue) noexcept
    {
        _state.m_dampingScaled = newValue / 100.0f;
        ReverbParameterProcessorParam__onUpdate (_state);
    }

    void ReverbParameterProcessorParam___wetLevel_f32 (ReverbParameterProcessorParam___State& _state, float newValue) noexcept
    {
        _state.m_wetLevelScaled = newValue / 100.0f;
        ReverbParameterProcessorParam__onUpdate (_state);
    }

    void ReverbParameterProcessorParam___dryLevel_f32 (ReverbParameterProcessorParam___State& _state, float newValue) noexcept
    {
        _state.m_dryLevelScaled = newValue / 100.0f;
        ReverbParameterProcessorParam__onUpdate (_state);
    }

    void ReverbParameterProcessorParam___width_f32 (ReverbParameterProcessorParam___State& _state, float newValue) noexcept
    {
        _state.m_widthScaled = newValue / 100.0f;
        ReverbParameterProcessorParam__onUpdate (_state);
    }

    void ReverbParameterProcessorParam__onUpdate (ReverbParameterProcessorParam___State& _state) noexcept
    {
        ReverbParameterProcessorParam___dryGainOut_f32 (_state, _state.m_dryLevelScaled * 2.0f);
        ReverbParameterProcessorParam___wetGain1Out_f32 (_state, ((0.5f * _state.m_wetLevelScaled) * 3.0f) * (1.0f + _state.m_widthScaled));
        ReverbParameterProcessorParam___wetGain2Out_f32 (_state, ((0.5f * _state.m_wetLevelScaled) * 3.0f) * (1.0f - _state.m_widthScaled));
        ReverbParameterProcessorParam___dampingOut_f32 (_state, _state.m_dampingScaled * 0.4f);
        ReverbParameterProcessorParam___feedbackOut_f32 (_state, (_state.m_roomSizeScaled * 0.28f) + 0.7f);
    }

    void ReverbParameterProcessorParam___initialise (ReverbParameterProcessorParam___State& _state) noexcept
    {
        _state.m_roomSizeScaled = 0.5f;
        _state.m_dampingScaled = 0.5f;
        _state.m_wetLevelScaled = 0.33f;
        _state.m_dryLevelScaled = 0.4f;
        _state.m_widthScaled = 1.0f;
    }

    void ReverbParameterProcessorParam___dryGainOut_f32 (ReverbParameterProcessorParam___State& _state, float value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        ParameterRamp___for__root__Reverb_dryGainParameterRamp___updateParameter_f32 (_2.m_dryGainParameterRamp_state, value);
    }

    void ReverbParameterProcessorParam___wetGain1Out_f32 (ReverbParameterProcessorParam___State& _state, float value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___updateParameter_f32 (_2.m_wetGain1ParameterRamp_state, value);
    }

    void ReverbParameterProcessorParam___wetGain2Out_f32 (ReverbParameterProcessorParam___State& _state, float value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___updateParameter_f32 (_2.m_wetGain2ParameterRamp_state, value);
    }

    void ReverbParameterProcessorParam___dampingOut_f32 (ReverbParameterProcessorParam___State& _state, float value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        ParameterRamp___for__root__Reverb_dampingParameterRamp___updateParameter_f32 (_2.m_dampingParameterRamp_state, value);
    }

    void ReverbParameterProcessorParam___feedbackOut_f32 (ReverbParameterProcessorParam___State& _state, float value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        ParameterRamp___for__root__Reverb_feedbackParameterRamp___updateParameter_f32 (_2.m_feedbackParameterRamp_state, value);
    }

    //==============================================================================
    void ParameterRamp___for__root__Reverb_dryGainParameterRamp___updateParameter_f32 (ParameterRamp___for__root__Reverb_dryGainParameterRamp___State& _state, float newTarget) noexcept
    {
        float diff = {}, rampSeconds = {};
        float _2 = {};

        _state.m_targetValue = newTarget;
        diff = static_cast<float> (_state.m_targetValue - _state.m_currentValue);
        _2 = soul__intrinsics___abs_specialised (static_cast<float> (diff));
        rampSeconds = static_cast<float> (_2 / 20.0f);
        _state.m_rampSamples = static_cast<int32_t> ((sampleRate * 1.0) * static_cast<double> (rampSeconds));
        _state.m_rampIncrement = static_cast<float> (diff / static_cast<float> (static_cast<float> (_state.m_rampSamples)));
    }

    void ParameterRamp___for__root__Reverb_dryGainParameterRamp__run (ParameterRamp___for__root__Reverb_dryGainParameterRamp___State& _state, ParameterRamp___for__root__Reverb_dryGainParameterRamp___IO& _io) noexcept
    {
        float out_value_parameterOut = {};
        int32_t _2 = {};

        out_value_parameterOut = 0;
        if (! (_state.m_rampSamples > 0)) goto _ifnot_0;
        _if_0: { _state.m_currentValue = _state.m_currentValue + _state.m_rampIncrement;
                 _2 = _state.m_rampSamples;
                 _state.m_rampSamples = _2 - 1;
        }
        _ifnot_0: { out_value_parameterOut = out_value_parameterOut + _state.m_currentValue;
                    _state.m__resumePoint = 1;
                    _io.m__out_parameterOut = out_value_parameterOut;
        }
    }

    void ParameterRamp___for__root__Reverb_dryGainParameterRamp___initialise (ParameterRamp___for__root__Reverb_dryGainParameterRamp___State& _state) noexcept
    {
        _state.m_targetValue = 0;
        _state.m_currentValue = 0;
        _state.m_rampIncrement = 0;
        _state.m_rampSamples = 0;
    }

    //==============================================================================
    void ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___updateParameter_f32 (ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___State& _state, float newTarget) noexcept
    {
        float diff = {}, rampSeconds = {};
        float _2 = {};

        _state.m_targetValue = newTarget;
        diff = static_cast<float> (_state.m_targetValue - _state.m_currentValue);
        _2 = soul__intrinsics___abs_specialised (static_cast<float> (diff));
        rampSeconds = static_cast<float> (_2 / 20.0f);
        _state.m_rampSamples = static_cast<int32_t> ((sampleRate * 1.0) * static_cast<double> (rampSeconds));
        _state.m_rampIncrement = static_cast<float> (diff / static_cast<float> (static_cast<float> (_state.m_rampSamples)));
    }

    void ParameterRamp___for__root__Reverb_wetGain1ParameterRamp__run (ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___State& _state, ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___IO& _io) noexcept
    {
        float out_value_parameterOut = {};
        int32_t _2 = {};

        out_value_parameterOut = 0;
        if (! (_state.m_rampSamples > 0)) goto _ifnot_0;
        _if_0: { _state.m_currentValue = _state.m_currentValue + _state.m_rampIncrement;
                 _2 = _state.m_rampSamples;
                 _state.m_rampSamples = _2 - 1;
        }
        _ifnot_0: { out_value_parameterOut = out_value_parameterOut + _state.m_currentValue;
                    _state.m__resumePoint = 1;
                    _io.m__out_parameterOut = out_value_parameterOut;
        }
    }

    void ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___initialise (ParameterRamp___for__root__Reverb_wetGain1ParameterRamp___State& _state) noexcept
    {
        _state.m_targetValue = 0;
        _state.m_currentValue = 0;
        _state.m_rampIncrement = 0;
        _state.m_rampSamples = 0;
    }

    //==============================================================================
    void ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___updateParameter_f32 (ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___State& _state, float newTarget) noexcept
    {
        float diff = {}, rampSeconds = {};
        float _2 = {};

        _state.m_targetValue = newTarget;
        diff = static_cast<float> (_state.m_targetValue - _state.m_currentValue);
        _2 = soul__intrinsics___abs_specialised (static_cast<float> (diff));
        rampSeconds = static_cast<float> (_2 / 20.0f);
        _state.m_rampSamples = static_cast<int32_t> ((sampleRate * 1.0) * static_cast<double> (rampSeconds));
        _state.m_rampIncrement = static_cast<float> (diff / static_cast<float> (static_cast<float> (_state.m_rampSamples)));
    }

    void ParameterRamp___for__root__Reverb_wetGain2ParameterRamp__run (ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___State& _state, ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___IO& _io) noexcept
    {
        float out_value_parameterOut = {};
        int32_t _2 = {};

        out_value_parameterOut = 0;
        if (! (_state.m_rampSamples > 0)) goto _ifnot_0;
        _if_0: { _state.m_currentValue = _state.m_currentValue + _state.m_rampIncrement;
                 _2 = _state.m_rampSamples;
                 _state.m_rampSamples = _2 - 1;
        }
        _ifnot_0: { out_value_parameterOut = out_value_parameterOut + _state.m_currentValue;
                    _state.m__resumePoint = 1;
                    _io.m__out_parameterOut = out_value_parameterOut;
        }
    }

    void ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___initialise (ParameterRamp___for__root__Reverb_wetGain2ParameterRamp___State& _state) noexcept
    {
        _state.m_targetValue = 0;
        _state.m_currentValue = 0;
        _state.m_rampIncrement = 0;
        _state.m_rampSamples = 0;
    }

    //==============================================================================
    void ParameterRamp___for__root__Reverb_dampingParameterRamp___updateParameter_f32 (ParameterRamp___for__root__Reverb_dampingParameterRamp___State& _state, float newTarget) noexcept
    {
        float diff = {}, rampSeconds = {};
        float _2 = {};

        _state.m_targetValue = newTarget;
        diff = static_cast<float> (_state.m_targetValue - _state.m_currentValue);
        _2 = soul__intrinsics___abs_specialised (static_cast<float> (diff));
        rampSeconds = static_cast<float> (_2 / 20.0f);
        _state.m_rampSamples = static_cast<int32_t> ((sampleRate * 1.0) * static_cast<double> (rampSeconds));
        _state.m_rampIncrement = static_cast<float> (diff / static_cast<float> (static_cast<float> (_state.m_rampSamples)));
    }

    void ParameterRamp___for__root__Reverb_dampingParameterRamp__run (ParameterRamp___for__root__Reverb_dampingParameterRamp___State& _state, ParameterRamp___for__root__Reverb_dampingParameterRamp___IO& _io) noexcept
    {
        float out_value_parameterOut = {};
        int32_t _2 = {};

        out_value_parameterOut = 0;
        if (! (_state.m_rampSamples > 0)) goto _ifnot_0;
        _if_0: { _state.m_currentValue = _state.m_currentValue + _state.m_rampIncrement;
                 _2 = _state.m_rampSamples;
                 _state.m_rampSamples = _2 - 1;
        }
        _ifnot_0: { out_value_parameterOut = out_value_parameterOut + _state.m_currentValue;
                    _state.m__resumePoint = 1;
                    _io.m__out_parameterOut = out_value_parameterOut;
        }
    }

    void ParameterRamp___for__root__Reverb_dampingParameterRamp___initialise (ParameterRamp___for__root__Reverb_dampingParameterRamp___State& _state) noexcept
    {
        _state.m_targetValue = 0;
        _state.m_currentValue = 0;
        _state.m_rampIncrement = 0;
        _state.m_rampSamples = 0;
    }

    //==============================================================================
    void ParameterRamp___for__root__Reverb_feedbackParameterRamp___updateParameter_f32 (ParameterRamp___for__root__Reverb_feedbackParameterRamp___State& _state, float newTarget) noexcept
    {
        float diff = {}, rampSeconds = {};
        float _2 = {};

        _state.m_targetValue = newTarget;
        diff = static_cast<float> (_state.m_targetValue - _state.m_currentValue);
        _2 = soul__intrinsics___abs_specialised (static_cast<float> (diff));
        rampSeconds = static_cast<float> (_2 / 20.0f);
        _state.m_rampSamples = static_cast<int32_t> ((sampleRate * 1.0) * static_cast<double> (rampSeconds));
        _state.m_rampIncrement = static_cast<float> (diff / static_cast<float> (static_cast<float> (_state.m_rampSamples)));
    }

    void ParameterRamp___for__root__Reverb_feedbackParameterRamp__run (ParameterRamp___for__root__Reverb_feedbackParameterRamp___State& _state, ParameterRamp___for__root__Reverb_feedbackParameterRamp___IO& _io) noexcept
    {
        float out_value_parameterOut = {};
        int32_t _2 = {};

        out_value_parameterOut = 0;
        if (! (_state.m_rampSamples > 0)) goto _ifnot_0;
        _if_0: { _state.m_currentValue = _state.m_currentValue + _state.m_rampIncrement;
                 _2 = _state.m_rampSamples;
                 _state.m_rampSamples = _2 - 1;
        }
        _ifnot_0: { out_value_parameterOut = out_value_parameterOut + _state.m_currentValue;
                    _state.m__resumePoint = 1;
                    _io.m__out_parameterOut = out_value_parameterOut;
        }
    }

    void ParameterRamp___for__root__Reverb_feedbackParameterRamp___initialise (ParameterRamp___for__root__Reverb_feedbackParameterRamp___State& _state) noexcept
    {
        _state.m_targetValue = 0;
        _state.m_currentValue = 0;
        _state.m_rampIncrement = 0;
        _state.m_rampSamples = 0;
    }

    //==============================================================================
    void ReverbChannel___for__root__Reverb_reverbChannelLeft___initialise (ReverbChannel___for__root__Reverb_reverbChannelLeft___State& _state) noexcept
    {
        _state.m_comb_1_state.m__arrayEntry = 0;
        _state.m_comb_1_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_1_state.m__processorId = 1;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1___initialise (_state.m_comb_1_state);
        _state.m_comb_2_state.m__arrayEntry = 0;
        _state.m_comb_2_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_2_state.m__processorId = 2;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2___initialise (_state.m_comb_2_state);
        _state.m_comb_3_state.m__arrayEntry = 0;
        _state.m_comb_3_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_3_state.m__processorId = 3;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3___initialise (_state.m_comb_3_state);
        _state.m_comb_4_state.m__arrayEntry = 0;
        _state.m_comb_4_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_4_state.m__processorId = 4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4___initialise (_state.m_comb_4_state);
        _state.m_comb_5_state.m__arrayEntry = 0;
        _state.m_comb_5_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_5_state.m__processorId = 5;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5___initialise (_state.m_comb_5_state);
        _state.m_comb_6_state.m__arrayEntry = 0;
        _state.m_comb_6_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_6_state.m__processorId = 6;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6___initialise (_state.m_comb_6_state);
        _state.m_comb_7_state.m__arrayEntry = 0;
        _state.m_comb_7_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_7_state.m__processorId = 7;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7___initialise (_state.m_comb_7_state);
        _state.m_comb_8_state.m__arrayEntry = 0;
        _state.m_comb_8_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_8_state.m__processorId = 8;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8___initialise (_state.m_comb_8_state);
        _state.m_allpass_1_state.m__arrayEntry = 0;
        _state.m_allpass_1_state.m__sessionID = _state.m__sessionID;
        _state.m_allpass_1_state.m__processorId = 9;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1___initialise (_state.m_allpass_1_state);
        _state.m_allpass_2_state.m__arrayEntry = 0;
        _state.m_allpass_2_state.m__sessionID = _state.m__sessionID;
        _state.m_allpass_2_state.m__processorId = 10;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2___initialise (_state.m_allpass_2_state);
        _state.m_allpass_3_state.m__arrayEntry = 0;
        _state.m_allpass_3_state.m__sessionID = _state.m__sessionID;
        _state.m_allpass_3_state.m__processorId = 11;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3___initialise (_state.m_allpass_3_state);
        _state.m_allpass_4_state.m__arrayEntry = 0;
        _state.m_allpass_4_state.m__sessionID = _state.m__sessionID;
        _state.m_allpass_4_state.m__processorId = 12;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4___initialise (_state.m_allpass_4_state);
    }

    void ReverbChannel___for__root__Reverb_reverbChannelLeft__run (ReverbChannel___for__root__Reverb_reverbChannelLeft___State& _state, ReverbChannel___for__root__Reverb_reverbChannelLeft___IO& _io) noexcept
    {
        float _2 = {}, _3 = {}, _4 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1___IO _5 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2___IO _6 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3___IO _7 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4___IO _8 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5___IO _9 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6___IO _10 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7___IO _11 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8___IO _12 = {};
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1___IO _13 = {};
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2___IO _14 = {};
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3___IO _15 = {};
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4___IO _16 = {};

        _2 = _io.m__in_audioIn;
        _3 = _io.m__in_damping;
        _4 = _io.m__in_feedback;
        _5 = ZeroInitialiser();
        _5.m__in_audioIn = _2;
        _5.m__in_dampingIn = _3;
        _5.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1__run (_state.m_comb_1_state, _5);
        _6 = ZeroInitialiser();
        _6.m__in_audioIn = _2;
        _6.m__in_dampingIn = _3;
        _6.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2__run (_state.m_comb_2_state, _6);
        _7 = ZeroInitialiser();
        _7.m__in_audioIn = _2;
        _7.m__in_dampingIn = _3;
        _7.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3__run (_state.m_comb_3_state, _7);
        _8 = ZeroInitialiser();
        _8.m__in_audioIn = _2;
        _8.m__in_dampingIn = _3;
        _8.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4__run (_state.m_comb_4_state, _8);
        _9 = ZeroInitialiser();
        _9.m__in_audioIn = _2;
        _9.m__in_dampingIn = _3;
        _9.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5__run (_state.m_comb_5_state, _9);
        _10 = ZeroInitialiser();
        _10.m__in_audioIn = _2;
        _10.m__in_dampingIn = _3;
        _10.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6__run (_state.m_comb_6_state, _10);
        _11 = ZeroInitialiser();
        _11.m__in_audioIn = _2;
        _11.m__in_dampingIn = _3;
        _11.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7__run (_state.m_comb_7_state, _11);
        _12 = ZeroInitialiser();
        _12.m__in_audioIn = _2;
        _12.m__in_dampingIn = _3;
        _12.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8__run (_state.m_comb_8_state, _12);
        _13 = ZeroInitialiser();
        _13.m__in_audioIn = ((((((_5.m__out_audioOut + _6.m__out_audioOut) + _7.m__out_audioOut) + _8.m__out_audioOut) + _9.m__out_audioOut) + _10.m__out_audioOut) + _11.m__out_audioOut) + _12.m__out_audioOut;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1__run (_state.m_allpass_1_state, _13);
        _14 = ZeroInitialiser();
        _14.m__in_audioIn = _13.m__out_audioOut;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2__run (_state.m_allpass_2_state, _14);
        _15 = ZeroInitialiser();
        _15.m__in_audioIn = _14.m__out_audioOut;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3__run (_state.m_allpass_3_state, _15);
        _16 = ZeroInitialiser();
        _16.m__in_audioIn = _15.m__out_audioOut;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4__run (_state.m_allpass_4_state, _16);
        _io.m__out_audioOut = _16.m__out_audioOut;
    }

    //==============================================================================
    void ReverbChannel___for__root__Reverb_reverbChannelRight___initialise (ReverbChannel___for__root__Reverb_reverbChannelRight___State& _state) noexcept
    {
        _state.m_comb_1_state.m__arrayEntry = 0;
        _state.m_comb_1_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_1_state.m__processorId = 13;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1___initialise (_state.m_comb_1_state);
        _state.m_comb_2_state.m__arrayEntry = 0;
        _state.m_comb_2_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_2_state.m__processorId = 14;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2___initialise (_state.m_comb_2_state);
        _state.m_comb_3_state.m__arrayEntry = 0;
        _state.m_comb_3_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_3_state.m__processorId = 15;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3___initialise (_state.m_comb_3_state);
        _state.m_comb_4_state.m__arrayEntry = 0;
        _state.m_comb_4_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_4_state.m__processorId = 16;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4___initialise (_state.m_comb_4_state);
        _state.m_comb_5_state.m__arrayEntry = 0;
        _state.m_comb_5_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_5_state.m__processorId = 17;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5___initialise (_state.m_comb_5_state);
        _state.m_comb_6_state.m__arrayEntry = 0;
        _state.m_comb_6_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_6_state.m__processorId = 18;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6___initialise (_state.m_comb_6_state);
        _state.m_comb_7_state.m__arrayEntry = 0;
        _state.m_comb_7_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_7_state.m__processorId = 19;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7___initialise (_state.m_comb_7_state);
        _state.m_comb_8_state.m__arrayEntry = 0;
        _state.m_comb_8_state.m__sessionID = _state.m__sessionID;
        _state.m_comb_8_state.m__processorId = 20;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8___initialise (_state.m_comb_8_state);
        _state.m_allpass_1_state.m__arrayEntry = 0;
        _state.m_allpass_1_state.m__sessionID = _state.m__sessionID;
        _state.m_allpass_1_state.m__processorId = 21;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1___initialise (_state.m_allpass_1_state);
        _state.m_allpass_2_state.m__arrayEntry = 0;
        _state.m_allpass_2_state.m__sessionID = _state.m__sessionID;
        _state.m_allpass_2_state.m__processorId = 22;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2___initialise (_state.m_allpass_2_state);
        _state.m_allpass_3_state.m__arrayEntry = 0;
        _state.m_allpass_3_state.m__sessionID = _state.m__sessionID;
        _state.m_allpass_3_state.m__processorId = 23;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3___initialise (_state.m_allpass_3_state);
        _state.m_allpass_4_state.m__arrayEntry = 0;
        _state.m_allpass_4_state.m__sessionID = _state.m__sessionID;
        _state.m_allpass_4_state.m__processorId = 24;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4___initialise (_state.m_allpass_4_state);
    }

    void ReverbChannel___for__root__Reverb_reverbChannelRight__run (ReverbChannel___for__root__Reverb_reverbChannelRight___State& _state, ReverbChannel___for__root__Reverb_reverbChannelRight___IO& _io) noexcept
    {
        float _2 = {}, _3 = {}, _4 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1___IO _5 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2___IO _6 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3___IO _7 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4___IO _8 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5___IO _9 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6___IO _10 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7___IO _11 = {};
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8___IO _12 = {};
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1___IO _13 = {};
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2___IO _14 = {};
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3___IO _15 = {};
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4___IO _16 = {};

        _2 = _io.m__in_audioIn;
        _3 = _io.m__in_damping;
        _4 = _io.m__in_feedback;
        _5 = ZeroInitialiser();
        _5.m__in_audioIn = _2;
        _5.m__in_dampingIn = _3;
        _5.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1__run (_state.m_comb_1_state, _5);
        _6 = ZeroInitialiser();
        _6.m__in_audioIn = _2;
        _6.m__in_dampingIn = _3;
        _6.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2__run (_state.m_comb_2_state, _6);
        _7 = ZeroInitialiser();
        _7.m__in_audioIn = _2;
        _7.m__in_dampingIn = _3;
        _7.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3__run (_state.m_comb_3_state, _7);
        _8 = ZeroInitialiser();
        _8.m__in_audioIn = _2;
        _8.m__in_dampingIn = _3;
        _8.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4__run (_state.m_comb_4_state, _8);
        _9 = ZeroInitialiser();
        _9.m__in_audioIn = _2;
        _9.m__in_dampingIn = _3;
        _9.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5__run (_state.m_comb_5_state, _9);
        _10 = ZeroInitialiser();
        _10.m__in_audioIn = _2;
        _10.m__in_dampingIn = _3;
        _10.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6__run (_state.m_comb_6_state, _10);
        _11 = ZeroInitialiser();
        _11.m__in_audioIn = _2;
        _11.m__in_dampingIn = _3;
        _11.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7__run (_state.m_comb_7_state, _11);
        _12 = ZeroInitialiser();
        _12.m__in_audioIn = _2;
        _12.m__in_dampingIn = _3;
        _12.m__in_feedbackLevelIn = _4;
        CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8__run (_state.m_comb_8_state, _12);
        _13 = ZeroInitialiser();
        _13.m__in_audioIn = ((((((_5.m__out_audioOut + _6.m__out_audioOut) + _7.m__out_audioOut) + _8.m__out_audioOut) + _9.m__out_audioOut) + _10.m__out_audioOut) + _11.m__out_audioOut) + _12.m__out_audioOut;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1__run (_state.m_allpass_1_state, _13);
        _14 = ZeroInitialiser();
        _14.m__in_audioIn = _13.m__out_audioOut;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2__run (_state.m_allpass_2_state, _14);
        _15 = ZeroInitialiser();
        _15.m__in_audioIn = _14.m__out_audioOut;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3__run (_state.m_allpass_3_state, _15);
        _16 = ZeroInitialiser();
        _16.m__in_audioIn = _15.m__out_audioOut;
        AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4__run (_state.m_allpass_4_state, _16);
        _io.m__out_audioOut = _16.m__out_audioOut;
    }

    //==============================================================================
    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1__run (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1___State& _state, AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1___IO&
    _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float in = {}, bufferedValue = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0; }
        _body_0: { _2 = _io.m__in_audioIn;
                   in = static_cast<float> (_2);
                   bufferedValue = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> (in + (bufferedValue * 0.5f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 225)), 225);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (bufferedValue - in);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1___initialise (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_1___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2__run (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2___State& _state, AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2___IO&
    _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float in = {}, bufferedValue = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0; }
        _body_0: { _2 = _io.m__in_audioIn;
                   in = static_cast<float> (_2);
                   bufferedValue = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> (in + (bufferedValue * 0.5f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 341)), 341);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (bufferedValue - in);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2___initialise (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_2___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3__run (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3___State& _state, AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3___IO&
    _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float in = {}, bufferedValue = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0; }
        _body_0: { _2 = _io.m__in_audioIn;
                   in = static_cast<float> (_2);
                   bufferedValue = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> (in + (bufferedValue * 0.5f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 441)), 441);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (bufferedValue - in);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3___initialise (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_3___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4__run (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4___State& _state, AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4___IO&
    _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float in = {}, bufferedValue = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0; }
        _body_0: { _2 = _io.m__in_audioIn;
                   in = static_cast<float> (_2);
                   bufferedValue = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> (in + (bufferedValue * 0.5f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 556)), 556);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (bufferedValue - in);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4___initialise (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_allpass_4___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1116)), 1116);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_1___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1188)), 1188);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_2___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1277)), 1277);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_3___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1356)), 1356);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_4___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1422)), 1422);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_5___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1491)), 1491);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_6___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1557)), 1557);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_7___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1617)), 1617);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelLeft_comb_8___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1__run (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1___State& _state, AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1___IO&
    _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float in = {}, bufferedValue = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0; }
        _body_0: { _2 = _io.m__in_audioIn;
                   in = static_cast<float> (_2);
                   bufferedValue = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> (in + (bufferedValue * 0.5f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 248)), 248);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (bufferedValue - in);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1___initialise (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_1___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2__run (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2___State& _state, AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2___IO&
    _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float in = {}, bufferedValue = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0; }
        _body_0: { _2 = _io.m__in_audioIn;
                   in = static_cast<float> (_2);
                   bufferedValue = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> (in + (bufferedValue * 0.5f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 364)), 364);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (bufferedValue - in);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2___initialise (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_2___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3__run (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3___State& _state, AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3___IO&
    _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float in = {}, bufferedValue = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0; }
        _body_0: { _2 = _io.m__in_audioIn;
                   in = static_cast<float> (_2);
                   bufferedValue = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> (in + (bufferedValue * 0.5f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 464)), 464);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (bufferedValue - in);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3___initialise (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_3___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4__run (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4___State& _state, AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4___IO&
    _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {};
        int32_t _resumePoint = {};
        float in = {}, bufferedValue = {};
        int32_t _3 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0; }
        _body_0: { _2 = _io.m__in_audioIn;
                   in = static_cast<float> (_2);
                   bufferedValue = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> (in + (bufferedValue * 0.5f));
                   _3 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_3 + 1), 579)), 579);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (bufferedValue - in);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4___initialise (AllpassFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_allpass_4___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1139)), 1139);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_1___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1211)), 1211);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_2___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1300)), 1300);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_3___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1379)), 1379);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_4___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1445)), 1445);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_5___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1514)), 1514);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_6___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1580)), 1580);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_7___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
    }

    //==============================================================================
    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8__run (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8___State& _state, CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {};
        int32_t _resumePoint = {};
        float out = {};
        int32_t _6 = {};

        out_value_audioOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _body_0;
        _block_0: { _state.m_bufferIndex = 0;
                    _state.m_last = 0;
        }
        _body_0: { out = static_cast<float> (_state.m_buffer[_state.m_bufferIndex]);
                   out_value_audioOut = out_value_audioOut + static_cast<float> (out);
                   _2 = _io.m__in_dampingIn;
                   _3 = _io.m__in_dampingIn;
                   _state.m_last = static_cast<float> ((out * static_cast<float> (1.0f - _2)) + static_cast<float> (_state.m_last * _3));
                   _4 = _io.m__in_audioIn;
                   _5 = _io.m__in_feedbackLevelIn;
                   _state.m_buffer[_state.m_bufferIndex] = static_cast<float> ((0.015f * static_cast<float> (_4)) + static_cast<float> (_state.m_last * _5));
                   _6 = _state.m_bufferIndex;
                   _state.m_bufferIndex = _intrin_wrap (static_cast<int32_t> (_internal___wrapInt32 (static_cast<int32_t> (_6 + 1), 1640)), 1640);
                   _state.m__resumePoint = 1;
                   _io.m__out_audioOut = out_value_audioOut;
        }
    }

    void CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8___initialise (CombFilter___for__root__ReverbChannel___for__root__Reverb_reverbChannelRight_comb_8___State& _state) noexcept
    {
        _state.m_buffer = ZeroInitialiser();
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

