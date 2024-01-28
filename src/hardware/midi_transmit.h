#ifndef MIDI_TRANSMIT_H
#define MIDI_TRANSMIT_H

#include <stdint.h>

typedef enum {
	MIDI_MESSAGE_TYPE_NOTE_ON,
	MIDI_MESSAGE_TYPE_NOTE_OFF,
} MidiMessageType;

typedef struct {
	uint8_t channel;
	uint8_t note_number;
	uint8_t velocity;
} MidiNoteOn;

typedef struct {
	uint8_t channel;
	uint8_t note_number;
	uint8_t velocity;
} MidiNoteOff;

typedef struct {
	MidiMessageType type;
	union {
		MidiNoteOn note_on;
		MidiNoteOff note_off;
	} data;
} MidiMessage;

#define MIDI_MESSAGE_NOTE_ON(channel_, note_number_, velocity_) \
	((MidiMessage) {                                            \
		.type = MIDI_MESSAGE_TYPE_NOTE_ON,                      \
		.data.note_on = { .channel = channel_, note_number_, .velocity = velocity_ } })

#define MIDI_MESSAGE_NOTE_OFF(channel_, note_number_, velocity_) \
	((MidiMessage) {                                             \
		.type = MIDI_MESSAGE_TYPE_NOTE_OFF,                      \
		.data.note_on = { .channel = channel_, note_number_, .velocity = velocity_ } })

void MidiTransmit_send_message(MidiMessage msg);

#endif /* MIDI_TRANSMIT_H */
