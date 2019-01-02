/* Author: Yi-Ting, Hsieh(a1691807	*/
## Included files: 
## SetUp.java, RandomArticleProducer.java, 
## NewsAggregator.java, NewsCreator.java, Client.java, 
## one pdf file and one instruction file.


## To run our program, please compile it first using following command:

First, Compile all the java programs
$ javac SetUp.java
$ javac RandomArticleProducer.java
$ javac NewsAggregator.java
$ javac NewsCreator.java
$ javac Client.java



## Everytime when you test the program, please call the SetUp for cleaning out
## the current directory(NA/, NC/ and C/) and setting up the environment
$ java SetUp

## Now you can create our NewsAggregator, it requires one parameter for setting up the maximum number of requests from all clients.
## Format: $ java NewsAggregator  maximum_Number_of_requests
## If it is just a simple test, you can just put it 1.
$ java NewsAggregator 1


## You can create a NewsCreator by assigning it's ID and news creation rate(NCR: in milliseconds)
## Format: $ java NewsCreator ID NCR
## The example is that we create a NC with ID=1 and NCR=10 milliseconds.
$ java NewsCreator 1 10


## You can also create a Client with ID to request NA for news
## Format: $ java Client id
## The example is how to create a Client with id=2
$ java Client 2



If you have any question, please contact me. 
Thanks
