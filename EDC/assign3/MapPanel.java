import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Component;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import javax.swing.JPanel;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.Stroke;
import java.awt.Toolkit;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseEvent;
import java.awt.geom.Rectangle2D;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;
import javax.swing.BorderFactory;
import javax.swing.JOptionPane;

public class MapPanel extends JPanel implements MouseListener, MouseMotionListener, MapListener {

    private MapEditor me;
    private Map map;
    private HashMap<Place, PlaceIcon> places;
    private HashMap<Road, RoadIcon> roads;
    private Rectangle rect;
    private Rectangle2D rect2D;
    private String name = "";
    private String length = "";
    private boolean dragMode = false;
    private boolean nrMode = false;
    private int dx, x1, x0, dy, y1, y0;

    private boolean save = false;

    public MapPanel(Map m, MapEditor me) {
        super(null);
        this.map = m;
        this.me = me;

        places = new HashMap<Place, PlaceIcon>();
        roads = new HashMap<Road, RoadIcon>();
        rect = new Rectangle(0, 0, 0, 0);
        rect2D = (Rectangle2D)rect;
        this.addMouseListener(this);
        this.addMouseMotionListener(this);
        this.setBorder(BorderFactory.createMatteBorder(1, 0, 0, 0, Color.BLACK));
        this.setVisible(true);
        this.setPreferredSize(new Dimension(Toolkit.getDefaultToolkit().getScreenSize().width - 32, Toolkit.getDefaultToolkit().getScreenSize().height - 150));
        repaint();
    }

    //Called whenever the number of places in the map has changed
    @Override
    public void placesChanged() {
        Set<Place> temp = map.getPlaces();
        for (Place p : temp) {
            if (!places.containsKey(p)) {
                PlaceIcon pi = new PlaceIcon(p, this);
                p.addListener(pi);
                this.places.put(p, pi);
                this.add(pi);
            }
        }

        Iterator itr = places.keySet().iterator();
        while (itr.hasNext()) {
            Place p = (Place) itr.next();
            if (!temp.contains(p)) {
                PlaceIcon pi = places.get(p);
                pi.setVisible(false);
                p.deleteListener(pi);
                itr.remove();
            }
        }

        repaint();
        if (this.map.getPlaces().size() > 1)  this.me.enableNewRoad(true);
        else    this.me.enableNewRoad(false);

        if (this.map.getPlaces().isEmpty()) this.me.enableEditMenuItems(false);
        else    this.me.enableEditMenuItems(true);
    }

    //Called whenever the number of roads in the map has changed
    @Override
    public void roadsChanged() {
        Set<Road> temp = map.getRoads();
        Iterator itr = roads.keySet().iterator();

        while (itr.hasNext()) {
            Road r = (Road) itr.next();
            if (!temp.contains(r)) {
                RoadIcon ri = roads.get(r);
                ri.setVisible(false);
                r.deleteListener(ri);
                itr.remove();
            }
        }

        for (Road r : temp) {
            if (!roads.containsKey(r)) {
                RoadIcon ri = new RoadIcon(r);
                if (r.isChosen())   ri.setChosen(true);

                r.addListener(ri);
                this.roads.put(r, ri);
                this.add(ri);
            } else {
                RoadIcon ri = roads.get(r);
                if (r.isChosen())   ri.setChosen(true);
                else    ri.setChosen(false);
            }
        }

        itr = roads.keySet().iterator();
        while (itr.hasNext())
            roads.get((Road) itr.next()).roadChanged();

        repaint();
    }

