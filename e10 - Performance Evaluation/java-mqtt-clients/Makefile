TARGETS = \
	MqttFunc.class \
	MqttMovingAverage.class \
	MqttSignalGenerator.class \

CP = -classpath ".;org.eclipse.paho.client.mqttv3-1.2.5.jar;json-20140107.jar"

# https://search.maven.org/search?q=a:org.eclipse.paho.client.mqttv3
# https://repo.eclipse.org/content/repositories/paho-snapshots/org/eclipse/paho/org.eclipse.paho.client.mqttv3/1.1.2-SNAPSHOT/
# https://github.com/jezhumble/javasysmon

all: ${TARGETS}

clean:
	touch ${TARGETS}
	rm    ${TARGETS}

mrproper: clean
	touch d~
	rm    *~

deps:
	wget https://repo1.maven.org/maven2/org/json/json/20140107/json-20140107.jar
	wget http://continuousdelivery.com/downloads/javasysmon/javasysmon-0.3.5.jar


MqttFunc.class: MqttFunc.java
	javac ${CP} MqttFunc.java

MqttFunc: MqttFunc.class
	java ${CP} MqttFunc

MqttMovingAverage.class: MqttMovingAverage.java
	javac ${CP} MqttMovingAverage.java

MqttMovingAverage: MqttMovingAverage.class
	java ${CP} MqttMovingAverage

MqttSignalGenerator.class: MqttSignalGenerator.java
	javac ${CP} MqttSignalGenerator.java

MqttSignalGenerator: MqttSignalGenerator.class
	java ${CP} MqttSignalGenerator

