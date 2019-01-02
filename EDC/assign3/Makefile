JFLAGS = -g
JC = javac
.SUFFIXES: .java .class
.java.class:
	$(JC) $(JFLAGS) $*.java

CLASSES = \
		  MapImpl.java \
		  Place.java \
		  Dijkstra.java \
		  MapIo.java \
		  PlaceListener.java \
		  Map.java \
		  RoadIcon.java \
		  ExtensionFileFilter.java \
		  MapListener.java \
		  Road.java \
		  MapPanel.java \
		  RoadListener.java \
		  MapEditor.java \
		  MapReaderWriter.java \
		  MapFormatException.java \
		  PlaceIcon.java

default: classes

classes: $(CLASSES:.java=.class)

clean:
	$(RM) *.class
