import socket
import ssl
import sys
import select
import zlib
import time



if(len(sys.argv) < 3):
	print("USAGE: ", sys.argv[0], " HOST:PORT TESTFILE")

ssl_sock = None
context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	s.settimeout(10)
	host,port = sys.argv[1].split(":");
	s.connect((host, int(port)))
	pollerObject = select.poll()
	pollerObject.register(s, select.POLLIN)

	with open(sys.argv[2], 'r') as lines:
		pos = lines.tell()
		l1 = lines.readline()
		lines.seek(pos)

		first_encrypt = 'e' not in l1.split("|")[0]
		if(not first_encrypt):
			ssl_sock = context.wrap_socket(s)
		while(len(pollerObject.poll(100)) > 0):
			if(ssl_sock == None):
				buf = s.recv(8192)
			else:
				buf = ssl_sock.recv(8192)
			if(len(buf.decode().strip()) > 0):
				print(buf.decode().strip())
			else:
				break
		
		for line in lines:
			status, content = line.strip().split("|")
			content = content.strip()
			encrypted = ('e' in status)
			compressed = ('c' in status)
			if encrypted and first_encrypt:
				first_encrypt = False
				ssl_sock = context.wrap_socket(s, server_hostname=host)
			if(compressed):
				content = zlib.compress(content.encode())
			else:
				content = content.encode()
			# print(content.decode())
			if(encrypted):
				ssl_sock.send(content)
			else:
				s.sendall(content)
			time.sleep(1)
			while(len(pollerObject.poll(100)) > 0):
				if(ssl_sock == None):
					buf = s.recv(8192)
				else:
					buf = ssl_sock.recv(8192)
				if(len(buf) > 0):
					if(compressed):
						buf = zlib.decompress(buf)
					print(buf.decode().strip())
				else:
					break
if ssl_sock != None:
	ssl_sock.close()