CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++11 

OBJS =		cancard.o servobase.o canservo.o serialservo.o fcdcdcservo.o algobase.o fcalgosimple.o fccontrollerbase.o controlrunner.o fclinuxrunner.o CanTest.o

LIBS = ./libcontrolcan.so -lpthread

TARGET =	./Debug/CanTest

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS)  $(LIBS) 

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

run:
	sudo $(TARGET)
