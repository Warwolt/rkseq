#include "hardware/midi_transmit.h"

#include "hardware/software_serial.h"

#define MIDI_STATUS_NOTE_ON(channel) ((uint8_t)(0x90 | (channel)))
#define MIDI_STATUS_NOTE_OFF(channel) ((uint8_t)(0x80 | (channel)))

void MidiTransmit_send_message(MidiMessage msg) {
	switch (msg.type) {
		case MIDI_MESSAGE_TYPE_NOTE_ON:
			SoftwareSerial_write(MIDI_STATUS_NOTE_ON(msg.data.note_on.channel));
			SoftwareSerial_write(msg.data.note_on.note_number);
			SoftwareSerial_write(msg.data.note_on.velocity);
			break;

		case MIDI_MESSAGE_TYPE_NOTE_OFF:
			SoftwareSerial_write(MIDI_STATUS_NOTE_OFF(msg.data.note_off.channel));
			SoftwareSerial_write(msg.data.note_off.note_number);
			SoftwareSerial_write(0); // velocity
			break;
	}
}