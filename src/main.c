#include "data/ring_buffer.h"
#include "debug/logging.h"
#include "hardware/gpio.h"
#include "hardware/hardware_serial.h"
#include "hardware/midi_transmit.h"
#include "hardware/rotary_encoder.h"
#include "hardware/segment_display.h"
#include "hardware/shift_register.h"
#include "hardware/software_serial.h"
#include "hardware/spi.h"
#include "hardware/timer0.h"
#include "hardware/timer1.h"
#include "input/button.h"
#include "input/time.h"
#include "sequencer/beat_clock.h"
#include "sequencer/midi_control.h"
#include "sequencer/step_sequencer.h"
#include "user_interface/user_interface.h"
#include "util/bits.h"
#include "util/math.h"
#include "util/timer.h"
#include "util/unused.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

typedef struct {
	RotaryEncoder rotary_encoder;
	SegmentDisplay segment_display;
} InterfaceDevices;

/* ----------------------- Interrupt service routines ----------------------- */
static SegmentDisplay* g_segment_display_ptr;
static BeatClock* g_beat_clock_ptr;

ISR(TIMER0_OVF_vect) {
	Time_timer0_overflow_irq();

	if (g_segment_display_ptr) {
		static uint8_t last_update = 0;
		last_update++;
		if (last_update > 5) {
			last_update = 0;
			SegmentDisplay_output_next_char(g_segment_display_ptr); // cycle to next digit
		}
	}
}

ISR(TIMER1_COMPA_vect) {
	// FIXME: Move this out into a local function that is used as callback here
	// Motivation: allows us to set everything up in main and make the entire
	// program understandable by reading it starting from main.
	if (g_beat_clock_ptr) {
		BeatClock_on_pulse(g_beat_clock_ptr);
		if (BeatClock_midi_pulse_ready(g_beat_clock_ptr)) {
			SoftwareSerial_write(MIDI_CLOCK_BYTE);
		}
		if (BeatClock_sixteenth_note_ready(g_beat_clock_ptr)) {
			static bool note_on = true;

			const uint8_t channel = 0;
			const uint8_t note = 64;
			const uint8_t velocity = 64;

			if (note_on) {
				MidiTransmit_send_message(MIDI_MESSAGE_NOTE_ON(channel, note, velocity));
			} else {
				// FIXME: remove velocity from off message
				MidiTransmit_send_message(MIDI_MESSAGE_NOTE_OFF(channel, note, 64));
			}

			note_on = !note_on;
		}
	}
}

ISR(PCINT2_vect) {
	SoftwareSerial_pin_change_irq();
}

ISR(USART_RX_vect) {
	HardwareSerial_rx_complete_irq();
}

ISR(USART_UDRE_vect) {
	HardwareSerial_tx_udr_empty_irq();
}

/* ------------------------------ Main Program ------------------------------ */
static void globally_enable_interrupts(void) {
	sei();
}

static UserInterfaceInput read_ui_input(InterfaceDevices* interface_devices) {
	return (UserInterfaceInput) {
		.rotary_encoder_diff = RotaryEncoder_read(&interface_devices->rotary_encoder),
	};
}

#define MICROSECONDS_PER_SECOND 1e6
#define BPM_PER_HZ 60

static void set_playback_tempo(BeatClock* beat_clock, Timer1 timer1, uint16_t new_tempo_deci_bpm) {
	beat_clock->tempo_deci_bpm = new_tempo_deci_bpm;
	const uint32_t usec_per_pulse = MICROSECONDS_PER_SECOND * BPM_PER_HZ / ((BEAT_CLOCK_SEQUENCER_PPQN * (new_tempo_deci_bpm / 10)));
	const uint16_t ticks_per_pulse = usec_per_pulse / TIMER1_USEC_PER_TICK;
	Timer1_set_period(timer1, ticks_per_pulse);
}

static void start_playback(BeatClock* beat_clock, Timer1 timer1) {
	BeatClock_start(beat_clock);
	Timer1_start(timer1);
}

static void stop_playback(BeatClock* beat_clock, Timer1 timer1) {
	BeatClock_stop(beat_clock);
	Timer1_stop(timer1);
}

static void update_segment_display(SegmentDisplay* segment_display, const UserInterface* user_interface) {
	for (int i = 0; i < 4; i++) {
		SegmentDisplay_set_char(segment_display, i, user_interface->segment_display_chars[i]);
		SegmentDisplay_set_period(segment_display, i, user_interface->segment_display_period_enabled[i]);
	}
}

