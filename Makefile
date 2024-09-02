all: serverM.cpp serverA.cpp serverB.cpp client.cpp
	g++ -o serverM serverM.cpp
	g++ -o serverA serverA.cpp
	g++ -o serverB serverB.cpp
	g++ -o client client.cpp

clean:
	rm -rf serverM serverA serverB client

.PHONY: serverM
serverM:
		./serverM

.PHONY: serverA
serverA:
		./serverA

.PHONY: serverB
serverB:
		./serverB

.PHONY: client
client:
		./client


.PHONY: all serverM serverA serverB client
