/* a1691807 Yi-Ting, Hsieh */
package calculator;
import java.io.*;

import java.rmi.AlreadyBoundException;
import java.rmi.NoSuchObjectException;
import java.rmi.RemoteException;
import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.server.UnicastRemoteObject;


/*****************************************************************************
*  The implementation class of our CalculatorServer.
*  The sever port is 1099 as default.
*  The sever name is CalculatorServer.
*****************************************************************************/
public class CalculatorServerImplementation implements CalculatorServer {
    private final String serverName;
    private final int severPort;
    private Registry serverReg;

    public CalculatorServerImplementation(String name, int port) {
        serverName = name;
        severPort = port;
    }

    public void runServer() throws RemoteException, AlreadyBoundException {
        CalculatorServer stub = (CalculatorServer) UnicastRemoteObject.exportObject(this, 0);
        // Bind the remote object's stub in the registry
        Registry registry = LocateRegistry.getRegistry();
        registry.rebind(serverName, stub);
    }

    public void stopServer() throws NoSuchObjectException {
        UnicastRemoteObject.unexportObject(this, true);
        UnicastRemoteObject.unexportObject(serverReg, true);
    }

    @Override
    public Calculator getCalculator() throws RemoteException {
        CalculatorImplementation calc = new CalculatorImplementation();
        
        // Export the Calculator
        Calculator stub = (Calculator) UnicastRemoteObject.exportObject(calc, 0);
        return stub;
    }

    public static void main(String args[]) {
        CalculatorServerImplementation server = new CalculatorServerImplementation("CalculatorServer", 1099);
        try {
            server.runServer();
            System.out.println("Server ready");
        } catch (Exception e) {
            System.err.println("Server exception: " + e.toString());
            e.printStackTrace();
        }
    }


 }
