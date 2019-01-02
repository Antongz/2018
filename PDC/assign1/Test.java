/* a1691807, Yi-Ting, Hsieh */
import java.io.*;
import java.util.Scanner;		//needed for Scanner
import java.util.Random;		//needed for Random
import java.math.BigInteger;	//needed for BigInteger

public class Test{
	static int n;
	static int number_Of_Threads;	//number of threads we want to create
	static int[][] A;
	static int[][] B;
	static int[][] C;
	static int result = 0;			//store our final result
	//Create a single shared Scanner for keyboard input
	private static Scanner scanner = new Scanner(System.in);

	public static void main(String[] args){
		//Setting up and generating our matrix A and matrix B
		result = 0;

		if(args.length>0){
			n = Integer.parseInt(args[0]);
			if(args.length==2){ number_Of_Threads = Integer.parseInt(args[1]);
			}else{ number_Of_Threads = n; }
		}else{
			System.out.println("Please input n: ");
			n = scanner.nextInt();
			number_Of_Threads = n;
		}

		A = new int[n][n];
		B = new int[n][n];
		C = new int[n][n];
		//Creating the object of Random class
		Random rand = new Random();
		//Filling matrix with random values between 0 to 1000
		for(int i=0;i<A.length;i++){ for(int j=0;j<A[i].length;j++){ A[i][j] =
		rand.nextInt(10); B[i][j] = rand.nextInt(100); } }

		// Printing A and B matrices
		//System.out.println("A:"); for(int i=0;i<n;i++){ for(int j=0;j<n;j++){
		//System.out.print(A[i][j] + " "); } System.out.println(); }
		//System.out.println("B:"); for(int i=0;i<n;i++){ for(int j=0;j<n;j++){
		//System.out.print(B[i][j] + " "); } System.out.println(); }

		//Thread
		try{
			//Object of multiply Class
			Multiply multiply = new Multiply(n,n);
			//Intializing Threads
			MatrixMultiplier[] thread = new MatrixMultiplier[number_Of_Threads];
			for(int i=0;i<number_Of_Threads;i++){ thread[i] = new
			MatrixMultiplier(multiply,n/number_Of_Threads); }

			//Implementing threads
			Thread th[] = new Thread[number_Of_Threads]; for(int
			i=0;i<number_Of_Threads;i++){ th[i] = new Thread(thread[i]); }

			//Starting threads
			for(int i=0;i<number_Of_Threads;i++){ th[i].start(); }
			for(int i=0;i<number_Of_Threads;i++){ th[i].join(); }
		}catch(Exception e){
			e.printStackTrace();
		}

		//Printing the C
		//System.out.println("Here is the C: ");
		//for(int i=0;i<n;i++){ for(int j=0;j<n;j++){ System.out.print(C[i][j] + " "); }
		//System.out.println(); }
		//System.out.println();

		//Print out our final result
		System.out.println("Final Result: " + result);
	}
}


//Multiply Class
class Multiply extends Test{
	private int i;				//the size of row
	private int j;				//the size of column
	private int SelectBlock;	//assign the #row for different threads

	public Multiply(int i,int j){
		this.i = i;
		this.j = j;
		SelectBlock = 0;
	}

	//Matrix Multiplication Function
	public synchronized void multiplyMatrix(){
		int sum = 0;
		int a = 0;
		int select = this.SelectBlock;
		if(SelectBlock>=i){ return; }
		SelectBlock++;
		for(a=0;a<i;a++){
			sum = 0;
			for(int b=0;b<j;b++){ sum += A[select][b] * B[b][a]; }
			C[select][a] = sum;
			// Calculate our final result
			if(select==a){
				result = result + C[select][a];
			}
		}
	}
}

//Thread Class
class MatrixMultiplier implements Runnable{
	private final Multiply mul;
	private int number_of_columns;
	public MatrixMultiplier(Multiply mul,int number_of_columns){
		this.mul = mul;
		this.number_of_columns = number_of_columns;
	}
	@Override
	public void run(){
		for(int i=0;i<number_of_columns;i++){ mul.multiplyMatrix(); }
	}
}
