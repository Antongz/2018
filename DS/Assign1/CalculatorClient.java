/* a1691807 Yi-Ting, Hsieh */
package calculator;

import java.io.*;
import java.util.*;
import java.util.Random;    // needed for Random
import java.nio.file.*;

import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;


/*****************************************************************************
*  The CalculatorClient will
*       1.) create two files for storing the result - one is
*           the correct_output.txt, and another one is our RMI_output.txt.
*       2.) allocate a random number(from 1 to 21) of threads of our TestClients.
*       3.) TestClients will request the RMI server for performing stack operations.
*****************************************************************************/
public class CalculatorClient {

    static private int number_of_Threads;

    // Create File
    public static void createFile(String path) {
        File newFile;
        try {
            newFile = new File(path);
            if (!newFile.exists()) {
                newFile.createNewFile();
            } else {
                newFile.delete();
                newFile.createNewFile();
            }
        } catch(IOException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        Random rand = new Random();
        number_of_Threads = rand.nextInt(20) + 1;

        createFile("correct_output.txt");
        createFile("RMI_output.txt");

        TestClients[] clients = new TestClients[number_of_Threads];
        for (int i=0; i<number_of_Threads; i++) {
            clients[i] = new TestClients(rand.nextInt(20) + 1, i);
        }

        //Implementing threads
        Thread thread[] = new Thread[number_of_Threads];
        for(int i=0; i<number_of_Threads; i++) {
            thread[i] = new Thread(clients[i]);
        }

        try {
            for (int i=0; i<number_of_Threads; i++) {
                thread[i].start();
            }
        } catch (Exception e) {
            System.err.println("Client exception: " + e.toString());
            e.printStackTrace();
        }
    }
}



/*****************************************************************************
*  The TestClients will perform the following operations:
*       1.) It will call the registry to obtain the stub object of CalculatorServer.
*       2.) It will run number_of_turn of requests, it contains the following
*           opations for each request:
*               i.)  pushValue -> pushValue -> pushOperator -> pop
*               ii.) pushValue -> pushValue -> pushOperator -> delayPop
*       3.) TestClients will store the result on buffer and write it to the
*           correct_output.txt and RMI_output.txt at the end.
*****************************************************************************/
class TestClients extends CalculatorClient implements Runnable {

    private int number_of_turns;
    private int ID;
    private String buffer_correct = "";
    private String buffer_RMI = "";
    Calculator calc;

    public TestClients(int number_of_turn, int id) {
        number_of_turns = number_of_turn;
        ID = id;
    }

    @Override
    public void run() {
        try {
            Registry registry = LocateRegistry.getRegistry(null);
            CalculatorServer stub = (CalculatorServer) registry.lookup("CalculatorServer");
            // Get a calculator stub from the server
            calc = stub.getCalculator();
        } catch (Exception e) {
            System.out.println(e);
        }

        // Creating the object rand of Random class
        Random rand = new Random();

        try {
            for (int i=0; i<number_of_turns; i++) {
                int randType = rand.nextInt(2);
                int operand1 = rand.nextInt(10000) + 1;
                int operand2 = rand.nextInt(10000) + 1;

                calc.pushValue(operand1);
                calc.pushValue(operand2);

                int operatorN = rand.nextInt(4);
                String operator;
                int result;
                switch (operatorN) {
                    case 1: operator = "+";
                            result = operand1 + operand2;
                            break;

                    case 2: operator = "-";
                            result = operand1 - operand2;
                            break;

                    case 3: operator = "*";
                            result = operand1 * operand2;
                            break;

                    case 0: operator = "/";
                            result = operand1 / operand2;
                            break;

                    default: operator = "+";
                            result = operand1 + operand2;
                            break;
                }
                calc.pushOperator(operator);

                // If randType is 0: user calls: pushValues -> pushOperator -> pop
                if (randType == 1) {
                    buffer_RMI += "The result of " + Integer.toString(operand1) + operator + Integer.toString(operand2) + " is: " + Integer.toString(calc.pop()) + "\n";
                    buffer_correct += "The result of " + Integer.toString(operand1) + operator + Integer.toString(operand2) + " is: " + Integer.toString(result) + "\n";
                }

                // If user calls: pushValues -> pushOperator -> delayPop
                else {
                    int delay = rand.nextInt(1000);
                    buffer_RMI += "The result of " + Integer.toString(operand1) + operator + Integer.toString(operand2) + " is: " + Integer.toString(calc.delayPop(delay)) + "\n";
                    buffer_correct += "The result of " + Integer.toString(operand1) + operator + Integer.toString(operand2) + " is: " + Integer.toString(result) + "\n";
                }
            }
        } catch (Exception e) {
            System.out.println(e);
        }
        write_Output();
    }

    public void write_Output() {
        FileOutputStream out = null;
        try {
            Files.write(Paths.get("RMI_output.txt"), buffer_RMI.getBytes(), StandardOpenOption.APPEND);
            Files.write(Paths.get("correct_output.txt"), buffer_correct.getBytes(), StandardOpenOption.APPEND);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
