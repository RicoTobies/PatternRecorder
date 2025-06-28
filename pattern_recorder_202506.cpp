#include <iostream>
#include <cstdlib>
#include <vector>
#include <array>
#include <SDL.h>
#include <SDL_ttf.h>
#include <lo/lo.h>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <numeric> // Für std::accumulate
#include <string>
#include <algorithm>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <cmath>
#include <thread>
#include <mutex>
#include <future>
#include <chrono>
#include <tinyxml2.h>


using namespace std;


struct SharedData {
    float zeitfakto;
    float variable2;
    float variable3;
    float dauer = 4;
    float bpm = 120;
    bool p1 = false;
    bool p2 = false;
    bool p3 = false;

};
struct Notescale {
    float timestamp1;
    float timestamp2;
    float notev2;
    float notev1;
    int startIndexrecord;
};
struct Note {
    float timestamp;
    int instrument;
    int pitch;
    int scl;
    float p2;
    bool mschrittweitanz;
    float schrittweitanz;
};
struct Note2 {
    float timestamp;
    int instrument;
    int pitch;
    bool glide;
    float p2;
    bool mschrittweitanz;
    float schrittweitanz;
};
// struct Note3 {
//     float timestamp;
//     int instrument;
//     int pitch;

//     // Standardkonstruktor (optional)
//     Note3() : timestamp(0), instrument(0), pitch(0) {}

//     // Konstruktor mit Parametern
//     Note3(float t, int inst, int p) : timestamp(t), instrument(inst), pitch(p) {}
// };

struct Note3 {
    float timestamp;
    int instrument;
    int pitch;
    bool glide;
    float p2;

    // Standardkonstruktor
    Note3() : timestamp(0), instrument(0), pitch(0), glide(false), p2(0.0f) {}

    // Konstruktor mit Parametern
    Note3(float t, int inst, int p, bool g = false, float p2 = 0.0f) : timestamp(t), instrument(inst), pitch(p), glide(g), p2(p2) {}

    // Konvertierungskonstruktor von Note3
    // Note4(const Note3& n3) : timestamp(n3.timestamp), instrument(n3.instrument), pitch(n3.pitch), glide(false) {}
};
struct xy {
    int x;
    int y;
};
struct xn {
    float i;
    float xn;
};
struct Modu {
    float time;
    int instrument;
    int pitch;
};
struct vecforrec {
    int recsteps;
    int recstep;
    float AnIxNm;
    float EnIxNm;
    int steps;
    int kis;
    float startTime;
    float endTime;
};
struct Festerfester {
    float startTime;
    float endTime;
    int startIndexnum;
    int endIndexnum;
    int schrittweitanz;
    bool firstrec;
};
struct playfofohi {
    Uint32 current_indices;
    float start_times;
};

class PatternRecorder {
public:
    // Audio parameters
    static const int SAMPLE_RATE = 44100;
    static const int AMPLITUDE = 28000;
    SDL_AudioDeviceID audioDevice;
    bool audioIsPlaying = false;
    float frequency = 440.0f; // Default frequency in Hz
    float targetFrequency = 440.0f; // Target frequency for glide
    float currentFrequency = 440.0f; // Current frequency during glide


    std::array<std::vector<Note3>, 8> pnotes;
    int currentPattern = 0;
    int currentNoteIndex = 0;
    bool editingGlide = false;
    float glideRate = 0.001f; // Adjust this value to control glide speed

    // Audio callback function
    static void audioCallback(void* userdata, Uint8* stream, int len) {
        PatternRecorder* recorder = static_cast<PatternRecorder*>(userdata);
        Sint16* buffer = reinterpret_cast<Sint16*>(stream);
        int samples = len / sizeof(Sint16);
        
        static double phase = 0.0;
        double phaseIncrement = 2.0 * M_PI * recorder->frequency / SAMPLE_RATE;
        
        for (int i = 0; i < samples; i++) {
            if (recorder->audioIsPlaying) {
                buffer[i] = static_cast<Sint16>(AMPLITUDE * sin(phase));
                phase += phaseIncrement;
                if (phase > 2.0 * M_PI) {
                    phase -= 2.0 * M_PI;
                }
            } else {
                buffer[i] = 0;
            }
        }
    }

    void initAudio() {
        SDL_AudioSpec want, have;
        SDL_zero(want);
        want.freq = SAMPLE_RATE;
        want.format = AUDIO_S16SYS;
        want.channels = 1;
        want.samples = 2048;
        want.callback = audioCallback;
        want.userdata = this;

        audioDevice = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
        if (audioDevice == 0) {
            std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
            return;
        }
        SDL_PauseAudioDevice(audioDevice, 0);
    }

    void playPatternAsSound(int patternIndex) {
        if (patternIndex >= 0 && patternIndex < pnotes.size()) {
            const auto& pattern = pnotes[patternIndex];
            if (pattern.empty()) {
                std::cout << "Pattern " << (patternIndex + 1) << " is empty!" << std::endl;
                return;
            }

            std::cout << "Playing pattern " << (patternIndex + 1) << " with " << pattern.size() << " notes" << std::endl;
            std::cout << "BPM: " << sharedData->bpm << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(plsl));
            
            audioIsPlaying = true;
            float lastFreq = 440.0f; // Starting frequency
            
            float playStartTime = SDL_GetTicks() / 1000.0f; // Aktuelle Zeit in Sekunden
            int noteIndex = 0;
            int currentLoop = 0;
            float playTime, playTimeold;
            
            while (currentLoop < loops) {
                // if (pattern[noteIndex].timestamp == 0) {
                //     playTime = static_cast<float>() * sharedData->zeitfakto;
                //     playTimeold = static_cast<float>(pattern[noteIndex-2].timestamp) * sharedData->zeitfakto;
                //     playStartTime -= pattern[pattern.size()-1].timestamp-pattern[pattern.size()-2].timestamp;
                // } else {                    
                //     playTime = static_cast<float>(pattern[noteIndex].timestamp) * sharedData->zeitfakto;
                //     playTimeold = static_cast<float>(pattern[noteIndex-1].timestamp) * sharedData->zeitfakto;
                // }
                if (noteIndex == 0) {
                    playTimeold = static_cast<float>(pattern[pattern.size()-2].timestamp-pattern[pattern.size()-1].timestamp) * sharedData->zeitfakto;
                } else {                    
                    playTimeold = static_cast<float>(pattern[noteIndex-1].timestamp) * sharedData->zeitfakto;
                }
                playTime = static_cast<float>(pattern[noteIndex].timestamp) * sharedData->zeitfakto;
                float currentTime = SDL_GetTicks() / 1000.0f;
                
                if (noteIndex >= pattern.size()) {
                    // Reset for next loop
                    playStartTime += pattern[pattern.size()-1].timestamp * sharedData->zeitfakto;
                    noteIndex = 0;
                    currentLoop++;
                    if (currentLoop >= loops) break;
                    continue;
                }
                
                const auto& note = pattern[noteIndex];
                
                float p2 = note.p2;
                float p = (currentTime - (playStartTime + playTimeold)) / (playTime-playTimeold);
                float q = static_cast<float>(noteIndex+1) / static_cast<float>(pattern.size());
                string ps = "";
                frequency = static_cast<float>(sin(6.28f*((p/4)+(q/4))+1.0d)/2.0d)*targetFrequency*(1+p2);//targetFrequency;
                // frequency = q * targetFrequency;
                for (float i = 0; i <= 1.0f; i += 0.1f) {
                    if (i <= p && i <= q) {
                        ps += "=";
                    } else if (i <= p) {
                        ps += "-";
                    } else if (i <= q) {
                        ps += "+";
                    } else {
                        ps += " ";
                    }
                }
                std::cout << "  [" << ps << "]\r";


                if (p>=0.42f) {
                // if ((currentTime - playStartTime) >= playTimeold + (0.42*(playTime-playTimeold))) {
                // if ((currentTime - playStartTime) >= playTimeold) {
                    // frequency = 0.0f;
                    // cout<<"frequency = 0.0f"<<endl;
                }
                // cout<<currentTime<<setw(15)<<currentTime-playStartTime<<setw(15)<<playTimeold<<setw(15)<<playTime<<endl;
                if(p>=1.0f) {
                // if ((currentTime - playStartTime) >= playTime) {
                    // Convert MIDI note to frequency (A4 = 69 = 440Hz)
                    targetFrequency = 440.0f * pow(2.0f, (note.pitch-toene - 69) / 12.0f);
                    
                    if (note.glide == 1 && noteIndex > 0) {
                        // Start from last frequency and glide to target
                        currentFrequency = lastFreq;
                        std::cout << "Playing note with glide: pitch=" << note.pitch-toene 
                                  << ", from freq=" << lastFreq << "Hz to " << targetFrequency << "Hz" << std::endl;
                        
                        // Calculate glide duration based on note timing
                        float glideDuration = note.timestamp * sharedData->zeitfakto;
                        auto glideStart = std::chrono::steady_clock::now();
                        
                        while (std::chrono::duration<float>(std::chrono::steady_clock::now() - glideStart).count() < glideDuration) {
                            float progress = std::chrono::duration<float>(std::chrono::steady_clock::now() - glideStart).count() / glideDuration;
                            currentFrequency = lastFreq + (targetFrequency - lastFreq) * progress;
                            frequency = currentFrequency;

                            // std::this_thread::sleep_for(std::chrono::microseconds(10));
                        }
                    } else {
                        // No glide - jump directly to target frequency
                        currentFrequency = targetFrequency;
                        frequency = currentFrequency;

                        std::cout << "Playing note: pitch=" << note.pitch-toene << ", freq=" << targetFrequency << "Hz" << std::endl;
                        std::cout << "  [          ]\r";
                    }
                    
                    lastFreq = targetFrequency; // Remember this frequency for next note
                    noteIndex++;
                }
                
                // std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
            
            audioIsPlaying = false;
            currentFrequency = 440.0f; // Reset to default
        } else {
            std::cout << "Invalid pattern index: " << patternIndex << std::endl;
        }
    }
    

    ~PatternRecorder() {
        if (audioDevice != 0) {
            SDL_CloseAudioDevice(audioDevice);
        }
    }

    std::vector<vector<float>> pdauer;
    std::vector<vector<int>> pselect;

    // std::vector<std::vector<float>> pdauer(9, std::vector<float>(1, 0.0f));

    vector<Note> notes;
    vector<Note> snotes;
    vector<Note> ssnotes;
    vector<Note> notesr;
    vector<Note3> notes3;
    vector<Note3> notes4;
    vector<Note3> notes4s;
    vector<Note3> notes5;
    vector<Note3> notes4p;
    vector<Note3> newNotes;
    vector<Notescale> notestoscale;    
    vector<Modu> mod1;
    vector<vecforrec> veforec;
    vector<playfofohi> playfofohi1;
    vector<Festerfester> festerfester;
    // vector<bool> isPlaying;

    std::vector<std::atomic<bool>> isPlaying;





    boost::interprocess::shared_memory_object shm;
    boost::interprocess::mapped_region region;
    SharedData* sharedData;
    float* shared_zeitfakto;
    float zeitfakto = 1.0f;
    // float beatDuration = 60.0f / sharedData->bpm; // Dauer eines Beats in Sekunden
    float startTime, endTime;
    int startIndex, endIndex;
    float startIndexnum, endIndexnum;
    float Indexnumlaenge;
    int schrittweitanz;
    int plsl = 0;
    int loops = 1;
    int numPatterns = 8;
    int patterns;
    int toene = 0;
    float factor = 1;
    // std::vector<std::atomic<bool>> isPlaying(8);

    bool totalDi = false;
    bool kk = true;
    bool kl = false;
    bool kr = false;
    bool firstrec, firstrec0 =  false;
    bool pgup = true;
    bool vmidi = false;

    PatternRecorder();
    PatternRecorder(SharedData* sd, int patterns);

    void updateZeitfakto(float newValue);
    void updateVariable2(float newValue);
    void updateVariable3(float newValue);
    void updatedauer(float newValue);
    void updatebpm(float newValue);
    void updatep1(bool newValue);
    void updatep2(bool newValue);
    void updatep3(bool newValue);
    
    std::string Eingabe();
    void setpdauer(int num);
    void select(int num);
    void reverse(int num);
    void vectorinsert(int num);
    void mal(int num, float factor);
    void durch(int num, float factor);
    void plus(int num, float factor);
    void minus(int num, float factor);
    void modul1();
    void notelenght();
    void noteglide();
    void noteinstr();
    void fdx();
    void anfendIndex();
    void createGraphWindow();
    void DisPlNotes();
    void totalDurationbreak(float startTime, float endTime);
    void zeitfaktor();
    void vecforrecf();
    void recntr();
    void recntrf();
    void record();
    void removeAndRecord();
    void remove();
    void removeduplicates(std::vector<Note3>& notes4);
    void play();
    void play2();
    void play3();
    void play4(int num, SDL_Event& event);
    void playPattern(int num);
    void play4thread(int num);
    void playforforhi(int num);
    void play34(int num);
    void scale();
    void scaleToRange(vector<Note3>& newNotes, float startTime, float endTime);
    void scaleToRangenum(vector<Note3>& newNotes, float startTime, float endTime, float startIndexnum, float endIndexnum, int schrittweitanz, bool firstrec);
    void transformTimestamps();
    void reverseTransformTimestamps();
    void quantizeNotes(vector<Note3>& notes, int quantization);
    void setBPM();
    void bpmtap();
    void saveToCSV(const string& filename, const std::array<std::vector<Note3>, 8> pnotes, int num);
    void readFromCSV(const string& filename, int num);
    void display(SDL_Renderer* renderer);
    void clearNotes(); // Neue Methode zum Löschen der Noten
    void clearNotes2(); // Neue Methode zum Löschen der Noten
    void toggleRenoisePlay(); // Toggle Play in Renoise
    void toggleRenoiseRecord(); // Toggle Record in Renoise
    void startRenoisePlayAndRecord(); // Start Play and Record in Renoise
    bool isRenoisePlayAndRecordEnabled() const; // Getter für den Status
    void sendNoteToRenoise(int instrument, int pitch); // Funktionsdeklaration
    int convertRenoiseNoteToMidi(const std::string& noteStr);
    void convertRenoisePatternToNotes(const std::string& patternXml);
    void pasteRenoisePattern();
    std::string getClipboardContent();
    void convertNotesToRenoisePattern(int patternLength);
    void convertNotesToSchismPattern(int patternLength);
    std::string convertMidiNoteToRenoiseNote(int midiNote);
    void sinusInterpolationBetweenTimestamps(const std::vector<Note3>& notes4, int steps_per_interval, float factorpi);
    std::vector<Note3> interpolateNotesWithSinusPitch(const std::vector<Note3>& notes4, int steps_per_interval, float factorpi, float noteanz);
    // void sendMidiControlFromNotes(const std::vector<Note3>& notes, int controllerNumber, int midiChannel = 0);

private:


    std::vector<std::future<void>> playingThreads; 
    // std::vector<std::thread> playingThreads;
    std::mutex playingMutex;
    std::mutex dataMutex; // Mutex for thread safety


    vector<Note2> notes2;
    vector<Note2> notes22;
    vector<Note3> notes33;
    vector<Note3> notes44;
    vector<Note3> scaledNotes;
    vector<Note3> scaledNotes2;

    std::string svalue = "";
    int currentPlayingNote = -1; // Aktuell gespielte Note
    bool renoisePlaying = false; // Status für Renoise Play
    bool renoiseRecording = false; // Status für Renoise Record
    bool renoisePlayAndRecordEnabled = false; // Status für Play und Record
    float timestampvalue = 0.0f;

    // void sendNoteToRenoise(int instrument, int pitch); // Funktionsdeklaration
    void sendBPMToRenoise(); // Funktionsdeklaration
};
// PatternRecorder::PatternRecorder() : shm(boost::interprocess::open_or_create, "ZeitfaktoSharedMemory", boost::interprocess::read_write) {
//     shm.truncate(sizeof(SharedData));
//     region = boost::interprocess::mapped_region(shm, boost::interprocess::read_write);
//     sharedData = static_cast<SharedData*>(region.get_address());
//     sharedData->zeitfakto = 1.0f;
//     sharedData->variable2 = 1.0f;
//     sharedData->variable3 = 1.0f;
//     sharedData->dauer = 4.0f;
//     sharedData->bpm = 120.0f;
// }

// PatternRecorder::PatternRecorder() 
//     : numPatterns(8),
//       isPlaying(8),
//       playingThreads(8),
//       pdauer(8, std::vector<float>(1, 0.0f)),
//       zeitfaktoShm(boost::interprocess::open_or_create, "ZeitfaktoSharedMemory", boost::interprocess::read_write),
//       zeitfaktoRegion(zeitfaktoShm, boost::interprocess::read_write),
//       imageShm(boost::interprocess::open_or_create, "MyImageShm", boost::interprocess::read_write),
//       imageRegion(imageShm, boost::interprocess::read_write)
// {
//     try {
//         // ZeitfaktoSharedMemory
//         std::cout << "Creating ZeitfaktoSharedMemory..." << std::endl;
//         zeitfaktoShm.truncate(std::max(sizeof(SharedData), static_cast<std::size_t>(1024)));
//         sharedData = static_cast<SharedData*>(zeitfaktoRegion.get_address());
        
//         if (sharedData == nullptr) {
//             throw std::runtime_error("Failed to map ZeitfaktoSharedMemory");
//         }

//         // Initialisierung der SharedData-Struktur
//         sharedData->zeitfakto = 1.0f;
//         sharedData->variable2 = 1.0f;
//         sharedData->variable3 = 1.0f;
//         sharedData->dauer = 4.0f;
//         sharedData->bpm = 120.0f;

//         // MyImageShm
//         std::cout << "Creating MyImageShm..." << std::endl;
//         imageShm.truncate(1024 * 1024); // Beispiel: 1 MB

//         std::cout << "PatternRecorder initialized successfully" << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "Error in PatternRecorder constructor: " << e.what() << std::endl;
//         throw;
//     }

//     for (int i = 0; i < 8; ++i) {
//         pdauer.push_back(std::vector<float>(1, 0.0f));
//     }
//     for (int i=0; i<9; ++i) {
//         playfofohi1.push_back({0, 0.0f});
//     }
// }



PatternRecorder::PatternRecorder() 
    : numPatterns(8),
      isPlaying(8),  // Initialisiert isPlaying mit der Größe patterns
      playingThreads(8),
      pdauer(8, std::vector<float>(1, 0.0f))

    {
    try {
        std::cout << "Removing existing shared memory..." << std::endl;
        boost::interprocess::shared_memory_object::remove("ZeitfaktoSharedMemory");

        std::cout << "Creating shared memory..." << std::endl;
        shm = boost::interprocess::shared_memory_object(boost::interprocess::create_only, "ZeitfaktoSharedMemory", boost::interprocess::read_write);

        std::cout << "Truncating shared memory..." << std::endl;
        shm.truncate(std::max(sizeof(SharedData), static_cast<std::size_t>(1024)));

        std::cout << "Mapping region..." << std::endl;
        region = boost::interprocess::mapped_region(shm, boost::interprocess::read_write);

        std::cout << "Getting address..." << std::endl;
        sharedData = static_cast<SharedData*>(region.get_address());
        
        if (sharedData == nullptr) {
            throw std::runtime_error("Failed to map shared memory");
        }

        std::cout << "Initializing shared data..." << std::endl;
        sharedData->zeitfakto = 1.0f;
        sharedData->variable2 = 1.0f;
        sharedData->variable3 = 1.0f;
        sharedData->dauer = 4.0f;
        sharedData->bpm = 120.0f;

        // ... andere Initialisierungen ...

        // Initialize audio system
        std::cout << "Initializing audio..." << std::endl;
        initAudio();
        
        std::cout << "PatternRecorder initialized successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error in PatternRecorder constructor: " << e.what() << std::endl;
        throw;
    }
    for (int i = 0; i < 8; ++i) {
    pdauer.push_back(std::vector<float>(1, 0.0f));
    }
    for (int i=0; i<9;++i) {
        playfofohi1.push_back({0, 0.0f});
    }
    // sharedData(sd), 

}



// PatternRecorder::PatternRecorder(SharedData* sd, int patterns) 
//     : sharedData(sd), 
//       numPatterns(patterns),
//       isPlaying(patterns),  // Initialisiert isPlaying mit der Größe patterns
//       playingThreads(patterns),
//       pdauer(patterns, std::vector<float>(1, 0.0f))
// {
//     // Weitere Initialisierungen, falls nötig
// }


void PatternRecorder::updateZeitfakto(float newValue) {
    sharedData->zeitfakto = newValue;
}
void PatternRecorder::updateVariable2(float newValue) {
    sharedData->variable2 = newValue;
}
void PatternRecorder::updateVariable3(float newValue) {
    sharedData->variable3 = newValue;
}
void PatternRecorder::updatedauer(float newValue) {
    sharedData->dauer = newValue;
}
void PatternRecorder::updatebpm(float newValue) {
    sharedData->bpm = newValue;
}
void PatternRecorder::updatep1(bool newValue) {
    sharedData->p1 = newValue;
}
void PatternRecorder::updatep2(bool newValue) {
    sharedData->p2 = newValue;
}
void PatternRecorder::updatep3(bool newValue) {
    sharedData->p3 = newValue;
}

// void PatternRecorder::sendMidiControlFromNotes(
//     const std::vector<Note3>& notes,
//     int controllerNumber,   // e.g., 1 = modulation wheel, 74 = filter cutoff
//     int midiChannel = 0     // 0-15
// ) {
//     RtMidiOut* midiout = nullptr;
//     try {
//         midiout = new RtMidiOut();
//         if (midiout->getPortCount() == 0) {
//             std::cerr << "No MIDI output ports available." << std::endl;
//             delete midiout;
//             return;
//         }
//         midiout->openPort(0); // Open first available port

