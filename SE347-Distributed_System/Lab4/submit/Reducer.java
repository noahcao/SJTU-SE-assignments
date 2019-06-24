package sjtu.sdic.mapreduce.core;

import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;
import sjtu.sdic.mapreduce.common.KeyValue;
import sjtu.sdic.mapreduce.common.Utils;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.nio.file.Files;
import java.util.*;

/**
 * Created by Cachhe on 2019/4/19.
 */
public class Reducer {

    /**
     *
     * 	doReduce manages one reduce task: it should read the intermediate
     * 	files for the task, sort the intermediate key/value pairs by key,
     * 	call the user-defined reduce function {@code reduceF} for each key,
     * 	and write reduceF's output to disk.
     *
     * 	You'll need to read one intermediate file from each map task;
     * 	{@code reduceName(jobName, m, reduceTask)} yields the file
     * 	name from map task m.
     *
     * 	Your {@code doMap()} encoded the key/value pairs in the intermediate
     * 	files, so you will need to decode them. If you used JSON, you can refer
     * 	to related docs to know how to decode.
     *
     *  In the original paper, sorting is optional but helpful. Here you are
     *  also required to do sorting. Lib is allowed.
     *
     * 	{@code reduceF()} is the application's reduce function. You should
     * 	call it once per distinct key, with a slice of all the values
     * 	for that key. {@code reduceF()} returns the reduced value for that
     * 	key.
     *
     * 	You should write the reduce output as JSON encoded KeyValue
     * 	objects to the file named outFile. We require you to use JSON
     * 	because that is what the merger than combines the output
     * 	from all the reduce tasks expects. There is nothing special about
     * 	JSON -- it is just the marshalling format we chose to use.
     *
     * 	Your code here (Part I).
     *
     *
     * @param jobName the name of the whole MapReduce job
     * @param reduceTask which reduce task this is
     * @param outFile write the output here
     * @param nMap the number of map tasks that were run ("M" in the paper)
     * @param reduceF user-defined reduce function
     */
    public static void doReduce(String jobName, int reduceTask, String outFile, int nMap, ReduceFunc reduceF) {
        Map<String, List<String>> resMap = new HashMap<>();
        for(int i=0; i < nMap; i++){
            // read files produced by doMap and build the key-value map
            try {
                String fileName = Utils.reduceName(jobName,i, reduceTask);
                Path filePath = new File(fileName).toPath();
                String content = new String(Files.readAllBytes(filePath));
                // get the json content from mapped files
                JSONArray jsonContent = JSONArray.parseArray(content);
                List<KeyValue> kvList = JSONObject.parseArray(jsonContent.toJSONString(), KeyValue.class);
                for(int keyId=0; keyId < kvList.size(); keyId++){
                    boolean inMap = resMap.containsKey(kvList.get(keyId).key);
                    if(!inMap){
                        resMap.put(kvList.get(keyId).key, new ArrayList<>());
                    }
                    String key = kvList.get(keyId).key;
                    String value = kvList.get(keyId).value;
                    resMap.get(key).add(value);
                }
            }
            catch (IOException e){
                e.printStackTrace();
            }
        }

        List<String> keyList = new ArrayList<>(resMap.keySet());
        Collections.sort(keyList);
        JSONObject jsonObj = new JSONObject();
        for(int i=0; i < keyList.size(); i++){
            String key = keyList.get(i);
            String[] valueArray = new String[resMap.get(key).size()];
            String[] targetList = resMap.get(key).toArray(valueArray);
            String value = reduceF.reduce(key, targetList);
            jsonObj.put(key, value);
        }

        String fileName = Utils.mergeName(jobName, reduceTask);
        File reduceFile = new File(fileName);
        try {
            if(!reduceFile.exists()){
                reduceFile.createNewFile();
            }

            FileWriter fw = new FileWriter(fileName);
            BufferedWriter bw = new BufferedWriter(fw);
            bw.write(jsonObj.toString());
            bw.close();
        }
        catch (IOException e){
            e.printStackTrace();
        }
    }
}
