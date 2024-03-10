#include "data/ring_buffer.h"
#include "data/serialize.h"
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
	// Input
	RotaryEncoder rotary_encoder;
	int8_t rotary_encoder_diff;
	ShiftRegister step_buttons_shift_reg;
	Button step_buttons[16];
	Button control_buttons[8];
	// Output
	SegmentDisplay segment_display;
	ShiftRegister step_leds_shift_reg;
} UserInterfaceDevices;

typedef struct {
	uint8_t last_update;
	Timer0* timer0;
	SegmentDisplay* segment_display;
} OnTimeTickContext;

typedef struct {
	bool sent_note_on_prev_step;
	SoftwareSerial* sw_serial;
	StepSequencer* step_sequencer;
} OnTempoTickContext;

/* ----------------------- Interrupt service routines ----------------------- */
static OnTimeTickContext g_timer0_ovf_callback_context;
static void (*g_timer0_ovf_callback)(OnTimeTickContext* context);
ISR(TIMER0_OVF_vect) {
	if (g_timer0_ovf_callback) {
		g_timer0_ovf_callback(&g_timer0_ovf_callback_context);
	}
}

static OnTempoTickContext g_timer1_compa_callback_context;
static void (*g_timer1_compa_callback)(OnTempoTickContext* context);
ISR(TIMER1_COMPA_vect) {
	if (g_timer1_compa_callback) {
		g_timer1_compa_callback(&g_timer1_compa_callback_context);
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

static void read_buttons(const ShiftRegister* step_buttons_shift_reg, uint32_t time_now_ms, Button step_buttons[16], Button control_buttons[8]) {
	uint8_t button_state_bytes[3] = { 0 };
	ShiftRegister_read(step_buttons_shift_reg, (uint8_t*)&button_state_bytes, 3);

	for (int i = 0; i < 16; i++) {
		Button_update(&step_buttons[i], (button_state_bytes[i / 8] >> (i % 8)) & 0x1, time_now_ms);
	}
	for (int i = 0; i < 8; i++) {
		Button_update(&control_buttons[i], (button_state_bytes[2] >> (i % 8)) & 0x1, time_now_ms);
	}
}

static void read_user_interface_devices(UserInterfaceDevices* ui_devices, uint32_t time_now_ms) {
	read_buttons(&ui_devices->step_buttons_shift_reg, time_now_ms, ui_devices->step_buttons, ui_devices->control_buttons);
	ui_devices->rotary_encoder_diff = RotaryEncoder_read(&ui_devices->rotary_encoder);
}

static uint8_t read_midi_byte(SoftwareSerial sw_serial) {
	uint8_t byte = 0;
	if (SoftwareSerial_available_bytes(sw_serial) > 0) {
		SoftwareSerial_read(sw_serial, &byte);
	}
	return byte;
}

static UserInterfaceEvents get_ui_events(const UserInterfaceDevices* ui_devices) {
	UserInterfaceEvents ui_events;
	ui_events.rotary_encoder_diff = ui_devices->rotary_encoder_diff;
	for (int i = 0; i < 16; i++) {
		ui_events.step_button_pressed[i] = Button_just_pressed(&ui_devices->step_buttons[i]);
	}
	for (int i = 0; i < 8; i++) {
		ui_events.control_button_pressed[i] = Button_just_pressed(&ui_devices->control_buttons[i]);
	}
	return ui_events;
}

#define MICROSECONDS_PER_SECOND 1e6
#define BPM_PER_HZ 60

static void set_playback_tempo(BeatClock* beat_clock, Timer1 timer1, uint16_t set_new_tempo_deci_bpm) {
	beat_clock->tempo_deci_bpm = set_new_tempo_deci_bpm;
	const uint32_t usec_per_pulse = MICROSECONDS_PER_SECOND * BPM_PER_HZ / ((BEAT_CLOCK_SEQUENCER_PPQN * (set_new_tempo_deci_bpm / 10)));
	const uint16_t ticks_per_pulse = usec_per_pulse / TIMER1_USEC_PER_TICK;
	Timer1_set_period(timer1, ticks_per_pulse);
}

static void start_playback(StepSequencer* step_sequencer, Timer1 timer1) {
	step_sequencer->playback_is_active = true;
	BeatClock_start(&step_sequencer->beat_clock);
	Timer1_start(timer1);
}

static void stop_playback(StepSequencer* step_sequencer, Timer1 timer1) {
	step_sequencer->playback_is_active = false;
	step_sequencer->step_index = 0;
	BeatClock_stop(&step_sequencer->beat_clock);
	Timer1_stop(timer1);
}

static void write_step_leds(const ShiftRegister* step_leds_shift_reg, const bool step_leds[16]) {
	uint8_t led_state_bytes[2] = { 0 };
	Serialize_pack_bits_into_bytes(step_leds, 16, led_state_bytes, 2, BIT_ORDERING_LSB_FIRST);
	ShiftRegister_write(step_leds_shift_reg, led_state_bytes, 2);
}

static void write_segment_display(SegmentDisplay* segment_display, const char segment_display_chars[4], const bool segment_display_period_enabled[4]) {
	for (int i = 0; i < 4; i++) {
		SegmentDisplay_set_char(segment_display, i, segment_display_chars[i]);
		SegmentDisplay_set_period(segment_display, i, segment_display_period_enabled[i]);
	}
}

static void write_user_interface_devices(UserInterfaceDevices* ui_devices, const UserInterface* user_interface) {
	write_segment_display(&ui_devices->segment_display, user_interface->segment_display_chars, user_interface->segment_display_period_enabled);
	write_step_leds(&ui_devices->step_leds_shift_reg, user_interface->step_leds);
}

static void execute_ui_commands(Timer1 timer1, StepSequencer* step_sequencer, const UserInterfaceCommands* commands) {
	if (commands->set_new_tempo_deci_bpm) {
		set_playback_tempo(&step_sequencer->beat_clock, timer1, commands->set_new_tempo_deci_bpm);
	}
	if (commands->start_playback) {
		start_playback(step_sequencer, timer1);
	}
	if (commands->stop_playback) {
		stop_playback(step_sequencer, timer1);
	}
	for (int i = 0; i < 16; i++) {
		step_sequencer->step_pattern[i] = commands->new_step_pattern[i];
	}
}

static void execute_midi_control_commands(Timer1 timer1, StepSequencer* step_sequencer, const MidiControlCommands* commands) {
	if (commands->switch_to_external_clock) {
		if (step_sequencer->beat_clock.source == BEAT_CLOCK_SOURCE_INTERNAL) {
			LOG_INFO("Switched to external beat clock\n");
			step_sequencer->beat_clock.source = BEAT_CLOCK_SOURCE_EXTERNAL;
			Timer1_stop(timer1);
		}
	}

	if (commands->switch_to_internal_clock) {
		if (step_sequencer->beat_clock.source == BEAT_CLOCK_SOURCE_EXTERNAL) {
			LOG_INFO("Switched to internal beat clock\n");
			step_sequencer->beat_clock.source = BEAT_CLOCK_SOURCE_INTERNAL;
			Timer1_start(timer1);
		}
	}
}

void on_time_tick(OnTimeTickContext* ctx) {
	if (ctx->timer0) {
		Time_on_timer0_overflow(*ctx->timer0);
	}

	if (ctx->segment_display) {
		ctx->last_update++;
		if (ctx->last_update > 1) {
			ctx->last_update = 0;
			SegmentDisplay_output_next_char(ctx->segment_display);
		}
	}
}

void on_tempo_tick(OnTempoTickContext* ctx) {
	if (ctx->step_sequencer && ctx->sw_serial) {
		StepSequencer* step_sequencer = ctx->step_sequencer;
		SoftwareSerial* sw_serial = ctx->sw_serial;
		BeatClock_count_pulse(&step_sequencer->beat_clock);

		if (BeatClock_midi_pulse_ready(&step_sequencer->beat_clock)) {
			MidiTransmit_send_message(*sw_serial, MIDI_MESSAGE_TIMING_CLOCK);
		}

		if (BeatClock_sixteenth_note_ready(&step_sequencer->beat_clock)) {
			const uint8_t channel = 0;
			const uint8_t note = 64;
			const uint8_t velocity = 64;

			// send note off for previous note
			if (ctx->sent_note_on_prev_step) {
				MidiTransmit_send_message(*sw_serial, MIDI_MESSAGE_NOTE_OFF(channel, note));
			}

			// if note active, send note on
			if (step_sequencer->step_pattern[step_sequencer->step_index]) {
				MidiTransmit_send_message(*sw_serial, MIDI_MESSAGE_NOTE_ON(channel, note, velocity));
				ctx->sent_note_on_prev_step = true;
			} else {
				ctx->sent_note_on_prev_step = false;
			}

			// step to next note
			step_sequencer->step_index = (step_sequencer->step_index + 1) % 16;
		}
	}
}

int main(void) {
	globally_enable_interrupts();

	/* Pins */
	const GpioPin midi_rx_pin = GpioPin_init(&PORTD, 2);
	const GpioPin midi_tx_pin = GpioPin_init(&PORTD, 3);
	const GpioPin encoder_a_pin = GpioPin_init(&PORTD, 4);
	const GpioPin encoder_b_pin = GpioPin_init(&PORTD, 5);
	const GpioPin display_data_pin = GpioPin_init(&PORTD, 6);
	const GpioPin display_latch_pin = GpioPin_init(&PORTD, 7);
	const GpioPin display_clock_pin = GpioPin_init(&PORTB, 0);
	const GpioPin step_buttons_latch_pin = GpioPin_init(&PORTB, 1);
	const GpioPin step_leds_latch_pin = GpioPin_init(&PORTB, 2);

	/* Hardware */
	Timer0 timer0 = Timer0_init();
	Timer1 timer1 = Timer1_init();
	HardwareSerial_init(9600); // uses PD0 and PD1
	SoftwareSerial sw_serial = SoftwareSerial_init(31250, midi_rx_pin, midi_tx_pin);
	Spi spi = Spi_init(SPI_DATA_ORDER_MSB_FIRST); // uses PB3 (MOSI), PB4 (MISO), and PB5 (SCK)
	UserInterfaceDevices ui_devices = {
		// Input
		.rotary_encoder = RotaryEncoder_init(encoder_a_pin, encoder_b_pin),
		.step_buttons_shift_reg = ShiftRegister_init(spi, step_buttons_latch_pin),
		.step_buttons = { 0 },
		.control_buttons = { 0 },
		// Output
		.step_leds_shift_reg = ShiftRegister_init(spi, step_leds_latch_pin),
		.segment_display = SegmentDisplay_init(display_clock_pin, display_latch_pin, display_data_pin),
	};

	/* Program state */
	StepSequencer step_sequencer = StepSequencer_init();
	MidiControl midi_control = MidiControl_init(timer0);
	UserInterface user_interface = UserInterface_init();

	/* Setup timer based interrupts */
	{
		g_timer0_ovf_callback_context = (OnTimeTickContext) {
			.last_update = 0,
			.timer0 = &timer0,
			.segment_display = &ui_devices.segment_display,
		};
		g_timer0_ovf_callback = &on_time_tick;

		g_timer1_compa_callback_context = (OnTempoTickContext) {
			.sent_note_on_prev_step = false,
			.step_sequencer = &step_sequencer,
			.sw_serial = &sw_serial,
		};
		g_timer1_compa_callback = &on_tempo_tick;
	}

	/* Run */
	LOG_INFO("Program Start\n");

	set_playback_tempo(&step_sequencer.beat_clock, timer1, DEFAULT_TEMPO);
	start_playback(&step_sequencer, timer1); // HACK, start playback immediately
	while (true) {
		/* Input */
		const uint32_t time_now_ms = Time_now_ms(timer0);
		const uint8_t midi_byte = read_midi_byte(sw_serial);
		read_user_interface_devices(&ui_devices, time_now_ms);

		// TODO move into UI
		if (Button_just_pressed(&ui_devices.control_buttons[0])) {
			start_playback(&step_sequencer, timer1);
		}
		if (Button_just_pressed(&ui_devices.control_buttons[1])) {
			stop_playback(&step_sequencer, timer1);
		}

		/* Update User Interface */
		const UserInterfaceEvents ui_events = get_ui_events(&ui_devices);
		const UserInterfaceCommands ui_cmds = UserInterface_update(&user_interface, &ui_events, &step_sequencer);
		execute_ui_commands(timer1, &step_sequencer, &ui_cmds);

		/* Update MIDI Control */
		const MidiControlCommands midi_cmds = MidiControl_update(&midi_control, midi_byte);
		execute_midi_control_commands(timer1, &step_sequencer, &midi_cmds);

		/* Output */
		write_user_interface_devices(&ui_devices, &user_interface);
	}
}
