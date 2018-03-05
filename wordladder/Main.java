/**
 * Created by noahcao on 2018/3/1.
 */

import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Scanner;
import java.io.BufferedReader;
import java.io.IOException;
import java.util.Queue;
import java.util.Stack;

public class Main {

    public static void main(String[] args) throws IOException{
        HashSet<String> dict  = new HashSet<String>();
        Scanner sc = new Scanner(System.in);
        System.out.print("Enter path of dictionary file:");
        String fileName = sc.nextLine();
        BufferedReader br  = new BufferedReader(new InputStreamReader(new FileInputStream(fileName)));
        String contentLine = br.readLine();
        while(contentLine != null){
            dict.add(contentLine);
            contentLine = br.readLine();
        }
        System.out.print("Enter start word of word ladder:");
        String start = sc.nextLine();;
        System.out.print("Enter end word of word ladder:");;
        String end = sc.nextLine();
        sc.close();
        WordLadder(start, end, dict);
        System.out.println(ladderLength(start, end, dict));
    }

    public static  void WordLadder(String start, String end, HashSet<String> dict){
        HashSet<String> usedWord = new HashSet<String>();
        Queue<Stack<String>> path = new LinkedList<Stack<String>>();
        Stack<String> wordStack = new Stack<String>();
        usedWord.add(start);
        wordStack.push(start);
        path.offer(wordStack);
        while(!path.isEmpty()){
            System.out.println("a stack in path");
            Stack<String> topStack = path.poll();
            if(!topStack.isEmpty()){
                System.out.println("a word in stack");
                String topWord = topStack.peek();
                int wordSize = topWord.length();
                for(int i = 0; i < wordSize; i++){
                    char[] wordArray = topWord.toCharArray();
                    for(char c = 'a'; c <= 'z'; c++){
                        if(wordArray[i] == c) continue;
                        else{
                            char[] tempWordArr = wordArray;
                            tempWordArr[i] = c;
                            String tempWord = new String(tempWordArr);
                            if(tempWord.equals(end)){
                                topStack.push(tempWord);
                                while(!topStack.isEmpty()){
                                    String currWord = topStack.pop();
                                    System.out.print(currWord + ' ');
                                }
                                System.out.println("");
                                return;
                            }
                            else{
                                if(dict.contains(tempWord)){
                                    if(!usedWord.contains(tempWord)){
                                        usedWord.add(tempWord);
                                        Stack<String> copiedStack = (Stack<String>)topStack.clone();
                                        copiedStack.push(tempWord);
                                        path.offer(copiedStack);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        System.out.println("finish");
    }

    public static int ladderLength(String start, String end, HashSet<String> dict){
        if (dict.size() == 0){
            return 0;
        }
        LinkedList<String> wordQueue = new LinkedList<String>();
        LinkedList<Integer> distanceQueue = new LinkedList<Integer>();

        wordQueue.add(start);
        distanceQueue.add(1);

        while(!wordQueue.isEmpty()){
            String currWord = wordQueue.pop();
            Integer currDistance = distanceQueue.pop();
            if(currWord.equals(end)){
                return currDistance;
            }
            for(int i = 0; i < currWord.length(); i++){
                char[] currCharArr = currWord.toCharArray();
                for(char c='a'; c<='z'; c++){
                    currCharArr[i] = c;

                    String newWord = new String(currCharArr);
                    if(dict.contains(newWord)){
                        wordQueue.add(newWord);
                        distanceQueue.add(currDistance+1);
                        dict.remove(newWord);
                    }
                }
            }
        }
        return 0;
    }
}
