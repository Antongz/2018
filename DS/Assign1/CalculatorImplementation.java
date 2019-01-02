/* a1691807, Yi-Ting, Hsieh */
package calculator;
import java.io.*;
import java.util.*;         //importing stack

import java.rmi.server.UnicastRemoteObject;
import java.rmi.Remote;
import java.rmi.RemoteException;


public class CalculatorImplementation implements Calculator {

    private Stack<Integer> stack = new Stack<Integer>();

    // This method will take the value of operand and push it on to the top of the calculator stack
    @Override
    public synchronized void pushValue(int operand) {
        stack.push(new Integer(operand));
    }

    // This method will push a String containing an operator ("+","-","*","/") to the stack,
    // which will cause the server to pop the two operands already on the stack,
    // apply the operation and then push the result back on to the stack.
    @Override
    public synchronized void pushOperator(String operator) throws UnsupportedOperationException, IllegalStateException {

        if (stack.empty()) throw new IllegalStateException();
        int operand2 = stack.pop();
        if (stack.empty()) throw new IllegalStateException();
        int operand1 = stack.pop();

        if (operator.equals("+")) {
            stack.push(operand1 + operand2);
        } else if (operator.equals("-")) {
            stack.push(operand1 - operand2);
        } else if (operator.equals("*")) {
            stack.push(operand1 * operand2);
        } else if (operator.equals("/")) {
            stack.push(operand1 / operand2);
        } else {
            // If the operator is not supported, throw UnsupportedOperationException
            throw new UnsupportedOperationException();
        }
    }

    // This method will pop the top of the calculator stack and return it to the client.
    @Override
    public synchronized int pop() {
        return stack.pop();
    }


    // This method will return true if the stack is empty, false otherwise.
    @Override
    public boolean isEmpty() {
        return stack.empty();
    }

    // This method will wait millis milliseconds before carrying out the pop operation as above.
    @Override
    public synchronized int delayPop(int millis) {
        try {
            Thread.sleep(millis);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        return stack.pop();
    }

}
