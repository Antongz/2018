import java.io.Reader;
import java.io.IOException;
import java.io.Writer;
import java.io.BufferedReader;
import java.util.Set;


public class MapReaderWriter implements MapIo
{
    //This class handles reading and writing map representations as
    //described in the practical specification

    public MapReaderWriter(){};

    //Read the description of a map from the
    //Reader r, and transfers it to Map, m.
    public void read (Reader r, Map m)
      throws IOException, MapFormatException {
          String currentLine = null;
          BufferedReader br = new BufferedReader(r);

          while ((currentLine = br.readLine()) != null) {
              String[] currentLineItems = currentLine.split("\\s");

              for (int x=0; x<currentLineItems.length; x++) {
                  // check is place/road/start/end
                  if (x==0) {
                      if (currentLineItems[0].equals("place")) {
                          m.newPlace(currentLineItems[1], Integer.parseInt(currentLineItems[2]), Integer.parseInt(currentLineItems[3]));
                      }
                      else if (currentLineItems[0].equals("road")) {
                           Place from, to;

                           try {
                               from = m.findPlace(currentLineItems[1]);
                               to = m.findPlace(currentLineItems[4]);

                           } catch (IllegalArgumentException ie) {
                               throw new IllegalArgumentException();
                           }

                           m.newRoad(from, to, currentLineItems[2],Integer.parseInt(currentLineItems[3]));
                       }
                       else if (currentLineItems[0].equals("start")) {
                           Place start;
                           try {
                               start = m.findPlace(currentLineItems[1]);
                           } catch (IllegalArgumentException ie) {
                               throw new IllegalArgumentException();
                           }
                           m.setStartPlace(start);
                       }
                       else if (currentLineItems[0].equals("end")) {
                           Place end;
                           try {
                               end = m.findPlace(currentLineItems[1]);
                           } catch (IllegalArgumentException ie) {
                               throw new IllegalArgumentException();
                           }
                           m.setEndPlace(end);
                       }
                  }
              }
          }

      }


    //Write a representation of the Map, m, to the w w.
    public void write(Writer w, Map m)
      throws IOException {
          Set<Place> place = m.getPlaces();
          Set<Road> road = m.getRoads();

          for (Place i : place) {
              w.append("place ");
              w.append(i.getName() + " ");
              w.append(i.getX() + " " + i.getY() + "\n");
          }
          w.append("\n");

          // road firstPlace roadName length secondPlace
          for (Road i : road) {
              w.append("road ");
              w.append(i.firstPlace().getName() + " ");
              w.append(i.roadName() + " " + i.length() + " " + i.secondPlace().getName() + "\n");
          }

          // start placeName
          w.append("start " + m.getStartPlace().getName() + "\n");

          // end placeName
          w.append("end " + m.getEndPlace().getName() + "\n");
      }
}
