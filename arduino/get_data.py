import serial
import time
print("hello")
ser = serial.Serial('COM8', 9600, timeout=5)
ser.flushInput()
while True:
	try:
		ser.write('H')
		ser_bytes = ser.readline()
		print(ser_bytes)
		if(ser_bytes == '9'):
				break
	except:
		break
ser.close()