    //Called whenever something about the map has changed
    //(other than places and roads)
    @Override
    public void otherChanged() {
        int distance = this.map.getTripDistance();
        me.setTripText("");

        if ((!this.map.getRoads().isEmpty()) && this.map.getStartPlace() != null && this.map.getEndPlace() != null) {
            if (distance < 0) {
                me.setTripText("No Route");
                me.setTipText("");
            } else if (distance > 0) {
                me.setTripText("Trip Distance: " + distance);
                me.setTipText("Follow the path highlighted in Green to reach " + this.map.getEndPlace().getName() + ", from " + this.map.getStartPlace().getName());
            } else {
                me.setTripText("Trip Distance: " + distance + "");
                me.setTipText("");
            }
        } else if (!this.map.getRoads().isEmpty() && (this.map.getStartPlace() != null ||this.map.getEndPlace() != null)) {
            me.setTripText("No Route");
            me.setTipText("set start and end places to find shortest path between the two");
        } else  { me.setTipText(""); }

        this.roadsChanged();
        this.deselect();
    }

    @Override
    protected void paintComponent(Graphics gs) {
        super.paintComponent(gs);
        Graphics2D g = (Graphics2D)gs;
        g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g.setColor(Color.MAGENTA);
        g.drawRect(rect.x, rect.y, rect.width, rect.height);

        if (dragMode) {
            g.setColor(Color.ORANGE);
            Stroke stroke = new BasicStroke(2);
            g.setStroke(stroke);
            g.drawLine(dx, dy, x1, y1);
            g.setStroke(new BasicStroke(1));
        }
    }

    //MouseListener
    @Override
    public void mouseEntered(MouseEvent me) {}
    @Override
    public void mouseExited(MouseEvent me) {}

    @Override
    public void mousePressed(MouseEvent me) {
   	    x0= me.getX();
   	    y0= me.getY();
   	    rect = new Rectangle(x0,y0,0,0);
        rect2D = (Rectangle2D)(new Rectangle((x0 - 3), (y0 - 3), 6, 6));
    }

    @Override
    public void mouseReleased(MouseEvent me) {
   	    rect= new Rectangle(0,0,0,0);
   	    repaint();
    }

    @Override
    public void mouseClicked(MouseEvent me) { this.nrCheck(true); }

    //MouseMotionListener
    @Override
    public void mouseDragged(MouseEvent me) {
   	    if (!nrMode){
            int x = me.getX();
   	        int y = me.getY();
   	        int rx = Math.min(x0, x);
   	        int ry = Math.min(y0, y);
   	        int rw = Math.abs(x - x0);
   	        int rh = Math.abs(y - y0);
   	        rect = new Rectangle(rx, ry, rw, rh);

   	        for( Component c: this.getComponents() ) {
   	            try {
                    if (c.getClass().getName().equals("PlaceIcon")) {
                           PlaceIcon pi= (PlaceIcon)c;
                           boolean isHit= rect.intersects(pi.getBounds());
                           pi.setSelected(isHit);
                    } else if (c.getClass().getName().equals("RoadIcon")) {
                           Rectangle2D re2d = (Rectangle2D)rect;
                           RoadIcon ri = (RoadIcon)c;
                           if (re2d.intersectsLine(ri.getLine()))   ri.setSelected(true);
                           else ri.setSelected(false);
                    }
                } catch(Exception e) {
                 //  System.out.println(e);
                }
   	        }

            repaint();
        }

    }

    @Override
    public void mouseMoved(MouseEvent me){
        if (dragMode) {
            x1 = me.getX();
            y1 = me.getY();
            repaint();
        }
    }

    public Set<Place> getSelPlaces() {
        Set<Place> temp =  new HashSet<Place>();
        Iterator itr = places.keySet().iterator();

        while(itr.hasNext()) {
            Place key = (Place)itr.next();
            PlaceIcon pl = (PlaceIcon)places.get(key);
            if (pl.isSelected())    temp.add(key);
        }
        return temp;
    }

    public Set<Road> getSelRoads() {
        Set<Road> temp =  new HashSet<Road>();
        Iterator itr = roads.keySet().iterator();

        while(itr.hasNext()) {
            Road key = (Road)itr.next();
            RoadIcon ri = (RoadIcon)roads.get(key);
            if (ri.isSelected()) temp.add(key);
        }
        return temp;
    }