//         std::vector<unsigned char> message;
//         for (const auto& note : notes) {
//             int value = note.pitch; // Ensure pitch is 0–127
//             if (value < 0) value = 0;
//             if (value > 127) value = 127;

//             message.clear();
//             message.push_back(0xB0 + midiChannel); // Control Change
//             message.push_back(controllerNumber);
//             message.push_back(value);
//             midiout->sendMessage(&message);
//         }
//         delete midiout;
//     }
//     catch (RtMidiError& error) {
//         error.printMessage();
//         if (midiout) delete midiout;
//     }
// }

void PatternRecorder::removeduplicates(std::vector<Note3>& notes4) {
    for (size_t i = 0; i < notes4.size() - 1; ++i) {
        if (notes4[i].timestamp==notes4[i+1].timestamp) notes4.erase(notes4.begin()+i+1, notes4.begin()+i+1);
    }
}
std::vector<Note3> PatternRecorder::interpolateNotesWithSinusPitch(const std::vector<Note3>& notes4, int steps_per_interval, float factorpi, float noteanz) {
    std::vector<Note3> result;
    if (notes4.empty()) return result;

    // Die erste Note übernehmen
    result.push_back(notes4[0]);

    for (size_t i = 0; i < notes4.size() - 1; ++i) {
        float t_start = notes4[i].timestamp;
        float t_end = notes4[i+1].timestamp;
        float range = t_end - t_start;
        if (range <= 0.0f) continue;

        // Zwischenwerte berechnen und einfügen
        for (int j = 1; j <= steps_per_interval; ++j) {
            float t = t_start + range * j / float(steps_per_interval + 1); // oder steps_per_interval, je nach Wunsch
            float normalized = (t - t_start) * (factorpi*M_PI) / range;
            float sine_value = std::sin(normalized);
            int pitch = static_cast<int>(sine_value * noteanz)+notes4[i].pitch; // Skalierung auf 0–127

            // Neue Note mit passendem Pitch und anderen Werten aus der vorherigen Note (oder anpassbar)
            Note3 new_note = notes4[i];
            new_note.timestamp = t;
            new_note.pitch = pitch;
            // Optional: andere Felder wie instrument, glide, p2 anpassen

            result.push_back(new_note);
        }

        // Die nächste Originalnote übernehmen
        result.push_back(notes4[i+1]);
    }

    return result;
}
void PatternRecorder::sinusInterpolationBetweenTimestamps(const std::vector<Note3>& notes4, int steps_per_interval, float factorpi ) {
    if (notes4.size() < 2) {
        std::cerr << "Error: Need at least 2 notes for interpolation." << std::endl;
        return;
    }

    for (size_t i = 0; i < notes4.size() - 1; ++i) {
        float t_start = notes4[i].timestamp;
        float t_end   = notes4[i+1].timestamp;
        float range   = t_end - t_start;
        if (range <= 0.0f) continue; // Überspringe, falls Zeitstempel gleich

        std::cout << "--- Interval " << i << ": " << t_start << " to " << t_end << " ---" << std::endl;

        for (int j = 0; j <= steps_per_interval; ++j) {
            float t = t_start + range * j / float(steps_per_interval);
            float normalized = (t - t_start) * (factorpi*M_PI) / range;
            float sine_value = std::sin(normalized);

            std::cout << "t = " << t << ", normalized = " << normalized << ", sin = " << sine_value << std::endl;
        }
    }
}

// Helper function to convert MIDI note number to Renoise note string
std::string PatternRecorder::convertMidiNoteToRenoiseNote(int midiNote) {
    const char* noteNames[] = { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-" };
    int noteNumber = midiNote % 12;
    int octave = midiNote / 12 - 1; // Renoise uses -1 as base octave
    
    std::string noteStr = noteNames[noteNumber];
    noteStr += std::to_string(octave);
    
    return noteStr;
}
void PatternRecorder::convertNotesToSchismPattern(int patternLength) {
    std::cout << "Converting notes to Schism pattern..." << std::endl;
    std::string noteStr, instr, lineStr, af, ed, chStr, effStr, empty, buff;
    af = "|";
    ed = ".\n";
    chStr = "..";
    effStr = "...";
    empty = "..........";
    buff = "Pasted Pattern - IT\n";
    bool bnote = 0;
    for (int i=0; i<patternLength;i++) {
        lineStr="";
        for (int ii=0; ii<notes4.size(); ii++) {
            if ((int)(notes4[ii].timestamp*(float)patternLength)==i) {
                noteStr = convertMidiNoteToRenoiseNote(notes4[ii].pitch);
                instr = "00";
                lineStr+=af;
                lineStr+=noteStr;
                lineStr+=instr;
                lineStr+=chStr;
                lineStr+=effStr;
                lineStr+=ed;
                bnote=1;
            }
        }
        if (bnote==0) {
            lineStr+=af;
            lineStr+=empty;
            lineStr+=ed;
        }
        buff+=lineStr;
        bnote=0;
    }    
    // Save to a temporary file
    std::string tempFile = "/tmp/schism_pattern.pit";
    std::ofstream outFile(tempFile);
    outFile << buff;
    outFile.close();
    
    // Copy to clipboard using xclip
    std::string command = "xclip -selection clipboard -i " + tempFile;
    system(command.c_str());
    
    // Wait a moment to ensure clipboard is ready
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Also copy to primary selection
    command = "xclip -selection primary -i " + tempFile;
    system(command.c_str());
    
    std::cout << "Pattern copied to clipboard with " << patternLength << " lines" << std::endl;

}
void PatternRecorder::convertNotesToRenoisePattern(int patternLength) {
    std::cout << "Converting notes to Renoise pattern..." << std::endl;
    
    // Create the XML document
    tinyxml2::XMLDocument doc;
    
    // Create XML declaration
    auto* decl = doc.NewDeclaration();
    doc.InsertFirstChild(decl);
    
    // Create root element
    auto* root = doc.NewElement("PatternClipboard.BlockBuffer");
    root->SetAttribute("doc_version", "0");
    doc.InsertEndChild(root);
    
    // Create Columns element
    auto* columns = doc.NewElement("Columns");
    root->InsertEndChild(columns);
    
    // Create outer Column element
    auto* outerColumn = doc.NewElement("Column");
    columns->InsertEndChild(outerColumn);
    
    // Create inner Column element
    auto* innerColumn = doc.NewElement("Column");
    outerColumn->InsertEndChild(innerColumn);
    
    // Create Lines element
    auto* lines = doc.NewElement("Lines");
    innerColumn->InsertEndChild(lines);
    
    // Calculate the time per line based on pattern length
    float timePerLine = 1.0f / patternLength;
    
    // Sort notes by timestamp to ensure they're in the correct order
    std::vector<Note3> sortedNotes = notes4;
    std::sort(sortedNotes.begin(), sortedNotes.end(), 
        [](const Note3& a, const Note3& b) {
            return a.timestamp < b.timestamp;
        });
    
    // Create empty Line elements first
    for (int i = 0; i < patternLength; i++) {
        auto* line = doc.NewElement("Line");
        line->SetAttribute("index", i);
        lines->InsertEndChild(line);
    }
    
    // Process each note and add to the appropriate Line
    for (const auto& note : sortedNotes) {
        // Skip the final timestamp marker (0,0,0)
        if (note.instrument == 0 && note.timestamp == 1) continue;
        
        // Calculate line index based on timestamp
        int lineIndex = static_cast<int>(note.timestamp / timePerLine);
        
        // Ensure line index is within bounds
        if (lineIndex >= patternLength) continue;
        
        // Find the line
        tinyxml2::XMLElement* line = nullptr;
        for (auto* existingLine = lines->FirstChildElement("Line"); existingLine; existingLine = existingLine->NextSiblingElement("Line")) {
            int existingIndex = 0;
            if (existingLine->Attribute("index")) {
                existingIndex = std::stoi(existingLine->Attribute("index"));
            }
            if (existingIndex == lineIndex) {
                line = existingLine;
                break;
            }
        }
        
        if (!line) continue; // Should never happen since we created all lines
        
        // Get or create NoteColumns element
        auto* noteColumns = line->FirstChildElement("NoteColumns");
        if (!noteColumns) {
            noteColumns = doc.NewElement("NoteColumns");
            line->InsertEndChild(noteColumns);
        }
        
        // Create NoteColumn element for this note
        auto* noteColumn = doc.NewElement("NoteColumn");
        noteColumns->InsertEndChild(noteColumn);
        
        // Create Note element
        auto* noteElement = doc.NewElement("Note");
        std::string noteStr = convertMidiNoteToRenoiseNote(note.pitch);
        noteElement->SetText(noteStr.c_str());
        noteColumn->InsertEndChild(noteElement);
        
        // Create Instrument element
        auto* instrumentElement = doc.NewElement("Instrument");
        instrumentElement->SetText("00");
        noteColumn->InsertEndChild(instrumentElement);
    }
    
    // Add ColumnType and SubColumnMask elements
    auto* columnType = doc.NewElement("ColumnType");
    columnType->SetText("NoteColumn");
    innerColumn->InsertEndChild(columnType);
    
    auto* subColumnMask = doc.NewElement("SubColumnMask");
    subColumnMask->SetText("true true true false false false false false");
    innerColumn->InsertEndChild(subColumnMask);
    
    // Convert XML to string
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    std::string patternXml = printer.CStr();
    
    // Save to a temporary file
    std::string tempFile = "/tmp/renoise_pattern.xml";
    doc.SaveFile(tempFile.c_str());
    
    // Copy to clipboard using xclip
    std::string command = "xclip -selection clipboard -i " + tempFile;
    system(command.c_str());
    
    // Wait a moment to ensure clipboard is ready
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Also copy to primary selection
    command = "xclip -selection primary -i " + tempFile;
    system(command.c_str());
    
    std::cout << "Pattern copied to clipboard with " << patternLength << " lines" << std::endl;
}

std::string PatternRecorder::getClipboardContent() {
    FILE* pipe = popen("xclip -selection clipboard -o", "r");
    if (!pipe) return "";
    
    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}
void PatternRecorder::pasteRenoisePattern() {
    std::string clipboardText = getClipboardContent();
    if (!clipboardText.empty()) {
        std::cout << "Clipboard content: " << clipboardText << std::endl;
        
        // Konvertiere das Pattern
        convertRenoisePatternToNotes(clipboardText);
    } else {
        std::cout << "No clipboard content available" << std::endl;
    }
}

// Helper function to convert Renoise note string to MIDI number
int PatternRecorder::convertRenoiseNoteToMidi(const std::string& noteStr) {
    if (noteStr == "Off") return -1; // Rest
    
    // Extract note letter and octave
    char noteLetter = noteStr[0];
    int octave = std::stoi(noteStr.substr(2));
    
    // Convert note letter to MIDI note number
    int noteNumber = 0;
    switch (noteLetter) {
        case 'C': noteNumber = 0; break;
        case 'C#': noteNumber = 1; break;
        case 'D': noteNumber = 2; break;
        case 'D#': noteNumber = 3; break;
        case 'E': noteNumber = 4; break;
        case 'F': noteNumber = 5; break;
        case 'F#': noteNumber = 6; break;
        case 'G': noteNumber = 7; break;
        case 'G#': noteNumber = 8; break;
        case 'A': noteNumber = 9; break;
        case 'A#': noteNumber = 10; break;
        case 'B': noteNumber = 11; break;
    }
    
    // Calculate final MIDI note number
    return (octave + 2) * 12 + noteNumber;
}

void PatternRecorder::convertRenoisePatternToNotes(const std::string& patternXml) {
    std::cout << "Starting to convert Renoise pattern..." << std::endl;
    
    // Parse the XML string
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError error = doc.Parse(patternXml.c_str());
    
    if (error != tinyxml2::XML_SUCCESS) {
        std::cout << "Error parsing XML: " << doc.ErrorStr() << std::endl;
        return;
    }
    
    // Get the root element
    auto* root = doc.FirstChildElement("PatternClipboard.BlockBuffer");
    if (!root) {
        std::cout << "Could not find root element PatternClipboard.BlockBuffer" << std::endl;
        return;
    }
    
    // Get the Columns element
    auto* columns = root->FirstChildElement("Columns");
    if (!columns) {
        std::cout << "Could not find Columns element" << std::endl;
        return;
    }
    
    // Get the first Column element (contains notes)
    auto* column = columns->FirstChildElement("Column");
    if (!column) {
        std::cout << "Could not find Column element" << std::endl;
        return;
    }
    
    // Get the inner Column element
    auto* innerColumn = column->FirstChildElement("Column");
    if (!innerColumn) {
        std::cout << "Could not find inner Column element" << std::endl;
        return;
    }
    
    // Get the Lines element
    auto* lines = innerColumn->FirstChildElement("Lines");
    if (!lines) {
        std::cout << "Could not find Lines element" << std::endl;
        return;
    }
    
    // Clear existing notes
    notes4.clear();
    std::cout << "Cleared existing notes" << std::endl;
    
    // Process each line
    int notesProcessed = 0;
    int firstLineIndex = -1;
    int lastLineIndex = -1;
    
    // First pass: collect line indices
    for (auto* line = lines->FirstChildElement("Line"); line; line = line->NextSiblingElement("Line")) {
        // Get the line index
        int lineIndex = 0;
        if (line->Attribute("index")) {
            lineIndex = std::stoi(line->Attribute("index"));
        }
        
        // Get the NoteColumns
        auto* noteColumns = line->FirstChildElement("NoteColumns");
        if (!noteColumns) continue;
        
        // Get the NoteColumn
        auto* noteColumn = noteColumns->FirstChildElement("NoteColumn");
        if (!noteColumn) continue;
        
        // Get the Note and Instrument
        auto* note = noteColumn->FirstChildElement("Note");
        auto* instrument = noteColumn->FirstChildElement("Instrument");
        
        if (note && instrument) {
            if (firstLineIndex == -1) {
                firstLineIndex = lineIndex;
            }
            lastLineIndex = lineIndex;
        }
    }
    
    if (firstLineIndex == -1 || lastLineIndex == -1) {
        std::cout << "No notes found in pattern" << std::endl;
        return;
    }
    
    // Calculate the time range
    float startTime = firstLineIndex * (60.0f / sharedData->bpm);
    float endTime = lastLineIndex * (60.0f / sharedData->bpm);
    float timeRange = endTime - startTime;
    
    // Calculate the total pattern length in seconds
    float totalPatternLength = 64.0f * (60.0f / sharedData->bpm); // 64 lines at current BPM
    
    // Second pass: process notes and scale timestamps
    notes4.clear();
    
    // Check if line 0 has no notes and add a pause
    auto* firstLine = lines->FirstChildElement("Line");
    if (firstLine) {
        int lineIndex = 0;
        if (firstLine->Attribute("index")) {
            lineIndex = std::stoi(firstLine->Attribute("index"));
        }
        
        if (lineIndex == 0) {
            auto* noteColumns = firstLine->FirstChildElement("NoteColumns");
            if (noteColumns) {
                auto* noteColumn = noteColumns->FirstChildElement("NoteColumn");
                if (noteColumn) {
                    auto* note = noteColumn->FirstChildElement("Note");
                    auto* instrument = noteColumn->FirstChildElement("Instrument");
                    
                    if (!note && !instrument) {
                        notes4.push_back({0, 0, 0}); // Add pause at start
                        std::cout << "Added pause at start of pattern" << std::endl;
                    }
                }
            }
        }
    }
    
    for (auto* line = lines->FirstChildElement("Line"); line; line = line->NextSiblingElement("Line")) {
        // Get the line index
        int lineIndex = 0;
        if (line->Attribute("index")) {
            lineIndex = std::stoi(line->Attribute("index"));
        }
        
        // Get the NoteColumns
        auto* noteColumns = line->FirstChildElement("NoteColumns");
        if (!noteColumns) continue;
        
        // Get the NoteColumn
        auto* noteColumn = noteColumns->FirstChildElement("NoteColumn");
        if (!noteColumn) continue;
        
        // Get the Note and Instrument
        auto* note = noteColumn->FirstChildElement("Note");
        auto* instrument = noteColumn->FirstChildElement("Instrument");
        
        if (note && instrument) {
            // Convert note string to MIDI number
            std::string noteStr = note->GetText();
            int midiNote = convertRenoiseNoteToMidi(noteStr);
            
            // Convert instrument string to number
            std::string instStr = instrument->GetText();
            int instrumentNum = std::stoi(instStr, nullptr, 16);
            
            // Calculate timestamp based on line index
            float timestamp = lineIndex * (60.0f / sharedData->bpm);
            
            // Scale timestamp to range 0-1 using total pattern length
            float scaledTimestamp = timestamp / totalPatternLength;
            
            // Create and add the note
            Note3 newNote(scaledTimestamp, instrumentNum, midiNote);
            notes4.push_back(newNote);
            notesProcessed++;
            
            std::cout << "Processed note: line=" << lineIndex 
                      << " note=" << noteStr << " inst=" << instStr 
                      << " midi=" << midiNote << " timestamp=" << scaledTimestamp << std::endl;
        }
    }
    
    if (notesProcessed == 0) {
        std::cout << "No notes were processed from the XML" << std::endl;
        return;
    }
    
    // Add a final timestamp at the end of the pattern
    float finalTimestamp = 1.0f; // Set to 1.0 to indicate end of pattern
    Note3 finalNote(finalTimestamp, 0, 0); // Use zero for instrument and note to indicate end
    notes4.push_back(finalNote);
    
    // Update the current pattern
    currentPattern = (currentPattern + 1) % numPatterns;
    pnotes[currentPattern] = notes4;
    
    std::cout << "Successfully converted " << notesProcessed << " notes" << std::endl;
}



void PatternRecorder::modul1() {
    int select;
    std::string value = "";
    std::string valuea = "";
    std::string valuee = "";
    cout<<"time 1, istrument 2, pitch 3"<<endl;
    value=Eingabe();
    select=stoi(value);
        cout<<"Anfangs- und Endabweichung in %"<<endl;
    valuea=Eingabe();
    valuee=Eingabe();

    for (float i=0; i<notes4.size();++i) {
        switch (select) {
            case 1:
                mod1.push_back({notes4[i].timestamp+notes4[i].timestamp*((stof(valuee)-stof(valuea))/(notes4.size()-1)*i/100), notes4[i].instrument, notes4[i].pitch});
                break;
            case 2:
                mod1.push_back({notes4[i].timestamp, notes4[i].instrument+notes4[i].instrument*((stof(valuee)-stof(valuea))/(notes4.size()-1)*i/100), notes4[i].pitch});
                break;
            case 3:
                mod1.push_back({notes4[i].timestamp, notes4[i].instrument, notes4[i].pitch+notes4[i].pitch*((stof(valuee)-stof(valuea))/(notes4.size()-1)*i/100)});
                break;
            default :
                cout<<"Invalid number!"<<endl;
        }
        cout<<i<<" "<<mod1[i].time<<" "<<mod1[i].instrument<<" "<<mod1[i].pitch<<endl;
        cout<<i<<" "<<notes4[i].timestamp<<" "<<notes4[i].instrument<<" "<<notes4[i].pitch<<endl;
    }
mod1.push_back({1,0,0});
}
void PatternRecorder::createGraphWindow() {
    cout<<"DrawGraph"<<endl;
    SDL_Window* graphWindow = SDL_CreateWindow("Graph Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* graphRenderer = SDL_CreateRenderer(graphWindow, -1, SDL_RENDERER_ACCELERATED);
    
    vector<SDL_Point> pitchGraph;
    vector<SDL_Point> stepGraph;
    vector<SDL_Point> pitchGraph2;
    vector<SDL_Point> stepGraph2;
    vector<SDL_Point> pitchstepGraph;
    vector<xy> stepxy;
    vector<xy> pitchstepGraph2;
    float startTime, endTime;
    float totalDuration; // Gesamtdauer des Zeitbereichs
    float startIndexnum, endIndexnum;
    cout << "Geben Sie den Anfangsindexnum ein: ";
    cin >> startIndexnum;
    cout << "Geben Sie den Endindexnum ein: ";
    cin >> endIndexnum;
    startTime = static_cast<float>(notes4[startIndexnum].timestamp);
    endTime = static_cast<float>(notes4[endIndexnum].timestamp);
    totalDuration = endTime - startTime; // Gesamtdauer des Zeitbereichs
    cout<<startTime<<" "<<endTime<<" "<<totalDuration<<endl;

    notes4.erase(notes4.begin() + startIndexnum, notes4.begin() + endIndexnum);
    cout << "Noten im Bereich [" << startIndexnum << ", " << endIndexnum << "] gelöscht." << endl;
    // Ausgabe des endgültigen notes4 Vectors im Terminal
    cout << "Endgültige Noten4:" << endl;
    for (const auto& note : notes4) {
       cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }


    // for (int i = 0; i <=799; ++i) {
    //     pitchGraph.push_back({i, 0});
    //     stepGraph.push_back({i, 0});
    // }

    
    bool quit = false;
    SDL_Event event;
    bool mousedownhold = false;
    bool aefill=false;
    bool graphcout=false;
    bool stepGraph2fill=false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_0) {
                    quit = true;
                } else if (event.key.keysym.sym == SDLK_f) {
                    stepGraph2fill=true;
                } else if (event.key.keysym.sym == SDLK_v) {
                    aefill = true;
                } else if (event.key.keysym.sym == SDLK_o) {
                    graphcout = true;
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (y < 200) { // Obere Hälfte für Tonhöhe
                // pitchGraph[x].y=y;
                pitchGraph.push_back({x, y});
            } else if (y<400 && y>200) { // Untere Hälfte für Schrittweite
                // stepGraph[x].y=y-200;
                stepGraph.push_back({x, y - 200});
            }
                // mousedownhold = true;
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                // mousedownhold = false;
            }
        }
        
        if (mousedownhold==true) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (y < 200) { // Obere Hälfte für Tonhöhe
                // pitchGraph[x].y=y;
                pitchGraph.push_back({x, y});
            } else if (y<400 && y>200) { // Untere Hälfte für Schrittweite
                // stepGraph[x].y=y-200;
                stepGraph.push_back({x, y - 200});
            }
        }
        bool xo=true;
        if (stepGraph2fill==true) {
            for (int i=0; i<=799; ++i) {
                stepxy.push_back({i,0});
            }
            for (int i=0; i<=stepGraph.size()-2; ++i) {
                float diffx;
                float diffy;
                if (stepGraph[i].x!=0 && xo==true) {
                    diffx=static_cast<float>(stepGraph[i].x);
                    diffy=static_cast<float>(stepGraph[i].y);

                    for (int ii=0;ii<static_cast<int>(stepGraph[i].x);++ii) {
                        int pitch;
                        pitch=(int)((ii+1)*(diffy/diffx));
                        stepxy[ii].y=pitch;
                        cout<<"diffx: "<<diffx<<"diffy: "<<diffy<<endl;
                        cout<<static_cast<int>(stepxy[ii].x)<<" "<<static_cast<int>(stepxy[ii].y)<<endl;
                    }
                    xo=false;
                }
                    diffx=static_cast<float>(stepGraph[i+1].x)-static_cast<float>(stepGraph[i].x);
                    diffy=static_cast<float>(stepGraph[i+1].y)-static_cast<float>(stepGraph[i].y);    
                    for (int ii=static_cast<int>(stepGraph[i].x);ii<static_cast<int>(stepGraph[i+1].x);++ii) {
                        int pitch;
                        if (diffy<0){
                            pitch=static_cast<int>(stepGraph[i].y)-(int)(((ii+1)-static_cast<int>(stepGraph[i].x))*(diffy/diffx));
                        }
                        else if (diffy>0){
                            pitch=static_cast<int>(stepGraph[i].y)+(int)(((ii+1)-static_cast<int>(stepGraph[i].x))*(diffy/diffx));
                        }
                        stepxy[ii].y=pitch;
                        cout<<"diffx: "<<diffx<<"diffy: "<<diffy<<endl;
                        cout<<static_cast<int>(stepxy[ii].x)<<" "<<static_cast<int>(stepxy[ii].y)<<endl;
                }
            }
            // Ausgabe des stepGraph2 Vectors im Terminal
            cout << "Endgültige stepGraph2:" << endl;
            for (const auto& note : stepxy) {
            cout << note.x << ", " << note.y << endl;
            }
        stepGraph2fill=false;
        }
        // for (int i=0; i<=pitchGraph.size(); ++i) {
        //     for (int j=pitchGraph[i].x; j<pitchGraph[i+1].x; ++j) {
        //         float diffy=(static_cast<float>(pitchGraph[i+1].y)-static_cast<float>(pitchGraph[i].y));
        //         float diffx=1/(static_cast<float>(pitchGraph[i+1].x)-static_cast<float>(pitchGraph[i].x));
        //         pitchGraph2.push_back({j, static_cast<int>((j-static_cast<float>(pitchGraph[i].x))*(diffy/diffx))});
        //     }
        // }
        // for (int i=0; i<=stepGraph.size(); ++i) {
        //     for (int j=stepGraph[i].x; j<stepGraph[i+1].x; ++j) {
        //         float diffy=(static_cast<float>(stepGraph[i+1].y)-static_cast<float>(stepGraph[i].y));
        //         float diffx=1/(static_cast<float>(stepGraph[i+1].x)-static_cast<float>(stepGraph[i].x));
        //         stepGraph2.push_back({j,static_cast<int>((j-static_cast<float>(stepGraph[i].x))*(diffy/diffx))});
        //     }
        // }

        SDL_SetRenderDrawColor(graphRenderer, 0, 0, 0, 255);
        SDL_RenderClear(graphRenderer);
        
        // Zeichne Tonhöhen-Graph
        SDL_SetRenderDrawColor(graphRenderer, 255, 0, 0, 255);
        for (size_t i = 1; i < pitchGraph.size(); ++i) {
            SDL_RenderDrawLine(graphRenderer, pitchGraph[i-1].x, pitchGraph[i-1].y, pitchGraph[i].x, pitchGraph[i].y);
        }
        
        // Zeichne Schrittweiten-Graph
        SDL_SetRenderDrawColor(graphRenderer, 0, 255, 0, 255);
        for (size_t i = 1; i < stepGraph.size(); ++i) {
            SDL_RenderDrawLine(graphRenderer, stepGraph[i-1].x, stepGraph[i-1].y + 200, stepGraph[i].x, stepGraph[i].y + 200);
        }

        // // Zeichne Tonhöhen-Graph
        // SDL_SetRenderDrawColor(graphRenderer, 255, 0, 0, 255);
        // for (size_t i = 1; i < pitchGraph.size(); ++i) {
        //     SDL_RenderDrawLine(graphRenderer, pitchGraph2[i-1].x, pitchGraph2[i-1].y, pitchGraph2[i].x, pitchGraph2[i].y);
        // }
        
        // // Zeichne Schrittweiten-Graph
        // SDL_SetRenderDrawColor(graphRenderer, 0, 255, 0, 255);
        // for (size_t i = 1; i < stepGraph.size(); ++i) {
        //     SDL_RenderDrawLine(graphRenderer, stepGraph2[i-1].x, stepGraph2[i-1].y + 200, stepGraph2[i].x, stepGraph2[i].y + 200);
        // }
        
        SDL_RenderPresent(graphRenderer);
        if (aefill==true) {
            pitchstepGraph2.clear();
            float fi = 0;
            if (stepGraph[0].x!=0) pitchstepGraph2.push_back({0, 0});
            for (int i = 0; i <= stepxy.size(); ++i) {
                if (stepxy[i].y!=0) {
                    fi=stepxy[i].y;
                    fi=fi*1;
                    i = i+(int)(fi);
                    if (i>stepxy.size()) break;
                    int y = stepxy[i].y;
                    pitchstepGraph2.push_back({i, y});
                }
            }
            // Ausgabe des endgültigen pitchstepGraph Vectors im Terminal
            cout << "Endgültige pitchstepGraph2:" << endl;
            for (const auto& note : pitchstepGraph2) {
            cout << note.x << ", " << note.y << endl;
            }
            // int ymap = std::max_element(pitchstepGraph.begin(), pitchstepGraph.end(),
            // [](const SDL_Point& a, const SDL_Point& b) { return a.y < b.y; })->y / 200;    
            // Zeichne Schrittweiten-Graph

            // SDL_SetRenderDrawColor(graphRenderer, 0, 0, 255, 255);
            // for (size_t i = 1; i <= pitchstepGraph.size(); ++i) {
            //     SDL_RenderDrawLine(graphRenderer, pitchstepGraph[i-1].x, pitchstepGraph[i-1].y + 400, pitchstepGraph[i].x, pitchstepGraph[i].y + 400);
            // }
            
            // SDL_RenderPresent(graphRenderer);
        aefill=false;
        }
        if (graphcout==true){
            cout<<"pitchGraph x :"<<std::setw(15)<<"pitchGraph y :"<<endl;
            for (int i = 0; i<=pitchGraph.size()-1; ++i) {
                cout<<pitchGraph[i].x<<std::setw(15)<<pitchGraph[i].y<<endl;
            }
            cout<<"stepGraph x :"<<std::setw(15)<<"stepGraph y :"<<endl;
            for (int i = 0; i<=stepGraph.size()-1; ++i) {
                cout<<stepGraph[i].x<<std::setw(15)<<stepGraph[i].y<<endl;
            }
        graphcout=false;
        }
    }

   
    
    // Konvertiere Graphen in Noten
    vector<Note3> newNotes;
    int pitchstepGraphsize=pitchstepGraph2.size()-1;
    cout<<"size"<< pitchstepGraphsize << endl;
    float psge;
    psge = static_cast<float>(pitchstepGraph2[pitchstepGraphsize].x);
    float psgf = 1/psge;
    cout<<psge<<" "<<psgf<<endl;
    for (int i = 0; i < pitchstepGraph2.size(); ++i) {
        float psgi = static_cast<float>(pitchstepGraph2[i].x);
        float normalizedTime = psgi*psgf;
        int pitch = 60 + ((static_cast<float>(pitchstepGraph2[i].y)) / 10);
        int instr = 30; 
        newNotes.push_back(Note3(normalizedTime, instr, pitch));
    }
    for (const auto& note : newNotes) {
       cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }

    cout<<startTime<<" "<<endTime<<endl;
    vector<Note3> newNotes2;
    for (int i = 0; i < newNotes.size(); ++i) {
        float mappedTime = startTime + static_cast<float>(newNotes[i].timestamp) * (endTime - startTime);
        newNotes2.push_back(Note3(mappedTime, newNotes[i].instrument, newNotes[i].pitch));
    }
    for (const auto& note : newNotes2) {
       cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }


        notes4.insert(notes4.begin() + startIndexnum, newNotes2.begin(), newNotes2.end()-1);
    // Ausgabe des endgültigen notes3 Vectors im Terminal
    cout << "Endgültige Noten4:" << endl;
    for (const auto& note : notes4) {
       cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }
    
    SDL_DestroyRenderer(graphRenderer);
    SDL_DestroyWindow(graphWindow);
}

