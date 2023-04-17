build: main.o ./page-migration/page-migration.o ./memory/memory.o
	g++ main.o ./page-migration/page-migration.o ./memory/memory.o -o simulator

main.o: main.cpp ./page-migration/page-migration.o ./memory/memory.o
	g++ -c main.cpp -o main.o

page-migration.o: ./page-migration/page-migration.cpp ./memory/memory.o
	g++ -c ./page-migration/page-migration.cpp -o ./page-migration/page-migration.o

memory.o: ./memory/memory.cpp
	g++ -c ./memory/memory.cpp -o ./memory/memory.o