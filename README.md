# Vehicle Security and Control System

An Arduino-based embedded system that simulates a car's safety and control electronics: seatbelt interlock, automatic headlights, engine temperature monitoring, and fuel level display on an LCD.

## Features

- **Seatbelt interlock** — engine start is blocked (with buzzer/LED warning) until the seatbelt switch is engaged
- **Automatic headlights** — an LDR (light sensor) triggers the headlight LED automatically in low-light conditions
- **Engine temperature monitoring** — an LM35 sensor reads temperature and converts the ADC value to degrees Celsius
- **Fuel level gauge** — a potentiometer simulates the fuel tank sensor; level is shown on the LCD for a few seconds after the door is closed
- **Door / ignition logic** — door switch and start button gate the engine and climate motor relays
- **LCD status display** and buzzer/RGB LED alerts for warnings (low fuel, seatbelt not fastened, etc.)

## Hardware

- Arduino (Uno/Mega-compatible)
- 16x2 LCD (via `LiquidCrystal` library, parallel interface)
- LDR (photoresistor) — light sensing
- LM35 — analog temperature sensor
- Potentiometer — simulated fuel level sensor
- Push buttons — engine start, seatbelt, door switch
- LEDs (headlight, seatbelt warning, low fuel, RGB status) + buzzer
- Relays/transistors driving motor and climate-control outputs

## Project Structure

```text
vehicle-security-and-control-system/
├── security-control-system.ino   # Main Arduino sketch (setup/loop, sensor + LCD logic)
└── security-and-control-system.pdsprj   # Proteus simulation project (circuit schematic)
```

The compiled `.hex` output is no longer tracked in git — it's a build artifact and is regenerated whenever the sketch is compiled.

## Getting Started

1. Open `security-and-control-system.pdsprj` in [Proteus](https://www.labcenter.com/) to view/simulate the full circuit, **or** wire the components listed above to an Arduino directly.
2. Open `security-control-system.ino` in the Arduino IDE (or PlatformIO).
3. Install the `LiquidCrystal` library (bundled with the Arduino IDE by default).
4. Select your board/port and upload.

## License

No license specified yet.