void PatternRecorder::zeitfaktor() {
    sharedData->zeitfakto=((60.0/sharedData->bpm)*sharedData->dauer);
    cout<<sharedData->zeitfakto<<endl;
}
std::string PatternRecorder::Eingabe()
{
    std::string input = "";
    while (true) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN) {
                    return input;
                }
                if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) {
                    int value = event.key.keysym.sym - SDLK_0;
                    input += std::to_string(value);
                    cout << "input: " << input << endl;
                } else if (event.key.keysym.sym == SDLK_DECIMALSEPARATOR) {
                    char value = event.key.keysym.sym;
                    input += std::to_string(value);
                    cout << "input: " << input << endl;
                }
            }
        }
    }
}
void PatternRecorder::select(int num) {
    cout << pnotes[num].size() << " num " << num << endl;

    // Ensure pselect is resized to accommodate num
    if (num >= pselect.size()) {
        pselect.resize(num + 1); // Resize to accommodate the index
    }

    // Clear previous selections for pselect[num]
    pselect[num].clear(); 

    cout << "1 for select, 0 for free" << endl;
    for (int i = 0; i < pnotes[num].size(); ++i) {
        cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
        std::string value = "";
        value = Eingabe();
        int sel = std::stoi(value);
        pselect[num].push_back(sel); // Correctly push back the selection
    }
}
void PatternRecorder::setpdauer(int num) {
    cout << "Setze Dauer für Pattern " << num << endl;
    pdauer[num][0] = std::stoi(Eingabe());
}
void PatternRecorder::reverse(int num) {
    for (int i=0;i<pnotes[num].size();++i) {
        if (pselect[num][i]==1) {
            cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
            pnotes[num][i].timestamp = 1- pnotes[num][i].timestamp;  
            cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
        }
    }    
    std::sort(pnotes[num].begin(), pnotes[num].end(), [](const Note3& a, const Note3& b) {
    return a.timestamp < b.timestamp;});
    for (int i=0;i<pnotes[num].size();++i) {
        cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
    }
}
void PatternRecorder::mal(int num, float factor) {
    for (int i=0;i<pnotes[num].size();++i) {
        if (pselect[num][i]==1) {
            cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
            pnotes[num][i].timestamp = pnotes[num][i].timestamp*factor;  
            cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
        }
    }    
    std::sort(pnotes[num].begin(), pnotes[num].end(), [](const Note3& a, const Note3& b) {
    return a.timestamp < b.timestamp;});
    for (int i=0;i<pnotes[num].size();++i) {
        cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
    }
}
void PatternRecorder::durch(int num, float factor) {
    for (int i=0;i<pnotes[num].size();++i) {
        if (pselect[num][i]==1) {
            cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
            pnotes[num][i].timestamp = pnotes[num][i].timestamp/factor;  
            cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
        }
    }    
    std::sort(pnotes[num].begin(), pnotes[num].end(), [](const Note3& a, const Note3& b) {
    return a.timestamp < b.timestamp;});
    for (int i=0;i<pnotes[num].size();++i) {
        cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
    }
}
void PatternRecorder::plus(int num, float factor) {
    for (int i=0;i<pnotes[num].size();++i) {
        if (pselect[num][i]==1) {
            cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
            pnotes[num][i].timestamp = pnotes[num][i].timestamp+factor;  
            cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
        }
    }    
    std::sort(pnotes[num].begin(), pnotes[num].end(), [](const Note3& a, const Note3& b) {
    return a.timestamp < b.timestamp;});
    for (int i=0;i<pnotes[num].size();++i) {
        cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
    }
}
void PatternRecorder::minus(int num, float factor) {
    for (int i=0;i<pnotes[num].size();++i) {
        if (pselect[num][i]==1) {
            cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
            pnotes[num][i].timestamp = pnotes[num][i].timestamp-factor;  
            cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
        }
    }    
    std::sort(pnotes[num].begin(), pnotes[num].end(), [](const Note3& a, const Note3& b) {
    return a.timestamp < b.timestamp;});
    for (int i=0;i<pnotes[num].size();++i) {
        cout << i << " " << pnotes[num][i].timestamp << ", " << pnotes[num][i].instrument << ", " << pnotes[num][i].pitch << endl;
    }
}
void PatternRecorder::vectorinsert(int num) {
    notes4.insert(notes4.begin(),pnotes[num].begin(),pnotes[num].end());
    std::sort(notes4.begin(), notes4.end(), [](const Note3& a, const Note3& b) {
        return a.timestamp < b.timestamp;
    });
    for (int i=0;i<notes4.size();++i) {
        cout << i << " " << notes4[i].timestamp << ", " << notes4[i].instrument << ", " << notes4[i].pitch << endl;
    }
}
// Helper function to compute the greatest common divisor
int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}

// Approximates a float as a fraction with denominator up to max_denominator
std::pair<int, int> floatToFraction(double timestampvalue, int max_denominator = 192, double epsilon = 1e-4) {
    int best_numer = 1, best_denom = 1;
    double best_error = std::abs(timestampvalue - 1.0);

    for (int denom = 1; denom <= max_denominator; ++denom) {
        int numer = static_cast<int>(std::round(timestampvalue * denom));
        double error = std::abs(timestampvalue - static_cast<double>(numer) / denom);
        if (error < best_error) {
            best_error = error;
            best_numer = numer;
            best_denom = denom;
        }
        if (error < epsilon) break; // good enough
    }
    // Reduce fraction
    int divisor = gcd(best_numer, best_denom);
    return {best_numer / divisor, best_denom / divisor};
}

// Converts a float to a musical fraction string
std::string rhythmFraction(double timestampvalue) {
    auto frac = floatToFraction(timestampvalue);
    return std::to_string(frac.first) + "/" + std::to_string(frac.second);
}
void PatternRecorder::DisPlNotes() {

    cout << "notes3" << endl;
    for (int i=0; i<notes3.size();++i) {
        cout << i << std::setw(10) << rhythmFraction(notes3[i].timestamp) << std::setw(10) << notes3[i].timestamp*sharedData->zeitfakto << ", " << notes3[i+1].timestamp*sharedData->zeitfakto-(notes3[i].timestamp*sharedData->zeitfakto) << ", " << notes3[i].instrument << ", " << notes3[i].pitch << ", " << notes3[i].p2 << endl;
    }
    cout << "notes4" << endl;
    for (int i=0; i<notes4.size();++i) {
        cout << i << std::setw(10) << rhythmFraction(notes4[i].timestamp) << std::setw(10) << notes4[i].timestamp*sharedData->zeitfakto << ", " << notes4[i+1].timestamp*sharedData->zeitfakto-(notes4[i].timestamp*sharedData->zeitfakto) << ", " << notes4[i].instrument << ", " << notes4[i].pitch << ", " << notes4[i].p2 << endl;
    }
    cout << "snotes" << endl;
    for (int i=0; i<snotes.size();++i) {
        cout << i << std::setw(10) << rhythmFraction(snotes[i].timestamp) << std::setw(10) << snotes[i].timestamp << ", " << snotes[i+1].instrument << notes4[i].pitch << endl;
    }

}

