Pattern Recorder 3 (PR3)
Ein fortschrittliches Pattern-Recording-Tool für die interaktive Erstellung und Manipulation von MIDI-Patterns.

Hauptfunktionen
Pattern-Management
8 Pattern-Slots zur Speicherung von MIDI-Sequenzen
Interaktive Pattern-Editor mit visueller Darstellung
Pattern-Export/Import als CSV-Dateien
Audio-Features
Echtzeit-Audio-Playback der Patterns
MIDI-Note-Quantisierung
Glissando-Effekte
Instrument-Selektion
Renoise-Integration
Direkte Kommunikation mit Renoise
Pattern-Export/Import als Renoise-Pattern
BPM-Synchronisation
Play/Record-Steuerung
Pattern-Manipulation
Zeitstreckung und -komprimierung
Note-Quantisierung
Instrument-Wechsel
Pattern-Reversing
Multiplikation und Division von Noten
Technische Details
Hauptkomponenten
PatternRecorder-Klasse
Verwaltung von 8 Pattern-Slots
Audio-Playback-Funktionalität
Renoise-Integration
Pattern-Manipulation
Datenstrukturen
cpp
CopyInsert
struct Note3 {
    float timestamp;
    int instrument;
    int pitch;
    bool glide;
    float p2;
};

struct Notescale {
    float timestamp1;
    float timestamp2;
    float notev2;
    float notev1;
    int startIndexrecord;
};
Audio-System
Echtzeit-Audio-Playback mit SDL2
MIDI-Note zu Frequenz-Konvertierung
Glissando-Effekte
BPM-basierte Timing
Benutzung
Grundlegende Befehle
1-8: Pattern-Slot-Auswahl
r: Aufnahme starten
p: Playback starten
s: Pattern speichern
l: Pattern laden
Erweiterte Funktionen
q: Note-Quantisierung
g: Glissando-Effekt
i: Instrument-Wechsel
t: BPM-Tap
Renoise-Integration
Ctrl+P: Play in Renoise
Ctrl+R: Record in Renoise
Ctrl+T: Pattern-Transfer
Systemanforderungen
SDL2
PortAudio
Boost.Interprocess
TinyXML2
OpenCV (optional)
Fehlerbehandlung
Null-Zeiger-Überprüfung
Speicherzugriffs-Validierung
Thread-Synchronisation
MIDI-Event-Validierung
Optimierungen
Thread-basierte Audio-Playback
Effiziente Pattern-Verarbeitung
Optimierte MIDI-Kommunikation
Speicher-optimierte Datenstrukturen
Zukunftige Erweiterungen
Erweiterte Pattern-Transformationen
Mehrfache Pattern-Overlay
Erweiterte Renoise-Integration
Pattern-Sharing-Funktionalität
