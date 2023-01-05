CXX = g++
CFLAGS = -std=c++14 -O2 -Wall -g 

TARGET = DK_server
OBJS = ./log/*.cc ./pool/*.cc ./timer/*.cc \
       ./http/*.cc ./server/*.cc \
       ./buffer/*.cc ./main.cc 

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o ./$(TARGET)  -lpthread -L/usr/lib64/mysql -lmysqlclient

clean:
	rm -rf $(TARGET)
