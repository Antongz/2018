/* a1691807, Yi-Ting, Hsieh */
import java.util.*;

/*****************************************************************************
*  FeedAvailabilityCheck
*  1. Checks whether the feed is expired or not(<15 seconds).
*****************************************************************************/
public class FeedAvailabilityCheck extends TimerTask {
    private final HashMap<String, Date> feeddates;
    private final HashMap<String, List> feedEntries;
    private int lamportClock;

    public FeedAvailabilityCheck(HashMap<String, Date> fd, HashMap<String, List> fe, int lamportclock) {
        this.feeddates = fd;
        this.feedEntries = fe;
        this.lamportClock = lamportclock;
    }

    // Delete feeds which are older than 15 seconds.
    public void run() {
        for (Map.Entry<String, Date> feedMap : feeddates.entrySet()) {
            Date now = new Date();
            Date feedsubmitdate = feedMap.getValue();
            long timediff = (now.getTime() - feedsubmitdate.getTime()) / 1000;

            if (timediff > 15) {
                System.out.println("Removing old feed:" + feedMap.getKey());
                feedEntries.remove(feedMap.getKey());
                feeddates.remove(feedMap.getKey());
                this.lamportClock++;
                System.out.println("The Lamport clock now @ " + this.lamportClock);
            }
        }
    }

}
