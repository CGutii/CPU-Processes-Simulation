//packages
import java.util.*;
import java.lang.Math.*;
import java.io.*;  
import java.util.Scanner; 
import java.util.concurrent.LinkedBlockingQueue; 
import java.text.DecimalFormat; 

public class sim{
    //input vars
    private static String traceFile;
    private static StringBuilder GHB_default = new StringBuilder("0");
    private static String GHB;
    private static String paddedZeros;
    private static double M;
    private static int N;
    private static int initStatus = 0;
    private static char currBranch;
    private static int mTable[];
    private static int correct = 0;
    private static int wrong = 0;

    public static void main(String args[]) throws Exception{
        //take inputs
        input(args);

        //important variables
        double size_of_table = Math.pow(2,M);
        mTable = new int [(int)size_of_table];
        Arrays.fill(mTable,2);

        //open file
        File file = new File(traceFile);  
        Scanner scan = new Scanner(file);

        //scan file until EOF
        while (scan.hasNextLine()) {

        String data = scan.nextLine();
        String[] traceInput = data.split(" ", 2);
        
        //convert address into binary string 
        String address = hexToBinary(traceInput[0]);
        String branch = traceInput[1];
        currBranch = branch.charAt(0);

        //send address and branch ( t|n ) into simulation
        sim(address,currBranch);
      }

      scan.close();

      output();

    }

    //takes input and organizes it
    public static void input(String args[]){
        //  java sim <GPB> <RB> <Trace_File>
        M = Double.parseDouble(args[0]);
        N = Integer.parseInt(args[1]);
        traceFile = args[2];
    }

    public static String hexToBinary(String hex) {
        int i = Integer.parseInt(hex, 16);
        String binaryString = Integer.toBinaryString(i);
        return binaryString;
    }

    //begins simulation (wrapper function)
    //parses address and branch
    public static void sim(String address, char branch){

        //take address and divide by 4 (get rid of last two bits)
        address = bitShiftLeft2(address);

        //use only the M least significant bits
        address = leastSigBits(address);

        //init GHB
        if (initStatus == 0)
        initGHB();
        //System.out.println("GHB before update: "+GHB);

        //find num of padded zeros
        findNumGHB();

        //add GHB to zeros
        String xorString = GHB + paddedZeros;

        //XOR the address and ghb+paddedZeros
        String index = AddBinaryStrings(address,xorString);

        //take index and compare with mTable and return boolean res
        boolean prediction = compareWithTable(index);

        //now update the GHB
        updateGHB(prediction);
        //printTable();
    }

    public static String bitShiftLeft2(String s)   
    {  
        //returns the string after removing the last character  
        return s.substring(0, s.length() - 2);  
    } 

    public static String leastSigBits(String s)   
    {  
        //returns the string after removing the last character  
        return s.substring(s.length()-(int)M, s.length());  
    } 

    public static String findNumGHB(){
        int MminusN = Math.abs((int)M-N);
        GHB_default.setLength(MminusN);
        paddedZeros = GHB_default.toString().replaceAll("[^0-1]", "0");

        return paddedZeros;
    }

    public static String initGHB(){
        if(N==0)
        GHB_default.setLength(N+1);

        else
        GHB_default.setLength(N);
        GHB = GHB_default.toString().replaceAll("[^0-1]", "0");
        initStatus = 1;

        return paddedZeros;
    }

    // XOR of the two Binary Strings
    public static String  AddBinaryStrings(String address, String xorString){
        String ans = "";
            for (int i = 0; i < M; i++)
            {
                // If the Character matches
                if (address.charAt(i) == xorString.charAt(i))
                    ans += "0";
                else
                    ans += "1";
            }
            return ans;
        }

    public static void updateGHB(boolean prediction){
        //convert string to binary int, shift
        //and then convert back to string
        double foo = (double)Integer.parseInt(GHB, 2);
        foo = (int)foo >> 1;
        //if the index check returned true, then add one to GHR
        if(prediction == true){
        foo = foo + Math.pow(2,N-1);
        GHB = Integer.toBinaryString((int)foo);
        }

        if(prediction == false){
            if(GHB.length() != N )
                GHB = "0" + GHB;
        }

    }  
    
    public static boolean compareWithTable(String stringIndex){
        //System.out.println(stringIndex);
          int index = Integer.parseInt(stringIndex, 2);
          int currNum = mTable[index];

          char predictedBranch;

        switch (currNum) {
            case 0:
                predictedBranch = 'n';
                if (predictedBranch == currBranch) {
                    correct++;
                    return true;
                } else {
                    wrong++;
                    mTable[index] += 1;
                    return false;
                }
            case 1:
            predictedBranch = 'n';
                if (predictedBranch == currBranch) {
                    correct++;
                    mTable[index] -= 1;
                    return true;
                } else {
                    wrong++;
                    mTable[index] += 1;
                    return false;
                }
            case 2:
            predictedBranch = 't';
                if (predictedBranch == currBranch) {
                    correct++;
                    mTable[index] += 1;
                    return true;
                } else {
                    wrong++;
                    mTable[index] -= 1;
                    return false;
                }
            case 3:
            predictedBranch = 't';
                if (predictedBranch == currBranch) {
                    correct++;
                    return true;
                } else {
                    wrong++;
                    mTable[index] -= 1;
                    return false;
                }
        }
        return true;

    }

    public static void output(){
        float mispredictionRatio = ((float)wrong / (float)(correct+wrong))*100; 
        System.out.printf((int)M+" "+(int)N+" ");
        System.out.printf("%.2f", mispredictionRatio);
    }

    public static void printTable(){
        for(int i=0; i< mTable.length;i++)
            System.out.print(mTable[i]);
    }

}