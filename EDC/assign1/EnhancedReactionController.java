/* a1691807 Yi-Ting, Hsieh */
public class EnhancedReactionController implements Controller
{
    private Gui this_gui;
    private Random this_rng;

    private int state = 0;
    private float time = 0;
    private float delayTime = 0;
    private float endTime = 0;
    private float[] scores = null;
    private int index;
    private float averTime;

    public EnhancedReactionController(){}

    //Connect controller to gui
    //(This method will be called before ANY other methods)
    @Override
    public void connect(Gui gui, Random rng)
    {
        this_gui = gui;
        this_rng = rng;
    }

    //Called to initialise the controller
    @Override
    public void init()
    {
        state = 0;
        time = 0;
        delayTime = 0;
        endTime = 0;
        scores = new float[3];
        index = 0;
        averTime = 0;
        this_gui.setDisplay("Insert coin");

    }

    //Called whenever a coin is inserted into the machine
    @Override
    public void coinInserted()
    {
        //the machine displays "Press GO!" and waits for the player to do so.
        if (state == 0)
        {
            state = 1;
            this_gui.setDisplay("Press GO!");
        }
    }

    //Called whenever the go/stop button is pressed
    @Override
    public void goStopPressed()
    {
        if (state == 1)
        {
            delayTime = this_rng.getRandom(100, 250);
            this_gui.setDisplay("Wait...");
            state = 2;
        }
        else if (state == 2)
        {
            this.init();
        }
        else if (state == 3)
        {
            scores[index] = endTime;
            index++;
            time = 0;
            state = 4;
        }
        else if (state == 4)
        {
            if (index < 3)
            {
                delayTime = this_rng.getRandom(100, 250);
                state = 2;
            }
            else
            {
                time = 0;
                averTime = (float) ((scores[0] + scores[1] + scores[2]) / 3);
                state = 5;
            }
        }
        else if (state == 5)
        {
            this.init();
        }

    }

    //Called to deliver a TICK to the controller
    @Override
    public void tick()
    {
        if (state == 1)
        {
            time++;
            // String string_time = String.format("%.2f", time/100);
            // this_gui.setDisplay(string_time);

            if (time >= 1000)
            {
                init();
            }
        }
        else if (state == 2)
        {
            time++;
            this_gui.setDisplay("Wait...");
            if (time >= delayTime)
            {
                state = 3;
                time = 0;
                String string_time = String.format("%.2f", time/100);
                this_gui.setDisplay(string_time);
            }
        }

        else if (state == 3)
        {
            time++;
            String string_time = String.format("%.2f", time/100);
            this_gui.setDisplay(string_time);
            endTime = time;

            if (time >= 200)
            {
                endTime = 200;
                scores[index] = time;
                index++;
                time = 0;
                state = 4;
            }
        }

        else if (state == 4)
        {
            time++;
            // String string_time = String.format("%.2f", endTime/100);
            // this_gui.setDisplay(string_time);
            this_gui.setDisplay("Wait...");

            if (time >= 300)
            {
                if (index < 3)
                {
                    delayTime = this_rng.getRandom(100, 250);
                    state = 2;
                    time = 0;
                }
                else
                {
                    time = 0;
                    averTime = (float) ((scores[0] + scores[1] + scores[2]) / 3);
                    String string_time = String.format("Average= %.2f", averTime/100);
                    this_gui.setDisplay(string_time);
                    state = 5;
                }
            }
        }
        else if (state == 5)
        {
            time++;

            if (time >= 500)
                this.init();
        }
    }

}
