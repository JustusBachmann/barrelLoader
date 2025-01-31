# barrelLoader

[Wokwi Project URL](https://wokwi.com/projects/420944776163138561)

### usage
- use `arduino-cli` to build repository
- `arduino-cli core update-index` update index first
- `arduino-cli core install arduino:avr` install arduino board
- `arduino-cli lib install U8g2 AccelStepper` install additional libraries
- `arduino-cli compile --fqbn arduino:avr:mega --output-dir ./build .` rebuild firmware
