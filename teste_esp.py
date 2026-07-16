import serial
import time

esp = serial.Serial("/dev/ttyACM0", 115200)

time.sleep(2)

esp.write(b"A\n")

print("enviado")