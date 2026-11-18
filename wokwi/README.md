# Run in Wokwi

1. Open https://wokwi.com/projects/new/arduino-uno
2. Replace the contents of the `sketch.ino` tab with [`sketch.ino`](sketch.ino).
3. Replace the contents of the `diagram.json` tab with [`diagram.json`](diagram.json).
4. Press the green **Play** button.

Wokwi has no soil moisture or ball tilt part, so:

- the **knob** (potentiometer) stands in for the moisture sensor: turn it right for wetter soil
- the **slide switch** stands in for the tilt sensor: left is upright, right is tilted

| Knob | Switch | Result |
|---|---|---|
| low | left | Green (safe) |
| high | left | Yellow + beeps (soil soaked) |
| low | right | Yellow + beeps (ground moved) |
| high | right | Red + alarm (danger) |

The Serial Monitor at the bottom shows moisture, tilt and the level. If the switch works the wrong way round, move the black wire from switch pin 1 to pin 3.
