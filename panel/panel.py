#!/usr/bin/env python

import socket

def block_recv(sock, n):
	data = b''
	while len(data) < n:
		data += sock.recv(n)
	return data

def do_send(sock):
	s = input('> ')
	sock.send(len(s).to_bytes(2, byteorder='little'))
	sock.send(bytes(s, 'utf-8'))

def do_recv(sock):
	data_len = int.from_bytes(block_recv(sock, 2), byteorder='little')
	if data_len > 0:
		print(str(block_recv(sock, data_len), 'ascii'))

def main():
	print('Connecting...')
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.bind(('127.0.0.1', 7766))
	sock.listen(1);
	conn, addr = sock.accept()
	print('Connected ' + addr[0] + ':' + str(addr[1]))
	while True:
		do_send(conn)
		do_recv(conn)
main()
