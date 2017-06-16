main: main.o interpreter.o api.o record_manager.o buffer_manager.o catalog_manager.o index_manager.o basic.o
	g++ -o main -g main.o interpreter.o api.o record_manager.o buffer_manager.o catalog_manager.o index_manager.o basic.o
main.o: main.cc interpreter.h catalog_manager.h basic.h buffer_manager.h exception.h const.h
	g++ -c -g main.cc
interpreter.o: interpreter.cc interpreter.h catalog_manager.h basic.h buffer_manager.h exception.h const.h
	g++ -c -g interpreter.cc
api.o: api.cc api.h buffer_manager.h record_manager.h const.h basic.h catalog_manager.h buffer_manager.h exception.h catalog_manager.h template_function.h
	g++ -c -g api.cc
# record: unit_test_for_record_manager.o record_manager.o buffer_manager.o catalog_manager.o index_manager.o basic.o
# 	g++ -o record -g unit_test_for_record_manager.o record_manager.o buffer_manager.o catalog_manager.o index_manager.o basic.o
# unit_test_for_record_manager.o: ./unit_test/unit_test_for_record_manager.cc record_manager.h
# 	g++ -c -g ./unit_test/unit_test_for_record_manager.cc
record_manager.o: record_manager.cc record_manager.h index_manager.h catalog_manager.h buffer_manager.h bplustree.h exception.h basic.h const.h template_function.h
	g++ -c -g record_manager.cc
buffer_manager.o: buffer_manager.cc buffer_manager.h const.h
	g++ -c -g buffer_manager.cc
catalog_manager.o: catalog_manager.cc catalog_manager.h buffer_manager.h basic.h exception.h const.h
	g++ -c -g catalog_manager.cc
index_manager.o: index_manager.cc index_manager.h basic.h buffer_manager.h bplustree.h const.h
	g++ -c -g index_manager.cc
basic.o: basic.cc basic.h
	g++ -c -g basic.cc



clear:
	rm *.o