package sjtu.sdic.mapreduce;

import org.apache.commons.io.filefilter.WildcardFileFilter;
import sjtu.sdic.mapreduce.common.KeyValue;
import sjtu.sdic.mapreduce.core.Master;
import sjtu.sdic.mapreduce.core.Worker;

import java.io.File;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by Cachhe on 2019/4/24.
 */
public class InvertedIndex {

    public static List<KeyValue> mapFunc(String file, String value) {
        List<KeyValue> kvList = new ArrayList<>();
        String[] digitArray = {".", ",", "?", "!", ":", ";", "\"", "'", "-", "_", "=",
                "+", "/", "\\", "[", "]", "{", "}", "<", ">", "|", "~", "`", "@",
                "#", "$", "%", "^", "&", "*", "(", ")"};
        for(int i=0; i < digitArray.length; i++){
            value = value.replace(digitArray[i], " ");
        }
        String[] tempString = value.split("\\s+");
        for(int i=0; i < tempString.length; i++){
            String s = tempString[i];
            if(s.matches("[a-zA-Z0-9]+")){
                kvList.add(new KeyValue(s, file));
            }
        }
        return kvList;
    }

    public static String reduceFunc(String key, String[] values) {
        int vLen = values.length;
        String resStr = "";
        Collection seenValues = new HashSet();
        for(int i=0; i < vLen; i++){
            if(seenValues.contains(values[i])){
                continue;
            }
            else {
                seenValues.add(values[i]);
                resStr = resStr + values[i] + ", ";
            }
        }
        if(vLen > 0){
            resStr = resStr.substring(0, resStr.length()-2);
        }
        String res = seenValues.size() + " " + resStr;
        return res;
    }

    public static void main(String[] args) {
        if (args.length < 3) {
            System.out.println("error: see usage comments in file");
        } else if (args[0].equals("master")) {
            Master mr;

            String src = args[2];
            File file = new File(".");
            String[] files = file.list(new WildcardFileFilter(src));
            if (args[1].equals("sequential")) {
                mr = Master.sequential("iiseq", files, 3, InvertedIndex::mapFunc, InvertedIndex::reduceFunc);
            } else {
                mr = Master.distributed("wcdis", files, 3, args[1]);
            }
            mr.mWait();
        } else {
            Worker.runWorker(args[1], args[2], InvertedIndex::mapFunc, InvertedIndex::reduceFunc, 100, null);
        }
    }
}
