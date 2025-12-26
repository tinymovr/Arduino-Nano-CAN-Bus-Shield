#include <SPI.h>
#include <mcp2515.h> // Library: 'autowp-mcp2515'

// --- Pin Definitions (From Schematic) ---
const int PIN_CAN_CS   = 10; // [cite: 146]
const int PIN_CAN_INT  = 2;  // [cite: 174]
const int PIN_CAN_RST  = 5;  // [cite: 26, 171]
const int PIN_CAN_STBY = 9;  // [cite: 79]
const int PIN_5V_SHDN  = 8;  // [cite: 74]

// --- CAN Bus Configuration ---
// Supported bitrates: CAN_5KBPS, CAN_10KBPS, CAN_20KBPS, CAN_31K25BPS, CAN_33KBPS,
//                     CAN_40KBPS, CAN_50KBPS, CAN_80KBPS, CAN_100KBPS, CAN_125KBPS,
//                     CAN_200KBPS, CAN_250KBPS, CAN_500KBPS, CAN_1000KBPS
const CAN_SPEED CAN_BITRATE = CAN_500KBPS;  // Change this to match your CAN network
const CAN_CLOCK CAN_CLOCK_FREQ = MCP_16MHZ;  // Shield uses 16MHz crystal

// --- Serial Configuration ---
const unsigned long SERIAL_TIMEOUT_MS = 3000;  // Timeout for Serial Monitor connection

// Create the MCP2515 instance
MCP2515 mcp(PIN_CAN_CS);

// Create a buffer for holding messages
struct can_frame canMsg;

// Helper function to print bitrate in human-readable format
void printBitrate(CAN_SPEED speed) {
  switch (speed) {
    case CAN_5KBPS:    Serial.print("5 kbps"); break;
    case CAN_10KBPS:   Serial.print("10 kbps"); break;
    case CAN_20KBPS:   Serial.print("20 kbps"); break;
    case CAN_31K25BPS: Serial.print("31.25 kbps"); break;
    case CAN_33KBPS:   Serial.print("33 kbps"); break;
    case CAN_40KBPS:   Serial.print("40 kbps"); break;
    case CAN_50KBPS:   Serial.print("50 kbps"); break;
    case CAN_80KBPS:   Serial.print("80 kbps"); break;
    case CAN_100KBPS:  Serial.print("100 kbps"); break;
    case CAN_125KBPS:  Serial.print("125 kbps"); break;
    case CAN_200KBPS:  Serial.print("200 kbps"); break;
    case CAN_250KBPS:  Serial.print("250 kbps"); break;
    case CAN_500KBPS:  Serial.print("500 kbps"); break;
    case CAN_1000KBPS: Serial.print("1000 kbps"); break;
    default:           Serial.print("Unknown"); break;
  }
}