void PatternRecorder::notelenght() {
    for (int i=0; i<notes4.size(); ++i) {
        cout << notes4[i].timestamp << ", " << notes4[i].instrument << ", " << notes4[i].pitch << endl;
        std::string value = "";
        value = Eingabe();
        float stamp = std::stof(value);
        notes4[i].timestamp = stamp;
    }
    for (int i = 0; i < notes4.size(); ++i) {
        cout<<notes4[i].timestamp<<endl;   
    }
    float scldiv = notes4[notes4.size()-1].timestamp;
    cout<<scldiv<<endl;
    for (int i = 0; i < notes4.size(); ++i) {
        notes4[i].timestamp=notes4[i].timestamp/scldiv;
        cout<<notes4[i].timestamp<<endl;
    }

}
void PatternRecorder::noteglide() {
    for (int i=0; i<notes4.size(); ++i) {
        cout << notes4[i].timestamp << ", " << notes4[i].instrument << ", " << notes4[i].pitch <<", " << notes4[i].glide << endl;
        std::string value = "";
        value = Eingabe();
        float stamp = std::stof(value);
        notes4[i].glide = stamp;
    }
    for (int i = 0; i < notes4.size(); ++i) {
        cout << notes4[i].timestamp << ", " << notes4[i].instrument << ", " << notes4[i].pitch <<", " << notes4[i].glide << endl;
    }

}
void PatternRecorder::noteinstr() {
    for (int i=0; i<notes4.size(); ++i) {
        cout << notes4[i].timestamp << ", " << notes4[i].instrument << ", " << notes4[i].pitch << endl;
        std::string value = "";
        value = Eingabe();
        float stamp = std::stof(value);
        notes4[i].instrument = stamp;
    }
    for (int i = 0; i < notes4.size(); ++i) {
        cout<<notes4[i].instrument<<endl;   
    }
}
void PatternRecorder::fdx() {
    cout<<"Geben Sie den Anfangsindex ein: ";
    std::string value = "";
    value = Eingabe();
    int Anfangsindex = std::stof(value);

    cout<<"Geben Sie den Endindex ein: ";
    value = Eingabe();
    int Endindex = std::stof(value);

    cout<<"Geben Sie die Frequenzspanne in Halbtönen ein: ";
    value = Eingabe();
    float fd = std::stof(value);
    cout<<"Geben Sie den Frequenzspannenteiler ein: ";
    value = Eingabe();
    float fdx = std::stof(value);
    float df = fd / fdx;
    float df2=60;
    int ii=1;
    for (int i=Anfangsindex; i<Endindex; ++i) {
        cout << notes4[i].timestamp << ", " << notes4[i].instrument << ", " << notes4[i].pitch << endl;
        if (ii>fdx) {
            ii=1;
            df2+=df*ii;
            df2+=12;
        }
        else {
            df2+=df*ii;
        }
        ii++;
        notes4[i].pitch = df2;
        
    }
    for (int i = 0; i < notes4.size(); ++i) {
        cout << notes4[i].timestamp << ", " << notes4[i].instrument << ", " << notes4[i].pitch << endl;
    }

}
void PatternRecorder::anfendIndex() {
    std::string svalue = "";
    cout << "Geben Sie den Anfangsindex (0 bis " << notes4.size() - 1 << ") ein: "<<endl;;
    svalue = Eingabe();
    startIndex = std::stoi(svalue);
    svalue = "";
    cout << "Geben Sie den Endindex (0 bis " << notes4.size() << ") ein: "<<endl;
    svalue = Eingabe();
    endIndex = std::stoi(svalue);
    if (endIndex==notes4.size()) endTime = 1;
}
void PatternRecorder::record() {
    notes.clear();
    snotes.clear();
    newNotes.clear();

    notestoscale.clear();
    cout << "Recording... Press '0' to stop." << endl;
    float currentTimestampstart = SDL_GetTicks() / 1000.0f; // Zeit relativ zur Startzeit
    float currentTimestamp = SDL_GetTicks() / 1000.0f;
    bool recordingStarted, recordingStarted0 = false; // Flag für Aufnahmebeginn
    bool settimestamp=false;
    bool alleseins = false;
    bool scaletoa = false;
    bool mschrittweitanz = false;
    int numfactor = 1;
    float rschrittweitanz =1.0f;  
    while (true) {
        SDL_Event event;
        int noteValue;
        int noteValue2;
        int noteValue3;
        int scl = 0;

        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_0) {
                    break; // Stop recording
                } else if (event.key.keysym.sym == SDLK_a) {
                    alleseins = !alleseins;
                } else if (alleseins && !scaletoa && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9)) {
                    noteValue = 1;
                    settimestamp=true;
                } else if (!scaletoa && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9)) {
                    noteValue = event.key.keysym.sym - SDLK_0;
                    noteValue = noteValue * numfactor;
                    settimestamp=true;
                } else if (scaletoa && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9)) {
                    noteValue = event.key.keysym.sym - SDLK_0;
                    noteValue = noteValue * numfactor;
                    settimestamp=true;
                    // if (scl==0) {
                    //     schrittweitanz=noteValue;
                    //     scaletoa=false;
                    // }
                } else if (event.key.keysym.sym == SDLK_PAGEUP) {
                    pgup=1;
                } else if (event.key.keysym.sym == SDLK_PAGEDOWN) {
                    pgup=0;
                } else if (event.key.keysym.sym == SDLK_s) {
                    kk=true;
                    kl=false;
                    kr=false;
                    scaletoa=!scaletoa;
                } else if (event.key.keysym.sym == SDLK_a) {
                    kl=true;
                    kr=false;
                    kk=false;
                    scaletoa=!scaletoa;
                } else if (event.key.keysym.sym == SDLK_d) {
                    kr=true;
                    kl=false;
                    kk=false;
                    scaletoa=!scaletoa;
                } else if (event.key.keysym.sym == SDLK_w) {
                    kk=false;
                    kl=false;
                    kr=false;
                    scaletoa=!scaletoa;
                } else if (event.key.keysym.sym == SDLK_y) {
                    schrittweitanz=3;
                    mschrittweitanz=true;
                } else if (event.key.keysym.sym == SDLK_q) {
                    numfactor=2;
                } else if (event.key.keysym.sym == SDLK_e) {
                    numfactor=4;
                } else if (event.key.keysym.sym == SDLK_x) {
                    rschrittweitanz=7.0f;
                    mschrittweitanz=true;
                } else if (event.key.keysym.sym == SDLK_c) {
                    rschrittweitanz=15.0f;
                    mschrittweitanz=true;
                } else if (event.key.keysym.sym == SDLK_v) {
                    rschrittweitanz=2.0f;
                    mschrittweitanz=true;
                } else if (event.key.keysym.sym == SDLK_b) {
                    rschrittweitanz=5.0f;
                    mschrittweitanz=true;
                } else if (event.key.keysym.sym == SDLK_n) {
                    rschrittweitanz=11.0f;
                    mschrittweitanz=true;
                } else if (event.key.keysym.sym == SDLK_m) {
                    rschrittweitanz=23.0f;
                    mschrittweitanz=true;
                } else if (event.key.keysym.sym >= SDLK_F1 && event.key.keysym.sym <= SDLK_F10) {
                    noteValue = event.key.keysym.sym - SDLK_F1+1; //+10 für Pause
                    noteValue = noteValue * numfactor;
                    kk=true;
                    kl=false;
                    kr=false;
                    scaletoa=true;
                    settimestamp=true;
                    numfactor=1;
                    // Setze den Zeitstempel beim ersten Tastendruck
                }
            // } else if (event.type == SDL_KEYUP) {
            //     scaletoa=false;
            }
        }

            if (settimestamp) {
                if (!recordingStarted) {
                    currentTimestampstart = SDL_GetTicks() / 1000.0f; // Zeit relativ zur Startzeit
                    // Füge die Note zur Liste hinzu, Instrument ist die Anzahl der bisherigen Noten
                    notes.push_back({0, static_cast<int>(notes.size()), noteValue, scaletoa, mschrittweitanz, rschrittweitanz}); // Instrument ist die Anzahl der bisherigen Noten
                    recordingStarted = true; // Aufnahme als gestartet markieren
                }
                else if (recordingStarted){
                    currentTimestamp = SDL_GetTicks() / 1000.0f - currentTimestampstart; // Zeit relativ zur Startzeit

                    // Füge die Note zur Liste hinzu, Instrument ist die Anzahl der bisherigen Noten
                    notes.push_back({currentTimestamp, static_cast<int>(notes.size()), noteValue, scaletoa, mschrittweitanz, rschrittweitanz}); // Instrument ist die Anzahl der bisherigen Noten
                }
                if (notes[notes.size()-2].scl==1) {
                    scaletoa=false;
                }
                settimestamp=false;
                mschrittweitanz=false;
            }
    }            

    // Füge die Endzeit hinzu
    float endTime = SDL_GetTicks() / 1000.0f - currentTimestampstart; // Berechne die Endzeit
    notes.push_back({endTime, 0, 0, scaletoa, 0, 0}); // Füge eine Dummy-Note für Endzeit hinzu
    for (const auto& note : notes) {
        cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << ", " << note.scl << ", " << note.mschrittweitanz << ", " << note.schrittweitanz << endl;
    }
    int ie = notes.size();
    cout<<"ie="<<ie<<endl;
    for (int i=0; i<ie; ++i) {
            if(notes[i].scl==1 && notes[i+1].scl==1) {
                firstrec=true;
                startTime=notes[i].timestamp;
                endTime=notes[i+2].timestamp;
                startIndexnum=(float)(notes[i].pitch);
                endIndexnum=(float)(notes[i+1].pitch);
                // schrittweitanz=(float)(notes[i+2].pitch);
                if(kk=true) Indexnumlaenge = 1 - (1/startIndexnum)- (1/endIndexnum);
                else if (kl=true) Indexnumlaenge = 1 - (1/startIndexnum);
                else if (kr=true) Indexnumlaenge = 1 - (1/endIndexnum);
                else Indexnumlaenge = 1;
                // if (notes[i].mschrittweitanz==false) {
                    rschrittweitanz=(endIndexnum-startIndexnum)*(endIndexnum-startIndexnum)/(endIndexnum-startIndexnum);
                    scaleToRangenum(newNotes, startTime, endTime, startIndexnum, endIndexnum, rschrittweitanz, firstrec);
                // }
                // else scaleToRangenum(newNotes, startTime, endTime, startIndexnum, endIndexnum, notes[i+1].schrittweitanz, firstrec);
                notes[i+1].pitch=1;
                notes[i].scl=0;
                notes[i+1].scl=0;
                notes.erase(notes.begin() + i, notes.begin() + i+2);
                notes.insert(notes.begin() + i, notesr.begin(), notesr.end());
                ie=notes.size();
                notesr.clear();
                i=0;
                firstrec=false;
            }
        
    }        
    
    // Pausen berechnen
    for (int i=0; i <= notes.size()-1; ++i) {
       if (notes[i].pitch>9) {
        int pitch2 = static_cast<int>(notes[i].pitch)-9;
        float stamp = (endTime-startTime)/(float)(pitch2);
        notes[i].timestamp=stamp;
        notes[i].instrument=1000;
        notes[i].pitch=0; 
       }
    }

    // // scalerangenum
    // if (notestoscale.empty()){
    //     cout<<"No scalingto"<<endl;
    // } else {
    //     for (int i=0; i<notestoscale.size();++i) {
    //         startTime=notestoscale[i].timestamp1;
    //         endTime=notestoscale[i].timestamp2;
    //         startIndex=notestoscale[i].startIndexrecord;
    //         startIndexnum=notestoscale[i].notev1;
    //         endIndexnum=notestoscale[i].notev2;
    //         schrittweitanz=(endIndexnum-startIndexnum)*(endIndexnum-startIndexnum)/(endIndexnum-startIndexnum);
    //         scaleToRangenum(newNotes, startTime, endTime, startIndexnum, endIndexnum, schrittweitanz, firstrec, notestoscale);
    //     }
    // }

    // // Ausgabe des notes Vectors im Terminal
    // cout << "Aufgenommene Noten:" << endl;
    // for (const auto& note : notes) {
    //     cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    // }

    for (const auto& note : notes) {
        cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << ", " << note.scl << endl;
    }

    scale(); // Aufruf der Skalierungsfunktion
    firstrec=false;
}

// void PatternRecorder::record() {
//     notes.clear();
//     cout << "Recording... Press '0' to stop." << endl;
//     float currentTimestampstart = SDL_GetTicks() / 1000.0f; // Zeit relativ zur Startzeit
//     float currentTimestamp = SDL_GetTicks() / 1000.0f;
//     bool recordingStarted = false; // Flag für Aufnahmebeginn
//     bool settimestamp=false;
//     bool alleseins = false;

//     while (true) {
//         SDL_Event event;
//         if (SDL_PollEvent(&event)) {
//             int noteValue;
//             if (event.type == SDL_KEYDOWN) {
//                 if (event.key.keysym.sym == SDLK_0) {
//                     break; // Stop recording
//                 } else if (event.key.keysym.sym == SDLK_a) {
//                     alleseins = !alleseins;
//                 } else if (alleseins && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9)) {
//                     noteValue = 1;
//                     settimestamp=true;
//                 } else if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9) {
//                     noteValue = event.key.keysym.sym - SDLK_0;
//                     settimestamp=true;
//                 } else if (event.key.keysym.sym >= SDLK_F1 && event.key.keysym.sym <= SDLK_F10) {
//                     noteValue = (event.key.keysym.sym - SDLK_F1) + 10;
//                     settimestamp=true;
//                     // Setze den Zeitstempel beim ersten Tastendruck
//                 }
//             }
//             if (settimestamp==true) {
//                 if (!recordingStarted) {
//                     currentTimestampstart = SDL_GetTicks() / 1000.0f; // Zeit relativ zur Startzeit
//                     // Füge die Note zur Liste hinzu, Instrument ist die Anzahl der bisherigen Noten
//                     notes.push_back({0, static_cast<int>(notes.size()), noteValue}); // Instrument ist die Anzahl der bisherigen Noten
//                     recordingStarted = true; // Aufnahme als gestartet markieren
//                 }
//                 else {
//                     currentTimestamp = SDL_GetTicks() / 1000.0f - currentTimestampstart; // Zeit relativ zur Startzeit

//                     // Füge die Note zur Liste hinzu, Instrument ist die Anzahl der bisherigen Noten
//                     notes.push_back({currentTimestamp, static_cast<int>(notes.size()), noteValue}); // Instrument ist die Anzahl der bisherigen Noten
//                 }
//                 settimestamp=false;
//             }
//         }
//     }
//     // Füge die Endzeit hinzu
//     float endTime = SDL_GetTicks() / 1000.0f - currentTimestampstart; // Berechne die Endzeit
//     notes.push_back({endTime, 0, 0}); // Füge eine Dummy-Note für Endzeit hinzu

//     // Pausen berechnen
//     for (int i=0; i <= notes.size()-1; ++i) {
//        if (notes[i].pitch>9) {
//         int pitch2 = static_cast<int>(notes[i].pitch)-9;
//         float stamp = (endTime-startTime)/(float)(pitch2);
//         notes[i].timestamp=stamp;
//         notes[i].instrument=1000;
//         notes[i].pitch=0; 
//        }
//     }

//     // Ausgabe des notes Vectors im Terminal
//     cout << "Aufgenommene Noten:" << endl;
//     for (const auto& note : notes) {
//         cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
//     }

//     scale(); // Aufruf der Skalierungsfunktion
// }

void PatternRecorder::removeAndRecord() {
    newNotes.clear();
    scaledNotes.clear();
    scaledNotes2.clear();
    if (notes4.empty()) {
        cout << "No notes recorded." << endl;
        return;
    }
    // cout << "Geben Sie den Anfangsindex (0 bis " << notes4.size() - 1 << ") ein: ";
    // cin >> startIndex;
    // cout << "Geben Sie den Endindex (0 bis " << notes4.size() - 1 << ") ein: ";
    // cin >> endIndex;

    if (startIndex < 0 || static_cast<size_t>(endIndex) > notes4.size() || startIndex > endIndex) {
        cout << "Ungültige Indices." << endl;
        return;
    }

    int instrument = notes4[startIndex].instrument;
    startTime = notes4[startIndex].timestamp;
    endTime = notes4[endIndex].timestamp;
    bool recordingStarted = false; // Flag für Aufnahmebeginn

    notes4.erase(notes4.begin() + startIndex, notes4.begin() + endIndex);
    cout << "Noten im Bereich [" << startIndex << ", " << endIndex << "] gelöscht." << endl;
    // Ausgabe des endgültigen notes4 Vectors im Terminal
    cout << "Endgültige Noten4:" << endl;
    for (const auto& note : notes4) {
       cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }

    cout << "Aufnahme neuer Noten... Drücken Sie '0' zum Stoppen." << endl;
    float recordingStartTime = SDL_GetTicks() / 1000.0f;
    bool settimestamp=false;
    int noteValue;
    bool alleseins = false;

    while (true) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            int noteValue;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_0) {
                    break;
                } else if (event.key.keysym.sym == SDLK_a) {
                    alleseins = !alleseins;
                } else if (alleseins && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9)) {
                    noteValue = 1;
                    settimestamp=true;
                } else if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9) {
                    noteValue = event.key.keysym.sym - SDLK_0;
                    settimestamp=true;
                } else if (event.key.keysym.sym >= SDLK_F1 && event.key.keysym.sym <= SDLK_F10) {
                    noteValue = (event.key.keysym.sym - SDLK_F1) + 10;
                    settimestamp=true;
                }
            }
            if (settimestamp==true) {
                if (!recordingStarted) {
                    // Füge die Note zur Liste hinzu, Instrument ist die Anzahl der bisherigen Noten
                    newNotes.push_back(Note3(0, static_cast<int>(notes3.size()), noteValue));
                    recordingStarted = true; // Aufnahme als gestartet markieren
                }
                
                else {
                    float currentTimestamp = SDL_GetTicks() / 1000.0f - recordingStartTime; // Zeit relativ zur Startzeit

                    // Füge die Note zur Liste hinzu, Instrument ist die Anzahl der bisherigen Noten
                    newNotes.push_back(Note3(currentTimestamp, static_cast<int>(notes3.size()), noteValue));
                }
            settimestamp=false;
            }
        }
    }
    // Füge die Endzeit hinzu
    float endTimerec = SDL_GetTicks() / 1000.0f - recordingStartTime; // Berechne die Endzeit
    newNotes.push_back(Note3(endTimerec, 0, 0)); // Dummy-Note für Endzeit
    // Ausgabe des newNotes Vectors im Terminal
    cout << "newNotes:" << endl;
    for (const auto& note : newNotes) {
       cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }
    // Pausen berechnen
    for (int i=0; i <= newNotes.size()-1; ++i) {
       if (newNotes[i].pitch>9) {
        int pitch2 = newNotes[i].pitch-9;
        float stamp = (endTime-startTime)/(float)(pitch2);
        newNotes[i].timestamp=stamp;
        newNotes[i].instrument=1000;
        newNotes[i].pitch=0; 
       }
    }

    scaleToRange(newNotes, startTime, endTime); // Korrigierter Aufruf
    notes4.insert(notes4.begin() + startIndex, scaledNotes2.begin(), scaledNotes2.end());
    // Ausgabe des endgültigen notes3 Vectors im Terminal
    cout << "Endgültige Noten4:" << endl;
    for (const auto& note : notes4) {
       cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }

}
void PatternRecorder::remove() {
    newNotes.clear();
    scaledNotes.clear();
    scaledNotes2.clear();
    if (notes4.empty()) {
        cout << "No notes recorded." << endl;
        return;
    }
    // cout << "Geben Sie den Anfangsindex (0 bis " << notes4.size() - 1 << ") ein: ";
    // cin >> startIndex;
    // cout << "Geben Sie den Endindex (0 bis " << notes4.size() - 1 << ") ein: ";
    // cin >> endIndex;

    if (startIndex < 0 || static_cast<size_t>(endIndex) > notes4.size() || startIndex > endIndex) {
        cout << "Ungültige Indices." << endl;
        return;
    }

    int instrument = notes4[startIndex].instrument;
    startTime = notes4[startIndex].timestamp;
    endTime = notes4[endIndex].timestamp;

    if (endTime==notes4[notes4.size()].timestamp) notes4.erase(notes4.begin()+ startIndex);
    else notes4.erase(notes4.begin() + startIndex, notes4.begin() + endIndex);

    cout << "Noten im Bereich [" << startIndex << ", " << endIndex << "] gelöscht." << endl;
    // Ausgabe des endgültigen notes4 Vectors im Terminal
    cout << "Endgültige Noten4:" << endl;
    for (const auto& note : notes4) {
       cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }
}

// void PatternRecorder::scaleToRange(vector<Note>& newNotes, float startTime, float endTime) {
//     if (newNotes.empty()) return;

//     float totalDuration = endTime - startTime; // Gesamtdauer des Zeitbereichs
//     float newDuration = newNotes.back().timestamp - newNotes.front().timestamp; // Gesamtdauer der neuen Noten

//     float scalingFactor = totalDuration / newDuration; // Berechnung des Skalierungsfaktors

//     // Mappen der neuen Noten auf den Zeitbereich
//     vector<Note> scaledNotes;
//     for (size_t i = 0; i < newNotes.size(); ++i) {
//         float scaledTimestamp = startTime + (newNotes[i].timestamp - newNotes.front().timestamp) * scalingFactor;
//         scaledNotes.push_back({scaledTimestamp, newNotes[i].instrument, newNotes[i].pitch});
//     }

//     // Ersetzen der neuen Noten durch die skalierten Noten
//     newNotes = scaledNotes;
// }


// std::vector<double> calculateSpiralRadii(double startRadius, double endRadius, double startPhi, double endPhi, std::vector<xn>& näschrittsums) {
//     double k = std::log(endRadius / startRadius) / (endPhi - startPhi);
//     std::vector<double> radii;
//     radii.reserve(näschrittsums.size());

//     double totalSum = näschrittsums.back().xn;
    
//     for (const auto& step : näschrittsums) {
//         double normalizedStep = step.xn / totalSum;
//         double phi = startPhi + normalizedStep * (endPhi - startPhi);
//         double radius = startRadius * std::exp(k * (phi - startPhi));
//         radii.push_back(radius);
//     }

