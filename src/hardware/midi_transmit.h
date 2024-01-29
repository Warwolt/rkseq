#ifndef MIDI_TRANSMIT_H
#define MIDI_TRANSMIT_H

#include <stdint.h>

#include "hardware/software_serial.h"

typedef enum {
	MIDI_MESSAGE_TYPE_TIMING_CLOCK,
	MIDI_MESSAGE_TYPE_NOTE_OFF,
	MIDI_MESSAGE_TYPE_NOTE_ON,
} MidiMessageType;

typedef struct {
	uint8_t channel;
	uint8_t note_number;
	uint8_t velocity;
} MidiNoteOn;

typedef struct {
	uint8_t channel;
	uint8_t note_number;
} MidiNoteOff;

typedef struct {
	MidiMessageType type;
	union {
		MidiNoteOn note_on;
		MidiNoteOff note_off;
	} data;
} MidiMessage;

#define MIDI_MESSAGE_TIMING_CLOCK               \
	((MidiMessage) {                            \
		.type = MIDI_MESSAGE_TYPE_TIMING_CLOCK, \
		.data = { 0 } })

#define MIDI_MESSAGE_NOTE_ON(channel_, note_number_, velocity_) \
	((MidiMessage) {                                            \
		.type = MIDI_MESSAGE_TYPE_NOTE_ON,                      \
		.data.note_on = {                                       \
			.channel = channel_,                                \
			.note_number = note_number_,                        \
			.velocity = velocity_,                              \
		} })

#define MIDI_MESSAGE_NOTE_OFF(channel_, note_number_) \
	((MidiMessage) {                                  \
		.type = MIDI_MESSAGE_TYPE_NOTE_OFF,           \
		.data.note_off = {                            \
			.channel = channel_,                      \
			.note_number = note_number_,              \
		} })

void MidiTransmit_send_message(SoftwareSerial sw_serial, MidiMessage msg);

#endif /* MIDI_TRANSMIT_H */
