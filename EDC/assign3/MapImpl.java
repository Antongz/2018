import java.util.List;
import java.util.Set;
import java.io.*;
import java.util.*;

public class MapImpl implements Map {

    private ArrayList<MapListener> mapListener;
    private Set<Place> places;
    private Set<Road> roads;
    private PlaceImpl startPlace, endPlace;


    public MapImpl(){
        places = new HashSet<Place>();
        roads = new HashSet<Road>();
        startPlace = null;
        endPlace = null;
        mapListener = new ArrayList<MapListener>();

    }

    //Add the MapListener ml to this map.
    //Note: A map can have multiple listeners
    @Override
    public void addListener(MapListener ml) {
        mapListener.add(ml);
    }


    //Delete the MapListener ml from this map.
    @Override
    public void deleteListener(MapListener ml) {
        mapListener.remove(ml);
    }


    //Create a new Place and add it to this map
    //Return the new place
    //Throws IllegalArgumentException if:
    //  the name is not valid or is the same as that
    //  of an existing place
    //Note: A valid placeName begins with a letter, and is
    //followed by optional letters, digits, or underscore characters
    @Override
    public Place newPlace(String placeName, int xPos, int yPos)
      throws IllegalArgumentException {

          boolean isDuplicated = false;
          for (Place i : this.places) {
              if (i.getName().equals(placeName)) {
                  isDuplicated = true;
                  break;
              }
          }

          if (isDuplicated || placeName == null || placeName.equals("")) {
              throw new IllegalArgumentException();
          } else if (!placeName.matches("^([A-Za-z])(\\w)*$")) {
            throw new IllegalArgumentException("Invalid place name: "+ placeName);
          }

          if (xPos < 0 || yPos < 0) {
              throw new IllegalArgumentException("Place: " + placeName + ", place coordinates cannot be negative");
          }

          PlaceImpl tempPlace = new PlaceImpl(placeName, xPos, yPos);
          tempPlace.setContaining(this);
          places.add((Place) tempPlace);
          this.placesChanged();
          return (Place)tempPlace;
      }


    //Remove a place from the map
    //If the place does not exist, returns without error
    @Override
    public void deletePlace(Place s) {
        if (places.contains(s)) {
            Set<Road> toroads = s.toRoads();
            for (Road r : toroads)
                this.deleteRoad(r);

            if (s.isStartPlace())   this.setStartPlace(null);
            if (s.isEndPlace()) this.setEndPlace(null);

            places.remove(s);
            this.placesChanged();
        }

    }


    //Find and return the Place with the given name
    //If no place exists with given name, return NULL
    @Override
    public Place findPlace(String placeName) {
        for (Place i : this.places) {
            if (i.getName().equals(placeName))
                return i;
        }

        return null;
    }


    //Return a set containing all the places in this map
    @Override
    public Set<Place> getPlaces() {
        return this.places;
    }


    //Create a new Road and add it to this map
    //Returns the new road.
    //Throws IllegalArgumentException if:
    //  the firstPlace or secondPlace does not exist or
    //  the roadName is invalid or
    //  the length is negative
    //Note: A valid roadName is either the empty string, or starts
    //with a letter and is followed by optional letters and digits
    @Override
    public Road newRoad(Place from, Place to,
      String roadName, int length)
      throws IllegalArgumentException {
          if (!roadName.matches("^([A-Za-z])([A-Za-z0-9])*$|^$")) {
           throw new IllegalArgumentException("Invalid road name: " + roadName);
          }
          if (!places.contains(from)) {
              throw new IllegalArgumentException();
          }
          if (!places.contains(to)) {
              throw new IllegalArgumentException();
          }

          if (length < 0) {
              throw new IllegalArgumentException();
          }

          //TODO...
          //Note: A valid roadName is either the empty string, or starts
          //with a letter and is followed by optional letters and digits
          for (Road rr : this.getRoads()) {
              if ((rr.firstPlace() == from || rr.secondPlace() == from) && (rr.firstPlace() == to || rr.secondPlace() == to)) {
                    throw new IllegalArgumentException("A road between " + from.getName() + " and " + to.getName() +" already exists in the map! Road Name: " + rr.roadName());
              }
          }

          if (roadName.equals("-")) roadName = "";

          Road tempRoad = new RoadImpl(from, to, roadName, length);
          this.roads.add(tempRoad);
          this.roadsChanged();
          return tempRoad;
      }


    //Remove a road r from the map
    //If the road does not exist, returns without error
    @Override
    public void deleteRoad(Road r) {
        if (roads.contains(r)) {
            roads.remove(r);
            this.roadsChanged();
        }
    }


