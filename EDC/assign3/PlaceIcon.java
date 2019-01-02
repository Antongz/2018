import java.awt.Color;
import java.awt.Font;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import javax.swing.JComponent;

public class PlaceIcon extends JComponent
    implements PlaceListener, MouseListener, MouseMotionListener {
    private static final int R = 32;
    private Place place;
    private int x0, y0;
    private boolean isSelected;
    private int piw, pih, pix, piy;
    private MapPanel mp;

    public PlaceIcon(Place p, MapPanel m) {
        this.place = p;
        this.mp = m;
        this.pix = place.getX();
        this.piy = place.getY();
        this.addMouseMotionListener(this);
        this.addMouseListener(this);
        this.setBounds(place.getX(), place.getY(), R+1, R+1);
        this.setVisible(true);
        this.setToolTipText(place.getName());
        repaint();
    }

    public void setSelected(boolean b) {
        if (isSelected == b)
            return;

        this.isSelected = b;
        repaint();
    }

    public boolean isSelected() { return this.isSelected; }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;

        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g2d.setFont(new Font("default", Font.PLAIN, 10));

        if (place.isStartPlace() && place.isEndPlace()) {
            if (isSelected)
                g2d.setColor(new Color(109, 207, 246));
            else
                g2d.setColor(new Color(255, 247, 153));
            g2d.fillRect(0, 0, R, R);
            g2d.setColor(Color.black);
            g2d.drawString("S&E", 1, 30);
        } else if (place.isStartPlace()) {
            if (isSelected)
                g2d.setColor(new Color(109, 207, 246));
            else
                g2d.setColor(new Color(240, 110, 170));
            g2d.fillRect(0, 0, R, R);
            g2d.setColor(Color.black);
            g2d.drawString("start", 1, 30);
        } else if (place.isEndPlace()) {
            if (isSelected)
                g2d.setColor(new Color(109, 207, 246));
            else
                g2d.setColor(new Color(242, 108, 79));
            g2d.fillRect(0, 0, R, R);
            g2d.setColor(Color.black);
            g2d.drawString("end", 1, 30);
        } else if (isSelected) {
            g2d.setColor(new Color(109, 207, 246));
            g2d.fillRect(0, 0, R, R);
        }

        g2d.setColor(new Color(117, 76, 36));
        g2d.drawRect(0, 0, R, R);
        g2d.drawString(this.place.getName(), 1, 10);
    }

    @Override
    public void placeChanged() {
        if (this.pix != place.getX() || this.piy != place.getY()) {
            mp.setSave(true);
	    }
        this.setLocation(place.getX(),place.getY());
        repaint();
    }

    @Override
    public void mouseEntered(MouseEvent me) { }

    @Override
    public void mouseExited(MouseEvent me) { }

    @Override
    public void mousePressed(MouseEvent me) { this.x0 = me.getX();  this.y0 = me.getY(); }

    @Override
    public void mouseReleased(MouseEvent me) { }

    @Override
    public void mouseClicked(MouseEvent me) {
        mp.deselectRoads(null);
        if (!mp.getnrMode())
            mp.deselectPlaces(this);
        isSelected = !isSelected;
        repaint();
        mp.nrCheck(false);
    }

    @Override
    public void mouseDragged(MouseEvent me) {
        int x = me.getX();      int y = me.getY();
        if (isSelected) {
            for (Place p : mp.getSelPlaces())
                p.moveBy(x - x0, y - y0);
        } else {
            piw = mp.getEditor().getMaximumSize().width - 50;
            pih = mp.getEditor().getMaximumSize().height - 160;
            int tempX = x - x0 + place.getX();
            int tempY = y - y0 + place.getY();

            if ((tempX < piw) && (tempY < pih) && (tempX > 0) && (tempY > 0))
                place.moveBy(x - x0, y - y0);
        }
    }

    @Override
    public void mouseMoved(MouseEvent me) { this.setToolTipText(this.place.getName()); }

}
