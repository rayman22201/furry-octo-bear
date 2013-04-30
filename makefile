all: server client service_f service_g

server: 
	gcc ./server.c -o ../server.o

client: 
	gcc ./client.c -o ../client.o
  
service_f: 
	gcc ./services/service_f.c -o ../services/f.service

service_g: 
	gcc ./services/service_g.c -o ../services/g.service

clean: clean_root	clean_services

clean_root: 
	rm -rf ../*o

clean_services: 
	rm -rf ../services/*service