    //Return a set containing all the roads in this map
    @Override
    public Set<Road> getRoads() {
        return this.roads;
    }


    //Set the place p as the starting place
    //If p==null, unsets the starting place
    //Throws IllegalArgumentException if the place p is not in the map
    @Override
    public void setStartPlace(Place p)
      throws IllegalArgumentException {
        if (places.contains(p) || p == null) {
            if (p == null) {
                startPlace = null;
                this.otherChanged();
            } else {
                startPlace = (PlaceImpl) p;
                this.startPlace.setContaining(this);
                this.otherChanged();
            }
        }
        else {
            throw new IllegalArgumentException("Unable to set start place, provided Place is not in the map!");
        }
      }


    //Return the starting place of this map
    @Override
    public Place getStartPlace() {
        return (Place)startPlace;
    }


    //Set the place p as the ending place
    //If p==null, unsets the ending place
    //Throws IllegalArgumentException if the place p is not in the map
    @Override
    public void setEndPlace(Place p)
      throws IllegalArgumentException {
          if (places.contains(p) || p == null) {
              if (p == null) {
                  endPlace = null;
                  this.otherChanged();
              } else {
                  endPlace = (PlaceImpl) p;
                  this.endPlace.setContaining(this);
                  this.otherChanged();
              }
          }
          else {
              throw new IllegalArgumentException("Unable to set end place, provided Place is not in the map!");
          }
      }


    //Return the ending place of this map
    @Override
    public Place getEndPlace() {
        return (Place)endPlace;
    }


    //Causes the map to compute the shortest trip between the
    //"start" and "end" places
    //For each road on the shortest route, sets the "isChosen" property
    //to "true".
    //Returns the total distance of the trip.
    //Returns -1, if there is no route from start to end
    @Override
    public int getTripDistance() {
        //reset chosen roads
       for (Road rr : this.getRoads()){
           RoadImpl r = (RoadImpl)rr;
           r.setChosen(false);
       }

       Dijkstra dk = new Dijkstra(this.getPlaces(),this.getRoads());

       int distance = dk.computePaths(this.getStartPlace(),this.getEndPlace());

       if (distance == Integer.MAX_VALUE) { return -1; }

       List<Place> path = dk.getShortestPathTo(this.getEndPlace());

       for (int i = 0; i < path.size(); i++) {
           Place pl = path.get(i);

           if (i + 1 < path.size()) {
               RoadImpl rd = (RoadImpl)pl.roadTo(path.get(i + 1));
               if (rd != null)  rd.setChosen(true);
           }
       }

       return distance;
    }


    //Return a string describing this map
    //Returns a string that contains (in this order):
    //for each place in the map, a line (terminated by \n)
    //  PLACE followed the toString result for that place
    //for each road in the map, a line (terminated by \n)
    //  ROAD followed the toString result for that road
    //if a starting place has been defined, a line containing
    //  START followed the name of the starting-place (terminated by \n)
    //if an ending place has been defined, a line containing
    //  END followed the name of the ending-place (terminated by \n)
    @Override
    public String toString() {
        String temp = "";
        for (Place i : this.places) {
            temp += "PLACE " + i.toString() + "\n";
        }
        temp += "\n";
        for (Road i : this.roads) {
            temp += "ROAD " + i.toString() + "\n";
        }


        if (startPlace != null) {
            temp += "START " + startPlace.getName() + "\n";
        } else {
            temp += "START\n";
        }

        if (endPlace != null) {
            temp += "END " + endPlace.getName() + "\n";
        } else {
            temp += "END\n";
        }
        temp += "\n";

        return temp;
    }

    private void placesChanged() {
        for (MapListener ml: mapListener)
            ml.placesChanged();
    }

    private void roadsChanged() {
        for (MapListener ml: mapListener)
            ml.roadsChanged();
    }

    private void otherChanged() {
        for (MapListener ml: mapListener)
            ml.otherChanged();
    }
}


class PlaceImpl implements Place {
    private Map thisMap;
    private Place previous = null;
    private ArrayList<PlaceListener> placeListener;
    private String name;
    private int dx, dy;
    // Set<Road> toroads;

    PlaceImpl(String name, int dx, int dy) {
        this.name = name;
        this.dx = dx;
        this.dy = dy;
        this.placeListener = new ArrayList<PlaceListener>();
    }


    //Add the PlaceListener pl to this place.
    //Note: A place can have multiple listeners
    @Override
    public void addListener(PlaceListener pl) {
        placeListener.add(pl);
    }


    //Delete the PlaceListener pl from this place.
    @Override
    public void deleteListener(PlaceListener pl) {
        placeListener.remove(pl);
    }


