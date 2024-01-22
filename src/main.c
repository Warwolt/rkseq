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

typedef struct {
	BeatClock beat_clock;
} StepSequencer;

StepSequencer StepSequencer_init(void) {
	return (StepSequencer) {
		.beat_clock = BeatClock_init(DEFAULT_BPM),
	};
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
	if (g_beat_clock_ptr && g_beat_clock_ptr->source == BEAT_CLOCK_SOURCE_INTERNAL) {
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

static void handle_ui_events(StepSequencer* step_sequencer, Timer1 timer1, UserInterfaceEvents ui_events) {
	if (ui_events.new_tempo_bpm) {
		set_playback_tempo(&step_sequencer->beat_clock, timer1, ui_events.new_tempo_bpm);
	}
	if (ui_events.start_playback) {
		start_playback(&step_sequencer->beat_clock, timer1);
	}
	if (ui_events.stop_playback) {
		stop_playback(&step_sequencer->beat_clock, timer1);
	}
}

static void update_segment_display(SegmentDisplay* segment_display, char* chars) {
	for (int i = 0; i < 4; i++) {
		SegmentDisplay_set_char(segment_display, i, chars[i]);
	}
}

static uint8_t read_midi_byte(void) {
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
	UserInterface user_interface = UserInterface_init();

	// Setup pointers for interrupts
	g_segment_display_ptr = &interface_devices.segment_display;
	g_beat_clock_ptr = &step_sequencer.beat_clock;

	MillisecondTimer midi_clock_rx_timer = MillisecondTimer_init(1000);

	/* Run */
	LOG_INFO("Program Start\n");
	set_playback_tempo(&step_sequencer.beat_clock, timer1, DEFAULT_BPM);
	start_playback(&step_sequencer.beat_clock, timer1);
	while (true) {
		const uint8_t midi_byte = read_midi_byte();
		const UserInterfaceInput ui_input = read_ui_input(&interface_devices);

		const UserInterfaceEvents ui_events = UserInterface_update(&user_interface, &ui_input, &step_sequencer.beat_clock);
		handle_ui_events(&step_sequencer, timer1, ui_events);

		switch (step_sequencer.beat_clock.source) {
			case BEAT_CLOCK_SOURCE_INTERNAL:
				if (midi_byte == MIDI_CLOCK_BYTE) {
					LOG_INFO("Switched to internal clock");
					step_sequencer.beat_clock.source = BEAT_CLOCK_SOURCE_EXTERNAL;
					MillisecondTimer_reset(&midi_clock_rx_timer);
				}
				break;
			case BEAT_CLOCK_SOURCE_EXTERNAL:
				if (MillisecondTimer_elapsed(&midi_clock_rx_timer)) {
					LOG_INFO("Switched to external clock");
					step_sequencer.beat_clock.source = BEAT_CLOCK_SOURCE_INTERNAL;
				}
				break;
		}

		update_segment_display(&interface_devices.segment_display, user_interface.segment_display_chars);
	}
}
