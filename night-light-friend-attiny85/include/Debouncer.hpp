#pragma once

template<typename GPIOType, typename T>
struct Debouncer {
    Debouncer(GPIOType& _gpio, bool _activeState, T _numDebounceTicks)
        : gpio             { _gpio },
          activeState      { _activeState },
          numDebounceTicks { _numDebounceTicks },
          lastStableState  { !_activeState },
          lastDebounceTick { 0 },
          activeEdgeP      { false },
          inactiveEdgeP    { false } {}

    // Call this regularly (e.g., once per watchdog tick)
    void update(T currentTick) {
        bool currentState = gpio.read();

        if (currentState != lastStableState) {
            // State changed — check if stable long enough
            if ((currentTick - lastDebounceTick) >= numDebounceTicks) {
                lastDebounceTick = currentTick;
                lastStableState = currentState;

                if (currentState == activeState) {
                    activeEdgeP = true;
                } else {
                    inactiveEdgeP = true;
                }
            }
        }
    }

    bool isActiveP() {
        if (activeEdgeP) {
            activeEdgeP = false;
            return true;
        }
        return false;
    }

    bool inInactiveP() {
        if (inactiveEdgeP) {
            inactiveEdgeP = false;
            return true;
        }
        return false;
    }

private:
    GPIOType& gpio;
    const bool activeState;
    const T numDebounceTicks;

    bool lastStableState;
    T lastDebounceTick;

    bool activeEdgeP;
    bool inactiveEdgeP;
};
