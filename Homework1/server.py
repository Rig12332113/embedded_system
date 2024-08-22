import socket
import numpy as np
import json
import time
import random
import matplotlib.pyplot as plot
import matplotlib.animation as animation

HOST = '192.168.50.33' # IP address
PORT = 12001 # Port to listen on (use ports > 1023)

temps = []
humidities = []
pressures = []
magnetos = []
gyros = []
accs = []
t = 1
fig = plot.subplots(figsize=(8,6))

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	s.bind((HOST, PORT))
	s.listen()
	conn, addr = s.accept()
	with conn:
		print('Connected by', addr)
		print(conn)
		while True:
			data = conn.recv(2048).decode('utf-8')
			print('Received from socket server : ', data)
			'''
			data = data.split()
			temp, humidity, pressure = data[0:3]
			magneto = data[3:6]
			gyro = data[6:9]
			acc = data[9:12]

			temps.append(temp)
			humidities.append(humidity)
			pressures.append(pressure)
			magnetos.append(magneto)
			gyros.append(gyro)
			accs.append(acc)

			print(f'temp: {temp}, humidity: {humidity}, pressure: {pressure}, magneto: {magneto}, gyro: {gyro}, acc: {acc}') 
			'''
			if (data.count('{') != 1):
				# Incomplete data are received. 
				choose = 0
				buffer_data = data.split('}')
				while buffer_data[choose][0] != '{':
					choose += 1
				data = buffer_data[choose] + '}'
			obj = json.loads(data)
			t = obj['s']
			
			plot.scatter(t, obj['x'], c='blue')
			plot.scatter(t, obj['y'], c='red') 
			plot.scatter(t, obj['z'], c='black') 
			plot.xlabel("sample num")
			plot.pause(0.0001)