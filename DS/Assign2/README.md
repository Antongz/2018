/* Author: Yi-Ting, Hsieh(a1691807)*/
##Included files: atom.xml, AtomFeed.java, AtomServer.java, ContentServer.java, FeedAvailabilityCheck.java, FeedEntry.java, FeedEntryComparator.java, FeedProcessor.java, GETClient.java, two testing folders:ContentServer1/ and ContentServer2., a Makefile, a testing_instruction.txt and a README.md file.

##To run our program, please compile it first using following command:
$ make 

##To uninstall the program
$ make clear

## To run the AtomServer(the default port number is 4567):
$ java AtomServer (#port)

## To run the ContentServer:
$ java ContentServer localhost:#port path_of_news

## To run the GETClient:
$ java GETClient localhost:#port

## In order to examinee whether our program fulfill the requirements, please check the testing_instruction.txt for further details.

If you have any question, please contact me. 
Thanks
