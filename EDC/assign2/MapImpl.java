import java.util.List;
import java.util.Set;
import java.io.*;
import java.util.*;

public class MapImpl implements Map {

    private ArrayList<MapListener> mapListener;
    private Set<Place> places;
    private Set<Road> roads;
    private Place startPlace, endPlace;


    public MapImpl(){
        places = new HashSet<Place>();
        roads = new HashSet<Road>();
        startPlace = null;
        endPlace = null;

    }

    //Add the MapListener ml to this map.
    //Note: A map can have multiple listeners
    public void addListener(MapListener ml) {
        //TODO...
        mapListener.add(ml);
    }


    //Delete the MapListener ml from this map.
    public void deleteListener(MapListener ml) {
        //TODO...
        mapListener.remove(ml);
    }


    //Create a new Place and add it to this map
    //Return the new place
    //Throws IllegalArgumentException if:
    //  the name is not valid or is the same as that
    //  of an existing place
    //Note: A valid placeName begins with a letter, and is
    //followed by optional letters, digits, or underscore characters
    public Place newPlace(String placeName, int xPos, int yPos)
      throws IllegalArgumentException {

          boolean isDuplicated = false;
          for (Place i : this.places) {
              if (i.getName().equals(placeName)) {
                  isDuplicated = true;
                  break;
              }
          }

          // TODO...
          //Note: A valid placeName begins with a letter, and is
          //followed by optional letters, digits, or underscore characters

          if (isDuplicated) {
              throw new IllegalArgumentException();
          }

          Place tempPlace = new PlaceImp(placeName, xPos, yPos);
          places.add(tempPlace);

          return tempPlace;
      }


    //Remove a place from the map
    //If the place does not exist, returns without error
    public void deletePlace(Place s) {
        if (places.contains(s))
            places.remove(s);

    }


    //Find and return the Place with the given name
    //If no place exists with given name, return NULL
    public Place findPlace(String placeName) {
        for (Place i : this.places) {
            if (i.getName().equals(placeName))
                return i;
        }

        return null;
    }


    //Return a set containing all the places in this map
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
    public Road newRoad(Place from, Place to,
      String roadName, int length)
      throws IllegalArgumentException {
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
          Road tempRoad = new RoadImp(from, to, roadName, length);
          this.roads.add(tempRoad);
          return tempRoad;
      }


    //Remove a road r from the map
    //If the road does not exist, returns without error
    public void deleteRoad(Road r) {
        if (roads.contains(r))
            roads.remove(r);
    }


    //Return a set containing all the roads in this map
    public Set<Road> getRoads() {
        return this.roads;
    }


    //Set the place p as the starting place
    //If p==null, unsets the starting place
    //Throws IllegalArgumentException if the place p is not in the map
    public void setStartPlace(Place p)
      throws IllegalArgumentException {
        if (places.contains(p)) {
            startPlace = p;
        }
        else {
            throw new IllegalArgumentException();
        }
      }


    //Return the starting place of this map
    public Place getStartPlace() {
        return startPlace;
    }


    //Set the place p as the ending place
    //If p==null, unsets the ending place
    //Throws IllegalArgumentException if the place p is not in the map
    public void setEndPlace(Place p)
      throws IllegalArgumentException {
          if (places.contains(p)) {
              endPlace = p;
          }
          else {
              throw new IllegalArgumentException();
          }
      }


    //Return the ending place of this map
    public Place getEndPlace() {
        return endPlace;
    }


    //Causes the map to compute the shortest trip between the
    //"start" and "end" places
    //For each road on the shortest route, sets the "isChosen" property
    //to "true".
    //Returns the total distance of the trip.
    //Returns -1, if there is no route from start to end
    public int getTripDistance() {
        return -1;
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
}


 class PlaceImp implements Place {
    private ArrayList<PlaceListener> placeListener;
    private String name;
    private int dx, dy;
    Set<Road> toRoads;

    PlaceImp() {}

    PlaceImp(String name, int dx, int dy) {
        this.name = name;
        this.dx = dx;
        this.dy = dy;
        toRoads = new HashSet<>();
    }


    //Add the PlaceListener pl to this place.
    //Note: A place can have multiple listeners
    public void addListener(PlaceListener pl) {
        //TODO...
        placeListener.add(pl);
    }


    //Delete the PlaceListener pl from this place.
    public void deleteListener(PlaceListener pl) {
        //TODO...
        placeListener.remove(pl);
    }


    //Return a set containing all roads that reach this place
    public Set<Road> toRoads() {
        return this.toRoads;
    }


    //Return the road from this place to dest, if it exists
    //Returns null, if it does not
    public Road roadTo(Place dest) {

        for (Road i : this.toRoads) {
            if (i.firstPlace().getName().equals(dest.getName()) || i.secondPlace().getName().equals(dest.getName()))
                return i;
        }

        return null;
    }


    //Move the position of this place
    //by (dx,dy) from its current position
    public void moveBy(int dx, int dy) {
        this.dx += dx;
        this.dy += dy;
    }


    //Return the name of this place
    public String getName() {
        return this.name;
    }


    //Return the X position of this place
    public int getX() {
        return this.dx;
    }


    //Return the Y position of this place
    public int getY() {
        return this.dy;
    }


    //Return true if this place is the starting place for a trip
    public boolean isStartPlace() {
        //TODO...
        return false;
    }


    //Return true if this place is the ending place for a trip
    public boolean isEndPlace() {
        //TODO...
        return false;
    }


    //Return a string containing information about this place
    //in the form (without the quotes, of course!) :
    //"placeName(xPos,yPos)"
    // place placeName xpos ypos
    public String toString() {
        return name + "(" +  Integer.toString(dx) + "," + Integer.toString(dy) + ")";
    }
}

 class RoadImp implements Road {
    private ArrayList<PlaceListener> placeListener;
    Place from, to;
    private String roadName;
    private int length;
    private boolean isChosen;

    RoadImp() {};

    RoadImp(Place from, Place to, String roadName, int length) {
        int temp = from.getName().compareTo(to.getName());
        if (temp < 0) {
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
    }
    //Add the RoadListener rl to this place.
    //Note: A road can have multiple listeners
    public void addListener(RoadListener rl) {
        //TODO
    }


    //Delete the RoadListener rl from this place.
    public void deleteListener(RoadListener rl) {
        //TODO

    }


    //Return the first place of this road
    //Note: The first place of a road is the place whose name
    //comes EARLIER in the alphabet.
    public Place firstPlace() {
        return this.from;
    }


    //Return the second place of this road
    //Note: The second place of a road is the place whose name
    //comes LATER in the alphabet.
    public Place secondPlace() {
        return this.to;
    }


    //Return true if this road is chosen as part of the current trip
    public boolean isChosen() {
        // TODO...
        return false;
    }


    //Return the name of this road
    public String roadName() {
        return this.roadName;
    }


    //Return the length of this road
    public int length() {
        return this.length;
    }


    //Return a string containing information about this road
    //in the form (without quotes, of course!):
    //"firstPlace(roadName:length)secondPlace"

    public String toString() {
        if (from != null && to != null) {
            return from.getName() + "(" + roadName + ":" + Integer.toString(length)
              + ")" +  to.getName();
        }
        return "[]";
    }
}
