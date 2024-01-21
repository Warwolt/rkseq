#include "data/ring_buffer.h"
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
#include "logging.h"
#include "sequencer/beat_clock.h"
#include "user_interface/user_interface.h"
#include "util/bits.h"
#include "util/math.h"
#include "util/microsecond_timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define MIDI_CLOCK_BYTE 0xF8
#define MIDI_START_BYTE 0xFA
#define MIDI_CONTINUE_BYTE 0xFB
#define MIDI_STOP_BYTE 0xFC

/* ----------------------- Interrupt service routines ----------------------- */
static SegmentDisplay* g_segment_display_ptr;

ISR(TIMER0_OVF_vect) {
	Time_timer0_overflow_irq();

	if (g_segment_display_ptr) {
		static uint8_t last_update = 0;
		last_update++;
		if (last_update > 5) {
			last_update = 0;
			SegmentDisplay_update(g_segment_display_ptr); // cycle to next digit
		}
	}
}

static BeatClock* g_beat_clock_ptr;

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

static void update_button_states(Button* buttons, uint8_t num_buttons, const ShiftRegister* shift_reg) {
	bool Button_input[256];
	ShiftRegister_read(shift_reg, Button_input, num_buttons);
	for (uint8_t i = 0; i < num_buttons; i++) {
		Button_update(&buttons[i], Button_input[i], Time_now_ms());
	}
}

// static uint8_t read_midi_byte(void) {
// 	uint8_t midi_byte = 0;
// 	if (SoftwareSerial_available_bytes() > 0) {
// 		SoftwareSerial_read(&midi_byte);
// 	}
// 	return midi_byte;
// }

static void set_playback_tempo(BeatClock* beat_clock, uint8_t bpm) {
	beat_clock->tempo_bpm = bpm;
	Timer1_set_period((1e6 * 60) / (BEAT_CLOCK_SEQUENCER_PPQN * bpm) / TIMER1_USEC_PER_TICK);
}

int main(void) {
	/* Setup */
	// const GpioPin debug_pin1 = GpioPin_init_mode(&PORTC, 3, PIN_MODE_OUTPUT);
	// const GpioPin debug_pin2 = GpioPin_init_mode(&PORTC, 4, PIN_MODE_OUTPUT);
	// const GpioPin pulse_pin = GpioPin_init_mode(&PORTC, 5, PIN_MODE_OUTPUT);
	const GpioPin midi_rx_pin = GpioPin_init(&PORTD, 2);
	const GpioPin midi_tx_pin = GpioPin_init(&PORTD, 3);
	const GpioPin encoder_a_pin = GpioPin_init(&PORTD, 4);
	const GpioPin encoder_b_pin = GpioPin_init(&PORTD, 5);
	const GpioPin display_data_pin = GpioPin_init(&PORTD, 6);
	const GpioPin display_latch_pin = GpioPin_init(&PORTD, 7);
	const GpioPin display_clock_pin = GpioPin_init(&PORTB, 0);
	const GpioPin step_buttons_latch_pin = GpioPin_init(&PORTB, 1);
	// const GpioPin step_leds_latch_pin = GpioPin_init(&PORTB, 2);

	globally_enable_interrupts();
	Timer0_init();
	Timer1_init();
	Timer1_start();
	HardwareSerial_init(9600); // uses PD0 and PD1
	SoftwareSerial_init(31250, midi_rx_pin, midi_tx_pin);
	Spi spi = Spi_init(SPI_DATA_ORDER_MSB_FIRST); // uses PB3, PB4 and PB5

	ShiftRegister step_buttons_shift_reg = ShiftRegister_init(spi, step_buttons_latch_pin);
	// ShiftRegister step_leds_shift_reg = ShiftRegister_init(spi, step_leds_latch_pin);
	RotaryEncoder rotary_encoder = RotaryEncoder_init(encoder_a_pin, encoder_b_pin);
	SegmentDisplay segment_display = SegmentDisplay_init(display_clock_pin, display_latch_pin, display_data_pin);
	Button step_buttons[16];

	BeatClock beat_clock = BeatClock_init(DEFAULT_BPM);
	set_playback_tempo(&beat_clock, DEFAULT_BPM);
	UserInterface user_interface = UserInterface_init();

	g_segment_display_ptr = &segment_display;
	g_beat_clock_ptr = &beat_clock;

	/* Run */
	LOG_INFO("Program Start\n");
	while (true) {
		/* Input */
		update_button_states(step_buttons, 8, &step_buttons_shift_reg);
		const UserInterfaceInput UserInterface_input = {
			.rotary_encoder_diff = RotaryEncoder_read(&rotary_encoder),
		};

		/* Update */
		const UserInterfaceEvents ui_events = UserInterface_update(&user_interface, &UserInterface_input, &beat_clock);
		if (ui_events.new_tempo_bpm) {
			set_playback_tempo(&beat_clock, ui_events.new_tempo_bpm);
		}
		if (ui_events.start_playback) {
			BeatClock_start(&beat_clock);
		}
		if (ui_events.stop_playback) {
			BeatClock_stop(&beat_clock);
		}

		/* Output */
		// Tempo display
		SegmentDisplay_set_char(&segment_display, 0, user_interface.segment_display_chars[0]);
		SegmentDisplay_set_char(&segment_display, 1, user_interface.segment_display_chars[1]);
		SegmentDisplay_set_char(&segment_display, 2, user_interface.segment_display_chars[2]);
		SegmentDisplay_set_char(&segment_display, 3, user_interface.segment_display_chars[3]);
	}
}
