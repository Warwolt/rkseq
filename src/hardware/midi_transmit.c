#include "hardware/midi_transmit.h"

#include "data/midi.h"
#include "hardware/software_serial.h"

void MidiTransmit_send_message(SoftwareSerial sw_serial, MidiMessage msg) {
	switch (msg.type) {
		case MIDI_MESSAGE_TYPE_TIMING_CLOCK:
			SoftwareSerial_write(sw_serial, 0xF8);

		case MIDI_MESSAGE_TYPE_NOTE_ON:
			SoftwareSerial_write(sw_serial, MIDI_NOTE_ON(msg.data.note_on.channel));
			SoftwareSerial_write(sw_serial, msg.data.note_on.note_number);
			SoftwareSerial_write(sw_serial, msg.data.note_on.velocity);
			break;

		case MIDI_MESSAGE_TYPE_NOTE_OFF:
			SoftwareSerial_write(sw_serial, MIDI_NOTE_OFF(msg.data.note_off.channel));
			SoftwareSerial_write(sw_serial, msg.data.note_off.note_number);
			SoftwareSerial_write(sw_serial, 0); // velocity
			break;
	}
}