void setup() {
  Serial.begin(115200);

  // Wait for Serial Monitor with timeout (for compatibility with all Nano boards)
  unsigned long startTime = millis();
  while (!Serial && (millis() - startTime < SERIAL_TIMEOUT_MS));

  Serial.println("Starting CAN Shield Test...");
  Serial.println();

  // --- 1. Hardware Pin Setup ---
  Serial.println("[1/4] Configuring pins...");
  // The MCP25625 and Power Booster need to be enabled manually
  pinMode(PIN_CAN_RST, OUTPUT);
  pinMode(PIN_CAN_STBY, OUTPUT);
  pinMode(PIN_5V_SHDN, OUTPUT);
  Serial.println("  - Pins configured as OUTPUT");

  // Enable the 5V Boost Regulator (AP3602)
  // Schematic has a pull-up (R1) to 3.3V on SHDN, suggesting HIGH = Enable
  digitalWrite(PIN_5V_SHDN, HIGH);
  Serial.println("  - 5V Boost enabled (PIN_5V_SHDN = HIGH)");

  // Disable Standby Mode on the CAN Chip (Low = Normal Operation)
  digitalWrite(PIN_CAN_STBY, LOW);
  Serial.println("  - Standby disabled (PIN_CAN_STBY = LOW)");

  // --- 2. Reset Sequence ---
  Serial.println("[2/4] Resetting MCP25625...");
  // The MCP25625 has an active-low reset pin
  digitalWrite(PIN_CAN_RST, LOW);
  delay(10);
  digitalWrite(PIN_CAN_RST, HIGH);
  delay(10);
  Serial.println("  - Hardware reset complete");

  // --- 3. Initialize CAN Controller ---
  Serial.println("[3/4] Initializing CAN controller...");

  // Explicitly initialize SPI (the library should do this, but let's be sure)
  SPI.begin();
  Serial.println("  - SPI initialized");

  // Software reset of the MCP2515
  Serial.println("  - Sending software reset...");
  mcp.reset();
  Serial.println("  - Software reset complete");

  // Config: Set bitrate and crystal frequency
  Serial.print("  - Setting bitrate (");
  printBitrate(CAN_BITRATE);
  Serial.println(")...");
  if (mcp.setBitrate(CAN_BITRATE, CAN_CLOCK_FREQ) == MCP2515::ERROR_OK) {
    Serial.println("  - Bitrate set successfully!");
  } else {
    Serial.println("  - ERROR: Failed to set bitrate!");
    Serial.println("  - Possible causes:");
    Serial.println("    * SPI connection issue (check wiring)");
    Serial.println("    * Wrong CS pin (currently pin 10)");
    Serial.println("    * MCP2515 not responding");
    while (1); // Stop here if failed
  }

  // --- 4. Set Mode ---
  Serial.println("[4/4] Setting CAN mode...");
  // USE LOOPBACK FOR TESTING WITHOUT WIRES
  // The chip sends messages to itself internally.
  if (mcp.setLoopbackMode() == MCP2515::ERROR_OK) {
    Serial.println("  - Loopback mode enabled (self-test)");
  } else {
    Serial.println("  - ERROR: Failed to set loopback mode!");
    while (1);
  }

  // UNCOMMENT the line below when you connect real wires to another device
  // mcp.setNormalMode();

  Serial.println();
  Serial.println("=============================");
  Serial.println("Setup Complete! Starting test loop...");
  Serial.println("=============================");
  Serial.println();
}

void loop() {
  static unsigned long loopCount = 0;
  loopCount++;

  Serial.print("Loop #");
  Serial.println(loopCount);

  // --- SENDING ---
  struct can_frame outputMsg;
  outputMsg.can_id  = 0x123; // Arbitrary ID
  outputMsg.can_dlc = 4;     // Length (0-8)
  outputMsg.data[0] = 0xDE;
  outputMsg.data[1] = 0xAD;
  outputMsg.data[2] = 0xBE;
  outputMsg.data[3] = 0xEF;

  Serial.print("  -> Sending message (ID: 0x");
  Serial.print(outputMsg.can_id, HEX);
  Serial.print(")... ");

  MCP2515::ERROR sendStatus = mcp.sendMessage(&outputMsg);
  if (sendStatus == MCP2515::ERROR_OK) {
    Serial.println("OK");
  } else {
    Serial.print("FAILED (Error code: ");
    Serial.print(sendStatus);
    Serial.println(")");
  }

  delay(100); // Small delay to allow processing

  // --- RECEIVING ---
  // In Loopback mode, we should receive what we just sent
  Serial.print("  <- Checking for messages... ");
  MCP2515::ERROR readStatus = mcp.readMessage(&canMsg);

  if (readStatus == MCP2515::ERROR_OK) {
    Serial.println("Received!");
    Serial.print("     ID: 0x");
    Serial.print(canMsg.can_id, HEX);
    Serial.print(" | Data (");
    Serial.print(canMsg.can_dlc);
    Serial.print(" bytes): ");

    for (int i = 0; i < canMsg.can_dlc; i++)  {
      if (canMsg.data[i] < 0x10) Serial.print("0"); // Leading zero
      Serial.print(canMsg.data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  } else if (readStatus == MCP2515::ERROR_NOMSG) {
    Serial.println("No message");
  } else {
    Serial.print("ERROR (code: ");
    Serial.print(readStatus);
    Serial.println(")");
  }

  Serial.println("-----------------------------");
  delay(1000); // Send once per second
}