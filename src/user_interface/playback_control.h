#ifndef PLAYBACK_CONTROL_H
#define PLAYBACK_CONTROL_H

#include "sequencer/beat_clock.h"
#include "user_interface/ui_devices.h"

void playback_control_update(ui_devices_t* ui_devices, beat_clock_t* beat_clock);

#endif /* PLAYBACK_CONTROL_H */
