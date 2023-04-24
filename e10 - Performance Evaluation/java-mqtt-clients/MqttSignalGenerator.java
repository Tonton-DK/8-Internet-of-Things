import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

public class MqttSignalGenerator extends Thread {
    static String broker = "tcp://localhost:1883";
    static int qos = 2;
    static String[] topics = {
        "siggen/room1/temp",
        "siggen/room1/rhum",
        "siggen/room2/temp",
        "siggen/room2/rhum",
    };
    static double[][] values = {
        {20.0, 50.0, 19.3, 61.7},
        {20.5, 55.3, 19.7, 59.4},
        {21.0, 63.1, 19.5, 58.8},
        {21.5, 65.6, 20.6, 56.3},
        {22.0, 77.2, 20.4, 63.5},
        {22.5, 76.8, 23.7, 62.2},
        {23.0, 84.9, 22.0, 68.1},
        {24.5, 81.0, 24.2, 73.6},
        {24.3, 89.7, 26.1, 72.9},
        {22.7, 93.3, 25.3, 76.4},
        {19.9, 74.8, 24.8, 78.7},
        {17.3, 59.4, 22.9, 74.3},
    };
    
    private MqttClient client;
    private MemoryPersistence persistence = new MemoryPersistence();
    private String topic;
    private int index;
    private int delay; // in ms
    
    public MqttSignalGenerator (int index, int delay) {
        this.topic = topics[index];
        this.index = index;
        this.delay = delay;
    }
    
    public void run () {
        try {
            // connect
            client = new MqttClient(broker, "java/siggen"+index, persistence);
            MqttConnectOptions connOpts = new MqttConnectOptions();
            connOpts.setCleanSession(true);
            client.connect(connOpts);
            
            int time = 0;
            while (true) {
                double value = values[time%values.length][index];
                String message = "{\"time\":"+time+",\"value\":"+value+"}";
                tx(topic, message.getBytes());
                time += 1;
                Thread.sleep(delay);
            }
        } catch (MqttException | InterruptedException e) {
            System.out.println("Exception caught in MqttSignalGenerator "+index+": "+e);
        }
    }
    
    public void tx (String topic, byte[] payload) throws MqttException {
        MqttMessage message = new MqttMessage(payload);
        message.setQos(qos);
        client.publish(topic, message);
    }
    
    public static void main(String[] args) {
        for (int i=0 ; i<topics.length ; i++) {
            MqttSignalGenerator gen = new MqttSignalGenerator(i, 1000);
            gen.start();
        }
    }
}
