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

    static HashSet<String> dict  = new HashSet<String>();
    static HashSet<String> usedWord = new HashSet<String>();
    static Queue<Stack<String>> path = new LinkedList<Stack<String>>();
    static Stack<String> topStack = new Stack<String>();

    public static void main(String[] args) throws IOException{
        //HashSet<String> dict  = new HashSet<String>();
        Scanner sc = new Scanner(System.in);
        System.out.print("Enter path of dictionary file:");
        String fileName = sc.nextLine();
        BufferedReader br  = new BufferedReader(new InputStreamReader(new FileInputStream(fileName)));
        String contentLine = br.readLine();
        while(contentLine != null){
            dict.add(contentLine);
            contentLine = br.readLine();
        }
        while(true){
            usedWord = new HashSet<String>();
            path = new LinkedList<Stack<String>>();
            topStack = new Stack<String>();
            System.out.print("Enter start word of word ladder (print Enter to quit):");
            String start = sc.nextLine();
            if(start.equals("")) break;
            System.out.print("Enter end word of word ladder:");
            String end = sc.nextLine();
            WordLadder(end, start, dict);
        }
        sc.close();
    }

    public static  void WordLadder(String start, String end, HashSet<String> dict){
        //HashSet<String> usedWord = new HashSet<String>();
        //Queue<Stack<String>> path = new LinkedList<Stack<String>>();
        Stack<String> wordStack = new Stack<String>();
        usedWord.add(start);
        wordStack.push(start);
        path.offer(wordStack);
        while(!path.isEmpty()){
            topStack = path.poll();
            if(!topStack.isEmpty()){
                String topWord = topStack.peek();
                int wordSize = topWord.length();
                for(int i = 0; i < wordSize; i++){
                    String tempWord;
                    boolean finish = false;
                    //char[] wordArray = topWord.toCharArray();
                    for(char c = 'a'; c <= 'z'; c++){
                        // 1. delete a character
                        if(wordSize > 1){
                            tempWord = topWord.substring(0,i) + topWord.substring(i+1,wordSize);
                            finish = checkWord(tempWord, topStack, end);
                            if(finish) return;
                        }
                        // 2. add a character
                        if(i == wordSize - 1){
                            tempWord = topWord.substring(0, wordSize) + c;
                            finish = checkWord(tempWord, topStack, end);
                            if(finish) return;
                        }
                        else{
                            tempWord = topWord.substring(0,i) + c + topWord.substring(i+1, wordSize);
                            finish = checkWord(tempWord, topStack, end);
                            if(finish) return;
                        }
                        // 3. replace a character
                        tempWord = topWord.substring(0,i) + c + topWord.substring(i+1,wordSize);
                        finish = checkWord(tempWord, topStack, end);
                        if(finish) return;
                    }
                }
            }
        }
        System.out.println("finish without finding a word ladder");
    }

    public static void printLadder(Stack<String> topStack){
        // print words building the word ladder
        while(!topStack.isEmpty()){
            String currWord = topStack.pop();
            System.out.print(currWord + ' ');
        }
        System.out.println("");
    }

    public static boolean checkWord(String tempWord, Stack<String>topStack, String end){
        // check whether tempWord == end, and finish or continue depending on the result
        if(tempWord.equals(end)){
            topStack.push(tempWord);
            printLadder(topStack);
            return true;
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
            return false;
        }
    }
}
