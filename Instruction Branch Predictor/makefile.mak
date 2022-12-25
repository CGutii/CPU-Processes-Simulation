#compiler
CC=gcc

#buildtarget
TARGET=SIM
FILE=Project1CacheSim

all:$(TARGET)

$(TARGET):$(FILE).C
	$(CC) -o $(TARGET) $(FILE).C

clean:
$(RM) $(TARGET)	