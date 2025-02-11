ISR(TIMER1_COMPA_vect) {
    if (state == State::SETPOSITION) {
        reloadTriggered = true; // Set flag to lock screen
    }
}