//     return radii;
// }
void PatternRecorder::totalDurationbreak(float startTime, float endTime) {
    float totalDuration = endTime - startTime; // Gesamtdauer des Zeitbereichs
    cout<<"startTime, endTime, totalDuration, totalDurationbreak "<<startTime<<", "<<endTime<<", "<<totalDuration<<", "<<rhythmFraction(totalDuration)<<endl;

}
void PatternRecorder::scaleToRangenum(vector<Note3>& newNotes, float startTime, float endTime, float startIndexnum, float endIndexnum, int schrittweitanz, bool firstrec) {

    float totalDuration = endTime - startTime; // Gesamtdauer des Zeitbereichs
    cout<<"startTime, endTime, totalDuration "<<startTime<<", "<<endTime<<", "<<totalDuration<<endl;

    vector<xn> näschritts;

    float schrittwa = 0;
    float schrittwe = 0;

    schrittwa = 1/startIndexnum;
    schrittwe = 1/endIndexnum;    

    cout<<"schrittwa"<<std::setw(15)<<"schrittwe"<<std::setw(15)<<"schrittweitanz"<<endl;
    cout<<schrittwa<<std::setw(15)<<schrittwe<<std::setw(15)<<schrittweitanz<<endl;
    cout<<endl;

if (!kk && !kl && !kr) {
    if(totalDi==1) Indexnumlaenge = totalDuration - (1/startIndexnum);
    else Indexnumlaenge = 1 - (1/startIndexnum);
    vector<xn> näschrittskl;
    kl=!kl;
    float näschritt;
    näschrittskl.push_back({0,0});
    if (kk || kl) {
        näschrittskl.push_back({0,schrittwa});
        näschritt=schrittwa;
    } else näschritt=0;

    if (schrittwa>schrittwe) {
    float kd = Indexnumlaenge-schrittwe*(float)(schrittweitanz);
    int sumanz = schrittweitanz*(schrittweitanz+1)/2;
    float kdtanz = kd/sumanz;
    cout<<"kd: "<<std::setw(15)<<"sumanz: "<<std::setw(15)<<"kdtanz: "<<endl;
    cout<<kd<<std::setw(15)<<sumanz<<std::setw(15)<<kdtanz<<endl;
    cout<<endl;
    cout<<"i: "<<std::setw(15)<<" näschritt: "<<std::setw(15)<<" schrittwe+(schrittweitanz-i+1)*kdtanz: "<<std::setw(15)<<" (schrittweitanz-i+1)*kdtanz: "<<endl;
        for (float i=1; i<=schrittweitanz;++i) {
                näschritt+=schrittwe+((schrittweitanz-i+1)*kdtanz);
                cout<<i<<std::setw(15)<<näschritt<<std::setw(15)<<schrittwe+(schrittweitanz-i+1)*kdtanz<<std::setw(15)<<(schrittweitanz-i+1)*kdtanz<<endl;
                näschrittskl.push_back({i,näschritt});
        }
    }
    else {
    float kd = Indexnumlaenge-schrittwa*(float)(schrittweitanz);
    int sumanz = schrittweitanz*(schrittweitanz+1)/2;
    float kdtanz = kd/sumanz;
    cout<<"kd: "<<std::setw(15)<<"sumanz: "<<std::setw(15)<<"kdtanz: "<<endl;
    cout<<kd<<std::setw(15)<<sumanz<<std::setw(15)<<kdtanz<<endl;
    cout<<endl;
    cout<<"i: "<<std::setw(15)<<" näschritt: "<<std::setw(15)<<" schrittwa+(i*kdtanz): "<<std::setw(15)<<" schrittwa+(i*kdtanz): "<<endl;
        for (float i=1; i<=schrittweitanz;++i) {
                näschritt+=schrittwa+(i*kdtanz);
                cout<<i<<std::setw(15)<<näschritt<<std::setw(15)<<schrittwa+(i*kdtanz)<<std::setw(15)<<schrittwa+(i*kdtanz)<<endl;
                näschrittskl.push_back({i,näschritt});
        }
    }
    cout<<endl;
    if (kl) näschrittskl.pop_back();    
    kl=!kl;
    kr=!kr;

    if(totalDi==1) Indexnumlaenge = totalDuration - (1/endIndexnum);
    else Indexnumlaenge = 1 - (1/endIndexnum);

    vector<xn> näschrittskr;
    näschrittskr.push_back({0,0});
    if (kk || kl) {
        näschrittskr.push_back({0,schrittwa});
        näschritt=schrittwa;
    } else näschritt=0; 

    if (schrittwa>schrittwe) {
    float kd = Indexnumlaenge-schrittwe*(float)(schrittweitanz);
    int sumanz = schrittweitanz*(schrittweitanz+1)/2;
    float kdtanz = kd/sumanz;
    cout<<"kd: "<<std::setw(15)<<"sumanz: "<<std::setw(15)<<"kdtanz: "<<endl;
    cout<<kd<<std::setw(15)<<sumanz<<std::setw(15)<<kdtanz<<endl;
    cout<<endl;
    cout<<"i: "<<std::setw(15)<<" näschritt: "<<std::setw(15)<<" schrittwe+(schrittweitanz-i+1)*kdtanz: "<<std::setw(15)<<" (schrittweitanz-i+1)*kdtanz: "<<endl;
        for (float i=1; i<=schrittweitanz;++i) {
                näschritt+=schrittwe+((schrittweitanz-i+1)*kdtanz);
                cout<<i<<std::setw(15)<<näschritt<<std::setw(15)<<schrittwe+(schrittweitanz-i+1)*kdtanz<<std::setw(15)<<(schrittweitanz-i+1)*kdtanz<<endl;
                näschrittskr.push_back({i,näschritt});
        }
    }
    else {
    float kd = Indexnumlaenge-schrittwa*(float)(schrittweitanz);
    int sumanz = schrittweitanz*(schrittweitanz+1)/2;
    float kdtanz = kd/sumanz;
    cout<<"kd: "<<std::setw(15)<<"sumanz: "<<std::setw(15)<<"kdtanz: "<<endl;
    cout<<kd<<std::setw(15)<<sumanz<<std::setw(15)<<kdtanz<<endl;
    cout<<endl;
    cout<<"i: "<<std::setw(15)<<" näschritt: "<<std::setw(15)<<" schrittwa+(i*kdtanz): "<<std::setw(15)<<" schrittwa+(i*kdtanz): "<<endl;
        for (float i=1; i<=schrittweitanz;++i) {
                näschritt+=schrittwa+(i*kdtanz);
                cout<<i<<std::setw(15)<<näschritt<<std::setw(15)<<schrittwa+(i*kdtanz)<<std::setw(15)<<schrittwa+(i*kdtanz)<<endl;
                näschrittskr.push_back({i,näschritt});
        }
    }
    cout<<endl;
    cout<<"näschrittskl: "<<std::setw(15)<<"näschrittskr: "<<std::setw(15)<<"näschritts: "<<std::setw(15)<<"näschrittsdiff: "<<endl;
    float näschrittdiff;
    for (int i = 0; i<näschrittskl.size();i++) {
        näschrittdiff = näschrittskl[i].xn - näschrittskr[i].xn;
        if (näschrittdiff<0) {
            näschritts.push_back({static_cast<float>(i), näschrittskl[i].xn+(näschrittdiff*(-1)/2)});
        } else näschritts.push_back({static_cast<float>(i), näschrittskl[i].xn-(näschrittdiff*(-1)/2)});
        cout<<näschrittskl[i].xn<<std::setw(15)<<näschrittskr[i].xn<<std::setw(15)<<näschritts[i].xn<<std::setw(15)<<näschrittdiff<<endl;    
    }
    cout<<endl;
    kr=!kr;
} else {
    float näschritt;
    näschritts.push_back({0,0});
    if (kk || kl) {
        näschritts.push_back({0,schrittwa});
        näschritt=schrittwa;
    } else näschritt=0;

    if (schrittwa>schrittwe) {
    float kd = Indexnumlaenge-schrittwe*(float)(schrittweitanz);
    int sumanz = schrittweitanz*(schrittweitanz+1)/2;
    float kdtanz = kd/sumanz;
    cout<<"kd: "<<std::setw(15)<<"sumanz: "<<std::setw(15)<<"kdtanz: "<<endl;
    cout<<kd<<std::setw(15)<<sumanz<<std::setw(15)<<kdtanz<<endl;
    cout<<endl;
    cout<<"i: "<<std::setw(15)<<" näschritt: "<<std::setw(15)<<" schrittwe+(schrittweitanz-i+1)*kdtanz: "<<std::setw(15)<<" (schrittweitanz-i+1)*kdtanz: "<<endl;
        for (float i=1; i<=schrittweitanz;++i) {
                näschritt+=schrittwe+((schrittweitanz-i+1)*kdtanz);
                cout<<i<<std::setw(15)<<näschritt<<std::setw(15)<<schrittwe+(schrittweitanz-i+1)*kdtanz<<std::setw(15)<<(schrittweitanz-i+1)*kdtanz<<endl;
                näschritts.push_back({i,näschritt});
        }
    }
    else {
    float kd = Indexnumlaenge-schrittwa*(float)(schrittweitanz);
    int sumanz = schrittweitanz*(schrittweitanz+1)/2;
    float kdtanz = kd/sumanz;
    cout<<"kd: "<<std::setw(15)<<"sumanz: "<<std::setw(15)<<"kdtanz: "<<endl;
    cout<<kd<<std::setw(15)<<sumanz<<std::setw(15)<<kdtanz<<endl;
    cout<<endl;
    cout<<"i: "<<std::setw(15)<<" näschritt: "<<std::setw(15)<<" schrittwa+(i*kdtanz): "<<std::setw(15)<<" schrittwa+(i*kdtanz): "<<endl;
        for (float i=1; i<=schrittweitanz;++i) {
                näschritt+=schrittwa+(i*kdtanz);
                cout<<i<<std::setw(15)<<näschritt<<std::setw(15)<<schrittwa+(i*kdtanz)<<std::setw(15)<<schrittwa+(i*kdtanz)<<endl;
                näschritts.push_back({i,näschritt});
        }
    }
    cout<<endl;
    if (kl) näschritts.pop_back();    

}


    float currentsteptimescl=0;
    for (float i=0;i<näschritts.size();++i){
        if (totalDi==1) {
        currentsteptimescl=näschritts[i].xn+startTime;
        }
        else currentsteptimescl=totalDuration*näschritts[i].xn+startTime;

        if (firstrec) {
            float p2 = i/(näschritts.size()+1);
            cout << "p2: " << p2 << endl;
            notesr.push_back({currentsteptimescl, static_cast<int>(notes.size()), 1, 0, p2});
        } else newNotes.push_back(Note3(currentsteptimescl, 20, 60+i));
    }
if (firstrec) {
    cout << "Endgültige notesr:" << endl;
    for (const auto& note : notesr) {
       cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << ", " << note.p2 << endl;
    }
}
else {
    cout << "Endgültige newNotes:" << endl;
    for (const auto& note : newNotes) {
       cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }
}
    if (firstrec) {

    } else {
        // if (startIndex==0) notes4.push_back({0,20,60});
        notes4.insert(notes4.begin() + startIndex, newNotes.begin(), newNotes.end());
        // Ausgabe des endgültigen notes3 Vectors im Terminal
        cout << "Endgültige Noten4:" << endl;
        for (const auto& note : notes4) {
        cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
        }
    }
kk=false;
kl=false;
kr=false;
}

void PatternRecorder::scaleToRange(vector<Note3>& newNotes, float startTime, float endTime) {
    if (newNotes.empty()) return;

    float totalDuration = endTime-startTime; // Gesamtdauer des Zeitbereichs
    float newDuration = newNotes.back().timestamp - newNotes.front().timestamp; // Gesamtdauer der neuen Noten

    float scalingFactor = totalDuration / newDuration; // Berechnung des Skalierungsfaktors
    cout<<startTime<<" "<<endTime<<" "<<newDuration*totalDuration<<" "<<totalDuration/newDuration<<endl;

    // Mappen der neuen Noten auf den Zeitbereich und Erstellen von Zwischennoten
    for (size_t i = 0; i < newNotes.size(); ++i) {
        float scaledTimestamp;
        scaledTimestamp = startTime + (newNotes[i].timestamp - newNotes.front().timestamp) * scalingFactor;
        int instrument = newNotes[i].instrument;
        int pitch = newNotes[i].pitch;

        // Füge die skalierten Noten hinzu
        scaledNotes.push_back(Note3(scaledTimestamp, instrument, pitch));
    }
    cout << "scaledNotes1:" << endl;
    for (const auto& note : scaledNotes) {
    cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }

    //Pausen berechnen
    for (int i = 0; i < scaledNotes.size(); ++i) {
        if (scaledNotes[i].instrument==1000) {
            for (int j = i; j < scaledNotes.size(); ++j) {
                scaledNotes[j+1].timestamp=static_cast<float>(scaledNotes[j+1].timestamp)+static_cast<float>(scaledNotes[i].timestamp);
            }
        }
    }
    for (int i = 0; i < scaledNotes.size(); ++i) {
        if (scaledNotes[i].instrument==1000) {
            scaledNotes.erase(scaledNotes.begin() + i);
        }
    }
    cout << "scaledNotes1:" << endl;
    for (const auto& note : scaledNotes) {
    cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }

    for (size_t i = 0; i < scaledNotes.size(); ++i) {
        // Erstellen von Zwischennoten basierend auf dem aktuellen Pitch
        if (i < scaledNotes.size() - 1) {
            float duration = scaledNotes[i + 1].timestamp - scaledNotes[i].timestamp; // Dauer des aktuellen Parts
            int noteValue = scaledNotes[i].pitch; // Aktuelle Note

            for (int j = 0; j < noteValue; ++j) {
                float partTimestamp = scaledNotes[i].timestamp + (duration / noteValue) * j;
                int instrument = scaledNotes[i].instrument + i;
                int intermediatePitch = 60 + j; // C4 ist Pitch 60, dann D4 ist Pitch 61 usw.
                scaledNotes2.push_back(Note3(partTimestamp, instrument, intermediatePitch));
            }
        }
    }
    cout << "scaledNotes2:" << endl;
    for (const auto& note : scaledNotes2) {
    cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }
    // //normalisieren
    // float sclfkt = static_cast<float>(scaledNotes2[scaledNotes2.size()-1].timestamp);
    // cout<<"sclfkt: "<<sclfkt<<endl;
    // for (size_t i = 0; i <= scaledNotes2.size(); ++i) {
    //     scaledNotes2[i].timestamp=static_cast<float>(scaledNotes[i].timestamp)*sclfkt+startTime;
    // }
    // cout << "scaledNotes2 in notes4:" << endl;
    // for (const auto& note : scaledNotes2) {
    // cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    // }

    // Ersetzen der neuen Noten durch die skalierten Noten
    // newNotes = scaledNotes;
}

void PatternRecorder::scale() {
    if (notes.empty()) return;
    notes2.clear();
    notes3.clear();
    notes4.clear();
    float totalDuration = notes.back().timestamp - notes.front().timestamp; // Gesamtdauer von der ersten bis zur letzten Note
    // float scaledDuration = (60.0f / bpm) * dauer; // Dauer von 8 Beats
    float scaledDuration = 1.0f; // Normalisieren
    float scalingFactor = scaledDuration / totalDuration;

    // Mappen auf Note2
    for (const auto& note : notes) {
        notes2.push_back({(note.timestamp - notes.front().timestamp) * scalingFactor, note.instrument, note.pitch, note.scl, note.p2});
    }

    // Ausgabe des mapped notes2 Vectors im Terminal
    cout << "Mapped Noten2:" << endl;
    for (const auto& note : notes2) {
        cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }

    //Pausen berechnen
    for (int i = 0; i < notes2.size(); ++i) {
        if (notes2[i].instrument==1000) {
            for (int j = i; j < notes2.size(); ++j) {
                notes2[j+1].timestamp=static_cast<float>(notes2[j+1].timestamp)+static_cast<float>(notes2[i].timestamp);
            }
        }
    }
    for (int i = 0; i < notes2.size(); ++i) {
        if (notes2[i].instrument==1000) {
            notes2.erase(notes2.begin() + i);
        }
    }

    //normalisieren
    float scldivisor = static_cast<float>(notes2[notes2.size()-1].timestamp);
    cout<<"scldivisor: "<<scldivisor<<endl;
    for (size_t i = 0; i <= notes2.size(); ++i) {
        notes2[i].timestamp=static_cast<float>(notes2[i].timestamp)/scldivisor;
    }

    // Ausgabe des mapped notes2 mit Pausen Vectors im Terminal
    cout << "Mapped normalized Noten2 with Breaks:" << endl;
    for (const auto& note : notes2) {
        cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
    }


    // Erstellen von Note3 basierend auf den Teilen zwischen den Tasten in notes2
    for (size_t i = 0; i < notes2.size(); ++i) {
        float duration = notes2[i + 1].timestamp - notes2[i].timestamp; // Dauer des aktuellen Parts
        int noteValue = notes2[i].pitch; // Aktuelle Note
        for (int j = 0; j < noteValue; ++j) { 
            float partTimestamp = notes2[i].timestamp + (duration / noteValue) * j;
            int instrument = notes2[i].instrument;
            int pitch = 60 + j; // C4 ist Pitch 60, dann D4 ist Pitch 61 usw.
            notes3.push_back(Note3(partTimestamp, instrument, pitch, 0, notes2[i].p2));
        }
    }
    notes3.push_back(Note3(1, 0, 0, 0, 0)); // Dummy-Note für Endzeit

   // Ausgabe des endgültigen notes3 Vectors im Terminal
   cout << "Endgültige Noten3:" << endl;
   for (const auto& note : notes3) {
       cout << note.timestamp << ", " << note.instrument << ", " << note.pitch << endl;
   }

    notes4=notes3;
    // notes4.erase(notes4.begin() + notes4.size());
}
void PatternRecorder::transformTimestamps() {
    if (notes4.empty()) return;

    cout << "Transforming timestamps...1||2 x Integrieren?" << endl;
    std:string val = "";
    val=Eingabe();
    int vali = stoi(val);
    // Erstelle eine quadratische Funktion, die die timestamps als Nullstellen hat
    // f(x) = (x - t1) * (x - t2) * ... * (x - tn)
    // Diese Funktion ist die 2. Ableitung einer kubischen Funktion
    // Integriere sie zweimal, um die ursprüngliche Funktion zu erhalten

    // Erstelle die Nullstellen-Array
    std::vector<float> timestamps;
    for (const auto& note : notes4) {
        timestamps.push_back(note.timestamp);
    }

    // Sortiere die timestamps
    std::sort(timestamps.begin(), timestamps.end());
    float integral = 0.0f;
    // Erstelle die neue Funktion
    for (auto& note : notes4) {
        float x = note.timestamp;
        float value = 1.0f;
        
        // Berechne das Produkt der Nullstellen
        for (float t : timestamps) {
            value *= (x - t);
        }

        if (vali == 1 || vali == 2) {          
        
            // Integriere zweimal
            // 1. Integration: F(x) = ∫f(x)dx
            float integral1 = 0.0f;
            for (float t : timestamps) {
                integral1 += (x - t) * (x - t) / 2.0f;
            }
            integral += integral1;
        }
        else if (vali == 2) {
            // 2. Integration: G(x) = ∫F(x)dx
            float integral2 = 0.0f;
            for (float t : timestamps) {
                integral2 += (x - t) * (x - t) * (x - t) / 6.0f;
            }
            integral += integral2;
        }    
        // Normalisiere den Wert
        note.timestamp = integral / (timestamps.size() * timestamps.size());
    }

    // Skaliere die timestamps auf das ursprüngliche Intervall
    float minTime = notes4.front().timestamp;
    float maxTime = notes4.back().timestamp;
    float range = maxTime - minTime;

    for (auto& note : notes4) {
        cout<<note.timestamp<<endl;
        // note.timestamp = minTime + (note.timestamp - minTime) * range / (maxTime - minTime);
    }
    for (auto& note : notes4) {
        cout<<"Normalisiert"<<endl;
        note.timestamp = (note.timestamp - minTime) / (maxTime - minTime);
        cout<<note.timestamp<<endl;
    }
}
void PatternRecorder::reverseTransformTimestamps() {
    if (notes4.empty()) return;

    cout << "Reversing timestamp transformation...1||2 Ableitungen?" << endl;
    std::string val = "";
    val = Eingabe();
    int vali = stoi(val);

    // Speichere die ursprünglichen timestamps für die Ableitung
    std::vector<float> timestamps;
    for (const auto& note : notes4) {
        timestamps.push_back(note.timestamp);
    }

    std::sort(timestamps.begin(), timestamps.end());

    // Erstelle die Ableitungsfunktion
    for (auto& note : notes4) {
        float x = note.timestamp;
        float derivative = 0.0f;

        if (vali == 1) {
            // 1. Ableitung: f'(x) = d/dx (∫f(x)dx)
            for (float t : timestamps) {
                if (t < x) {
                    derivative += sqrt((x - t) * 2.0f);
                }
            }
        }
        else if (vali == 2) {
            // 2. Ableitung: f'(x) = d/dx (∫∫f(x)dxdx)
            for (float t : timestamps) {
                if (t < x) {
                    derivative += pow((x - t) * 3.0f, 1.0f/3.0f);
                }
            }
        }

        // Normalisiere den Wert
        note.timestamp = derivative / timestamps.size();
    }

    // Skaliere die timestamps zurück auf das ursprüngliche Intervall
    float minTime = notes4.front().timestamp;
    float maxTime = notes4.back().timestamp;
    float range = maxTime - minTime;

    for (auto& note : notes4) {
        cout << note.timestamp << endl;
    }

    for (auto& note : notes4) {
        cout << "Normalisiert" << endl;
        note.timestamp = (note.timestamp - minTime) / range;
        cout << note.timestamp << endl;
    }

    cout << "Timestamp transformation reversed with " << vali << " derivatives." << endl;
}
// void PatternRecorder::reverseTransformTimestamps() {
//     if (notes4.empty()) return;

//     cout << "Reversing timestamp transformation...1||2 Ableitungen?" << endl;
//     std::string val = "";
//     val = Eingabe();
//     int vali = stoi(val);

//     // Speichere die ursprünglichen timestamps für die Ableitung
//     std::vector<float> timestamps;
//     for (const auto& note : notes4) {
//         timestamps.push_back(note.timestamp);
//     }

//     std::sort(timestamps.begin(), timestamps.end());

//     // Erstelle die Ableitungsfunktion
//     for (auto& note : notes4) {
//         float x = note.timestamp;
//         float derivative = 0.0f;

//         if (vali == 1) {
//             // 1. Ableitung: f'(x) = d/dx (∫f(x)dx)
//             for (float t : timestamps) {
//                 derivative = sqrt((x - t)*2.0f);
//             }
//         }
//         else if (vali == 2) {
//             // 2. Ableitung: f'(x) = d/dx (∫∫f(x)dxdx)
//             for (float t : timestamps) {
//                 derivative = pow((x - t)*3.0f, (1.0/3.0f));
//             }
            
//         }

//         // Normalisiere den Wert
//         note.timestamp = derivative / (timestamps.size() * timestamps.size());
//     }

//     // Skaliere die timestamps zurück auf das ursprüngliche Intervall
//     float minTime = notes4.front().timestamp;
//     float maxTime = notes4.back().timestamp;
//     float range = maxTime - minTime;

//     for (auto& note : notes4) {
//         cout<<note.timestamp<<endl;

//         // note.timestamp = minTime + (note.timestamp - minTime) * range / (maxTime - minTime);
//     }
//     for (auto& note : notes4) {
//         cout<<"Normalisiert"<<endl;
//         note.timestamp = (note.timestamp - minTime) / (maxTime - minTime);
//         cout<<note.timestamp<<endl;
//     }

//     cout << "Timestamp transformation reversed with " << vali << " derivatives." << endl;
// }

void PatternRecorder::quantizeNotes(vector<Note3>& notes, int quantization) {
    if (notes.empty()) return;

    float quantizationStep = (60.0f / sharedData->bpm) / quantization; // Dauer eines Quantisierungsschritts

    for (auto& note : notes) {
        // Quantisiere den Zeitstempel
        int steps = round(note.timestamp / quantizationStep);
        note.timestamp = steps * quantizationStep;
    }

    // Sortiere die Noten nach ihren neuen Zeitstempeln
    sort(notes.begin(), notes.end(), [](const Note3& a, const Note3& b) {
        return a.timestamp < b.timestamp;
    });
}
void PatternRecorder::bpmtap() {
    float tapTimebefore, tapTime;
    float bpmtap = 0;
    float bpmtaps = 0;
    float bpmtapq = 0;
    float count=1;
    bool firstpress=true;
    while (true) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_0) {
                    break;
                } else if (event.key.keysym.sym == SDLK_9) {
                    if (firstpress==true) {
                        tapTimebefore = SDL_GetTicks() / 1000.0f; // Aktuelle Zeit in Sekunden
                        firstpress=false;
                    }
                    else if (firstpress==false){
                        float currenttapTime = SDL_GetTicks() / 1000.0f; // Aktuelle Zeit in Sekunden
                        tapTime=currenttapTime-tapTimebefore;
                        bpmtap = 60.0f / tapTime;
                        bpmtaps += bpmtap;
                        bpmtapq = bpmtaps / count;
                        cout<<"tapTime: "<<tapTime<<" bpmtap: "<<bpmtap<<" bpmtaps: "<<bpmtaps<<" bpmtapq: "<<bpmtapq<<" count: "<<count<<endl;
                        tapTimebefore=currenttapTime;
                        count++;
                    }
                }
            }
        }
    }
    sharedData->bpm=bpmtapq;
    sendBPMToRenoise(); // Sende die BPM an Renoise

}
void PatternRecorder::play() {
    if (notes3.empty()) { // Überprüfen, ob notes3 leer ist
        cout << "No notes recorded." << endl;
        return;
    }

    cout << "Playing pattern at " << sharedData->bpm << " BPM..." << endl;

    SDL_Delay(plsl); // Kurze Pause, um CPU zu entlasten

    float play3StartTime1 = SDL_GetTicks() / 1000.0f; // Aktuelle Zeit in Sekunden
    int v1 =0;
    int l1 =0;
    float playTime1;
    while (l1<loops) {
        playTime1 = static_cast<float>(notes3[v1].timestamp)*sharedData->zeitfakto; // Zeitstempel relativ zur Startzeit des Abspielens
        float currentTime=SDL_GetTicks()/ 1000.0f;
        if (l1<loops) {
            if (v1<notes3.size()) {
                if ((currentTime - play3StartTime1) >= playTime1) {
                    if (v1==notes3.size()-1) {
                        play3StartTime1 += notes3[v1].timestamp*sharedData->zeitfakto;
                        l1++;
                        v1=0;
                        if (l1==loops) break;
                    }
                    else {
                        sendNoteToRenoise(notes3[v1].instrument+sharedData->variable2, notes3[v1].pitch+sharedData->variable3); // Sende Note an Renoise
                        v1++;
                        cout<<currentTime-play3StartTime1<<std::setw(20)<<playTime1<<std::setw(10)<<v1<<std::setw(10)<<l1<<endl;
                    }
                }
            }
        }
    }
}
void PatternRecorder::play2() {
    if (notes4.empty()) { // Überprüfen, ob notes3 leer ist
        cout << "No notes recorded." << endl;
        return;
    }

    cout << "Playing pattern at " << sharedData->bpm << " BPM..." << endl;

    SDL_Delay(plsl); // Kurze Pause, um CPU zu entlasten

    float play3StartTime1 = SDL_GetTicks() / 1000.0f; // Aktuelle Zeit in Sekunden
    int v1 =0;
    int l1 =0;
    float playTime1;

    while (l1<loops) {
        playTime1 = static_cast<float>(notes4[v1].timestamp)*sharedData->zeitfakto; // Zeitstempel relativ zur Startzeit des Abspielens
        float currentTime=SDL_GetTicks()/ 1000.0f;
        // cout<<currentTime-play3StartTime1<<std::setw(20)<<playTime1<<std::setw(10)<<v1<<std::setw(10)<<l1<<endl;
        if (l1<loops) {
            if (v1<notes4.size()) {
                if ((currentTime - play3StartTime1) >= playTime1) {
                    if (v1==notes4.size()-1) {
                        play3StartTime1 += notes4[v1].timestamp*sharedData->zeitfakto;
                        l1++;
                        v1=0;
                        if (l1==loops) break;
                    }
                    else {
                        // sendNoteToRenoise(notes4[v1].instrument+sharedData->variable2, notes4[v1].pitch+sharedData->variable3); // Sende Note an Renoise
                        sendNoteToRenoise(notes4[v1].instrument+sharedData->variable2, notes4[v1].pitch+sharedData->variable3); // Sende Note an Renoise
                        cout<<l1<<" "<<v1<<" "<<notes4[v1].timestamp<<" "<<notes4[v1].instrument<<" "<<notes4[v1].pitch<<endl;
                        v1++;
                        
                    }
                }
            }
        }
    }
}