static uint8_t maybe_read_midi_byte(void) {
	uint8_t byte = 0;
	if (SoftwareSerial_available_bytes() > 0) {
		SoftwareSerial_read(&byte);
	}
	return byte;
}

static void handle_ui_events(Timer1 timer1, StepSequencer* step_sequencer, const UserInterfaceEvents* ui_events) {
	if (ui_events->new_tempo_deci_bpm) {
		set_playback_tempo(&step_sequencer->beat_clock, timer1, ui_events->new_tempo_deci_bpm);
	}
	if (ui_events->start_playback) {
		start_playback(&step_sequencer->beat_clock, timer1);
	}
	if (ui_events->stop_playback) {
		stop_playback(&step_sequencer->beat_clock, timer1);
	}
}

static void handle_midi_control_events(Timer1 timer1, StepSequencer* step_sequencer, const MidiControlEvents* midi_events) {
	if (midi_events->switch_to_external_clock) {
		if (step_sequencer->beat_clock.source == BEAT_CLOCK_SOURCE_INTERNAL) {
			LOG_INFO("Switched to external beat clock\n");
			step_sequencer->beat_clock.source = BEAT_CLOCK_SOURCE_EXTERNAL;
			Timer1_stop(timer1);
		}
	}

	if (midi_events->switch_to_internal_clock) {
		if (step_sequencer->beat_clock.source == BEAT_CLOCK_SOURCE_EXTERNAL) {
			LOG_INFO("Switched to internal beat clock\n");
			step_sequencer->beat_clock.source = BEAT_CLOCK_SOURCE_INTERNAL;
			Timer1_start(timer1);
		}
	}
}

int main(void) {
	/* Setup */
	const GpioPin midi_rx_pin = GpioPin_init(&PORTD, 2);
	const GpioPin midi_tx_pin = GpioPin_init(&PORTD, 3);
	const GpioPin encoder_a_pin = GpioPin_init(&PORTD, 4);
	const GpioPin encoder_b_pin = GpioPin_init(&PORTD, 5);
	const GpioPin display_data_pin = GpioPin_init(&PORTD, 6);
	const GpioPin display_latch_pin = GpioPin_init(&PORTD, 7);
	const GpioPin display_clock_pin = GpioPin_init(&PORTB, 0);
	const GpioPin step_buttons_latch_pin = GpioPin_init(&PORTB, 1);
	const GpioPin step_leds_latch_pin = GpioPin_init(&PORTB, 2);

	globally_enable_interrupts();
	Timer0_init();
	Timer1 timer1 = Timer1_init();
	HardwareSerial_init(9600); // uses PD0 and PD1
	SoftwareSerial_init(31250, midi_rx_pin, midi_tx_pin);
	Spi spi = Spi_init(SPI_DATA_ORDER_MSB_FIRST); // uses PB3, PB4 and PB5

	ShiftRegister step_buttons_shift_reg = ShiftRegister_init(spi, step_buttons_latch_pin);
	ShiftRegister step_leds_shift_reg = ShiftRegister_init(spi, step_leds_latch_pin);
	UNUSED(step_buttons_shift_reg);
	UNUSED(step_leds_shift_reg);

	InterfaceDevices interface_devices = {
		.rotary_encoder = RotaryEncoder_init(encoder_a_pin, encoder_b_pin),
		.segment_display = SegmentDisplay_init(display_clock_pin, display_latch_pin, display_data_pin),
	};
	StepSequencer step_sequencer = StepSequencer_init();
	MidiControl midi_control = MidiControl_init();
	UserInterface user_interface = UserInterface_init();

	// Setup pointers for interrupts
	g_segment_display_ptr = &interface_devices.segment_display;
	g_beat_clock_ptr = &step_sequencer.beat_clock;

	/* Run */
	LOG_INFO("Program Start\n");
	set_playback_tempo(&step_sequencer.beat_clock, timer1, DEFAULT_TEMPO); // set initial tempo
	start_playback(&step_sequencer.beat_clock, timer1); // HACK, start playback immediately
	while (true) {
		/* User Interface */
		const UserInterfaceInput ui_input = read_ui_input(&interface_devices);
		const UserInterfaceEvents ui_events = UserInterface_update(&user_interface, &ui_input, &step_sequencer);
		handle_ui_events(timer1, &step_sequencer, &ui_events);

		/* MIDI Control */
		const uint8_t midi_byte = maybe_read_midi_byte();
		const MidiControlEvents midi_events = MidiControl_update(&midi_control, midi_byte);
		handle_midi_control_events(timer1, &step_sequencer, &midi_events);

		/* Interface Devices */
		update_segment_display(&interface_devices.segment_display, &user_interface);
	}
}
