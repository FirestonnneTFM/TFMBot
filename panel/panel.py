#!/usr/bin/env python

import socket
import time

def block_recv(sock, n):
	data = b''
	while len(data) < n:
		data += sock.recv(n)
	return data

def do_send(sock):
	s = input('> ').strip()
	s_len = len(s)
	if s_len < 1:
		return False
	cmd = None
	msg = None
	if s[0] == '/':
		if s_len < 4:
			print('Command is too short to be valid')
			return False
		elif s_len == 5:
			msg = ''
		else:
			msg = s[5:]
		cmd = s[1:4]
	else:
		cmd = 'say'
		msg = s
	sock.send(bytes(cmd, 'ascii'))
	sock.send(len(msg).to_bytes(2, byteorder='little'))
	sock.send(bytes(msg, 'ascii'))
	return True

def do_recv(sock):
	bye = block_recv(sock, 1)[0] != 0
	data_len = int.from_bytes(block_recv(sock, 2), byteorder='little')
	if data_len > 0:
		print(str(block_recv(sock, data_len), 'ascii'))
	if bye:
		raise BrokenPipeError

def main():
	print('Connecting...')
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.bind(('127.0.0.1', 7766))
	sock.listen(1);
	conn, addr = sock.accept()
	print('Connected ' + addr[0] + ':' + str(addr[1]))
	while True:
		if do_send(conn):
			do_recv(conn)
while True:
	try:
		main()
	except BrokenPipeError:
		print('Connection interrupted')
		time.sleep(1)
	except OSError as e:
		print(e)
		time.sleep(3)
	except KeyboardInterrupt:
		print()
		print('Keyboard interrupt')
		break