void PatternRecorder::play4(int num, SDL_Event& event) {
    std::lock_guard<std::mutex> lock(playingMutex);
    if (num < 0 || num >= numPatterns || pnotes[num].empty()) {
        std::cout << "Invalid pattern number or empty pattern: " << num << std::endl;
        return;
    }
    
    if (isPlaying[num].load()) {
        isPlaying[num].store(false);
    } else {
        isPlaying[num].store(true);
        playingThreads[num] = std::async(std::launch::async, &PatternRecorder::playPattern, this, num);
    }
}
void PatternRecorder::playPattern(int num) {
    if (pnotes[num].empty()) {
        std::cout << "No notes recorded for pattern " << num << "." << std::endl;
        return;
    }

    std::cout << "Playing pattern " << num << " at " << sharedData->bpm << " BPM..." << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(plsl));

    auto play3StartTime1 = std::chrono::steady_clock::now();
    int v1 = 0;
    int l1 = 0;
    std::chrono::duration<float> playTime1;

    while (l1 < loops && isPlaying[num].load()) {
        playTime1 = std::chrono::duration<float>(pnotes[num][v1].timestamp * sharedData->zeitfakto / sharedData->dauer * pdauer[num][0]);
        
        if (std::chrono::steady_clock::now() - play3StartTime1 >= playTime1) {
            if (v1 == pnotes[num].size() - 1) {
                play3StartTime1 += std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<float>(pnotes[num][v1].timestamp * sharedData->zeitfakto / sharedData->dauer * pdauer[num][0]));
                // play3StartTime1 += std::chrono::duration<float>(pnotes[num][v1].timestamp * sharedData->zeitfakto / sharedData->dauer * pdauer[num][0]);
                l1++;
                v1 = 0;
                if (l1 == loops) break;
            } else {
                {
                    std::lock_guard<std::mutex> lock(dataMutex);
                    sendNoteToRenoise(pnotes[num][v1].instrument + sharedData->variable2, pnotes[num][v1].pitch + sharedData->variable3);
                }
                
                std::cout << "Pattern " << num << ": " << l1 << " " << v1 << " " << pnotes[num][v1].timestamp << " " 
                            << pnotes[num][v1].instrument << " " << pnotes[num][v1].pitch << std::endl;
                v1++;
            }
        }
        std::this_thread::yield(); // Gibt anderen Threads die Möglichkeit zu laufen
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    isPlaying[num].store(false);
}

void PatternRecorder::playforforhi(int num) {

    if (pnotes[num].empty()) { // Überprüfen, ob notes3 leer ist
        cout << "No notes recorded." << endl;
        return;
    }


    SDL_Delay(plsl); // Kurze Pause, um CPU zu entlaste1n

    float note_time;
    float currentTime;
    int l1 =0;

    // playfofohi1[num].current_indices = 0; // Reset Index
    // playfofohi1[num].start_times = SDL_GetTicks() / 1000.0f;
    // std::cout<<"Taste: "<<index<<" "<<playfofohi1[indexx].start_times<<std::endl;

    while (l1<loops) {
        SDL_Event event;
        int indexx = 0;
        float timeis = 0.0f;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN){
                int key = event.key.keysym.sym;
                if (key>=SDLK_1 && key<=SDLK_8) {
                    indexx = key - SDLK_0;
                }
            }
            playfofohi1[indexx].current_indices = 0; // Reset Index
            timeis  = SDL_GetTicks() / 1000.0f;
            playfofohi1[indexx].start_times = timeis;
            std::cout<<"Taste: "<<indexx<<" "<<playfofohi1[indexx].start_times<<std::endl;
        }        



        for (int index = 0; index <8; ++index) {
            // currentTime=SDL_GetTicks()/ 1000.0f;
            if (pnotes[index].empty()) {
            } else {
                note_time = pnotes[index][playfofohi1[index].current_indices].timestamp*pdauer[index][0]; // *pdauer[index][0];
                // cout<<"Index: "<<index<<"note_time: "<<note_time<<" "<<playfofohi1[index].start_times<<" "<<currentTime - playfofohi1[index].start_times<<" "<<currentTime<<endl;
                if (((SDL_GetTicks() / 1000.0f) - playfofohi1[index].start_times)>=note_time) {
                    // cout<<"--"<<endl;
                    if (playfofohi1[index].current_indices==pnotes[index].size()-1) {
                        playfofohi1[index].current_indices=0;
                        l1++;
                    // cout<<"---"<<endl;
                        // if (l1==loops) break;
                    }
                    else {
                        sendNoteToRenoise(pnotes[index][playfofohi1[index].current_indices].instrument+sharedData->variable2, 60); // Sende Note an Renoise
                        cout<<playfofohi1[index].current_indices<<" "<<pnotes[index][playfofohi1[index].current_indices].timestamp<<endl;
                        playfofohi1[index].current_indices++;

                    }

                }
            }
        }
    }
}
void PatternRecorder::play3() {
    if (notes4.empty()) { // Überprüfen, ob notes3 leer ist
        cout << "No notes recorded." << endl;
        return;
    }

    cout << "Playing pattern at " << sharedData->bpm << " BPM..." << endl;

    SDL_Delay(plsl); // Kurze Pause, um CPU zu entlasten

    float play3StartTime1 = SDL_GetTicks() / 1000.0f; // Aktuelle Zeit in Sekunden
    float play3StartTime2 = play3StartTime1; // Aktuelle Zeit in Sekunden
    int v1 =0;
    int v2 =0;
    int l1 =0;
    int l2 =0;
    float playTime1, playTime2;
    while ((l1<loops) && (l2<loops)) {
        playTime1 = static_cast<float>(notes4[v1].timestamp)*sharedData->zeitfakto; // Zeitstempel relativ zur Startzeit des Abspielens
        playTime2 = static_cast<float>(mod1[v2].time)*sharedData->zeitfakto; // Zeitstempel relativ zur Startzeit des Abspielens
        float currentTime=SDL_GetTicks()/ 1000.0f;
        // cout<<currentTime-play3StartTime1<<std::setw(20)<<currentTime-play3StartTime2<<std::setw(20)<<playTime1<<std::setw(10)<<playTime2<<std::setw(10)<<v1<<std::setw(10)<<l1<<std::setw(10)<<v2<<std::setw(10)<<l2<<endl;
        if (l1<loops) {
            if (v1<notes4.size()) {
                if ((currentTime - play3StartTime1) >= playTime1) {
                    if (v1==notes4.size()-1) {
                        play3StartTime1 += notes4[v1].timestamp*sharedData->zeitfakto;
                        l1++;
                        v1=0;
                        if (l1==loops) break;
                    }
                    else {
                        sendNoteToRenoise(notes4[v1].instrument+sharedData->variable2, notes4[v1].pitch+sharedData->variable3); // Sende Note an Renoise
                        v1++;
                        cout<<l1<<" "<<v1<<" "<<notes4[v1].timestamp<<" "<<notes4[v1].instrument<<" "<<notes4[v1].pitch<<endl;

                    }
                }
            }
        }
        if (l2<loops) {
            if (v2<mod1.size()) {
                if ((currentTime - play3StartTime2) >= playTime2) {
                    if (v2==mod1.size()-1) {
                        play3StartTime2 += mod1[v2].time*sharedData->zeitfakto;
                        l2++;
                        v2=0;
                        if (l1==loops) break;
                    }
                    else {
                        sendNoteToRenoise(mod1[v2].instrument+sharedData->variable2, mod1[v2].pitch+sharedData->variable3); // Sende Note an Renoise
                        v2++;
                        cout<<l2<<" "<<v2<<" "<<mod1[v2].time<<" "<<mod1[v2].instrument<<" "<<mod1[v2].pitch<<endl;
                    }
                }
            }
        }
    }
}
void PatternRecorder::play34(int num) {
    if (pnotes[num].empty()) { // Überprüfen, ob notes3 leer ist
        cout << "No notes recorded." << endl;
        return;
    }

    cout << "Playing pattern at " << sharedData->bpm << " BPM..." << endl;

    SDL_Delay(plsl); // Kurze Pause, um CPU zu entlasten

    float play3StartTime1 = SDL_GetTicks() / 1000.0f; // Aktuelle Zeit in Sekunden
    float play3StartTime2 = play3StartTime1; // Aktuelle Zeit in Sekunden
    int v1 =0;
    int v2 =0;
    int l1 =0;
    int l2 =0;
    float playTime1, playTime2;
    while ((l1<loops) && (l2<loops)) {
        playTime1 = static_cast<float>(pnotes[num][v1].timestamp)*sharedData->zeitfakto/sharedData->dauer*pdauer[num][0]; // Zeitstempel relativ zur Startzeit des Abspielens
        playTime2 = static_cast<float>(mod1[v2].time)*sharedData->zeitfakto/sharedData->dauer*pdauer[num][0]; // Zeitstempel relativ zur Startzeit des Abspielens
        float currentTime=SDL_GetTicks()/ 1000.0f;
        // cout<<currentTime-play3StartTime1<<std::setw(20)<<currentTime-play3StartTime2<<std::setw(20)<<playTime1<<std::setw(10)<<playTime2<<std::setw(10)<<v1<<std::setw(10)<<l1<<std::setw(10)<<v2<<std::setw(10)<<l2<<endl;
        if (l1<loops) {
            if (v1<pnotes[num].size()) {
                if ((currentTime - play3StartTime1) >= playTime1) {
                    if (v1==pnotes[num].size()-1) {
                        play3StartTime1 += pnotes[num][v1].timestamp*sharedData->zeitfakto/sharedData->dauer*pdauer[num][0];
                        l1++;
                        v1=0;
                        if (l1==loops-1) break;
                    }
                    else {
                        sendNoteToRenoise(pnotes[num][v1].instrument+sharedData->variable2, pnotes[num][v1].pitch+sharedData->variable3); // Sende Note an Renoise
                        v1++;
                        cout<<l1<<" "<<v1<<" "<<pnotes[num][v1].timestamp<<" "<<pnotes[num][v1].instrument<<" "<<pnotes[num][v1].pitch<<endl;

                    }
                }
            }
        }
        if (l2<loops) {
            if (v2<mod1.size()) {
                if ((currentTime - play3StartTime2) >= playTime2) {
                    if (v2==mod1.size()-1) {
                        play3StartTime2 += mod1[v2].time*sharedData->zeitfakto/sharedData->dauer*pdauer[num][0];
                        l2++;
                        v2=0;
                        if (l1==loops-1) break;
                    }
                    else {
                        sendNoteToRenoise(mod1[v2].instrument+sharedData->variable2, mod1[v2].pitch+sharedData->variable3); // Sende Note an Renoise
                        v2++;
                        cout<<l2<<" "<<v2<<" "<<mod1[v2].time<<" "<<mod1[v2].instrument<<" "<<mod1[v2].pitch<<endl;
                    }
                }
            }
        }
    }
}
void PatternRecorder::recntr() {
    cout << "Geben Sie den Anfangsindex ein: "<<endl;
    startIndex = stoi(Eingabe());
    cout << "Geben Sie den Endindex ein: "<<endl;
    endIndex = stoi(Eingabe());
    for (int i = startIndex; i < endIndex; ++i) {
        cout << "Geben Sie den instrnum ein: "<<endl;
        notes4[i].instrument = stoi(Eingabe());
    }
    DisPlNotes();
}
void PatternRecorder::recntrf() {
    cout << "Geben Sie den Anfangsindex ein: "<<endl;
    startIndex = stoi(Eingabe());
    cout << "Geben Sie den Endindex ein: "<<endl;
    endIndex = stoi(Eingabe());
    for (int i = startIndex; i < endIndex; ++i) {
        cout << "Geben Sie den instrnum ein: "<<endl;
        notes4[i].pitch = stoi(Eingabe())+60;
    }
    DisPlNotes();
}
void PatternRecorder::vecforrecf() {
    cout << "Geben Sie die recsteps ein: "<<endl;
    int recsteps = stoi(Eingabe());
    for (int i = 0; i < recsteps; ++i) {
        vecforrec event;
        cout << "Geben Sie den Anfangsindexnum ein: "<<endl;
        event.AnIxNm = stof(Eingabe());
        cout << "Geben Sie den Endindexnum ein: "<<endl;
        event.EnIxNm = stof(Eingabe());
        cout << "Geben Sie die schrittweitanz ein: "<<endl;
        event.steps = stoi(Eingabe());
        cout << "Geben Sie den kis ein: "<<endl;
        event.kis = stoi(Eingabe());
        event.startTime = notes4[i].timestamp;
        event.endTime = notes4[i+1].timestamp;
        event.recsteps = recsteps;
        event.recstep = i;
        veforec.push_back(event);
    }
    cout << "veforec:" << endl;
    for (const auto& note : veforec) {
        cout << note.recsteps << ", " << note.recstep << ", " << note.AnIxNm << ", " << note.EnIxNm << ", " << note.steps << ", " << note.kis << endl;
    }

    // notes3.clear();
    // for (int i=0;i<recsteps;++i) {
    //     notes3.push_back({i/recsteps, veforec[i].recstep, 60});
    // }
    // notes4=notes3;
    // DisPlNotes();
    for (int i=0;i<recsteps;++i) {
       switch (veforec[i].kis) {
        case 1: !kk; break;
        case 2: !kl; break;
        case 3: !kr; break;
        case 4: break;
        default: break;
       }
       for (int ii=0;ii<notes4.size();++ii) {
        if (notes4[ii].timestamp==veforec[i].startTime) {
            startIndex = ii;
            endIndex = ii+1;
        }
       }
       startIndexnum = veforec[i].AnIxNm;
       endIndexnum = veforec[i].EnIxNm;
       schrittweitanz = veforec[i].steps;
       startTime = notes4[i].timestamp;
       endTime = notes4[i+1].timestamp;
       cout<<"startIndex: "<<startIndex<<" endIndex: "<<endIndex<<" startIndexnum: "<<startIndexnum<<" endIndexnum: "<<endIndexnum<<" schrittweitanz: "<<schrittweitanz<<" startTime: "<<startTime<<" endTime: "<<endTime<<endl;
       remove();
       scaleToRangenum(newNotes, startTime, endTime, startIndexnum, endIndexnum, schrittweitanz, firstrec);
       kk, kl, kr = false;
    }
}
void PatternRecorder::setBPM() {
    // this->bpm = bpm;
    // this->beatDuration = 60.0f / sharedData->bpm;
    sendBPMToRenoise(); // Sende die BPM an Renoise
}

void PatternRecorder::sendNoteToRenoise(int instrument, int pitch) {
    // if (vmidi==1) {
    //     sendMidiControlFromNotes(notes4, 1, 0);
    // }
    string port = to_string(7005);
    lo_address t = lo_address_new("localhost", port.c_str()); // Port als C-String

    // OSC-Befehl zum Triggern der Note
    lo_send(t, "/renoise/trigger/note_on", "iiii", -1, -1, pitch, 127); // -1 für MIDI-Channel, Instrument, Note, Velocity
}

void PatternRecorder::sendBPMToRenoise() {
    string port = to_string(7005);
    lo_address t = lo_address_new("localhost", port.c_str()); // Port als C-String

    // OSC-Befehl zum Senden der BPM
    lo_send(t, "/renoise/song/bpm", "f", sharedData->bpm); // Sende die BPM als float
}

void PatternRecorder::saveToCSV(const string& filename, const std::array<std::vector<Note3>, 8> pnotes,int num) {
    const std::vector<Note3> ppattern = pnotes[num];
    ofstream file(filename);
    for (const auto& note : ppattern) {
            file << note.timestamp << "," << note.instrument << "," << note.pitch << "," << note.glide << endl;
        }
    
    file.close();
}

void PatternRecorder::readFromCSV(const string& filename, int num) {
    if (num == 9) {
        for (int i = 0; i < 9; ++i) {
            pnotes[i].clear();
        }
    } else pnotes[num].clear();
    
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open file: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream iss(line);
        string token;
        vector<string> tokens;
        
        while (getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() >= 4) {
            Note3 note;
            note.timestamp = stof(tokens[0]);
            note.instrument = stoi(tokens[1]);
            note.pitch = stoi(tokens[2]);
            note.glide = (tokens[3] == "1" || tokens[3] == "true");
            pnotes[num].push_back(note);
        }
    }
    file.close();
    cout << "Loaded " << pnotes[num].size() << " notes into pattern " << num << endl;
}


