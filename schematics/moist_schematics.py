from skidl import *

# Set the part library (KiCad libs assumed)
lib_search_paths[KICAD] = ['/usr/share/kicad/library', '/home/olga/.local/share/kicad/8.0/3rdparty/library/com_github_espressif_kicad-libraries']  # Adjust if needed

# Create ESP32 component (simplified example, or custom)
esp32 = Part('MCU_Espressif', 'ESP32-S2', footprint='Module:ESP32-S2')  # You may need to define this in your KiCad lib
#esp32 = Part('MCU_Module', 'ESP32', footprint='Module:ESP32')

# Define parts
led = Part('Device', 'LED', footprint='LED_SMD:LED_0805')
resistor = Part('Device', 'R', value='330', footprint='Resistor_SMD:R_0805')

# Define nets
vcc = Net('VCC')
gnd = Net('GND')
gpio = Net('GPIO2')

# Connect power
esp32['3V3'] += vcc
esp32['GND'] += gnd
led[2] += gnd

# Connect GPIO to resistor and LED
esp32['IO2'] += gpio
gpio += resistor[1]
resistor[2] += led[1]

# Generate the netlist
generate_netlist()
