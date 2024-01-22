#include "data/ring_buffer.h"
#include "debug/logging.h"
#include "hardware/gpio.h"
#include "hardware/hardware_serial.h"
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

#define MIDI_CLOCK_BYTE 0xF8
#define MIDI_START_BYTE 0xFA
#define MIDI_CONTINUE_BYTE 0xFB
#define MIDI_STOP_BYTE 0xFC

typedef struct {
	RotaryEncoder rotary_encoder;
	SegmentDisplay segment_display;
} InterfaceDevices;

#define MIDI_CONTROL_CLOCK_TIMEOUT_MS 1000

typedef struct {
	bool switch_to_internal_clock;
	bool switch_to_external_clock;
} MidiControlEvents;

typedef struct {
	MillisecondTimer midi_clock_timeout_timer;
} MidiControl;

static MidiControl MidiControl_init(void) {
	return (MidiControl) {
		.midi_clock_timeout_timer = MillisecondTimer_init(MIDI_CONTROL_CLOCK_TIMEOUT_MS),
	};
}

static MidiControlEvents MidiControl_update(MidiControl* midi_control, uint8_t midi_byte) {
	MidiControlEvents events = { 0 };

	if (midi_byte == MIDI_CLOCK_BYTE) {
		events.switch_to_external_clock = true;
		MillisecondTimer_reset(&midi_control->midi_clock_timeout_timer);
	}

	if (MillisecondTimer_elapsed(&midi_control->midi_clock_timeout_timer)) {
		events.switch_to_internal_clock = true;
	}

	return events;
}

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
	if (g_beat_clock_ptr) {
		BeatClock_on_pulse(g_beat_clock_ptr);
		if (BeatClock_midi_pulse_ready(g_beat_clock_ptr)) {
			SoftwareSerial_write(MIDI_CLOCK_BYTE);
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

static void set_playback_tempo(BeatClock* beat_clock, Timer1 timer1, uint8_t bpm) {
	beat_clock->tempo_bpm = bpm;
	Timer1_set_period(timer1, (1e6 * 60) / (BEAT_CLOCK_SEQUENCER_PPQN * bpm) / TIMER1_USEC_PER_TICK);
}

static void start_playback(BeatClock* beat_clock, Timer1 timer1) {
	BeatClock_start(beat_clock);
	Timer1_start(timer1);
}

static void stop_playback(BeatClock* beat_clock, Timer1 timer1) {
	BeatClock_stop(beat_clock);
	Timer1_stop(timer1);
}

static void update_segment_display(SegmentDisplay* segment_display, char* chars) {
	for (int i = 0; i < 4; i++) {
		SegmentDisplay_set_char(segment_display, i, chars[i]);
	}
}

static uint8_t maybe_read_midi_byte(void) {
	uint8_t byte = 0;
	if (SoftwareSerial_available_bytes() > 0) {
		SoftwareSerial_read(&byte);
	}
	return byte;
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
	set_playback_tempo(&step_sequencer.beat_clock, timer1, DEFAULT_BPM); // set initial tempo
	start_playback(&step_sequencer.beat_clock, timer1); // HACK, start playback immediately
	while (true) {
		/* User Interface */
		const UserInterfaceInput ui_input = read_ui_input(&interface_devices);
		const UserInterfaceEvents ui_events = UserInterface_update(&user_interface, &ui_input, &step_sequencer.beat_clock);
		if (ui_events.new_tempo_bpm) {
			set_playback_tempo(&step_sequencer.beat_clock, timer1, ui_events.new_tempo_bpm);
		}
		if (ui_events.start_playback) {
			start_playback(&step_sequencer.beat_clock, timer1);
		}
		if (ui_events.stop_playback) {
			stop_playback(&step_sequencer.beat_clock, timer1);
		}
		update_segment_display(&interface_devices.segment_display, user_interface.segment_display_chars);

		/* MIDI Control */
		const uint8_t midi_byte = maybe_read_midi_byte();
		const MidiControlEvents midi_events = MidiControl_update(&midi_control, midi_byte);
		switch (step_sequencer.beat_clock.source) {
			// Internal -> External
			case BEAT_CLOCK_SOURCE_INTERNAL:
				if (midi_events.switch_to_external_clock) {
					LOG_INFO("Switched to external beat clock\n");
					step_sequencer.beat_clock.source = BEAT_CLOCK_SOURCE_EXTERNAL;
					Timer1_stop(timer1);
				}
				break;

			// External -> Internal
			case BEAT_CLOCK_SOURCE_EXTERNAL:
				if (midi_events.switch_to_internal_clock) {
					LOG_INFO("Switched to internal beat clock\n");
					step_sequencer.beat_clock.source = BEAT_CLOCK_SOURCE_INTERNAL;
					Timer1_start(timer1);
				}
				break;
		}
	}
}