void PatternRecorder::display(SDL_Renderer* renderer) {
    // Textdarstellung
    TTF_Font* font = TTF_OpenFont("/home/apricot/Dokumente/sagan.ttf", 10); // Kleinere Schriftgröße
    if (font == nullptr) {
        cerr << "Failed to load font: " << TTF_GetError() << endl;
        return;
    }

    // Text für BPM
    stringstream bpmText;
    bpmText << "BPM: " << sharedData->bpm;
    SDL_Color textColor = {255, 255, 255}; // Weiß
    SDL_Surface* surface = TTF_RenderText_Solid(font, bpmText.str().c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstrect = {10, 10, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Text für aufgezeichnete Zahlen
    stringstream notesText;
    notesText << "Recorded Notes: ";
    for (const auto& note : notes) {
        notesText << note.pitch << " ";
    }

    surface = TTF_RenderText_Solid(font, notesText.str().c_str(), textColor);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    dstrect = {10, 50, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Nur wenn notes4 nicht leer ist, zeichne die Grafen
    if (!notes4.empty()) {
        // Zeichne die Achsen
        SDL_SetRenderDrawColor(renderer, 63, 63, 63, 255);
        SDL_RenderDrawLine(renderer, 0, surface->h-5, surface->w, surface->h-5);  // x-Achse
        SDL_RenderDrawLine(renderer, 0, 95, 0, surface->h-5);  // y-Achse
        // Zeichne die Funktion mit Nullstellen
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Rot
        for (size_t i = 0; i < notes4.size() - 1; ++i) {
            int x1 = static_cast<int>(notes4[i].timestamp * surface->w);
            int x2 = static_cast<int>(notes4[i + 1].timestamp * surface->w);
            int y1 = surface->h - 5;
            int y2 = surface->h - 5;
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }

        // Zeichne die erste Integration
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Grün
        for (size_t i = 0; i < notes4.size() - 1; ++i) {
            int x1 = static_cast<int>(notes4[i].timestamp * surface->w);
            int x2 = static_cast<int>(notes4[i + 1].timestamp * surface->w);
            int y1 = surface->h - 5;
            int y2 = surface->h - 5;
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }

        // Zeichne die zweite Integration
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Blau
        for (size_t i = 0; i < notes4.size() - 1; ++i) {
            int x1 = static_cast<int>(notes4[i].timestamp * surface->w);
            int x2 = static_cast<int>(notes4[i + 1].timestamp * surface->w);
            int y1 = surface->h - 5;
            int y2 = surface->h - 5;
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
    }

    // // Aktuell gespielte Note in einer anderen Farbe anzeigen
    // if (currentPlayingNote != -1) {
    //     stringstream currentNoteText;
    //     currentNoteText << "Playing Note: " << currentPlayingNote;
    //     SDL_Color currentNoteColor = {255, 0, 0}; // Rot
    //     surface = TTF_RenderText_Solid(font, currentNoteText.str().c_str(), currentNoteColor);
    //     texture = SDL_CreateTextureFromSurface(renderer, surface);
    //     dstrect = {10, 90, surface->w, surface->h};
    //     SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    //     SDL_FreeSurface(surface);
    //     SDL_DestroyTexture(texture);
    // }

    TTF_CloseFont(font);
}

void PatternRecorder::toggleRenoisePlay() {
    renoisePlaying = !renoisePlaying; // Toggle den Status
    string command = renoisePlaying ? "start" : "stop"; // Bestimme den Befehl
    lo_address t = lo_address_new("localhost", "7005"); // Erstelle eine neue LO-Adresse

    // Sende OSC-Befehl an Renoise zum Starten oder Stoppen der Wiedergabe
    if (renoisePlaying) {
        lo_send(t, "/renoise/transport/start", ""); // Start der Wiedergabe
    } else {
        lo_send(t, "/renoise/transport/stop", ""); // Stop der Wiedergabe
    }
    
    cout << "Renoise Play " << command << endl; // Ausgabe des aktuellen Befehls
}

void PatternRecorder::toggleRenoiseRecord() {
    renoiseRecording = !renoiseRecording; // Toggle den Status
    lo_address t = lo_address_new("localhost", "7005"); // Erstelle eine neue LO-Adresse

    // Sende OSC-Befehl an Renoise zum Aktivieren oder Deaktivieren des Aufnahme-Modus
    lo_send(t, "/renoise/song/edit/mode", "B", renoiseRecording); // Boolean für Aufnahmemodus
    
    string command = renoiseRecording ? "start" : "stop"; // Bestimme den Befehl
    cout << "Renoise Record " << command << endl; // Ausgabe des aktuellen Befehls
}

void PatternRecorder::startRenoisePlayAndRecord() {
    renoisePlayAndRecordEnabled = true; // Aktivieren der Aufnahme in Renoise
    toggleRenoiseRecord(); // Start Record in Renoise
    toggleRenoisePlay(); // Start Play in Renoise
}

bool PatternRecorder::isRenoisePlayAndRecordEnabled() const {
    return renoisePlayAndRecordEnabled;
}

void PatternRecorder::clearNotes() {
    notes.clear(); // Löscht alle aufgenommenen Noten
    notes2.clear(); // Löscht alle aufgenommenen Noten
    notes3.clear(); // Löscht alle aufgenommenen Noten
    cout << "Recorded notes cleared." << endl;
}
void PatternRecorder::clearNotes2() {
    notes4.clear(); // Löscht alle aufgenommenen Noten
    newNotes.clear(); // Löscht alle aufgenommenen Noten
    scaledNotes.clear(); // Löscht alle aufgenommenen Noten
    scaledNotes2.clear(); // Löscht alle aufgenommenen Noten
    mod1.clear();

    cout << "Recorded notes4 + scaledNotes cleared." << endl;
}

int main(int argc, char* argv[]) {
    bool playrenoise = false;
    bool qPressed = false;
    bool kpressed = false;
    bool tpressed = false;
    bool sbool = false;
    bool sboolx = false;
    bool sboolinsert = false;
    bool dbool = false;
    bool present = false;
    bool playp = false;
    bool playpkey = false;
    bool playpm = false;
    bool record = false;
    bool ntr = false;
    bool save = false;
    bool snotes4 = false;
    int num;
    bool sdlkd = false;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    TTF_Init(); // Initialisiere SDL_ttf

    SDL_Window* window = SDL_CreateWindow("Pattern Recorder", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 200, 200, 0); // Fenstergröße 200x200
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // PatternRecorder recorder;
    // recorder.initialize(8);  // Für 8 Patterns

    PatternRecorder recorder1;

    SharedData sharedData;
    // PatternRecorder recorder(&sharedData, 8);  // 8 Patterns

    // for (int i=0;i<recorder.numPatterns;i++) {
    //     recorder.isPlaying.push_back(0);
    // }
    
    while (true) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_k) {
                    kpressed=true;
                    recorder1.kk=false;
                    recorder1.kl=false;
                    recorder1.kr=false;
                } else if (kpressed && event.key.keysym.sym == SDLK_1) {
                    cout<<"Geben Sie den Anfangsindex ein: "<<endl;
                    int startIndex2 = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Endindex ein: "<<endl;
                    int endIndex2 = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Anfangsindexnumstart ein: "<<endl;
                    int startIndexnumstart = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Anfangsindexnumend ein: "<<endl;
                    int startIndexnumend = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Endindexnumstart ein: "<<endl;
                    int endIndexnumstart = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Endindexnumend ein: "<<endl;
                    int endIndexnumend = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Startschrittweitanz ein: "<<endl;
                    int startschrittweitanz = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Endschrittweitanz ein: "<<endl;
                    int endschrittweitanz = stoi(recorder1.Eingabe());
                    int stepsi = endIndex2 - startIndex2;
                    int stepsschr = endschrittweitanz - startschrittweitanz;
                    int steps2anf = startIndexnumend - startIndexnumstart;
                    int steps2end = endIndexnumend - endIndexnumstart;
                    float stepsizeschr = (float)stepsschr / (float)stepsi;
                    float stepsizeanf = (float)steps2anf / (float)stepsi;
                    float stepsizeend = (float)steps2end / (float)stepsi;
                    cout<<"stepsizeanf "<<stepsizeanf<<" stepsizeend "<<stepsizeend<<endl;
                    int stepsizeschrint = (int)stepsizeschr;
                    int stepsizeanfint = (int)stepsizeanf;
                    int stepsizeendint = (int)stepsizeend;
                    cout<<"stepsizeanfint "<<stepsizeanfint<<" stepsizeendint "<<stepsizeendint<<endl;
                    int i = 0;
                    int ii = 0;
                    Festerfester event1;
                    while (ii < stepsi) {
                        float steps2anfi = (ii*stepsizeanf);
                        float steps2endi = (ii*stepsizeend);
                        float schrittwi = startschrittweitanz+(ii*stepsizeschr);
                        event1.startTime = recorder1.notes4[startIndex2+i+ii].timestamp;
                        event1.endTime = recorder1.notes4[startIndex2+i+1+ii].timestamp;
                        event1.startIndexnum = startIndexnumstart+steps2anfi;
                        event1.endIndexnum = endIndexnumstart+steps2endi;
                        event1.schrittweitanz = (int)schrittwi;
                        event1.firstrec = 0;
                        recorder1.newNotes.clear();
                        // recorder1.festerfester.push_back(event1);
                        recorder1.startIndex=startIndex2+i+ii;
                        cout<<event1.startTime<<setw(15)<<event1.endTime<<setw(15)<<event1.startIndexnum<<setw(15)<<event1.endIndexnum<<setw(15)<<event1.schrittweitanz<<endl;
                        if (event1.endTime==recorder1.notes4[recorder1.notes4.size()].timestamp) recorder1.notes4.erase(recorder1.notes4.begin()+ recorder1.startIndex);
                        else recorder1.notes4.erase(recorder1.notes4.begin() + recorder1.startIndex, recorder1.notes4.begin() + recorder1.startIndex+1);
                        recorder1.scaleToRangenum(recorder1.newNotes, event1.startTime, event1.endTime, event1.startIndexnum, event1.endIndexnum, event1.schrittweitanz, 0);
                        
                        ii++;
                        i=i+(int)schrittwi;
                    }
                    kpressed=false;

                } else if (kpressed && event.key.keysym.sym == SDLK_2) {

                    cout<<"Geben Sie den Anfangsindex ein: "<<endl;
                    int startIndex2 = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Endindex ein: "<<endl;
                    int endIndex2 = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Anfangsindexnumstart ein: "<<endl;
                    int startIndexnumstart = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Anfangsindexnumend ein: "<<endl;
                    int startIndexnumend = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Endindexnumstart ein: "<<endl;
                    int endIndexnumstart = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Endindexnumend ein: "<<endl;
                    int endIndexnumend = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Startschrittweitanz ein: "<<endl;
                    int startschrittweitanz = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Endschrittweitanz ein: "<<endl;
                    int endschrittweitanz = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Selectpitch ein: "<<endl;
                    int selectpitch = stoi(recorder1.Eingabe());
                    int stepsi = endIndex2 - startIndex2;
                    int stepsschr = endschrittweitanz - startschrittweitanz;
                    int steps2anf = startIndexnumend - startIndexnumstart;
                    int steps2end = endIndexnumend - endIndexnumstart;
                    float stepsizeschr = (float)stepsschr / (float)stepsi;
                    float stepsizeanf = (float)steps2anf / (float)stepsi;
                    float stepsizeend = (float)steps2end / (float)stepsi;
                    cout<<"stepsizeanf "<<stepsizeanf<<" stepsizeend "<<stepsizeend<<endl;
                    int stepsizeschrint = (int)stepsizeschr;
                    int stepsizeanfint = (int)stepsizeanf;
                    int stepsizeendint = (int)stepsizeend;
                    cout<<"stepsizeanfint "<<stepsizeanfint<<" stepsizeendint "<<stepsizeendint<<endl;
                    int i = 0;
                    int ii = 0;
                    int iii = 1;
                    float startTimebefore;
                    Festerfester event1;
                    
                    while (ii < stepsi) {
                        iii=0;
                        while (recorder1.notes4[startIndex2+iii].pitch!=selectpitch) {
                            iii++;
                        }
                        float steps2anfi = (ii*stepsizeanf);
                        float steps2endi = (ii*stepsizeend);
                        float schrittwi = startschrittweitanz+(ii*stepsizeschr);
                        event1.startTime = recorder1.notes4[startIndex2+ii].timestamp;
                        event1.endTime = recorder1.notes4[startIndex2+1+ii].timestamp;
                        event1.startIndexnum = startIndexnumstart+steps2anfi;
                        event1.endIndexnum = endIndexnumstart+steps2endi;
                        event1.schrittweitanz = (int)schrittwi;
                        event1.firstrec = 0;
                        recorder1.newNotes.clear();
                        // recorder1.festerfester.push_back(event1);
                        recorder1.startIndex=startIndex2+ii;
                        cout<<event1.startTime<<setw(15)<<event1.endTime<<setw(15)<<event1.startIndexnum<<setw(15)<<event1.endIndexnum<<setw(15)<<event1.schrittweitanz<<endl;
                        if (recorder1.notes4[startIndex2+ii].pitch==selectpitch) {                        
                            if (event1.endTime==recorder1.notes4[recorder1.notes4.size()].timestamp) recorder1.notes4.erase(recorder1.notes4.begin()+ recorder1.startIndex);
                            else recorder1.notes4.erase(recorder1.notes4.begin() + recorder1.startIndex, recorder1.notes4.begin() + recorder1.startIndex+1);
                            recorder1.scaleToRangenum(recorder1.newNotes, event1.startTime, event1.endTime, event1.startIndexnum, event1.endIndexnum, event1.schrittweitanz, 0);
                        }
                        i=(int)schrittwi;
                        ii=i+ii+iii;
                    }
                    kpressed=false;

                } else if (kpressed && event.key.keysym.sym == SDLK_3) {
                    cout<<"Geben Sie den Anfangsindex ein: "<<endl;
                    int startIndex2 = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Endindex ein: "<<endl;
                    int endIndex2 = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Anfangsindexnum ein: "<<endl;
                    int startIndexnum = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Endindexnum ein: "<<endl;
                    int endIndexnum= stoi(recorder1.Eingabe());
                    int stepsi = endIndex2 - startIndex2;
                    int stepsin = endIndexnum - startIndexnum;
                    float stepsizeadd = stepsin / stepsi;
                    float eqalstepnotes4 = 1 / (float)recorder1.notes4.size();
                    recorder1.notes4p.clear();
                    recorder1.notes4p = recorder1.notes4;
                    
                    for (int i=startIndex2; i<endIndex2; i++) {
                        float currentstepsize = recorder1.notes4[i+1].timestamp - recorder1.notes4[i].timestamp;
                        float newstep = recorder1.notes4[i+1].timestamp - (currentstepsize / (((float)i*stepsizeadd)+(float)startIndexnum));
                        recorder1.notes4p[i+1].timestamp = newstep;
                        recorder1.notes4p[i+1].pitch = 72;
    
                    }
                    // recorder1.notes4p.erase(recorder1.notes4p.begin(), recorder1.notes4p.begin());    
                    // recorder1.notes4p.erase(recorder1.notes4p.end(), recorder1.notes4p.end());    
                    // recorder1.pnotes[stoi(num)].insert(recorder1.pnotes[stoi(num)-1].begin(), recorder1.notes4p.begin(), recorder1.notes4p.end());
                    
                    recorder1.notes4.insert(recorder1.notes4.begin(), recorder1.notes4p.begin(), recorder1.notes4p.end());
                    // Sort notes by timestamp to ensure they're in the correct order
    
                    std::sort(recorder1.notes4.begin(), recorder1.notes4.end(), 
                        [](const Note3& a, const Note3& b) {
                            return a.timestamp < b.timestamp;
                        });
                    kpressed=false;
                } else if (kpressed && event.key.keysym.sym == SDLK_4) {
                    cout<<"Geben Sie den Anfangsindex ein: "<<endl;
                    int startIndex2 = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Endindex ein: "<<endl;
                    int endIndex2 = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Anfangsindexnum ein: "<<endl;
                    int startIndexnumstart = stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den Endindexnum ein: "<<endl;
                    int endIndexnumstart= stoi(recorder1.Eingabe());
                    cout<<"Geben Sie den selectpitch ein: "<<endl;
                    int selectpitch= stoi(recorder1.Eingabe());
                    int stepsi = endIndex2 - startIndex2;
                    int stepsin = endIndexnumstart - startIndexnumstart;
                    float stepsizeadd = stepsin / stepsi;
                    float eqalstepnotes4 = 1 / (float)recorder1.notes4.size();
                    recorder1.notes4p.clear();
                    recorder1.notes4p = recorder1.notes4;
                    
                    for (int i=startIndex2; i<endIndex2; i++) {
                        float currentstepsize = recorder1.notes4[i+1].timestamp - recorder1.notes4[i].timestamp;
                        float newstep = recorder1.notes4[i+1].timestamp - (currentstepsize / (((float)i*stepsizeadd)+(float)startIndexnumstart));
                        if (recorder1.notes4p[i+1].pitch==selectpitch) {
                            recorder1.notes4p[i+1].timestamp = newstep;
                        }
                    cout<<i<<endl;
                    }
                    // recorder1.notes4p.erase(recorder1.notes4p.begin(), recorder1.notes4p.begin());    
                    // recorder1.notes4p.erase(recorder1.notes4p.end(), recorder1.notes4p.end());    
                    // recorder1.pnotes[stoi(num)].insert(recorder1.pnotes[stoi(num)-1].begin(), recorder1.notes4p.begin(), recorder1.notes4p.end());
                    recorder1.notes4.insert(recorder1.notes4.begin()+startIndex2, recorder1.notes4p.begin(), recorder1.notes4p.end());
                    // Sort notes by timestamp to ensure they're in the correct order
                    
                    cout<<"insert"<<endl;
                    std::sort(recorder1.notes4.begin(), recorder1.notes4.end(), 
                        [](const Note3& a, const Note3& b) {
                            return a.timestamp < b.timestamp;
                        });
                    cout<<"sorted"<<endl;
                    kpressed=false;
                } else if (kpressed && event.key.keysym.sym == SDLK_5) {
                    float eqalstepnotes4 = 1 / (float)recorder1.notes4.size();
                    cout << "pnotes" << num << endl;
                    std::string num = recorder1.Eingabe();
                    recorder1.notes4p.clear();
                    recorder1.notes4p = recorder1.notes4;

                    for (int i=0; i<recorder1.notes4.size(); i++) {
                        float currentstepsizefactor = 1 / (float)recorder1.notes4[i+1].timestamp - recorder1.notes4[i].timestamp;
                        float currentpnotestepsize = recorder1.pnotes[stoi(num)-1][i+1].timestamp - recorder1.pnotes[stoi(num)-1][i].timestamp;
                        float newstep = currentpnotestepsize * currentstepsizefactor;
                        recorder1.notes4p[i+1].timestamp = recorder1.pnotes[stoi(num)-1][i].timestamp + newstep;
    
                    }
                    // recorder1.notes4p.erase(recorder1.notes4p.begin(), recorder1.notes4p.begin());    
                    // recorder1.notes4p.erase(recorder1.notes4p.end(), recorder1.notes4p.end());    
                    // recorder1.pnotes[stoi(num)].insert(recorder1.pnotes[stoi(num)-1].begin(), recorder1.notes4p.begin(), recorder1.notes4p.end());
                    
                    recorder1.notes4=recorder1.notes4p;
                    recorder1.notes4.insert(recorder1.notes4.begin(), recorder1.pnotes[stoi(num)-1].begin(), recorder1.pnotes[stoi(num)-1].end());
                    // Sort notes by timestamp to ensure they're in the correct order
    
                    std::sort(recorder1.notes4.begin(), recorder1.notes4.end(), 
                        [](const Note3& a, const Note3& b) {
                            return a.timestamp < b.timestamp;
                        });
                    kpressed=false;
                } else if (kpressed && event.key.keysym.sym == SDLK_6) {
                    recorder1.removeduplicates(recorder1.notes4);
                    kpressed=false;
                } else if (kpressed && event.key.keysym.sym == SDLK_7) {
                    recorder1.anfendIndex();
                    recorder1.totalDurationbreak(recorder1.startTime, recorder1.endTime);
                    kpressed=false;
                } else if (kpressed && event.key.keysym.sym == SDLK_8) {
                    recorder1.totalDi=!recorder1.totalDi;
                    cout<<"totalDi: "<<recorder1.totalDi<<endl;
                    kpressed=false;
                } else if (kpressed && event.key.keysym.sym == SDLK_DOWN) {
                    recorder1.anfendIndex();
                    recorder1.kk=true;
                    recorder1.remove();
                    cout<<"startTime "<<recorder1.startTime<<" endTime "<<recorder1.endTime<<" totalDuration "<<recorder1.startTime-recorder1.endTime<<endl;
                    cout << "Geben Sie den Anfangsindexnum ein: "<<endl;
                    recorder1.startIndexnum = stof(recorder1.Eingabe());
                    cout << "Geben Sie den Endindexnum ein: "<<endl;
                    recorder1.endIndexnum = stof(recorder1.Eingabe());
                    if(recorder1.totalDi==0) {
                        recorder1.Indexnumlaenge = 1 - (1/recorder1.startIndexnum)- (1/recorder1.endIndexnum);
                    } else {
                        recorder1.Indexnumlaenge = (recorder1.endTime - recorder1.startTime) - (1/recorder1.startIndexnum)- (1/recorder1.endIndexnum);
                    }
                    cout << "Indexnumlaenge: "<<recorder1.Indexnumlaenge<<endl;
                    cout << "Geben Sie den schrittweitanz ein zwischen: "<<recorder1.Indexnumlaenge/(1/recorder1.startIndexnum)<<" und "<<recorder1.Indexnumlaenge/(1/recorder1.endIndexnum)<<endl;
                    recorder1.schrittweitanz = stoi(recorder1.Eingabe());
                    recorder1.scaleToRangenum(recorder1.newNotes, recorder1.startTime, recorder1.endTime, recorder1.startIndexnum, recorder1.endIndexnum, recorder1.schrittweitanz, recorder1.firstrec);
                    kpressed=false;
                } else if (kpressed && event.key.keysym.sym == SDLK_UP) {
                    recorder1.anfendIndex();
                    recorder1.kk=false;
                    recorder1.remove();
                    cout<<"startTime "<<recorder1.startTime<<" endTime "<<recorder1.endTime<<" totalDuration "<<recorder1.startTime-recorder1.endTime<<endl;
                    cout << "Geben Sie den Anfangsindexnum ein: "<<endl;
                    recorder1.startIndexnum = stof(recorder1.Eingabe());
                    cout << "Geben Sie den Endindexnum ein: "<<endl;
                    recorder1.endIndexnum = stof(recorder1.Eingabe());
                    if(recorder1.totalDi==0) {
                        recorder1.Indexnumlaenge = 1;
                    } else {
                        recorder1.Indexnumlaenge = (recorder1.endTime - recorder1.startTime) - (1/recorder1.startIndexnum);
                    }
                    cout << "Indexnumlaenge: "<<recorder1.Indexnumlaenge<<endl;
                    cout << "Geben Sie den schrittweitanz ein zwischen: "<<recorder1.Indexnumlaenge/(1/recorder1.startIndexnum)<<" und "<<recorder1.Indexnumlaenge/(1/recorder1.endIndexnum)<<endl;
                    recorder1.schrittweitanz = stoi(recorder1.Eingabe());
                    recorder1.scaleToRangenum(recorder1.newNotes, recorder1.startTime, recorder1.endTime, recorder1.startIndexnum, recorder1.endIndexnum, recorder1.schrittweitanz, recorder1.firstrec);
                    kpressed=false;
                } else if (kpressed && event.key.keysym.sym == SDLK_LEFT) {
                    recorder1.anfendIndex();
                    recorder1.kl=true;
                    recorder1.remove();
                    cout<<"startTime "<<recorder1.startTime<<" endTime "<<recorder1.endTime<<" totalDuration "<<recorder1.startTime-recorder1.endTime<<endl;
                    cout << "Geben Sie den Anfangsindexnum ein: "<<endl;
                    recorder1.startIndexnum = stof(recorder1.Eingabe());
                    cout << "Geben Sie den Endindexnum ein: "<<endl;
                    recorder1.endIndexnum = stof(recorder1.Eingabe());
                    if(recorder1.totalDi==0) {
                        recorder1.Indexnumlaenge = 1 - (1/recorder1.startIndexnum);
                    } else {
                        recorder1.Indexnumlaenge = (recorder1.endTime - recorder1.startTime) - (1/recorder1.startIndexnum);
                    }

                    cout << "Indexnumlaenge: "<<recorder1.Indexnumlaenge<<endl;
                    cout << "Geben Sie den schrittweitanz ein zwischen: "<<recorder1.Indexnumlaenge/(1/recorder1.startIndexnum)<<" und "<<recorder1.Indexnumlaenge/(1/recorder1.endIndexnum)<<endl;
                    recorder1.schrittweitanz = stoi(recorder1.Eingabe());
                    recorder1.scaleToRangenum(recorder1.newNotes, recorder1.startTime, recorder1.endTime, recorder1.startIndexnum, recorder1.endIndexnum, recorder1.schrittweitanz, recorder1.firstrec);
                    kpressed=false;
                } else if (kpressed && event.key.keysym.sym == SDLK_RIGHT) {
                    recorder1.anfendIndex();
                    recorder1.kr=true;
                    recorder1.remove();
                    cout<<"startTime "<<recorder1.startTime<<" endTime "<<recorder1.endTime<<" totalDuration "<<recorder1.startTime-recorder1.endTime<<endl;
                    cout << "Geben Sie den Anfangsindexnum ein: "<<endl;
                    recorder1.startIndexnum = stof(recorder1.Eingabe());
                    cout << "Geben Sie den Endindexnum ein: "<<endl;
                    recorder1.endIndexnum = stof(recorder1.Eingabe());
                    if(recorder1.totalDi==0) {
                        recorder1.Indexnumlaenge = 1 - (1/recorder1.endIndexnum);
                    } else {
                        recorder1.Indexnumlaenge = (recorder1.endTime - recorder1.startTime) - (1/recorder1.endIndexnum);
                    }

                    cout << "Indexnumlaenge: "<<recorder1.Indexnumlaenge<<endl;
                    cout << "Geben Sie den schrittweitanz ein zwischen: "<<recorder1.Indexnumlaenge/(1/recorder1.startIndexnum)<<" und "<<recorder1.Indexnumlaenge/(1/recorder1.endIndexnum)<<endl;
                    recorder1.schrittweitanz = stoi(recorder1.Eingabe());
                    recorder1.scaleToRangenum(recorder1.newNotes, recorder1.startTime, recorder1.endTime, recorder1.startIndexnum, recorder1.endIndexnum, recorder1.schrittweitanz, recorder1.firstrec);
                    kpressed=false;
                } else if (event.key.keysym.sym == SDLK_g) {
                    recorder1.createGraphWindow();
                } else if (event.key.keysym.sym == SDLK_z) {
                    recorder1.pasteRenoisePattern();
                } else if (event.key.keysym.sym == SDLK_r) {
                    record=true;
                } else if (record && event.key.keysym.sym == SDLK_1) {
                    recorder1.clearNotes(); // Clear recorded notes
                    recorder1.record();
                    record=false;
                } else if (event.key.keysym.sym == SDLK_2) {
                    recorder1.clearNotes(); // Clear recorded notes
                    std::string svalue = "";
                    cout<<"Geben Sie die Anzahl der Noten ein: "<<endl;
                    svalue = recorder1.Eingabe();
                    for (int i = 0; i < stoi(svalue)+1; i++) {
                        recorder1.notes4.push_back({(1.0f/stof(svalue))*(float)i,0,59+i});
                    }
                    record=false;
                } else if (event.key.keysym.sym == SDLK_z) {
                    recorder1.anfendIndex();
                    recorder1.remove();
                } else if (event.key.keysym.sym == SDLK_w) {
                    std::string svalue = "";
                    svalue = recorder1.Eingabe();
                    recorder1.plsl = std::stoi(svalue);
                    cout<<"plsl: "<<recorder1.plsl<<endl;
                } else if (event.key.keysym.sym == SDLK_t) {
                    tpressed=true;
                } else if (tpressed && (event.key.keysym.sym == SDLK_UP) || tpressed && (event.key.keysym.sym == SDLK_DOWN)) {
                    if (event.key.keysym.sym == SDLK_DOWN) {
                        recorder1.bpmtap();
                    } else {
                    std::string svalue = "";
                    float value;
                    svalue = recorder1.Eingabe();
                    recorder1.sharedData->bpm = std::stof(svalue);
                    recorder1.setBPM();
                    recorder1.zeitfaktor();
                    cout << "BPM: "<<recorder1.sharedData->bpm<<endl;;
                    }
                    tpressed=false;
                } else if (event.key.keysym.sym == SDLK_q) {
                    qPressed = true;
                } else if (qPressed && (event.key.keysym.sym == SDLK_3 || event.key.keysym.sym == SDLK_4)) {
                    int quantizationValue;
                    cout << "Geben Sie den Quantisierungswert ein (z.B. 16 für 16tel, 32 für 32tel): ";
                    std::string value = "";
                    value=recorder1.Eingabe();
                    quantizationValue = std::stoi(value);
                    
                    if (event.key.keysym.sym == SDLK_3) {
                        recorder1.quantizeNotes(recorder1.notes3, quantizationValue);
                        cout << "notes3 quantisiert mit " << quantizationValue << "-tel" << endl;
                    } else {
                        recorder1.quantizeNotes(recorder1.notes4, quantizationValue);
                        cout << "notes4 quantisiert mit " << quantizationValue << "-tel" << endl;
                    }
                    qPressed = false;
                } else if (event.key.keysym.sym == SDLK_m) {
                    recorder1.modul1();
                } else if (event.key.keysym.sym == SDLK_d) {
                    sdlkd=true;
                } else if (sdlkd && event.key.keysym.sym == SDLK_1) {
                    recorder1.DisPlNotes();
                // } else if (event.key.keysym.sym == SDLK_p) {
                //     if (playrenoise) {
                //             recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
                //             recorder1.play(); // Pattern abspielen
                //             recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
                //     }
                //     else { 
                //             recorder1.play(); // Pattern abspielen
                //     }
                //     // if (recorder1.isRenoisePlayAndRecordEnabled()) {
                //     //     recorder1.toggleRenoiseRecord(); // Starte die Aufnahme in Renoise
                //     // }
                sdlkd=false;
            } else if (sdlkd && event.key.keysym.sym == SDLK_2) {
                // sinusInterpolationBetweenTimestamps(recorder1.notes4, 10);
                sdlkd=false;
            } else if (sdlkd && event.key.keysym.sym == SDLK_3) {
                recorder1.notes5.clear();
                recorder1.notes4s.clear();
                cout<<"Anz Interpolationen?"<<endl;
                std::string svalue1 = recorder1.Eingabe();
                cout<<"x*Pi?"<<endl;
                std::string svalue11 = recorder1.Eingabe();
                cout<<"1/x*Pi?"<<endl;
                std::string svalue12 = recorder1.Eingabe();
                cout<<"Oszillationsamplitude in Halbtontasten?"<<endl;
                std::string svalue111 = recorder1.Eingabe();
                recorder1.notes5 = recorder1.interpolateNotesWithSinusPitch(recorder1.notes4, std::stoi(svalue1), std::stof(svalue11)/std::stof(svalue12), std::stof(svalue111));
                recorder1.notes4 = recorder1.notes5;
                sdlkd=false;
            } else if (sdlkd && event.key.keysym.sym == SDLK_4) {
                recorder1.notes4 = recorder1.notes4s;
                sdlkd=false;
            } else if (sdlkd && event.key.keysym.sym == SDLK_5) {
                recorder1.vmidi = !recorder1.vmidi;
                sdlkd=false;
            // } else if (event.key.keysym.sym == SDLK_q) {
            //     v1=!v1;
            } else if (event.key.keysym.sym == SDLK_o) {
                    if (playrenoise) {
                            recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
                            recorder1.play2(); // Pattern abspielen
                            recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
                    }
                    else {
                            recorder1.play2(); // Pattern abspielen
                    }
                    // if (recorder1.isRenoisePlayAndRecordEnabled()) {
                    //     recorder1.toggleRenoiseRecord(); // Starte die Aufnahme in Renoise
                    // }
                // } else if (event.key.keysym.sym == SDLK_i) {
                //     if (playrenoise) {
                //             recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
                //             recorder1.play3(); // Pattern abspielen
                //             recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
                //     }
                //     else {
                //             recorder1.play3(); // Pattern abspielen
                //     }
                } else if (event.key.keysym.sym == SDLK_j) {
                    present=true;
                    cout<<"jpresav"<<endl;
                    } else if (present && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_8)) {
                    int num = event.key.keysym.sym - SDLK_0 -1;
                    cout << "pnotes" << num << endl;
                    recorder1.pnotes[num] = recorder1.notes4;

                    for (const auto& note : recorder1.pnotes[num]) {
                        cout << note.timestamp << "," << note.instrument << "," << note.pitch << "," << note.p2 << endl;
                    }
    
                    recorder1.pdauer[num][0]=recorder1.sharedData->dauer;
                    cout<<recorder1.pdauer[num][0]<<endl;

                    present = false;


                } else if (event.key.keysym.sym == SDLK_h) {
                    cout<<"playp on"<<endl;
                    playp=true;
                    } else if (playp && (event.key.keysym.sym >= SDLK_m)) {
                        playpm=!playpm;
                        cout<<"playpm on/off"<<playpm<<endl;
                    // } else if (playp && !playpm && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_8)) {
                    //     num = event.key.keysym.sym - SDLK_0 -1;
                    //     cout<<"playnum: "<<num<<endl;
                    //     recorder1.play4(num, event); // Pattern abspielen
                    } else if (playp && !playpm && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_8)) {
                        num = event.key.keysym.sym - SDLK_0 - 1;
                        cout << "playnum: " << num << endl;
                        
                        // Berechne die Dauer eines 16tel-Schlags in Millisekunden
                        float sixteenthNoteDuration = (60.0f / recorder1.sharedData->bpm) * 1000 / 4;
                        
                        // Berechne die aktuelle Zeit und die Zeit bis zum nächsten 16tel-Schlag
                        Uint32 currentTime = SDL_GetTicks();
                        Uint32 nextSixteenthNote = currentTime + (sixteenthNoteDuration - (currentTime % static_cast<Uint32>(sixteenthNoteDuration)));
                        
                        // Warte bis zum nächsten 16tel-Schlag
                        SDL_Delay(nextSixteenthNote - currentTime);
                        
                        // Starte play4
                        recorder1.play4(num, event);
                        } else if (playp && playpm && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_8)) {
                            num = event.key.keysym.sym - SDLK_0 -1;
                            cout<<"playnum: "<<num<<endl;
                                    recorder1.play34(num); // Pattern abspielen
                        } else if ((playp || playpm) && (event.key.keysym.sym == SDLK_9)) {
                        playp=false;
                        playpm=false;
                        cout<<"playp/m off"<<endl;
                } else if (event.key.keysym.sym == SDLK_s) {
                    sbool=1;
                } else if (sbool && event.key.keysym.sym == SDLK_UP) {
                    sboolinsert=1;
                } else if (sboolinsert && sbool && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_8)) {
                    int num = event.key.keysym.sym - SDLK_0 -1;
                    recorder1.vectorinsert(num);
                    sboolinsert=0;
                    sbool=0;
                } else if (sbool && event.key.keysym.sym == SDLK_x) {
                    sboolx=1;
                } else if (sbool && sboolx && event.key.keysym.sym == SDLK_1) {
                    cout<<"convert notes to Renoise pattern with length?"<<endl;
                    string length = "";
                    length = recorder1.Eingabe();
                    recorder1.convertNotesToRenoisePattern(stoi(length));
                    sboolx=0;
                    sbool=0;
                } else if (sbool && sboolx && event.key.keysym.sym == SDLK_2) {
                    cout<<"convert notes to Renoise pattern with length?"<<endl;
                    string length = "";
                    length = recorder1.Eingabe();
                    recorder1.convertNotesToSchismPattern(stoi(length));
                    sboolx=0;
                    sbool=0;
                } else if (sbool && event.key.keysym.sym == SDLK_0) {
                        save=!save;
                        sbool=0;
                    } else if (sbool && event.key.keysym.sym == SDLK_i) {
                        cout<<"snotes3 on/off"<<endl;
                        snotes4=!snotes4;
                    } else if (sbool && event.key.keysym.sym == SDLK_p) {
                        
                    } else if (sbool && event.key.keysym.sym == SDLK_9) {
                        for (int i=1;i<9;++i) {
                            recorder1.saveToCSV("pattern "+to_string(i)+".csv", recorder1.pnotes, i);
                            cout << "Pattern "+to_string(i)+" saved to pattern.csv" << endl;
                        }
                        sbool=0;
                    } else if (snotes4 && sbool && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_8)) {
                        int num = event.key.keysym.sym - SDLK_0 -1;
                        recorder1.notes4=recorder1.pnotes[num];
                        cout << "Pattern "+to_string(num)+" notes4" << endl;
                        snotes4=0;
                        sbool=0;
                    } else if (sbool && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_8)) {
                        int num = event.key.keysym.sym - SDLK_0 -1;
                        recorder1.saveToCSV("pattern "+to_string(num)+".csv", recorder1.pnotes, num);
                        cout << "Pattern "+to_string(num)+" saved to pattern.csv" << endl;
                        sbool=0;
                } else if (event.key.keysym.sym == SDLK_f) {
                        dbool=1;
                    } else if (dbool && event.key.keysym.sym == SDLK_9) {
                        for (int i=1;i<9;++i) {
                            recorder1.readFromCSV("pattern "+to_string(i)+".csv", i);
                            cout << "Pattern "+to_string(i)+" loaded from pattern.csv" << endl;
                        }
                        dbool=0;
                    } else if (dbool && (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_8)) {
                        int num = event.key.keysym.sym - SDLK_0 -1;
                        recorder1.readFromCSV("pattern "+to_string(num)+".csv", num);
                        cout << "Pattern "+to_string(num)+" loaded from pattern.csv" << endl;
                        dbool=0;
                } else if (event.key.keysym.sym == SDLK_c) {
                    recorder1.clearNotes(); // Clear recorded notes
                } else if (event.key.keysym.sym == SDLK_y) {
                    bool patternSelectMode = true;
                    std::cout << "Pattern Select Mode: Press 1-8 to play pattern with sound, 9 to exit" << std::endl;
                    
                    while (patternSelectMode) {
                        SDL_Event patternEvent;
                        while (SDL_PollEvent(&patternEvent)) {
                            if (patternEvent.type == SDL_KEYDOWN) {
                                if (patternEvent.key.keysym.sym >= SDLK_1 && patternEvent.key.keysym.sym <= SDLK_8) {
                                    int patternNum = patternEvent.key.keysym.sym - SDLK_1;
                                    std::cout << "Playing pattern " << (patternNum + 1) << " with sound" << std::endl;
                                    recorder1.playPatternAsSound(patternNum);
                                } else if (patternEvent.key.keysym.sym == SDLK_9) {
                                    std::cout << "Exiting pattern select mode" << std::endl;
                                    patternSelectMode = false;
                                    break;
                                }
                            // } else if (patternEvent.type == SDL_QUIT) {
                            //     quit = true;
                            //     patternSelectMode = false;
                            //     break;
                            }
                        }
                        SDL_Delay(10); // Small delay to prevent CPU overuse
                    }
                    
                } else if (event.key.keysym.sym == SDLK_x) {
                    recorder1.clearNotes2(); // Clear recorded notes
                } else if (event.key.keysym.sym == SDLK_b) {
                    ntr=!ntr;
                    cout<<"ntr:"<<ntr<<endl;
                    } else if (ntr && (event.key.keysym.sym == SDLK_PAGEUP)) {
                        recorder1.transformTimestamps();
                        ntr=!ntr;
                    } else if (ntr && (event.key.keysym.sym == SDLK_PAGEDOWN)) {
                        recorder1.reverseTransformTimestamps();
                        ntr=!ntr;
                    } else if (ntr && (event.key.keysym.sym == SDLK_DOWN)) {
                        recorder1.recntr();
                        ntr=!ntr;
                    } else if (ntr && (event.key.keysym.sym == SDLK_RIGHT)) {
                        recorder1.recntrf();
                        ntr=!ntr;
                    } else if (ntr && (event.key.keysym.sym == SDLK_LEFT)) {
                        recorder1.notelenght();
                        ntr=!ntr;
                    } else if (ntr && (event.key.keysym.sym == SDLK_UP)) {
                        cout<<"Glide Rate?"<<endl;
                        recorder1.noteglide();
                        ntr=!ntr;
                } else if (event.key.keysym.sym == SDLK_v) {
                    // recorder1.vecforrecf();
                    recorder1.fdx();
                } else if (event.key.keysym.sym == SDLK_LEFT) {
                    recorder1.toene--;
                } else if (event.key.keysym.sym == SDLK_RIGHT) {
                    recorder1.toene++;
                } else if (event.key.keysym.sym == SDLK_UP) {
                    recorder1.toene+=12;
                } else if (event.key.keysym.sym == SDLK_DOWN) {
                    recorder1.toene-=12;
                } else if (event.key.keysym.sym == SDLK_e) {
                    recorder1.anfendIndex();
                    recorder1.removeAndRecord();
                } else if (event.key.keysym.sym == SDLK_p) {
                    cout<<"Set Pattern Dauer für Pattern?"<<endl;
                    std::string svalue = "";
                    svalue = recorder1.Eingabe();
                    recorder1.setpdauer(std::stoi(svalue)-1);
                } else if (event.key.keysym.sym == SDLK_l) {
                    std::string svalue = "";
                    svalue = recorder1.Eingabe();
                    recorder1.sharedData->dauer = std::stoi(svalue);
                    cout << "Dauer in Beats?" << recorder1.sharedData->dauer << endl;
                    recorder1.zeitfaktor();
                } else if (event.key.keysym.sym == SDLK_u) {
                    std::string svalue = "";
                    svalue = recorder1.Eingabe();
                    recorder1.loops = std::stoi(svalue);
                    cout << "Loops " << recorder1.loops << endl;
                } else if (event.key.keysym.sym == SDLK_F5) {
                    recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
                } else if (event.key.keysym.sym == SDLK_F8) {
                    // recorder1.startRenoisePlayAndRecord(); // Start Play und Record in Renoise
                playrenoise=!playrenoise;
                } else if (event.key.keysym.sym == SDLK_F9) {
                    recorder1.toggleRenoiseRecord(); // Toggle Record in Renoise
                } else if (event.key.keysym.sym == SDLK_F2) {
                    std::string svalue = "";
                    cout<<"Select pnotes[num]"<<endl;
                    svalue = recorder1.Eingabe();
                    int num = stoi(svalue)-1;
                    recorder1.select(num);
                } else if (event.key.keysym.sym == SDLK_F3) {
                    std::string svalue = "";
                    cout<<"Select for pnotes[num] to reverse"<<endl;
                    svalue = recorder1.Eingabe();
                    int num = stoi(svalue)-1;
                    recorder1.reverse(num);
                } else if (event.key.keysym.sym == SDLK_F4) {
                    std::string svalue = "";
                    cout<<"Select pnotes[num] to operate"<<endl;
                    svalue = recorder1.Eingabe();
                    int num = stoi(svalue)-1;
                    cout<<"1 *, 2 /, 3 +, 4 -"<<endl;
                    svalue = recorder1.Eingabe();
                    int operate = stoi(svalue);
                    cout<<"Faktor: ?"<<endl;
                    svalue = recorder1.Eingabe();
                    recorder1.factor = stoi(svalue);
                    if (operate == 1) {
                        recorder1.mal(num,recorder1.factor);
                    } else if (operate == 2) {
                        recorder1.durch(num,recorder1.factor);
                    } else if (operate == 3) {
                        recorder1.plus(num,recorder1.factor);
                    } else if (operate == 4) {
                        recorder1.minus(num,recorder1.factor);
                    }
                }
            } else if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_q) {
                    qPressed = false;
                } else if (event.key.keysym.sym == SDLK_k) {
                    kpressed=true;
                }
            }
        }
        if (recorder1.sharedData->p1 == true) {
            if (playrenoise) {
                    recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
                    recorder1.play(); // Pattern abspielen
                    recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
            }
            else { 
                    recorder1.play(); // Pattern abspielen
            }
        } else if (recorder1.sharedData->p2 == true) {
            if (playrenoise) {
                    recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
                    recorder1.play2(); // Pattern abspielen
                    recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
            }
            else {
                    recorder1.play2(); // Pattern abspielen
            }
        } else if (recorder1.sharedData->p3 == true) {
            if (playrenoise) {
                    recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
                    recorder1.play3(); // Pattern abspielen
                    recorder1.toggleRenoisePlay(); // Toggle Play in Renoise
            }
            else {
                    recorder1.play3(); // Pattern abspielen
            }
        }

