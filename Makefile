client: bs_client.cc
	g++ -o bs_client bs_client.cc -lboost_system -lboost_thread -lpthread -lncurses -std=c++11
server: bs_server.cc
	g++ -o bs_server bs_server.cc -lboost_system -lboost_thread -lpthread -lncurses -std=c++11
