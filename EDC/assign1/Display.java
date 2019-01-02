/* a1691807 Yi-Ting, Hsieh */
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.*;

public class Display extends JFrame implements Gui
{
    private Controller connect_controller;
    private JFrame frame = null;
    private JTextField textString = null;
    private JButton insertCoin_button = null;
    private JButton goStop_button = null;

    //constructor
    public Display()
    {
        super("Title bar");
        setLayout(new FlowLayout());
        textString = new JTextField();
        insertCoin_button = new JButton();
        goStop_button = new JButton();

        add(textString);
        add(insertCoin_button);
        add(goStop_button);

        setVisible(true);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.setSize(400, 400);
    }


    //Connect gui to controller
    //(This method will be called before ANY other methods)
    public void connect(Controller controller)
    {
        connect_controller = controller;
        //action listeners
        insertCoin_button.addActionListener(new ActionListener()
        {
            @Override
            public void actionPerformed(ActionEvent e)
            {
                connect_controller.coinInserted();
            }
        });

        goStop_button.addActionListener(new ActionListener()
        {
            @Override
            public void actionPerformed(ActionEvent e)
            {
                connect_controller.goStopPressed();
            }
        });

    }

    //Initialise the gui
    @Override
    public void init()
    {
        insertCoin_button.setText("Insert coin");
        goStop_button.setText("go/stop");
        this.setDisplay("Insert coin");

    }

    //Change the displayed text
    @Override
    public void setDisplay(String s)
    {
        textString.setText(s);
    }
}
