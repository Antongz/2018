/* Author: Yi-Ting, Hsieh(a1691807)*/
##Included files: Calculator.java CalculatorImplementation.java, CalculatorServer.java CalculatorServerImplementation.java, CalculatorClient.java and one README.md file.

##To run our program, please compile it first using following command:
$ javac -d . Calculator.java CalculatorImplementation.java CalculatorServer.java CalculatorServerImplementation.java CalculatorClient.java


## Once it's compiled, make sure you start the registry before running our program:
For example, on the Solaris(tm) Operating System:
$ rmiregistry &
Or, on Windows platforms:
$ start rmiregistry 

## To run the CalculatorServer:
$ java -classpath . calculator.CalculatorServerImplementation &

## To run the CalculatorClient:
$ java -classpath . calculator.CalculatorClient 

Our CalculatorClient can randomly assigne number of threads to run(from 1 to 21). Each client thread will randomly perform number of turns of requests(1 to 21), while each requests contains:
	                i.)  pushValue -> pushValue -> pushOperator -> pop
                	ii.) pushValue -> pushValue -> pushOperator -> delayPop
And the CalculatorClient will write the results to two files, one is our expect result: correct_output.txt, another is the RMI result: RMI_output.txt.

## In order to check whether the result as expected:
$ diff -w  correct_output.txt RMI_output.txt
## If the RMI output is as expected, the terminal will show nothing on the screen.

You can also cat out the output:
$ cat RMI_output.txt


If you have any question, please contact me. 
Thanks