    //Return a set containing all roads that reach this place
    @Override
    public Set<Road> toRoads() {
        Set<Road> existingRoads = thisMap.getRoads();
        Set<Road> toroads = new HashSet<Road>();

        for (Road r : existingRoads)
            if ((r.secondPlace().getName().equals(this.name)) || (r.firstPlace().getName().equals(this.name)))
                toroads.add(r);

        return toroads;
    }


    //Return the road from this place to dest, if it exists
    //Returns null, if it does not
    @Override
    public Road roadTo(Place dest) {
        Set<Road> existingRoads = thisMap.getRoads();
        Road roadto = null;

        for (Road i : existingRoads) {
            if (i.firstPlace().getName().equals(this.name) && i.secondPlace().getName().equals(dest.getName()))
                roadto = i;
            else if (i.secondPlace().getName().equals(this.name) && i.firstPlace().getName().equals(dest.getName()))
                roadto = i;
        }

        return roadto;
    }


    //Move the position of this place
    //by (dx,dy) from its current position
    @Override
    public void moveBy(int dx, int dy) {
        this.dx += dx;
        this.dy += dy;
        this.placeChanged();
    }


    //Return the name of this place
    @Override
    public String getName() {
        return this.name;
    }


    //Return the X position of this place
    @Override
    public int getX() {
        return this.dx;
    }


    //Return the Y position of this place
    @Override
    public int getY() {
        return this.dy;
    }


    //Return true if this place is the starting place for a trip
    @Override
    public boolean isStartPlace() {
        return this.equals(this.thisMap.getStartPlace());
    }


    //Return true if this place is the ending place for a trip
    @Override
    public boolean isEndPlace() {
        return this.equals(this.thisMap.getEndPlace());
    }


    //Return a string containing information about this place
    //in the form (without the quotes, of course!) :
    //"placeName(xPos,yPos)"
    // place placeName xpos ypos
    @Override
    public String toString() {
        return name + "(" +  Integer.toString(dx) + "," + Integer.toString(dy) + ")";
    }

    public void setContaining(Map m) {
        thisMap = m;
    }

    public void setPrevious(Place p) {
        this.previous = p;
    }

    public Place getPrevious() {
        return this.previous;
    }

    private void placeChanged() {
        for (PlaceListener pl : placeListener)
            pl.placeChanged();

        for (Road r : this.toRoads()) {
            RoadImpl ri = (RoadImpl) r;
            ri.roadChanged();
        }
    }
}

class RoadImpl implements Road {
    private ArrayList<RoadListener> roadListener;
    Place from, to;
    private String roadName;
    private int length;
    private boolean ischosen;

    RoadImpl(Place from, Place to, String roadName, int length) {
        int temp = from.getName().compareTo(to.getName());
        if (temp <= 0) {
            this.from = from;
            this.to = to;
        } else if (temp > 0) {
            this.from = to;
            this.to = from;
        } else {
            ;//from == to
        }
        this.roadName = roadName;
        this.length = length;
        this.ischosen = false;
        roadListener = new ArrayList<RoadListener>();
    }
    //Add the RoadListener rl to this place.
    //Note: A road can have multiple listeners
    @Override
    public void addListener(RoadListener rl) {
        roadListener.add(rl);
    }


    //Delete the RoadListener rl from this place.
    @Override
    public void deleteListener(RoadListener rl) {
        roadListener.remove(rl);
    }


    //Return the first place of this road
    //Note: The first place of a road is the place whose name
    //comes EARLIER in the alphabet.
    @Override
    public Place firstPlace() {
        return this.from;
    }


    //Return the second place of this road
    //Note: The second place of a road is the place whose name
    //comes LATER in the alphabet.
    @Override
    public Place secondPlace() {
        return this.to;
    }

    public void setChosen(boolean c) {
        this.ischosen = c;
        this.roadChanged();
    }

    //Return true if this road is chosen as part of the current trip
    @Override
    public boolean isChosen() { return ischosen; }

    //Return the name of this road
    @Override
    public String roadName() { return this.roadName; }

    //Return the length of this road
    @Override
    public int length() { return this.length; }


    //Return a string containing information about this road
    //in the form (without quotes, of course!):
    //"firstPlace(roadName:length)secondPlace"
    @Override
    public String toString() {
        if (from != null && to != null) {
            return from.getName() + "(" + roadName + ":" + Integer.toString(length)
              + ")" +  to.getName();
        }
        return "[]";
    }

    public void roadChanged() {
        for (RoadListener rl : roadListener)
            rl.roadChanged();
    }
}
