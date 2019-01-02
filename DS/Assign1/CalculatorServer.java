/* a1691807 Yi-Ting, Hsieh */
package calculator;
import java.io.*;

import java.rmi.Remote;
import java.rmi.RemoteException;

/*****************************************************************************
*  The interface of our CalculatorServer.
*  Provide a getCalculator() for Client to call.
*****************************************************************************/
public interface CalculatorServer extends Remote {
    public Calculator getCalculator() throws RemoteException;
 }
