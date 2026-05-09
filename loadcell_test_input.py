import serial
import time

#Initialize Arduino Communication
arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(2)

command_map = {"raw": "R", "ripe": "P", "decay": "D"}

while True:
	label = input("Enter 'raw', 'ripe', or 'decay': ").strip().lower()
	
	if label == "exit":
		print("Exit.")
		break
		
	if label in command_map:
		command = command_map[label]
		arduino.reset_input_buffer()
		arduino.write(command.encode())
		print(f"Send to Arduino: {command}")
		
		time.sleep(0.5)
		while arduino.in_waiting:
			response = arduino.readline().decode().strip()
			if response:
				print(f"Arduino Response: {response}")
	else:
		print("invalid")
