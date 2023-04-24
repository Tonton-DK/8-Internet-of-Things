import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.json.*;
import java.util.Arrays;
import java.util.Map;
import java.util.HashMap;

public class MqttMovingAverage implements MqttCallback {
    static String broker = "tcp://localhost:1883";
    static int qos = 2;
    final int WINDOW_SIZE = 3;
    
    MqttClient client;
    Map<String, double[]> windows;
    Map<String, Integer>  indices;
    
    public MqttMovingAverage (String broker, String topic) throws MqttException {
        MemoryPersistence persistence = new MemoryPersistence();
        client = new MqttClient(broker, "java/mavg", persistence);
        MqttConnectOptions connOpts = new MqttConnectOptions();
        connOpts.setCleanSession(true);
        client.setCallback(this);
        client.connect(connOpts);
        client.subscribe(topic, qos);
        
        windows = new HashMap<String, double[]>();
        indices = new HashMap<String, Integer>();
    }
    
    public void deliveryComplete(IMqttDeliveryToken token) {}
    public void connectionLost(Throwable cause) {
        System.out.println("Connection lost. Cause: " + cause);
        System.out.println("Did you start up a client with the same ID?");
        System.exit(1);
    }
    
    public void messageArrived (String topic, MqttMessage message) throws MqttException {
        try {
            String[] topic_parts = topic.split("/");
            String[] mavg_topic_parts = Arrays.copyOfRange(topic_parts, 1, topic_parts.length);
            String mavg_topic = "mavg/"+String.join("/", mavg_topic_parts);
            JSONObject jo = new JSONObject(new String(message.getPayload()));
            double time  = jo.getDouble("time");
            double value = jo.getDouble("value");
            
            // init
            if (!windows.containsKey(topic)) {
                double[] window = new double[WINDOW_SIZE];
                for (int i=0 ; i<WINDOW_SIZE ; i++) window[i] = 0;
                windows.put(topic, window);
                indices.put(topic, 0);
            }
            
            // update
            double[] window = windows.get(topic);
            int index = indices.get(topic);
            window[index%WINDOW_SIZE] = value;
            indices.put(topic, index+1);
            
            // publish
            double sum = 0;
            for (int i=0 ; i<WINDOW_SIZE ; i++) sum += window[i];
            tx(mavg_topic, sample2json(time, sum/WINDOW_SIZE));
        } catch (JSONException e) {
            System.out.println("Received exception in messageArrived: "+e);
        }
    }
    
    public byte[] sample2json (double time, double value) {
        JSONObject jo = new JSONObject();
        jo.put("time", time);
        jo.put("value", value);
        return jo.toString().getBytes();
    }
    
    public void tx (String topic, byte[] payload) throws MqttException {
        MqttMessage message = new MqttMessage(payload);
        message.setQos(qos);
        client.publish(topic, message);
    }
    
    public static void main(String[] args) throws MqttException, InterruptedException {
        MqttMovingAverage ma = new MqttMovingAverage(broker, "siggen/+/+");
        
        // stay alive
        while (true) {
            Thread.sleep(1000);
        }
    }
}
