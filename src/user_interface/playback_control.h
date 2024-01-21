// FIXME: rename this whole file to playback_ui.h

#ifndef PLAYBACK_CONTROL_H
#define PLAYBACK_CONTROL_H

#include "sequencer/beat_clock.h"
#include "user_interface/ui_devices.h"

typedef struct {
	bool start_playback;
	bool stop_playback;
	int8_t tempo_diff;
} playback_control_events_t;

playback_control_events_t playback_control_update(const ui_devices_input_t* input, const beat_clock_t* beat_clock);

#endif /* PLAYBACK_CONTROL_H */
