JFLAGS = -g
JC = javac
.SUFFIXES: .java .class
.java.class:
	$(JC) $(JFLAGS) $*.java

CLASSES = \
		  AtomFeed.java \
		  AtomServer.java \
		  ContentServer.java \
		  FeedAvailabilityCheck.java \
		  FeedEntry.java \
		  FeedEntryComparator.java \
		  FeedProcessor.java \
		  GETClient.java

default: classes

classes: $(CLASSES:.java=.class)

clean:
	$(RM) *.class