if (save) {
                    std::string svalue = "";
                    cout<<"Width?"<<endl;
                    svalue = recorder1.Eingabe();
                    int width = std::stoi(svalue);

                    int ii, iii;
                    // cin>>width;
                    // ii=0;
                    // iii=0;
                    ofstream file("./KeyEventRender.txt");
                    for (int la=0;la<recorder1.loops;++la) {
                        ii=0;
                        iii=0;
                        for (float a=0;a<(width/recorder1.loops);++a) {
                            // cout<<a<<endl;
                            cout<<a+(la*width/recorder1.loops)<<"  "<<a<<"  "<<recorder1.notes4[ii].timestamp*width<<"  "<<ii<<endl;
                            
                            
                            if (a>=(recorder1.notes4[ii].timestamp*width/recorder1.loops)) {
                                iii=1;
                                cout<<"set "<<a+(width*la)<<" "<<recorder1.notes4[ii].timestamp*a<<endl; 
                                ii++;
                            }
                            else iii=0;
                            
                            file << a+(width/recorder1.loops*la) << ";" << iii << ";" << iii << ";" << iii << endl;

                        }
                    }
                    file.close();
                    cout<<"txt saved"<<endl;
                    save=!save;

}

        // Fensterinhalt aktualisieren
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Hintergrundfarbe
        SDL_RenderClear(renderer);
        recorder1.display(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}