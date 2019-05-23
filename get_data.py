import serial
import time
ser = serial.Serial('COM3', 9600)
ser.flushInput()
ser.write('H')
print('hello')
while ser.readable():
	try:
		ser_bytes = ser.readline()
		print(ser_bytes)
		if ser_bytes == '9\r\n':
			break
	except:
		break
ser.close()
