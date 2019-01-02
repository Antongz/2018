import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import javax.swing.JComponent;
import java.awt.BasicStroke;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Stroke;
import java.awt.RenderingHints;
import java.awt.Toolkit;
import java.awt.geom.Line2D;

public class RoadIcon extends JComponent implements RoadListener {
    private static final int R = 16;
    private Road road;
    private int x1, x2, y1, y2;
    private boolean isSelected, isChosen;
    private Line2D.Double line = new Line2D.Double();

    public RoadIcon(Road r) {
        this.road = r;
        this.getDimensions();
        this.setVisible(true);
        repaint();
    }

    public void setSelected(boolean b) {
        if (isSelected == b)    return;

        isSelected = b;
        repaint();
    }

    public boolean isSelected() { return this.isSelected; }

    public void setChosen(boolean b) {
        if (isChosen == b)  return;

        isChosen = b;
        repaint();
    }

    public boolean isChosen() { return this.isChosen; }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;

        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        Font f = new Font("default", Font.BOLD, 11);

        if (isSelected) {
            g2d.setColor(new Color(0, 191, 243));
            Stroke stroke = new BasicStroke(2);
            g2d.setStroke(stroke);
        } else if (isChosen) {
            g2d.setColor(new Color(57, 181, 74));
            Stroke stroke = new BasicStroke(2);
            g2d.setStroke(stroke);
        } else {
            g2d.setColor(new Color(115, 99, 87));
            f = new Font("default", Font.PLAIN, 11);
        }

        line.setLine(x1, y1, x2, y2);
        g2d.draw(line);

        int centerX = x1 + ((x2 - x1) / 2);
        int centerY = y1 + ((y2 - y1) / 2);

        double deg = Math.toDegrees(Math.atan2(centerY - y2, centerX - x2) + Math.PI);
        if (deg > 90 && deg < 270)  deg += 180;

        double angle = Math.toRadians(deg);
        String roadText = road.roadName() + "(" + road.length() + ")";
        FontMetrics fm = g2d.getFontMetrics(f);
        int sw = fm.stringWidth(roadText);

        g2d.setFont(f);
        g2d.rotate(angle, centerX, centerY);
        g2d.drawString(roadText, centerX - (sw / 2), centerY - 10);
        g2d.rotate(-angle, centerX, centerY);
    }

    @Override
    public void roadChanged() {
        if (road.isChosen())    this.setChosen(true);
        this.getDimensions();
        repaint();
    }

    private void getDimensions() {
        int souX = road.firstPlace().getX() + R;
        int souY = road.firstPlace().getY() + R;
        int desX = road.secondPlace().getX() + R;
        int desY = road.secondPlace().getY() + R;

        double rw = Math.abs(desX - souX) / R;
        double rh = Math.abs(desY - souY) / R;
        double s = Math.max(rw, rh);

        x1 = (int) (souX - (desX - souX) / s);
        y1 = (int) (souY - (desY - souY) / s);
        x2 = (int) (desX - (desX - souX) / s);
        y2 = (int) (desY - (desY - souY) / s);

        this.setBounds(0, 0, Toolkit.getDefaultToolkit().getScreenSize().width, Toolkit.getDefaultToolkit().getScreenSize().height);
    }

    public Line2D getLine() { return line; }

    public String name() { return this.road.roadName(); }

}
