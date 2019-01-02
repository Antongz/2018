/* Author: Yi-Ting, Hsieh(a1691807	*/
##Included files: Test.java, one pdf file and one instruction file.

##To run our program, please compile it first using following command:
$ javac Test.java


Once it's compiled, you can call our program. Our program support two argumemts,
one is the size of our matrix, another is the demanded number of thread. 
##Format: $ java program size_of_matrix number_of_threads
$ java Test n t
##Because I only use int type to store the value, the n is limit to 2400.

Or you can use the run.sh to execute the java program.
This script will use time command to check the runtime and print out to screen,
or you can store the result to the file.
It reads the following two format:
##Format1: $ sh run.sh
##Format2: $ sh run.sh file_name
$ sh run.sh

Once you execute it, it will ask what n you want to test. Please enter values and 
press the return button one by one. Once you finish it, press ctrl+D. Next, it will
ask the demanded number of thread. And you should get your result now. 

If you only want to see the runtime, you can use our extract.sh.
##Format: $sh extract.sh fileName_from_result.sh new_file_name


If you have any question, please contact me. 
Thanks
