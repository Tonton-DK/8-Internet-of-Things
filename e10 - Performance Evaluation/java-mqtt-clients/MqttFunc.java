import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.json.*;
import java.util.Arrays;
import java.util.Map;
import java.util.HashMap;
import java.lang.Math.*;

public class MqttFunc implements MqttCallback {
    static String broker = "tcp://localhost:1883";
    static int qos = 2;
    
    MqttClient client;
    Map<String, Double> temps;
    Map<String, Double> rhums;
    
    public MqttFunc (String broker, String topic) throws MqttException {
        MemoryPersistence persistence = new MemoryPersistence();
        client = new MqttClient(broker, "java/func", persistence);
        MqttConnectOptions connOpts = new MqttConnectOptions();
        connOpts.setCleanSession(true);
        client.setCallback(this);
        client.connect(connOpts);
        client.subscribe(topic, qos);
        
        temps = new HashMap<String, Double>();
        rhums = new HashMap<String, Double>();
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
            String temp_topic = "mavg/"+topic_parts[1]+"/temp";
            String rhum_topic = "mavg/"+topic_parts[1]+"/rhum";
            String ahum_topic = "func/"+topic_parts[1]+"/ahum";
            
            JSONObject jo = new JSONObject(new String(message.getPayload()));
            double time  = jo.getDouble("time");
            double value = jo.getDouble("value");
            
            // init
            if (!temps.containsKey(temp_topic)) {
                temps.put(temp_topic, 0.0);
                rhums.put(rhum_topic, 0.0);
            }
            
            // update
            if (topic_parts[2].equals("temp")) {
                temps.put(temp_topic, value);
            } else if (topic_parts[2].equals("rhum")) {
                rhums.put(rhum_topic, value);
            } else {
                return;
            }
            
            // publish
            double ahum = calc_abs_hum(temps.get(temp_topic), rhums.get(rhum_topic));
            tx(ahum_topic, sample2json(time, ahum));
        } catch (JSONException e) {
            System.out.println("Received exception in messageArrived: "+e);
        }
    }
    
    // unit: g/m³
    double calc_abs_hum (double temp, double rhum) {
        return 6.112*Math.pow(Math.E, (17.67*temp)/(temp+243.5))*rhum*2.1674/(273.15+temp);
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
        MqttFunc ma = new MqttFunc(broker, "mavg/+/+");
        
        // stay alive
        while (true) {
            Thread.sleep(1000);
        }
    }
}
