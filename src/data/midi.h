#ifndef MIDI_H
#define MIDI_H

#define MIDI_NO_MSG 0x00
#define MIDI_TIMING_CLOCK 0xF8
#define MIDI_START 0xFA
#define MIDI_CONTINUE 0xFB
#define MIDI_STOP 0xFC
#define MIDI_NOTE_ON(channel) ((uint8_t)(0x90 | (channel)))
#define MIDI_NOTE_OFF(channel) ((uint8_t)(0x80 | (channel)))

#endif /* MIDI_H */