    public Set<RoadIcon> getRoadIcons() {
        Set<RoadIcon> ris =  new HashSet<RoadIcon>();
        Iterator itr = roads.keySet().iterator();

        while(itr.hasNext()) {
            Road key = (Road)itr.next();
            ris.add(roads.get(key));
        }
        return ris;
    }



    public void setnrMode(String n, String l){
        this.me.setTipText("Click on a place icon to select origin");
        this.setCursor(Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR));
        this.nrMode = true;
        this.name = n;
        this.length = l;
        this.deselect();
        repaint();
    }

    private void createNR() {
        this.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
        Set<Place> newPlaces = this.getSelPlaces();
        Place[] parray = newPlaces.toArray(new Place[0]);

        try {
            if (!length.matches("^\\d+$"))  throw new IllegalArgumentException("Invalid road length value: "+ length);

            map.newRoad(parray[0], parray[1], name, Integer.parseInt(length));
            this.otherChanged();
            this.setSave(true);
        } catch(Exception e) {
           int mc = JOptionPane.ERROR_MESSAGE;
           JOptionPane.showMessageDialog(null, e.getMessage(), "Error", mc);
        }

        deselect();
   	    this.length = "";
        this.name = "";
        this.nrMode = false;
        this.dragMode = false;
        this.dx =0;
        this.dy = 0;

    }

    public void nrCheck(boolean b) {
        if (this.nrMode) {
            if (this.getSelPlaces().isEmpty()) {
               this.dragMode = false;
               this.dx = 0;
               this.dy = 0;
               me.setTipText("Click on a place icon to select origin");
            } else if(this.getSelPlaces().size() == 1) {
                this.dragMode = true;
                this.dx = this.getMousePosition().x;
                this.dy = this.getMousePosition().y;
                this.x1 = this.dx;
                this.y1 = this.dy;
                me.setTipText("Click on a place icon to select destination");
            } else if (this.getSelPlaces().size() == 2) { this.createNR(); }
        } else {
            boolean outside = b;
            if (!b) rect2D = (Rectangle2D)(new Rectangle((0),(0),0,0));

            for (Component c : this.getComponents()) {
                if (c.getClass().getName().equals("RoadIcon")) {
                    RoadIcon ri = (RoadIcon)c;
                    if (ri.getLine().intersects(rect2D)){
                        this.deselectRoads(ri);
                        this.deselectPlaces(null);
                        ri.setSelected(!ri.isSelected());
                        outside = false;
                    }
                }
            }
            if (outside)    this.deselect();
        }
        repaint();
    }

    public void setSave(boolean b) { this.save = b; }

    public boolean getSave() { return this.save; }

    public void deselect() {
        for (Component c : this.getComponents()) {
            try {
                if (c.getClass().getName().equals("PlaceIcon")) {
                    PlaceIcon pi = (PlaceIcon)c;
                    pi.setSelected(false);
                } else if (c.getClass().getName().equals("RoadIcon")) {
                    RoadIcon ri = (RoadIcon)c;
                    ri.setSelected(false);
                }
            } catch(Exception e) {
                //System.out.println(e);}
            }
        }
    }

    public void deselectPlaces(PlaceIcon pime) {
        for (Component c : this.getComponents()) {
            try {
                if (c.getClass().getName().equals("PlaceIcon")) {
                    PlaceIcon pi= (PlaceIcon)c;
                    if (!pi.equals(pime))   pi.setSelected(false);
                }
            } catch(Exception e) {
                //System.out.println(e);}
            }
        }
    }

    public void deselectRoads(RoadIcon rime) {
        for(Component c : this.getComponents()) {
            try {
                if (c.getClass().getName().equals("RoadIcon")) {
                    RoadIcon ri = (RoadIcon)c;
                    if (!ri.equals(rime))   ri.setSelected(false);

                }
            } catch(Exception e) {
                //System.out.println(e);}
            }
        }

    }

    public MapEditor getEditor() { return this.me; }

    public boolean getnrMode() { return this.nrMode; }

}
