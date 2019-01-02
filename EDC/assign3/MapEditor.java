import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.LayoutManager;
import java.awt.Menu;
import java.awt.MenuBar;
import java.awt.MenuItem;
import java.awt.MenuShortcut;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Reader;
import java.io.Writer;
import java.util.Collection;
import java.util.LinkedList;
import java.util.Set;
import javax.swing.filechooser.FileFilter;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class MapEditor extends JFrame implements ActionListener {
    private MenuBar menuBar = new MenuBar();
    private Menu file  = new Menu();
    private Menu edit = new Menu();
    private MenuItem openFile = new MenuItem();
	private MenuItem saveFile = new MenuItem();
    private MenuItem appendFile = new MenuItem();
	private MenuItem quit = new MenuItem();
    private MenuItem newPlace = new MenuItem();
	private MenuItem newRoad = new MenuItem();
	private MenuItem setStart = new MenuItem();
    private MenuItem unsetStart = new MenuItem();
	private MenuItem setEnd = new MenuItem();
	private MenuItem unsetEnd = new MenuItem();
    private MenuItem delete = new MenuItem();
    private MapReaderWriter mrw = new MapReaderWriter();
    private MapPanel mapanel;
    private Map map, temMap;
    private JPanel jp;
    private JLabel td, tip;
    private String fileName = "Untitled";
    private JScrollPane scroller;
    private boolean checkClose = false;


    public MapEditor() {
        // start to draw out menu
        Dimension dim = Toolkit.getDefaultToolkit().getScreenSize();
        this.setMinimumSize(new Dimension(800, 600));
        this.setMaximizedBounds(new Rectangle(dim));
        this.setMaximumSize(dim);
        this.setLocation(dim.width/2 - this.getSize().width/2, dim.height/2 - this.getSize().height/2);
        this.setTitle(this.fileName + " - MapEditor");
        this.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);


        this.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent w) {
                MapEditor me = (MapEditor) w.getSource();
                me.closeCall();
            }
        });


        this.addComponentListener(new ComponentListener() {
            @Override
            public void componentHidden(ComponentEvent ce) {
                // displayMessage(ce.getComponent().getClass().getName() + " --- Hidden");
            }

            @Override
            public void componentMoved(ComponentEvent ce) {
                // displayMessage(ce.getComponent().getClass().getName() + "--- Moved");
            }

            @Override
            public void componentResized(ComponentEvent ce) {
                mapanel.roadsChanged();
            }

            @Override
            public void componentShown(ComponentEvent ce) {
                ;// do nothing
            }
        });


        // Add Panel
        jp = new JPanel();
        jp.setLayout((LayoutManager) new FlowLayout(FlowLayout.LEFT));
        td = new JLabel("");
        td.setFont(new Font("default", Font.BOLD, 14));
        tip = new JLabel("");
        tip.setFont(new Font("default", Font.PLAIN, 12));
        jp.add(td);     jp.add(tip);

        map = new MapImpl();
        this.mapanel = new MapPanel(map, this);
        map.addListener(this.mapanel);

        scroller = new JScrollPane(this.mapanel, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
        scroller.setBorder(null);
        scroller.getViewport().addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent ce) {
                mapanel.roadsChanged();
            }
        });

        this.getContentPane().add(jp, BorderLayout.PAGE_START);
        this.getContentPane().add(scroller, BorderLayout.CENTER);

        this.setMenuBar(this.menuBar);
        this.menuBar.add(this.file);
        this.menuBar.add(this.edit);
        this.file.setLabel("File");
        this.edit.setLabel("Edit");

        // Menu items
        this.openFile.setLabel("Open");
        this.openFile.addActionListener(this);
        this.openFile.setShortcut(new MenuShortcut(KeyEvent.VK_O, false));
        this.file.add(this.openFile);

        this.appendFile.setLabel("Append");
        this.appendFile.addActionListener(this);
        this.appendFile.setShortcut(new MenuShortcut(KeyEvent.VK_A, false));
        this.appendFile.setEnabled(true);
        this.file.add(this.appendFile);

        this.saveFile.setLabel("Save as");
        this.saveFile.addActionListener(this);
        this.saveFile.setShortcut(new MenuShortcut(KeyEvent.VK_S, false));
        this.saveFile.setEnabled(true);
        this.file.add(this.saveFile);

        this.quit.setLabel("Quit");
        this.quit.setShortcut(new MenuShortcut(KeyEvent.VK_Q, false));
        this.quit.addActionListener(this);
        this.file.add(this.quit);

        //Edit menu items
        this.newPlace.setLabel("New Place");
        this.newPlace.addActionListener(this);
        this.edit.add(this.newPlace);

        this.newRoad.setLabel("New Road");
        this.newRoad.addActionListener(this);
        this.newRoad.setEnabled(false);
        this.edit.add(this.newRoad);

        this.setStart.setLabel("Set Start");
        this.setStart.addActionListener(this);
        this.setStart.setEnabled(false);
        this.edit.add(this.setStart);

        this.unsetStart.setLabel("Unset Start");
        this.unsetStart.addActionListener(this);
        this.unsetStart.setEnabled(false);
        this.edit.add(this.unsetStart);

        this.setEnd.setLabel("Set End");
        this.setEnd.addActionListener(this);
        this.setEnd.setEnabled(false);
        this.edit.add(this.setEnd);

        this.unsetEnd.setLabel("Unset End");
        this.unsetEnd.addActionListener(this);
        this.unsetEnd.setEnabled(false);
        this.edit.add(this.unsetEnd);

        this.delete.setLabel("Delete");
        this.delete.setShortcut(new MenuShortcut(KeyEvent.VK_D, false));
        this.delete.addActionListener(this);
        this.delete.setEnabled(false);
        this.edit.add(this.delete);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == this.quit) { this.closeCall(); }
        else if (e.getSource() == this.openFile) {
            if (this.mapanel.getSave()) {
                int result = JOptionPane.showConfirmDialog(null, "Save changes to current map?", "Save changes", JOptionPane.YES_NO_OPTION);
                if (result == JOptionPane.YES_OPTION)   this.savefile();
            }

            JFileChooser open = new JFileChooser();
            open.setDialogTitle("Open Map");
            ExtensionFileFilter filter1 = new ExtensionFileFilter("Map Files (*.map)", new String[] { "MAP" });
            open.setFileFilter(filter1);
            int option = open.showOpenDialog(this);
            if (option == JFileChooser.APPROVE_OPTION) {
                boolean readsuccess = true;
                String file = open.getSelectedFile().getName();
                try {
                    temMap = new MapImpl();
                    Reader r = new FileReader(open.getSelectedFile().getPath());
                    this.fileName = open.getSelectedFile().getName();
                    mrw.read(r, temMap);
                } catch (MapFormatException mfe) {
                    readsuccess = false;
                    String[] errorstring = mfe.toString().split(":");
                    int success = Integer.parseInt(errorstring[0]) - 1;
                    if (success > 0) {
                        int result = JOptionPane.showConfirmDialog(null, "Error while parsing line " + errorstring[0] + " in \"" + file + "\"\n" + mfe.toString()
                            + "\n\nOpen file with data until line " + success + "?",
                            "Warning", JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE);

                        if (result == JOptionPane.YES_OPTION)   readsuccess = true;
                    } else {
                        JOptionPane.showMessageDialog(null, "Error while parsing line " + errorstring[0] + " in \"" + file + "\"\n" + mfe.toString(),
                            "Error", JOptionPane.ERROR_MESSAGE);
                    }
                } catch (IOException ie) {
                    readsuccess = false;
                    JOptionPane.showMessageDialog(null, ie.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
                }

                if (readsuccess) {
                    this.setTitle(this.fileName + " - Map Editor");
                    this.mapanel.removeAll();
                    emptyMap(map);
                    copyMap(temMap, map);
                    this.resizeCall();
                }
            }
        }
        else if (e.getSource() == this.appendFile) {
            JFileChooser append = new JFileChooser();
            append.setApproveButtonText("Append");
            append.setApproveButtonToolTipText("Append selected file");
            append.setDialogTitle("Append Map");
            ExtensionFileFilter filter1 = new ExtensionFileFilter("Map Files (*.map)", new String[] { "MAP" });
            append.setFileFilter(filter1);
            int option = append.showOpenDialog(this);

            if (option == JFileChooser.APPROVE_OPTION) {
                temMap = new MapImpl();
                copyMap(map, temMap);
                boolean readsuccess = true;
                String file = append.getSelectedFile().getName();

                try {
                    Reader r = new FileReader(append.getSelectedFile().getPath());
                    mrw.read(r, temMap);
                } catch (MapFormatException mfe) {
                    readsuccess = false;
                    String[] errorstring = mfe.toString().split(":");
                    int success = Integer.parseInt(errorstring[0]) - 1;
                    if (success > 0) {
                        int result = JOptionPane.showConfirmDialog(null, "Error while parsing line " + errorstring[0] + " in \"" + file + "\"\n" + mfe.toString()
                            + "\n\nOpen file with data until line " + success + "?",
                            "Warning", JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE);

                        if (result == JOptionPane.YES_OPTION)   readsuccess = true;
                    } else {
                        JOptionPane.showMessageDialog(null, "Error while parsing line " + errorstring[0] + " in \"" + file + "\"\n" + mfe.toString(),
                            "Error", JOptionPane.ERROR_MESSAGE);
                    }
                } catch (IOException ie) {
                    readsuccess = false;
                    JOptionPane.showMessageDialog(null, ie.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
                }

                if (readsuccess) {
                    this.mapanel.removeAll();
                    emptyMap(map);
                    copyMap(temMap, map);
                    this.mapanel.setSave(true);
                    this.resizeCall();
                }
            }
        }
        else if (e.getSource() == this.saveFile) { this.savefile(); }
        else if (e.getSource() == this.newPlace) {
            String pname = JOptionPane.showInputDialog(null, "Place Name:", "Add New place", JOptionPane.PLAIN_MESSAGE);

            try {
                if (pname != null) {
                    map.newPlace(pname, ((this.getContentPane().getWidth()) / 2) - 16, ((this.getContentPane().getHeight()) / 2) - 16);
                    mapanel.otherChanged();
                    mapanel.setSave(true);
                }
            } catch (Exception ex) {
                JOptionPane.showMessageDialog(null, ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
            }
        }
        else if (e.getSource() == this.newRoad) {
            if (map.getPlaces().size() < 2) {
                JOptionPane.showMessageDialog(null, "Add at least two places to create a road",
                    "Warning", JOptionPane.WARNING_MESSAGE);
            } else {
                JTextField rname = new JTextField();
                JTextField rlength = new JTextField();

                final JComponent[] inputs = new JComponent[] { new JLabel("Road name:"), rname, new JLabel("Road length"), rlength };
                int status = JOptionPane.showConfirmDialog(null, inputs, "Add new road", JOptionPane.OK_CANCEL_OPTION);

                if (status == JOptionPane.OK_OPTION) {
                    String road = rname.getText();
                    String length = rlength.getText();
                    mapanel.setnrMode(road, length);
                }
            }
        }
        else if (e.getSource() == this.setStart) {
            Set<Place> myplaces = mapanel.getSelPlaces();
            int size = myplaces.size();

            if (size > 1) {
                JOptionPane.showMessageDialog(null, "More than one place is selected", "Warning", JOptionPane.WARNING_MESSAGE);
            } else if (size == 1) {
                for (Place p : myplaces) {
                    if (p != map.getStartPlace()) {
                        map.setStartPlace(p);
                        mapanel.setSave(true);
                    }
                }
            } else {
                JOptionPane.showMessageDialog(null, "No place is selected", "Warning", JOptionPane.WARNING_MESSAGE);
            }
        }
        else if (e.getSource() == this. unsetStart) {
            if (map.getStartPlace() != null) {
                map.setStartPlace(null);
                mapanel.setSave(true);
            }
        }
        else if (e.getSource() == this.setEnd) {
            Set<Place> myplaces = mapanel.getSelPlaces();
            int size = myplaces.size();

            if (size > 1) {
                JOptionPane.showMessageDialog(null, "More than one place is selected", "Warning", JOptionPane.WARNING_MESSAGE);
            } else if (size == 1) {
                for (Place p : myplaces) {
                    if (p != map.getEndPlace()) {
                        map.setEndPlace(p);
                        mapanel.setSave(true);
                    }
                }
            } else {
                JOptionPane.showMessageDialog(null, "No place is selected", "Warning", JOptionPane.WARNING_MESSAGE);
            }
        }
        else if (e.getSource() == this. unsetEnd) {
            if (map.getEndPlace() != null) {
                map.setEndPlace(null);
                mapanel.setSave(true);
            }
        }
        else if (e.getSource() == this.delete) {
            Set<Place> myplaces = mapanel.getSelPlaces();
            Set<Road> myroads = mapanel.getSelRoads();

            if (myplaces.isEmpty() && myroads.isEmpty()) {
                JOptionPane.showMessageDialog(null, "Empty selection", "Warning", JOptionPane.WARNING_MESSAGE);
            } else {
                if (!myroads.isEmpty()) {
                    for (Road r : myroads) {
                        map.deleteRoad(r);
                    }
                }
                if (!myplaces.isEmpty()) {
                    for (Place p : myplaces) {
                        map.deletePlace(p);
                    }
                }
                mapanel.setSave(true);
                mapanel.otherChanged();
            }
        }
    }

    public static void main(String args[]) {
        MapEditor me = new MapEditor();
        me.setVisible(true);
    }

    private void copyMap(Map source, Map dest) {
        Set<Place> temPlaces = source.getPlaces();
        for (Place p : temPlaces)
            dest.newPlace(p.getName(), p.getX(), p.getY());

        Set<Road> temRoads = source.getRoads();
        for (Road r : temRoads) {
            dest.newRoad(dest.findPlace(r.firstPlace().getName()), dest.findPlace(r.secondPlace().getName()),
            r.roadName(), r.length());
        }
        if (source.getStartPlace() != null) {
            Place start = dest.findPlace(source.getStartPlace().getName());
            dest.setStartPlace(start);
        }
        else    dest.setStartPlace(null);

        if (source.getEndPlace() != null) {
            Place end = dest.findPlace(source.getEndPlace().getName());
            dest.setEndPlace(end);
        }
        else    dest.setEndPlace(null);

    }

    private void emptyMap(Map m) {
        Set<Place> newPlaces = m.getPlaces();
        Collection<Place> rPlace = new LinkedList<Place>(newPlaces);
        for (Place p : newPlaces)
            rPlace.add(p);

        newPlaces.removeAll(rPlace);

        Set<Road> newRoads = m.getRoads();
        Collection<Road> rRoad = new LinkedList<Road>(newRoads);
        for (Road r : newRoads)
            newRoads.add(r);

        newRoads.removeAll(rRoad);

        m.setStartPlace(null);
        m.setEndPlace(null);
    }

    private void savefile() {
        boolean accept = false;
        File f = null;
        do {
            f = new File(this.fileName);
            JFileChooser fileChooser = new JFileChooser();
            fileChooser.setDialogTitle("Save Map");
            fileChooser.setSelectedFile(f);
            ExtensionFileFilter filter1 = new ExtensionFileFilter("Map Files (*.map)", new String[] { "MAP" });
            fileChooser.setFileFilter(filter1);

            if (fileChooser.showSaveDialog(null) == JFileChooser.APPROVE_OPTION) {
                String filepath = fileChooser.getSelectedFile().getAbsolutePath();

                if (filepath.lastIndexOf(".map") == -1)
                    filepath += ".map";
                f = new File(filepath);

                if (f.exists()) {
                    int result = JOptionPane.showConfirmDialog(null, f.getName() + "File already exists, overwrite?",
                        "Existing file", JOptionPane.YES_NO_OPTION);

                    if (result == JOptionPane.YES_OPTION)
                        accept = true;
                } else  accept = true;
            } else {
                accept = true;
                f = null;
                if (this.checkClose == true)
                    this.checkClose = false;
            }
        } while (!accept);

        if (accept && (f != null)) {
            try {
                this.fileName = f.getName();
                Writer out = new BufferedWriter(new FileWriter(f.getAbsolutePath()));
                mrw.write(out, map);
                out.close();
                this.setTitle(this.fileName + " - Map Editor");
                mapanel.setSave(false);
            } catch (Exception e) {
                JOptionPane.showMessageDialog(null, e.toString(), "Error", JOptionPane.ERROR_MESSAGE);
            }
        }
    }

    private void closeCall() {
        this.checkClose = true;
        if (mapanel.getSave()) {
            int result = JOptionPane.showConfirmDialog(null, "Save changes to current map?", "Save changes", JOptionPane.YES_NO_CANCEL_OPTION);
            if (result == JOptionPane.YES_OPTION)
                this.savefile();
            else if (result == JOptionPane.CANCEL_OPTION)
                this.checkClose = false;
        }

        if (this.checkClose) {
            this.dispose();
            System.exit(0);
        }
    }

    public void setTripText(String d) {
        this.td.setText(d);
    }

    public void setTipText(String t) {
        if ((!t.equals("")) && (!this.td.getText().equals("")))
            this.tip.setText(" ("+t+")");
        else
            this.tip.setText(t);
    }

    public void enableNewRoad(boolean b) {
        this.newRoad.setEnabled(b);
    }

    public void enableEditMenuItems(boolean b) {
        this.setEnd.setEnabled(b);
        this.unsetEnd.setEnabled(b);
        this.setStart.setEnabled(b);
        this.unsetStart.setEnabled(b);
        this.delete.setEnabled(b);
    }

    private void resizeCall() {
        int x = 0, y = 0;
        Set<Place> actualPlaces = map.getPlaces();
        for (Place p : actualPlaces) {
            if (p.getX() >= x)
                x = p.getX();
            if (p.getY() >= y)
                y = p.getY();
        }

        if ((x > this.getMaximumSize().width) || (y > this.getMaximumSize().height)) {
            this.setExtendedState(JFrame.MAXIMIZED_BOTH);
            JOptionPane.showMessageDialog(null, "Map contains places outside maximum viewable area of your main screen", "Warning!", JOptionPane.WARNING_MESSAGE);
        } else
            this.setSize(x + 100, y + 150);
    }

}
