/* a1691807, Yi-Ting, Hsieh */
import java.util.Comparator;

/*****************************************************************************
*  FeedEntryComparator
*  1. Checks which FeedEntry is newer.
*****************************************************************************/
public class FeedEntryComparator implements Comparator<FeedEntry> {
    @Override
    public int compare(FeedEntry fe1, FeedEntry fe2) {
        return fe2.getDate().compareTo(fe1.getDate());
    }
}
