# CAN Bus Loopback Test

A diagnostic Arduino sketch for testing MCP25625 CAN Bus shields using loopback mode. This project helps verify that your CAN hardware is functioning correctly before connecting to a real CAN network.

This is the official repository for the [Arduino Nano CAN Bus Shield](https://tinymovr.com/products/arduino).

## Features

- Hardware initialization and power management for MCP25625-based shields
- Loopback mode for self-testing without external CAN devices
- Detailed serial output for debugging
- Sends and receives test messages to verify full CAN stack operation

## Hardware Requirements

- Arduino board (Uno, Mega, etc.)
- MCP25625 CAN Bus shield with:
  - 16MHz crystal oscillator
  - AP3602 5V boost regulator
  - Standard SPI connection

## 3D Model

A STEP file ([nano-shield.step](nano-shield.step)) is included in this repository for mechanical design and enclosure integration.

## Pin Configuration

| Pin | Function | Description |
|-----|----------|-------------|
| 10  | CS       | SPI Chip Select |
| 2   | INT      | Interrupt pin |
| 5   | RST      | Hardware reset (active low) |
| 9   | STBY     | Standby control (LOW = normal operation) |
| 8   | 5V_SHDN  | 5V boost regulator shutdown (HIGH = enabled) |

## Dependencies

This sketch requires the **autowp-mcp2515** library:

```bash
# Install via Arduino Library Manager
# Search for: "autowp-mcp2515"
```

Or install manually from: https://github.com/autowp/arduino-mcp2515

## Usage

### Testing in Loopback Mode (Default)

1. Upload the sketch to your Arduino
2. Open Serial Monitor at 115200 baud
3. Observe the initialization sequence and test messages
4. You should see messages being sent and received successfully

Expected output:
```
Starting CAN Shield Test...
[1/4] Configuring pins...
[2/4] Resetting MCP25625...
[3/4] Initializing CAN controller...
[4/4] Setting CAN mode...
Setup Complete! Starting test loop...

Loop #1
  -> Sending message (ID: 0x123)... OK
  <- Checking for messages... Received!
     ID: 0x123 | Data (4 bytes): DE AD BE EF
```

### Connecting to a Real CAN Network

1. Connect your CAN shield to a CAN bus (CANH and CANL)
2. Ensure proper termination resistors (120Ω) on the bus
3. In the code, comment out line 78 and uncomment line 86:
   ```cpp
   // mcp.setLoopbackMode();  // Comment this out
   mcp.setNormalMode();       // Uncomment this line
   ```
4. Upload and monitor for real CAN traffic

## CAN Bus Configuration

- **Bitrate**: 500 kbps
- **Crystal**: 16 MHz
- **Mode**: Loopback (for testing) or Normal (for production)

## Troubleshooting

### "ERROR: Failed to set bitrate!"

- Check SPI wiring (MOSI, MISO, SCK, CS)
- Verify CS pin is set correctly (default: pin 10)
- Ensure MCP2515/MCP25625 is properly powered
- Confirm 16MHz crystal oscillator is present

### No Messages Received in Loopback Mode

- Check that loopback mode is enabled
- Verify the MCP2515 initialized successfully
- Try increasing the delay between send and receive

### Hardware Not Responding

- Verify 5V boost regulator is enabled (PIN_5V_SHDN = HIGH)
- Check that standby mode is disabled (PIN_CAN_STBY = LOW)
- Try the hardware reset sequence again

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.
