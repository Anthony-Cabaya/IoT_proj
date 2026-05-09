import time
import serial
import RPi.GPIO as GPIO
from hx711 import HX711

#Initialize Arduino Communication
arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=1)

DOUT_PIN = 5
SCK_PIN = 6

scale = HX711(DOUT_PIN, SCK_PIN)
scale.set_reading_format("MSB", "MSB")
scale.set_reference_unit(1000)
scale.reset()
scale.tare()

weight_threshold = 100.0  # Weight threshold in grams
load_cell_interval = 0.1  # Load cell reading interval in seconds
weight_hold_time = 3  # Time in seconds for weight to stay above threshold

previousMillisLoadCell = time.time()
weightAboveThresholdTime = 0
weightExceededThreshold = False

try:
    while True:
        currentMillis = time.time()
        
        if currentMillis - previousMillisLoadCell >= load_cell_interval:
            previousMillisLoadCell = currentMillis

            weights = [scale.get_weight(5) for _ in range(3)]  # Get weight reading in grams
            weight = round(sum(weights) / len(weights), 2)

            # Print weight in grams
            if weight > 0.05:  # Ignore tiny weight fluctuations
                print(f"Weight (grams): {weight:.2f}")

                # Check if weight has exceeded the threshold
                if weight >= weight_threshold:
                    # If weight exceeds threshold for the first time, start timing
                    if not weightExceededThreshold:
                        weightExceededThreshold = True
                        weightAboveThresholdTime =time.time()
                    if time.time() - weightAboveThresholdTime >= weight_hold_time:
                        arduino.write(b'1\n') # Send '1' as signal to move Loadcell Servo
                        print("Weight exceeded threshold for 3 seconds, Move Servo for Loadcell.")
                        time.sleep(2)
                        
                        # Mango Size
                        if weight <= 249:
                            category = 'S'
                            mango_size = "small"
                        elif 250 <= weight <= 299:
                            category = 'M'
                            mango_size = "medium"
                        else:
                            category = 'L'
                            mango_size = "large"
                        print(f"Size: {mango_size}")
                        arduino.write(category.encode() + b'\n')
                        time.sleep(2)
                else:
                    weightExceededThreshold = False
                    weightAboveThresholdTime = 0
            else:
                scale.tare()  # Reset the scale to zero if the reading is unstable

        time.sleep(0.01)  # Small delay to avoid excessive CPU usage

except KeyboardInterrupt:
    print("Stopping...")
    GPIO.cleanup()
